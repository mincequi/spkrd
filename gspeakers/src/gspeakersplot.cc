/* gspeakersplot.cc
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

#include <math.h>
#include "gspeakersplot.h"

#define MAX_NEG_VALUE -40
#define MAX_POS_VALUE 10

GSpeakersPlot::GSpeakersPlot() {
  // in the ctor you can only allocate colors, 
  // get_window() returns 0 because we have not be realized
  Gdk_Colormap colormap_ = get_default_colormap ();
  set_usize(300, 400);
  blue_  = Gdk_Color("blue");
  red_   = Gdk_Color("red");
  green_ = Gdk_Color("green");
  white_ = Gdk_Color("white");
  black_ = Gdk_Color("black");
  
  fg_color = black_;
  bg_color = white_;
  
  colormap_.alloc(blue_);
  colormap_.alloc(red_);
  colormap_.alloc(green_);
  colormap_.alloc(white_);
  colormap_.alloc(black_);
  colormap = colormap_;
  f = Gdk_Font( "-adobe-helvetica-medium-o-normal-*-*-*-*-*-*-*-*-*" );
}

/*
 * This is the function that does all the drawing on the window
 */
gint GSpeakersPlot::expose_event_impl(GdkEventExpose* e) {
  int x, y;
  int size_x, size_y;
  int box_x, box_y, box_size_x, box_size_y;
  int xaxis_y;
  int total_space_x, half_space_x;
  int inc_space_y;
  window.clear();
  window.get_size( size_x, size_y );

  /* Draw a box */
  box_x = BOX_FRAME_SIZE;
  box_y = BOX_FRAME_SIZE;
  box_size_x = size_x - 2 * BOX_FRAME_SIZE;
  box_size_y = size_y - 2 * BOX_FRAME_SIZE;
  xaxis_y = size_y - BOX_FRAME_SIZE;
  
  /* Draw the grid, this is locked to <1000 Hz by now */
  gc.set_foreground( black_ );
  
  window.draw_rectangle(gc, false, box_x, box_y, box_size_x, box_size_y);
  total_space_x = size_x - 2 * BOX_FRAME_SIZE;
  half_space_x = round( total_space_x / 2 );
  
  /* Draw the logaritmic vertical x-lines */
  for (int i = 2; i < 10; i++) {
    int x = BOX_FRAME_SIZE + round( log10(i) * half_space_x );
    window.draw_line(gc, x, BOX_FRAME_SIZE, x, xaxis_y + 3);
    if ( ( i == 2 ) || ( i == 5 ) ) {
      char *buf = new char[3];
      sprintf( buf, "%3d", 10 * i);
      window.draw_string( f, gc, x - 4, xaxis_y + 10, string( buf ) );
    } 
  }
  for (int i = 1; i < 10; i++) {
    int x = BOX_FRAME_SIZE + round( log10(i) * half_space_x );
    window.draw_line(gc, half_space_x + x, BOX_FRAME_SIZE, half_space_x + x, xaxis_y + 3);
    if ( ( i == 2 ) || ( i == 5 ) || (i == 1) ) {
      char *buf = new char[3];
      sprintf( buf, "%3d", 100 * i);
      window.draw_string( f, gc, half_space_x + x - 4, xaxis_y + 10, string( buf ) );
    } 
  }
  window.draw_string( f, gc, box_size_x - 20, xaxis_y + 10, "Frequency (Hz)" );
  
  /* Draw vertical lines */
  inc_space_y = round( box_size_y /  (double)N_VERTICAL_LINES );
  for (int i = MAX_NEG_VALUE; i < MAX_POS_VALUE; i = i + 5) {
    y = round( box_size_y + BOX_FRAME_SIZE - 
	       ( (double)(-MAX_NEG_VALUE) + (double)i ) * 
	       ( box_size_y / (double)( -MAX_NEG_VALUE + MAX_POS_VALUE ) ) );
    window.draw_line(gc, BOX_FRAME_SIZE - 3, y, size_x - BOX_FRAME_SIZE + 3, y);
    char *buf = new char[3];
    sprintf( buf, "%3d", i);
    window.draw_string( f, gc, BOX_FRAME_SIZE - 18, y + 4, string( buf ) );
  }
  window.draw_string( f, gc, BOX_FRAME_SIZE - 15, BOX_FRAME_SIZE - 5, "Magnitude (dB)" );

  /* Finally draw the plots */
  int n_plots = dbmag.size();
  double f_div, f_mapped;

  for ( int i = 0; i < n_plots; i++ ) {
    double *db_mag = dbmag[i];
    gc.set_foreground( *colors[i] );

    for ( int f = 10; f < UPPER_LIMIT; f++ ) {

      if ( f < 100 ) {
	/* Devide by 10 to only log numbers 0 < number < 10 */
	f_div = (double)f / 10;
	f_mapped = log10( f_div );
	/* This is the x coordinate */
	x = BOX_FRAME_SIZE + round( half_space_x * f_mapped );
      } else {
	/* Devide by 100 to only log numbers 0 < number < 10 */
	f_div = (double)f / 100;
	f_mapped = log10( f_div );
	/* This is the x coordinate */
	x = BOX_FRAME_SIZE + half_space_x + round( half_space_x * f_mapped );
      }

      /* Zero-level is on 3/4 of box_size_y, map -60 - 20 dB onto this scale  */
      if ( db_mag[f] < MAX_NEG_VALUE ) {
	db_mag[f] = MAX_NEG_VALUE;
      } else if ( db_mag[f] > MAX_POS_VALUE ) {
	db_mag[f] = MAX_POS_VALUE;
      }
      /* Calculate y-coordinate */
      y = round( box_size_y + BOX_FRAME_SIZE - 
		 ( (double)(-MAX_NEG_VALUE) + db_mag[f] ) * 
		 ( box_size_y / (double)( -MAX_NEG_VALUE + MAX_POS_VALUE ) ) );
      /* Don't draw anything if we got zeros */
      if ( db_mag[f] > MAX_NEG_VALUE ) {
	window.draw_rectangle( gc, true, x, y, 1, 1 );
      }
    }
  }
  return true;
}

