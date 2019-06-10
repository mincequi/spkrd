/*
  crossoverlist Copyright (C) 2002 Daniel Sundberg

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

#ifndef __GFILTER_CROSSOVERLIST_H
#define __GFILTER_CROSSOVERLIST_H

#include "common.h"
#include "crossover.h"

#include <iosfwd>
#include <string>
#include <vector>

/// This is a class that pretty much just contains a list of crossovers
/// and xml output functionality
class CrossoverList {
public:
  CrossoverList() = default;

  // Construct a part from an xml node
  CrossoverList(const std::string& filename);

  // Convert data for a part to an xml node, throws std::runtime_error on failure
  // Maybe this one should throw an exception?
  void to_xml(const std::string& filename);

  // Print part data to stdout
  friend std::ostream& operator<<(std::ostream& o, const CrossoverList& crossover_list);

  std::vector<Crossover>& crossover_list() { return m_crossover_list; }

  std::vector<Crossover> const& crossover_list() const { return m_crossover_list; }

  // Remove all items from the corssover list
  void clear();

protected:
  /* Member variables */
  std::vector<Crossover> m_crossover_list;
};

#endif
