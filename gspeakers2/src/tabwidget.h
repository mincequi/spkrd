/*
  $Id$
  
  tabwidget Copyright (C) 2002 Daniel Sundberg

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

#ifndef __GSPEAKERS_TABWIDGET_H
#define __GSPEAKERS_TABWIDGET_H

#include <gtkmm.h>
#include "common.h"

using namespace std;

class TabWidget : public Gtk::HBox
{
public:
  TabWidget(string filename, string text);
private:
  Gtk::Image *image;
};

#endif
