/*
  $Id$

  file_chooser_dialog Copyright (C) 2004 Daniel Sundberg

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

#include <gtkmm/filechooserdialog.h>

class file_chooser_dialog
{
public:
    enum { FILE_CHOOSER_OPEN, FILE_CHOOSER_SAVE, FILE_CHOOSER_CANCEL };

public:
    file_chooser_dialog(const Glib::ustring& title,
                               Gtk::FileChooserAction action = Gtk::FILE_CHOOSER_ACTION_OPEN,
                               const std::string& default_filename = "");

    Glib::ustring const& get_filename() const { return m_filename; }

private:
    Glib::ustring m_filename;

    Gtk::FileChooserDialog m_file_chooser;
};
