/*
  $Id$
  
  tabwidget Copyright (C) 2002 Daniel Sundberg

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

#include "tabwidget.h"

TabWidget::TabWidget(string filename, string text) : Gtk::HBox()
{
  try {
    Glib::RefPtr<Gdk::Pixbuf> icon = Gdk::Pixbuf::create_from_file(string(GSPEAKERS_PREFIX) + "/share/pixmaps/" + filename);
    image = manage(new Gtk::Image(icon));
    pack_start(*image);
  } catch (Gdk::PixbufError e) {
#ifdef OUTPUT_DEBUG
    cout << e.code() << endl;
#endif
  }  catch (Glib::FileError fe) {
#ifdef OUTPUT_DEBUG
    cout << fe.code() << endl;
#endif
  }
  
  pack_start(*(manage(new Gtk::Label(text))));
  show_all();

}
