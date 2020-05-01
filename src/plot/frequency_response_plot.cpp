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

#include "frequency_response_plot.hpp"

#include "filter_network.hpp"
#include "signal.hpp"
#include "driver_list.hpp"

#include <cmath>
#include <fstream>
#include <iostream>

frequency_response_plot::frequency_response_plot()
    : m_plot(1, 20000, 50, 110, true, 0), m_color("blue")
{
    m_speakerlist = nullptr;

    add(m_plot);

    m_plot.set_y_label(_("Magnitude / dB"));

    show_all();

    g_settings.defaultValueBool("DisableFilterAmp", false);

    signal_speakerlist_loaded.connect(
        sigc::mem_fun(*this, &frequency_response_plot::on_speakerlist_loaded));
    signal_add_crossover_plot.connect(
        sigc::mem_fun(*this, &frequency_response_plot::on_add_plot));
    signal_crossover_selected.connect(
        sigc::mem_fun(*this, &frequency_response_plot::on_crossover_selected));
}

frequency_response_plot::~frequency_response_plot() = default;

auto lerp(std::vector<gspk::point> const& freq_resp_points, double x) -> double
{
    auto first_i = std::lower_bound(freq_resp_points.begin(),
                                    freq_resp_points.end(),
                                    x,
                                    gspk::comparison{});

    if (first_i != begin(freq_resp_points))
    {
        std::advance(first_i, -1);
    }

    auto second_i = std::upper_bound(begin(freq_resp_points),
                                     end(freq_resp_points),
                                     x,
                                     gspk::comparison{});

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

auto frequency_response_plot::parse_frequency_response_file(std::string const& filename)
    -> std::vector<gspk::point>
{
    std::vector<gspk::point> points;

    if (filename.empty())
    {
        points.emplace_back(0, 0);
        points.emplace_back(20000, 0);
        return points;
    }

    std::cout << "frequency_response_plot::on_add_plot: freq_resp_file = " << filename
              << '\n';

    std::ifstream input_file(filename.c_str());

    if (!input_file.is_open())
    {
        throw std::runtime_error(_("Could not open ") + filename);
    }

    while (input_file)
    {
        double frequency;
        double magnitude;
        char comma;

        input_file >> frequency >> comma >> magnitude;

        if (comma != ',')
        {
            throw std::runtime_error("Frequency response requires comma separated "
                                     "values in "
                                     + filename);
        }
        points.emplace_back(std::round(frequency), magnitude);
    }
    return points;
}

auto frequency_response_plot::on_add_plot(std::vector<gspk::point> const& plot_points,
                                          Gdk::Color const& color,
                                          int& output_plot_index,
                                          filter_network* network) -> int
{
#ifndef NDEBUG
    std::puts("frequency_response_plot::on_add_plot");
#endif

    auto const& plot_index = output_plot_index;

    driver current_driver;
    if (m_speakerlist != nullptr)
    {
        current_driver = m_speakerlist->get_by_id_string(network->get_speaker());
    }

    std::vector<gspk::point> freq_resp_points = this->parse_frequency_response_file(
        current_driver.get_freq_resp_filename());

    std::vector<gspk::point> points;

    for (auto const& filter_point : plot_points)
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

    // If plot_index is in the graph, replace the old point-vector, if plot_index not in
    // graph insert it at the end of the vector
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

    std::vector<gspk::point> pnts;

    if (!m_points.empty())
    {
        pnts = m_points[0];

        Gdk::Color c2("red");

        m_plot.add_plot(m_points[0], c2);

        for (std::size_t j = 1; j < m_points.size(); j++)
        {
            for (std::size_t k = 0; k < m_points[j].size(); k++)
            {
                pnts[k].set_y(10
                              * std::log10(std::pow(10, pnts[k].get_y() / 10)
                                           + std::pow(10, (m_points[j])[k].get_y() / 10)));
            }
            m_plot.add_plot(m_points[j], c2);
        }
    }
    // Add summed freq resp plot to the plot and select this plot since
    // it's the most important plot in this graph
    m_plot.select_plot(m_plot.add_plot(pnts, m_color));

    return 0;
}

void frequency_response_plot::clear()
{
    m_points.clear();
    m_nets.clear();
    m_plot.remove_all_plots();
}

void frequency_response_plot::on_crossover_selected(Crossover*)
{
    std::puts("frequency_response_plot::on_crossover_selected");
    clear();
}

void frequency_response_plot::on_speakerlist_loaded(driver_list* driver_list)
{
#ifndef NDEBUG
    std::puts("frequency_response_plot::on_speakerlist_loaded");
#endif
    m_speakerlist = driver_list;
}
