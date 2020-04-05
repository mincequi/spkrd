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

#include "cell_renderer_popup.hpp"
#include "popupentry.h"

#include <gdkmm/general.h>
#include <gdkmm/monitor.h>

#include <gtk/gtkmain.h>

#include <algorithm>
#include <iostream>
#include <memory>

namespace
{
auto grab_on_window(const Glib::RefPtr<Gdk::Window>& window, guint32 activate_time) -> bool
{
    std::puts("grab_on_window");
    return true;
}
} // anonymous namespace

cell_renderer_popup::cell_renderer_popup()
    : Glib::ObjectBase(typeid(cell_renderer_popup)), Gtk::CellRendererText(), popup_window_(Gtk::WINDOW_POPUP)
{
    std::cout << "cell_renderer_popup::cell_renderer_popup\n";

    signal_show_popup_.connect(sigc::mem_fun(*this, &cell_renderer_popup::on_show_popup));
    signal_hide_popup_.connect(sigc::mem_fun(*this, &cell_renderer_popup::on_hide_popup));

    popup_window_.signal_button_press_event().connect(
        sigc::mem_fun(*this, &cell_renderer_popup::on_button_press_event));
    popup_window_.signal_key_press_event().connect(
        sigc::mem_fun(*this, &cell_renderer_popup::on_key_press_event));
    // popup_window_.signal_style_changed().connect(sigc::mem_fun(*this, &cell_renderer_popup::on_style_changed));
}

cell_renderer_popup::~cell_renderer_popup() = default;

auto cell_renderer_popup::get_popup_entry() -> PopupEntry* { return popup_entry_; }

auto cell_renderer_popup::get_popup_window() -> Gtk::Window* { return &popup_window_; }

void cell_renderer_popup::set_focus_widget(Gtk::Widget& focus_widget)
{
    focus_widget_ = &focus_widget;
}

auto cell_renderer_popup::get_focus_widget() -> Gtk::Widget* { return focus_widget_; }

auto cell_renderer_popup::signal_show_popup()
    -> sigc::signal5<void, const Glib::ustring&, int, int, int, int>&
{
    return signal_show_popup_;
}

auto cell_renderer_popup::signal_hide_popup() -> sigc::signal0<void>& { return signal_hide_popup_; }

void cell_renderer_popup::hide_popup() { signal_hide_popup_(); }

void cell_renderer_popup::get_size_vfunc(Gtk::Widget& widget,
                                         const Gdk::Rectangle* cell_area,
                                         int* x_offset,
                                         int* y_offset,
                                         int* width,
                                         int* height) const
{
    // BUG commented to get gtk3 port done
    // Gtk::CellRendererText::get_size_vfunc(widget, cell_area, x_offset, y_offset, width, height);

    // We cache this because it takes really long to get the width.
    if (button_width_ < 0)
    {
        button_width_ = PopupEntry::get_button_width();
    }

    if (width != nullptr)
    {
        *width += button_width_;
    }
}

auto cell_renderer_popup::start_editing_vfunc(GdkEvent*,
                                              Gtk::Widget&,
                                              const Glib::ustring& path,
                                              const Gdk::Rectangle&,
                                              const Gdk::Rectangle&,
                                              Gtk::CellRendererState) -> Gtk::CellEditable*
{
    std::cout << "cell_renderer_popup::start_editing_vfunc\n";

    if (!property_editable())
    {
        return nullptr;
    }

    std::unique_ptr<PopupEntry> popup_entry(new PopupEntry(path));

    popup_entry->signal_editing_done().connect(
        sigc::mem_fun(*this, &cell_renderer_popup::on_popup_editing_done));
    // popup_entry->signal_arrow_clicked().connect(sigc::mem_fun(*this,
    // &cell_renderer_popup::on_popup_arrow_clicked));
    popup_entry->signal_hide().connect(sigc::mem_fun(*this, &cell_renderer_popup::on_popup_hide));

    std::cout << property_text() << std::endl;

    popup_entry->set_text(property_text());
    popup_entry->show();

    // Release auto_ptr<> ownership, and let gtkmm manage the widget.
    popup_entry_ = Gtk::manage(popup_entry.release());

    return popup_entry_;
}

