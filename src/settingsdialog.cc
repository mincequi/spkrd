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
#include "gspeakersfilechooser.h"

#include <gtkmm/frame.h>
#include <gtkmm/stock.h>
#include <gtkmm/table.h>

#include <iostream>

#define NOF_TABLE_ROWS 10

SettingsDialog::SettingsDialog()
    : Gtk::Dialog(_("Settings"), true, true), m_main_notebook(),
      m_spice_browse_button(_("Browse...")), m_spice_use_berkley(_("Berkley SPICE3f5")),
      m_spice_use_ngspice(_("NG-SPICE-reworked")),
      m_spice_use_gnucap(_("GNUCAP SPICE implementation")),
      m_autoupdate_filter_plots(
          _("Automatically update crossover plots when a parameter has changed")),
      m_draw_driver_imp_plot(_("Draw driver impedance plot")),
      m_draw_driver_freq_resp_plot(_("Draw driver frequency response plot")),
      m_disable_filter_amp(_("Disable filter amplification")),
      m_save_mainwindow_size(_("Save main window size")),
      m_save_mainwindow_position(_("Save main window position")),
      m_scale_crossover_image_parts(_("Scale components in crossover visual view")),
      m_use_driver_impedance(_("Use driver impedance instead of rdc when calculating crossover")),
      m_toolbar_style() {
  m_file_selection = NULL;
  close_button = manage(new Gtk::Button(Gtk::Stock::CLOSE));
  get_action_area()->pack_start(*close_button);
  close_button->signal_clicked().connect(mem_fun(*this, &SettingsDialog::on_close));

  get_vbox()->pack_start(m_main_notebook);
  m_main_notebook.set_border_width(5);

  /* General page */
  Gtk::Frame* general_frame = manage(new Gtk::Frame(""));
  general_frame->set_border_width(5);
  general_frame->set_shadow_type(Gtk::SHADOW_NONE);
  static_cast<Gtk::Label*>(general_frame->get_label_widget())
      ->set_markup("<b>" + Glib::ustring(_("General settings")) + "</b>");
  Gtk::Table* general_table = manage(new Gtk::Table(NOF_TABLE_ROWS, 3, false));
  general_table->set_border_width(12);
  general_table->set_spacings(5);
  general_table->attach(m_save_mainwindow_size, 0, 3, 0, 1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);
  general_table->attach(m_save_mainwindow_position, 0, 3, 1, 2, Gtk::EXPAND | Gtk::FILL,
                        Gtk::SHRINK);
  general_frame->add(*general_table);
  m_main_notebook.append_page(*general_frame, _("General"));

  /* Toolbar page */
  Gtk::Frame* toolbar_frame = manage(new Gtk::Frame(""));
  toolbar_frame->set_border_width(5);
  toolbar_frame->set_shadow_type(Gtk::SHADOW_NONE);
  static_cast<Gtk::Label*>(toolbar_frame->get_label_widget())
      ->set_markup("<b>" + Glib::ustring(_("Toolbar settings")) + "</b>");
  Gtk::Menu* menu = manage(new Gtk::Menu());
  Gtk::Menu_Helpers::MenuList& menulist = menu->items();
  menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Icons only")));
  menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Text only")));
  menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Text and icons")));
  menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Text and icons (horz)")));
  m_toolbar_style.set_menu(*menu);
  Gtk::Table* tbar_table = manage(new Gtk::Table(NOF_TABLE_ROWS, 4, false));
  tbar_table->set_border_width(12);
  tbar_table->set_spacings(5);
  tbar_table->attach(*manage(new Gtk::Label(_("Toolbar style: "), Gtk::ALIGN_LEFT)), 0, 1, 0, 1,
                     Gtk::SHRINK, Gtk::SHRINK);
  tbar_table->attach(m_toolbar_style, 1, 2, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
  toolbar_frame->add(*tbar_table);
  m_main_notebook.append_page(*toolbar_frame, _("Toolbars"));

  /* SPICE page */
  Gtk::Frame* spice_frame = manage(new Gtk::Frame(""));
  spice_frame->set_border_width(5);
  spice_frame->set_shadow_type(Gtk::SHADOW_NONE);
  static_cast<Gtk::Label*>(spice_frame->get_label_widget())
      ->set_markup("<b>" + Glib::ustring(_("SPICE interpreter settings")) + "</b>");
  Gtk::Table* spice_table = manage(new Gtk::Table(NOF_TABLE_ROWS, 3, false));
  spice_table->set_border_width(12);
  spice_table->set_spacings(5);
  spice_table->attach(
      *manage(new Gtk::Label(_("Full path to SPICE executable: "), Gtk::ALIGN_LEFT)), 0, 1, 0, 1,
      Gtk::SHRINK, Gtk::SHRINK);
  spice_table->attach(m_spice_path_entry, 1, 2, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
  spice_table->attach(m_spice_browse_button, 2, 3, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
  m_spice_browse_button.signal_clicked().connect(mem_fun(*this, &SettingsDialog::on_spice_browse));

  /* Radio buttons */

  Gtk::RadioButton::Group group = m_spice_use_berkley.get_group();
  m_spice_use_ngspice.set_group(group);
  m_spice_use_gnucap.set_group(group);

  spice_table->attach(m_spice_use_berkley, 0, 3, 1, 2, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);
  spice_table->attach(m_spice_use_ngspice, 0, 3, 2, 3, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);
  spice_table->attach(m_spice_use_gnucap, 0, 3, 3, 4, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);

  spice_frame->add(*spice_table);
  m_main_notebook.append_page(*spice_frame, _("SPICE"));

  /* Driver page */
  Gtk::Frame* driver_frame = manage(new Gtk::Frame(""));
  driver_frame->set_border_width(5);
  driver_frame->set_shadow_type(Gtk::SHADOW_NONE);
  static_cast<Gtk::Label*>(driver_frame->get_label_widget())
      ->set_markup("<b>" + Glib::ustring(_("Driver tab settings")) + "</b>");
  Gtk::Table* driver_table = manage(new Gtk::Table(NOF_TABLE_ROWS, 3, false));
  driver_table->set_border_width(12);
  driver_table->set_spacings(5);
  driver_table->attach(m_draw_driver_imp_plot, 0, 3, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK);
  driver_table->attach(m_draw_driver_freq_resp_plot, 0, 3, 1, 2, Gtk::FILL | Gtk::EXPAND,
                       Gtk::SHRINK);
  driver_frame->add(*driver_table);
  m_main_notebook.append_page(*driver_frame, _("Drivers"));

  /* Crossover page */
  Gtk::Frame* crossover_frame = manage(new Gtk::Frame(""));
  crossover_frame->set_border_width(5);
  crossover_frame->set_shadow_type(Gtk::SHADOW_NONE);
  static_cast<Gtk::Label*>(crossover_frame->get_label_widget())
      ->set_markup("<b>" + Glib::ustring(_("Crossover network tab settings")) + "</b>");
  Gtk::Table* crossover_table = manage(new Gtk::Table(NOF_TABLE_ROWS, 3, false));
  crossover_table->set_border_width(12);
  crossover_table->set_spacings(5);
  crossover_table->attach(m_autoupdate_filter_plots, 0, 3, 0, 1, Gtk::FILL | Gtk::EXPAND,
                          Gtk::SHRINK);
  crossover_table->attach(m_disable_filter_amp, 0, 3, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK);
  crossover_table->attach(m_scale_crossover_image_parts, 0, 3, 2, 3, Gtk::FILL | Gtk::EXPAND,
                          Gtk::SHRINK);
  crossover_table->attach(m_use_driver_impedance, 0, 3, 3, 4, Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK);
  crossover_frame->add(*crossover_table);
  m_main_notebook.append_page(*crossover_frame, _("Crossovers"));

  show_all();

  m_spice_path_entry.set_text(g_settings.getValueString("SPICECmdLine"));
  if (g_settings.getValueBool("SPICEUseNGSPICE")) {
    m_spice_use_ngspice.set_active(true);
  } else if (g_settings.getValueBool("SPICEUseGNUCAP")) {
    m_spice_use_gnucap.set_active(true);
  } else {
    m_spice_use_berkley.set_active(true);
  }
  m_autoupdate_filter_plots.set_active(g_settings.getValueBool("AutoUpdateFilterPlots"));
  m_draw_driver_imp_plot.set_active(g_settings.getValueBool("DrawDriverImpPlot"));
  m_draw_driver_freq_resp_plot.set_active(g_settings.getValueBool("DrawDriverFreqRespPlot"));
  m_toolbar_style.set_history(g_settings.getValueUnsignedInt("ToolbarStyle"));
  m_save_mainwindow_size.set_active(g_settings.getValueBool("SetMainWindowSize"));
  m_save_mainwindow_position.set_active(g_settings.getValueBool("SetMainWindowPosition"));
  m_disable_filter_amp.set_active(g_settings.getValueBool("DisableFilterAmp"));
  m_scale_crossover_image_parts.set_active(g_settings.getValueBool("ScaleCrossoverImageParts"));
  m_use_driver_impedance.set_active(g_settings.getValueBool("UseDriverImpedance"));

  /* Setup configuration option change handlers */
  m_save_mainwindow_size.signal_clicked().connect(sigc::bind<GSpeakers::Settings>(
      mem_fun(*this, &SettingsDialog::on_config_option_change), GSpeakers::SAVE_MAIN_WINDOW_SIZE));
  m_save_mainwindow_position.signal_clicked().connect(sigc::bind<GSpeakers::Settings>(
      mem_fun(*this, &SettingsDialog::on_config_option_change), GSpeakers::SAVE_MAIN_WINDOW_SIZE));

  m_autoupdate_filter_plots.signal_clicked().connect(
      sigc::bind<GSpeakers::Settings>(mem_fun(*this, &SettingsDialog::on_config_option_change),
                                      GSpeakers::AUTO_UPDATE_CROSSOVER_PLOT));
  m_draw_driver_imp_plot.signal_clicked().connect(sigc::bind<GSpeakers::Settings>(
      mem_fun(*this, &SettingsDialog::on_config_option_change), GSpeakers::DRAW_DRIVER_IMP_PLOT));
  m_draw_driver_freq_resp_plot.signal_clicked().connect(
      sigc::bind<GSpeakers::Settings>(mem_fun(*this, &SettingsDialog::on_config_option_change),
                                      GSpeakers::DRAW_DRIVER_FREQ_RESP_PLOT));
  m_disable_filter_amp.signal_clicked().connect(sigc::bind<GSpeakers::Settings>(
      mem_fun(*this, &SettingsDialog::on_config_option_change), GSpeakers::DISABLE_FILTER_AMP));
  m_scale_crossover_image_parts.signal_clicked().connect(sigc::bind<GSpeakers::Settings>(
      mem_fun(*this, &SettingsDialog::on_config_option_change), GSpeakers::SCALE_FILTER_PARTS));
  m_use_driver_impedance.signal_clicked().connect(sigc::bind<GSpeakers::Settings>(
      mem_fun(*this, &SettingsDialog::on_config_option_change), GSpeakers::USE_DRIVER_IMPEDANCE));
  m_spice_path_entry.signal_changed().connect(sigc::bind<GSpeakers::Settings>(
      mem_fun(*this, &SettingsDialog::on_config_option_change), GSpeakers::SPICE_PATH));
  m_spice_use_ngspice.signal_clicked().connect(sigc::bind<GSpeakers::Settings>(
      mem_fun(*this, &SettingsDialog::on_config_option_change), GSpeakers::SPICE_TYPE));
  m_spice_use_berkley.signal_clicked().connect(sigc::bind<GSpeakers::Settings>(
      mem_fun(*this, &SettingsDialog::on_config_option_change), GSpeakers::SPICE_TYPE));
  m_spice_use_gnucap.signal_clicked().connect(sigc::bind<GSpeakers::Settings>(
      mem_fun(*this, &SettingsDialog::on_config_option_change), GSpeakers::SPICE_TYPE));

  m_toolbar_style.signal_changed().connect(sigc::bind<GSpeakers::Settings>(
      mem_fun(*this, &SettingsDialog::on_config_option_change), GSpeakers::TOOLBAR_STYLE));
}

SettingsDialog::~SettingsDialog() {
#ifdef OUTPUT_DEBUG
  std::cout << "SettingsDialog::~SettingsDialog" << std::endl;
#endif
}

void SettingsDialog::on_config_option_change(GSpeakers::Settings setting) {
  std::cout << "SettingsDialog::on_config_option_change: " << setting << std::endl;
  switch (setting) {
  case GSpeakers::SAVE_MAIN_WINDOW_SIZE:
    g_settings.setValue("SetMainWindowSize", m_save_mainwindow_size.get_active());
    break;
  case GSpeakers::SAVE_MAIN_WINDOW_POSITION:
    g_settings.setValue("SetMainWindowPosition", m_save_mainwindow_position.get_active());
    break;
  case GSpeakers::AUTO_UPDATE_CROSSOVER_PLOT:
    g_settings.setValue("AutoUpdateFilterPlots", m_autoupdate_filter_plots.get_active());
    break;
  case GSpeakers::DRAW_DRIVER_IMP_PLOT:
    g_settings.setValue("DrawDriverImpPlot", m_draw_driver_imp_plot.get_active());
    break;
  case GSpeakers::DRAW_DRIVER_FREQ_RESP_PLOT:
    g_settings.setValue("DrawDriverFreqRespPlot", m_draw_driver_freq_resp_plot.get_active());
    break;
  case GSpeakers::DISABLE_FILTER_AMP:
    g_settings.setValue("DisableFilterAmp", m_disable_filter_amp.get_active());
    break;
  case GSpeakers::SCALE_FILTER_PARTS:
    g_settings.setValue("ScaleCrossoverImageParts", m_scale_crossover_image_parts.get_active());
    break;
  case GSpeakers::USE_DRIVER_IMPEDANCE:
    g_settings.setValue("UseDriverImpedance", m_use_driver_impedance.get_active());
  case GSpeakers::SPICE_PATH:
    g_settings.setValue("SPICECmdLine", m_spice_path_entry.get_text());
    break;
  case GSpeakers::TOOLBAR_STYLE:
    g_settings.setValue("ToolbarStyle", m_toolbar_style.get_history());
    break;
  case GSpeakers::SPICE_TYPE:
    if (m_spice_use_berkley.get_active()) {
      g_settings.setValue("SPICEUseNGSPICE", false);
      g_settings.setValue("SPICEUseGNUCAP", false);
      m_spice_path_entry.set_text("spice3");
    } else if (m_spice_use_ngspice.get_active()) {
      g_settings.setValue("SPICEUseNGSPICE", true);
      g_settings.setValue("SPICEUseGNUCAP", false);
      m_spice_path_entry.set_text("ngspice");
    } else {
      g_settings.setValue("SPICEUseNGSPICE", false);
      g_settings.setValue("SPICEUseGNUCAP", true);
      m_spice_path_entry.set_text("gnucap");
    }

    break;
  default:
    // do nothing
    break;
  }
  try {
    g_settings.save();
  } catch (std::runtime_error const& e) {
#ifdef OUTPUT_DEBUG
    std::cout << "SettingsDialog::on_config_option_change: " << e.what() << std::endl;
//#else
// Popup messagebox here ?
#endif
  }
}

void SettingsDialog::on_close() {
#ifdef OUTPUT_DEBUG
  std::cout << "SettingsDialog::on_close" << std::endl;
#endif
  hide();
}

void SettingsDialog::on_spice_browse() {
#ifdef OUTPUT_DEBUG
  std::cout << "SettingsDialog::on_spice_browse" << std::endl;
#endif
  GSpeakersFileChooserDialog* fc =
      new GSpeakersFileChooserDialog(_("Select SPICE executable"), Gtk::FILE_CHOOSER_ACTION_OPEN);
  std::string filename = fc->get_filename();
  if (filename.length() > 0) {
    g_settings.setValue("SPICECmdLine", m_filename);
    m_spice_path_entry.set_text(m_filename);
  }
#ifdef OUTPUT_DEBUG
  std::cout << "SettingsDialog::on_spice_browse: " << m_filename << std::endl;
#endif
}
