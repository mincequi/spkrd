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

#include "common.h"

#include <glibmm/miscutils.h>

#include <gdkmm/pixbuf.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

Settings g_settings;

sigc::signal1<void, Crossover*> signal_crossover_selected;
sigc::signal1<void, SpeakerList*> signal_speakerlist_loaded;
sigc::signal1<void, Box*> signal_box_selected;
sigc::signal1<void, Box*> signal_add_to_boxlist;
sigc::signal3<void, Box*, Speaker*, Gdk::Color&> signal_add_plot;
sigc::signal1<void, Box*> signal_box_modified;
sigc::signal2<int, std::vector<GSpeakers::Point>&, Gdk::Color&> signal_add_box_plot;
sigc::signal1<void, int> signal_remove_box_plot;
sigc::signal1<void, int> signal_hide_box_plot;
sigc::signal1<void, int> signal_select_plot;
sigc::signal0<void> signal_net_modified_by_wizard;
sigc::signal1<void, Net*> signal_net_modified_by_user;
sigc::signal1<void, int> signal_new_crossover;
sigc::signal0<void> signal_plot_crossover;
sigc::signal4<int, std::vector<GSpeakers::Point>&, Gdk::Color&, int*, Net*>
    signal_add_crossover_plot;
sigc::signal0<void> signal_save_open_files;

namespace GSpeakers {
Glib::ustring double_to_ustring(double d) {
  char* str = nullptr;
  GString* buffer = g_string_new(str);
  g_string_printf(buffer, "%f", d);
  return Glib::ustring(buffer->str);
}

Glib::ustring double_to_ustring(double d, int format_len, int format_dec) {
  char* str1 = nullptr;
  GString* buffer1 = g_string_new(str1);
  g_string_printf(buffer1, "%%%d.%df", format_len, format_dec);
  char* str2 = nullptr;
  GString* buffer2 = g_string_new(str2);
  g_string_printf(buffer2, buffer1->str, d);
  return Glib::ustring(buffer2->str);
}

Glib::ustring int_to_ustring(int d) {
  char* str = nullptr;
  GString* buffer = g_string_new(str);
  g_string_printf(buffer, "%d", d);
  return Glib::ustring(buffer->str);
}

Gtk::Widget& image_widget(const std::string& filename) {
  Gtk::Widget* im;
  try {
#ifdef TARGET_WIN32
    Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file(filename);
#else
    Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file(
        std::string(GSPEAKERS_PREFIX) + std::string("/share/pixmaps/") + filename);
#endif
    im = manage(new Gtk::Image(pixbuf));
  } catch (Glib::FileError const& fe) {
    im = manage(new Gtk::Label());
  } catch (Gdk::PixbufError const& pe) {
    im = manage(new Gtk::Label());
  }
  return *im;
}

Glib::ustring short_filename(const Glib::ustring& filename, unsigned length) {

  Glib::ustring shorted_filename;
  if (filename.length() >= length) {
    Glib::ustring file = Glib::path_get_basename(filename);
    Glib::ustring dir = Glib::path_get_dirname(filename);
    int base_length = length - file.length() - round(length / 4);
    if (base_length < 2) {
      base_length = 2;
    }
    shorted_filename = filename.substr(0, base_length);
    int space_left = length - file.length() - shorted_filename.length() - 6;
    if (space_left <= 0) {
      space_left = 1;
    }
    shorted_filename =
        shorted_filename + "..." + dir.substr(dir.length() - space_left, dir.length()) + "/" + file;
  } else {
    return filename;
  }
  return shorted_filename;
}

Gtk::Tooltips& tooltips() {
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

} // namespace GSpeakers
