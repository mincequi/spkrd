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

#include <gdkmm/pixbuf.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <glibmm/miscutils.h>
#include "common.h"

Settings g_settings;

signal1<void, Crossover *> signal_crossover_selected;
signal1<void, SpeakerList *> signal_speakerlist_loaded;
signal1<void, Box *> signal_box_selected;
signal1<void, Box *> signal_add_to_boxlist;
signal3<void, Box *, Speaker *, Gdk::Color&> signal_add_plot;
signal1<void, Box *> signal_box_modified;
signal2<int, vector<GSpeakers::Point>&, Gdk::Color&> signal_add_box_plot;
signal1<void, int> signal_remove_box_plot;
signal1<void, int> signal_hide_box_plot;
signal1<void, int> signal_select_plot;
signal0<void> signal_net_modified_by_wizard;
signal1<void, Net *> signal_net_modified_by_user;
signal1<void, int> signal_new_crossover;
signal0<void> signal_plot_crossover;
signal4<int, vector<GSpeakers::Point>&, Gdk::Color&, int *, Net *> signal_add_crossover_plot;
signal0<void> signal_save_open_files;

namespace GSpeakers {
  Glib::ustring double_to_ustring(double d) 
  {
    char *str = NULL;
    GString *buffer = g_string_new(str);
    g_string_printf(buffer, "%f", d);
    return Glib::ustring(buffer->str);
  }
  
  Glib::ustring double_to_ustring(double d, int format_len, int format_dec) 
  {
    char *str1 = NULL;
    GString *buffer1 = g_string_new(str1);
    g_string_printf(buffer1, "%%%d.%df", format_len, format_dec);
    char *str2 = NULL;
    GString *buffer2 = g_string_new(str2);
    g_string_printf(buffer2, buffer1->str, d);
    return Glib::ustring(buffer2->str);
  }
  
  Glib::ustring int_to_ustring(int d)
  {
    char *str = NULL;
    GString *buffer = g_string_new(str);
    g_string_printf(buffer, "%d", d);
    return Glib::ustring(buffer->str);
  }
  
  int round( double d ) {
    if ( ( d - (int)d ) >= 0.5 ) {
      return (int)( (int)d + 1 );
    } else {
      return (int)(d);
    }
    return 0;
  }
  
  Gtk::Widget& image_widget(const string& filename)
  {
    Gtk::Widget *im;
    try {
#ifdef TARGET_WIN32
      Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file(filename);
#else
      Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file(string(GSPEAKERS_PREFIX) + string("/share/pixmaps/") + filename);
#endif
      im = manage(new Gtk::Image(pixbuf));
    } catch (Glib::FileError fe) {
      im = manage(new Gtk::Label());
    } catch (Gdk::PixbufError pe) {
      im = manage(new Gtk::Label());
    }
    return *im;
  }

//   Gtk::Widget& image_widget(const Gtk::StockID& stockid)
//   {
//     Gtk::Widget *im;

//     return *im;
//   }
  Glib::ustring short_filename(const Glib::ustring& filename, unsigned length)
  {
    using namespace std;

    Glib::ustring shorted_filename;
    if (filename.length() >= length) {
      Glib::ustring file = Glib::path_get_basename(filename);
      Glib::ustring dir = Glib::path_get_dirname(filename);
      int base_length = length - file.length() - round(length/4);
      if (base_length < 2) {
	base_length = 2;
      }
      shorted_filename = filename.substr(0, base_length);
      int space_left = length - file.length() - shorted_filename.length() - 6;
      if (space_left <= 0) {
	space_left = 1;
      }
      shorted_filename = shorted_filename + "..." + dir.substr(dir.length() - space_left, dir.length()) + "/" + file;
    } else {
      return filename;
    }
    return shorted_filename;
  }
  
  Gtk::Tooltips& tooltips() 
  {
    static Gtk::Tooltips tooltips;
    return tooltips;
  }
  
  bool& driverlist_modified() {
    static bool driverlist_mod = false;
    return driverlist_mod;
  }
  
  bool& enclosurelist_modified() {
    static bool enclosurelist_mod = false;
    return enclosurelist_mod;
  }
  
  bool& crossoverlist_modified() {
    static bool crossoverlist_mod = false;
    return crossoverlist_mod;
  }

  bool& meassurementlist_modified() {
    static bool meassurementlist_mod = false;
    return meassurementlist_mod;
  }

}
