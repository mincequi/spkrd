/* 
 * boxlist.cc
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

#include <gdk--/color.h>
#include "boxlist.h"

BoxList::BoxList( GSpeakersCFG *icfg ) : Gtk::ScrolledWindow() {
  char *headers[] = { "ID", "Type", "Vol1", "F1b", "Speaker", "Box", "" };
  cfg = icfg;
  selected_row = -1;
  set_policy( GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC );
  clist = manage( new Gtk::CList( headers ) );
  clist->select_row.connect( slot( this, &BoxList::row_selected ) );
  add( *clist );
  
  show_all();
}

void BoxList::add_row( vector<string> text, Gdk_Color *c ) {
  /* Fix the row vector */
  text.push_back("");

  clist->rows().push_back( text );
  clist->columns_autosize();

  int last_row = clist->rows().size();

  /* Create the style we use to set the background of the fist cell on every row
     We want to set the bg to be able to indentify the right plot in the plotwindow */
  style = Gtk::Style::create();
  style->set_base( GTK_STATE_NORMAL, *c );
  style->set_bg( GTK_STATE_SELECTED, *c );
  ((clist->rows()[last_row - 1])[0]).set_style( *style );
}

void BoxList::row_selected(gint p0, gint p1, GdkEvent* p2) {
  selected_row = p0;
}

int BoxList::remove_selected_row() {
  int r = selected_row;
  clist->remove_row( selected_row );
  selected_row = -1;

  return r;
}

void BoxList::clear() {
  clist->clear();
}
