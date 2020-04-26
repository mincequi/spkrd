/*
  $Id$

  crossoverpaned Copyright (C) 2002-2004 Daniel Sundberg

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

#include "crossover_history.hpp"
#include "crossover_image_view.hpp"
#include "crossover_tree_view.hpp"
#include "crossover_wizard.hpp"

#include "filter_plot.hpp"
#include "frequency_response_plot.hpp"
#include "total_filter_plot.hpp"

#include <gtkmm/menuitem.h>
#include <gtkmm/notebook.h>
#include <gtkmm/hvpaned.h>
#include <gtkmm/toolbar.h>

class crossover_pane : public Gtk::HPaned
{
public:
    crossover_pane();

    ~crossover_pane() override;

    auto get_menu() -> Gtk::MenuItem&;

    auto get_toolbar() -> Gtk::Toolbar&;

    void select_first_crossover();

protected:
    void on_settings_changed(std::string const& settings);

    void on_plot_crossover();

    void on_plot_notebook_switch_page(Gtk::Widget* page, guint page_num);

    void on_new_crossover_menu_action(int);

    void set_save_state(bool save_state);

private:
    Gtk::Notebook m_crossover_notebook;
    Gtk::Notebook m_plot_notebook;

    crossover_wizard m_crossover_wizard;
    crossover_tree_view m_crossover_treeview;
    crossover_history m_crossover_history;
    filter_plot m_filter_plot;
    total_filter_plot m_total_filter_plot;
    frequency_response_plot m_summed_freq_resp_plot;
    crossover_image_view m_crossover_image_view;

    Gtk::MenuItem m_menu_item;
    Gtk::Toolbar* m_toolbar{nullptr};
};
