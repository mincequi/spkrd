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

#pragma once

#include "settings.h"

#include <glibmm/ustring.h>

#include <gdkmm/color.h>
#include <gtkmm/widget.h>

#include <libintl.h>
#include <sigc++/sigc++.h>

#include <string>
#include <utility>
#include <vector>

#define _(string) gettext(string)
#define gettext_noop(string) string
#define N_(string) gettext_noop(string)

namespace gspk
{
class point;
}

namespace gspk
{
auto to_ustring(double d) -> Glib::ustring;
auto to_ustring(double d, int format_len, int format_dec) -> Glib::ustring;
auto int_to_ustring(int d) -> Glib::ustring;

auto image_widget(std::string const& filename) -> Gtk::Widget&;

auto short_filename(Glib::ustring const& filename, unsigned length = 30) -> Glib::ustring;

auto driverlist_modified() -> bool&;
auto enclosurelist_modified() -> bool&;
auto crossoverlist_modified() -> bool&;
auto measurementlist_modified() -> bool&;
} // namespace gspk

class enclosure;
class driver;
class Crossover;
class filter_network;
class driver_list;

extern Settings g_settings;
