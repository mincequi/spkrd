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

#include "gspeakerscolor.h"

GSpeakersColor::GSpeakersColor() {
  m_counter = 0;

  m_colors.push_back("black");   // 0, 0, 0
  m_colors.push_back("red");     // 255   0   0
  m_colors.push_back("blue");    // 0, 0, 255
  m_colors.push_back("magenta"); // 255   0 255
  m_colors.push_back("DarkSlateGrey");
  m_colors.push_back("goldenrod");      // 218, 165, 32
  m_colors.push_back("MidnightBlue");   // 25, 25, 112
  m_colors.push_back("SlateGrey");      // 112, 128, 144
  m_colors.push_back("DarkTurquoise");  // 0, 206, 209
  m_colors.push_back("CornflowerBlue"); // 100, 149, 237
  m_colors.push_back("SlateBlue");      // 106, 90, 205
  m_colors.push_back("SeaGreen");       // 46, 188, 143
  m_colors.push_back("yellow");         // 255, 255, 0
  m_colors.push_back("RoyalBlue");      // 65, 105, 225
  m_colors.push_back("SpringGreen");    // 0, 255, 127
  m_colors.push_back("DimGrey");        // 105, 105, 105
  m_colors.push_back("HotPink");        // 255 105 180
  m_colors.push_back("DeepSkyBlue");    // 0, 191, 255
  m_colors.push_back("ForestGreen");    // 34, 139, 34
  m_colors.push_back("PaleVioletRed");  // 219 112 147
  m_colors.push_back("CadetBlue");      // 95, 158, 160
  m_colors.push_back("IndianRed");      // 205, 92, 92
  m_colors.push_back("orange");         // 255, 165,   0
  m_colors.push_back("green");
  m_colors.push_back("MediumPurple");  // 147 112 219
  m_colors.push_back("DarkGreen");     // 0, 100, 0
  m_colors.push_back("RosyBrown");     // 188, 143, 143
  m_colors.push_back("LightSeaGreen"); // 32, 178, 170
  m_colors.push_back("SaddleBrown");   // 139, 69, 19
  m_colors.push_back("VioletRed");     // 208  32 144
  m_colors.push_back("SandyBrown");    // 244, 164, 96
  m_colors.push_back("chocolate");     // 210, 105,  30
  m_colors.push_back("firebrick");     // 178, 34,  34
  m_colors.push_back("gold");          // 255, 215, 0
  m_colors.push_back("DarkViolet");    // 148   0 211
  m_colors.push_back("DarkSalmon");    // 233, 150, 122
  m_colors.push_back("coral");         // 255 127  80
}

GSpeakersColor::~GSpeakersColor() {}

std::string GSpeakersColor::get_color_string() {
 std::string s = m_colors[m_counter];
  m_counter = (m_counter + 1) % m_colors.size();
  return s;
}

void GSpeakersColor::unget_color_string(string s) {
  m_colors.erase(get_iterator_from_string(s));
  m_colors.insert(m_colors.begin() + m_counter, s);
}

std::vector<string>::iterator GSpeakersColor::get_iterator_from_string(string s) {
  for (vector<string>::iterator from = m_colors.begin(); from != m_colors.end(); ++from) {
    if (string(*from) == s) {
      return from;
    }
  }
  return m_colors.begin();
}
