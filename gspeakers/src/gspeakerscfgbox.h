/* 
 * gspeakerscfgbox.h
 *
 * Copyright (C) 2001 Daniel Sundberg <dss@home.se>
 *
 * http://sumpan.campus.luth.se/software/gspeakers
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

#ifndef __GSPEAKERSCFGBOX_H
#define __GSPEAKERSCFGBOX_H
#include <string>
#include <gnome--/propertybox.h>
#include <gtk--/entry.h>
#include <gtk--/button.h>
#include <gtk--/table.h>
#include <gtk--/fontselection.h>
#include <gtk--/spinbutton.h>
#include <gtk--/checkbutton.h>
#include "gspeakerscfg.h"
#include "gspeakersplot.h"

class GSpeakersCFGBox : public Gnome::PropertyBox {
 public:
  GSpeakersCFGBox( GSpeakersPlot *iplot, GSpeakersCFG *icfg );

 protected:
  /* Callbacks */
  virtual void apply_impl( gint page_num );
  virtual gboolean close_impl();
  
  void browse_font();
  void browse_font_ok();
  void browse_font_cancel();

 private:
  string oldfont;
  GSpeakersCFG *cfg;
  GSpeakersPlot *plot;
  Gtk::Entry *font_entry;
  Gtk::Button *select_font_button;
  Gtk::Table *table;
  Gtk::FontSelectionDialog *font_dialog;
  Gtk::RadioButton *solid_radio;
  Gtk::RadioButton *ddash_radio;
  Gtk::RadioButton *dash_radio;
  Gtk::SpinButton *line_size_spin;

  Gtk::RadioButton *text_only_radio;
  Gtk::RadioButton *icons_only_radio;
  Gtk::RadioButton *text_and_icons_radio;

  Gtk::CheckButton *show_tooltips_check;

  void reset_radio_state();
}; 

#endif
