/*
  GFilter (C) Daniel Sundberg 2002

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License version 2
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "gfilter.h"

#include <sigc++/slot.h>

#include <iostream>

GFilter::GFilter() : m_Button1(gettext("Click Me")), m_Button2(gettext("Click me also")) {
  set_title("gtkmm_hello");
  set_border_width(10);

  // Connect the button's "clicked" signal to the on_button_clicked() signal handler:
  m_Button1.signal_clicked().connect(sigc::slot(*this, &GFilter::on_button_clicked));

  add(m_Button1);
  show_all();
}

void GFilter::on_button_clicked() { std::cout << gettext("The button was clicked.") << std::endl; }
