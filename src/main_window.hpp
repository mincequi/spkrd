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

#pragma once

#include "common.h"
#include "crossover/crossover_pane.hpp"
#include "enclosure_pane.hpp"
#include "settings.h"
#include "sidebar_notebook.hpp"
#include "driver_editor.hpp"

#include <gtkmm/menu.h>
#include <gtkmm/window.h>
#include <gtkmm/menubar.h>

namespace spkrd
{
class main_window : public Gtk::Window
{
public:
    main_window();

private:
    void on_quit();

    auto on_delete_event(GdkEventAny* event) -> bool override;

    void on_quit_common();

    void on_about();

    void on_save_all();

    void on_edit_settings();

    void on_switch_page(Gtk::Widget* page, int page_number);

    void on_edit_menu_expose_event();

private:
    void set_title_and_icons();

    void set_defaults();

    void connect_driver_tab();

    void connect_enclosure_tab();

    void connect_crossover_tab();

    void initialise_menu_bar();

    void initialise_toolbar();

private:
    Gtk::Box m_main_vbox;
    Gtk::HPaned m_driver_hpaned;
    Gtk::VPaned m_driver_vpaned;

    Gtk::MenuBar m_menubar;
    Gtk::MenuItem m_file_menu_item;
    Gtk::MenuItem m_edit_menu_item;
    Gtk::MenuItem m_help_menu_item;

    Gtk::Menu m_file_menu;
    Gtk::Menu m_edit_menu;
    Gtk::Menu m_help_menu;

    sidebar_notebook m_main_notebook;

    enclosure_pane m_enclosure_paned;
    crossover_pane m_crossover_paned;
    driver_editor m_speaker_editor;

    bool m_in_quit_phase{false};
};
}
