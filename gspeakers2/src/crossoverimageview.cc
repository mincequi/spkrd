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
#include "crossoverimageview.h"
#include "common.h"

CrossoverImageView::CrossoverImageView()
{
  using SigC::slot;
  
  visible = false;
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
  /* Clear to white background color */
  m_refGC->set_rgb_fg_color(white);
  m_refPixmap->draw_rectangle(m_refGC, true, 0, 0, get_allocation().width, get_allocation().height);
  m_refGC->set_rgb_fg_color(black);

  draw_capacitor(13, 10, 10, 80, 80, false);
}

void CrossoverImageView::on_crossover_selected(Crossover *selected_crossover)
{
  cout << "CrossoverImageView::on_crossover_selected" << endl;
}

void CrossoverImageView::draw_capacitor(int id, int x, int y, int width, int height, bool rotate)
{
  if (visible == true) {
    int half_space_y  = GSpeakers::round(height / 2);
    int half_space_x  = GSpeakers::round(width / 2);
    int small_space_x = GSpeakers::round(width / 20);
    int small_space_y = GSpeakers::round(height / 20);
  
    m_refLayout->set_text("C" + GSpeakers::int_to_ustring(id));
    m_refPixmap->draw_layout(m_refGC, x, y, m_refLayout);
    
    if (rotate == true) {
      /* Horizontal line in capacitor */
      m_refPixmap->draw_line(m_refGC, x + half_space_y, y, x + half_space_y, y + half_space_y - small_space_y);
      m_refPixmap->draw_line(m_refGC, x + half_space_y, y + half_space_y + small_space_y, x + half_space_y, y + height);
    
      /* Vertical lines in capacitor */
      m_refPixmap->draw_line(m_refGC, x + small_space_x, y + half_space_y - small_space_y,
                                      x + width - small_space_x, y + half_space_y - small_space_y);
      m_refPixmap->draw_line(m_refGC, x + small_space_x, y + half_space_y + small_space_y,
                                      x + width - small_space_x, y + half_space_y + small_space_y);
    } else {
      /* Horizontal line in capacitor */
      m_refPixmap->draw_line(m_refGC, x, y + half_space_y, x + half_space_x - small_space_x, y + half_space_y);
      m_refPixmap->draw_line(m_refGC, x + half_space_x + small_space_x, y + half_space_y, x + width, y + half_space_y);
    
      /* Vertical lines in capacitor */
      m_refPixmap->draw_line(m_refGC, x + half_space_x - small_space_x, y + small_space_y, 
                                      x + half_space_x - small_space_x, y + height - small_space_y);
      m_refPixmap->draw_line(m_refGC, x + half_space_x + small_space_x, y + small_space_y, 
                                      x + half_space_x + small_space_x, y + height - small_space_y);
    }
  }
}
