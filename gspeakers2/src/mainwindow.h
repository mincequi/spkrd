/*
  $Id$

  mainwindow Copyright (C) 2002 Daniel Sundberg

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

#ifndef __GSPEAKERS_MAINWINDOW
#define __GSPEAKERS_MAINWINDOW

#include <gtkmm/window.h>
#include <gtkmm/menu.h>
#include "common.h"
#include "settings.h"
#include "enclosurepaned.h"
#include "speakereditor.h"
#include "crossoverpaned.h"

class MainWindow : public Gtk::Window
{
public:
  MainWindow();

private:
  Gtk::VBox m_main_vbox;
  Gtk::Notebook m_main_notebook;
  Gtk::HPaned m_driver_hpaned;
  Gtk::VPaned m_driver_vpaned;

  Gtk::MenuBar m_menubar;
  Gtk::Menu m_file_menu;
  Gtk::Menu m_edit_menu;
  Gtk::Menu m_help_menu;

  EnclosurePaned enclosure_paned;
  CrossoverPaned crossover_paned;
  Speaker_ListStore speaker_editor;

  bool in_quit_phase;

  /* Callbacks */
  void on_quit();
  virtual bool on_delete_event(GdkEventAny *event);
  void on_quit_common();
  void on_about();
  void on_edit_settings();
  void on_switch_page(GtkNotebookPage* page, guint page_num);
};

#endif
