/*
  $Id$

  boxlist Copyright (C) 2002 Daniel Sundberg

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

#include "enclosure.hpp"

#include <libxml/tree.h>

#include <iosfwd>
#include <string>
#include <vector>

/// This is a class that handles a list of boxes
class enclosure_list
{
public:
    enclosure_list() = default;

    /// Construct a part from an xml file
    enclosure_list(const std::string& filename);

    /// Convert data for a part to an xml node, throws std::runtime_error on failure
    void to_xml(const std::string& filename);

    /// Print part data to stdout
    friend auto operator<<(std::ostream& o, const enclosure_list& box_list) -> std::ostream&;

    /// \return box_list vector
    auto box_list() -> std::vector<enclosure>& { return m_box_list; }

    /// \return box_list vector
    auto box_list() const -> std::vector<enclosure> const& { return m_box_list; }

    /// Clear all items in the list
    void clear() { m_box_list.clear(); }

protected:
    std::vector<enclosure> m_box_list;
};
