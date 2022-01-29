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

#include "passive_component.hpp"

#include "common.h"
#include "xml_parser.hpp"

#include <array>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <utility>

passive_component::passive_component(int type, double value, std::string unit)
    : gspkObject(type), m_value{value}, m_unit{std::move(unit)}
{
}

passive_component::passive_component(xmlNodePtr parent) : gspkObject()
{
    gspk::check_name(parent, "part");

    auto* child = parent->children;

    // Check all fields exist
    for (auto const& label : {"type", "value", "unit"})
    {
        gspk::check_name(child, label);
        child = child->next;
    }

    child = parent->children;
    m_type = gspk::parse_int(child, "type");

    child = child->next;
    m_value = gspk::parse_double(child, "value");

    child = child->next;
    m_unit = gspk::parse_string(child, "unit");
}

auto passive_component::to_xml_node(xmlNodePtr parent) -> xmlNodePtr
{
    std::array<gchar, 8> buffer;

    xmlNodePtr part = xmlNewChild(parent, nullptr, (xmlChar*)("part"), nullptr);
    xmlNodePtr field = xmlNewChild(part, nullptr, (xmlChar*)("type"), nullptr);
    xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_type));
    field = xmlNewChild(part, nullptr, (xmlChar*)("value"), nullptr);
    xmlNodeSetContent(field, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_value));
    field = xmlNewChild(part, nullptr, (xmlChar*)("unit"), nullptr);
    xmlNodeSetContent(field, (xmlChar*)(m_unit.c_str()));

    return part;
}

void passive_component::set_value(double value) { m_value = value; }

void passive_component::set_unit(std::string unit) { m_unit = std::move(unit); }

auto passive_component::get_value() const -> double { return m_value; }

auto passive_component::get_unit() const -> std::string const& { return m_unit; }

void passive_component::on_part_value_changed(int id, double new_value)
{
    if (id == get_id())
    {
        std::cout << "Signal emitted in: " << get_id() << "\n";
        m_value = new_value;
    }
}

void passive_component::on_part_unit_changed(int id, std::string new_unit)
{
    if (id == get_id())
    {
        m_unit = std::move(new_unit);
    }
}

void passive_component::on_part_type_changed(int id, int new_type)
{
    if (id == get_id())
    {
        m_type = new_type;
    }
}

auto operator<<(std::ostream& o, const passive_component& part) -> std::ostream&
{
    return o << _("***passive_component***") << "\n"
             << _("Id   : ") << part.m_id << "\n"
             << _("Type : ") << part.m_type << "\n"
             << _("Value: ") << part.m_value << "\n"
             << _("Unit : ") << part.m_unit << "\n";
}
