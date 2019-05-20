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
#include <math.h>

TotalFilterPlot::TotalFilterPlot() : plot(1, 20000) {
  using namespace sigc;

  add(plot);
  // signal_add_box_plot.connect(mem_fun(plot, &GSpeakersPlot::add_plot));
  // signal_remove_box_plot.connect(mem_fun(plot, &GSpeakersPlot::remove_plot));
  // signal_hide_box_plot.connect(mem_fun(plot, &GSpeakersPlot::hide_plot));
  // signal_select_plot.connect(mem_fun(plot, &GSpeakersPlot::select_plot));
  signal_plot_crossover.connect(mem_fun(*this, &TotalFilterPlot::on_plot_crossover));
  signal_add_crossover_plot.connect(mem_fun(*this, &TotalFilterPlot::on_add_plot));
  //  signal_crossover_selected.connect(mem_fun(*this, &TotalFilterPlot::on_crossover_selected));

  plot.set_y_label(_("Magnitude / dB"));
  show_all();
  m_color = new Gdk::Color("blue");
}

TotalFilterPlot::~TotalFilterPlot() {}

int TotalFilterPlot::on_add_plot(vector<GSpeakers::Point>& points, Gdk::Color& color, int* i,
                                 Net* n) {
#ifdef OUTPUT_DEBUG
  cout << "TotalFilterPlot::on_add_plot" << endl;
#endif
  /* Search for *i in the graph */
  int position = -1;
  int l = 0;
  for (vector<int>::iterator iter = m_nets.begin(); iter != m_nets.end(); ++iter) {
    if (*i == *iter) {
      position = l;
    }
    l++;
  }

  // cout << "TotalFilterPlot::on_add_plot: position, *i = " << position << ", " << *i << endl;

  /* Om *i is in the graph, replace the old point-vector, if *i not in graph, insert it at the end
   * of the vector */
  if ((position != -1) && (m_points.size() > 0)) {
    m_points[position] = points;
  } else {
    m_points.push_back(points);
    m_nets.push_back(*i);
  }

  // cout << "TotalFilterPlot::on_add_plot: m_nets.size(), m_points.size() = " << m_nets.size() <<
  // ", " << m_points.size() << endl;

  /* sum the plots into one great plot */
  Gdk::Color c("red");
  vector<GSpeakers::Point> pnts;
  plot.remove_all_plots();
  if (m_points.size() > 1) {
    pnts = m_points[0];
    plot.add_plot(m_points[0], c);
    for (unsigned j = 1; j < m_points.size(); j++) {
      for (unsigned k = 0; k < m_points[j].size(); k++) {
        pnts[k].set_y(
            10 * log10(pow(10, pnts[k].get_y() / 10) + pow(10, (m_points[j])[k].get_y() / 10)));
      }
      plot.add_plot(m_points[j], c);
    }
  }

  // kolla om *i finns i grafen
  // om *i finns, dra bort den och lägg sedan till den nya
  // om *i inte finns, lägg till den och spara vektorn
  // rita om plotten

  // if (position != -1) {
  //  cout << "TotalFilterPlot::on_add_plot: replace plot" << endl;

  plot.select_plot(plot.add_plot(pnts, *m_color));
  //} else {
  //  plot.add_plot(pnts, *m_color);
  //  cout << "TotalFilterPlot::on_add_plot: add plot" << endl;
  //}
  //  if (*i == -1) {
  //    *i = plot.add_plot(points, color);
  //  } else {
  //    plot.replace_plot(*i, points, color);
  //  }
  // cout << "TotalFilterPlot::on_add_plot: after add plot" << endl;
  return 0;
}

void TotalFilterPlot::clear() {
  m_points.erase(m_points.begin(), m_points.end());
  m_nets.erase(m_nets.begin(), m_nets.end());
  plot.remove_all_plots();
}

void TotalFilterPlot::on_crossover_selected(Crossover*) { clear(); }

void TotalFilterPlot::on_plot_crossover() {
  // clear();
}

bool TotalFilterPlot::on_delete_event(GdkEventAny* event) {
  delete m_color;
  /* Don't wanna delete this window */
  return true;
}
