/*
  $Id$

  cellitemcrossover Copyright (C) 2002 Daniel Sundberg

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

#include "cellitemcrossover.h"

CellItem_Crossover::CellItem_Crossover()
{
  m_label = "";
  m_type  = 0;
  m_value = 0.0;
  m_unit  = "";
  m_id    = 0;
  m_value_str = "";
  m_type_str = "";
}

CellItem_Crossover::CellItem_Crossover(Glib::ustring label, int type, double value, Glib::ustring unit, int id)
{
  m_label = label;
  m_type  = type;
  m_value = value;
  m_unit  = unit;
  m_id    = id;
  m_value_str = "";
  m_type_str = "";
}

CellItem_Crossover::CellItem_Crossover(Part part)
{
  int id = part.get_id();
  m_id = part.get_id();
  m_type = part.get_type();
  m_value = part.get_value();
  m_unit = part.get_unit();
  char *str = NULL;
  GString *buffer = g_string_new(str);
    
  switch (m_type) {
    case PART_TYPE_CAPACITOR:
      g_string_printf(buffer, "C%d", id);
      m_unit = m_unit + "F";
      break;
    case PART_TYPE_INDUCTOR:
      g_string_printf(buffer, "L%d", id);
      m_unit = m_unit + "H";
      break;
    case PART_TYPE_RESISTOR:
      g_string_printf(buffer, "R%d", id);
      m_unit = m_unit + "Ohm";
      break;
    default:
      g_string_printf(buffer, "%d", id);
      break;
  }
  m_label = Glib::ustring(buffer->str);
}


CellItem_Crossover::CellItem_Crossover(Glib::ustring label, const std::vector<CellItem_Crossover>& children)
{
  m_label    = label;
  m_children = children;
  m_type     = 0;
  m_value    = 0;
  m_unit     = "";
  m_id       = 0;
}

CellItem_Crossover::CellItem_Crossover(const CellItem_Crossover& src)
{
  operator=(src);
}

CellItem_Crossover::~CellItem_Crossover()
{
}

CellItem_Crossover& CellItem_Crossover::operator=(const CellItem_Crossover& src)
{
  m_label    = src.m_label;
  m_type     = src.m_type;
  m_value    = src.m_value;
  m_unit     = src.m_unit;
  m_children = src.m_children;
  m_id       = src.m_id;
  m_type_str = src.m_type_str;
  m_value_str = src.m_value_str;

  return *this;
}
