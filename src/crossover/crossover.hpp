/*
  crossover Copyright (C) 2002 Daniel Sundberg

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

#include "common.h"
#include "gspeakersobject.h"
#include "filter_network.hpp"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <iosfwd>
#include <vector>

// You should be able to use one or more of the type properties
constexpr auto CROSSOVER_TYPE_LOWPASS = 1;
constexpr auto CROSSOVER_TYPE_SUBSONIC = 2;
constexpr auto CROSSOVER_TYPE_HIGHPASS = 4;
constexpr auto CROSSOVER_TYPE_TWOWAY = 8;
// Two and a half way filter will be CROSSOVER_TYPE_LOWPASS | CROSSOVER_TYPE_TWOWAY
constexpr auto CROSSOVER_TYPE_THREEWAY = 16;
constexpr auto CROSSOVER_TYPE_FOURWAY = 32;

class Crossover : public GSpeakersObject
{
public:
    Crossover(int type = CROSSOVER_TYPE_TWOWAY, std::string id_string = "Crossover");

    /// Construct a part from an xml node
    Crossover(xmlNodePtr parent);

    /// Convert data for a part to an xml node
    /// \throw std::runtime_error on failure
    xmlNodePtr to_xml_node(xmlNodePtr parent) noexcept(false);

    /// Print part data to stdout
    friend std::ostream& operator<<(std::ostream& o, Crossover const& crossover);

    auto networks() -> std::vector<filter_network>& { return m_networks; }

    auto networks() const -> std::vector<filter_network> const& { return m_networks; }

    /// \return id_string for this crossover
    auto get_id_string() const -> std::string const&;

    void set_id_string(std::string id_string);

protected:
    /// Used to parse xml
    void parse_type(xmlNodePtr node);

    void parse_networks(xmlNodePtr node);

    void parse_id_string(xmlNodePtr node);

protected:
    std::vector<filter_network> m_networks;
    std::string m_id_string;
};
