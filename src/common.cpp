/*
  $Id$

  common Copyright (C) 2002 Daniel Sundberg

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

#include "common.h"

#include <glibmm/miscutils.h>

#include <gdkmm/pixbuf.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

#include <iostream>

Settings g_settings;

namespace gspk
{
auto to_ustring(double d) -> Glib::ustring { return Glib::ustring(std::to_string(d)); }

auto to_ustring(double d, int format_len, int format_dec) -> Glib::ustring
{
    char* str1 = nullptr;
    GString* buffer1 = g_string_new(str1);
    g_string_printf(buffer1, "%%%d.%df", format_len, format_dec);
    char* str2 = nullptr;
    GString* buffer2 = g_string_new(str2);
    g_string_printf(buffer2, buffer1->str, d);
    return Glib::ustring(buffer2->str);
}

auto int_to_ustring(int d) -> Glib::ustring { return Glib::ustring(std::to_string(d)); }

auto image_widget(std::string const& filename) -> Gtk::Widget&
{
    try
    {
#if defined(TARGET_WIN32) || defined(__APPLE__)
        auto pixbuf = Gdk::Pixbuf::create_from_file(filename);
#else
        auto pixbuf = Gdk::Pixbuf::create_from_file(
            std::string(GSPEAKERS_PREFIX) + std::string("/share/pixmaps/") + filename);
#endif
        return *Gtk::manage(new Gtk::Image(pixbuf));
    }
    catch (Gdk::PixbufError const& pe)
    {
        return *Gtk::manage(new Gtk::Label{});
    }
}

auto short_filename(const Glib::ustring& filename, unsigned length) -> Glib::ustring
{
    Glib::ustring shorted_filename;
    if (filename.length() >= length)
    {
        Glib::ustring file = Glib::path_get_basename(filename);
        Glib::ustring dir = Glib::path_get_dirname(filename);

        int base_length = length - file.length() - std::round(length / 4);
        if (base_length < 2)
        {
            base_length = 2;
        }
        shorted_filename = filename.substr(0, base_length);
        int space_left = length - file.length() - shorted_filename.length() - 6;
        if (space_left <= 0)
        {
            space_left = 1;
        }
        shorted_filename += "..." + dir.substr(dir.length() - space_left, dir.length())
                            + "/" + file;
    }
    else
    {
        return filename;
    }
    return shorted_filename;
}

auto driverlist_modified() -> bool&
{
    static bool driverlist_mod = false;
    return driverlist_mod;
}

auto enclosurelist_modified() -> bool&
{
    static bool enclosurelist_mod = false;
    return enclosurelist_mod;
}

auto crossoverlist_modified() -> bool&
{
    static bool crossoverlist_mod = false;
    return crossoverlist_mod;
}

auto measurementlist_modified() -> bool&
{
    static bool meassurementlist_mod = false;
    return meassurementlist_mod;
}

} // namespace gspk
