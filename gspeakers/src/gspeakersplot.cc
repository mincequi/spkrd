/* gspeakersplot.cc
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

#include <math.h>
#include "gspeakersplot.h"

#define UPPER_LIMIT 1000

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
}

gint GSpeakersPlot::expose_event_impl(GdkEventExpose* e) {
  // here is where we draw on the window
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
  half_space_x = (int)(total_space_x / 2);
  
  /* Draw the logaritmic vertical x-lines */
  for (int i = 2; i < 10; i++) {
    int x = BOX_FRAME_SIZE + (int)(log10(i) * half_space_x);
    window.draw_line(gc, x, BOX_FRAME_SIZE, x, xaxis_y + 3);
  }
  for (int i = 1; i < 10; i++) {
    int x = BOX_FRAME_SIZE + (int)(log10(i) * half_space_x);
    window.draw_line(gc, half_space_x + x, BOX_FRAME_SIZE, half_space_x + x, xaxis_y + 3);
  }
  
  /* Draw vertical lines */
  inc_space_y = (int)(box_size_y / N_VERTICAL_LINES);
  for (int i = 1; i < N_VERTICAL_LINES; i++) {
    int y = BOX_FRAME_SIZE + i * inc_space_y;
    window.draw_line(gc, BOX_FRAME_SIZE - 3, y, size_x - BOX_FRAME_SIZE + 3, y);
  }

  /* Finally draw the plots */
  int n_plots = dbmag.size();
  //double *db_mag;
  double f_div, f_mapped;
  int x, y;

  for ( int i = 0; i < n_plots; i++ ) {
    double *db_mag = dbmag[i];
    gc.set_foreground( *colors[i] );
    //    cout << colors[i]->red_p() << ":" << colors[i]->green_p() << ":" << colors[i]->blue_p() << endl;

    for ( int f = 10; f < UPPER_LIMIT; f++ ) {

      if ( f < 100 ) {
	/* Devide by 10 to only log numbers 0 < number < 10 */
	f_div = (double)f / 10;
	f_mapped = log10( f_div );
	/* This is the x coordinate */
	x = BOX_FRAME_SIZE + (int)( half_space_x * f_mapped );
      } else {
	/* Devide by 100 to only log numbers 0 < number < 10 */
	f_div = (double)f / 100;
	f_mapped = log10( f_div );
	/* This is the x coordinate */
	x = BOX_FRAME_SIZE + half_space_x + (int)( half_space_x * f_mapped );
      }

      /* Zero-level is on 3/4 of box_size_y, map -60 - 20 dB onto this scale  */
      if ( db_mag[f] < -60 ) {
	db_mag[f] = -60.0;
      } else if ( db_mag[f] > 20 ) {
	db_mag[f] = 20.0;
      }
      if ( db_mag[f] > 0 ) {
	y = (int)( size_y - BOX_FRAME_SIZE - ( 60 + db_mag[f] ) * ( box_size_y / 80 ) );
      } else { 
	y = (int)( size_y - BOX_FRAME_SIZE - ( 60 + db_mag[f] ) * ( box_size_y / 80 ) );
      }
      window.draw_rectangle( gc, true, x - 2, y - 2, 5, 5 );
    }
  }

  return true;
}

void GSpeakersPlot::add_plot(Box *b, Speaker *s, Gdk_Color *c) {
  //  cout << "add plot" << endl;
  double *db_mag = new double[UPPER_LIMIT];
  double A, B, C, D, fn2, fn4, fr, vr, qr, qb;

  /* Calculate the frequency response for the supplied speaker and box */
  if ( b->type == PORTED ) {
    
    for (int f = 1; f < UPPER_LIMIT; f++) {
      A = pow( ( b->fb1 / s->fs ), 2 );
      B = A / s->qts + b->fb1 / ( 7 * s->fs * s->qts );
      C = 1 + A + ( s->vas / b->vol1 ) + b->fb1 / ( 7 * s->fs * s->qts );
      D = 1 / s->qts + b->fb1 / ( 7 * s->fs );
      fn2 = pow( ( f / s->fs ), 2 );
      fn4 = pow( fn2, 2 );
      db_mag[f] = 10 * log10( pow( fn4, 2 ) / ( pow( ( fn4 - C * fn2 + A ), 2 ) + 
					       fn2 * pow( ( D * fn2 - B), 2 ) ) );
//        if (f < 200)
//  	cout << db_mag[f] << ", ";
    }

  } else if ( b->type == SEALED ) {
    for (int f = 1; f < UPPER_LIMIT; f++) {
      fr = pow( ( f / b->fb1 ), 2 );
      vr = s->vas / b->vol1;
      qr = sqrt( vr + 1 );
      qb = 1 / ( ( 1 / s->qts ) / qr + 0.1 );
      db_mag[f] = 10 * log10( pow( fr, 2 ) / ( pow( ( fr - 1 ), 2 ) + fr / ( pow( qb, 2 ) ) ) );
//        if (f < 200)
//  	cout << db_mag[f] << ", ";
    }
  }
  //  cout << endl;
  dbmag.push_back( db_mag );
  //  cout << c->red_p() << ":" << c->green_p() << ":" << c->blue_p() << endl;
  colors.push_back( new Gdk_Color( c ) );
  int i = colors.size();
  colormap.alloc( *colors[i - 1] );
  /* Finally redraw the plot to show the new one */
  expose_event_impl( NULL );
}

void GSpeakersPlot::remove_plot( int n ) {
  cout << "remove selected plot" << endl;
  for (
       vector<double *>::iterator from = dbmag.begin(); 
       from != dbmag.end(); 
       ++from
       ) {
    if (*from == dbmag[n]) {
      delete dbmag[n];
      dbmag.erase( from );
      }
  }
  for (
       vector<Gdk_Color *>::iterator from = colors.begin(); 
       from != colors.end(); 
       ++from
       ) {
    if (*from == colors[n]) { 
      delete colors[n];
      colors.erase( from );
    }
  }
  redraw();
}

void GSpeakersPlot::redraw() {
  /* I suppose I can do this??? Seems to work... */
  expose_event_impl( NULL );
}
