/* 
 * speakertoolbar.h
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

#ifndef __SPEAKERTOOLBAR_H
#define __SPEAKERTOOLBAR_H

#include <string>
#include <vector>
#include <list>
#include <gtk--/handlebox.h>
#include <gtk--/button.h>
#include <gtk--/combo.h>
#include <gtk--/entry.h>
#include <gtk--/box.h>
#include <gtk--/fileselection.h>
#include <libxml/tree.h>

class Speaker {
 public:
  Speaker() {};
  Speaker(string iname, string iqts, string ivas, string ifs) {
    name = iname;
    qts = atof( iqts.c_str() );
    vas = atof( ivas.c_str() );
    fs = atof( ifs.c_str() );
  };

  Speaker(string iname, double iqts, double ivas, double ifs) {
    name = iname;
    vas = ivas;
    qts = iqts;
    fs = ifs;
  };

  string name; 
  double qts;
  double vas;
  double fs;
};

class SpeakerToolbar : public Gtk::HandleBox {
 public:
  SpeakerToolbar( string inname = "speakers1.xml" );
  ~SpeakerToolbar();
  
  Speaker *get_current_speaker();
  Speaker *get_speaker() { return get_current_speaker() ; }
  void new_spk();
  void new_xml();
  void open();
  void save();
  void save_as();
  void remove();
  void load_xml(string filename);

 private:
  void open_action( Gtk::FileSelection *s );
  void save_as_action( Gtk::FileSelection *s );
  void cancel_action( Gtk::FileSelection *s );
  void save_data_to_xml();
  
  int set_speaker_data( string speaker );
  void combo_changed();
  /* We store the speakers in xml in memory */
  xmlDocPtr doc;
  vector<string> speakers_list;
  vector<Speaker> spk_list;
  Speaker *spk;
  string current_file;
  bool new_is_hit;

  Gtk::HBox *hbox;
  Gtk::Button *new_spk_button;
  Gtk::Button *new_xml_button;
  Gtk::Button *open_button;
  Gtk::Button *save_button;
  Gtk::Button *save_as_button;
  Gtk::Button *remove_button;
  Gtk::Combo *speaker_combo;
  Gtk::Entry *vas_entry;
  Gtk::Entry *fs_entry;
  Gtk::Entry *qts_entry;
  Gtk::Entry *filename_entry;
}; 

#endif
