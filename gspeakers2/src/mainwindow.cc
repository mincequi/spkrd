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
#include "tabwidget.h"

#define NOTEBOOK_PAGE_DRIVERS   0
#define NOTEBOOK_PAGE_ENCLOSURE 1
#define NOTEBOOK_PAGE_FILTER    2

MainWindow::MainWindow()
{
  in_quit_phase = false;
  crossover_paned.select_first_crossover();
  /* set program icon and title */
  try {
    Glib::RefPtr<Gdk::Pixbuf> main_icon = Gdk::Pixbuf::create_from_file(string(GSPEAKERS_PREFIX) + "/share/pixmaps/gspeakers.png");
    set_icon(main_icon);
  } catch (Gdk::PixbufError e) {
#ifdef OUTPUT_DEBUG
    cout << e.code() << endl;
#endif
  }  catch (Glib::FileError fe) {
#ifdef OUTPUT_DEBUG
    cout << fe.code() << endl;
#endif
  }
  set_title("GSpeakers-" + string(VERSION));

  /* add a vbox to the window */
  add(m_main_vbox);

  g_settings.defaultValueBool("SetMainWindowSize", true);
  g_settings.defaultValueUnsignedInt("MainWindowWidth", 640);
  g_settings.defaultValueUnsignedInt("MainWindowHeight", 480);
  g_settings.defaultValueBool("SetMainWindowPosition", false);
  g_settings.defaultValueUnsignedInt("MainWindowPositionX", 0);
  g_settings.defaultValueUnsignedInt("MainWindowPositionY", 0);
  g_settings.defaultValueBool("UseAdvancedSpeakerModel", true);
  g_settings.defaultValueBool("AutoUpdateFilterPlots", true);
  g_settings.defaultValueUnsignedInt("ToolbarStyle", Gtk::TOOLBAR_BOTH);
  g_settings.defaultValueString("SPICECmdLine", "spice3");  
  
  /* You should be able to specify this in the settings dialog, if the window manager can set the size of the window
     it may as well do it, at least sawfish can do this */
  if (g_settings.getValueBool("SetMainWindowSize") == true) {
    resize(g_settings.getValueUnsignedInt("MainWindowWidth"), g_settings.getValueUnsignedInt("MainWindowHeight"));
  } 
  if (g_settings.getValueBool("SetMainWindowPosition") == true) {
    move(g_settings.getValueUnsignedInt("MainWindowPositionX"), g_settings.getValueUnsignedInt("MainWindowPositionY"));
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
  	Gtk::Menu::MenuList& menulist = m_help_menu.items();

  	menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("About...", GSpeakers::image_widget("stock_menu_about.png"), 
                                                         slot(*this, &MainWindow::on_about) ) );
  }
  m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem("_File", m_file_menu) );
  m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem("_Edit", m_edit_menu) );
  m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem("_Drivers", speaker_editor.get_menu() ) );
  m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem("E_nclosure", enclosure_paned.get_menu() ) );
  m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem("_Crossover", crossover_paned.get_menu()) );
  m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem("_Help", m_help_menu) );

  //Add the MenuBar to the window:
  m_main_vbox.pack_start(m_menubar, Gtk::PACK_SHRINK);
  
  /* add toolbars */
  speaker_editor.get_toolbar().hide();
  m_main_vbox.pack_start(speaker_editor.get_toolbar(), Gtk::SHRINK);
  enclosure_paned.get_toolbar().hide();
  m_main_vbox.pack_start(enclosure_paned.get_toolbar(), Gtk::SHRINK);
  crossover_paned.get_toolbar().hide();
  m_main_vbox.pack_start(crossover_paned.get_toolbar(), Gtk::SHRINK);
  
  /* Add main notebook */
  m_main_vbox.pack_start(m_main_notebook);
  
  /* Driver tab */
  m_main_notebook.append_page(m_driver_hpaned, *manage(new TabWidget("driver_small.png", "Driver")));
  m_driver_hpaned.add1(speaker_editor.get_editor_table());
  g_settings.defaultValueUnsignedInt("DriverMainPanedPosition", 400);
  g_settings.defaultValueUnsignedInt("DriverPlotPanedPosition", 250);
  m_driver_hpaned.set_position(g_settings.getValueUnsignedInt("DriverMainPanedPosition"));
  m_driver_vpaned.set_position(g_settings.getValueUnsignedInt("DriverPlotPanedPosition"));
  m_driver_hpaned.add2(m_driver_vpaned);
  m_driver_vpaned.add1(speaker_editor.get_plot());
  m_driver_vpaned.add2(speaker_editor.get_treeview_table());
  
  /* Enclosure tab */
  m_main_notebook.append_page(enclosure_paned, *manage(new TabWidget("speaker_small.png", "Enclosure")));
  
  /* Crossover tab */
  m_main_notebook.append_page(crossover_paned, *manage(new TabWidget("filter_small.png", "Crossover") ) );

  show_all_children();
  
  //signal_plot_crossover();  
  /* For some reason I had to put this row after show */
  m_main_notebook.signal_switch_page().connect(slot(*this, &MainWindow::on_switch_page));
  m_main_notebook.set_current_page(g_settings.getValueUnsignedInt("MainNotebookPage"));
}

