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

SidebarNotebook::SidebarNotebook() : Gtk::HBox()
{
	m_notebook = manage(new Gtk::Notebook());
	m_sidebar = egg_sidebar_new();
	GtkHBox *vbox = gobj();
	gtk_box_pack_start(GTK_BOX(vbox), m_sidebar, false, true, 0);

	pack_start(*m_notebook, true, true, 0);
}

int SidebarNotebook::append_page(Widget& child, Widget& tab_label)
{
	return m_notebook->append_page(child, tab_label);
}

int SidebarNotebook::append_page(Widget& child, const Gtk::StockID& stock_id, const std::string& text)
{
	EggSidebarButton *button = egg_sidebar_button_new(text.c_str());
	egg_sidebar_button_set(button, stock_id.get_string().c_str(), text.c_str(), TRUE);
	egg_sidebar_append(EGG_SIDEBAR(m_sidebar), button);
	return m_notebook->append_page(child);
}

void SidebarNotebook::set_current_page(int page_num)
{
	m_notebook->set_current_page(page_num);
}

int SidebarNotebook::get_current_page () const
{
	return m_notebook->get_current_page();
}
	
Glib::SignalProxy2< void, GtkNotebookPage*, guint > SidebarNotebook::signal_switch_page()
{
	return m_notebook->signal_switch_page();
}
