/* crossoverimageview.h
 *
 * $Id$
 *
 * Copyright (C) 2001-2002 Daniel Sundberg <dss@home.se>
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

#ifndef __CROSSOVER_IMAGE_VIEW_H
#define __CROSSOVER_IMAGE_VIEW_H

#include <gtkmm/drawingarea.h>
#include <gdkmm/pixmap.h>
#include <gdkmm/gc.h>
#include <gdkmm/colormap.h>
#include <pangomm/layout.h>
#include <gdkmm/color.h>

class CrossoverImageView : public Gtk::DrawingArea {
public:
  CrossoverImageView();
  ~CrossoverImageView();
private:
  bool on_expose_event(GdkEventExpose* event);
  bool on_configure_event(GdkEventConfigure* event);
  
  Glib::RefPtr<Gdk::Pixmap> m_refPixmap;
  Glib::RefPtr<Gdk::GC> m_refGC;
  Glib::RefPtr<Gdk::Colormap> m_refColormap;

  Glib::RefPtr<Pango::Layout> m_refLayout;
  Gdk::Color black, white;
};

#endif
