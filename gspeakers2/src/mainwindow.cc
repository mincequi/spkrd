/*
  $Id$
  
  mainwindow Copyright (C) 2002-2003 Daniel Sundberg

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
  //GSpeakers::tooltips().enable();
  in_quit_phase = false;
  crossover_paned.select_first_crossover();
  /* set program icon and title */
  try {
#ifdef TARGET_WIN32
    Glib::RefPtr<Gdk::Pixbuf> main_icon = Gdk::Pixbuf::create_from_file("gspeakers.png");
#else
    Glib::RefPtr<Gdk::Pixbuf> main_icon = Gdk::Pixbuf::create_from_file(string(GSPEAKERS_PREFIX) + "/share/pixmaps/gspeakers.png");
#endif
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
  g_settings.defaultValueBool("AutoUpdateFilterPlots", false);
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
        Gtk::Widget *im = manage(new Gtk::Image(Gtk::Stock::SAVE, Gtk::ICON_SIZE_MENU));
        menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem(_("_Save all files"),  
                                                             *im,  
                                                             mem_fun(*this, &MainWindow::on_save_all) ) );
        menulist.push_back( Gtk::Menu_Helpers::SeparatorElem() );
  	menulist.push_back( Gtk::Menu_Helpers::StockMenuElem(Gtk::Stock::QUIT, mem_fun(*this, &MainWindow::on_quit) ) );
  }
  {
  	Gtk::Menu::MenuList& menulist = m_edit_menu.items();

  	menulist.push_back( Gtk::Menu_Helpers::StockMenuElem(Gtk::Stock::PREFERENCES, mem_fun(*this, &MainWindow::on_edit_settings) ) );
  }
  {
  	Gtk::Menu::MenuList& menulist = m_help_menu.items();
  	menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem(_("About"), GSpeakers::image_widget("stock_menu_about.png"), 
                                                         mem_fun(*this, &MainWindow::on_about) ) );
  }
  m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem(_("_File"), m_file_menu) );
  m_file_menu.signal_expose_event().connect(mem_fun(*this, &MainWindow::on_edit_menu_expose_event));
  m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem(_("_Edit"), m_edit_menu) );
  m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem(_("_Drivers"), speaker_editor.get_menu() ) );
  m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem(_("E_nclosure"), enclosure_paned.get_menu() ) );
  m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem(_("_Crossover"), crossover_paned.get_menu()) );
  m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem(_("_Help"), m_help_menu) );

  /* Add the MenuBar to the window */
  m_main_vbox.pack_start(m_menubar, Gtk::PACK_SHRINK);
  
  /* add toolbars */
  speaker_editor.get_toolbar().hide();
  m_main_vbox.pack_start(speaker_editor.get_toolbar(), Gtk::PACK_SHRINK);
  enclosure_paned.get_toolbar().hide();
  m_main_vbox.pack_start(enclosure_paned.get_toolbar(), Gtk::PACK_SHRINK);
  crossover_paned.get_toolbar().hide();
  m_main_vbox.pack_start(crossover_paned.get_toolbar(), Gtk::PACK_SHRINK);
  
  /* Add main notebook */
  m_main_vbox.pack_start(m_main_notebook);
  
  /* Driver tab */
  m_main_notebook.append_page(m_driver_hpaned, *manage(new TabWidget("driver_small.png", _("Driver"))));
  m_driver_hpaned.add1(speaker_editor.get_editor_table());
  g_settings.defaultValueUnsignedInt("DriverMainPanedPosition", 400);
  g_settings.defaultValueUnsignedInt("DriverPlotPanedPosition", 250);
  m_driver_hpaned.set_position(g_settings.getValueUnsignedInt("DriverMainPanedPosition"));
  m_driver_vpaned.set_position(g_settings.getValueUnsignedInt("DriverPlotPanedPosition"));
  m_driver_hpaned.add2(m_driver_vpaned);
  m_driver_vpaned.add1(speaker_editor.get_plot());
  m_driver_vpaned.add2(speaker_editor.get_treeview_table());
  
  /* Enclosure tab */
  m_main_notebook.append_page(enclosure_paned, *manage(new TabWidget("speaker_small.png", _("Enclosure"))));
  
  /* Crossover tab */
  m_main_notebook.append_page(crossover_paned, *manage(new TabWidget("filter_small.png", _("Crossover") ) ));

  show_all_children();
  
  /* For some reason I had to put this row after show */
  m_main_notebook.signal_switch_page().connect(mem_fun(*this, &MainWindow::on_switch_page));
  m_main_notebook.set_current_page(g_settings.getValueUnsignedInt("MainNotebookPage"));
}

