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

#include "driverfreqrespplot.h"

DriverFreqRespPlot::DriverFreqRespPlot() : Gtk::Frame(""), plot(1, 20000, 50, 110, true, 0, true) {
  set_shadow_type(Gtk::SHADOW_NONE);
  static_cast<Gtk::Label*>(get_label_widget())
      ->set_markup("<b>" + Glib::ustring(_("Frequency response and impedance")) + "</b>");
  set_border_width(5);
  sw.add(plot);
  sw.set_border_width(12);
  sw.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_NEVER);
  add(sw);
  show_all();
  plot.set_y_label(_("Magnitude / dB"));
  plot.set_y_label2(_("Impedance / Ohm"));
}

void DriverFreqRespPlot::clear() { plot.remove_all_plots(); }

void DriverFreqRespPlot::add_plot(vector<GSpeakers::Point>& points, Gdk::Color& color) {
  plot.add_plot(points, color);
}

void DriverFreqRespPlot::replace_plot(int i, std::vector<GSpeakers::Point>& points,
                                      Gdk::Color& color) {
  plot.replace_plot(i, points, color);
}
