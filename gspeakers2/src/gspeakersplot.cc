/* gspeakersplot.cc
 *
 * Copyright (C) 2001-2002 Daniel Sundberg <dss@home.se>
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
#include "common.h"

GSpeakersPlot::GSpeakersPlot(int lower_x, int upper_x, int lower_y, int upper_y, bool logx, int y_zero_freq)
{
  m_lower_x = lower_x;
  m_upper_x = upper_x;
  if (m_upper_x > 10000) m_upper_x = 20000; else m_upper_x = 1000;
  m_lower_y = lower_y;
  m_upper_y = upper_y;
  m_logx = logx;
  m_y_zero_freq = y_zero_freq;
  m_linesize = 1;
}

bool GSpeakersPlot::on_expose_event(GdkEventExpose* event)
{
  /* 
   * Copy the area that needs to be updated from the pixmap
   * to the window
   */
  get_window()->draw_drawable(get_style()->get_fg_gc(get_state()),
  		     m_refPixmap,
		     /* Only copy the area that was exposed. */
		     event->area.x, event->area.y,
		     event->area.x, event->area.y,
		     event->area.width, event->area.height);
  m_selected_plot = -1;
  //cout << "GSpeakersPlot: on_expose" << endl;

  return false;
}

bool GSpeakersPlot::on_configure_event(GdkEventConfigure* event)
{
  /* Init the pixmap, here we use a pixmap, then we do all drawing to the pixmap, will probably be faster 
     than redrawing the entire thing every time */
  
  m_refPixmap = Gdk::Pixmap::create(get_window(), get_allocation().width, get_allocation().height, -1);
  m_refGC = get_style()->get_fg_gc(get_state());

  m_refColormap = m_refGC->get_colormap();
  white = Gdk::Color("white");
  black = Gdk::Color("black");
  m_refColormap->alloc_color(white);
  m_refColormap->alloc_color(black);

  Glib::RefPtr<Pango::Context> refPangoContext = get_pango_context();
  //Pango::FontDescription font_description = refPangoContext->get_font_description();
  
  //m_refFont = refPangoContext->load_font(font_description);

  m_refLayout = Pango::Layout::create(refPangoContext);

  redraw();

  /* We've handled the configure event, no need for further processing. */
  return true;
}

