/* crossoverimageview.cc
 *
 * $Id$
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

#include <pangomm/context.h>
#include <gdkmm/rectangle.h>
#include "crossoverimageview.h"
#include "common.h"

CrossoverImageView::CrossoverImageView()
{
  using SigC::slot;
  
  visible = false;
  crossover = NULL;
  signal_crossover_selected.connect(slot(*this, &CrossoverImageView::on_crossover_selected));
}

CrossoverImageView::~CrossoverImageView()
{
}

bool CrossoverImageView::on_expose_event(GdkEventExpose* event)
{
  get_window()->draw_drawable(get_style()->get_fg_gc(get_state()),
  		     m_refPixmap,
		     /* Only copy the area that was exposed. */
		     event->area.x, event->area.y,
		     event->area.x, event->area.y,
		     event->area.width, event->area.height);
  return false;
}

bool CrossoverImageView::on_configure_event(GdkEventConfigure* event)
{
  visible = true;
  m_refPixmap = Gdk::Pixmap::create(get_window(), get_allocation().width, get_allocation().height, -1);
  
  m_refGC = get_style()->get_fg_gc(get_state());

  m_refColormap = m_refGC->get_colormap();
  white = Gdk::Color("white");
  black = Gdk::Color("black");
  m_refColormap->alloc_color(white);
  m_refColormap->alloc_color(black);

  Glib::RefPtr<Pango::Context> refPangoContext = get_pango_context();
  m_refLayout = Pango::Layout::create(refPangoContext);
  
  redraw();

  /* We've handled the configure event, no need for further processing. */
  return true;
}

void CrossoverImageView::redraw()
{
  if (visible == true) {
    /* Clear to white background color */
    m_refGC->set_rgb_fg_color(white);
    m_refPixmap->draw_rectangle(m_refGC, true, 0, 0, get_allocation().width, get_allocation().height);
    m_refGC->set_rgb_fg_color(black);

    if (crossover != NULL) {
      int vert_space_per_net_devider = 0;
      if (crossover->get_type() & CROSSOVER_TYPE_LOWPASS) vert_space_per_net_devider++;
      if (crossover->get_type() & CROSSOVER_TYPE_SUBSONIC) vert_space_per_net_devider++;
      if (crossover->get_type() & CROSSOVER_TYPE_HIGHPASS) vert_space_per_net_devider++;
      if (crossover->get_type() & CROSSOVER_TYPE_TWOWAY) vert_space_per_net_devider += 2;
      if (crossover->get_type() & CROSSOVER_TYPE_THREEWAY) vert_space_per_net_devider += 3;
      if (crossover->get_type() & CROSSOVER_TYPE_FOURWAY) vert_space_per_net_devider += 4;
      
      if (vert_space_per_net_devider == 0) return;
      
      int window_height = get_allocation().height;
      int window_width  = get_allocation().width;
      int vert_space_per_net = GSpeakers::round(window_height / vert_space_per_net_devider);
      
      cout << "vert_space_per_net: " << vert_space_per_net << endl;
      
      /* Draw first net here */
      vector<Net>& net_vector = *crossover->networks();
      for (unsigned i = 0; i < net_vector.size(); i++) {
        
        int net_vert_devider = 3; 
        int part_height = GSpeakers::round(vert_space_per_net / net_vert_devider);
        
        int net_horz_devider = 2;
        switch (net_vector[i].get_lowpass_order()) {
          case NET_ORDER_1ST:
            net_horz_devider++;
            break;
          case NET_ORDER_2ND:
            net_horz_devider += 2;
            break;
          case NET_ORDER_3RD:
            net_horz_devider += 3;
            break;
          case NET_ORDER_4TH:
            net_horz_devider += 4;
            break;
        }
        switch (net_vector[i].get_highpass_order()) {
          case NET_ORDER_1ST:
            net_horz_devider++;
            break;
          case NET_ORDER_2ND:
            net_horz_devider += 2;
            break;
          case NET_ORDER_3RD:
            net_horz_devider += 3;
            break;
          case NET_ORDER_4TH:
            net_horz_devider += 4;
            break;
        }
        if (net_vector[i].get_has_imp_corr() == true) net_horz_devider++;
        if (net_vector[i].get_has_damp() == true)     net_horz_devider += 2;
        int part_width = GSpeakers::round(window_width / net_horz_devider);
        
        cout << "part_height = " << part_height << ", part_width = " << part_width << endl;
        draw_connector(0, i * vert_space_per_net, part_width, part_height, true);
        draw_connector(0, (i + 1) * vert_space_per_net - part_height, part_width, part_height, false);
        
        int order = net_vector[i].get_lowpass_order();
        vector<Part>& part_vector = *net_vector[i].parts();
        if (order > 0) {
          vector<Part> lowpass_parts(part_vector.begin(), part_vector.begin() + order);
          draw_lowpass_net(part_width, i * vert_space_per_net, part_width, part_height, lowpass_parts);
        }
        
        
        
        
        
      }
      
    }



//draw_capacitor(13, 10, 10, width, height, false);
    draw_inductor(15, 200, 200, 49, 87, false);
    draw_capacitor(14, 300, 200, 49, 87, true);
    //draw_inductor(15, 100, 10, width, height, false);
    //draw_inductor(16, 100, 100, width, height, true);
    //draw_connector(200, 10, width, height, true);
    //draw_connector(200, 100, width, height, false);
    //draw_resistor(18, 10, 200, width, height, false);
    //draw_corner(100, 200, width, height, true);
    //draw_corner(300, 10, width, height, false);
    //draw_t_cross(300, 100, width, height, true);
    //draw_t_cross(300, 200, width, height, false);

  }
}