/*
 * Add a plot to the drawingarea
 */
void GSpeakersPlot::add_plot(double *db_mag, Gdk_Color *c) {
  /* Insert the dbmag-vector (and related color) into vectors of dB magnitude */
  dbmag.push_back( db_mag );
  colors.push_back( new Gdk_Color( c ) );
  int i = colors.size();
  colormap.alloc( *colors[i - 1] );

  /* Finally redraw the plot to show the new one */
  expose_event_impl( NULL );
}

/*
 * Remove plot #n
 */
void GSpeakersPlot::remove_plot( int n ) {
  cout << "remove selected plot:" << n << endl;
  if ( ( dbmag.size() - 1 ) == (unsigned)n ) {
    delete dbmag[n];
    delete colors[n];
    dbmag.pop_back();
    colors.pop_back();
  } else {
    for (
	 vector<double *>::iterator from = dbmag.begin(); 
	 from != dbmag.end(); 
	 from++
	 ) {
      if (*from == dbmag[n]) {
	delete dbmag[n];
	dbmag.erase( from );
      }
    }
    for (
	 vector<Gdk_Color *>::iterator from = colors.begin(); 
	 from != colors.end(); 
	 from++
	 ) {
      if (*from == colors[n]) { 
	delete colors[n];
      colors.erase( from );
      }
    }
  }
  redraw();
}

/*
 * Remove all plots
 */
void GSpeakersPlot::remove_all_plots() {
  int size_v = colors.size();
  for ( int i = size_v - 1; i >= 0; i-- ) {
    delete dbmag[i];
    delete colors[i];
    dbmag.pop_back();
    colors.pop_back();
  }
  redraw();
}

/*
 * Update the drawingarea
 */
void GSpeakersPlot::redraw() {
  /* I suppose I can do this??? Seems to work... */
  expose_event_impl( NULL );
}

/*
 * Helper function to convert double->int and round it to nearest int
 *
 * The compiler wouldn't accept round (in math.h) so...
 */
int GSpeakersPlot::round( double x ) {
  if ( ( x - (int)x ) >= 0.5 ) {
    return (int)( (int)x + 1 );
  } else {
    return (int)(x);
  }
  return 0;
}
