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

#include <gtk/gtkentry.h> /* see XXX below */
#include <gtkmm.h>
#include <iostream>

using namespace std;

PopupEntry::PopupEntry(const Glib::ustring& path)
    : Glib::ObjectBase(typeid(PopupEntry)), Gtk::EventBox(), Gtk::CellEditable(), path_(path),
      entry_(0), editing_canceled_(false) {
  cout << "PopupEntry::PopupEntry" << endl;

  // Gtk::HBox *const hbox = new Gtk::HBox(false, 0);
  // add(*Gtk::manage(hbox));
  spin_button_ = manage(new Gtk::SpinButton(*(new Gtk::Adjustment(0, 0, 1000, 0.1, 1.0)), 0.0, 4));
  entry_ = spin_button_;
  entry_->set_has_frame(false);
  entry_->gobj()->is_cell_renderer = true; // XXX

  // hbox->pack_start(*spin_button_, Gtk::PACK_SHRINK);
  // hbox->pack_start(*spin_button_);
  add(*spin_button_);
  set_flags(Gtk::CAN_FOCUS);
  add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);

  show_all_children();
}

PopupEntry::~PopupEntry() {}

Glib::ustring PopupEntry::get_path() const { return path_; }

void PopupEntry::set_text(const Glib::ustring& text) {
  spin_button_->set_value(atof(text.c_str()));
}

Glib::ustring PopupEntry::get_text() const { return entry_->get_text(); }

void PopupEntry::select_region(int start_pos, int end_pos) {
  entry_->select_region(start_pos, end_pos);
}

bool PopupEntry::get_editing_canceled() const {
  cout << "PopupEntry::get_editing_canceled" << endl;
  return editing_canceled_;
}

// static
int PopupEntry::get_button_width() {
  cout << "PopupEntry::get_button_width" << endl;
  Gtk::Window window(Gtk::WINDOW_POPUP);

  Gtk::Button* const button = new Gtk::Button();
  window.add(*Gtk::manage(button));

  // Urgh.  Hackish :/
  window.move(-500, -500);
  window.show_all();

  Gtk::Requisition requisition = {
      0,
  };
  window.size_request(requisition);

  return requisition.width;
}

sigc::signal0<void>& PopupEntry::signal_arrow_clicked() { return signal_arrow_clicked_; }

bool PopupEntry::on_key_press_event(GdkEventKey* event) {
  cout << "PopupEntry::on_key_press_event" << endl;
  if (event->keyval == GDK_Escape) {
    editing_canceled_ = true;

    editing_done();
    remove_widget();

    return true;
  }

  entry_->grab_focus();

  // Hackish :/ Synthesize a key press event for the entry.

  GdkEvent tmp_event;
  memcpy(&tmp_event, event, sizeof(GdkEventKey));

  tmp_event.key.window = Glib::unwrap(entry_->get_window()); // XXX
  tmp_event.key.send_event = true;

  entry_->event(&tmp_event);

  return Gtk::EventBox::on_key_press_event(event);
}
/*
bool PopupEntry::on_button_press_event(GdkEventButton *event)
{
  cout << "PopupEntry::on_button_press_event" << endl;
  return true;
}
*/
void PopupEntry::start_editing_vfunc(GdkEvent* event) {
  cout << "PopupEntry::start_editing_vfunc" << endl;
  using sigc::mem_fun;

  entry_->select_region(0, -1);

  entry_->signal_activate().connect(mem_fun(*this, &Self::on_entry_activate));
  // entry_->signal_key_press_event().connect(mem_fun(*this, &Self::on_entry_key_press_event));
}

void PopupEntry::on_entry_activate() {
  cout << "PopupEntry::on_entry_activate" << endl;
  editing_done();
  // remove_widget(); // TODO: this line causes the widget to be removed twice -- dunno why
}

bool PopupEntry::on_entry_key_press_event(GdkEventKey* event) {
  cout << "PopupEntry::on_entry_key_press_event" << endl;
  if (event->keyval == GDK_Escape) {
    editing_canceled_ = true;

    editing_done();
    remove_widget();

    return true;
  }

  return false;
}
