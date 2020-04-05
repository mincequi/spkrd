/* totalfilterplot
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

#include "total_filter_plot.hpp"

#include "common.h"

#include <cmath>
#include <iostream>

total_filter_plot::total_filter_plot() : m_plot(1, 20000), m_color("blue")
{
    add(m_plot);

    signal_plot_crossover.connect(sigc::mem_fun(*this, &total_filter_plot::on_plot_crossover));
    signal_add_crossover_plot.connect(sigc::mem_fun(*this, &total_filter_plot::on_add_plot));

    m_plot.set_y_label(_("Magnitude / dB"));

    show_all();
}

total_filter_plot::~total_filter_plot() = default;

auto total_filter_plot::on_add_plot(std::vector<GSpeakers::Point> const& points,
                                    Gdk::Color const& color,
                                    int& output_plot_index,
                                    Net* n) -> int
{
    auto const plot_index = output_plot_index;

    /* Search for plot_index in the graph */
    auto const position = std::find(cbegin(m_nets), cend(m_nets), plot_index);

    // If plot_index is in the graph, replace the old point-vector, if ,
    // plot_index not in graph insert it at the end of the vector */
    if (position != end(m_nets) && !m_points.empty())
    {
        m_points[std::distance(cbegin(m_nets), position)] = points;
    }
    else
    {
        m_points.push_back(points);
        m_nets.push_back(plot_index);
    }

    // sum the plots into one master plot
    std::vector<GSpeakers::Point> pnts;

    auto c = Gdk::Color("red");

    m_plot.remove_all_plots();

    if (m_points.size() > 1)
    {
        pnts = m_points[0];
        m_plot.add_plot(m_points[0], c);

        for (std::size_t j = 1; j < m_points.size(); j++)
        {
            for (std::size_t k = 0; k < m_points[j].size(); k++)
            {
                pnts[k].set_y(10
                              * std::log10(std::pow(10, pnts[k].get_y() / 10.0)
                                           + std::pow(10, (m_points[j])[k].get_y() / 10.0)));
            }
            m_plot.add_plot(m_points[j], c);
        }
    }
    m_plot.select_plot(m_plot.add_plot(pnts, m_color));

    return 0;
}

void total_filter_plot::clear()
{
    m_points.clear();
    m_nets.clear();

    m_plot.remove_all_plots();
}

void total_filter_plot::on_crossover_selected(Crossover*) { clear(); }

void total_filter_plot::on_plot_crossover() {}

auto total_filter_plot::on_delete_event(GdkEventAny* event) -> bool
{
    // Don't delete this window
    return true;
}
