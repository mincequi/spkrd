/*
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
Signal0<void> signal_part_modified;
Signal1<void, SigC::Object *> signal_crossover_selected;
Signal1<void, string> signal_speakerlist_loaded;
Signal1<void, Box *> signal_box_selected;
Signal1<void, Box *> signal_add_to_boxlist;
Signal3<void, Box *, Speaker *, Gdk::Color&> signal_add_plot;
Signal1<void, Box *> signal_box_modified;
Signal2<void, vector<GSpeakers::Point>&, Gdk::Color&> signal_add_box_plot;

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
}
