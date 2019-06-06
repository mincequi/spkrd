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

#include "sidebarnotebook.h"
#include "common.h"
#include "gspeakersstock.h"

#include <iostream>

void on_button_clicked1(GtkWidget* widget, SidebarNotebook* me);
void on_button_clicked2(GtkWidget* widget, SidebarNotebook* me);
void on_button_clicked3(GtkWidget* widget, SidebarNotebook* me);

SidebarNotebook::SidebarNotebook() : Gtk::HBox() {
  m_notebook = Gtk::manage(new Gtk::Notebook());
  m_sidebar = egg_sidebar_new();
  GtkHBox* vbox = gobj();
  gtk_box_pack_start(GTK_BOX(vbox), m_sidebar, 0, 1, 0);

  button0 = egg_sidebar_button_new(GSPEAKERS_STOCK_DRIVER);
  egg_sidebar_button_set(button0, GSPEAKERS_STOCK_DRIVER, _("Driver"), TRUE);
  egg_sidebar_append(EGG_SIDEBAR(m_sidebar), button0);
  g_signal_connect(G_OBJECT(button0), "clicked", G_CALLBACK(on_button_clicked1), this);

  button1 = egg_sidebar_button_new(GSPEAKERS_STOCK_SPEAKER);
  egg_sidebar_button_set(button1, GSPEAKERS_STOCK_SPEAKER, _("Enclosure"), TRUE);
  egg_sidebar_append(EGG_SIDEBAR(m_sidebar), button1);
  g_signal_connect(G_OBJECT(button1), "clicked", G_CALLBACK(on_button_clicked2), this);

  button2 = egg_sidebar_button_new(GSPEAKERS_STOCK_FILTER);
  egg_sidebar_button_set(button2, GSPEAKERS_STOCK_FILTER, _("Crossover"), TRUE);
  egg_sidebar_append(EGG_SIDEBAR(m_sidebar), button2);
  g_signal_connect(G_OBJECT(button2), "clicked", G_CALLBACK(on_button_clicked3), this);

  pack_start(*m_notebook, true, true, 0);
  m_notebook->set_show_tabs(false);
}

int SidebarNotebook::append_page(Widget& child, Widget& tab_label) {
  return m_notebook->append_page(child, tab_label);
}

int SidebarNotebook::append_page(Widget& child) { return m_notebook->append_page(child); }

void SidebarNotebook::set_current_page(int page_num) {
  switch (page_num) {
  case 0:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button0), TRUE);
    break;
  case 1:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button1), TRUE);
    break;
  case 2:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button2), TRUE);
    break;
  default:
    break;
  }
  m_notebook->set_current_page(page_num);
}

int SidebarNotebook::get_current_page() const { return m_notebook->get_current_page(); }

Glib::SignalProxy2<void, GtkNotebookPage*, guint> SidebarNotebook::signal_switch_page() {
  return m_notebook->signal_switch_page();
}

void on_button_clicked1(GtkWidget* widget, SidebarNotebook* me) {
  me->m_notebook->set_current_page(0);
}

void on_button_clicked2(GtkWidget* widget, SidebarNotebook* me) {
  me->m_notebook->set_current_page(1);
}

void on_button_clicked3(GtkWidget* widget, SidebarNotebook* me) {
  me->m_notebook->set_current_page(2);
}
