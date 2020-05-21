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
#include "unit_conversion.hpp"

#include <cmath>
#include <iostream>

namespace spkrd
{
total_filter_plot::total_filter_plot() : m_plot(1, 20000), m_color("blue")
{
    add(m_plot);

    signal_add_crossover_plot.connect(sigc::mem_fun(*this, &total_filter_plot::on_add_plot));
    signal_crossover_selected.connect(
        sigc::mem_fun(*this, &total_filter_plot::on_crossover_selected));

    m_plot.set_y_label(_("Magnitude / dB"));

    show_all();
}

auto total_filter_plot::on_add_plot(std::vector<point> const& line_points,
                                    Gdk::Color const& colour,
                                    int& plot_index,
                                    filter_network*) -> int
{
    std::cout << "plot index = " << plot_index << '\n';

    // if (m_points.empty())
    // {
    m_points.push_back(line_points);
    m_networks.push_back(plot_index);
    // }
    // else
    // {
    //     auto const position = std::find(cbegin(m_networks), cend(m_networks), plot_index);
    //
    //     if (position != end(m_networks))
    //     {
    //         m_points[std::distance(cbegin(m_networks), position)] = line_points;
    //     }
    //     else
    //     {
    //         throw std::runtime_error("Attempted to add a plot the plot index wasn't in "
    //                                  "the list");
    //     }
    // }

    std::cout << "\tTOTAL POINTS = " << m_points.size() << '\n';
    for (auto const& points : m_points)
    {
        std::cout << "\tnumber of points = " << points.size() << '\n';
    }

    m_plot.remove_all_plots();
    m_plot.add_plot(line_points, colour);

    // Plot individual contributions
    std::for_each(cbegin(m_points), std::prev(cend(m_points)), [&](auto const& point) {
        m_plot.add_plot(point, Gdk::Color("black"));
    });

    // Superimpose the plots to visualise complete frequency response
    std::vector<point> summed_points = m_points.front();

    std::for_each(std::next(cbegin(m_points)), cend(m_points), [&](auto const& points) {
        std::transform(begin(points),
                       end(points),
                       begin(summed_points),
                       begin(summed_points),
                       [&](auto const& plot_point, auto const& summed_point) {
                           return point{summed_point.get_x(),
                                        magnitude_to_dB(
                                            dB_to_magnitude(summed_point.get_y())
                                            + dB_to_magnitude(plot_point.get_y()))};
                       });
    });

    // Add and select the superimposed points
    m_plot.select_plot(m_plot.add_plot(summed_points, m_color));

    return m_points.size();
}

void total_filter_plot::clear()
{
    std::cout << "Deleting all the plots and attempting to clear the screen\n";

    m_points.clear();
    m_networks.clear();
    m_plot.remove_all_plots();
}

void total_filter_plot::on_crossover_selected(Crossover*)
{
    std::puts("\ntotal_filter_plot:: Crossover selected in total_filter_plot\n");

    this->clear();
}
}
