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

#include "boxlist.h"
#include "common.h"

#include <libxml/parser.h>

BoxList::BoxList(const std::string& filename)
{
    xmlKeepBlanksDefault(0);

    xmlDocPtr doc = xmlParseFile(filename.c_str());

    if (doc != nullptr)
    {
        xmlNodePtr node = xmlDocGetRootElement(doc);
        if (node != nullptr && std::string((char*)node->name) == "boxlist")
        {
            if (node->children != nullptr)
            {
                xmlNodePtr children = node->children;
                while (children != nullptr)
                {
                    try
                    {
                        m_box_list.emplace_back(children);
                    }
                    catch (std::runtime_error const& e)
                    {
                        throw e;
                    }
                    children = children->next;
                }
            }
        }
        else
        {
            throw std::runtime_error(_("BoxList: boxlist node not found"));
        }
    }
    else
    {
        throw std::runtime_error(_("BoxList: Xml file not found"));
    }
}

void BoxList::to_xml(std::string const& filename)
{
    xmlDocPtr doc = xmlNewDoc((xmlChar*)("1.0"));

    xmlNodePtr node = xmlNewDocNode(doc, nullptr, (xmlChar*)("boxlist"), nullptr);

    xmlDocSetRootElement(doc, node);

    for (auto& from : m_box_list)
    {
        from.to_xml_node(node);
    }

    // Save xml file
    if (xmlSaveFile(filename.c_str(), doc) == -1)
    {
        throw std::runtime_error(_("BoxList: Could not save to ") + filename);
    }
}

std::ostream& operator<<(std::ostream& output, const BoxList& box_list)
{
    output << "Box List\n";

    for (const auto& from : box_list.m_box_list)
    {
        output << from;
    }
    return output;
}
