/*
 * $Id$
 *
 * Copyright (C) 2004 Daniel Sundberg <dss@home.se>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "sidebar_notebook.hpp"
#include "common.h"

sidebar_notebook::sidebar_notebook() : Gtk::Box(Gtk::ORIENTATION_HORIZONTAL)
{
    pack_start(m_notebook, true, true, 0);
    m_notebook.set_show_tabs(true);
}

auto sidebar_notebook::append_page(Widget& child, Widget& tab_label) -> int
{
    return m_notebook.append_page(child, tab_label);
}

void sidebar_notebook::set_current_page(int page_num) { m_notebook.set_current_page(page_num); }

auto sidebar_notebook::get_current_page() const -> int { return m_notebook.get_current_page(); }
