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
#include <gtk--/radiobutton.h>
#include <gtk--/adjustment.h>
#include <gnome--/dialog.h>
#include "gspeakerscfgbox.h"

/*
 * Construct and initialize the properties dialog
 */
GSpeakersCFGBox::GSpeakersCFGBox( GSpeakersPlot *iplot, GSpeakersCFG *icfg ) 
  : Gnome::PropertyBox() {
  Gtk::Adjustment *adj;
  Gtk::Label *l;
  cfg = icfg;
  plot = iplot;

  /* Plot config page */
  solid_radio = manage( new Gtk::RadioButton( "Solid line" ) );
  ddash_radio = manage( new Gtk::RadioButton( "Dashed line" ) );
  dash_radio = manage( new Gtk::RadioButton( "On/Off dashed line" ) );
  ddash_radio->set_group( solid_radio->group() );
  dash_radio->set_group( solid_radio->group() );
  solid_radio->clicked.connect( slot( this, &GSpeakersCFGBox::changed ) );
  dash_radio->clicked.connect( slot( this, &GSpeakersCFGBox::changed ) );
  ddash_radio->clicked.connect( slot( this, &GSpeakersCFGBox::changed ) );

  adj = manage( new Gtk::Adjustment( (gfloat)cfg->get_line_size(), (gfloat)1, (gfloat)30 ) );
  line_size_spin = manage( new Gtk::SpinButton( *adj, (gfloat)1, 0 ) );
  line_size_spin->changed.connect( slot( this, &GSpeakersCFGBox::changed ) );
  line_size_spin->set_wrap( true );
  line_size_spin->set_snap_to_ticks( true );

  l = manage( new Gtk::Label( "Font: " ) );
  font_entry = manage( new Gtk::Entry() );
  font_entry->set_text( cfg->get_font() );
  font_entry->changed.connect( slot( this, &GSpeakersCFGBox::changed ) );

  select_font_button = manage( new Gtk::Button( "Browse..." ) );
  select_font_button->clicked.connect( slot( this, &GSpeakersCFGBox::browse_font ) );

  table = manage( new Gtk::Table( 6, 3 ) );
  table->attach( *l, 0, 1, 5, 6 );
  table->attach( *font_entry, 1, 2, 5, 6 );
  table->attach( *select_font_button, 2, 3, 5, 6 );
  table->attach( *solid_radio, 0, 2, 0, 1);
  table->attach( *ddash_radio, 0, 2, 1, 2);
  table->attach( *dash_radio, 0, 2, 3, 4);
  l = manage( new Gtk::Label( "Line size: ") );
  table->attach( *l, 0, 1, 4, 5);
  table->attach( *line_size_spin, 1, 2, 4, 5);
  l = manage( new Gtk::Label( "Plot" ) );
  append_page( *table, *l );
  table->show_all();

  /* Toolbar style page */
  text_and_icons_radio = manage( new Gtk::RadioButton( "Icons and text" ) );
  text_only_radio = manage( new Gtk::RadioButton( "Text only" ) );
  icons_only_radio = manage( new Gtk::RadioButton( "Icons only" ) );
  text_only_radio->set_group( text_and_icons_radio->group() );
  icons_only_radio->set_group( text_and_icons_radio->group() );
  text_and_icons_radio->clicked.connect( slot( this, &GSpeakersCFGBox::changed ) );
  text_only_radio->clicked.connect( slot( this, &GSpeakersCFGBox::changed ) );
  icons_only_radio->clicked.connect( slot( this, &GSpeakersCFGBox::changed ) );

  show_tooltips_check = manage( new Gtk::CheckButton( "Show tooltips" ) );
  show_tooltips_check->clicked.connect( slot( this, &GSpeakersCFGBox::changed ) );

  table = manage( new Gtk::Table( 4, 1 ) );
  table->attach( *text_and_icons_radio, 0, 1, 0, 1 );
  table->attach( *text_only_radio, 0, 1, 1, 2 );
  table->attach( *icons_only_radio, 0, 1, 2, 3 );
  table->attach( *show_tooltips_check, 0, 1, 3, 4 );

  l = manage( new Gtk::Label( "Toolbars" ) );
  append_page( *table, *l );
  table->show_all();

  realize();
  reset_radio_state();
}

void GSpeakersCFGBox::apply_impl( gint page_num ) {
  GdkLineStyle style = GDK_LINE_SOLID;
  int toolbar_style = 1;

  /* Write the config to cfg-file */
  oldfont = font_entry->get_text();
  cfg->set_font( font_entry->get_text() );
  plot->set_font( font_entry->get_text() );
  if ( solid_radio->get_active() == true ) {
    style = GDK_LINE_SOLID;
  } else if ( dash_radio->get_active() == true ) {
    style = GDK_LINE_ON_OFF_DASH;
  } else if ( ddash_radio->get_active() == true ) {
    style = GDK_LINE_DOUBLE_DASH;
  }
  cfg->set_line_style( style );
  plot->set_line_style( style );

  if ( text_and_icons_radio->get_active() == true ) {
    toolbar_style = TEXT_AND_ICONS;
  } else if ( text_only_radio->get_active() == true ) {
    toolbar_style = TEXT_ONLY;
  } else if ( icons_only_radio->get_active() == true ) {
    toolbar_style = ICONS_ONLY;
  }
  cfg->set_toolbar_style( toolbar_style );

  if ( show_tooltips_check->get_active() == true ) {
    cfg->set_show_tooltips( true );
    cfg->tooltips->enable();
  } else {
    cfg->set_show_tooltips( false );
    cfg->tooltips->disable();
  }

  cfg->set_line_size( line_size_spin->get_value_as_int() );
  plot->set_line_size( line_size_spin->get_value_as_int() );

  plot->redraw();
  if ( page_num == 1 ) {
    Gnome::Dialogs::ok("Please restart gspeakers to activate the new toolbar-settings." );
  }
}

gboolean GSpeakersCFGBox::close_impl() {
  /* Close the dialog and reset changed values */
  font_entry->set_text( oldfont );
  reset_radio_state();
  line_size_spin->set_value( (gfloat)cfg->get_line_size() );
  return FALSE;
}

void GSpeakersCFGBox::browse_font() {
  /* Setup the font-selection dialog */
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

void GSpeakersCFGBox::reset_radio_state() {
  switch ( cfg->get_line_style() ) {
  case GDK_LINE_SOLID:
    solid_radio->set_active( true );
    break;
  case GDK_LINE_ON_OFF_DASH:
    dash_radio->set_active( true );
    break;
  case GDK_LINE_DOUBLE_DASH:
    ddash_radio->set_active( true );
    break;
  }
  switch ( cfg->get_toolbar_style() ) {
  case TEXT_AND_ICONS:
    text_and_icons_radio->set_active( true );
    break;
  case TEXT_ONLY:
    text_only_radio->set_active( true );
    break;
  case ICONS_ONLY:
    icons_only_radio->set_active( true );
    break;
  }
  if ( cfg->get_show_tooltips() == true ) {
    show_tooltips_check->set_active( true );
  } else {
    show_tooltips_check->set_active( false );
  }
}
