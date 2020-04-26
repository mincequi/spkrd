/* crossover_image_view.hpp
 *
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

#pragma once

#include "crossover.hpp"
#include "driver_list.hpp"

#include <gdkmm/rgba.h>
#include <gtkmm/drawingarea.h>

#include <string>
#include <vector>

/// The crossover_image_view is a widget that will display the
/// current crossover as an image of the component layout
class crossover_image_view : public Gtk::DrawingArea
{
public:
    crossover_image_view();

private:
    auto on_draw(Cairo::RefPtr<Cairo::Context> const& context) -> bool override;

    [[deprecated]] auto on_expose_event(GdkEventExpose* event) -> bool;
    [[deprecated]] auto on_configure_event(GdkEventConfigure* event) -> bool override;

    void on_crossover_selected(Crossover* selected_crossover);
    void on_speakerlist_selected(driver_list* selected_speaker_list);
    void on_net_modified();

    void on_settings_changed(const std::string& s);

    void redraw(Cairo::RefPtr<Cairo::Context> const& context);

    /*
     * Draw capacitor to refPixmap
     *
     * id     = component id
     * x      = horizontal position
     * y      = vertical position
     * width  = width
     * height = height
     * rotate = rotate this component 90 degrees
     */
    void draw_capacitor(Cairo::RefPtr<Cairo::Context> const& context,
                        int id,
                        int x,
                        int y,
                        int width,
                        int height,
                        bool rotate = false);

    void draw_inductor(Cairo::RefPtr<Cairo::Context> const& context,
                       int id,
                       int x,
                       int y,
                       int width,
                       int height,
                       bool rotate = false);

    void draw_resistor(Cairo::RefPtr<Cairo::Context> const& context,
                       int id,
                       int x,
                       int y,
                       int width,
                       int height,
                       bool rotate = false);

    void draw_connector(Cairo::RefPtr<Cairo::Context> const& context,
                        int x,
                        int y,
                        int width,
                        int height,
                        bool positive = true);

    void draw_t_cross(Cairo::RefPtr<Cairo::Context> const& context,
                      int x,
                      int y,
                      int width,
                      int height,
                      bool upper = true);

    void draw_corner(Cairo::RefPtr<Cairo::Context> const& context,
                     int x,
                     int y,
                     int width,
                     int height,
                     bool upper = true);

    void draw_line(Cairo::RefPtr<Cairo::Context> const& context,
                   int x,
                   int y,
                   int width,
                   int height,
                   bool rotate = false);

    void draw_woofer(Cairo::RefPtr<Cairo::Context> const& context,
                     int x,
                     int y,
                     int width,
                     int height,
                     bool positive_up = true);

    void draw_midrange(Cairo::RefPtr<Cairo::Context> const& context,
                       int x,
                       int y,
                       int width,
                       int height,
                       bool positive_up = true);

    void draw_tweeter(Cairo::RefPtr<Cairo::Context> const& context,
                      int x,
                      int y,
                      int width,
                      int height,
                      bool positive_up = true);

    void draw_lowpass_net(Cairo::RefPtr<Cairo::Context> const& context,
                          int x,
                          int y,
                          int part_width,
                          int part_height,
                          std::vector<passive_component> const& parts);

    void draw_highpass_net(Cairo::RefPtr<Cairo::Context> const& context,
                           int x,
                           int y,
                           int part_width,
                           int part_height,
                           std::vector<passive_component> const& parts);

    void draw_imp_corr_net(Cairo::RefPtr<Cairo::Context> const& context,
                           int x,
                           int y,
                           int part_width,
                           int part_height,
                           passive_component const& capacitor,
                           passive_component const& resistor);

    void draw_damp_net(Cairo::RefPtr<Cairo::Context> const& context,
                       int x,
                       int y,
                       int part_width,
                       int part_height,
                       passive_component const& r1,
                       passive_component const& r2);

    void draw_driver(Cairo::RefPtr<Cairo::Context> const& context,
                     int x,
                     int y,
                     int part_width,
                     int part_height,
                     driver const& speaker);

    void draw_text(Cairo::RefPtr<Cairo::Context> const& context,
                   std::string const& text,
                   int const center_x,
                   int const center_y);

private:
    bool m_visible{false};
    bool m_scale_image_parts;

    Glib::RefPtr<Pango::Layout> m_refLayout;
    Gdk::RGBA black, white;

    Crossover* m_crossover{nullptr};
    driver_list* m_speaker_list{nullptr};
};
