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

#include <fstream>
#include <math.h>
#include "summedfreqrespplot.h"
#include "net.h"

#define NOF_POINTS 30

SummedFreqRespPlot::SummedFreqRespPlot() : 
  plot(1, 20000, 50, 110, true, 0)
{
  add(plot);
  
  plot.set_y_label("Magnitude / dB");
  show_all();
  m_color = new Gdk::Color("blue");
  
  g_settings.defaultValueBool("DisableFilterAmp", false);
  
  signal_speakerlist_loaded.connect(slot(*this, &SummedFreqRespPlot::on_speakerlist_loaded));
  signal_add_crossover_plot.connect(slot(*this, &SummedFreqRespPlot::on_add_plot));
  signal_crossover_selected.connect(slot(*this, &SummedFreqRespPlot::on_crossover_selected));
}

SummedFreqRespPlot::~SummedFreqRespPlot()
{

}

int SummedFreqRespPlot::on_add_plot(vector<GSpeakers::Point>& filter_points, Gdk::Color& color, int *i, Net *n)
{
#ifdef OUTPUT_DEBUG
  cout << "SummedFreqRespPlot::on_add_plot" << endl;
#endif

  Speaker s = m_speakerlist->get_speaker_by_id_string(n->get_speaker());
  vector<GSpeakers::Point> freq_resp_points;
  if (s.get_freq_resp_filename() != "") {
    ifstream fin(s.get_freq_resp_filename().c_str());
    cout << "SummedFreqRespPlot::on_add_plot: freq_resp_file = " << s.get_freq_resp_filename() << endl;
    if (fin.good()) {
      for (int j = 0; j < NOF_POINTS; j++) {
        char *buffer = new char[100];
        fin.getline(buffer, 100, '\n');
      
        float f1, f2;
        sscanf(buffer, "%f,%f", &f1, &f2);
        GSpeakers::Point p(GSpeakers::round(f1), f2 );
        freq_resp_points.push_back(p);
      }
    } else {
      cout << "Could not open " << s.get_freq_resp_filename() << endl;
      return -1;
    }
  } else {
    for (int i = 0; i < NOF_POINTS; i++) {
      GSpeakers::Point p(0, 0);
      freq_resp_points.push_back(p);
    }
  }
  vector<GSpeakers::Point> points;
  for (int j = 0; j < NOF_POINTS; j++) {
    double filter_point = filter_points[j].get_y();
    if (g_settings.getValueBool("DisableFilterAmp") == true) {
      if (filter_point > 0.0) {
        filter_point = 0.0;
      }
    }
    GSpeakers::Point p(filter_points[j].get_x(), freq_resp_points[j].get_y() + filter_point);
    points.push_back(p);
  }
  
  Gdk::Color c2("red");
  //plot.add_plot(points, c2);

  /* Search for *i in the graph */
  int position = -1;
  int l = 0;
  for (
    vector<int>::iterator iter = m_nets.begin();
    iter != m_nets.end();
    ++iter)
  {
    if (*i == *iter) {
      position = l;
    }
    l++;
  }

  /* If *i is in the graph, replace the old point-vector, if *i not in graph, insert it at the end of the vector */
  if ((position != -1) && (m_points.size() > 0)) {
    m_points[position] = points;
  } else {
    m_points.push_back(points);
    m_nets.push_back(*i);
  }

  plot.remove_all_plots();
  vector<GSpeakers::Point> pnts;
  if (m_points.size() > 1) {
    pnts = m_points[0];
    plot.add_plot(m_points[0], c2);
    for (unsigned j = 1; j < m_points.size(); j++) {
      for (unsigned k = 0; k < m_points[j].size(); k++) {
        pnts[k].set_y( 10 * log10( pow(10, pnts[k].get_y() / 10) + 
                       pow(10, (m_points[j])[k].get_y() / 10) ) );
      }
      plot.add_plot(m_points[j], c2);
    }
  }
  /* Add summed freq resp plot to the plot and select this plot since it's the most important plot in this graph */
  plot.select_plot(plot.add_plot(pnts, *m_color));

  return 0;
}

void SummedFreqRespPlot::clear()
{
  m_points.erase(m_points.begin(), m_points.end());
  m_nets.erase(m_nets.begin(), m_nets.end());
  plot.remove_all_plots();
}

void SummedFreqRespPlot::on_crossover_selected(Crossover *)
{
  cout << "SummedFreqRespPlot::on_crossover_selected" << endl;
  clear();
}

void SummedFreqRespPlot::on_speakerlist_loaded(SpeakerList *speaker_list)
{
#ifdef OUTPUT_DEBUG
  cout << "SummedFreqRespPlot::on_speakerlist_loaded" << endl;
#endif
  m_speakerlist = speaker_list;
}
