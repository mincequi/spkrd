/*
  $Id$

  common Copyright (C) 2002 Daniel Sundberg

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License version 2
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef __GFILTER_COMMON_H
#define __GFILTER_COMMON_H

#include <string>
#include <sigc++/sigc++.h>
#include <gtkmm.h>
#include <vector>
#include <iostream>
#include "settings.h"
#include "../config.h"

using namespace SigC;
using namespace std;

namespace GSpeakers {
  class Point;
  Glib::ustring double_to_ustring(double d);
  Glib::ustring double_to_ustring(double d, int format_len, int format_dec);
  Glib::ustring int_to_ustring(int d);
  int round(double d);
  Gtk::Widget& image_widget(string filename);
}   

class Box;
class Speaker;
class Crossover;
class Net;
class SpeakerList;

/*
 *  We define a Exception class for our program just for fun (and consistancy)
 *
 */
class GSpeakersException {
  string _what;
public:
  GSpeakersException(const string& what_arg): _what (what_arg) { }
  virtual ~GSpeakersException() {}
  virtual const char* what () const { return _what.c_str (); }
};

extern Settings g_settings;

/*
 * signal_crossover_selected
 *
 * Emit this signal when you want to change current crossover.
 *
 * SigC::Object is the new crossover
 */
//extern Signal1<void, SigC::Object *> signal_crossover_selected;
extern Signal1<void, Crossover *> signal_crossover_selected;

/*
 * signal_speakerlist_loaded
 *
 * Emit this signal when you want to change current speakerlist.
 *
 * SpeakerList arg is the new SpeakerList
 */
//extern Signal1<void, string> signal_speakerlist_loaded;
extern Signal1<void, SpeakerList *> signal_speakerlist_loaded;

/*
 * signal_box_selected
 * 
 * Emit this signal when you want to change the current box
 *
 * Box * is a ptr to the new box
 */
extern Signal1<void, Box *> signal_box_selected;
extern Signal1<void, Box *> signal_box_modified;
extern Signal1<void, Box *> signal_add_to_boxlist;
extern Signal3<void, Box *, Speaker *, Gdk::Color&> signal_add_plot;
extern Signal2<int, vector<GSpeakers::Point>&, Gdk::Color&> signal_add_box_plot;
extern Signal1<void, int> signal_remove_box_plot;
extern Signal1<void, int> signal_hide_box_plot;
extern Signal1<void, int> signal_select_plot;
/* Define two signals for crossover parts updates */
extern Signal0<void> signal_net_modified_by_wizard;  // listen to this in crossover treeview
extern Signal1<void, Net *> signal_net_modified_by_user;    // listan to this in filter wizard
extern Signal1<void, int> signal_new_crossover;
extern Signal0<void> signal_plot_crossover;
extern Signal4<int, vector<GSpeakers::Point>&, Gdk::Color&, int *, Net *> signal_add_crossover_plot;

#endif //__GFILTER_COMMON_H
