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

#include "../config.h"
#include "mainwindow.h"
#include "crossover.h"
#include "settingsdialog.h"

MainWindow::MainWindow() :
  m_main_vbox(),
  m_main_notebook(),
  m_box_hpaned(),
  m_box_edit_vpaned(),
  m_box_plot_vpaned(),
  m_crossover_hpaned1(),
  m_crossover_hpaned2(),
  m_crossover_vpaned(),
  m_crossover_plot_notebook(),
  
  box_editor(),
  box_history(),
  plot_history(),  
  //crossover_wizard(),
  
  
  crossover_wizard(),
  
  speaker_list_selector(),
  //filter_plot(),
  box_plot(),
  crossover_treeview(),
  
  crossover_history(),
  
  
  filter_plot(),
  total_filter_plot()
  
{
  add(m_main_vbox);
  m_main_vbox.set_spacing(3);
//  signal_plot_crossover.connect(slot(*this, &MainWindow::on_on_plot_crossover));
   
  set_title("GSpeakers-" + string(VERSION));

  g_settings.defaultValueBool("SetMainWindowSize", true);
  g_settings.defaultValueUnsignedInt("MainWindowWidth", 640);
  g_settings.defaultValueUnsignedInt("MainWindowHeight", 480);
  g_settings.defaultValueBool("UseAdvancedSpeakerModel", true);
  g_settings.defaultValueBool("AutoUpdateFilterPlots", true);
  
  /* You should be able to specify this in the settings dialog, if the window manager can set the size of the window
     it may as well do it, at least sawfish can do this */
  if (g_settings.getValueBool("SetMainWindowSize") == true) {
    set_default_size(g_settings.getValueUnsignedInt("MainWindowWidth"), g_settings.getValueUnsignedInt("MainWindowHeight"));
  } 
  
  /* Setup the menu */
  {
  	Gtk::Menu::MenuList& menulist = m_file_menu.items();

  	menulist.push_back( Gtk::Menu_Helpers::StockMenuElem(Gtk::Stock::QUIT,
      slot(Gtk::Main::quit) ) );
  }
  {
  	Gtk::Menu::MenuList& menulist = m_edit_menu.items();

  	menulist.push_back( Gtk::Menu_Helpers::StockMenuElem(Gtk::Stock::PREFERENCES, slot(*this, &MainWindow::on_edit_settings) ) );
  }

  {
  	Gtk::Menu::MenuList& menulist = m_crossover_menu.items();
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("New _highpass crossover", 
                        bind<int>(slot(*this, &MainWindow::on_crossover_menu_action), CROSSOVER_TYPE_LOWPASS) ) );
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("New _subsonic crossover", 
                        bind<int>(slot(*this, &MainWindow::on_crossover_menu_action) , CROSSOVER_TYPE_SUBSONIC) ) );
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("New _highpass crossover", 
                        bind<int>(slot(*this, &MainWindow::on_crossover_menu_action), CROSSOVER_TYPE_HIGHPASS) ) );
        menulist.push_back( Gtk::Menu_Helpers::MenuElem("New _2-way crossover", 
                        bind<int>(slot(*this, &MainWindow::on_crossover_menu_action), CROSSOVER_TYPE_TWOWAY) ) );
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("New 2._5-way crossover", 
                        bind<int>(slot(*this, &MainWindow::on_crossover_menu_action), CROSSOVER_TYPE_LOWPASS | CROSSOVER_TYPE_TWOWAY) ) );
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("New _3-way crossover", 
                        bind<int>(slot(*this, &MainWindow::on_crossover_menu_action), CROSSOVER_TYPE_THREEWAY) ) );
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("New _4-way crossover", 
                        bind<int>(slot(*this, &MainWindow::on_crossover_menu_action), CROSSOVER_TYPE_FOURWAY) ) );
    menulist.push_back( Gtk::Menu_Helpers::SeparatorElem() );
    //menulist.push_back( Gtk::Menu_Helpers::MenuElem("Update crossover", slot(*this, &MainWindow::on_update_crossover)) );
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Plot crossover", slot(*this, &MainWindow::on_plot_crossover)) );
  }
  {
  	Gtk::Menu::MenuList& menulist = m_help_menu.items();

  	menulist.push_back( Gtk::Menu_Helpers::MenuElem("About...", slot(*this, &MainWindow::on_about) ) );
  }
  m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem("_File", m_file_menu) );
  m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem("_Edit", m_edit_menu) );
  m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem("_Crossover", m_crossover_menu) );
  m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem("_Help", m_help_menu) );

  //Add the MenuBar to the window:
  m_main_vbox.pack_start(m_menubar, Gtk::PACK_SHRINK);
  
  m_main_vbox.pack_start(speaker_list_selector, Gtk::PACK_SHRINK, Gtk::PACK_SHRINK);
  
  m_main_vbox.pack_start(m_main_notebook);
  
  /* Enclosur etab */
  m_main_notebook.append_page(m_box_hpaned, "Enclosure");
  
  /* Main paned for the enclosure tab */
  m_box_hpaned.add1(m_box_edit_vpaned);
  m_box_hpaned.add2(m_box_plot_vpaned);
  g_settings.defaultValueUnsignedInt("BoxMainPanedPosition", 250);
  m_box_plot_vpaned.set_position(g_settings.getValueUnsignedInt("BoxMainPanedPosition"));

  /* The left part, the editor and the history */
  m_box_edit_vpaned.add1(box_editor);
  m_box_edit_vpaned.add2(box_history);
  g_settings.defaultValueUnsignedInt("BoxEditPanedPosition", 300);
  m_box_plot_vpaned.set_position(g_settings.getValueUnsignedInt("BoxEditPanedPosition"));

  /* The right part, plot and plothistory */
  m_box_plot_vpaned.add1(box_plot);
  m_box_plot_vpaned.add2(plot_history);
  g_settings.defaultValueUnsignedInt("BoxPlotPanedPosition", 300);
  m_box_plot_vpaned.set_position(g_settings.getValueUnsignedInt("BoxPlotPanedPosition"));
  
  /* Crossover tab */
  m_main_notebook.append_page(m_crossover_hpaned1, "Crossover");
  m_crossover_hpaned1.add1(crossover_wizard);
  g_settings.defaultValueUnsignedInt("CrossoverPaned1Position", 220);
  m_crossover_hpaned1.set_position(g_settings.getValueUnsignedInt("CrossoverPaned1Position"));
  m_crossover_hpaned1.add2(m_crossover_hpaned2);
  m_crossover_hpaned2.add1(crossover_treeview);
  m_crossover_vpaned.add1(m_crossover_plot_notebook);
  m_crossover_plot_notebook.append_page(filter_plot, "Crossover freq resp");
  m_crossover_plot_notebook.append_page(total_filter_plot, "Total crossover freq resp");
  m_crossover_vpaned.add2(crossover_history);
  m_crossover_hpaned2.add2(m_crossover_vpaned);
  g_settings.defaultValueUnsignedInt("CrossoverPaned2Position", 220);
  m_crossover_hpaned2.set_position(g_settings.getValueUnsignedInt("CrossoverPaned2Position"));
  g_settings.defaultValueUnsignedInt("CrossoverPlotVPanedPosition", 220);
  m_crossover_vpaned.set_position(g_settings.getValueUnsignedInt("CrossoverPlotVPanedPosition"));

  

  /* Setup the paned widget for the box plot tab */
  //g_settings.defaultValueUnsignedInt("BoxPlotPanedPosition", 300);
  
  //g_settings.defaultValueUnsignedInt("CPanelPanedPosition", 220);
  //m_cpanel_paned.set_position(g_settings.getValueUnsignedInt("CPanelPanedPosition"));

  /* append pages to left notebook */
  //m_cpanel_notebook.append_page(m_boxpanel_vbox, "Enclosure");
  //m_cpanel_notebook.append_page(m_cpanel_paned, "Crossover");
  //m_cpanel_notebook.append_page(crossover_wizard, "Crossover wizard");
  //g_settings.defaultValueUnsignedInt("CPanelNotebookPage", 0);
    
  //m_cpanel_scrolled_window.add(m_cpanel_notebook);
  //m_cpanel_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    
  /* Add widgets to the main window */
  //m_main_paned.add1(m_cpanel_scrolled_window);
  //m_main_paned.add2(m_plot_notebook);
  //g_settings.defaultValueUnsignedInt("MainWindowPanedPosition", 350);
  //m_main_paned.set_position(g_settings.getValueUnsignedInt("MainWindowPanedPosition"));
  //signal_plot_crossover();  
  show_all_children();
  //signal_plot_crossover();  
  /* For some reason I had to put this row after show */
  m_main_notebook.set_current_page(g_settings.getValueUnsignedInt("MainNotebookPage"));
  
}

