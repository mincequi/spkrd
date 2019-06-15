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

#include <utility>

CellItem_Crossover::CellItem_Crossover(Glib::ustring label,
                                       int type,
                                       double value,
                                       Glib::ustring unit,
                                       int id)
    : m_label(std::move(label)), m_id(id), m_type(type), m_value(value), m_unit(std::move(unit))
{
}

CellItem_Crossover::CellItem_Crossover(Part const& part)
    : m_id(part.get_id()), m_type(part.get_type()), m_value(part.get_value()), m_unit(part.get_unit())
{
    char* str = nullptr;
    GString* buffer = g_string_new(str);

    switch (m_type)
    {
        case PART_TYPE_CAPACITOR:
            g_string_printf(buffer, "C%d", m_id);
            m_unit = m_unit + "F";
            break;
        case PART_TYPE_INDUCTOR:
            g_string_printf(buffer, "L%d", m_id);
            m_unit = m_unit + "H";
            break;
        case PART_TYPE_RESISTOR:
            g_string_printf(buffer, "R%d", m_id);
            m_unit = m_unit + "Ohm";
            break;
        default:
            g_string_printf(buffer, "%d", m_id);
            break;
    }
    m_label = Glib::ustring(buffer->str);
}

CellItem_Crossover::CellItem_Crossover(Glib::ustring label,
                                       const std::vector<CellItem_Crossover>& children)
    : m_label(std::move(label)), m_children(children)
{
}
