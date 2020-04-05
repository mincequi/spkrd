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

#pragma once

#include "passive_component.hpp"

#include <glibmm/ustring.h>

#include <vector>

/// This is a cellitem for a crossover used in the current crossover treeview
/// crossover_tree_view syncs this class with the underlying data container (the Crossover class)
class CellItem_Crossover
{
public:
    CellItem_Crossover() = default;

    CellItem_Crossover(passive_component const& part);

    CellItem_Crossover(Glib::ustring label, int type, double value, Glib::ustring unit, int id);

    /// Construct a cellitem with children
    CellItem_Crossover(Glib::ustring label, std::vector<CellItem_Crossover> const& children);

public:
    Glib::ustring m_label;

    int m_id = 0;
    int m_type = 0;
    double m_value = 0.0;

    Glib::ustring m_unit;
    Glib::ustring m_type_str;
    Glib::ustring m_value_str;

    std::vector<CellItem_Crossover> m_children;
};
