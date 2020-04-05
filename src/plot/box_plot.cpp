/* gspeakersboxplot
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

#include "common.h"

#include "box_plot.hpp"

box_plot::box_plot() : Gtk::Frame(""), m_plot(1, 1000)
{
    set_border_width(2);
    set_shadow_type(Gtk::SHADOW_NONE);

    m_label.set_markup("<b>" + Glib::ustring(_("Enclosure frequency response")) + "</b>");
    set_label_widget(m_label);

    m_vbox.set_border_width(12);
    m_vbox.pack_start(m_scrolled_window);

    m_scrolled_window.add(m_plot);
    m_scrolled_window.set_shadow_type(Gtk::SHADOW_IN);
    m_scrolled_window.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_NEVER);

    add(m_vbox);

    signal_add_box_plot.connect(sigc::mem_fun(m_plot, &plot::add_plot));
    signal_remove_box_plot.connect(sigc::mem_fun(m_plot, &plot::remove_plot));
    signal_hide_box_plot.connect(sigc::mem_fun(m_plot, &plot::hide_plot));
    signal_select_plot.connect(sigc::mem_fun(m_plot, &plot::select_plot));

    m_plot.set_y_label(_("Magnitude / dB"));

    show_all_children();
}
