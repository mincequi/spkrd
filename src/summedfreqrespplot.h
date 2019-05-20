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

#ifndef __SUMMED_FREQ_RESP_PLOT
#define __SUMMED_FREQ_RESP_PLOT

#include "common.h"
#include "gspeakersplot.h"
#include "speakerlist.h"
#include <gtkmm.h>
#include <vector>

/*
 * This is a wrapper class for GSpeakersPlot
 *
 */
class SummedFreqRespPlot : public Gtk::Frame {
public:
  SummedFreqRespPlot();
  virtual ~SummedFreqRespPlot();
  void clear();
  int on_add_plot(vector<GSpeakers::Point>&, Gdk::Color&, int*, Net*);

private:
  GSpeakersPlot plot;
  vector<int> m_nets;
  Gdk::Color* m_color;
  vector<vector<GSpeakers::Point>> m_points;
  SpeakerList* m_speakerlist;

  void on_crossover_selected(Crossover*);
  void on_speakerlist_loaded(SpeakerList* speaker_list);
};

#endif
