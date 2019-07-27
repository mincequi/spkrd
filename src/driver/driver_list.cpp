/*
  speakerlist Copyright (C) 2002 Daniel Sundberg

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

#include "driver_list.hpp"

#include "common.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "nlohmann/json.hpp"

#include <fstream>

driver_list::driver_list(const std::string& file_name)
{
    if (file_name.empty())
    {
        return;
    }

    // Read in the json file containing the drivers
    nlohmann::json root;

    {
        std::ifstream file(file_name);

        if (!file.is_open())
        {
            throw std::runtime_error("File " + file_name + " was not found.  Please ensure it exists");
        }
        file >> root;
    }

    if (root.find("drivers") == end(root))
    {
        throw std::runtime_error("Input file for drivers was not valid");
    }

    for (auto const& driver : root["drivers"])
    {
        m_drivers.emplace_back(driver);
    }
}

void driver_list::to_xml(const std::string& file_name)
{
    xmlDocPtr doc = xmlNewDoc((xmlChar*)("1.0"));

    xmlNodePtr node = xmlNewDocNode(doc, nullptr, (xmlChar*)("speakerlist"), nullptr);
    xmlDocSetRootElement(doc, node);

    // Iterate through all speakers
    for (auto& from : m_drivers)
    {
        from.to_xml_node(node);
    }

    // Save xml file
    if (xmlSaveFile(file_name.c_str(), doc) == -1)
    {
        throw std::runtime_error(_("driver_list: Could not save to ") + file_name);
    }
}

std::ostream& operator<<(std::ostream& output, const driver_list& driver_list)
{
    output << _("driver List") << "\n";

    for (auto& from : driver_list.m_drivers)
    {
        output << from;
    }
    return output;
}

driver driver_list::get_by_id_string(std::string const& id_string)
{
    auto const location = std::find_if(begin(m_drivers),
                                       end(m_drivers),
                                       [&id_string](auto const& driver) {
                                           return driver.get_id_string() == id_string;
                                       });

    if (location == end(m_drivers))
    {
        throw std::runtime_error("The driver " + id_string
                                 + " was not found in the list of available drivers");
    }
    return location != end(m_drivers) ? *location : m_drivers.front();
}
