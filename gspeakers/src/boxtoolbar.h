/* 
 * boxtoolbar.h
 *
 * Copyright (C) 2001-2002 Daniel Sundberg <dss@home.se>
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

#ifndef __BOXTOOLBAR_H
#define __BOXTOOLBAR_H

#include <gtk--/handlebox.h>
#include <gtk--/button.h>
#include <gtk--/combo.h>
#include <gtk--/entry.h>
#include <gtk--/box.h>
#include <gtk--/menu.h>
#include <gtk--/optionmenu.h>
#include <gtk--/fileselection.h>
#include <gtk--/eventbox.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <string>
#include <vector>
#include "gspeakerscfg.h"

#define SEALED 10001
#define PORTED 10002

class Box {
 public:
  Box() {};
  Box(string iname, string ivol1, string itype = "Sealed", string ifb1 = "", 
      string ivol2 = "", string ifb2 = "") {
    name = iname;
    if ( itype == "Sealed" ) {
      type = SEALED;
    } else if ( itype == "Ported" ) {
      type = PORTED;
    } else {
      type = SEALED;
    }
    vol1 = atof( ivol1.c_str() );
    fb1 = atof( ifb1.c_str() );
    vol2 = atof( ivol2.c_str() );
    fb2 = atof( ifb2.c_str() );
  };

  Box(string iname, double ivol1, int itype = SEALED, double ifb1 = 0, 
      double ivol2 = 0, double ifb2 = 0) {
    name = iname;
    vol1 = ivol1;
    type = itype;
    fb1 = ifb1;
    vol2 = ivol2;
    fb2 = ifb2;
  };

  Box(string iname, string ivol1, int itype = SEALED, string ifb1 = "", 
      string ivol2 = "", string ifb2 = "") {
    name = iname;
    type = itype;
    vol1 = atof( ivol1.c_str() );
    fb1 = atof( ifb1.c_str() );
    vol2 = atof( ivol2.c_str() );
    fb2 = atof( ifb2.c_str() );
  };



  string name; 
  int type;
  double vol1;
  double fb1;
  double vol2;
  double fb2;
};

class BoxToolbar : public Gtk::HandleBox {
 public:
  BoxToolbar( string inname = "enclosures1.xml", GSpeakersCFG *icfg = NULL );
  ~BoxToolbar();
  
  void new_box();
  void new_xml();
  void open();
  void save();
  void save_as();
  void remove();
  void load_xml(string filename);
  void info();
  void option_menu_changed( int new_type );
  void set_box_data(double vol1, double fb1 = 0, double vol2 = 0, double fb2 = 0);
  Box *get_box(void);
  void set_toolbar_style( int style );
  vector<Box> get_all_boxes() { return box_list; }
  void vol1_entry_changed();
  void changed();

 private:
  void open_action( Gtk::FileSelection *s );
  void save_as_action( Gtk::FileSelection *s );
  void cancel_action( Gtk::FileSelection *s );
  void save_data_to_xml();
  
  int set_box_data( string );
  void combo_changed();

  bool new_is_hit;
  int current_boxtype;
  string current_file;
  vector<Box> box_list;
  vector<string> box_string_list;
  xmlDocPtr doc;

  Gtk::HBox *hbox;
  Gtk::Button *new_box_button;
  Gtk::Button *new_xml_button;
  Gtk::Button *open_button;
  Gtk::Button *save_button;
  Gtk::Button *save_as_button;
  Gtk::Button *remove_button;
  Gtk::Entry *filename_entry;
  Gtk::Combo *box_combo;
  Gtk::OptionMenu *box_type_option;
  Gtk::Menu *types_menu;
  Gtk::Button *info_button;
  Gtk::Entry *vol1_entry;
  Gtk::Entry *fb1_entry;
  Gtk::Entry *vol2_entry;
  Gtk::Entry *fb2_entry;
  Gtk::EventBox *evbox;

  GSpeakersCFG *cfg;
}; 






#endif
