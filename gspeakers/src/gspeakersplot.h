/* gspeakersplot.h
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

#ifndef __GSPEAKERSPLOT_H
#define __GSPEAKERSPLOT_H

#include <math.h>
#include <vector>
#include <gnome--/main.h>
#include <gnome--/app.h>
#include <gtk--/statusbar.h>
#include <gtk--/button.h>
#include <gtk--/label.h>
#include <gtk--/frame.h>
#include <gtk--/drawingarea.h>
#include <gtk--/window.h>
#include <gdk--/color.h>
#include "boxtoolbar.h"
#include "speakertoolbar.h"

#define BOX_FRAME_SIZE 20
#define N_VERTICAL_LINES 9
#define UPPER_LIMIT 1000

/*
 * This is a class that can draw plots. It has add_plot, remove_plot 
 * and a few other methods you can use. It has logarithmic x-axis and 
 * a flat y-axis. The x-axis is supposed to be used for frequency and 
 * the y-axis can be used for something linear, for example dB.
 */
class GSpeakersPlot : public Gtk::DrawingArea {
  Gdk_GC       gc;
  Gdk_Color    fg_color, bg_color, blue_, red_, green_, white_, black_;
  Gdk_Window   window;
  Gdk_Colormap colormap;
  Gdk_Font     f;
public:
  GSpeakersPlot();
  void add_plot(double *dbmag, Gdk_Color *c);
  void remove_plot( int n );
  void remove_all_plots();
  void redraw();

protected:
  /* This is the function that draws the graphics */
  virtual gint expose_event_impl(GdkEventExpose* e); 
  
  virtual void realize_impl()
  {
    // we need to do the default realize
    Gtk::DrawingArea::realize_impl();
    
    // Now we can allocate any additional resources we need
    window = get_window();
    gc.create( window ); 
    window.set_background( colormap.white() );
    window.clear();
    gc.set_foreground( colormap.black() );
  }
  
  /* Vectors that hold the y magnitude points for the plots (dbmag) and 
     the corresponding colors (colors) */
  vector<double *> dbmag;
  vector<Gdk_Color *> colors;

 private:
  int round( double );

};

#endif
