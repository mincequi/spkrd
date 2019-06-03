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
#include <cmath>

TotalFilterPlot::TotalFilterPlot() : plot(1, 20000), m_color(std::make_unique<Gdk::Color>("blue")) {

  add(plot);

  signal_plot_crossover.connect(mem_fun(*this, &TotalFilterPlot::on_plot_crossover));
  signal_add_crossover_plot.connect(mem_fun(*this, &TotalFilterPlot::on_add_plot));

  plot.set_y_label(_("Magnitude / dB"));
  show_all();
}

TotalFilterPlot::~TotalFilterPlot() = default;

int TotalFilterPlot::on_add_plot(std::vector<GSpeakers::Point>& points, Gdk::Color& color, int* i,
                                 Net* n) {
#ifdef OUTPUT_DEBUG
  std::cout << "TotalFilterPlot::on_add_plot" << std::endl;
#endif
  /* Search for *i in the graph */
  auto const position = std::find(begin(m_nets), end(m_nets), *i);

  /* If *i is in the graph, replace the old point-vector, if *i not in graph, insert it at the end
   * of the vector */
  if (position != end(m_nets) && !m_points.empty()) {
    m_points[std::distance(begin(m_nets), position)] = points;
  } else {
    m_points.push_back(points);
    m_nets.push_back(*i);
  }

  // sum the plots into one great plot
  Gdk::Color c("red");
  std::vector<GSpeakers::Point> pnts;
  plot.remove_all_plots();
  if (m_points.size() > 1) {
    pnts = m_points[0];
    plot.add_plot(m_points[0], c);
    for (std::size_t j = 1; j < m_points.size(); j++) {
      for (std::size_t k = 0; k < m_points[j].size(); k++) {
        pnts[k].set_y(10 * std::log10(std::pow(10, pnts[k].get_y() / 10) +
                                      std::pow(10, (m_points[j])[k].get_y() / 10)));
      }
      plot.add_plot(m_points[j], c);
    }
  }
  plot.select_plot(plot.add_plot(pnts, *m_color));

  return 0;
}

void TotalFilterPlot::clear() {
  m_points.erase(m_points.begin(), m_points.end());
  m_nets.erase(m_nets.begin(), m_nets.end());
  plot.remove_all_plots();
}

void TotalFilterPlot::on_crossover_selected(Crossover*) { clear(); }

void TotalFilterPlot::on_plot_crossover() {}

bool TotalFilterPlot::on_delete_event(GdkEventAny* event) {
  m_color.reset();
  /* Don't wanna delete this window */
  return true;
}
