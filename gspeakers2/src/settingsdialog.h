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

#ifndef __GSPEAKERS_SETTINGSDLG_H
#define __GSPEAKERS_SETTINGSDLG_H

#include <gtkmm.h>
#include "common.h"

using namespace std;
using namespace SigC;

class SettingsDialog : public Gtk::Dialog
{
public:
  SettingsDialog();
  virtual ~SettingsDialog();
private:
  Gtk::Notebook m_main_notebook;
  Gtk::Button m_spice_browse_button;
  Gtk::Entry *m_spice_path_entry;
  Gtk::CheckButton m_autoupdate_filter_plots, m_use_advanced_speaker_model;
  Gtk::VBox m_vbox;
  Gtk::HBox m_hbox;
  
  Gtk::FileSelection *m_file_selection;
  
  Gtk::Button *close_button, *apply_button;

  string m_filename;
  
  void on_close();
  void on_apply();
  void on_spice_browse();
  void on_file_ok(Gtk::FileSelection *f);
};

#endif
