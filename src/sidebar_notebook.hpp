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

#ifndef __SIDEBAR_NOTEBOOK_H
#define __SIDEBAR_NOTEBOOK_H

#include <gtkmm/box.h>
#include <gtkmm/notebook.h>

class sidebar_notebook : public Gtk::Box
{
public:
    sidebar_notebook();

    auto append_page(Gtk::Widget& child, Gtk::Widget& tab_label) -> int;

    void set_current_page(int page_num);

    auto get_current_page() const -> int;

    auto data() -> Gtk::Notebook& { return m_notebook; }

private:
    Gtk::Notebook m_notebook;
};

#endif
