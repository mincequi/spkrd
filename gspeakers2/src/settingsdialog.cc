/*
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

SettingsDialog::SettingsDialog() : Gtk::Dialog("GSpeakers settings...", true, true),
  m_main_notebook(),
  m_spice_browse_button("Browse..."),
  //m_spice_path_entry(),
  m_autoupdate_filter_plots("Automaticly update crossover plots"),
  m_use_advanced_speaker_model("Use advanced speaker SPICE model"),
  m_vbox(),
  m_hbox()

{
  m_file_selection = NULL;
  apply_button = manage(new Gtk::Button(Gtk::Stock::APPLY));
  close_button = manage(new Gtk::Button(Gtk::Stock::CLOSE));
  get_action_area()->pack_start(*apply_button);
  get_action_area()->pack_start(*close_button);
  
  apply_button->signal_clicked().connect(slot(*this, &SettingsDialog::on_apply));
  close_button->signal_clicked().connect(slot(*this, &SettingsDialog::on_close));
  
  get_vbox()->pack_start(m_main_notebook);
  
  /* Only one page at the moment */
    
  m_hbox.pack_start(*manage(new Gtk::Label("Full path to SPICE executable: ")));
  m_spice_path_entry = manage(new Gtk::Entry());
  m_hbox.pack_start(*m_spice_path_entry);
  m_hbox.pack_start(m_spice_browse_button);
  
  m_vbox.pack_start(m_hbox); 
  m_vbox.pack_start(m_autoupdate_filter_plots);  
  m_vbox.pack_start(m_use_advanced_speaker_model);
  m_spice_browse_button.signal_clicked().connect(slot(*this, &SettingsDialog::on_spice_browse));
  m_main_notebook.append_page(m_vbox, "General");
  show_all();
  
  g_settings.defaultValueString("SPICECmdLine", "spice3");
    
  m_spice_path_entry->set_text(g_settings.getValueString("SPICECmdLine"));
  m_autoupdate_filter_plots.set_active(g_settings.getValueBool("AutoUpdateFilterPlots"));
  m_use_advanced_speaker_model.set_active(g_settings.getValueBool("UseAdvancedSpeakerModel"));
}

SettingsDialog::~SettingsDialog() 
{
#ifdef OUTPUT_DEBUG
  cout << "SettingsDialog::~SettingsDialog" << endl;
#endif

//  if (m_file_selection != NULL) {
//    delete m_file_selection;
//  }
}

void SettingsDialog::on_apply()
{
#ifdef OUTPUT_DEBUG
  cout << "SettingsDialog::on_apply" << endl;
#endif
  g_settings.setValue("SPICECmdLine", m_spice_path_entry->get_text());
  g_settings.setValue("AutoUpdateFilterPlots", m_autoupdate_filter_plots.get_active());
  g_settings.setValue("UseAdvancedSpeakerModel", m_use_advanced_speaker_model.get_active());
  g_settings.save();
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
  m_file_selection = new Gtk::FileSelection("Select SPICE executable...");
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
  m_spice_path_entry->set_text(m_filename);
}