MainWindow::~MainWindow()
{
  on_quit();
}

void MainWindow::on_quit()
{
  cout << "MainWindow::quit" << endl;
  g_settings.setValue("BoxMainPanedPosition", m_box_hpaned.get_position());
  g_settings.setValue("BoxEditPanedPosition", m_box_edit_vpaned.get_position());
  g_settings.setValue("BoxPlotPanedPosition", m_box_plot_vpaned.get_position());
  g_settings.setValue("CrossoverPaned1Position", m_crossover_hpaned1.get_position());
  g_settings.setValue("CrossoverPaned2Position", m_crossover_hpaned2.get_position());
  g_settings.setValue("CrossoverPlotVPanedPosition", m_crossover_vpaned.get_position());

  int width, height;
  get_size(width, height);
  g_settings.setValue("MainWindowWidth", width);
  g_settings.setValue("MainWindowHeight", height);
  g_settings.setValue("MainNotebookPage", m_main_notebook.get_current_page());
  
  g_settings.save();
  
}

void MainWindow::on_about()
{
  cout << "MainWindow::about" << endl;
  Gtk::MessageDialog m(*this, "GSpeakers-" + string(VERSION) + "\n\n(C) Daniel Sundberg <dss@home.se>\n\nhttp://gspeakers.sf.net", 
                        Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true, true);
  m.run();
}

void MainWindow::on_crossover_menu_action(int type) 
{
  cout << "MainWindow::on_crossover_menu_action: " << type << endl;
  signal_new_crossover(type);
  /* Switch to crossover wizard page since you probably want to start there */
  //m_cpanel_notebook.set_current_page(2);
}

void MainWindow::on_plot_crossover()
{
  cout << "MainWindow::on_plot_crossover" << endl;
  filter_plot.clear();
  signal_plot_crossover();
}

void MainWindow::on_on_plot_crossover()
{
  
}


void MainWindow::on_update_crossover()
{
  cout << "MainWindow::on_update_crossover" << endl;
}

void MainWindow::on_edit_settings()
{
  cout << "MainWindow::on_edit_settings" << endl;
  SettingsDialog d;
  d.run();
}
