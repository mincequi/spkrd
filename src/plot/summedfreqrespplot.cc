/*
 * $Id$
 *
 * Copyright (C) 2001-2002 Daniel Sundberg <dss@home.se>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include "summedfreqrespplot.h"

#include "net.h"

#include <cmath>
#include <fstream>
#include <iostream>

SummedFreqRespPlot::SummedFreqRespPlot() : m_plot(1, 20000, 50, 110, true, 0), m_color("blue")
{
    m_speakerlist = nullptr;

    add(m_plot);

    m_plot.set_y_label(_("Magnitude / dB"));

    show_all();

    g_settings.defaultValueBool("DisableFilterAmp", false);

    signal_speakerlist_loaded.connect(
        sigc::mem_fun(*this, &SummedFreqRespPlot::on_speakerlist_loaded));
    signal_add_crossover_plot.connect(sigc::mem_fun(*this, &SummedFreqRespPlot::on_add_plot));
    signal_crossover_selected.connect(
        sigc::mem_fun(*this, &SummedFreqRespPlot::on_crossover_selected));
}

SummedFreqRespPlot::~SummedFreqRespPlot() = default;

double lerp(std::vector<GSpeakers::Point> const& freq_resp_points, double x)
{
    auto first_i = std::lower_bound(freq_resp_points.begin(),
                                    freq_resp_points.end(),
                                    x,
                                    GSpeakers::comparison{});

    if (first_i != begin(freq_resp_points))
    {
        std::advance(first_i, -1);
    }

    auto second_i = std::upper_bound(begin(freq_resp_points),
                                     end(freq_resp_points),
                                     x,
                                     GSpeakers::comparison{});

    double x0 = first_i->get_x();
    double x1 = second_i->get_x();

    if (x0 == x1)
    {
        return first_i->get_y();
    }

    double y0 = first_i->get_y();
    double y1 = second_i->get_y();

    return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
}

int SummedFreqRespPlot::on_add_plot(std::vector<GSpeakers::Point> const& filter_points,
                                    Gdk::Color& color,
                                    int* output_plot_index,
                                    Net* n)
{
#ifndef NDEBUG
    std::puts("SummedFreqRespPlot::on_add_plot");
#endif

    auto const& plot_index = *output_plot_index;

    Speaker s;
    if (m_speakerlist != nullptr)
    {
        s = m_speakerlist->get_speaker_by_id_string(n->get_speaker());
    }

    std::vector<GSpeakers::Point> freq_resp_points;

    if (!s.get_freq_resp_filename().empty())
    {
        std::cout << "SummedFreqRespPlot::on_add_plot: freq_resp_file = "
                  << s.get_freq_resp_filename() << "\n";

        std::ifstream input_file(s.get_freq_resp_filename().c_str());

        if (!input_file.is_open())
        {
            throw std::runtime_error(_("Could not open ") + s.get_freq_resp_filename());
        }

        while (input_file)
        {
            double frequency;
            double magnitude;
            char comma;

            input_file >> frequency >> comma >> magnitude;

            if (comma != ',')
            {
                throw std::runtime_error("Frequency response requires comma separated values in "
                                         + s.get_freq_resp_filename());
            }
            freq_resp_points.emplace_back(std::round(frequency), magnitude);
        }
    }
    else
    {
        freq_resp_points.emplace_back(0, 0);
        freq_resp_points.emplace_back(20000, 0);
    }

    std::vector<GSpeakers::Point> points;

    for (auto const& filter_point : filter_points)
    {
        double filter_y = filter_point.get_y();

        if (g_settings.getValueBool("DisableFilterAmp") && filter_y > 0.0)
        {
            filter_y = 0.0;
        }
        points.emplace_back(filter_point.get_x(),
                            lerp(freq_resp_points, filter_point.get_x()) + filter_y);
    }

    // Search for plot_index in the graph
    auto const location = std::find(begin(m_nets), end(m_nets), plot_index);

    // If plot_index is in the graph, replace the old point-vector, if plot_index not in graph
    // insert it at the end of the vector
    if (location != end(m_nets) && !m_points.empty())
    {
        m_points[std::distance(begin(m_nets), location)] = points;
    }
    else
    {
        m_points.push_back(points);
        m_nets.push_back(plot_index);
    }

    m_plot.remove_all_plots();

    std::vector<GSpeakers::Point> pnts;

    if (!m_points.empty())
    {
        pnts = m_points[0];

        Gdk::Color c2("red");

        // m_plot.add_plot(m_points[0], c2);

        for (std::size_t j = 1; j < m_points.size(); j++)
        {
            for (std::size_t k = 0; k < m_points[j].size(); k++)
            {
                pnts[k].set_y(10
                              * std::log10(std::pow(10, pnts[k].get_y() / 10)
                                           + std::pow(10, (m_points[j])[k].get_y() / 10)));
            }
            // m_plot.add_plot(m_points[j], c2);
        }
    }
    // Add summed freq resp plot to the plot and select this plot since
    //  it's the most important plot in this graph
    m_plot.select_plot(m_plot.add_plot(pnts, m_color));

    return 0;
}

void SummedFreqRespPlot::clear()
{
    m_points.clear();
    m_nets.clear();

    m_plot.remove_all_plots();
}

void SummedFreqRespPlot::on_crossover_selected(Crossover*)
{
    std::puts("SummedFreqRespPlot::on_crossover_selected");
    clear();
}

void SummedFreqRespPlot::on_speakerlist_loaded(speaker_list* speaker_list)
{
#ifndef NDEBUG
    std::puts("SummedFreqRespPlot::on_speakerlist_loaded");
#endif
    m_speakerlist = speaker_list;
}
