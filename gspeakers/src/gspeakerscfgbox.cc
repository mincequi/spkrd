/* 
 * gspeakerscfgbox.cc
 *
 * Copyright (C) 2001 Daniel Sundberg <dss@home.se>
 *
 * http://sumpan.campus.luth.se/software/gspeakers
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include <gtk--/label.h>
#include "gspeakerscfgbox.h"

GSpeakersCFGBox::GSpeakersCFGBox( GSpeakersCFG *icfg ) : Gnome::PropertyBox() {
  Gtk::Label *l;
  cfg = icfg;

  l = manage( new Gtk::Label( "Font: " ) );
  font_entry = manage( new Gtk::Entry() );
  font_entry->set_text( cfg->get_font() );
  font_entry->changed.connect( slot( this, &GSpeakersCFGBox::changed ) );

  select_font_button = manage( new Gtk::Button( "Browse..." ) );
  select_font_button->clicked.connect( slot( this, &GSpeakersCFGBox::browse_font ) );

  table = manage( new Gtk::Table( 1, 3 ) );
  table->attach( *l, 0, 1, 0, 1 );
  table->attach( *font_entry, 1, 2, 0, 1 );
  table->attach( *select_font_button, 2, 3, 0, 1 );
  
  l = manage( new Gtk::Label( "Plot" ) );
  append_page( *table, *l );
  table->show_all();
}

void GSpeakersCFGBox::apply_impl( gint page_num ) {
  oldfont = font_entry->get_text();
  cfg->set_font( font_entry->get_text() );
}

void GSpeakersCFGBox::close_impl( gint page_num ) {
  font_entry->set_text( oldfont );
}

void GSpeakersCFGBox::browse_font() {
  font_dialog = manage( new Gtk::FontSelectionDialog( "Select font..." ) );
  font_dialog->set_font_name( font_entry->get_text() );
  font_dialog->get_ok_button()->clicked.connect( slot( this, &GSpeakersCFGBox::browse_font_ok ) );
  font_dialog->get_cancel_button()->clicked.connect( slot( this, &GSpeakersCFGBox::browse_font_cancel ) );

  font_dialog->show();
}

void GSpeakersCFGBox::browse_font_ok() {
  oldfont = font_entry->get_text();
  font_entry->set_text( font_dialog->get_font_name() );
  font_dialog->destroy();
}
void GSpeakersCFGBox::browse_font_cancel() {
  font_dialog->destroy();
}
