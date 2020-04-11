/*
  $Id$

  frequency_response_editor Copyright (C) 2002 Daniel Sundberg

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

#include "common.h"

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/table.h>

#include <vector>

class frequency_response_editor : public Gtk::Dialog
{
public:
    frequency_response_editor(std::string filename = "");

    ~frequency_response_editor() override = default;

    auto get_filename() const -> std::string const& { return m_filename; }

private:
    void on_save();

    void on_save_as();

    void on_close();

    auto get_x_vector() -> std::vector<double>;

private:
    Gtk::Table m_table;
    Gtk::Label m_label;

    std::vector<Gtk::Entry*> dbmag_entries;

    Gtk::Button m_save_button;
    Gtk::Button m_saveas_button;
    Gtk::Button m_close_button;

    std::string m_filename;
};
