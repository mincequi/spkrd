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

#include "gspeakersfilterplot.h"

GSpeakersFilterPlot::GSpeakersFilterPlot() : plot(1, 20000) {

  add(plot);
  // signal_add_box_plot.connect(sigc::mem_fun(plot, &GSpeakersPlot::add_plot));
  // signal_remove_box_plot.connect(sigc::mem_fun(plot, &GSpeakersPlot::remove_plot));
  // signal_hide_box_plot.connect(sigc::mem_fun(plot, &GSpeakersPlot::hide_plot));
  // signal_select_plot.connect(sigc::mem_fun(plot, &GSpeakersPlot::select_plot));
  signal_add_crossover_plot.connect(sigc::mem_fun(*this, &GSpeakersFilterPlot::on_add_plot));
  signal_crossover_selected.connect(
      sigc::mem_fun(*this, &GSpeakersFilterPlot::on_crossover_selected));
  plot.set_y_label(_("Magnitude / dB"));
  show_all();
}

GSpeakersFilterPlot::~GSpeakersFilterPlot() {}

int GSpeakersFilterPlot::on_add_plot(std::vector<GSpeakers::Point>& points, Gdk::Color& color, int* i,
                                     Net* n) {
#ifdef OUTPUT_DEBUG
  std::cout << "GSpeakersFilterPlot::on_add_plot" << std::endl;
#endif
  if (*i == -1) {
    *i = plot.add_plot(points, color);
  } else {
    plot.replace_plot(*i, points, color);
  }
  plot.select_plot(-1);
  return 0;
}

void GSpeakersFilterPlot::clear() { plot.remove_all_plots(); }

void GSpeakersFilterPlot::on_crossover_selected(Crossover*) { clear(); }

bool GSpeakersFilterPlot::on_delete_event(GdkEventAny* event) {
  /* Don't wanna delete this window */
  return true;
}
