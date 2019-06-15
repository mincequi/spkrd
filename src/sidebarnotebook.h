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
#include <gtkmm/buttonbox.h>
#include <gtkmm/notebook.h>
#include <gtkmm/togglebutton.h>

class SidebarNotebook : public Gtk::HBox
{
public:
    SidebarNotebook();

    int append_page(Gtk::Widget& child, Gtk::Widget& tab_label);

    int append_page(Gtk::Widget& child);

    void set_current_page(int page_num);

    int get_current_page() const;

private:
    void on_button_clicked1();
    void on_button_clicked2();
    void on_button_clicked3();

private:
    Gtk::Notebook* m_notebook;
    Gtk::VBox m_button_box;
    Gtk::ToggleButton m_driver_btn;
    Gtk::ToggleButton m_enclosure_btn;
    Gtk::ToggleButton m_crossover_btn;
};

#endif