void CrossoverImageView::on_crossover_selected(Crossover *selected_crossover)
{
  cout << "CrossoverImageView::on_crossover_selected" << endl;
  crossover = selected_crossover;
  if (visible == true) {
    cout << "invalidate rectangle" << endl;
    redraw();
    Gdk::Rectangle update_rect(0, 0, get_allocation().width, get_allocation().height);
    get_window()->invalidate_rect(update_rect, true);
  }

//  redraw();
}

void CrossoverImageView::draw_lowpass_net(int x, int y, int part_width, int part_height, std::vector<Part>& parts)
{
  for (unsigned i = 0; i < parts.size(); i++) {
    if (parts[i].get_type() == PART_TYPE_INDUCTOR) {
      draw_inductor(parts[i].get_id(), x + part_width * i, y, part_width, part_height);
      draw_line(x + part_width * i, y + 2 * part_height, part_width, part_height);
    } else {
      draw_t_cross(x + part_width * i, y, part_width, part_height);
      draw_capacitor(parts[i].get_id(), x + part_width * i, y + part_height, part_width, part_height, true);
      draw_t_cross(x + part_width * i, y + 2 * part_height, part_width, part_height, false);
    }
  }
}

void CrossoverImageView::draw_highpass_net(int x, int y, int part_width, int part_height, std::vector<Part>& parts)
{

}

void CrossoverImageView::draw_capacitor(int id, int x, int y, int width, int height, bool rotate)
{
  int half_space_y  = GSpeakers::round(height / 2);
  int half_space_x  = GSpeakers::round(width / 2);
  int small_space_x = GSpeakers::round(width / 20);
  int small_space_y = GSpeakers::round(height / 20);
  
  m_refLayout->set_text("C" + GSpeakers::int_to_ustring(id));
  m_refPixmap->draw_layout(m_refGC, x, y, m_refLayout);
    
  if (rotate == true) {
    /* Horizontal line in capacitor */
    m_refPixmap->draw_line(m_refGC, x + half_space_x, y, x + half_space_x, y + half_space_y - small_space_y);
    m_refPixmap->draw_line(m_refGC, x + half_space_x, y + half_space_y + small_space_y, x + half_space_x, y + height);
    
    /* Vertical lines in capacitor */
    m_refPixmap->draw_line(m_refGC, x + 2 * small_space_x, y + half_space_y - small_space_y,
                                    x + width - 2 * small_space_x, y + half_space_y - small_space_y);
    m_refPixmap->draw_line(m_refGC, x + 2 * small_space_x, y + half_space_y + small_space_y,
                                    x + width - 2 * small_space_x, y + half_space_y + small_space_y);
  } else {
    /* Horizontal line in capacitor */
    m_refPixmap->draw_line(m_refGC, x, y + half_space_y, x + half_space_x - small_space_x, y + half_space_y);
    m_refPixmap->draw_line(m_refGC, x + half_space_x + small_space_x, y + half_space_y, x + width, y + half_space_y);
  
    /* Vertical lines in capacitor */
    m_refPixmap->draw_line(m_refGC, x + half_space_x - small_space_x, y + 2 * small_space_y, 
                                    x + half_space_x - small_space_x, y + height - 2 * small_space_y);
    m_refPixmap->draw_line(m_refGC, x + half_space_x + small_space_x, y + 2 * small_space_y, 
                                    x + half_space_x + small_space_x, y + height - 2 * small_space_y);
  }
}

