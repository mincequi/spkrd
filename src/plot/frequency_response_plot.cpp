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
#include "unit_conversion.hpp"

#include <cmath>
#include <fstream>
#include <iostream>

frequency_response_plot::frequency_response_plot()
    : m_plot(1, 20000, 50, 110, true, 0), m_color("blue")
{
    add(m_plot);

    m_plot.set_y_label(_("Magnitude / dB"));

    g_settings.defaultValueBool("DisableFilterAmp", false);

    signal_drivers_loaded.connect(
        sigc::mem_fun(*this, &frequency_response_plot::on_drivers_loaded));
    signal_add_crossover_plot.connect(
        sigc::mem_fun(*this, &frequency_response_plot::on_add_plot));
    signal_crossover_selected.connect(
        sigc::mem_fun(*this, &frequency_response_plot::on_crossover_selected));

    show_all();
}

frequency_response_plot::~frequency_response_plot() = default;

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

    std::cout << "frequency_response_plot::parse_frequency_response_file: freq_resp_file "
                 "= "
              << filename << '\n';

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
    std::puts("frequency_response_plot::on_add_plot");

    auto const& plot_index = output_plot_index;

    driver current_driver;
    if (m_drivers != nullptr)
    {
        current_driver = m_drivers->get_by_id_string(network->get_speaker());
    }

    auto freq_resp_points = this->parse_frequency_response_file(
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
                            gspk::lerp(freq_resp_points, filter_point.get_x()) + filter_y);
    }

    // Search for plot_index in the graph
    auto const location = std::find(begin(m_networks), end(m_networks), plot_index);

    // If plot_index is in the graph, replace the old point-vector
    // If plot_index not in graph insert it at the end of the vector
    // if (location != end(m_networks) && !m_points.empty())
    // {
    //     m_points[std::distance(begin(m_networks), location)] = points;
    // }
    // else
    // {
    m_points.push_back(points);
    m_networks.push_back(plot_index);
    // }

    std::puts("frequency_response_plot::on_add_plot removing all plots\n");
    m_plot.remove_all_plots();

    if (m_points.empty())
    {
        return 0;
    }

    // Plot individual contributions
    std::for_each(cbegin(m_points), cend(m_points), [this](auto const& point) {
        m_plot.add_plot(point, Gdk::Color("red"));
    });

    std::vector<gspk::point> summed_points = m_points.front();

    std::for_each(std::next(cbegin(m_points)), cend(m_points), [&](auto const& line_points) {
        std::transform(begin(line_points),
                       end(line_points),
                       begin(summed_points),
                       begin(summed_points),
                       [&](auto const& line_point, auto const& summed_point) {
                           return gspk::point{summed_point.get_x(),
                                              gspk::magnitude_to_dB(
                                                  gspk::dB_to_magnitude(summed_point.get_y())
                                                  + gspk::dB_to_magnitude(
                                                        line_point.get_y()))};
                       });
    });
    // Add summed freq resp plot to the plot and select this plot since
    // it's the most important plot in this graph
    m_plot.select_plot(m_plot.add_plot(summed_points, m_color));

    return 0;
}

void frequency_response_plot::clear()
{
    m_points.clear();
    m_networks.clear();
    m_plot.remove_all_plots();
}

void frequency_response_plot::on_crossover_selected(Crossover*)
{
    std::puts("frequency_response_plot::on_crossover_selected is clearing the data");

    this->clear();
}

void frequency_response_plot::on_drivers_loaded(
    std::shared_ptr<driver_list const> const& driver_list)
{
    m_drivers = driver_list;
}