void MainWindow::on_switch_page(GtkNotebookPage* page, guint page_num)
{
#ifdef OUTPUT_DEBUG
  cout << "MainWindow::on_switch_page" << endl;
#endif
  if (in_quit_phase == false) {
    switch (page_num) {
      case NOTEBOOK_PAGE_DRIVERS:
        if (speaker_editor.get_toolbar().is_visible() == false) {
          speaker_editor.get_toolbar().show();
        }
        if (enclosure_paned.get_toolbar().is_visible() == true) {
          enclosure_paned.get_toolbar().hide();
        }
        if (crossover_paned.get_toolbar().is_visible() == true) {
          crossover_paned.get_toolbar().hide();
        }
        break;
      case NOTEBOOK_PAGE_ENCLOSURE:
        if (speaker_editor.get_toolbar().is_visible() == true) {
          speaker_editor.get_toolbar().hide();
        }
        if (enclosure_paned.get_toolbar().is_visible() == false) {
          enclosure_paned.get_toolbar().show();
        }
        if (crossover_paned.get_toolbar().is_visible() == true) {
          crossover_paned.get_toolbar().hide();
        }
        break;
      case NOTEBOOK_PAGE_FILTER:
        if (speaker_editor.get_toolbar().is_visible() == true) {
          speaker_editor.get_toolbar().hide();
        }
        if (enclosure_paned.get_toolbar().is_visible() == true) {
          enclosure_paned.get_toolbar().hide();
        }
        if (crossover_paned.get_toolbar().is_visible() == false) {
          crossover_paned.get_toolbar().show();
        }
        break;
    }
  }
}

MainWindow::~MainWindow()
{
  on_quit();
}

void MainWindow::on_quit()
{
#ifdef OUTPUT_DEBUG
  cout << "MainWindow::quit" << endl;
#endif

  in_quit_phase = true; // used to avoid segfault when some widget gets destructed at different times...
  g_settings.setValue("DriverMainPanedPosition", m_driver_hpaned.get_position());
  g_settings.setValue("DriverPlotPanedPosition", m_driver_vpaned.get_position());

  /* Save size of window */
  int width, height;
  get_size(width, height);
  g_settings.setValue("MainWindowWidth", width);
  g_settings.setValue("MainWindowHeight", height);

  /* Save position */
  int pos_x, pos_y;
  get_position(pos_x, pos_y);
  g_settings.setValue("MainWindowPositionX", pos_x);
  g_settings.setValue("MainWindowPositionY", pos_y);
  
  /* Save current notebook page */
  g_settings.setValue("MainNotebookPage", m_main_notebook.get_current_page());

  /* finally save settings */
  g_settings.save();
}

void MainWindow::on_about()
{
#ifdef OUTPUT_DEBUG
  cout << "MainWindow::about" << endl;
#endif
  Gtk::MessageDialog m(*this, "GSpeakers-" + string(VERSION) + "\n\n(C) Daniel Sundberg <dss@home.se>\n\nhttp://gspeakers.sf.net", 
                        Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true, true);
  m.run();
}

void MainWindow::on_edit_settings()
{
#ifdef OUTPUT_DEBUG
  cout << "MainWindow::on_edit_settings" << endl;
#endif
  SettingsDialog *d = new SettingsDialog();
  d->run();
  delete d;
}
