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

#ifndef __CROSSOVER_PANED_H
#define __CROSSOVER_PANED_H

#include "common.h"

#include "crossover_history.hpp"
#include "crossover_image_view.hpp"
#include "crossovertreeview.h"
#include "crossover_wizard.hpp"

#include "gspeakersfilterplot.h"
#include "summedfreqrespplot.h"
#include "totalfilterplot.h"

#include <gtkmm/menuitem.h>
#include <gtkmm/notebook.h>
#include <gtkmm/hvpaned.h>
#include <gtkmm/toolbar.h>

class crossover_pane : public Gtk::HPaned
{
public:
    crossover_pane();

    ~crossover_pane() override;

    Gtk::MenuItem& get_menu();

    Gtk::Toolbar& get_toolbar();

    void select_first_crossover();

protected:
    void on_settings_changed(std::string const& settings);

    void on_plot_crossover();

    void on_new_crossover_menu_action(int);

    void set_save_state(bool b);

private:
    Gtk::Notebook m_crossover_notebook;
    Gtk::Notebook m_plot_notebook;

    crossover_wizard m_crossover_wizard;
    CrossoverTreeView m_crossover_treeview;
    crossover_history m_crossover_history;
    GSpeakersFilterPlot m_filter_plot;
    TotalFilterPlot m_total_filter_plot;
    SummedFreqRespPlot m_summed_freq_resp_plot;
    crossover_image_view m_crossover_image_view;

    Gtk::MenuItem m_menu_item;
    Gtk::Toolbar* m_toolbar{nullptr};
};

#endif
