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

sigc::signal1<void, Crossover*> signal_crossover_selected;
sigc::signal1<void, driver_list*> signal_speakerlist_loaded;
sigc::signal1<void, enclosure*> signal_box_selected;
sigc::signal1<void, enclosure*> signal_add_to_boxlist;
sigc::signal3<void, enclosure*, driver*, Gdk::Color&> signal_add_plot;
sigc::signal1<void, enclosure*> signal_box_modified;
sigc::signal2<int, std::vector<GSpeakers::Point>&, Gdk::Color&> signal_add_box_plot;
sigc::signal1<void, int> signal_remove_box_plot;
sigc::signal1<void, int> signal_hide_box_plot;
sigc::signal1<void, int> signal_select_plot;
sigc::signal0<void> signal_net_modified_by_wizard;
sigc::signal1<void, filter_network*> signal_net_modified_by_user;
sigc::signal1<void, int> signal_new_crossover;
sigc::signal0<void> signal_plot_crossover;
sigc::signal4<int, std::vector<GSpeakers::Point> const&, Gdk::Color const&, int&, Net*>
    signal_add_crossover_plot;
sigc::signal0<void> signal_save_open_files;

namespace GSpeakers
{
auto double_to_ustring(double d) -> Glib::ustring
{
    char* str = nullptr;
    GString* buffer = g_string_new(str);
    g_string_printf(buffer, "%f", d);
    return Glib::ustring(buffer->str);
}

auto double_to_ustring(double d, int format_len, int format_dec) -> Glib::ustring
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
#ifdef TARGET_WIN32
        auto pixbuf = Gdk::Pixbuf::create_from_file(filename);
#else
        auto pixbuf = Gdk::Pixbuf::create_from_file(std::string(GSPEAKERS_PREFIX)
                                                    + std::string("/share/pixmaps/") + filename);
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
        shorted_filename += "..." + dir.substr(dir.length() - space_left, dir.length()) + "/" + file;
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

} // namespace GSpeakers
