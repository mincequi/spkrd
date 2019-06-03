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

#include <gtk/gtkentry.h>

#include <gtkmm/window.h>

#include <cstdio>
#include <cstring>
#include <utility>

PopupEntry::PopupEntry(Glib::ustring path)
    : Glib::ObjectBase(typeid(PopupEntry)), Gtk::EventBox(), Gtk::CellEditable(),
      path_(std::move(path)), entry_(nullptr), editing_canceled_(false) {

  std::puts("PopupEntry::PopupEntry");

  spin_button_ = manage(new Gtk::SpinButton(*(new Gtk::Adjustment(0, 0, 1000, 0.1, 1.0)), 0.0, 4));
  entry_ = spin_button_;
  entry_->set_has_frame(false);
  entry_->gobj()->is_cell_renderer = true;

  add(*spin_button_);
  set_flags(Gtk::CAN_FOCUS);
  add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);

  show_all_children();
}

Glib::ustring PopupEntry::get_path() const { return path_; }

void PopupEntry::set_text(const Glib::ustring& text) {
  spin_button_->set_value(atof(text.c_str()));
}

Glib::ustring PopupEntry::get_text() const { return entry_->get_text(); }

void PopupEntry::select_region(int start_pos, int end_pos) {
  entry_->select_region(start_pos, end_pos);
}

bool PopupEntry::get_editing_canceled() const {

  std::puts("PopupEntry::get_editing_canceled");

  return editing_canceled_;
}

// static
int PopupEntry::get_button_width() {
  std::puts("PopupEntry::get_button_width");

  Gtk::Window window(Gtk::WINDOW_POPUP);

  window.add(*Gtk::manage(new Gtk::Button()));

  // Urgh.  Hackish :/
  window.move(-500, -500);
  window.show_all();

  Gtk::Requisition requisition = {0};
  window.size_request(requisition);

  return requisition.width;
}

sigc::signal0<void>& PopupEntry::signal_arrow_clicked() { return signal_arrow_clicked_; }

bool PopupEntry::on_key_press_event(GdkEventKey* event) {

  std::puts("PopupEntry::on_key_press_event");
  if (event->keyval == GDK_Escape) {
    editing_canceled_ = true;

    editing_done();
    remove_widget();

    return true;
  }

  entry_->grab_focus();

  // Hackish :/ Synthesize a key press event for the entry.
  GdkEvent tmp_event;
  std::memcpy(&tmp_event, event, sizeof(GdkEventKey));

  tmp_event.key.window = Glib::unwrap(entry_->get_window());
  tmp_event.key.send_event = 1;

  entry_->event(&tmp_event);

  return Gtk::EventBox::on_key_press_event(event);
}

void PopupEntry::start_editing_vfunc(GdkEvent* event) {
  std::puts("PopupEntry::start_editing_vfunc");

  entry_->select_region(0, -1);
  entry_->signal_activate().connect(sigc::mem_fun(*this, &Self::on_entry_activate));
}

void PopupEntry::on_entry_activate() {
  std::puts("PopupEntry::on_entry_activate");
  editing_done();
}

bool PopupEntry::on_entry_key_press_event(GdkEventKey* event) {

  std::puts("PopupEntry::on_entry_key_press_event");

  if (event->keyval != GDK_Escape) {
    return false;
  }

  editing_canceled_ = true;

  editing_done();
  remove_widget();

  return true;
}
