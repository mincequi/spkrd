/* crossoverimageview.h
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

#ifndef __CROSSOVER_IMAGE_VIEW_H
#define __CROSSOVER_IMAGE_VIEW_H

#include "crossover.h"
#include "speaker_list.hpp"

#include <gdkmm/rgba.h>
#include <gtkmm/drawingarea.h>

#include <pangomm/layout.h>

#include <string>
#include <vector>

/// The CrossoverImageView is a widget that will display the
/// current crossover as an image of the component layout.
class CrossoverImageView : public Gtk::DrawingArea
{
public:
    CrossoverImageView();

private:
    bool on_draw(Cairo::RefPtr<Cairo::Context> const& context) override;

    bool on_expose_event(GdkEventExpose* event);
    bool on_configure_event(GdkEventConfigure* event) override;

    void on_crossover_selected(Crossover* selected_crossover);
    void on_speakerlist_selected(speaker_list* selected_speaker_list);
    void on_net_modified();
    void on_settings_changed(const std::string& s);

    void redraw();

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
    void draw_capacitor(int id, int x, int y, int width, int height, bool rotate = false);
    void draw_inductor(int id, int x, int y, int width, int height, bool rotate = false);
    void draw_resistor(int id, int x, int y, int width, int height, bool rotate = false);
    void draw_connector(int x, int y, int width, int height, bool positive = true);

    void draw_t_cross(int x, int y, int width, int height, bool upper = true);
    void draw_corner(int x, int y, int width, int height, bool upper = true);
    void draw_line(int x, int y, int width, int height, bool rotate = false);

    void draw_woofer(int x, int y, int width, int height, bool positive_up = true);
    void draw_midrange(int x, int y, int width, int height, bool positive_up = true);
    void draw_tweeter(int x, int y, int width, int height, bool positive_up = true);

    void draw_lowpass_net(int x, int y, int part_width, int part_height, std::vector<Part> const& parts);
    void draw_highpass_net(int x, int y, int part_width, int part_height, std::vector<Part> const& parts);

    void draw_imp_corr_net(int x,
                           int y,
                           int part_width,
                           int part_height,
                           Part const& capacitor,
                           Part const& resistor);
    void draw_damp_net(int x, int y, int part_width, int part_height, Part const& r1, Part const& r2);
    void draw_driver(int x, int y, int part_width, int part_height, Speaker const& speaker);

private:
    bool m_visible{false};
    bool m_scale_image_parts;

    // /// Pixel map
    // Glib::RefPtr<Gdk::Pixmap> m_refPixmap;
    // /// Graphics context
    // Glib::RefPtr<Gdk::GC> m_refGC;
    //
    // Glib::RefPtr<Gdk::Colormap> m_refColormap;

    Glib::RefPtr<Pango::Layout> m_refLayout;
    Gdk::RGBA black, white;

    Crossover* crossover{nullptr};
    speaker_list* m_speaker_list{nullptr};
};

#endif
