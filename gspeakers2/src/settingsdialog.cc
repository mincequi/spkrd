/*
  $Id$

  settingsdlg Copyright (C) 2002 Daniel Sundberg

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

#include "settingsdialog.h"

#define NOF_TABLE_ROWS 8

SettingsDialog::SettingsDialog() : Gtk::Dialog(_("GSpeakers settings..."), true, true),
  m_main_notebook(),
  m_spice_browse_button(_("Browse...")),
  m_autoupdate_filter_plots(_("Automaticly update crossover plots when a parameter has changed")),
  m_draw_driver_imp_plot(_("Draw driver impedance plot")),
  m_draw_driver_freq_resp_plot(_("Draw driver frequency response plot")), 
  m_disable_filter_amp(_("Disable filter amplification")), 
  m_save_mainwindow_size(_("Save main window size")),
  m_save_mainwindow_position(_("Save main window position")),
  m_scale_crossover_image_parts(_("Scale components in crossover visual view")),
  m_toolbar_style()
{
  m_file_selection = NULL;
  apply_button = manage(new Gtk::Button(Gtk::Stock::APPLY));
  close_button = manage(new Gtk::Button(Gtk::Stock::CLOSE));
  get_action_area()->pack_start(*apply_button);
  get_action_area()->pack_start(*close_button);
  apply_button->signal_clicked().connect(slot(*this, &SettingsDialog::on_apply));
  close_button->signal_clicked().connect(slot(*this, &SettingsDialog::on_close));
 
  get_vbox()->pack_start(m_main_notebook);
  
  /* General page */
  Gtk::Table *general_table = manage(new Gtk::Table(NOF_TABLE_ROWS, 3, true));
  general_table->attach(m_save_mainwindow_size, 0, 3, 0, 1);
  general_table->attach(m_save_mainwindow_position, 0, 3, 1, 2);
  m_main_notebook.append_page(*general_table, _("General"));
  
  /* Toolbar page */
  Gtk::Menu *menu = manage(new Gtk::Menu());
  Gtk::Menu_Helpers::MenuList& menulist = menu->items();
  menulist.push_back( Gtk::Menu_Helpers::MenuElem(_("Icons only")) );
  menulist.push_back( Gtk::Menu_Helpers::MenuElem(_("Text only")) );
  menulist.push_back( Gtk::Menu_Helpers::MenuElem(_("Text and icons")) );
  menulist.push_back( Gtk::Menu_Helpers::MenuElem(_("Text and icons (horz)")) );
  m_toolbar_style.set_menu(*menu);
  Gtk::Table *tbar_table = manage(new Gtk::Table(NOF_TABLE_ROWS, 4, true));
  tbar_table->attach(*manage(new Gtk::Label(_("Toolbar style: "), Gtk::ALIGN_LEFT)), 0, 1, 0, 1);
  tbar_table->attach(m_toolbar_style, 1, 2, 0, 1);
  m_main_notebook.append_page(*tbar_table, _("Toolbars"));

  /* SPICE page */
  Gtk::Table *spice_table = manage(new Gtk::Table(NOF_TABLE_ROWS, 3, true));
  spice_table->attach(*manage(new Gtk::Label(_("Full path to SPICE executable: "), Gtk::ALIGN_LEFT)), 0, 1, 0, 1);
  //m_spice_path_entry = manage(new Gtk::Entry());
  spice_table->attach(m_spice_path_entry, 1, 2, 0, 1);
  spice_table->attach(m_spice_browse_button, 2, 3, 0, 1);
  m_spice_browse_button.signal_clicked().connect(slot(*this, &SettingsDialog::on_spice_browse));
  m_main_notebook.append_page(*spice_table, _("SPICE"));

  /* Driver page */
  Gtk::Table *driver_table = manage(new Gtk::Table(NOF_TABLE_ROWS, 1, true));
  driver_table->attach(m_draw_driver_imp_plot, 0, 3, 0, 1);
  driver_table->attach(m_draw_driver_freq_resp_plot, 0, 3, 1, 2);
  m_main_notebook.append_page(*driver_table, _("Drivers"));
  
  /* Crossover page */
  Gtk::Table *crossover_table = manage(new Gtk::Table(NOF_TABLE_ROWS, 1, true));
  crossover_table->attach(m_autoupdate_filter_plots, 0, 3, 0, 1);  
  crossover_table->attach(m_disable_filter_amp, 0, 3, 1, 2);
  crossover_table->attach(m_scale_crossover_image_parts, 0, 3, 2, 3);
  m_main_notebook.append_page(*crossover_table, _("Crossovers"));
      
  show_all();
  
  m_spice_path_entry.set_text(g_settings.getValueString("SPICECmdLine"));
  m_autoupdate_filter_plots.set_active(g_settings.getValueBool("AutoUpdateFilterPlots"));
  m_draw_driver_imp_plot.set_active(g_settings.getValueBool("DrawDriverImpPlot"));
  m_draw_driver_freq_resp_plot.set_active(g_settings.getValueBool("DrawDriverFreqRespPlot"));
  m_toolbar_style.set_history(g_settings.getValueUnsignedInt("ToolbarStyle"));
  m_save_mainwindow_size.set_active(g_settings.getValueBool("SetMainWindowSize"));
  m_save_mainwindow_position.set_active(g_settings.getValueBool("SetMainWindowPosition"));
  m_disable_filter_amp.set_active(g_settings.getValueBool("DisableFilterAmp"));
  m_scale_crossover_image_parts.set_active(g_settings.getValueBool("ScaleCrossoverImageParts"));
}

