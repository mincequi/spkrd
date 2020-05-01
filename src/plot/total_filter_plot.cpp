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
#include "signal.hpp"

#include <cmath>
#include <iostream>

total_filter_plot::total_filter_plot() : m_plot(1, 20000), m_color("blue")
{
    add(m_plot);

    signal_plot_crossover.connect(
        sigc::mem_fun(*this, &total_filter_plot::on_plot_crossover));
    signal_add_crossover_plot.connect(sigc::mem_fun(*this, &total_filter_plot::on_add_plot));

    m_plot.set_y_label(_("Magnitude / dB"));

    show_all();
}

total_filter_plot::~total_filter_plot() = default;

auto total_filter_plot::on_add_plot(std::vector<gspk::point> const& line_points,
                                    Gdk::Color const& line_colour,
                                    int& plot_index,
                                    filter_network*) -> int
{
    // Search for plot_index in the graph
    auto const position = std::find(cbegin(m_nets), cend(m_nets), plot_index);

    // If plot_index is in the graph, replace the old point-vector, if ,
    // plot_index not in graph insert it at the end of the vector */
    if (position != end(m_nets) && !m_points.empty())
    {
        m_points[std::distance(cbegin(m_nets), position)] = line_points;
    }
    else
    {
        m_points.push_back(line_points);
        m_nets.push_back(plot_index);
    }

    m_plot.remove_all_plots();

    if (m_points.empty())
    {
        return 0;
    }

    // Plot individual contributions
    std::for_each(cbegin(m_points), cend(m_points), [this](auto const& point) {
        m_plot.add_plot(point, Gdk::Color("red"));
    });

    // Superimpose the plots to visualise complete frequency response
    std::vector<gspk::point> summed_points = m_points.front();

    auto dB_to_magnitude = [](auto const dB) { return std::pow(10, dB / 10.0); };
    auto magnitude_to_dB = [](auto const magnitude) {
        return 10.0 * std::log10(magnitude);
    };

    std::for_each(std::next(cbegin(m_points)), cend(m_points), [&](auto const& line_points) {
        std::transform(begin(line_points),
                       end(line_points),
                       begin(summed_points),
                       begin(summed_points),
                       [&](auto const& line_point, auto const& summed_point) {
                           return gspk::point{summed_point.get_x(),
                                              magnitude_to_dB(
                                                  dB_to_magnitude(summed_point.get_y())
                                                  + dB_to_magnitude(line_point.get_y()))};
                       });
    });

    m_plot.select_plot(m_plot.add_plot(summed_points, m_color));

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