void CrossoverImageView::draw_inductor(int id, int x, int y, int width, int height, bool rotate)
{
  int half_space_y  = GSpeakers::round(height / 2);
  int half_space_x  = GSpeakers::round(width / 2);
  int small_space_x = GSpeakers::round(width / 20);
  int small_space_y = GSpeakers::round(height / 20);

  m_refLayout->set_text("L" + GSpeakers::int_to_ustring(id));
  m_refPixmap->draw_layout(m_refGC, x, y, m_refLayout);

  if (rotate == true) {
    /* Horizontal line in inductor */
    m_refPixmap->draw_line(m_refGC, x + half_space_x, y, x + half_space_x, y + (2 * small_space_y));
    m_refPixmap->draw_line(m_refGC, x + half_space_x, y + height - (2 * small_space_y), x + half_space_x, y + height);

    /* Arcs in inductor */
    for (int i = 0; i <= 12; i +=4) {
      m_refPixmap->draw_arc(m_refGC, false, x + half_space_x - (2 * small_space_x), y + (2 * small_space_y) + (i * small_space_y), 
                                            4 * small_space_x, 4 * small_space_y, 17280, 11520);
    }

  } else {
    /* Horizontal line in inductor */
    m_refPixmap->draw_line(m_refGC, x, y + half_space_y, x + 2 * small_space_x, y + half_space_y);
    m_refPixmap->draw_line(m_refGC, x + width - 2 * small_space_x, y + half_space_y, x + width, y + half_space_y);
  
    /* Arcs in inductor */
    for (int i = 0; i <= 12; i += 4) {
      m_refPixmap->draw_arc(m_refGC, false, x + 2 * small_space_x + i * small_space_x, y + half_space_y - (2 * small_space_y), 
                                            4 * small_space_x, 4 * small_space_y, 0, 11520);
    }
  }
}

void CrossoverImageView::draw_resistor(int id, int x, int y, int width, int height, bool rotate)
{
  int half_space_y  = GSpeakers::round(height / 2);
  int half_space_x  = GSpeakers::round(width / 2);
  int small_space_x = GSpeakers::round(width / 20);
  int small_space_y = GSpeakers::round(height / 20);

  m_refLayout->set_text("R" + GSpeakers::int_to_ustring(id));
  m_refPixmap->draw_layout(m_refGC, x, y, m_refLayout);

  if (rotate == true) {
    /* Horizontal line in resistor */
    m_refPixmap->draw_line(m_refGC, x + half_space_x, y, x + half_space_x, y + 3 * small_space_y);
    m_refPixmap->draw_line(m_refGC, x + half_space_x, y + height - 3 * small_space_y, x + half_space_x, y + height);

    /* Draw rectangular box in resistor */
    m_refPixmap->draw_rectangle(m_refGC, false, x + half_space_x - 2 * small_space_x, y + 3 * small_space_y, 
                                                4 * small_space_x, height - 6 * small_space_y);

  } else {
    /* Horizontal line in resistor */
    m_refPixmap->draw_line(m_refGC, x, y + half_space_y, x + (3 * small_space_x), y + half_space_y);
    m_refPixmap->draw_line(m_refGC, x + width - (3 * small_space_x), y + half_space_y, x + width, y + half_space_y);

    /* Draw rectangular box in resistor */
    m_refPixmap->draw_rectangle(m_refGC, false, x + 3 * small_space_x, y + half_space_y - 2 * small_space_y, 
                                                width - 6 * small_space_x, 4 * small_space_y);
  
  }
}

