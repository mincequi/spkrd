/*
  $Id$

  Copyright (C) 1998-2002 The gtkmm Development Team & Daniel Sundberg

  The file is pretty much ripped right out of the gtkmm-2.0
  example custom cellrenderer with small modifications by
  Daniel Sundberg <dss@home.se>.

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

#include "popupentry.h"

#include <gtkmm/window.h>

#include <cstdio>
#include <cstring>
#include <utility>

PopupEntry::PopupEntry(Glib::ustring path)
    : Glib::ObjectBase(typeid(PopupEntry)),
      Gtk::EventBox(),
      Gtk::CellEditable(),
      m_path(std::move(path)),
      m_entry(nullptr),
      m_spin_digits(Gtk::Adjustment::create(0, 0, 1000, 0.1, 1.0)),
      m_editing_canceled(false)
{
    std::puts("PopupEntry::PopupEntry");

    m_entry = m_spin_button = Gtk::manage(new Gtk::SpinButton(m_spin_digits, 0.0, 4));
    m_entry->set_has_frame(false);

    add(*m_spin_button);
    add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);

    show_all_children();
}

auto PopupEntry::get_path() const -> Glib::ustring const& { return m_path; }

void PopupEntry::set_text(const Glib::ustring& text)
{
    m_spin_button->set_value(atof(text.c_str()));
}

auto PopupEntry::get_text() const -> Glib::ustring { return m_entry->get_text(); }

void PopupEntry::select_region(int start_pos, int end_pos)
{
    m_entry->select_region(start_pos, end_pos);
}

auto PopupEntry::get_editing_canceled() const -> bool
{
    std::puts("PopupEntry::get_editing_canceled");

    return m_editing_canceled;
}

auto PopupEntry::get_button_width() -> int
{
    std::puts("PopupEntry::get_button_width");

    Gtk::Window window(Gtk::WINDOW_POPUP);

    window.add(*Gtk::manage(new Gtk::Button()));

    // Urgh.  Hackish :/
    window.move(-500, -500);
    window.show_all();

    int width, height;
    window.get_size(width, height);

    return width;
}

auto PopupEntry::signal_arrow_clicked() -> sigc::signal0<void>& { return m_signal_arrow_clicked; }

auto PopupEntry::on_key_press_event(GdkEventKey* event) -> bool
{
    std::puts("PopupEntry::on_key_press_event");
    if (event->keyval == GDK_KEY_Escape)
    {
        m_editing_canceled = true;

        editing_done();
        remove_widget();

        return true;
    }

    m_entry->grab_focus();

    // Hackish :/ Synthesize a key press event for the entry.
    GdkEvent tmp_event;
    std::memcpy(&tmp_event, event, sizeof(GdkEventKey));

    tmp_event.key.window = Glib::unwrap(m_entry->get_window());
    tmp_event.key.send_event = 1;

    m_entry->event(&tmp_event);

    return Gtk::EventBox::on_key_press_event(event);
}

void PopupEntry::start_editing_vfunc(GdkEvent* event)
{
    std::puts("PopupEntry::start_editing_vfunc");

    m_entry->select_region(0, -1);
    m_entry->signal_activate().connect(sigc::mem_fun(*this, &Self::on_entry_activate));
}

void PopupEntry::on_entry_activate()
{
    std::puts("PopupEntry::on_entry_activate");
    editing_done();
}

auto PopupEntry::on_entry_key_press_event(GdkEventKey* event) -> bool
{
    std::puts("PopupEntry::on_entry_key_press_event");

    if (event->keyval != GDK_KEY_Escape)
    {
        return false;
    }

    m_editing_canceled = true;

    editing_done();
    remove_widget();

    return true;
}
