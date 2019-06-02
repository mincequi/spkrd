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

#include <glib.h>

BoxList::BoxList(const std::string& filename) {
  xmlNodePtr children;

  xmlDocPtr doc = xmlParseFile(filename.c_str());
  if (doc != nullptr) {
    xmlNodePtr node = xmlDocGetRootElement(doc);
    if ((node != nullptr) && (g_ascii_strcasecmp((char*)node->name, "boxlist") == 0)) {
      if (node->children != nullptr) {
        children = node->children;
        while (children != nullptr) {
          try {
            m_box_list.emplace_back(children);
          } catch (GSpeakersException const& e) {
            throw e;
          }
          children = children->next;
        }
      }
    } else {
      throw GSpeakersException(_("BoxList: boxlist node not found"));
    }
  } else {
    throw GSpeakersException(_("BoxList: Xml file not found"));
  }
}

void BoxList::to_xml(const std::string& filename) {

  xmlDocPtr doc = xmlNewDoc((xmlChar*)("1.0"));

  xmlNodePtr node = xmlNewDocNode(doc, nullptr, (xmlChar*)("boxlist"), nullptr);
  xmlDocSetRootElement(doc, node);

  /* Iterate through all boxes */
  for (auto& from : m_box_list) {
    from.to_xml_node(node);
  }

  /* Save xml file */
  if (xmlSaveFile(filename.c_str(), doc) == -1) {
    throw GSpeakersException(_("BoxList: Could not save to ") + filename);
  }
}

std::ostream& operator<<(std::ostream& o, const BoxList& box_list) {
  o << "Box List\n";

  for (const auto& from : box_list.m_box_list) {
    o << from;
  }
  return o;
}

std::vector<Box>& BoxList::box_list() { return m_box_list; }

std::vector<Box> const& BoxList::box_list() const { return m_box_list; }

void BoxList::clear() { m_box_list.erase(m_box_list.begin(), m_box_list.end()); }
