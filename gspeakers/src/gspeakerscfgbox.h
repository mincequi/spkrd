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
#include "gspeakerscfg.h"

class GSpeakersCFGBox : public Gnome::PropertyBox {
 public:
  GSpeakersCFGBox( GSpeakersCFG *icfg );

 protected:
  /* Callbacks */
  virtual void apply_impl( gint page_num );
  virtual void close_impl( gint page_num );
  
  void browse_font();
  void browse_font_ok();
  void browse_font_cancel();

 private:
  string oldfont;
  GSpeakersCFG *cfg;
  Gtk::Entry *font_entry;
  Gtk::Button *select_font_button;
  Gtk::Table *table;
  Gtk::FontSelectionDialog *font_dialog;
}; 

#endif
