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

#include "enclosurepaned.h"

#define MENU_INDEX_SAVE                 6
#define MENU_INDEX_DELETE               8
#define MENU_INDEX_DELETE_BOXPLOT       15
#define TOOLBAR_INDEX_SAVE              5
#define TOOLBAR_INDEX_DELETE            7
#define TOOLBAR_INDEX_DELETE_BOXPLOT    12

EnclosurePaned::EnclosurePaned()
{
  nof_plots = 0;
  plot_selected = false;
  m_tbar = NULL;
  g_settings.settings_changed.connect(slot(*this, &EnclosurePaned::on_settings_changed));
  
  /* Main paned for the enclosure tab */
  add1(m_edit_vpaned);
  add2(m_plot_vpaned);
  g_settings.defaultValueUnsignedInt("BoxMainPanedPosition", 250);
  set_position(g_settings.getValueUnsignedInt("BoxMainPanedPosition"));

  /* The left part, the editor and the history */
  m_edit_vpaned.add1(box_editor);
  m_edit_vpaned.add2(box_history);
  g_settings.defaultValueUnsignedInt("BoxEditPanedPosition", 300);
  m_edit_vpaned.set_position(g_settings.getValueUnsignedInt("BoxEditPanedPosition"));

  /* The right part, plot and plothistory */
  m_plot_vpaned.add1(box_plot);
  m_plot_vpaned.add2(plot_history);
  g_settings.defaultValueUnsignedInt("BoxPlotPanedPosition", 300);
  m_plot_vpaned.set_position(g_settings.getValueUnsignedInt("BoxPlotPanedPosition"));

  signal_enclosure_set_save_state.connect( slot(*this, &EnclosurePaned::set_save_state) );
  signal_remove_box_plot.connect(slot(*this, &EnclosurePaned::on_remove_boxplot));
  signal_select_plot.connect(slot(*this, &EnclosurePaned::on_plot_selected));
  signal_add_plot.connect(slot(*this, &EnclosurePaned::on_add_plot));
}

EnclosurePaned::~EnclosurePaned()
{
#ifdef OUTPUT_DEBUG
  cout << "EnclosurePaned::~EnclosurePaned" << endl;
#endif
  g_settings.setValue("BoxMainPanedPosition", get_position());
  g_settings.setValue("BoxEditPanedPosition", m_edit_vpaned.get_position());
  g_settings.setValue("BoxPlotPanedPosition", m_plot_vpaned.get_position());
  try {
    g_settings.save();
  } catch (std::runtime_error e) {
#ifdef OUTPUT_DEBUG
    cout << "Main: " << e.what() << endl;
#endif
  }
}

Gtk::Menu& EnclosurePaned::get_menu()
{
  Gtk::Menu::MenuList& menulist = m_menu.items();
  menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("_New Enclosure", GSpeakers::image_widget("stock_new_enclosure_16.png"), 
                      slot(box_history, &BoxHistory::on_new) ) );
  menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("New _Copy", GSpeakers::image_widget("stock_new_enclosure_copy_16.png"), 
                      slot(box_history, &BoxHistory::on_new_copy) ) );
  menulist.push_back( Gtk::Menu_Helpers::SeparatorElem() );
  menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("N_ew Enclosure Xml", GSpeakers::image_widget("stock_new_enclosure_xml_16.png"), 
                      slot(box_history, &BoxHistory::on_new_xml) ) ); 
  menulist.push_back( Gtk::Menu_Helpers::MenuElem("A_ppend Enclosure Xml...", 
                      slot(box_history, &BoxHistory::on_append_xml) ) );
  menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("_Open Xml...", GSpeakers::image_widget("open_xml_16.png"),   
                      slot(box_history, &BoxHistory::on_open_xml) ) );
  menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("_Save Xml", GSpeakers::image_widget("save_xml_16.png"),   
                      slot(box_history, &BoxHistory::on_save) ) );
  menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("Save Xml _As", GSpeakers::image_widget("save_as_xml_16.png"),   
                      slot(box_history, &BoxHistory::on_save_as) ) );
  menulist.push_back( Gtk::Menu_Helpers::SeparatorElem() );
  menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("_Delete Enclosure", GSpeakers::image_widget("delete_enclosure_16.png"),  
                      slot(box_history, &BoxHistory::on_remove) ) ); 
  menulist.push_back( Gtk::Menu_Helpers::SeparatorElem() );
  menulist.push_back( Gtk::Menu_Helpers::MenuElem("Append To _Xml", slot(box_editor, &BoxEditor::on_append_to_boxlist_clicked) ) ); 
  menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Plot", slot(box_editor, &BoxEditor::on_append_to_plot_clicked) ) ); 
  menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("_Optimize Enclosure", GSpeakers::image_widget("opt_enclosure_16.png"), 
                        slot(box_editor, &BoxEditor::on_optimize_button_clicked) ) ); 
  menulist.push_back( Gtk::Menu_Helpers::SeparatorElem() );
  menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("Delete Plot", GSpeakers::image_widget("delete_plot_16.png"), 
                        slot(plot_history, &PlotHistory::on_remove) ) ); 
  
  //menulist.push_back( Gtk::Menu_Helpers::MenuElem("Delete Selected Plo_t", 
  //                    slot(*this, &EnclosurePaned::on_delete_plot) ) ); 
  menulist[MENU_INDEX_SAVE].set_sensitive(false);
  menulist[MENU_INDEX_DELETE_BOXPLOT].set_sensitive(false);
  //menulist[MENU_INDEX_DELETE].set_sensitive(false);
  return m_menu;
}

