/*
  $Id$

  GSpeakersFileChooserDialog Copyright (C) 2004 Daniel Sundberg

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

#include "gspeakersfilechooser.h"

#include <gtkmm/stock.h>

GSpeakersFileChooserDialog::GSpeakersFileChooserDialog(const Glib::ustring& title,
                                                       Gtk::FileChooserAction action,
                                                       const std::string& default_filename)
    : m_file_chooser(std::make_unique<Gtk::FileChooserDialog>(title, action)) {

  bool flag = false;

  m_file_chooser->add_button(Gtk::Stock::CANCEL, FILE_CHOOSER_CANCEL);
  switch (action) {
  case Gtk::FILE_CHOOSER_ACTION_OPEN:
    m_file_chooser->add_button(Gtk::Stock::OPEN, FILE_CHOOSER_OPEN);
    break;
  case Gtk::FILE_CHOOSER_ACTION_SAVE:
    m_file_chooser->add_button(Gtk::Stock::SAVE, FILE_CHOOSER_SAVE);
    break;
  default:
    break;
  }

  if (!default_filename.empty()) {
    m_file_chooser->set_filename(default_filename);
  }

  while (!flag) {
    auto r = m_file_chooser->run();
    switch (r) {
    case FILE_CHOOSER_SAVE:
    case FILE_CHOOSER_OPEN:
      m_filename = m_file_chooser->get_filename();
      m_file_chooser->hide();
      if (!m_filename.empty()) {
        flag = true;
      }
      break;
    default:
      if (action == Gtk::FILE_CHOOSER_ACTION_SAVE) {
        m_filename = "";
      }
      m_file_chooser->hide();
      flag = true;
      break;
    }
  }
}

Glib::ustring& GSpeakersFileChooserDialog::get_filename() { return m_filename; }
