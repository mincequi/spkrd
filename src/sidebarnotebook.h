/*
 * $Id$
 *
 * Copyright (C) 2004 Daniel Sundberg <dss@home.se>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __SIDEBAR_NOTEBOOK_H
#define __SIDEBAR_NOTEBOOK_H

#include <gtkmm/box.h>
#include <gtkmm/notebook.h>

#include "libegg/eggsidebar.h"

class SidebarNotebook : public Gtk::HBox {
public:
  SidebarNotebook();
  int append_page(Widget& child, Widget& tab_label);
  int append_page(Widget& child);
  void set_current_page(int page_num);
  int get_current_page() const;
  Glib::SignalProxy2<void, GtkNotebookPage*, guint> signal_switch_page();

private:
  friend void on_button_clicked1(GtkWidget* widget, SidebarNotebook* me);
  friend void on_button_clicked2(GtkWidget* widget, SidebarNotebook* me);
  friend void on_button_clicked3(GtkWidget* widget, SidebarNotebook* me);
  Gtk::Notebook* m_notebook;
  GtkWidget* m_sidebar;
  EggSidebarButton *button0, *button1, *button2;
};

#endif
