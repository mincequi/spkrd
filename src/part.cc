/*
  part Copyright (C) 2002 Daniel Sundberg

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

#include "part.h"
#include "common.h"
#include <cstdlib>
#include <glib/gstrfuncs.h>
#include <iostream>
#include <sigc++/sigc++.h>
#include <sstream>
#include <utility>

Part::Part(int type, double value, std::string unit) : GSpeakersObject() {
  m_type = type;
  m_value = value;
  m_unit = std::move(unit);
}

Part::Part(xmlNodePtr parent) : GSpeakersObject() {
  if ((parent != nullptr) && (std::string((char*)parent->name) == std::string("part"))) {
    try {
      parse_type(parent->children);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Part: part node not found"));
  }
}

void Part::parse_type(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("type"))) {
    std::istringstream((char*)xmlNodeGetContent(node)) >> m_type;
    try {
      parse_value(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Part: type node not found"));
  }
}

void Part::parse_value(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("value"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_value;
    m_value = g_ascii_strtod((gchar*)xmlNodeGetContent(node), nullptr);
    try {
      parse_unit(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Part: value node not found"));
  }
}

void Part::parse_unit(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("unit"))) {
    m_unit = std::string((char*)xmlNodeGetContent(node));
  } else {
    throw GSpeakersException(_("Part: unit node not found"));
  }
}

xmlNodePtr Part::to_xml_node(xmlNodePtr parent) {
  xmlNodePtr part, field;
  auto* buffer = new gchar[8];

  part = xmlNewChild(parent, nullptr, (xmlChar*)("part"), nullptr);
  field = xmlNewChild(part, nullptr, (xmlChar*)("type"), nullptr);
  xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_type));
  field = xmlNewChild(part, nullptr, (xmlChar*)("value"), nullptr);
  xmlNodeSetContent(field, (xmlChar*)g_ascii_dtostr(buffer, 8, m_value));
  field = xmlNewChild(part, nullptr, (xmlChar*)("unit"), nullptr);
  xmlNodeSetContent(field, (xmlChar*)(m_unit.c_str()));

  return part;
}

void Part::set_value(double value) {

  m_value = value;
  // std::cout << "Part.id: " << get_id() << "...value set to " << m_value << std::endl;
}

void Part::set_unit(std::string unit) { m_unit = std::move(unit); }

double Part::get_value() { return m_value; }

std::string Part::get_unit() { return m_unit; }

void Part::on_part_value_changed(int id, double new_value) {
  if (id == get_id()) {
    // std::cout << "Signal emitted in: " << get_id() << std::endl;
    m_value = new_value;
  }
}

void Part::on_part_unit_changed(int id, std::string new_unit) {
  if (id == get_id()) {
    // std::cout << "Signal emitted in: " << get_id() << std::endl;
    m_unit = std::move(new_unit);
  }
}

void Part::on_part_type_changed(int id, int new_type) {
  if (id == get_id()) {
    // std::cout << "Signal emitted in: " << get_id() << std::endl;
    m_type = new_type;
  }
}

std::ostream& operator<<(std::ostream& o, const Part& part) {
  return o << _("***Part***") << std::endl
           << _("Id   : ") << part.m_id << std::endl
           << _("Type : ") << part.m_type << std::endl
           << _("Value: ") << part.m_value << std::endl
           << _("Unit : ") << part.m_unit << std::endl;
}
