/*
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


#ifndef __GSPEAKERS_CELLITEM_CROSSOVER
#define __GSPEAKERS_CELLITEM_CROSSOVER

#include <gtkmm.h>
#include "part.h"

/*
 * This is a cellitem for a crossover used in the current crossover treeview
 *
 * CrossoverTreeView syncs this class with the underlying data container (the Crossover class)
 *
 */
class CellItem_Crossover
{
public:
  CellItem_Crossover();
  
  /* Construct a cellitem from a part */
  CellItem_Crossover(Part part);
  
  CellItem_Crossover(Glib::ustring label, int type, double value, Glib::ustring unit, int id);
  
  /* Use this constructor to construct a cellitem with children */
  CellItem_Crossover(Glib::ustring label, const std::vector<CellItem_Crossover>& children);
  
  CellItem_Crossover(const CellItem_Crossover& src);
  CellItem_Crossover& operator=(const CellItem_Crossover& src);
  ~CellItem_Crossover();
  
  /* Member variables */
  Glib::ustring m_label;
  int           m_id;
  int           m_type;
  double        m_value;
  Glib::ustring m_unit;
  std::vector<CellItem_Crossover> m_children;
};

#endif