void cell_renderer_popup::on_show_popup(const Glib::ustring& path, int x1, int y1, int x2, int y2)
{
    std::puts("cell_renderer_popup::on_show_popup");
    // I'm not sure this is ok to do, but we need to show the window to be
    // able to get the allocation right.
    popup_window_.move(-500, -500);
    popup_window_.show();

    const Gtk::Allocation alloc = popup_window_.get_allocation();

    auto x = x2;
    auto y = y2;

    const int button_height = y2 - y1;

    auto const display = Gdk::Display::get_default();
    auto const monitor = display->get_primary_monitor();

    Gdk::Rectangle screen_geometry;
    monitor->get_geometry(screen_geometry);

    auto screen_height = screen_geometry.get_height() - y;
    auto const screen_width = screen_geometry.get_width();

    // Check if it fits in the available height.
    if (alloc.get_height() > screen_height)
    {
        // It doesn't fit, so we see if we have the minimum space needed.
        if ((alloc.get_height() > screen_height) && (y - button_height > screen_height))
        {
            // We don't, so we show the popup above the cell instead of below it.
            screen_height = y - button_height;
            y -= (alloc.get_height() + button_height);
            y = std::max(0, y);
        }
    }

    // We try to line it up with the right edge of the column, but we don't
    // want it to go off the edges of the screen.
    x = std::min(x, screen_width);

    x -= alloc.get_width();
    x = std::max(0, x);

    gtk_grab_add(popup_window_.Gtk::Widget::gobj());

    popup_window_.move(x, y);
    popup_window_.show();

    shown_ = true;

    if (focus_widget_ != nullptr) focus_widget_->grab_focus();

    grab_on_window(popup_window_.get_window(), gtk_get_current_event_time());
}

void cell_renderer_popup::on_hide_popup()
{
    std::puts("cell_renderer_popup::on_hide_popup");
    gtk_grab_remove(popup_window_.Gtk::Widget::gobj());
    popup_window_.hide();

    if (popup_entry_ != nullptr) popup_entry_->editing_done();

    // This may look weird (the test), but the weak pointer will actually
    // be nulled out for some cells, like the date cell.
    if (popup_entry_ != nullptr) popup_entry_->remove_widget();

    shown_ = false;
    editing_canceled_ = false;
}

auto cell_renderer_popup::on_button_press_event(GdkEventButton* event) -> bool
{
    std::puts("cell_renderer_popup::on_button_press");

    if (event->button != 1)
    {
        return false;
    }

    // If the event happened outside the popup, cancel editing.

    // gdk_event_get_root_coords ((GdkEvent *) event, &x, &y);
    const double x = event->x_root;
    const double y = event->y_root;

    int xoffset = 0, yoffset = 0;
    popup_window_.get_window()->get_root_origin(xoffset, yoffset);

    const Gtk::Allocation alloc = popup_window_.get_allocation();

    xoffset += alloc.get_x();
    yoffset += alloc.get_y();

    const int x1 = alloc.get_x() + xoffset;
    const int y1 = alloc.get_y() + yoffset;
    const int x2 = x1 + alloc.get_width();
    const int y2 = y1 + alloc.get_height();

    if (x > x1 && x < x2 && y > y1 && y < y2) return false;

    editing_canceled_ = true;
    signal_hide_popup_();

    return false;
}

auto cell_renderer_popup::on_key_press_event(GdkEventKey* event) -> bool
{
    std::puts("cell_renderer_popup::on_key_press");

    switch (event->keyval)
    {
        case GDK_KEY_Escape:
            editing_canceled_ = true;
            break;
        case GDK_KEY_Return:
        case GDK_KEY_KP_Enter:
        case GDK_KEY_ISO_Enter:
        case GDK_KEY_3270_Enter:
            editing_canceled_ = false;
            break;
        default:
            return true;
    }

    signal_hide_popup_();

    return true;
}

void cell_renderer_popup::on_style_changed(Glib::RefPtr<Gtk::Style> const&)
{
    // Invalidate the cache.
    button_width_ = -1;
}

void cell_renderer_popup::on_popup_editing_done()
{
    std::puts("cell_renderer_popup::on_popup_editing_done");

    if (editing_canceled_ || popup_entry_->get_editing_canceled())
    {
        return;
    }

    edited(popup_entry_->get_path(), popup_entry_->get_text());
}

void cell_renderer_popup::on_popup_arrow_clicked()
{
    std::puts("cell_renderer_popup::on_popup_arrow_clicked");

    if (shown_)
    {
        editing_canceled_ = true;
        signal_hide_popup_();
        return;
    }

    if (!grab_on_window(popup_entry_->get_window(), gtk_get_current_event_time()))
    {
        return;
    }

    popup_entry_->select_region(0, 0);

    int x = 0, y = 0;
    popup_entry_->get_window()->get_origin(x, y);

    Gtk::Allocation const alloc = popup_entry_->get_allocation();

    signal_show_popup_(popup_entry_->get_path(), x, y, x + alloc.get_width(), y + alloc.get_height());
}

void cell_renderer_popup::on_popup_hide()
{
    std::puts("cell_renderer_popup::on_popup_hide");
    popup_entry_ = nullptr;
}
