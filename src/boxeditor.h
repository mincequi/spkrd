/*
  $Id$

  boxeditor Copyright (C) 2002 Daniel Sundberg

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

#ifndef __GSPEAKERS_BOXEDITOR
#define __GSPEAKERS_BOXEDITOR

#include "box.h"
#include "gspeakerscolor.h"
#include "speakerlist.h"
#include <gtkmm/box.h>
#include <gtkmm/combo.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/menu.h>
#include <gtkmm/optionmenu.h>
#include <gtkmm/table.h>

using namespace std;
using namespace sigc;

/*
 * This class will contain the control panel where you can adjust
 * settings for the enclosure you're working with.
 */
class BoxEditor : public Gtk::Frame {
public:
  BoxEditor();
  virtual ~BoxEditor();
  void on_optimize_button_clicked();
  void on_append_to_boxlist_clicked();
  void on_append_to_plot_clicked();
  void on_calc_port_clicked();
  void append_and_plot();
  void on_vb1_entry_activated();

protected:
  /* Callbacks */
  void on_box_selected(Box* b);
  void on_speaker_list_loaded(SpeakerList*);
  void on_combo_entry_changed();
  void on_box_data_changed(int i);

  /* Member widgets */
  Gtk::Table m_table;
  Gtk::VBox m_vbox;
  Gtk::HBox m_hbox;
  Gtk::Combo m_bass_speaker_combo;
  Gtk::Label m_speaker_qts_label, m_speaker_vas_label, m_speaker_fs_label;
  Gtk::Entry m_id_string_entry, m_vb1_entry, m_fb1_entry, m_vb2_entry, m_fb2_entry;
  Gtk::OptionMenu m_box_type_optionmenu;
  Gtk::Menu m_option_menu;

private:
  Box* m_box;
  SpeakerList* m_speaker_list;
  Speaker m_current_speaker;
  bool disable_signals;
  GSpeakersColor m_color_list;
  bool speaker_list_is_loaded;
};

#endif
