/* 
 * enclosurelist.h
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

#ifndef __BOXLIST_H
#define __BOXLIST_H

#include <vector>
#include <gdk--/color.h>
#include <gtk--/clist.h>
#include <gtk--/scrolledwindow.h>
#include <gtk--/style.h>

class BoxList : public Gtk::ScrolledWindow {
 public:
  BoxList();
  //  ~BoxList();
  void add_row( vector<string> text, Gdk_Color *c );
  int remove_selected_row();
  void clear();

 protected:
  void row_selected(gint p0, gint p1, GdkEvent* p2);
  
 private:
  Gtk::CList *clist;
  Gtk::Style *style;
  double red, blue, green;
  int selected_row;
}; 

#endif
