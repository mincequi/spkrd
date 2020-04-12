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

#pragma once

#include "enclosure.hpp"
#include "colours.hpp"
#include "driver_list.hpp"

#include <gtkmm/box.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/grid.h>

/// This class will contain the control panel where you can adjust
/// settings for the enclosure you're working with.
class enclosure_editor : public Gtk::Frame
{
public:
    enclosure_editor();

    ~enclosure_editor() override = default;

    /// This function executes when you have clicked the optimize button.
    void on_optimize_button_clicked();

    void on_append_to_boxlist_clicked();

    /// This function executes when you have clicked the plot button
    void on_append_to_plot_clicked();

    void on_calc_port_clicked();

    void append_and_plot();

    void on_vb1_entry_activated();

protected:
    void on_box_selected(enclosure* b);

    void on_speaker_list_loaded(driver_list*);

    void on_combo_entry_changed();

    void on_box_data_changed(int i);
    /// Callback when enclosure type has been changed
    void on_enclosure_changed();

protected:
    Gtk::Grid m_grid;
    Gtk::Box m_vbox;
    Gtk::Box m_hbox;
    Gtk::ComboBoxText m_bass_speaker_combo;
    Gtk::Label m_speaker_qts_label, m_speaker_vas_label, m_speaker_fs_label;
    Gtk::Label m_efficiency_bandwidth_product_label;
    Gtk::Label m_frame_label;
    Gtk::Entry m_id_string_entry, m_vb1_entry, m_fb1_entry, m_vb2_entry, m_fb2_entry;
    Gtk::ComboBoxText m_box_type_combo;

private:
    enclosure* m_box = nullptr;
    driver_list* m_speaker_list;
    driver m_current_speaker;
    colours m_color_list;
    bool m_disable_signals{false};
    bool speaker_list_is_loaded{false};
};
