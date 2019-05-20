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

#ifndef __DRIVER_FREQ_RESP_PLOT_H
#define __DRIVER_FREQ_RESP_PLOT_H

#include "common.h"
#include "gspeakersplot.h"
#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>

/*
 * This is a wrapper class for GSpeakersPlot
 *
 */
class DriverFreqRespPlot : public Gtk::Frame {
public:
  DriverFreqRespPlot();
  ~DriverFreqRespPlot();
  void clear();
  void add_plot(vector<GSpeakers::Point>&, Gdk::Color&);
  void replace_plot(int,std::vector<GSpeakers::Point>&, Gdk::Color&);

private:
  GSpeakersPlot plot;
  Gtk::ScrolledWindow sw;
};

#endif