int GSpeakersPlot::add_plot(vector<GSpeakers::Point> &ref_point_vector, Gdk::Color& ref_color)
{
  cout << "GSpeakersPlot: on add plot" << endl;
  m_refGC->set_rgb_fg_color(ref_color);
  m_visible_plots.push_back(true);
  m_colors.push_back(ref_color);
  m_points.push_back(ref_point_vector);
  
  int total_space_x = get_allocation().width - (2 * BOX_FRAME_SIZE);
  int half_space_x = round( total_space_x / 2 );
  int box_x, box_y, box_width, box_height;
  box_x = BOX_FRAME_SIZE;
  box_y = BOX_FRAME_SIZE;
  box_width = get_allocation().width - (2 * BOX_FRAME_SIZE);
  box_height = get_allocation().height - (2 * BOX_FRAME_SIZE);


  vector<Gdk::Point> points;
  double f_div, f_mapped;
  int x, y;
  
  vector<GSpeakers::Point>::iterator iter;
  for ( iter = ref_point_vector.begin(); iter != ref_point_vector.end(); ++iter ) {
    if (m_upper_x == 20000) {
      if ( (*iter).get_x() < 100 ) {
        /* Devide by 10 to only log numbers 0 < number < 10 */
        f_div = (double)((*iter).get_x()) / 10;
        f_mapped = log10( f_div );
        /* This is the x coordinate */
        x = BOX_FRAME_SIZE + round( half_space_x / 2 * f_mapped );
      } else if ((*iter).get_x() >= 100) {
        /* Devide by 100 to only log numbers 0 < number < 10 */
        f_div = (double)((*iter).get_x()) / 100;
        f_mapped = log10( f_div );
        /* This is the x coordinate */
        x = round(BOX_FRAME_SIZE + half_space_x / 2 + round( half_space_x / 2 * f_mapped ));
      } else if ((*iter).get_x() >= 1000) {
        /* Devide by 1000 to only log numbers 0 < number < 10 */
        f_div = (double)((*iter).get_x()) / 1000;
        f_mapped = log10( f_div );
        /* This is the x coordinate */
        x = BOX_FRAME_SIZE + half_space_x + round( (half_space_x / 2) * f_mapped );
      } else if ((*iter).get_x() >= 10000) {
        /* Devide by 1000 to only log numbers 0 < number < 10 */
        f_div = (double)((*iter).get_x()) / 10000;
        f_mapped = log10( f_div );
        /* This is the x coordinate */
        x = round(BOX_FRAME_SIZE + 1.5 * half_space_x + round( (half_space_x / 2) * f_mapped ));
      }
  

    } else {
      if ( (*iter).get_x() < 100 ) {
        /* Devide by 10 to only log numbers 0 < number < 10 */
        f_div = (double)((*iter).get_x()) / 10;
        f_mapped = log10( f_div );
        /* This is the x coordinate */
        x = BOX_FRAME_SIZE + round( half_space_x * f_mapped );
      } else if ((*iter).get_x() >= 100) {
        /* Devide by 100 to only log numbers 0 < number < 10 */
        f_div = (double)((*iter).get_x()) / 100;
        f_mapped = log10( f_div );
        /* This is the x coordinate */
        x = BOX_FRAME_SIZE + half_space_x + round( half_space_x * f_mapped );
      }    
    }

    /* Zero-level is on 3/4 of box_size_y, map -60 - 20 dB onto this scale  */
    if ( (*iter).get_y() < MAX_NEG_VALUE ) {
      (*iter).set_y(MAX_NEG_VALUE);
    } else if ((*iter).get_y() > MAX_POS_VALUE ) {
      (*iter).set_y(MAX_POS_VALUE);
    }
    /* Calculate y-coordinate */
    y = round( box_height + BOX_FRAME_SIZE - 
               ( (double)(-MAX_NEG_VALUE) + (*iter).get_y() ) * 
               ( box_height / (double)( -MAX_NEG_VALUE + MAX_POS_VALUE ) ) );
    /* Don't draw anything if we got zeros */
    if ( (*iter).get_y() > MAX_NEG_VALUE ) {
      //	if ( ( old_x == 0 ) || ( old_y == 0 ) ) { old_x = x; old_y = y; }
      //	window.draw_line( gc, old_x, old_y, x, y );
      Gdk::Point p;
      p.set_x(x);
      p.set_y(y);
      points.push_back(p);
    } 
  }
  /* Don't draw the line until we have it all done */
  m_refPixmap->draw_lines( m_refGC, points );

  /* Reset rgb fg color */
  m_refGC->set_rgb_fg_color(black);
  
  //select_plot(m_colors.size() - 1);
  Gdk::Rectangle update_rect(0, 0, get_allocation().width, get_allocation().height);
  get_window()->invalidate_rect(update_rect, false);
  /* Return index of the new plot so that the owner of this plot can keep track of plots */
  return m_colors.size() - 1;
}

void GSpeakersPlot::replace_plot(int index, vector<GSpeakers::Point> &p, Gdk::Color& ref_color)
{
  m_points[index] = p;
  m_colors[index] = ref_color;
  
  redraw();
  Gdk::Rectangle update_rect(0, 0, get_allocation().width, get_allocation().height);
  get_window()->invalidate_rect(update_rect, false);
}

void GSpeakersPlot::remove_plot(int n)
{
  int i = 0;
  cout << "n == " << n << endl;
  
  /* For some reason something goes wrong when we select the last row so we add a special case for that event */
  if (n == (int)(m_points.size() - 1)) {
    cout << "GSpeakersPlot: last list item" << endl;
    m_points.erase(m_points.begin() + m_points.size());
    m_colors.erase(m_colors.begin() + m_colors.size());
    m_visible_plots.erase(m_visible_plots.begin() + m_visible_plots.size());
  } else {
  
    for (vector< vector<GSpeakers::Point> >::iterator iter = m_points.begin();
         iter != m_points.end();
         ++iter, i++)
    {
      if (n == i) {
        m_points.erase(iter);
      }
    }
    
    i = 0;
    for (vector<Gdk::Color>::iterator iter = m_colors.begin();
         iter != m_colors.end();
         ++iter, i++)
    {
      if (n == i) {
        m_colors.erase(iter);
      }
    }
    i = 0;
    for (vector<bool>::iterator iter = m_visible_plots.begin();
         iter != m_visible_plots.end();
         ++iter, i++)
    {
      if (n == i) {
        m_visible_plots.erase(iter);
      }
    }
  }
  cout << "loop 3" << endl;
  m_selected_plot = -1;
  redraw();
  Gdk::Rectangle update_rect(0, 0, get_allocation().width, get_allocation().height);
  get_window()->invalidate_rect(update_rect, false);
}

