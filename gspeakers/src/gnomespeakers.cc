/* gnomespeakers.cc
 *
 * Copyright (C) 2001 Daniel Sundberg <dss@home.se>
 *
 * http://sumpan.campus.luth.se/software/jags
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

#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <gnome--/main.h>
#include <gnome--/app.h>
#include <gtk--/statusbar.h>
#include <gtk--/button.h>
#include <gtk--/label.h>
#include <gtk--/frame.h>
#include <gtk--/drawingarea.h>
#include <gtk--/window.h>
#include "gnomespeakers.h"
#include "speakertoolbar.h"
#include "boxtoolbar.h"
#include "simtoolbar.h"
#include "boxlist.h"
#include "gspeakersplot.h"
#include "../config.h"

GnomeSpeakersApp::GnomeSpeakersApp() : Gnome::App("Gnome Speakers", "GnomeSpeakers"), 
  vbox(), sbar()
{
  /* First we have to check if this user got config-files */
  char *home = getenv("HOME");
  string cfg_path = string( home ) + string( "/.gspeakers" );
  string cfg_file_name = cfg_path + string( "/gspeakers.conf" );
  string spk_file_name = cfg_path + string( "/speakers1.xml" );
  string box_file_name = cfg_path + string( "/enclosures1.xml" );

  FILE *fp = fopen( cfg_file_name.c_str(), "r" );
  if ( fp != NULL ) {
    /* We got config, proceed */
  } else {
    /* Copy config to home-dir */
    cout << "Gotta fetch some config" << endl;
    string cmd = string("mkdir ") + cfg_path;
    cout << "Creating config dir: " << cfg_path << endl;
    system( cmd.c_str() );
    cmd = string("cp ") + string( MYAPP_PREFIX ) + string("/share/gspeakers/xml/* ") + cfg_path;
    cout << "Copying xml- and cfg-files to your config-dir...";   
    system( cmd.c_str() );
    cout << "done" << endl;
  }
  cfg = new GSpeakersCFG();
  SpeakerToolbar *stoolbar = manage( new SpeakerToolbar( spk_file_name, cfg ) );
  BoxToolbar *btoolbar = manage( new BoxToolbar( box_file_name, cfg ) );
  /* Plot-widget should not have the config: if we want to put this in a separate 
     library it shouldn't have to depend on gspeakers config. We set the widgets 
     config-parameters with set-methods...example: plot->set_line_style( DASH )
  */ 
  GSpeakersPlot *d = manage( new GSpeakersPlot( ) );
  BoxList *l = manage( new BoxList( cfg ) );
  SimToolbar *simtoolbar = manage( new SimToolbar( stoolbar, btoolbar, d, l, cfg ) );

  set_statusbar( sbar );
  set_contents( vbox );

  vbox.pack_start( *stoolbar, false, false );
  vbox.pack_start( *btoolbar, false, false );
  vbox.pack_start( *simtoolbar, false, false );
  
  paned = manage( new Gtk::HPaned() );
  Gtk::Frame *frame1 = manage( new Gtk::Frame() );
  Gtk::Frame *frame2 = manage( new Gtk::Frame() );

  l->set_usize(150, 100);
  
  frame1->add( *l );

  /* Begin the dirty drawingarea code */

  d->show();
  /* end the dirty drawing area code */

  frame2->add( *d );

  frame1->set_shadow_type( GTK_SHADOW_IN );
  frame2->set_shadow_type( GTK_SHADOW_IN );

  paned->add1( *frame1 );
  paned->add2( *frame2 );


  vbox.pack_start( *paned );

  show_all();
}

GnomeSpeakersApp::~GnomeSpeakersApp()
{

}

gint 
GnomeSpeakersApp::delete_event_impl(GdkEventAny* e)
{
  Gtk::Main::quit();
  return false; 
}
