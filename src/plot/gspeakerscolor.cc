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

#include <algorithm>

GSpeakersColor::GSpeakersColor()
{
    m_colors.emplace_back("black");   // 0, 0, 0
    m_colors.emplace_back("red");     // 255   0   0
    m_colors.emplace_back("blue");    // 0, 0, 255
    m_colors.emplace_back("magenta"); // 255   0 255
    m_colors.emplace_back("DarkSlateGrey");
    m_colors.emplace_back("goldenrod");      // 218, 165, 32
    m_colors.emplace_back("MidnightBlue");   // 25, 25, 112
    m_colors.emplace_back("SlateGrey");      // 112, 128, 144
    m_colors.emplace_back("DarkTurquoise");  // 0, 206, 209
    m_colors.emplace_back("CornflowerBlue"); // 100, 149, 237
    m_colors.emplace_back("SlateBlue");      // 106, 90, 205
    m_colors.emplace_back("SeaGreen");       // 46, 188, 143
    m_colors.emplace_back("yellow");         // 255, 255, 0
    m_colors.emplace_back("RoyalBlue");      // 65, 105, 225
    m_colors.emplace_back("SpringGreen");    // 0, 255, 127
    m_colors.emplace_back("DimGrey");        // 105, 105, 105
    m_colors.emplace_back("HotPink");        // 255 105 180
    m_colors.emplace_back("DeepSkyBlue");    // 0, 191, 255
    m_colors.emplace_back("ForestGreen");    // 34, 139, 34
    m_colors.emplace_back("PaleVioletRed");  // 219 112 147
    m_colors.emplace_back("CadetBlue");      // 95, 158, 160
    m_colors.emplace_back("IndianRed");      // 205, 92, 92
    m_colors.emplace_back("orange");         // 255, 165,   0
    m_colors.emplace_back("green");
    m_colors.emplace_back("MediumPurple");  // 147 112 219
    m_colors.emplace_back("DarkGreen");     // 0, 100, 0
    m_colors.emplace_back("RosyBrown");     // 188, 143, 143
    m_colors.emplace_back("LightSeaGreen"); // 32, 178, 170
    m_colors.emplace_back("SaddleBrown");   // 139, 69, 19
    m_colors.emplace_back("VioletRed");     // 208  32 144
    m_colors.emplace_back("SandyBrown");    // 244, 164, 96
    m_colors.emplace_back("chocolate");     // 210, 105,  30
    m_colors.emplace_back("firebrick");     // 178, 34,  34
    m_colors.emplace_back("gold");          // 255, 215, 0
    m_colors.emplace_back("DarkViolet");    // 148   0 211
    m_colors.emplace_back("DarkSalmon");    // 233, 150, 122
    m_colors.emplace_back("coral");         // 255 127  80
}

std::string const& GSpeakersColor::get_color_string()
{
    std::string const& colour_value = m_colors[m_counter];
    m_counter = (m_counter + 1) % m_colors.size();
    return colour_value;
}

void GSpeakersColor::unget_color_string(const std::string& colour_value)
{
    m_colors.erase(get_iterator_from_string(colour_value));
    m_colors.insert(m_colors.begin() + m_counter, colour_value);
}

std::vector<std::string>::iterator GSpeakersColor::get_iterator_from_string(const std::string& colour_value)
{
    auto const location = std::find(begin(m_colors), end(m_colors), colour_value);
    return location == end(m_colors) ? location : begin(m_colors);
}