void CrossoverImageView::draw_connector(int x, int y, int width, int height, bool positive)
{
  int half_space_x  = GSpeakers::round(width / 2);
  int half_space_y  = GSpeakers::round(height / 2);
  int small_space_x = GSpeakers::round(width / 20);
  int small_space_y = GSpeakers::round(height / 20);

  if (positive == true) {
    m_refLayout->set_text("+");
  } else {
    m_refLayout->set_text("-");
  }
  m_refPixmap->draw_layout(m_refGC, x + GSpeakers::round(half_space_x / 2),  y + GSpeakers::round(half_space_y / 2), m_refLayout);
        
  /* Draw a "connector" cricle */
  m_refPixmap->draw_arc(m_refGC, false, x + half_space_x - 2 * small_space_x, y + half_space_y - 2 * small_space_y, 
                                        4 * small_space_x, 4 * small_space_y, 0, 23040);
  /* Draw line from connector to edge of this component */
  m_refPixmap->draw_line(m_refGC, x + half_space_x + 2 * small_space_x, y + half_space_y, x + width, y + half_space_y);
}

void CrossoverImageView::draw_t_cross(int x, int y, int width, int height, bool upper)
{
  int half_space_x  = GSpeakers::round(width / 2);
  int half_space_y  = GSpeakers::round(height / 2);
  
  m_refPixmap->draw_line(m_refGC, x, y + half_space_y, x + width, y + half_space_y);
  
  if (upper == true) {
    m_refPixmap->draw_line(m_refGC, x + half_space_x, y + half_space_y, x + half_space_x, y + height);
  } else {
    m_refPixmap->draw_line(m_refGC, x + half_space_x, y, x + half_space_x, y + half_space_y);
  }
}

void CrossoverImageView::draw_corner(int x, int y, int width, int height, bool upper)
{
  int half_space_x  = GSpeakers::round(width / 2);
  int half_space_y  = GSpeakers::round(height / 2);
  
  m_refPixmap->draw_line(m_refGC, x, y + half_space_y, x + half_space_x, y + half_space_y);
  
  if (upper == true) {
    m_refPixmap->draw_line(m_refGC, x + half_space_y, y + half_space_y, x + half_space_x, y + height);
  } else {
    m_refPixmap->draw_line(m_refGC, x + half_space_x, y, x + half_space_x, y + half_space_y);
  }
}

void CrossoverImageView::draw_line(int x, int y, int width, int height, bool rotate)
{
  if (rotate == true) {
    int half_space_x  = GSpeakers::round(width / 2);
    m_refPixmap->draw_line(m_refGC, x + half_space_x, y, x + half_space_x, y + height);
  } else {
    int half_space_y  = GSpeakers::round(height / 2);
    m_refPixmap->draw_line(m_refGC, x, y + half_space_y, x + width, y + half_space_y);
  }
}

void CrossoverImageView::draw_woofer(int x, int y, int width, int height, bool positive_up)
{
  int half_space_x  = GSpeakers::round(width / 2);
  int half_space_y  = GSpeakers::round(height / 2);
  int small_space_x = GSpeakers::round(width / 20);
  int small_space_y = GSpeakers::round(height / 20);
  
  if (positive_up == true) {
    m_refLayout->set_text("+");
    m_refPixmap->draw_layout(m_refGC, x + 3 * small_space_x, y + small_space_y, m_refLayout);
  } else {
    m_refLayout->set_text("-");
    m_refPixmap->draw_layout(m_refGC, x + 3 * small_space_x, y + small_space_y, m_refLayout);
  }
    
  /* Draw a vertical line through the driver */
  m_refPixmap->draw_line(m_refGC, x + half_space_x, y, x + half_space_x, y + width);
  /* Draw the actual woofer */
  m_refPixmap->draw_rectangle(m_refGC, false, x + 6 * small_space_x, y + GSpeakers::round(width / 4), 
                                              4 * small_space_x, half_space_y);
  m_refPixmap->draw_line(m_refGC, x + half_space_x, y + half_space_y - 4 * small_space_y, 
                                  x + width - 4 * small_space_x, y);
  m_refPixmap->draw_line(m_refGC, x + half_space_x, y + half_space_y + 4 * small_space_y, 
                                  x + width - 4 * small_space_x, y + height);
  m_refPixmap->draw_line(m_refGC, x + width - 4 * small_space_x, y,
                                  x + width - 4 * small_space_x, y + height);


}

