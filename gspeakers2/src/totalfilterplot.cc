/* totalfilterplot
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

#include "totalfilterplot.h"

TotalFilterPlot::TotalFilterPlot() : 
  plot(1, 20000)
{
  add(plot);
  m_color = Gdk::Color("blue");
  //signal_add_box_plot.connect(slot(plot, &GSpeakersPlot::add_plot));
  //signal_remove_box_plot.connect(slot(plot, &GSpeakersPlot::remove_plot));
  //signal_hide_box_plot.connect(slot(plot, &GSpeakersPlot::hide_plot));
  //signal_select_plot.connect(slot(plot, &GSpeakersPlot::select_plot));
  signal_add_crossover_plot.connect(slot(*this, &TotalFilterPlot::on_add_plot));
  signal_crossover_selected.connect(slot(*this, &TotalFilterPlot::on_crossover_selected));
  show_all();
}

TotalFilterPlot::~TotalFilterPlot()
{

}

int TotalFilterPlot::on_add_plot(vector<GSpeakers::Point>& points, Gdk::Color& color, int *i)
{
  cout << "TotalFilterPlot::on_add_plot" << endl;
  /* om vi har *i i grafen, dra bort den och lägg till den nya */
  /* om vi inte har *i i grafen, lägg till den till grafen och spara samt spara index*/
//  if (*i == -1) {
//    *i = plot.add_plot(points, color);
//  } else {
//    plot.replace_plot(*i, points, color);
//  }
  return 0;
}

void TotalFilterPlot::clear()
{
  /* töm vektorer här också */
  plot.remove_all_plots();
}

void TotalFilterPlot::on_crossover_selected(Crossover *)
{
  clear();
}

bool TotalFilterPlot::on_delete_event(GdkEventAny *event)
{
  /* Don't wanna delete this window */
  return true;
}
