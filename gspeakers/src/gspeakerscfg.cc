/* 
 * gspeakerscfg.cc
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

#include <string>
#include <stdio.h>
#include "gspeakerscfg.h"

string cfg_file = string( getenv("HOME") ) + string("/.gspeakers/gspeakers.conf");

GSpeakersCFG::GSpeakersCFG() : CConfig( (char *)cfg_file.c_str() ) {
  tooltips = new Gtk::Tooltips();
};

GSpeakersCFG::~GSpeakersCFG() {
  delete tooltips;
}

string GSpeakersCFG::get_font() {
  string s = string( getConfigKey( "plotfont" ) );
  if ( s.size() > 0 ) {
    return s;
  } else {
    /* Default font */
    return "-adobe-helvetica-medium-o-normal-*-*-*-*-*-*-*-*-*";
  }
}

GdkLineStyle GSpeakersCFG::get_line_style() {
  string s = string( getConfigKey( "linestyle" ) );
  //  cout << s << ":" << endl;
  if ( s == "solid" ) {
    return GDK_LINE_SOLID;
  } else if ( s == "onoffdash" ) {
    return GDK_LINE_ON_OFF_DASH;
  } else if ( s == "doubledash" ) {
    return GDK_LINE_DOUBLE_DASH;
  } else {
    return GDK_LINE_SOLID;
  }
}

int GSpeakersCFG::get_line_size() {
  return (int)atoi( getConfigKey( "linesize" ) );
}

void GSpeakersCFG::set_font( string newfont ) {
  writeConfigKey( (char *)("plotfont"), (char *)(newfont.c_str()) );
}

void GSpeakersCFG::set_line_style( GdkLineStyle style ) {
  switch ( style ) {
  case GDK_LINE_SOLID:
    writeConfigKey( (char *)("linestyle"), (char *)("solid") );
    break;
  case GDK_LINE_ON_OFF_DASH:
    writeConfigKey( (char *)("linestyle"), (char *)("onoffdash") );
    break;
  case GDK_LINE_DOUBLE_DASH:
    writeConfigKey( (char *)("linestyle"), (char *)("doubledash") );
    break;
  }
}

void GSpeakersCFG::set_line_size( int size ) {
  char *buf = new char[2];
  sprintf( buf, "%d", size );
  writeConfigKey( (char *)("linesize"), buf );
}
