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

#include "mainwindow.h"

MainWindow::MainWindow() :
  m_main_vbox(),
  m_main_paned(),
  m_cpanel_scrolled_window(), 
  m_cpanel_notebook(),
  m_plot_notebook(),
  m_boxpanel_scrolled_window(),
  m_filterpanel_scrolled_window(),
  m_boxpanel_vbox(),
  m_boxplot_paned(),
  
  box_editor(),
  box_history(),
  plot_history(),  
  speaker_list_selector(),
  box_plot()
  
{

  Gtk::Button *b3 = manage(new Gtk::Button("hej"));
  
  add(m_main_vbox);
  set_default_size(640, 480);
  
  m_main_vbox.pack_start(m_main_paned);
  
  
  m_boxplot_paned.add1(box_plot);
  m_boxplot_paned.add2(plot_history);
  m_plot_notebook.append_page(m_boxplot_paned, "BoxPlot");
  m_boxplot_paned.set_position(400);

  m_boxpanel_vbox.pack_start(speaker_list_selector);
  m_boxpanel_vbox.pack_start(box_editor);
  m_boxpanel_vbox.pack_start(box_history);
    
  m_cpanel_notebook.append_page(m_boxpanel_vbox, "Enclosure");
  m_cpanel_notebook.append_page(*b3, "Crossover");
  
  m_cpanel_scrolled_window.add(m_cpanel_notebook);
  m_cpanel_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    
  m_main_paned.add1(m_cpanel_scrolled_window);
  m_main_paned.add2(m_plot_notebook);
  m_main_paned.set_position(350);

  show_all_children();
}

MainWindow::~MainWindow()
{

}