void GSpeakersPlot::remove_all_plots()
{
//  for (unsigned i = m_points.size(); i > 0; --i) {
//    remove_plot((int)i);
//  }
  //cout << "number of point vectors: " << m_points.size() << endl;
  m_points.erase(m_points.begin(), m_points.end());
  m_colors.erase(m_colors.begin(), m_colors.end());
  m_visible_plots.erase(m_visible_plots.begin(), m_visible_plots.end());
//  for (vector< vector<GSpeakers::Point> >::iterator iter = m_points.begin();
//       iter != m_points.end();
//       ++iter)
//  {
//    m_points.erase(iter);
//  }
//  for (vector<Gdk::Color>::iterator iter = m_colors.begin();
//       iter != m_colors.end();
//       ++iter)
//  {
//    m_colors.erase(iter);
//  }
//  for (vector<bool>::iterator iter = m_visible_plots.begin();
 //      iter != m_visible_plots.end();
 //      ++iter)
 // {
 //   m_visible_plots.erase(iter);
 // }
  redraw();
  Gdk::Rectangle update_rect(0, 0, get_allocation().width, get_allocation().height);
  get_window()->invalidate_rect(update_rect, false);
  
}

void GSpeakersPlot::hide_plot(int n)
{
  m_visible_plots[n] = !m_visible_plots[n];
  redraw();
  Gdk::Rectangle update_rect(0, 0, get_allocation().width, get_allocation().height);
  get_window()->invalidate_rect(update_rect, false);

}

void GSpeakersPlot::select_plot(int index) 
{
  m_selected_plot = index;
  redraw();
  Gdk::Rectangle update_rect(0, 0, get_allocation().width, get_allocation().height);
  get_window()->invalidate_rect(update_rect, false);
}

