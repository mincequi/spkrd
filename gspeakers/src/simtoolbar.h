/* 
 * simtoolbar.h
 *
 * Copyright (C) 2001 Daniel Sundberg <dss@home.se>
 *
 * http://sumpan.campus.luth.se/software/jags
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

#ifndef __SIMTOOLBAR_H
#define __SIMTOOLBAR_H

#include <gtk--/handlebox.h>
#include <gtk--/button.h>
#include <gtk--/box.h>
#include <gdk--/color.h>
#include "gspeakerscfg.h"
#include "speakertoolbar.h"
#include "boxtoolbar.h"
#include "gspeakersplot.h"
#include "boxlist.h"

#define COLOR_RED 1
#define COLOR_GREEN 2
#define COLOR_BLUE 3

/*
 * This is the simulation toolbar, here we have the plot-button and stuff like that
 *
 * This class has all info and is the heart in this app.
 *
 */
class SimToolbar : public Gtk::HandleBox {
 public:
  SimToolbar(SpeakerToolbar *stoolbar, BoxToolbar *btoolbar, GSpeakersPlot *gplot, BoxList *blist, GSpeakersCFG *icfg);
  ~SimToolbar();

 protected:
  /* Callbacks */
  void sim_clicked();
  void rem_plot_clicked();
  void opt_box_clicked();
  void set_new_color();
  void about_clicked();
  void rem_all_clicked();

  SpeakerToolbar *sbar;
  BoxToolbar *bbar;
  GSpeakersPlot *plot;
  BoxList *blist;

  Gtk::HBox *hbox;
  Gtk::Button *sim_button;
  Gtk::Button *rem_plot_button;
  Gtk::Button *opt_box_button;
  Gtk::Button *about_button;
  Gtk::Button *rem_all_button;
 private:
  Gdk_Color *color;
  double red, green, blue;
  int color_i, color_j, color_k;
  int last_color;
  GSpeakersCFG *cfg;
}; 






#endif
