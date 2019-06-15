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

#include "sidebarnotebook.h"
#include "common.h"

SidebarNotebook::SidebarNotebook()
    : Gtk::HBox(),
      m_notebook(Gtk::manage(new Gtk::Notebook())),
      m_driver_btn("Driver"),
      m_enclosure_btn("Enclosure"),
      m_crossover_btn("Crossover")
{
    m_button_box.add(m_driver_btn);
    m_button_box.add(m_enclosure_btn);
    m_button_box.add(m_crossover_btn);

    pack_start(m_button_box);
    pack_start(*m_notebook, true, true, 0);
    m_notebook->set_show_tabs(false);
}

int SidebarNotebook::append_page(Widget& child, Widget& tab_label)
{
    return m_notebook->append_page(child, tab_label);
}

int SidebarNotebook::append_page(Widget& child) { return m_notebook->append_page(child); }

void SidebarNotebook::set_current_page(int page_num)
{
    switch (page_num)
    {
        case 0:
            // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button0), TRUE);
            break;
        case 1:
            // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button1), TRUE);
            break;
        case 2:
            // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button2), TRUE);
            break;
        default:
            break;
    }
    m_notebook->set_current_page(page_num);
}

int SidebarNotebook::get_current_page() const { return m_notebook->get_current_page(); }

void SidebarNotebook::on_button_clicked1() { m_notebook->set_current_page(0); }

void SidebarNotebook::on_button_clicked2() { m_notebook->set_current_page(1); }

void SidebarNotebook::on_button_clicked3() { m_notebook->set_current_page(2); }
