/*
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

#include <gtkmm.h>
#include "common.h"
#include "settings.h"
#include "speakerlistselector.h"
#include "gspeakersboxplot.h"
#include "boxeditor.h"
#include "plothistory.h"
#include "boxhistory.h"
#include "crossovertreeview.h"
#include "crossoverhistory.h"
#include "crossoverwizard.h"

class MainWindow : public Gtk::Window
{
public:
  MainWindow();
  virtual ~MainWindow();
  
private:
  Gtk::VBox m_main_vbox;
  Gtk::HPaned m_main_paned;
  Gtk::ScrolledWindow m_cpanel_scrolled_window;
  Gtk::Notebook m_cpanel_notebook, m_plot_notebook;
  Gtk::ScrolledWindow m_boxpanel_scrolled_window, m_filterpanel_scrolled_window;
  Gtk::VBox m_boxpanel_vbox;
  Gtk::VPaned m_boxplot_paned, m_cpanel_paned;

  Gtk::MenuBar m_menubar;
  Gtk::Menu m_file_menu;
  Gtk::Menu m_help_menu;

  /* Init BoxEditor before BoxHistory, this will make BoxHistory signal BoxEditor with selected box, which is nice (tm) */
  BoxEditor box_editor;
  BoxHistory box_history;
  /* Plot history will always be empty at startup for now */
  PlotHistory plot_history;
  SpeakerListSelector speaker_list_selector;

  GSpeakersBoxPlot box_plot;

  CrossoverTreeView crossover_treeview;
  CrossoverHistory crossover_history;  

  CrossoverWizard crossover_wizard;

  /* Callbacks */
  void on_quit();
  void on_about();
};

#endif