bool MainWindow::on_delete_event(GdkEventAny *event)
{
#ifdef OUTPUT_DEBUG
  cout << "MainWindow::on_delete_event: do you want to save unsaved documents?" << endl;  
#endif
  using namespace GSpeakers;

  /* Popup dialog and ask if user want to save changes */
  if (driverlist_modified() || enclosurelist_modified() || crossoverlist_modified() || meassurementlist_modified()) {
    cout << "MainWindow::on_quit: opening confirmation dialog" << endl;
    Gtk::Dialog *d = new Gtk::Dialog("", true);
    d->set_border_width(6);
    d->get_vbox()->set_spacing(12);
    Gtk::HBox *hbox = manage(new Gtk::HBox());
    d->get_vbox()->pack_start(*hbox);
    hbox->set_border_width(6);
    hbox->set_spacing(12);

    Gtk::Image *image = manage(new Gtk::Image(Gtk::Stock::DIALOG_WARNING, Gtk::ICON_SIZE_DIALOG));
    hbox->pack_start(*image);
    
    d->get_action_area()->set_border_width(12);
    d->get_action_area()->set_spacing(6);

    Gtk::VBox *vbox = manage(new Gtk::VBox());
    Gtk::Label *label1 = manage(new Gtk::Label("", Gtk::ALIGN_LEFT));
    label1->set_markup(Glib::ustring("<b>") + _("Save changes before closing?") + Glib::ustring("</b>\n\n"));
    vbox->pack_start(*label1);
    //Gtk::Label *label2 = manage(new Gtk::Label("\n\n"));
    //vbox->pack_start(*label2);
    Gtk::Label *label3 = manage(new Gtk::Label(_("There are unsaved files in GSpeakers. If you choose") + Glib::ustring("\n") +  
                                               _("to quit without saving all changes since last save") + Glib::ustring("\n") +  
                                               _("will be lost."), Gtk::ALIGN_LEFT));
    vbox->pack_start(*label3);
    hbox->pack_start(*vbox);

    d->add_button("Close without saving", 0);
    d->add_button(Gtk::Stock::CANCEL, 1);
    d->add_button(Gtk::Stock::SAVE, 2);
    d->show_all();
    
    int response = d->run();
    d->hide();
  
    switch (response) {
      case 0:
        break;
      case 1:
        return true;
        break;
      case 2:
        signal_save_open_files();
        break;
      default:
        break;
    }
    delete d;
  } 
  on_quit_common();
  
  return false;
}

void MainWindow::on_quit()
{
#ifdef OUTPUT_DEBUG
  cout << "MainWindow::on_quit: do you want to save unsaved documents?" << endl;  
#endif
  
  using namespace GSpeakers;

  /* Popup dialog and ask if user want to save changes */
  if (driverlist_modified() || enclosurelist_modified() || crossoverlist_modified() || meassurementlist_modified()) {
    cout << "MainWindow::on_quit: opening confirmation dialog" << endl;
    Gtk::Dialog *d = new Gtk::Dialog("", true);
    d->set_border_width(6);
    d->get_vbox()->set_spacing(12);
    Gtk::HBox *hbox = manage(new Gtk::HBox());
    d->get_vbox()->pack_start(*hbox);
    hbox->set_border_width(6);
    hbox->set_spacing(12);

    Gtk::Image *image = manage(new Gtk::Image(Gtk::Stock::DIALOG_WARNING, Gtk::ICON_SIZE_DIALOG));
    hbox->pack_start(*image);
    
    d->get_action_area()->set_border_width(12);
    d->get_action_area()->set_spacing(6);

    Gtk::VBox *vbox = manage(new Gtk::VBox());
    Gtk::Label *label1 = manage(new Gtk::Label("", Gtk::ALIGN_LEFT));
    label1->set_markup(Glib::ustring("<b>") + _("Save changes before closing?") + Glib::ustring("</b>\n\n"));
    vbox->pack_start(*label1);
    //Gtk::Label *label2 = manage(new Gtk::Label("\n\n"));
    //vbox->pack_start(*label2);
    Gtk::Label *label3 = manage(new Gtk::Label(_("There are unsaved files in GSpeakers. If you choose") + Glib::ustring("\n") +  
                                               _("to quit without saving all changes since last save") + Glib::ustring("\n") +  
                                               _("will be lost."), Gtk::ALIGN_LEFT));
    vbox->pack_start(*label3);
    hbox->pack_start(*vbox);

    d->add_button("Close without saving", 0);
    d->add_button(Gtk::Stock::CANCEL, 1);
    d->add_button(Gtk::Stock::SAVE, 2);
    d->show_all();
    
    int response = d->run();
    d->hide();
  
    switch (response) {
      case 0:
        break;
      case 1:
        return;
        break;
      case 2:
        signal_save_open_files();
        break;
      default:
        break;
    }
    delete d;
  } 
  
  on_quit_common();

  Gtk::Main::quit();
}

void MainWindow::on_quit_common()
{
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
  try {
    g_settings.save();
  } catch (std::runtime_error e) {
#ifdef OUTPUT_DEBUG
    cout << "MainWindow::on_delete_event: could not save settings" << endl;
#endif
  }
}

void MainWindow::on_save_all()
{
  signal_save_open_files();
}

bool MainWindow::on_edit_menu_expose_event(GdkEventExpose *event)
{
#ifdef OUTPUT_DEBUG
  cout << "MainWindow::on_filemenu_popup" << endl;
#endif
  /* Check whether to ungrey "save all" menuitem or not */
  using namespace GSpeakers;
  if (driverlist_modified() || enclosurelist_modified() || crossoverlist_modified() || meassurementlist_modified()) {
    m_file_menu.items()[0].set_sensitive(true);
  } else {
    m_file_menu.items()[0].set_sensitive(false);
  }
  
  return false;

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

void MainWindow::on_about()
{
#ifdef OUTPUT_DEBUG
  cout << "MainWindow::about" << endl;
#endif
  Gtk::MessageDialog m(*this, "GSpeakers-" + string(VERSION) + 
                       "\n\n(C) Daniel Sundberg <sumpan@sumpan.com>\n\nhttp://gspeakers.sf.net", false, 
                       Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
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