void GSpeakersPlot::redraw()
{
  /* Clear to white background color */
  m_refGC->set_rgb_fg_color(white);
  m_refPixmap->draw_rectangle(m_refGC, true, 0, 0, get_allocation().width, get_allocation().height);
  
   /* Calc coordinates for a rectangular box */
  int box_x, box_y, box_width, box_height;
  box_x = BOX_FRAME_SIZE;
  box_y = BOX_FRAME_SIZE;
  box_width = get_allocation().width - (2 * BOX_FRAME_SIZE);
  box_height = get_allocation().height - (2 * BOX_FRAME_SIZE);
    
  /* Draw the box */
  m_refGC->set_rgb_fg_color(black);
  m_refPixmap->draw_rectangle(m_refGC, false, box_x, box_y, box_width, box_height);
  
  if (m_logx == true) {
    draw_log_grid();
  } else {
    draw_lin_grid();
  }
  
  /* Draw some horizontal lines */
  //int inc_space_y = round( box_height /  (double)N_VERTICAL_LINES );
  for (int i = MAX_NEG_VALUE; i < MAX_POS_VALUE; i = i + 5) {
    int y = round( box_height + BOX_FRAME_SIZE - 
	       ( (double)(-MAX_NEG_VALUE) + (double)i ) * 
	       ( box_height / (double)( -MAX_NEG_VALUE + MAX_POS_VALUE ) ) );
    m_refPixmap->draw_line(m_refGC, BOX_FRAME_SIZE - 3, y, get_allocation().width - BOX_FRAME_SIZE + 3, y);
    m_refLayout->set_text(int_to_ustring3(i));
    m_refPixmap->draw_layout(m_refGC, BOX_FRAME_SIZE - 18, y - 6, m_refLayout);
  }

  int total_space_x = get_allocation().width - (2 * BOX_FRAME_SIZE);
  int half_space_x = round( total_space_x / 2 );

  /* Map points in m_points to screen points */
  int n_plots = m_points.size();
  for (int i = 0; i < n_plots; i++) {
    if (m_visible_plots[i] == true) {
      m_refGC->set_rgb_fg_color(m_colors[i]);
      
      vector<Gdk::Point> points;
      double f_div, f_mapped;
      int x, y;
      
      vector<GSpeakers::Point>::iterator iter;
      for ( iter = m_points[i].begin(); iter != m_points[i].end(); ++iter ) {

        if (m_upper_x == 20000) {
          if ( (*iter).get_x() < 100 ) {
            /* Devide by 10 to only log numbers 0 < number < 10 */
            f_div = (double)((*iter).get_x()) / 10;
            f_mapped = log10( f_div );
            /* This is the x coordinate */
            x = BOX_FRAME_SIZE + round( half_space_x / 2 * f_mapped );
          } else if ((*iter).get_x() >= 100) {
            /* Devide by 100 to only log numbers 0 < number < 10 */
            f_div = (double)((*iter).get_x()) / 100;
            f_mapped = log10( f_div );
            /* This is the x coordinate */
            x = round(BOX_FRAME_SIZE + half_space_x / 2 + round( half_space_x / 2 * f_mapped ));
          } else if ((*iter).get_x() >= 1000) {
            /* Devide by 1000 to only log numbers 0 < number < 10 */
            f_div = (double)((*iter).get_x()) / 1000;
            f_mapped = log10( f_div );
            /* This is the x coordinate */
            x = BOX_FRAME_SIZE + half_space_x + round( (half_space_x / 2) * f_mapped );
          } else if ((*iter).get_x() >= 10000) {
            /* Devide by 1000 to only log numbers 0 < number < 10 */
            f_div = (double)((*iter).get_x()) / 10000;
            f_mapped = log10( f_div );
            /* This is the x coordinate */
            x = round(BOX_FRAME_SIZE + 1.5 * half_space_x + round( (half_space_x / 2) * f_mapped ));
          }
      
    
        } else {
          if ( (*iter).get_x() < 100 ) {
            /* Devide by 10 to only log numbers 0 < number < 10 */
            f_div = (double)((*iter).get_x()) / 10;
            f_mapped = log10( f_div );
            /* This is the x coordinate */
            x = BOX_FRAME_SIZE + round( half_space_x * f_mapped );
          } else if ((*iter).get_x() >= 100) {
            /* Devide by 100 to only log numbers 0 < number < 10 */
            f_div = (double)((*iter).get_x()) / 100;
            f_mapped = log10( f_div );
            /* This is the x coordinate */
            x = BOX_FRAME_SIZE + half_space_x + round( half_space_x * f_mapped );
          }    
        }
  
        /* Zero-level is on 3/4 of box_size_y, map -60 - 20 dB onto this scale  */
        if ( (*iter).get_y() < MAX_NEG_VALUE ) {
          (*iter).set_y(MAX_NEG_VALUE);
        } else if ((*iter).get_y() > MAX_POS_VALUE ) {
          (*iter).set_y(MAX_POS_VALUE);
        }
        /* Calculate y-coordinate */
        y = round( box_height + BOX_FRAME_SIZE - 
                   ( (double)(-MAX_NEG_VALUE) + (*iter).get_y() ) * 
                   ( box_height / (double)( -MAX_NEG_VALUE + MAX_POS_VALUE ) ) );
        /* Don't draw anything if we got zeros */
        if ( (*iter).get_y() > MAX_NEG_VALUE ) {
          //	if ( ( old_x == 0 ) || ( old_y == 0 ) ) { old_x = x; old_y = y; }
          //	window.draw_line( gc, old_x, old_y, x, y );
          Gdk::Point p;
          p.set_x(x);
          p.set_y(y);
          points.push_back(p);
        } 
      }
      /* Don't draw the line until we have it all done */
      if (i == m_selected_plot) {
        m_linesize = m_linesize + 2;
        m_refGC->set_line_attributes(m_linesize, Gdk::LINE_SOLID, Gdk::CAP_NOT_LAST, Gdk::JOIN_MITER);
        cout << "select plot" << endl;
      }
      m_refPixmap->draw_lines( m_refGC, points );
      if (i == m_selected_plot) {
        m_linesize = m_linesize - 2;
        m_refGC->set_line_attributes(m_linesize, Gdk::LINE_SOLID, Gdk::CAP_NOT_LAST, Gdk::JOIN_MITER);
        cout << "select plot" << endl;
      }

    }
  }

  /* Reset rgb fg color */
  m_refGC->set_rgb_fg_color(black);
}
  
