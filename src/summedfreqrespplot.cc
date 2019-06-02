/*
 * $Id$
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

#include "summedfreqrespplot.h"
#include "net.h"
#include <cmath>
#include <fstream>

SummedFreqRespPlot::SummedFreqRespPlot() : plot(1, 20000, 50, 110, true, 0) {

  m_speakerlist = nullptr;

  add(plot);

  plot.set_y_label(_("Magnitude / dB"));
  show_all();
  m_color = new Gdk::Color("blue");

  g_settings.defaultValueBool("DisableFilterAmp", false);

  signal_speakerlist_loaded.connect(mem_fun(*this, &SummedFreqRespPlot::on_speakerlist_loaded));
  signal_add_crossover_plot.connect(mem_fun(*this, &SummedFreqRespPlot::on_add_plot));
  signal_crossover_selected.connect(mem_fun(*this, &SummedFreqRespPlot::on_crossover_selected));
}

SummedFreqRespPlot::~SummedFreqRespPlot() = default;

double lerp(std::vector<GSpeakers::Point> freq_resp_points, double x) {
  auto first_i = std::lower_bound(freq_resp_points.begin(), freq_resp_points.end(), x,
                                  GSpeakers::Point::_CompareX);
  if (first_i != freq_resp_points.begin()) {
    first_i = first_i - 1;
  }
  auto second_i = std::upper_bound(freq_resp_points.begin(), freq_resp_points.end(), x,
                                   GSpeakers::Point::_CompareX);

  double x0 = first_i->get_x();
  double x1 = second_i->get_x();
  if (x0 == x1) {
    return first_i->get_y();
  }
  double y0 = first_i->get_y();
  double y1 = second_i->get_y();

  return y0 + ((y1 - y0) * ((x - x0) / (x1 - x0)));
}

int SummedFreqRespPlot::on_add_plot(std::vector<GSpeakers::Point>& filter_points, Gdk::Color& color,
                                    int* i, Net* n) {
#ifdef OUTPUT_DEBUG
  std::cout << "SummedFreqRespPlot::on_add_plot" << std::endl;
#endif

  Speaker s;
  if (m_speakerlist != nullptr) {
    s = m_speakerlist->get_speaker_by_id_string(n->get_speaker());
  }
  std::vector<GSpeakers::Point> freq_resp_points;
  if (!s.get_freq_resp_filename().empty()) {
    std::ifstream fin(s.get_freq_resp_filename().c_str());
    std::cout << "SummedFreqRespPlot::on_add_plot: freq_resp_file = " << s.get_freq_resp_filename()
              << std::endl;
    if (fin.good()) {
      while (!fin.eof()) {
        char* buffer = new char[100];
        fin.getline(buffer, 100, '\n');

        float f1, f2;
        // sscanf(buffer, "%f,%f", &f1, &f2);
        char* substr_ptr = strtok(buffer, ",");
        f1 = g_ascii_strtod(substr_ptr, nullptr);
        substr_ptr = strtok(nullptr, ",");
        f2 = g_ascii_strtod(substr_ptr, nullptr);

        if ((f1 != 0) && (f2 != 0)) { // Skip potential column headers
          GSpeakers::Point p(GSpeakers::round(f1), f2);
          freq_resp_points.push_back(p);
        }
        delete buffer;
      }
    } else {
      std::cout << _("Could not open ") << s.get_freq_resp_filename() << std::endl;
      return -1;
    }
  } else {
    GSpeakers::Point p1(0, 0);
    freq_resp_points.push_back(p1);
    GSpeakers::Point p2(20000, 0);
    freq_resp_points.push_back(p2);
  }
  std::vector<GSpeakers::Point> points;
  for (auto& filter_point : filter_points) {
    double filter_y = filter_point.get_y();
    if (g_settings.getValueBool("DisableFilterAmp")) {
      if (filter_y > 0.0) {
        filter_y = 0.0;
      }
    }
    double resp_y = lerp(freq_resp_points, filter_point.get_x());
    GSpeakers::Point p(filter_point.get_x(), resp_y + filter_y);
    points.push_back(p);
  }

  Gdk::Color c2("red");
  // plot.add_plot(points, c2);

  /* Search for *i in the graph */
  int position = -1;
  int l = 0;
  for (int& m_net : m_nets) {
    if (*i == m_net) {
      position = l;
    }
    l++;
  }

  /* If *i is in the graph, replace the old point-vector, if *i not in graph, insert it at the end
   * of thestd::vector */
  if ((position != -1) && (!m_points.empty())) {
    m_points[position] = points;
  } else {
    m_points.push_back(points);
    m_nets.push_back(*i);
  }

  plot.remove_all_plots();
  std::vector<GSpeakers::Point> pnts;
  if (!m_points.empty()) {
    pnts = m_points[0];
    plot.add_plot(m_points[0], c2);
    for (unsigned j = 1; j < m_points.size(); j++) {
      for (unsigned k = 0; k < m_points[j].size(); k++) {
        pnts[k].set_y(
            10 * log10(pow(10, pnts[k].get_y() / 10) + pow(10, (m_points[j])[k].get_y() / 10)));
      }
      plot.add_plot(m_points[j], c2);
    }
  }
  /* Add summed freq resp plot to the plot and select this plot since it's the most important plot
   * in this graph */
  plot.select_plot(plot.add_plot(pnts, *m_color));

  return 0;
}

void SummedFreqRespPlot::clear() {
  m_points.erase(m_points.begin(), m_points.end());
  m_nets.erase(m_nets.begin(), m_nets.end());
  plot.remove_all_plots();
}

void SummedFreqRespPlot::on_crossover_selected(Crossover*) {
  std::cout << "SummedFreqRespPlot::on_crossover_selected" << std::endl;
  clear();
}

void SummedFreqRespPlot::on_speakerlist_loaded(SpeakerList* speaker_list) {
#ifdef OUTPUT_DEBUG
  std::cout << "SummedFreqRespPlot::on_speakerlist_loaded" << std::endl;
#endif
  m_speakerlist = speaker_list;
}
