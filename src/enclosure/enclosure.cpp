/*
  $Id$

  box Copyright (C) 2002 Daniel Sundberg

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

#include "enclosure.hpp"

#include "common.h"
#include "xml_parser.hpp"

#include <sstream>
#include <utility>

using namespace spkrd;

enclosure::enclosure(std::string id_string,
                     int type,
                     double vb1,
                     double fb1,
                     double vb2,
                     double fb2,
                     std::string const& driver)
    : GSpeakersObject(),
      m_id_string(std::move(id_string)),
      m_vb1(vb1),
      m_fb1(fb1),
      m_vb2(vb2),
      m_fb2(fb2),
      m_driver(driver)
{
    m_type = type;
}

enclosure::enclosure(xmlNodePtr parent) : GSpeakersObject()
{
    if (parent != nullptr && std::string((char*)parent->name) == "box")
    {
        try
        {
            // parse_id_string(parent->children);

            auto node = parent->children;

            m_id_string = parse_string(node, "id_string");
            node = node->next;

            m_type = parse_int(node, "type");
            node = node->next;

            m_vb1 = parse_double(node, "vb1");
            node = node->next;

            m_fb1 = parse_double(node, "fb1");
            node = node->next;

            m_vb2 = parse_double(node, "vb2");
            node = node->next;

            m_fb2 = parse_double(node, "fb2");
            node = node->next;

            m_driver = parse_string(node, "driver");
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("enclosure: box node not found"));
    }
}

xmlNodePtr enclosure::to_xml_node(xmlNodePtr parent)
{
    std::array<char, 8> buffer;

    xmlNodePtr box = xmlNewChild(parent, nullptr, (xmlChar*)("box"), nullptr);

    xmlNodePtr child = xmlNewChild(box, nullptr, (xmlChar*)("id_string"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)m_id_string.c_str());
    child = xmlNewChild(box, nullptr, (xmlChar*)("type"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_strdup_printf("%d", m_type));
    child = xmlNewChild(box, nullptr, (xmlChar*)("vb1"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_vb1));
    child = xmlNewChild(box, nullptr, (xmlChar*)("fb1"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_fb1));
    child = xmlNewChild(box, nullptr, (xmlChar*)("vb2"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_vb2));
    child = xmlNewChild(box, nullptr, (xmlChar*)("fb2"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_fb2));
    child = xmlNewChild(box, nullptr, (xmlChar*)("driver"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)m_driver.c_str());

    return box;
}

std::ostream& operator<<(std::ostream& output, const enclosure& box)
{
    output << _("enclosure") << "\n"
           << _("Id:        ") << box.m_id << "\n"
           << _("Id-string: ") << box.m_id_string << "\n"
           << _("Type:      ") << box.m_type << "\n"
           << _("Vb1:       ") << box.m_vb1 << "\n"
           << _("Fb1:       ") << box.m_fb1 << "\n"
           << _("driver:   ") << box.m_driver << "\n";
    return output;
}
