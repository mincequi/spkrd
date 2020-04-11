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

#pragma once

#include "plot.hpp"

#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>

#include <vector>

/// This is a wrapper class for GSpeakersPlot
class driver_frequency_response_plot : public Gtk::Frame
{
public:
    driver_frequency_response_plot();

    void clear() { m_plot.remove_all_plots(); }

    void add_plot(std::vector<gspk::point>&, Gdk::Color const& colour);

    void replace_plot(int, std::vector<gspk::point>&, Gdk::Color const& colour);

private:
    plot m_plot;
    Gtk::Label m_label;
    Gtk::ScrolledWindow sw;
};
