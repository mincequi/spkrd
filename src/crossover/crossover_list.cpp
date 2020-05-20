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

#include "crossover_list.hpp"

#include <libxml/parser.h>
#include <libxml/tree.h>

namespace spkrd
{
crossover_list::crossover_list(const std::string& filename)
{
    xmlKeepBlanksDefault(0);

    xmlDocPtr doc = xmlParseFile(filename.c_str());

    if (doc == nullptr)
    {
        throw std::runtime_error(_("crossover_list: Xml file not found"));
    }

    xmlNodePtr node = xmlDocGetRootElement(doc);

    if (node != nullptr && g_ascii_strcasecmp((char*)node->name, "crossoverlist") == 0)
    {
        if (node->children != nullptr)
        {
            xmlNodePtr children = node->children;
            while (children != nullptr)
            {
                try
                {
                    m_data.emplace_back(children);
                }
                catch (std::runtime_error const& error)
                {
                    throw error;
                }
                children = children->next;
            }
        }
    }
    else
    {
        throw std::runtime_error(_("crossover_list: crossoverlist node not found"));
    }
}

void crossover_list::to_xml(std::string const& filename)
{
    xmlDocPtr doc = xmlNewDoc((xmlChar*)("1.0"));

    xmlNodePtr node = xmlNewDocNode(doc, nullptr, (xmlChar*)("crossoverlist"), nullptr);
    xmlDocSetRootElement(doc, node);

    for (auto& from : m_data)
    {
        from.to_xml_node(node);
    }

    if (xmlSaveFile(filename.c_str(), doc) == -1)
    {
        throw std::runtime_error(_("crossover_list: Could not save to ") + filename);
    }
}

auto operator<<(std::ostream& output, crossover_list const& crossover_list) -> std::ostream&
{
    output << "Crossover List\n";

    for (const auto& from : crossover_list.m_data)
    {
        output << from;
    }
    return output;
}
}