void CrossoverImageView::draw_midrange(int x, int y, int width, int height, bool positive_up)
{
  int half_space_x  = GSpeakers::round(width / 2);
  int half_space_y  = GSpeakers::round(height / 2);
  int small_space_x = GSpeakers::round(width / 20);
  int small_space_y = GSpeakers::round(height / 20);
  
  if (positive_up == true) {
    m_refLayout->set_text("+");
    m_refPixmap->draw_layout(m_refGC, x + 5 * small_space_x, y + 3 * small_space_y, m_refLayout);
  } else {
    m_refLayout->set_text("-");
    m_refPixmap->draw_layout(m_refGC, x + 5 * small_space_x, y + 3 * small_space_y, m_refLayout);
  }
    
  /* Draw a vertical line through the driver */
  m_refPixmap->draw_line(m_refGC, x + half_space_x, y, x + half_space_x, y + width);
  /* Draw the actual woofer */
  m_refPixmap->draw_rectangle(m_refGC, false, x + half_space_x - 2 * small_space_x, y + 7 * small_space_y, 
                                              2 * small_space_x, 6 * small_space_y);
  m_refPixmap->draw_line(m_refGC, x + half_space_x, y + half_space_y - 2 * small_space_y, 
                                  x + half_space_x + 4 * small_space_x, y + 2 * small_space_y);
  m_refPixmap->draw_line(m_refGC, x + half_space_x, y + half_space_y + 2 * small_space_y, 
                                  x + half_space_x + 4 * small_space_x, y + height - 2 * small_space_y);
  m_refPixmap->draw_line(m_refGC, x + width - 6 * small_space_x, y + 2 * small_space_y,
                                  x + width - 6 * small_space_x, y + height - 2 * small_space_y);
    
}

void CrossoverImageView::draw_tweeter(int x, int y, int width, int height, bool positive_up)
{
  int half_space_x  = GSpeakers::round(width / 2);
  int half_space_y  = GSpeakers::round(height / 2);
  int small_space_x = GSpeakers::round(width / 20);
  int small_space_y = GSpeakers::round(height / 20);
  
  if (positive_up == true) {
    m_refLayout->set_text("+");
    m_refPixmap->draw_layout(m_refGC, x + 5 * small_space_x, y + 3 * small_space_y, m_refLayout);
  } else {
    m_refLayout->set_text("-");
    m_refPixmap->draw_layout(m_refGC, x + 5 * small_space_x, y + 3 * small_space_y, m_refLayout);
  }

  /* Draw a vertical line through the driver */
  m_refPixmap->draw_line(m_refGC, x + half_space_x, y, x + half_space_x, y + width);
  /* Draw the actual woofer */
  m_refPixmap->draw_rectangle(m_refGC, false, x + half_space_x, y + 7 * small_space_y, 
                                              2 *  small_space_x, 6 * small_space_y);

  m_refPixmap->draw_arc(m_refGC, false, x + half_space_x + small_space_x, y + half_space_y - small_space_y, 
                                        2 * small_space_x, 2 * small_space_y, 17280, 11520);
  m_refPixmap->draw_line(m_refGC, x + half_space_x + 2 * small_space_x, y + half_space_y - 5 * small_space_y, 
                                  x + half_space_x + 2 * small_space_x, y + half_space_y + 5 * small_space_y);                    
}
