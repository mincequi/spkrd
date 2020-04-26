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
#include "popup_entry.hpp"

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
    : Glib::ObjectBase(typeid(cell_renderer_popup)), Gtk::CellRendererText(), m_popup_window(Gtk::WINDOW_POPUP)
{
    std::cout << "cell_renderer_popup::cell_renderer_popup\n";

    m_signal_show_popup.connect(sigc::mem_fun(*this, &cell_renderer_popup::on_show_popup));
    m_signal_hide_popup.connect(sigc::mem_fun(*this, &cell_renderer_popup::on_hide_popup));

    m_popup_window.signal_button_press_event().connect(
        sigc::mem_fun(*this, &cell_renderer_popup::on_button_press_event));
    m_popup_window.signal_key_press_event().connect(
        sigc::mem_fun(*this, &cell_renderer_popup::on_key_press_event));
    // m_popup_window.signal_style_changed().connect(sigc::mem_fun(*this, &cell_renderer_popup::on_style_changed));
}

cell_renderer_popup::~cell_renderer_popup() = default;

auto cell_renderer_popup::get_popup_entry() -> popup_entry* { return m_popup_entry; }

auto cell_renderer_popup::get_popup_window() -> Gtk::Window* { return &m_popup_window; }

void cell_renderer_popup::set_focus_widget(Gtk::Widget& focus_widget)
{
    m_focus_widget = &focus_widget;
}

auto cell_renderer_popup::get_focus_widget() -> Gtk::Widget* { return m_focus_widget; }

auto cell_renderer_popup::signal_show_popup()
    -> sigc::signal5<void, const Glib::ustring&, int, int, int, int>&
{
    return m_signal_show_popup;
}

auto cell_renderer_popup::signal_hide_popup() -> sigc::signal0<void>&
{
    return m_signal_hide_popup;
}

void cell_renderer_popup::hide_popup() { m_signal_hide_popup(); }

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
    if (m_button_width < 0)
    {
        m_button_width = popup_entry::get_button_width();
    }

    if (width != nullptr)
    {
        *width += m_button_width;
    }
}

auto cell_renderer_popup::start_editing_vfunc(GdkEvent*,
                                              Gtk::Widget&,
                                              const Glib::ustring& path,
                                              const Gdk::Rectangle&,
                                              const Gdk::Rectangle&,
                                              Gtk::CellRendererState) -> Gtk::CellEditable*
{
    std::puts("cell_renderer_popup::start_editing_vfunc");

    if (!property_editable())
    {
        return nullptr;
    }

    std::unique_ptr<popup_entry> popup_entry_box(new popup_entry(path));

    popup_entry_box->signal_editing_done().connect(
        sigc::mem_fun(*this, &cell_renderer_popup::on_popup_editing_done));
    // popup_entry_box->signal_arrow_clicked().connect(sigc::mem_fun(*this,
    // &cell_renderer_popup::on_popup_arrow_clicked));
    popup_entry_box->signal_hide().connect(sigc::mem_fun(*this, &cell_renderer_popup::on_popup_hide));

    std::cout << property_text() << std::endl;

    popup_entry_box->set_text(property_text());
    popup_entry_box->show();

    // Release auto_ptr<> ownership, and let gtkmm manage the widget.
    m_popup_entry = Gtk::manage(popup_entry_box.release());

    return m_popup_entry;
}

void cell_renderer_popup::on_show_popup(const Glib::ustring& path, int x1, int y1, int x2, int y2)
{
    std::puts("cell_renderer_popup::on_show_popup");
    // I'm not sure this is ok to do, but we need to show the window to be
    // able to get the allocation right.
    m_popup_window.move(-500, -500);
    m_popup_window.show();

    const Gtk::Allocation alloc = m_popup_window.get_allocation();

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

    gtk_grab_add(m_popup_window.Gtk::Widget::gobj());

    m_popup_window.move(x, y);
    m_popup_window.show();

    m_shown = true;

    if (m_focus_widget != nullptr)
    {
        m_focus_widget->grab_focus();
    }

    grab_on_window(m_popup_window.get_window(), gtk_get_current_event_time());
}

void cell_renderer_popup::on_hide_popup()
{
    std::puts("cell_renderer_popup::on_hide_popup");
    gtk_grab_remove(m_popup_window.Gtk::Widget::gobj());
    m_popup_window.hide();

    if (m_popup_entry != nullptr)
    {
        m_popup_entry->editing_done();
    }

    // This may look weird (the test), but the weak pointer will actually
    // be nulled out for some cells, like the date cell.
    if (m_popup_entry != nullptr)
    {
        m_popup_entry->remove_widget();
    }

    m_shown = m_editing_canceled = false;
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
    m_popup_window.get_window()->get_root_origin(xoffset, yoffset);

    const Gtk::Allocation alloc = m_popup_window.get_allocation();

    xoffset += alloc.get_x();
    yoffset += alloc.get_y();

    const int x1 = alloc.get_x() + xoffset;
    const int y1 = alloc.get_y() + yoffset;
    const int x2 = x1 + alloc.get_width();
    const int y2 = y1 + alloc.get_height();

    if (x > x1 && x < x2 && y > y1 && y < y2) return false;

    m_editing_canceled = true;
    m_signal_hide_popup();

    return false;
}

auto cell_renderer_popup::on_key_press_event(GdkEventKey* event) -> bool
{
    std::puts("cell_renderer_popup::on_key_press");

    switch (event->keyval)
    {
        case GDK_KEY_Escape:
            m_editing_canceled = true;
            break;
        case GDK_KEY_Return:
        case GDK_KEY_KP_Enter:
        case GDK_KEY_ISO_Enter:
        case GDK_KEY_3270_Enter:
            m_editing_canceled = false;
            break;
        default:
            return true;
    }

    m_signal_hide_popup();

    return true;
}

void cell_renderer_popup::on_style_changed(Glib::RefPtr<Gtk::Style> const&)
{
    // Invalidate the cache.
    m_button_width = -1;
}

void cell_renderer_popup::on_popup_editing_done()
{
    std::puts("cell_renderer_popup::on_popup_editing_done");

    if (m_editing_canceled || m_popup_entry->get_editing_canceled())
    {
        return;
    }

    edited(m_popup_entry->get_path(), m_popup_entry->get_text());
}

void cell_renderer_popup::on_popup_arrow_clicked()
{
    std::puts("cell_renderer_popup::on_popup_arrow_clicked");

    if (m_shown)
    {
        m_editing_canceled = true;
        m_signal_hide_popup();
        return;
    }

    if (!grab_on_window(m_popup_entry->get_window(), gtk_get_current_event_time()))
    {
        return;
    }

    m_popup_entry->select_region(0, 0);

    int x = 0, y = 0;
    m_popup_entry->get_window()->get_origin(x, y);

    Gtk::Allocation const alloc = m_popup_entry->get_allocation();

    m_signal_show_popup(m_popup_entry->get_path(), x, y, x + alloc.get_width(), y + alloc.get_height());
}

void cell_renderer_popup::on_popup_hide()
{
    std::puts("cell_renderer_popup::on_popup_hide");
    m_popup_entry = nullptr;
}