void GSpeakersPlot::draw_log_grid()
{
  int total_space_x = get_allocation().width - (2 * BOX_FRAME_SIZE);
  int quarter_space_x = round( total_space_x / 4 );
  int half_space_x = round( total_space_x / 2 );
  int xaxis_y_position = get_allocation().height - BOX_FRAME_SIZE;

  /* Draw the logaritmic vertical x-lines */
  if (m_upper_x == 20000) {
    half_space_x = quarter_space_x;
  }
  for (int i = 0; i <= 10; i++) {
    int x = BOX_FRAME_SIZE + round( log10(i) * half_space_x );
    m_refPixmap->draw_line(m_refGC, x, BOX_FRAME_SIZE, x, xaxis_y_position + 3);

    /* Draw text below some vertical lines */
    if ( ( i == 2 ) || ( i == 5 ) ) {
      m_refLayout->set_text(int_to_ustring(10 * i));
      m_refPixmap->draw_layout(m_refGC, x - 4, xaxis_y_position + 5, m_refLayout);
    } 
  }

  for (int i = 1; i <= 10; i++) {
    int x = BOX_FRAME_SIZE + round( log10(i) * half_space_x );
    m_refPixmap->draw_line(m_refGC, half_space_x + x, BOX_FRAME_SIZE, half_space_x + x, xaxis_y_position + 3);
    if ( ( i == 2 ) || ( i == 5 ) || (i == 1) || (i == 10)) {
      m_refLayout->set_text(int_to_ustring(100 * i));
      m_refPixmap->draw_layout(m_refGC, half_space_x + x - 8, xaxis_y_position + 5, m_refLayout);
    } 
  }
  /* Draw some more vertical lines if upper limit is 20000 Hz */
  if (m_upper_x == 20000) {
    for (int i = 1; i <= 10; i++) {
      int x = BOX_FRAME_SIZE + round( log10(i) * half_space_x );
      m_refPixmap->draw_line(m_refGC, 2 * half_space_x + x, BOX_FRAME_SIZE, 2 * half_space_x + x, xaxis_y_position + 3);
  
      /* Draw text below some vertical lines */
      if ( ( i == 2 ) || ( i == 5 ) ) {
        m_refLayout->set_text(int_to_ustring(i) + "k");
        m_refPixmap->draw_layout(m_refGC, 2 * half_space_x + x - 8, xaxis_y_position + 5, m_refLayout);
      } 
    }
  
    for (int i = 1; i <= 10; i++) {
      int x = BOX_FRAME_SIZE + round( log10(i) * half_space_x );
      m_refPixmap->draw_line(m_refGC, 3 * half_space_x + x, BOX_FRAME_SIZE, 3 * half_space_x + x, xaxis_y_position + 3);
      if ( ( i == 2 ) || (i == 5) || (i == 10)) {
        m_refLayout->set_text(int_to_ustring(10 * i) + "k");
        m_refPixmap->draw_layout(m_refGC, 3 * half_space_x + x - 8, xaxis_y_position + 5, m_refLayout);
      } 
    }
  }
}

/*
 * Draw the vertical lines in a linear plot 
 */
void GSpeakersPlot::draw_lin_grid()
{
  /* Calc total horizontal space inside the box */
  int total_space_x = get_allocation().width - (2 * BOX_FRAME_SIZE);
  
  /* Calculate the xaxis vertical position, from upper edge of the drawingarea */
  int xaxis_y_position = get_allocation().height - BOX_FRAME_SIZE;
  
  /* Determine distance (in x units) between the vertical lines */
  int freq_range = m_upper_x - m_lower_x;
  int x_inc = 10;
  if ((freq_range > 200) && (freq_range <= 2000)) {
    x_inc = 100;
  } else if ((freq_range > 2000) && (freq_range <= 20000)) {
    x_inc = 1000;
  } 
  
  /* Calculate number of pixels between each vertical line */
  int x_inc_pixels = round(((double)x_inc / (double)freq_range) * (double)total_space_x);
  
  /* Calculate number of lines we're going to draw */
  int n_lines = round((double)freq_range / (double)x_inc);
  
  /* Draw the vertical lines to the pixmap */
  for (int i = 1; i < n_lines; i++) {
    m_refPixmap->draw_line(m_refGC, BOX_FRAME_SIZE + i * x_inc_pixels, BOX_FRAME_SIZE, 
                                    BOX_FRAME_SIZE + i * x_inc_pixels, xaxis_y_position + 3);
  }
}  
  
void GSpeakersPlot::draw_horz_grid()
{
}
  
void GSpeakersPlot::set_font( const string& font )
{

}

void GSpeakersPlot::set_line_style( Gdk::LineStyle &style )
{

}

void GSpeakersPlot::set_line_size( int size )
{

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

Glib::ustring GSpeakersPlot::int_to_ustring(int d)
{
  char *str = NULL;
  GString *buffer = g_string_new(str);
  g_string_printf(buffer, "%d", d);
  return Glib::ustring(buffer->str);
}

Glib::ustring GSpeakersPlot::int_to_ustring3(int d)
{
  char *str = NULL;
  GString *buffer = g_string_new(str);
  g_string_printf(buffer, "%3d", d);
  return Glib::ustring(buffer->str);
}
