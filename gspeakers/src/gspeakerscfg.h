/* 
 * gspeakerscfg.h
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

#ifndef __GSPEAKERSCFG_H
#define __GSPEAKERSCFG_H

#include <string>
#include <gdk--/gc.h>
#include <gtk--/tooltips.h>
#include "cconfig.h"

#define TEXT_AND_ICONS 1
#define TEXT_ONLY      2
#define ICONS_ONLY     3

class GSpeakersCFG : public CConfig {
 public:
  GSpeakersCFG();
  ~GSpeakersCFG();
  string get_font();
  GdkLineStyle get_line_style();
  int get_line_size();
  int get_toolbar_style();

  void set_font( string newfont );
  void set_line_style( GdkLineStyle style );
  void set_line_size( int size );
  void set_toolbar_style( int style );
  Gtk::Tooltips *tooltips;

 private:
};

#endif
