/*
  gspeakerscolor Copyright (C) 2002 Daniel Sundberg

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

#ifndef __GSPEAKERS_COLOR
#define __GSPEAKERS_COLOR

#include <gtkmm.h>
#include <string>

class GSpeakersColor {
public:
  GSpeakersColor();

  std::string get_color_string();
  void unget_color_string(std::string s);

private:
  std::vector<std::string>::iterator get_iterator_from_string(std::string s);

private:
  std::vector<std::string> m_colors;
  int m_counter = 0;
};

#endif
