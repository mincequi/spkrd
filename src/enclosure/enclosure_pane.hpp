/*
  $Id$

  enclosurepaned Copyright (C) 2002 Daniel Sundberg

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

#include "enclosure_editor.hpp"

#include "enclosure_history.hpp"
#include "common.h"
#include "box_plot.hpp"
#include "plot_history.hpp"

#include <gtkmm/handlebox.h>
#include <gtkmm/menu.h>
#include <gtkmm/paned.h>
#include <gtkmm/toolbar.h>

class enclosure_pane : public Gtk::HPaned
{
public:
    enclosure_pane();

    ~enclosure_pane() override;

    auto get_menu() -> Gtk::MenuItem&;

    auto get_toolbar() -> Gtk::Toolbar&;

protected:
    void on_settings_changed(const std::string&);

    void set_save_state(bool b);

    void on_plot_selected(int);

    void on_remove_boxplot(int);

    void on_add_plot(enclosure*, driver*, Gdk::Color&);

private:
    enclosure_editor m_box_editor;
    enclosure_history m_box_history;
    plot_history m_plot_history;
    box_plot m_box_plot;

    Gtk::Paned m_edit_vpaned;
    Gtk::Paned m_plot_vpaned;

    Gtk::MenuItem m_menu_item;
    Gtk::Toolbar* m_tbar{nullptr};

    int nof_plots{0};
    bool plot_selected{false};
};
