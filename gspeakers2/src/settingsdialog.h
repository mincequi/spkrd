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

#ifndef __GSPEAKERS_SETTINGSDLG_H
#define __GSPEAKERS_SETTINGSDLG_H

#include <gtkmm/dialog.h>
#include <gtkmm/notebook.h>
#include <gtkmm/entry.h>
#include <gtkmm/optionmenu.h>
#include <gtkmm/fileselection.h>
#include "common.h"

using namespace std;
using namespace sigc;

namespace GSpeakers {
  enum Settings { 
    SAVE_MAIN_WINDOW_SIZE, 
    SAVE_MAIN_WINDOW_POSITION, 
    TOOLBAR_STYLE, 
    SPICE_PATH, 
    DRAW_DRIVER_IMP_PLOT, 
    DRAW_DRIVER_FREQ_RESP_PLOT,
    AUTO_UPDATE_CROSSOVER_PLOT,
    DISABLE_FILTER_AMP,
    SCALE_FILER_PARTS, 
    SPICE_USE_NGSPICE
  }; 
}

class SettingsDialog : public Gtk::Dialog
{
public:
  SettingsDialog();
  virtual ~SettingsDialog();
private:
  Gtk::Notebook m_main_notebook;
  Gtk::Button m_spice_browse_button;
  Gtk::Entry m_spice_path_entry;
  Gtk::CheckButton m_spice_use_ngspice;
  Gtk::CheckButton m_autoupdate_filter_plots;
  Gtk::CheckButton m_draw_driver_imp_plot, m_draw_driver_freq_resp_plot, m_disable_filter_amp;
  Gtk::CheckButton m_save_mainwindow_size, m_save_mainwindow_position;
  Gtk::CheckButton m_scale_crossover_image_parts;


  Gtk::OptionMenu m_toolbar_style;

  Gtk::FileSelection *m_file_selection;
  
  Gtk::Button *close_button;

  string m_filename;
  
  void on_close();
  void on_spice_browse();
  void on_file_ok(Gtk::FileSelection *f);
  void on_config_option_change(GSpeakers::Settings setting);
};

#endif
