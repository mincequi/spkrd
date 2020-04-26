/*
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

#include "crossover_image_view.hpp"

#include "common.h"
#include "signal.hpp"

#include <gdkmm/rectangle.h>
#include <pangomm/context.h>

#include <iostream>

crossover_image_view::crossover_image_view()
{
    g_settings.defaultValueBool("ScaleCrossoverImageParts", true);
    m_scale_image_parts = g_settings.getValueBool("ScaleCrossoverImageParts");

    g_settings.settings_changed.connect(
        sigc::mem_fun(*this, &crossover_image_view::on_settings_changed));

    signal_crossover_selected.connect(
        sigc::mem_fun(*this, &crossover_image_view::on_crossover_selected));

    signal_net_modified_by_wizard.connect(
        sigc::mem_fun(*this, &crossover_image_view::on_net_modified));

    signal_speakerlist_loaded.connect(
        sigc::mem_fun(*this, &crossover_image_view::on_speakerlist_selected));
}

auto crossover_image_view::on_expose_event(GdkEventExpose* event) -> bool
{
    return false;
}

auto crossover_image_view::on_draw(Cairo::RefPtr<Cairo::Context> const& context) -> bool
{
    m_visible = true;

    this->redraw(context);

    return true;
}

auto crossover_image_view::on_configure_event(GdkEventConfigure* event) -> bool
{
    m_visible = true;

    white = Gdk::RGBA("rgb (0.0, 0.0, 0.0)");
    black = Gdk::RGBA("rgb (1.0, 1.0, 1.0)");

    m_refLayout = Pango::Layout::create(get_pango_context());

    // We've handled the configure event, no need for further processing
    return true;
}

void crossover_image_view::redraw(Cairo::RefPtr<Cairo::Context> const& context)
{
    if (!m_visible)
    {
        return;
    }

    auto const& allocation = get_allocation();

    // Clear to white background color
    context->save();
    context->set_source_rgb(1.0, 1.0, 1.0);
    context->rectangle(0, 0, allocation.get_width(), allocation.get_height());
    context->stroke();
    context->restore();

    if (m_crossover == nullptr)
    {
        return;
    }

    std::int32_t vert_space_per_net_divider = 0;

    if (m_crossover->get_type() == CROSSOVER_TYPE_LOWPASS)
    {
        vert_space_per_net_divider += 1;
    }
    if (m_crossover->get_type() == CROSSOVER_TYPE_SUBSONIC)
    {
        vert_space_per_net_divider += 1;
    }
    if (m_crossover->get_type() == CROSSOVER_TYPE_HIGHPASS)
    {
        vert_space_per_net_divider += 1;
    }
    if (m_crossover->get_type() == CROSSOVER_TYPE_TWOWAY)
    {
        vert_space_per_net_divider += 2;
    }
    if (m_crossover->get_type() == CROSSOVER_TYPE_THREEWAY)
    {
        vert_space_per_net_divider += 3;
    }
    if (m_crossover->get_type() == CROSSOVER_TYPE_FOURWAY)
    {
        vert_space_per_net_divider += 4;
    }

    if (vert_space_per_net_divider == 0)
    {
        return;
    }

    auto const window_height = allocation.get_height();
    auto const window_width = allocation.get_width();
    auto const vert_space_per_net = std::round(
        window_height / static_cast<double>(vert_space_per_net_divider));

    // Draw first network here
    auto const& networks = m_crossover->networks();

    for (std::size_t i = 0; i < networks.size(); i++)
    {
        int net_vert_divider = 3;
        int net_horz_divider = 2;

        int part_height = std::round(vert_space_per_net
                                     / static_cast<double>(net_vert_divider));

        switch (networks[i].get_lowpass_order())
        {
            case NET_ORDER_1ST:
                net_horz_divider += 1;
                break;
            case NET_ORDER_2ND:
                net_horz_divider += 2;
                break;
            case NET_ORDER_3RD:
                net_horz_divider += 3;
                break;
            case NET_ORDER_4TH:
                net_horz_divider += 4;
                break;
        }
        switch (networks[i].get_highpass_order())
        {
            case NET_ORDER_1ST:
                net_horz_divider += 1;
                break;
            case NET_ORDER_2ND:
                net_horz_divider += 2;
                break;
            case NET_ORDER_3RD:
                net_horz_divider += 3;
                break;
            case NET_ORDER_4TH:
                net_horz_divider += 4;
                break;
        }

        if (networks[i].get_has_imp_corr())
        {
            net_horz_divider += 1;
        }
        if (networks[i].get_has_damp())
        {
            net_horz_divider += 2;
        }

        auto part_width = std::round(window_width / static_cast<double>(net_horz_divider));

        if (m_scale_image_parts)
        {
            if (part_width > (1.5 * part_height) && networks[i].parts().size() <= 4)
            {
                part_width = part_height;
            }
            else if (part_width > 3 * part_height)
            {
                part_width = std::round(1.7 * part_height);
            }
        }

        draw_connector(context, 0, i * vert_space_per_net, part_width, part_height, true);
        draw_connector(context,
                       0,
                       i * vert_space_per_net + 2 * part_height,
                       part_width,
                       part_height,
                       false);

        auto const lowpass_order = networks[i].get_lowpass_order();
        auto const highpass_order = networks[i].get_highpass_order();

        std::vector<passive_component> const& part_vector = networks[i].parts();

        // lowpass part
        if (lowpass_order > 0)
        {
            std::vector<passive_component> lowpass_parts(part_vector.begin(),
                                                         part_vector.begin()
                                                             + lowpass_order);
            draw_lowpass_net(context,
                             part_width,
                             i * vert_space_per_net,
                             part_width,
                             part_height,
                             lowpass_parts);
        }

        // highpass part
        if (highpass_order > 0)
        {
            std::vector<passive_component> highpass_parts(part_vector.begin()
                                                              + lowpass_order,
                                                          part_vector.begin() + lowpass_order
                                                              + highpass_order);
            this->draw_highpass_net(context,
                                    part_width + lowpass_order * part_width,
                                    i * vert_space_per_net,
                                    part_width,
                                    part_height,
                                    highpass_parts);
        }

        int driver_offset = 0;

        if (networks[i].get_has_imp_corr())
        {
            draw_imp_corr_net(context,
                              (1 + lowpass_order + highpass_order) * part_width,
                              i * vert_space_per_net,
                              part_width,
                              part_height,
                              networks[i].get_imp_corr_R(),
                              networks[i].get_imp_corr_C());
            driver_offset++;
        }
        if (networks[i].get_has_damp())
        {
            draw_damp_net(context,
                          (1 + lowpass_order + highpass_order + driver_offset) * part_width,
                          i * vert_space_per_net,
                          part_width,
                          part_height,
                          networks[i].get_damp_R1(),
                          networks[i].get_damp_R2());
            driver_offset += 2;
        }

        driver speaker;

        if (m_speaker_list != nullptr)
        {
            speaker = m_speaker_list->get_by_id_string(networks[i].get_speaker());
        }
        draw_driver(context,
                    (1 + lowpass_order + highpass_order + driver_offset) * part_width,
                    i * vert_space_per_net,
                    part_width,
                    part_height,
                    speaker);
    }
}

void crossover_image_view::on_settings_changed(const std::string& setting)
{
    if (setting == "ScaleCrossoverImageParts")
    {
        m_scale_image_parts = g_settings.getValueBool("ScaleCrossoverImageParts");

        if (m_visible)
        {
            get_window()->invalidate_rect(Gdk::Rectangle(0,
                                                         0,
                                                         get_allocation().get_width(),
                                                         get_allocation().get_height()),
                                          true);
        }
    }
}

void crossover_image_view::on_crossover_selected(Crossover* selected_crossover)
{
    m_crossover = selected_crossover;
    if (m_visible)
    {
        Gdk::Rectangle update_rect(0,
                                   0,
                                   get_allocation().get_width(),
                                   get_allocation().get_height());
        get_window()->invalidate_rect(update_rect, true);
    }
}

void crossover_image_view::on_net_modified()
{
    if (m_crossover != nullptr)
    {
        on_crossover_selected(m_crossover);
    }
}

void crossover_image_view::on_speakerlist_selected(driver_list* selected_speaker_list)
{
    m_speaker_list = selected_speaker_list;
    if (m_visible)
    {
        Gdk::Rectangle update_rect(0,
                                   0,
                                   get_allocation().get_width(),
                                   get_allocation().get_height());
        get_window()->invalidate_rect(update_rect, true);
    }
}

void crossover_image_view::draw_lowpass_net(Cairo::RefPtr<Cairo::Context> const& context,
                                            int x,
                                            int y,
                                            int part_width,
                                            int part_height,
                                            std::vector<passive_component> const& parts)
{
    for (std::size_t i = 0; i < parts.size(); i++)
    {
        if (parts[i].get_type() == PART_TYPE_INDUCTOR)
        {
            draw_inductor(context,
                          parts[i].get_id(),
                          x + part_width * i,
                          y,
                          part_width,
                          part_height);
            draw_line(context,
                      x + part_width * i,
                      y + 2 * part_height,
                      part_width,
                      part_height);
        }
        else
        {
            draw_t_cross(context, x + part_width * i, y, part_width, part_height);
            draw_capacitor(context,
                           parts[i].get_id(),
                           x + part_width * i,
                           y + part_height,
                           part_width,
                           part_height,
                           true);
            draw_t_cross(context,
                         x + part_width * i,
                         y + 2 * part_height,
                         part_width,
                         part_height,
                         false);
        }
    }
}

void crossover_image_view::draw_highpass_net(Cairo::RefPtr<Cairo::Context> const& context,
                                             int x,
                                             int y,
                                             int part_width,
                                             int part_height,
                                             std::vector<passive_component> const& parts)
{
    for (std::size_t i = 0; i < parts.size(); i++)
    {
        if (parts[i].get_type() == PART_TYPE_CAPACITOR)
        {
            draw_capacitor(context,
                           parts[i].get_id(),
                           x + part_width * i,
                           y,
                           part_width,
                           part_height);
            draw_line(context,
                      x + part_width * i,
                      y + 2 * part_height,
                      part_width,
                      part_height);
        }
        else
        {
            draw_t_cross(context, x + part_width * i, y, part_width, part_height);
            draw_inductor(context,
                          parts[i].get_id(),
                          x + part_width * i,
                          y + part_height,
                          part_width,
                          part_height,
                          true);
            draw_t_cross(context,
                         x + part_width * i,
                         y + 2 * part_height,
                         part_width,
                         part_height,
                         false);
        }
    }
}

void crossover_image_view::draw_imp_corr_net(Cairo::RefPtr<Cairo::Context> const& context,
                                             int x,
                                             int y,
                                             int part_width,
                                             int part_height,
                                             passive_component const& capacitor,
                                             passive_component const& resistor)
{
    auto const local_part_height = std::round(part_height / 2.0);

    draw_t_cross(context, x, y, part_width, part_height, true);

    draw_capacitor(context,
                   capacitor.get_id(),
                   x,
                   y + part_height,
                   part_width,
                   local_part_height,
                   true);

    draw_resistor(context,
                  resistor.get_id(),
                  x,
                  y + part_height + local_part_height,
                  part_width,
                  local_part_height,
                  true);

    draw_t_cross(context, x, y + 2 * part_height, part_width, part_height, false);
}

void crossover_image_view::draw_damp_net(Cairo::RefPtr<Cairo::Context> const& context,
                                         int x,
                                         int y,
                                         int part_width,
                                         int part_height,
                                         passive_component const& r1,
                                         passive_component const& r2)
{
    draw_resistor(context, r1.get_id(), x, y, part_width, part_height, false);
    draw_line(context, x, y + 2 * part_height, part_width, part_height, false);
    draw_t_cross(context, x + part_width, y, part_width, part_height, true);
    draw_resistor(context,
                  r2.get_id(),
                  x + part_width,
                  y + part_height,
                  part_width,
                  part_height,
                  true);
    draw_t_cross(context, x + part_width, y + 2 * part_height, part_width, part_height, false);
}

void crossover_image_view::draw_driver(Cairo::RefPtr<Cairo::Context> const& context,
                                       int x,
                                       int y,
                                       int part_width,
                                       int part_height,
                                       driver const& speaker)
{
    draw_corner(context, x, y, part_width, part_height, true);

    if (speaker.get_type() == SPEAKER_TYPE_MIDRANGE)
    {
        draw_midrange(context, x, y + part_height, part_width, part_height);
    }
    else if (speaker.get_type() == SPEAKER_TYPE_BASS)
    {
        draw_woofer(context, x, y + part_height, part_width, part_height);
    }
    else
    {
        draw_tweeter(context, x, y + part_height, part_width, part_height);
    }
    draw_corner(context, x, y + 2 * part_height, part_width, part_height, false);
}

void crossover_image_view::draw_capacitor(Cairo::RefPtr<Cairo::Context> const& context,
                                          int id,
                                          int x,
                                          int y,
                                          int width,
                                          int height,
                                          bool rotate)
{
    auto const half_space_y = std::round(height / 2.0);
    auto const half_space_x = std::round(width / 2.0);
    auto const small_space_x = std::round(width / 20.0);
    auto const small_space_y = std::round(height / 20.0);

    context->set_font_size(18);
    context->move_to(x, y);
    context->select_font_face("FreeSans",
                              Cairo::FONT_SLANT_NORMAL,
                              Cairo::FONT_WEIGHT_NORMAL);
    context->show_text("C" + std::to_string(id));

    if (rotate)
    {
        // Horizontal line in capacitor
        context->move_to(std::round(x + half_space_x), y);
        context->line_to(std::round(x + half_space_x),
                         std::round(y + half_space_y - small_space_y));
        context->stroke();

        context->move_to(std::round(x + half_space_x),
                         y + std::round(half_space_y + small_space_y));
        context->line_to(std::round(x + half_space_x), y + height);
        context->stroke();

        // Vertical lines in capacitor
        context->move_to(std::round(x + 2 * small_space_x),
                         std::round(y + half_space_y - small_space_y));
        context->line_to(std::round(x + width - 2 * small_space_x),
                         std::round(y + half_space_y - small_space_y));
        context->stroke();

        context->move_to(std::round(x + 2 * small_space_x),
                         std::round(y + half_space_y + small_space_y));
        context->line_to(std::round(x + width - 2 * small_space_x),
                         std::round(y + half_space_y + small_space_y));
        context->stroke();
    }
    else
    {
        // Horizontal line in capacitor
        context->move_to(x, std::round(y + half_space_y));
        context->line_to(std::round(x + half_space_x - small_space_x),
                         std::round(y + half_space_y));
        context->stroke();

        context->move_to(std::round(x + half_space_x + small_space_x),
                         std::round(y + half_space_y));
        context->line_to(x + width, std::round(y + half_space_y));
        context->stroke();

        // Vertical lines in capacitor
        context->move_to(std::round(x + half_space_x - small_space_x),
                         std::round(y + 2 * small_space_y));
        context->line_to(std::round(x + half_space_x - small_space_x),
                         std::round(y + height - 2 * small_space_y));
        context->stroke();

        context->move_to(std::round(x + half_space_x + small_space_x),
                         std::round(y + 2 * small_space_y));
        context->line_to(std::round(x + half_space_x + small_space_x),
                         std::round(y + height - 2 * small_space_y));
        context->stroke();
    }
}

void crossover_image_view::draw_inductor(Cairo::RefPtr<Cairo::Context> const& context,
                                         int id,
                                         int x,
                                         int y,
                                         int width,
                                         int height,
                                         bool rotate)
{
    auto const half_space_y = std::round(height / 2.0);
    auto const half_space_x = std::round(width / 2.0);
    auto const small_space_x = std::round(width / 20.0);
    auto const small_space_y = std::round(height / 20.0);

    context->set_font_size(18);
    context->move_to(x, y);
    context->select_font_face("FreeSans",
                              Cairo::FONT_SLANT_NORMAL,
                              Cairo::FONT_WEIGHT_NORMAL);
    context->show_text("L" + std::to_string(id));

    // Vertical inductor
    if (rotate)
    {
        // Horizontal line in inductor
        context->move_to(x + half_space_x, y);
        context->line_to(x + half_space_x, y + 2 * small_space_y);
        context->stroke();

        context->move_to(x + half_space_x, y + height - 2 * small_space_y);
        context->line_to(x + half_space_x, y + height);
        context->stroke();

        // Arcs in inductor
        for (int i = 0; i <= 12; i += 4)
        {
            context->arc(x + half_space_x,
                         y + 4 * small_space_y + i * small_space_y,
                         2 * small_space_y,
                         3.0 * M_PI / 2.0,
                         M_PI / 2.0);
            context->stroke();
        }
    }
    else
    {
        // Horizontal line in inductor
        context->move_to(x, y + half_space_y);
        context->line_to(x + 2 * small_space_x, y + half_space_y);
        context->stroke();

        context->move_to(x + width - 2 * small_space_x, y + half_space_y);
        context->line_to(x + width, y + half_space_y);
        context->stroke();

        // Arcs in inductor
        for (int i = 0; i <= 12; i += 4)
        {
            context->arc(x + 4 * small_space_x + i * small_space_x,
                         y + half_space_y,
                         2 * small_space_x,
                         M_PI,
                         2.0 * M_PI);
            context->stroke();
        }
    }
}

void crossover_image_view::draw_resistor(Cairo::RefPtr<Cairo::Context> const& context,
                                         int id,
                                         int x,
                                         int y,
                                         int width,
                                         int height,
                                         bool rotate)
{
    auto const half_space_y = std::round(height / 2.0);
    auto const half_space_x = std::round(width / 2.0);
    auto const small_space_x = std::round(width / 20.0);
    auto const small_space_y = std::round(height / 20.0);

    context->set_font_size(18);
    context->move_to(x, y);
    context->select_font_face("FreeSans",
                              Cairo::FONT_SLANT_NORMAL,
                              Cairo::FONT_WEIGHT_NORMAL);
    context->show_text("R" + std::to_string(id));

    if (rotate)
    {
        // Horizontal line in resistor
        context->move_to(x + half_space_x, y);
        context->line_to(x + half_space_x, y + 3 * small_space_y);
        context->stroke();

        context->move_to(x + half_space_x, y + height - 3 * small_space_y);
        context->line_to(x + half_space_x, y + height);
        context->stroke();

        // Draw rectangular box in resistor
        context->rectangle(x + half_space_x - 2 * small_space_x,
                           y + 3 * small_space_y,
                           4 * small_space_x,
                           height - 6 * small_space_y);
        context->stroke();
    }
    else
    {
        // Horizontal line in resistor
        context->move_to(x, y + half_space_y);
        context->line_to(x + (3 * small_space_x), y + half_space_y);
        context->stroke();

        context->move_to(x + width - (3 * small_space_x), y + half_space_y);
        context->line_to(x + width, y + half_space_y);
        context->stroke();

        // Draw rectangular box in resistor
        context->rectangle(x + 3 * small_space_x,
                           y + half_space_y - 2 * small_space_y,
                           width - 6 * small_space_x,
                           4 * small_space_y);
        context->stroke();
    }
}

void crossover_image_view::draw_connector(Cairo::RefPtr<Cairo::Context> const& context,
                                          int x,
                                          int y,
                                          int width,
                                          int height,
                                          bool positive)
{
    auto const half_space_x = std::round(width / 2.0);
    auto const half_space_y = std::round(height / 2.0);
    auto const small_space_x = std::round(width / 20.0);

    context->set_font_size(18);
    context->move_to(x + std::round(half_space_x / 2.0),
                     y + std::round(half_space_y / 2.0));
    context->select_font_face("FreeSans",
                              Cairo::FONT_SLANT_NORMAL,
                              Cairo::FONT_WEIGHT_NORMAL);
    context->show_text((positive ? "+" : "-"));
    context->stroke();

    // Draw a "connector" circle
    context->arc(x + half_space_x, y + half_space_y, 2.0 * small_space_x, 0.0, 2.0 * M_PI);

    // Draw line from connector to edge of this component
    context->move_to(x + half_space_x + 2 * small_space_x, y + half_space_y);
    context->line_to(x + width, y + half_space_y);
    context->stroke();
}

void crossover_image_view::draw_t_cross(Cairo::RefPtr<Cairo::Context> const& context,
                                        int x,
                                        int y,
                                        int width,
                                        int height,
                                        bool upper)
{
    auto const half_space_x = std::round(width / 2.0);
    auto const half_space_y = std::round(height / 2.0);

    context->move_to(x, y + half_space_y);
    context->line_to(x + width, y + half_space_y);
    context->stroke();

    if (upper)
    {
        context->move_to(x + half_space_x, y + half_space_y);
        context->line_to(x + half_space_x, y + height);
    }
    else
    {
        context->move_to(x + half_space_x, y);
        context->line_to(x + half_space_x, y + half_space_y);
    }
    context->stroke();
}

void crossover_image_view::draw_corner(Cairo::RefPtr<Cairo::Context> const& context,
                                       int x,
                                       int y,
                                       int width,
                                       int height,
                                       bool upper)
{
    auto const half_space_x = std::round(width / 2.0);
    auto const half_space_y = std::round(height / 2.0);

    context->move_to(x, y + half_space_y);
    context->line_to(x + half_space_x, y + half_space_y);
    context->stroke();

    if (upper)
    {
        context->move_to(x + half_space_x, y + half_space_y);
        context->line_to(x + half_space_x, y + height);
    }
    else
    {
        context->move_to(x + half_space_x, y);
        context->line_to(x + half_space_x, y + half_space_y);
    }
    context->stroke();
}

void crossover_image_view::draw_line(Cairo::RefPtr<Cairo::Context> const& context,
                                     int x,
                                     int y,
                                     int width,
                                     int height,
                                     bool rotate)
{
    if (rotate)
    {
        auto const half_space_x = std::round(width / 2.0);
        context->move_to(x + half_space_x, y);
        context->line_to(x + half_space_x, y + height);
    }
    else
    {
        auto const half_space_y = std::round(height / 2.0);
        context->move_to(x, y + half_space_y);
        context->line_to(x + width, y + half_space_y);
    }
    context->stroke();
}

void crossover_image_view::draw_woofer(Cairo::RefPtr<Cairo::Context> const& context,
                                       int x,
                                       int y,
                                       int width,
                                       int height,
                                       bool positive_up)
{
    auto const half_space_x = std::round(width / 2.0);
    auto const half_space_y = std::round(height / 2.0);
    auto const small_space_x = std::round(width / 20.0);
    auto const small_space_y = std::round(height / 20.0);

    context->set_font_size(18);
    context->move_to(x + 3 * small_space_x, y + small_space_y);
    context->select_font_face("FreeSans",
                              Cairo::FONT_SLANT_NORMAL,
                              Cairo::FONT_WEIGHT_NORMAL);
    context->show_text((positive_up ? "+" : "-"));
    context->stroke();

    // Draw a vertical line through the driver
    context->move_to(x + half_space_x, y);
    context->line_to(x + half_space_x, y + height);
    context->stroke();

    // Draw the actual woofer
    context->rectangle(x + half_space_x - 4 * small_space_x,
                       y + half_space_y - 5 * small_space_y,
                       4 * small_space_x,
                       10 * small_space_y);
    context->stroke();

    context->move_to(x + half_space_x, y + half_space_y - 4 * small_space_y);
    context->line_to(x + half_space_x + 6 * small_space_x, y);
    context->stroke();

    context->move_to(x + half_space_x, y + half_space_y + 4 * small_space_y);
    context->line_to(x + half_space_x + 6 * small_space_x, y + height);
    context->stroke();

    context->move_to(x + half_space_x + 6 * small_space_x, y);
    context->line_to(x + half_space_x + 6 * small_space_x, y + height);
    context->stroke();
}

void crossover_image_view::draw_midrange(Cairo::RefPtr<Cairo::Context> const& context,
                                         int x,
                                         int y,
                                         int width,
                                         int height,
                                         bool positive_up)
{
    auto const half_space_x = std::round(width / 2.0);
    auto const half_space_y = std::round(height / 2.0);
    auto const small_space_x = std::round(width / 20.0);
    auto const small_space_y = std::round(height / 20.0);

    context->set_font_size(18);
    context->move_to(x + 5 * small_space_x, y + 3 * small_space_y);
    context->select_font_face("FreeSans",
                              Cairo::FONT_SLANT_NORMAL,
                              Cairo::FONT_WEIGHT_NORMAL);
    context->show_text((positive_up ? "+" : "-"));
    context->stroke();

    // Draw a vertical line through the driver
    context->move_to(x + half_space_x, y);
    context->line_to(x + half_space_x, y + height);
    context->stroke();

    // Draw the actual woofer
    context->rectangle(x + half_space_x - 2 * small_space_x,
                       y + half_space_y - 3 * small_space_y,
                       2 * small_space_x,
                       6 * small_space_y);
    context->stroke();

    context->move_to(x + half_space_x, y + half_space_y - 2 * small_space_y);
    context->line_to(x + half_space_x + 4 * small_space_x, y + 2 * small_space_y);
    context->stroke();

    context->move_to(x + half_space_x, y + half_space_y + 2 * small_space_y);
    context->line_to(x + half_space_x + 4 * small_space_x, y + height - 2 * small_space_y);
    context->stroke();

    context->move_to(x + half_space_x + 4 * small_space_x, y + 2 * small_space_y);
    context->line_to(x + half_space_x + 4 * small_space_x, y + height - 2 * small_space_y);
    context->stroke();
}

void crossover_image_view::draw_tweeter(Cairo::RefPtr<Cairo::Context> const& context,
                                        int x,
                                        int y,
                                        int width,
                                        int height,
                                        bool positive_up)
{
    auto const half_space_x = std::round(width / 2.0);
    auto const half_space_y = std::round(height / 2.0);
    auto const small_space_x = std::round(width / 20.0);
    auto const small_space_y = std::round(height / 20.0);

    context->set_font_size(18);
    context->move_to(x + 5 * small_space_x, y + 3 * small_space_y);
    context->select_font_face("FreeSans",
                              Cairo::FONT_SLANT_NORMAL,
                              Cairo::FONT_WEIGHT_NORMAL);
    context->show_text((positive_up ? "+" : "-"));
    context->stroke();

    // Draw a vertical line through the driver
    context->move_to(x + half_space_x, y);
    context->line_to(x + half_space_x, y + height);
    context->stroke();

    // Draw the driver
    context->rectangle(x + half_space_x,
                       y + half_space_y - 3 * small_space_y,
                       2 * small_space_x,
                       6 * small_space_y);
    context->stroke();

    // Draw tweeter dome
    context->arc(x + half_space_x + 2 * small_space_x,
                 y + half_space_y,
                 small_space_y,
                 3.0 * M_PI / 2.0,
                 M_PI / 2.0);
    context->stroke();

    // Draw flat plate
    context->move_to(x + half_space_x + 2 * small_space_x,
                     y + half_space_y - 5 * small_space_y);
    context->line_to(x + half_space_x + 2 * small_space_x,
                     y + half_space_y + 5 * small_space_y);
    context->stroke();
}

void crossover_image_view::draw_text(Cairo::RefPtr<Cairo::Context> const& context,
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
