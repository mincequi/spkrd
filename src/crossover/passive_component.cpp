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

#include <libxml/parser.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <utility>

passive_component::passive_component(int type, double value, std::string unit) : GSpeakersObject()
{
    m_type = type;
    m_value = value;
    m_unit = std::move(unit);
}

passive_component::passive_component(xmlNodePtr parent) : GSpeakersObject()
{
    if (parent != nullptr && std::string((char*)parent->name) == "part")
    {
        try
        {
            parse_type(parent->children);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("passive_component: part node not found"));
    }
}

void passive_component::parse_type(xmlNodePtr node)
{
    if ((node != nullptr) && (std::string((char*)node->name) == std::string("type")))
    {
        std::istringstream((char*)xmlNodeGetContent(node)) >> m_type;
        try
        {
            parse_value(node->next);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("passive_component: type node not found"));
    }
}

void passive_component::parse_value(xmlNodePtr node)
{
    if ((node != nullptr) && (std::string((char*)node->name) == std::string("value")))
    {
        std::istringstream((char*)xmlNodeGetContent(node)) >> m_value;
        try
        {
            parse_unit(node->next);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("passive_component: value node not found"));
    }
}

void passive_component::parse_unit(xmlNodePtr node)
{
    if ((node != nullptr) && (std::string((char*)node->name) == std::string("unit")))
    {
        m_unit = std::string((char*)xmlNodeGetContent(node));
    }
    else
    {
        throw std::runtime_error(_("passive_component: unit node not found"));
    }
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
