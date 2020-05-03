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

driver_list::driver_list(const std::string& filename)
{
    // Allows human readable (formatted) XML documents without receiving
    // whitespace errors
    xmlKeepBlanksDefault(0);

    xmlDocPtr doc = xmlParseFile(filename.c_str());

    if (doc == nullptr)
    {
        throw std::runtime_error(_("driver_list: XML file not found"));
    }

    xmlNodePtr node = xmlDocGetRootElement(doc);

    if (node != nullptr && std::string((char*)node->name) == "speakerlist")
    {
        if (node->children != nullptr)
        {
            xmlNodePtr children = node->children;

            while (children != nullptr)
            {
                m_drivers.emplace_back(children);

                children = children->next;
            }
        }
    }
    else
    {
        throw std::runtime_error(_("driver_list: speakerlist node not found"));
    }
}

void driver_list::to_xml(const std::string& filename)
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
    if (xmlSaveFile(filename.c_str(), doc) == -1)
    {
        throw std::runtime_error(_("driver_list: Could not save to ") + filename);
    }
}

auto operator<<(std::ostream& output, driver_list const& driver_list) -> std::ostream&
{
    output << _("driver List") << "\n";

    for (auto& from : driver_list.data())
    {
        output << from;
    }
    return output;
}

auto driver_list::get_by_id_string(std::string const& id_string) const -> driver
{
    auto const location = std::find_if(begin(m_drivers),
                                       end(m_drivers),
                                       [&id_string](auto const& speaker) {
                                           return speaker.get_id_string() == id_string;
                                       });

    if (location != end(m_drivers))
    {
        return *location;
    }

    if (!m_drivers.empty())
    {
        return m_drivers.front();
    }
    return driver();
}
