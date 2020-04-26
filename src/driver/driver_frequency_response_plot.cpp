/*
 * $Id$
 *
 * driverfreqrespplot
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

#include "driver_frequency_response_plot.hpp"

#include "common.h"

driver_frequency_response_plot::driver_frequency_response_plot()
    : Gtk::Frame(""), m_plot(1, 20000, 50, 110, true, 0, true)
{
    set_shadow_type(Gtk::SHADOW_NONE);

    m_label.set_markup("<b>" + Glib::ustring(_("Frequency response and impedance")) + "</b>");
    set_label_widget(m_label);

    set_border_width(5);

    sw.add(m_plot);
    sw.set_border_width(12);
    sw.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_NEVER);

    add(sw);

    show_all();

    m_plot.set_y_label(_("Magnitude / dB"));
    m_plot.set_y_label2(_("Impedance / Ohm"));
}

void driver_frequency_response_plot::add_plot(std::vector<gspk::point>& points,
                                              Gdk::Color const& color)
{
    m_plot.add_plot(points, color);
}

void driver_frequency_response_plot::replace_plot(int i,
                                                  std::vector<gspk::point>& points,
                                                  Gdk::Color const& color)
{
    m_plot.replace_plot(i, points, color);
}
