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

#ifndef __GSPEAKERS_FILTER_PLOT
#define __GSPEAKERS_FILTER_PLOT

#include "common.h"
#include "plot.hpp"

#include <gtkmm/frame.h>

/// This is a wrapper class for GSpeakersPlot
/// The reason why we have this class is that we want
/// an extra layer (where we can connect signals and so on)
/// between the program and the plot widget.
class GSpeakersFilterPlot : public Gtk::Frame
{
public:
    GSpeakersFilterPlot();

    ~GSpeakersFilterPlot() override = default;

    void clear();

    int on_add_plot(std::vector<GSpeakers::Point> const& points,
                    Gdk::Color const& color,
                    int& i,
                    Net* n);

private:
    [[deprecated]] bool on_delete_event(GdkEventAny* event) override;

    void on_crossover_selected(Crossover*);

private:
    plot m_plot;
};

#endif