SettingsDialog::~SettingsDialog() 
{
#ifdef OUTPUT_DEBUG
  cout << "SettingsDialog::~SettingsDialog" << endl;
#endif
}

void SettingsDialog::on_apply()
{
#ifdef OUTPUT_DEBUG
  cout << "SettingsDialog::on_apply" << endl;
#endif
  g_settings.setValue("SPICECmdLine", m_spice_path_entry.get_text());
  g_settings.setValue("AutoUpdateFilterPlots", m_autoupdate_filter_plots.get_active());
  g_settings.setValue("ToolbarStyle", m_toolbar_style.get_history());
  g_settings.setValue("DrawDriverImpPlot", m_draw_driver_imp_plot.get_active());
  g_settings.setValue("DrawDriverFreqRespPlot", m_draw_driver_freq_resp_plot.get_active());
  g_settings.setValue("SetMainWindowSize", m_save_mainwindow_size.get_active());
  g_settings.setValue("SetMainWindowPosition", m_save_mainwindow_position.get_active());
  g_settings.setValue("DisableFilterAmp", m_disable_filter_amp.get_active());
  g_settings.setValue("ScaleCrossoverImageParts", m_scale_crossover_image_parts.get_active());
  
  try {
    g_settings.save();
  } catch (std::runtime_error e) {
#ifdef OUTPUT_DEBUG
    cout << "SettingsDialog::on_apply " << e.what() << endl;
#endif
  }
}

void SettingsDialog::on_close()
{
#ifdef OUTPUT_DEBUG
  cout << "SettingsDialog::on_close" << endl;
#endif
  hide();
}

void SettingsDialog::on_spice_browse()
{
#ifdef OUTPUT_DEBUG
  cout << "SettingsDialog::on_spice_browse" << endl;
#endif
  m_file_selection = new Gtk::FileSelection(_("Select SPICE executable..."));
  m_file_selection->get_ok_button()->signal_clicked().connect(
                      bind<Gtk::FileSelection *>(slot(*this, &SettingsDialog::on_file_ok), m_file_selection) );
  m_file_selection->run();
  m_file_selection->hide();
  delete m_file_selection;
  if (m_filename != "") {
    g_settings.setValue("SPICECmdLine", m_filename);
  }
  m_filename = "";
#ifdef OUTPUT_DEBUG
  cout << "SettingsDialog::on_spice_browse: " << m_filename << endl;
#endif
}

void SettingsDialog::on_file_ok(Gtk::FileSelection *f)
{
#ifdef OUTPUT_DEBUG
  cout << "SettingsDialog::on_file_ok" << endl;
#endif
  m_filename = f->get_filename();
  m_spice_path_entry.set_text(m_filename);
}
