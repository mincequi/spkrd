/* gspeakersplot.cc
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

#include "plot.hpp"
#include "common.h"

#include <cmath>
#include <iostream>

plot::plot(int lower_x, int upper_x, int lower_y, int upper_y, bool logx, int y_zero_freq, bool enable_sec_scale)
    : m_linesize{1},
      m_lower_x{lower_x},
      m_upper_x{upper_x > 10000 ? 20000 : 1000},
      m_lower_y{lower_y},
      m_upper_y{upper_y},
      m_y_zero_freq{y_zero_freq},
      m_enable_sec_scale{enable_sec_scale},
      m_logx{logx},
      m_visible{false}
{
}

bool plot::on_draw(Cairo::RefPtr<Cairo::Context> const& context)
{
    m_context = context;

    this->redraw(context);

    return true;
}

bool plot::on_expose_event(GdkEventExpose* event)
{
    m_selected_plot = -1;

    return false;
}

bool plot::on_configure_event(GdkEventConfigure* event)
{
    m_visible = true;

    // We've handled the configure event, no need for further processing.
    return true;
}

int plot::add_plot(std::vector<GSpeakers::Point> const& ref_point_vector, Gdk::Color const& ref_color)
{
    auto const& allocation = get_allocation();

    m_visible_plots.push_back(true);
    m_colors.push_back(ref_color);
    m_points.push_back(ref_point_vector);

    int total_space_x = allocation.get_width() - 2 * BOX_FRAME_SIZE;
    int half_space_x = std::round(total_space_x / 2);

    auto const box_height = allocation.get_height() - 2 * BOX_FRAME_SIZE;

    std::vector<Gdk::Point> points;

    for (auto& point : ref_point_vector)
    {
        int x, y;

        if (m_upper_x == 20000)
        {
            if (point.get_x() < 100)
            {
                // Divide by 10 to only log numbers 0 < number < 10
                auto const f_mapped = std::log10(point.get_x() / 10.0);
                // This is the x coordinate
                x = BOX_FRAME_SIZE + std::round(half_space_x / 2 * f_mapped);
            }
            else if (point.get_x() >= 100)
            {
                // Divide by 100 to only log numbers 0 < number < 10
                auto const f_mapped = std::log10(point.get_x() / 100.0);
                // This is the x coordinate
                x = std::round(BOX_FRAME_SIZE + half_space_x / 2
                               + std::round(half_space_x / 2 * f_mapped));
            }
        }
        else
        {
            if (point.get_x() < 100)
            {
                // Divide by 10 to only log numbers 0 < number < 10
                auto const f_mapped = std::log10(point.get_x() / 10.0);
                // This is the x coordinate
                x = BOX_FRAME_SIZE + std::round(half_space_x * f_mapped);
            }
            else if (point.get_x() >= 100)
            {
                // Divide by 100 to only log numbers 0 < number < 10
                auto const f_mapped = std::log10(point.get_x() / 100.0);
                // This is the x coordinate
                x = BOX_FRAME_SIZE + half_space_x + std::round(half_space_x * f_mapped);
            }
        }

        // Zero-level is on 3/4 of box_size_y, map -60 - 20 dB onto this scale
        point.set_y(point.get_y() < m_lower_y ? m_lower_y : m_upper_y);

        // Calculate y-coordinate
        y = std::round(box_height + BOX_FRAME_SIZE
                       - (point.get_y() - m_lower_y) * box_height
                             / static_cast<double>(m_upper_y - m_lower_y));

        // Don't draw anything if we got zeros
        if (point.get_y() > m_lower_y && point.get_y() < m_upper_y)
        {
            points.emplace_back(x, y);
        }
    }

    // Don't draw the line until we have it all done
    if (m_visible)
    {
        this->draw_lines(m_context, points, ref_color);

        select_plot(m_colors.size() - 1);

        Gdk::Rectangle update_rect(0, 0, allocation.get_width(), allocation.get_height());
        get_window()->invalidate_rect(update_rect, false);
    }
    // Return index of the new plot so that the owner of
    // this plot can keep track of plots
    return m_colors.size() - 1;
}

void plot::draw_lines(Cairo::RefPtr<Cairo::Context> const& context,
                      std::vector<Gdk::Point> const& points,
                      Gdk::Color const& colour)
{
    if (points.empty())
    {
        return;
    }

    context->save();

    context->set_source_rgb(colour.get_red_p(), colour.get_green_p(), colour.get_blue_p());

    context->move_to(points.front().get_x(), points.front().get_y());

    std::for_each(std::next(begin(points)), end(points), [&](auto const& point) {
        context->line_to(point.get_x(), point.get_y());
        context->stroke();
        context->move_to(point.get_x(), point.get_y());
    });
    context->restore();
}

void plot::replace_plot(int const plot_index,
                        std::vector<GSpeakers::Point> const& points,
                        Gdk::Color const& ref_color)
{
    m_points[plot_index] = points;
    m_colors[plot_index] = ref_color;
}

void plot::remove_plot(int n)
{
    if (n > static_cast<int>(m_points.size()))
    {
        throw std::runtime_error("Plot removed was greater than number of available plots");
    }

    m_points.erase(std::next(begin(m_points), n));
    m_colors.erase(std::next(begin(m_colors), n));
    m_visible_plots.erase(std::next(begin(m_visible_plots), n));

    m_selected_plot = -1;

    if (m_visible)
    {
        Gdk::Rectangle update_rect(0, 0, get_allocation().get_width(), get_allocation().get_height());
        get_window()->invalidate_rect(update_rect, false);
    }
}

void plot::remove_all_plots()
{
    m_points.clear();
    m_colors.clear();
    m_visible_plots.clear();

    if (m_visible)
    {
        Gdk::Rectangle update_rect(0, 0, get_allocation().get_width(), get_allocation().get_height());
        get_window()->invalidate_rect(update_rect, false);
    }
}

void plot::hide_plot(int n)
{
    m_visible_plots[n] = !m_visible_plots[n];
    if (m_visible)
    {
        Gdk::Rectangle update_rect(0, 0, get_allocation().get_width(), get_allocation().get_height());
        get_window()->invalidate_rect(update_rect, false);
    }
}

void plot::select_plot(int index)
{
    m_selected_plot = index;

    if (m_visible)
    {
        Gdk::Rectangle update_rect(0, 0, get_allocation().get_width(), get_allocation().get_height());
        get_window()->invalidate_rect(update_rect, false);
    }
}

void plot::redraw(Cairo::RefPtr<Cairo::Context> const& context)
{
    auto const& allocation = get_allocation();

    // Clear to white background color
    context->save();
    context->set_source_rgb(1.0, 1.0, 1.0);
    context->rectangle(0, 0, allocation.get_width(), allocation.get_height());
    context->restore();

    // Calc coordinates for a rectangular box
    auto const box_x = BOX_FRAME_SIZE;
    auto const box_y = BOX_FRAME_SIZE;
    auto const box_width = allocation.get_width() - 2 * BOX_FRAME_SIZE;
    auto const box_height = allocation.get_height() - 2 * BOX_FRAME_SIZE;

    // Draw black rectangle
    context->save();
    context->set_source_rgb(0.0, 0.0, 0.0);
    context->rectangle(box_x, box_y, box_width, box_height);
    context->restore();

    if (m_logx)
    {
        draw_log_grid(context);
    }
    else
    {
        draw_linear_grid(context);
    }

    // Draw horizontal lines
    for (int i = m_lower_y; i < m_upper_y; i += 5)
    {
        int y = std::round(box_height + BOX_FRAME_SIZE
                           - (-m_lower_y + i) * box_height
                                 / static_cast<double>(-m_lower_y + m_upper_y));

        context->move_to(BOX_FRAME_SIZE - 3, y);
        context->line_to(allocation.get_width() - BOX_FRAME_SIZE + 3, y);
        context->stroke();

        this->draw_text_box(context, int_to_ustring3(i), BOX_FRAME_SIZE - 27, y - 8);

        if (m_enable_sec_scale)
        {
            this->draw_text_box(context,
                                int_to_ustring3(i - m_lower_y),
                                allocation.get_width() - BOX_FRAME_SIZE + 5,
                                y - 6);
        }
    }

    if (!m_y_label1.empty())
    {
        this->draw_text_box(context, m_y_label1, BOX_FRAME_SIZE / 2, BOX_FRAME_SIZE / 3);
    }

    if (!m_y_label2.empty())
    {
        this->draw_text_box(context, m_y_label2, box_width - 2.5 * BOX_FRAME_SIZE, BOX_FRAME_SIZE / 3);
    }

    auto const total_space_x = allocation.get_width() - 2 * BOX_FRAME_SIZE;
    auto const half_space_x = std::round(total_space_x / 2);

    // Map points in m_points to screen points
    auto const n_plots = m_points.size();

    for (std::size_t i = 0; i < n_plots; i++)
    {
        if (m_visible_plots[i])
        {
            std::vector<Gdk::Point> points;

            double f_div, f_mapped;
            int x, y;

            for (auto& point : m_points[i])
            {
                if (m_upper_x == 20000)
                {
                    if (point.get_x() < 100)
                    {
                        // Divide by 10 to only log numbers 0 < number < 10
                        f_div = point.get_x() / 10.0;
                        f_mapped = std::log10(f_div);

                        // This is the x coordinate
                        x = BOX_FRAME_SIZE + std::round(half_space_x / 2 * f_mapped);
                    }
                    else if (point.get_x() >= 100)
                    {
                        // Divide by 100 to only log numbers 0 < number < 10
                        f_div = point.get_x() / 100.0;
                        f_mapped = std::log10(f_div);

                        // This is the x coordinate
                        x = std::round(BOX_FRAME_SIZE + half_space_x / 2
                                       + std::round(half_space_x / 2 * f_mapped));
                    }
                    else if (point.get_x() >= 1000)
                    {
                        // Divide by 1000 to only log numbers 0 < number < 10
                        f_div = point.get_x() / 1000.0;
                        f_mapped = std::log10(f_div);
                        // This is the x coordinate
                        x = BOX_FRAME_SIZE + half_space_x + std::round((half_space_x / 2) * f_mapped);
                    }
                    else if (point.get_x() >= 10000)
                    {
                        // Divide by 1000 to only log numbers 0 < number < 10
                        f_div = point.get_x() / 10000.0;
                        f_mapped = std::log10(f_div);
                        // This is the x coordinate
                        x = std::round(BOX_FRAME_SIZE + 1.5 * half_space_x
                                       + std::round((half_space_x / 2) * f_mapped));
                    }
                }
                else
                {
                    if (point.get_x() < 100)
                    {
                        // Divide by 10 to only log numbers 0 < number < 10
                        f_div = point.get_x() / 10.0;
                        f_mapped = std::log10(f_div);
                        // This is the x coordinate
                        x = BOX_FRAME_SIZE + std::round(half_space_x * f_mapped);
                    }
                    else if (point.get_x() >= 100)
                    {
                        // Divide by 100 to only log numbers 0 < number < 10
                        f_div = point.get_x() / 100.0;
                        f_mapped = std::log10(f_div);
                        // This is the x coordinate
                        x = BOX_FRAME_SIZE + half_space_x + std::round(half_space_x * f_mapped);
                    }
                }

                // Zero-level is on 3/4 of box_size_y, map -60 - 20 dB onto this scale
                if (point.get_y() < m_lower_y)
                {
                    point.set_y(m_lower_y);
                }
                else if (point.get_y() > m_upper_y)
                {
                    point.set_y(m_upper_y);
                }

                // Calculate y-coordinate
                y = std::round(box_height + BOX_FRAME_SIZE
                               - (point.get_y() - m_lower_y) * box_height
                                     / static_cast<double>(m_upper_y - m_lower_y));

                // Don't draw anything if we got zeros
                if (point.get_y() > m_lower_y && point.get_y() < m_upper_y)
                {
                    points.emplace_back(x, y);
                }
            }

            // Draw only if the vector is not empty
            if (!points.empty())
            {
                context->save();

                if (static_cast<int>(i) == m_selected_plot)
                {
                    context->set_line_width(m_linesize + 1);
                }

                auto const& color = m_colors.at(i);

                context->set_source_rgb(color.get_red_p(), color.get_green_p(), color.get_blue_p());

                context->move_to(points.front().get_x(), points.front().get_y());

                std::for_each(std::next(begin(points)), end(points), [&](auto const& point) {
                    context->line_to(point.get_x(), point.get_y());
                    context->stroke();
                    context->move_to(point.get_x(), point.get_y());
                });

                context->restore();
            }
        }
    }
}

void plot::draw_log_grid(Cairo::RefPtr<Cairo::Context> const& context)
{
    auto const& allocation = get_allocation();

    context->set_line_width(0.5);
    context->set_source_rgb(0.0, 0.0, 0.0);

    auto total_space_x = allocation.get_width() - 2 * BOX_FRAME_SIZE;
    auto quarter_space_x = std::round(total_space_x / 4);
    auto half_space_x = std::round(total_space_x / 2);
    auto xaxis_y_position = allocation.get_height() - BOX_FRAME_SIZE;

    // Draw the logarithmic vertical x-lines
    if (m_upper_x == 20000)
    {
        half_space_x = quarter_space_x;
    }

    for (int i = 0; i <= 10; i++)
    {
        auto const x = BOX_FRAME_SIZE + std::round(std::log10(i) * half_space_x);

        context->move_to(x, BOX_FRAME_SIZE);
        context->line_to(x, xaxis_y_position + 3);
        context->stroke();

        // Draw text below some vertical lines
        if (i == 2 || i == 5)
        {
            this->draw_text_box(context, std::to_string(10 * i), x - 4, xaxis_y_position + 5);
        }
    }

    for (int i = 1; i <= 10; i++)
    {
        int x = BOX_FRAME_SIZE + std::round(std::log10(i) * half_space_x);

        context->move_to(half_space_x + x, BOX_FRAME_SIZE);
        context->line_to(half_space_x + x, xaxis_y_position + 3);
        context->stroke();

        // Special case: draw 1k instead of 1000
        if (i == 10)
        {
            this->draw_text_box(context, "1k", half_space_x + x - 8, xaxis_y_position + 5);
        }
        else if (i == 2 || i == 5 || i == 1)
        {
            this->draw_text_box(context,
                                std::to_string(100 * i),
                                half_space_x + x - 8,
                                xaxis_y_position + 5);
        }
    }

    // Draw some more vertical lines if upper limit is 20000 Hz
    if (m_upper_x != 20000)
    {
        return;
    }

    for (int i = 1; i <= 10; i++)
    {
        auto const x = BOX_FRAME_SIZE + std::round(std::log10(i) * half_space_x);

        context->move_to(2 * half_space_x + x, BOX_FRAME_SIZE);
        context->line_to(2 * half_space_x + x, xaxis_y_position + 3);
        context->stroke();

        // Draw text below some vertical lines
        if (i == 2 || i == 5)
        {
            this->draw_text_box(context,
                                std::to_string(i) + "k",
                                2 * half_space_x + x - 8,
                                xaxis_y_position + 5);
        }
    }

    for (int i = 1; i <= 10; i++)
    {
        auto const x = BOX_FRAME_SIZE + std::round(std::log10(i) * half_space_x);

        context->move_to(3 * half_space_x + x, BOX_FRAME_SIZE);
        context->line_to(3 * half_space_x + x, xaxis_y_position + 3);
        context->stroke();

        if (i == 2 || i == 5 || i == 10)
        {
            this->draw_text_box(context,
                                std::to_string(10 * i) + "k",
                                3 * half_space_x + x - 8,
                                xaxis_y_position + 5);
        }
    }
}

void plot::draw_linear_grid(Cairo::RefPtr<Cairo::Context> const& context)
{
    context->set_line_width(0.5);
    context->set_source_rgb(0.0, 0.0, 0.0);

    // Compute total horizontal space inside the box
    auto const total_space_x = get_allocation().get_width() - 2 * BOX_FRAME_SIZE;

    // Calculate the xaxis vertical position, from upper edge of the drawingarea
    auto const xaxis_y_position = get_allocation().get_height() - BOX_FRAME_SIZE;

    // Determine distance (in x units) between the vertical lines
    auto const frequency_range = m_upper_x - m_lower_x;

    auto const x_inc = frequency_range > 200 && frequency_range <= 2000
                           ? 100
                           : frequency_range > 2000 && frequency_range <= 20000 ? 1000 : 10;

    // Calculate number of pixels between each vertical line
    auto const x_inc_pixels = std::round(x_inc / static_cast<double>(frequency_range) * total_space_x);

    // Calculate number of lines we're going to draw
    auto const n_lines = std::round(frequency_range / static_cast<double>(x_inc));

    // Draw the vertical lines to the pixmap
    for (int i = 1; i < n_lines; i++)
    {
        context->move_to(BOX_FRAME_SIZE + i * x_inc_pixels, BOX_FRAME_SIZE);
        context->line_to(BOX_FRAME_SIZE + i * x_inc_pixels, xaxis_y_position + 3);
        context->stroke();
    }
}

void plot::draw_text_box(Cairo::RefPtr<Cairo::Context> const& context,
                         std::string const& text,
                         int const rectangle_width,
                         int const rectangle_height)
{
    Pango::FontDescription font;
    font.set_family("Monospace");

    auto layout = create_pango_layout(text);

    layout->set_font_description(font);

    // TODO Use the previous values to centre the text
    // get the text dimensions (it updates the variables -- by reference)
    // int text_width;
    // int text_height;
    // layout->get_pixel_size(text_width, text_height);
    // Position the text in the middle
    // context->move_to((rectangle_width - text_width) / 2, (rectangle_height - text_height) / 2);

    context->move_to(rectangle_width, rectangle_height);

    layout->show_in_cairo_context(context);
}

void plot::set_y_label(const std::string& text)
{
    m_y_label1 = text;

    if (m_visible)
    {
        auto const& allocation = get_allocation();
        Gdk::Rectangle update_rect(0, 0, allocation.get_width(), allocation.get_height());
        get_window()->invalidate_rect(update_rect, false);
    }
}

void plot::set_y_label2(const std::string& text)
{
    m_y_label2 = text;

    if (m_visible)
    {
        auto const& allocation = get_allocation();
        Gdk::Rectangle update_rect(0, 0, allocation.get_width(), allocation.get_height());
        get_window()->invalidate_rect(update_rect, false);
    }
}

Glib::ustring plot::int_to_ustring3(int d)
{
    char* str = nullptr;
    GString* buffer = g_string_new(str);
    g_string_printf(buffer, "%3d", d);
    return buffer->str;
}