Gtk::Widget& EnclosurePaned::get_toolbar()
{
  if (m_tbar == NULL) {
    m_tbar = manage(new Gtk::Toolbar());
    m_tbar->tools().push_back( Gtk::Toolbar_Helpers::ButtonElem("New Enclosure", GSpeakers::image_widget("stock_new_enclosure_24.png"), 
                             slot(box_history, &BoxHistory::on_new), "Create new enclosure") );
    m_tbar->tools().push_back( Gtk::Toolbar_Helpers::ButtonElem("New Copy", GSpeakers::image_widget("stock_new_enclosure_copy_24.png"), 
                             slot(box_history, &BoxHistory::on_new_copy), "Copy currently selected enclosure") );
    m_tbar->tools().push_back( Gtk::Toolbar_Helpers::Space() );
    m_tbar->tools().push_back( Gtk::Toolbar_Helpers::ButtonElem("New Xml", GSpeakers::image_widget("stock_new_enclosure_xml_24.png"), 
                             slot(box_history, &BoxHistory::on_new_xml), "Create new enclosure xml (list)") );
    m_tbar->tools().push_back( Gtk::Toolbar_Helpers::ButtonElem("Open Xml",  GSpeakers::image_widget("open_xml_24.png"),
                             slot(box_history, &BoxHistory::on_open_xml), "Open enclosure xml (list)") );
    m_tbar->tools().push_back( Gtk::Toolbar_Helpers::ButtonElem("Save Xml",  GSpeakers::image_widget("save_xml_24.png"),
                             slot(box_history, &BoxHistory::on_save), "Save enclosure xml (list)") );
    m_tbar->tools().push_back( Gtk::Toolbar_Helpers::Space() );
    m_tbar->tools().push_back( Gtk::Toolbar_Helpers::ButtonElem("Delete Enclosure",  GSpeakers::image_widget("delete_enclosure_24.png"),   
                             slot(box_history, &BoxHistory::on_remove), "Delete selected enclosure") );
    m_tbar->tools().push_back( Gtk::Toolbar_Helpers::Space() );
    m_tbar->tools().push_back( Gtk::Toolbar_Helpers::ButtonElem("Optimize", GSpeakers::image_widget("opt_enclosure_24.png"), 
                                 slot(box_editor, &BoxEditor::on_optimize_button_clicked), "Optimize enclosure volume") );
    m_tbar->tools().push_back( Gtk::Toolbar_Helpers::ButtonElem("Plot", GSpeakers::image_widget("append_plot_enclosure_24.png"), 
                                 slot(box_editor, &BoxEditor::append_and_plot), "Append to enclosure list (xml) and plot") );
    m_tbar->tools().push_back( Gtk::Toolbar_Helpers::Space() );
    m_tbar->tools().push_back( Gtk::Toolbar_Helpers::ButtonElem("Delete Plot", GSpeakers::image_widget("delete_plot_24.png"), 
                                 slot(plot_history, &PlotHistory::on_remove), "Delete selected plot from plotlist") );
                                 
    m_toolbar.add(*m_tbar);
    m_tbar->set_toolbar_style((Gtk::ToolbarStyle)g_settings.getValueUnsignedInt("ToolbarStyle"));
    m_tbar->tools()[TOOLBAR_INDEX_SAVE].get_widget()->set_sensitive(false);
    m_tbar->tools()[TOOLBAR_INDEX_DELETE_BOXPLOT].get_widget()->set_sensitive(false);
    //tbar->tools()[TOOLBAR_INDEX_DELETE].get_widget()->set_sensitive(false);
  }
  return m_toolbar;
}

void EnclosurePaned::on_settings_changed(const string& s)
{
  if (s == "ToolbarStyle") {
#ifndef TARGET_WIN32
    m_tbar->set_toolbar_style((Gtk::ToolbarStyle)g_settings.getValueUnsignedInt("ToolbarStyle"));
#endif
  }
}

void EnclosurePaned::set_save_state(bool b)
{
  m_tbar->tools()[TOOLBAR_INDEX_SAVE].get_widget()->set_sensitive(b);
  m_menu.items()[MENU_INDEX_SAVE].set_sensitive(b);
}

void EnclosurePaned::on_plot_selected(int)
{
#ifdef OUTPUT_DEBUG
  cout << "EnclsourePaned::on_plot_selected" << endl;
#endif
  plot_selected = true;
  m_tbar->tools()[TOOLBAR_INDEX_DELETE_BOXPLOT].get_widget()->set_sensitive(true);
  m_menu.items()[MENU_INDEX_DELETE_BOXPLOT].set_sensitive(true);
}

void EnclosurePaned::on_remove_boxplot(int)
{
#ifdef OUTPUT_DEBUG
  cout << "EnclsourePaned::on_remove_boxplot: nof_plots = " << nof_plots << endl;
#endif
  if (nof_plots > 0) {
    nof_plots--;
  }
  if (nof_plots < 1) {
    m_tbar->tools()[TOOLBAR_INDEX_DELETE_BOXPLOT].get_widget()->set_sensitive(false);
    m_menu.items()[MENU_INDEX_DELETE_BOXPLOT].set_sensitive(false);
  }
}

void EnclosurePaned::on_add_plot(Box *b, Speaker *s, Gdk::Color& c)
{
#ifdef OUTPUT_DEBUG
  cout << "EnclsourePaned::on_add_plot" << endl;
#endif
  nof_plots++;
}
