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

Settings g_settings;

Signal0<void> signal_part_modified;
Signal1<void, Crossover *> signal_crossover_selected;
Signal1<void, SpeakerList *> signal_speakerlist_loaded;
Signal1<void, Box *> signal_box_selected;
Signal1<void, Box *> signal_add_to_boxlist;
Signal3<void, Box *, Speaker *, Gdk::Color&> signal_add_plot;
Signal1<void, Box *> signal_box_modified;
Signal2<int, vector<GSpeakers::Point>&, Gdk::Color&> signal_add_box_plot;
Signal1<void, int> signal_remove_box_plot;
Signal1<void, int> signal_hide_box_plot;
Signal1<void, int> signal_select_plot;
Signal0<void> signal_net_modified_by_wizard;
Signal1<void, Net *> signal_net_modified_by_user;
Signal1<void, int> signal_new_crossover;
Signal0<void> signal_plot_crossover;
Signal4<int, vector<GSpeakers::Point>&, Gdk::Color&, int *, Net *> signal_add_crossover_plot;
Signal0<void> signal_clear_filter_plots;

namespace GSpeakers {
  Glib::ustring double_to_ustring(double d) 
  {
    char *str = NULL;
    GString *buffer = g_string_new(str);
    g_string_printf(buffer, "%f", d);
    return Glib::ustring(buffer->str);
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
  
  Gtk::Widget& image_widget(string filename)
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
}
