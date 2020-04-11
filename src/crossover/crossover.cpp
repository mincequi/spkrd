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

#include "crossover.hpp"

#include <sstream>
#include <utility>

Crossover::Crossover(int type, std::string id_string)
    : GSpeakersObject(type), m_id_string(std::move(id_string))
{
    if (m_type == CROSSOVER_TYPE_SUBSONIC)
    {
        m_networks.emplace_back(NET_TYPE_HIGHPASS, NET_NOT_PRESENT, NET_ORDER_1ST);
    }
    if (m_type == CROSSOVER_TYPE_LOWPASS)
    {
        m_networks.emplace_back(NET_TYPE_LOWPASS, NET_ORDER_1ST, NET_NOT_PRESENT);
    }
    if (m_type == CROSSOVER_TYPE_HIGHPASS)
    {
        m_networks.emplace_back(NET_TYPE_HIGHPASS, NET_NOT_PRESENT, NET_ORDER_1ST);
    }
    if (m_type == CROSSOVER_TYPE_TWOWAY)
    {
        m_networks.emplace_back(NET_TYPE_LOWPASS, NET_ORDER_1ST, NET_NOT_PRESENT);
        m_networks.emplace_back(NET_TYPE_HIGHPASS, NET_NOT_PRESENT, NET_ORDER_1ST);
    }
    if (m_type == CROSSOVER_TYPE_THREEWAY)
    {
        m_networks.emplace_back(NET_TYPE_LOWPASS, NET_ORDER_1ST, NET_NOT_PRESENT);
        m_networks.emplace_back(NET_TYPE_LOWPASS | NET_TYPE_HIGHPASS, NET_ORDER_1ST, NET_ORDER_1ST);
        m_networks.emplace_back(NET_TYPE_HIGHPASS, NET_NOT_PRESENT, NET_ORDER_1ST);
    }
    if (m_type == CROSSOVER_TYPE_FOURWAY)
    {
        m_networks.emplace_back(NET_TYPE_LOWPASS, NET_ORDER_1ST, NET_NOT_PRESENT);
        m_networks.emplace_back(NET_TYPE_LOWPASS | NET_TYPE_HIGHPASS, NET_ORDER_1ST, NET_ORDER_1ST);
        m_networks.emplace_back(NET_TYPE_LOWPASS | NET_TYPE_HIGHPASS, NET_ORDER_1ST, NET_ORDER_1ST);
        m_networks.emplace_back(NET_TYPE_HIGHPASS, NET_NOT_PRESENT, NET_ORDER_1ST);
    }
}

Crossover::Crossover(xmlNodePtr parent) : GSpeakersObject()
{
    if (parent != nullptr && std::string(reinterpret_cast<char const*>(parent->name)) == "crossover")
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
        throw std::runtime_error(_("Crossover: crossover node expected"));
    }
}

void Crossover::parse_type(xmlNodePtr node)
{
    if (node != nullptr && g_ascii_strncasecmp((char*)node->name, "type", 4) == 0)
    {
        std::istringstream((char*)xmlNodeGetContent(node)) >> m_type;
        try
        {
            parse_networks(node->next);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("Crossover: type node expected"));
    }
}

void Crossover::parse_networks(xmlNodePtr node)
{
    xmlNodePtr child;

    if ((node != nullptr) && (g_ascii_strncasecmp((char*)node->name, "networks", 8) == 0))
    {
        child = node->children;
        while (child != nullptr)
        {
            try
            {
                m_networks.emplace_back(child);
            }
            catch (std::runtime_error const& error)
            {
                throw error;
            }
            child = child->next;
        }
        try
        {
            parse_id_string(node->next);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
}

void Crossover::parse_id_string(xmlNodePtr node)
{
    if ((node != nullptr) && (g_ascii_strncasecmp((char*)node->name, "id_string", 9) == 0))
    {
        m_id_string = std::string((char*)xmlNodeGetContent(node));
    }
    else
    {
        throw std::runtime_error(_("Crossover: id_string node expected"));
    }
}

auto Crossover::to_xml_node(xmlNodePtr parent) -> xmlNodePtr
{
    xmlNodePtr crossover = xmlNewChild(parent, nullptr, (xmlChar*)("crossover"), nullptr);

    xmlNodePtr field = xmlNewChild(crossover, nullptr, (xmlChar*)("type"), nullptr);
    xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_type));

    field = xmlNewChild(crossover, nullptr, (xmlChar*)("networks"), nullptr);

    for (auto& network : m_networks)
    {
        network.to_xml_node(field);
    }

    field = xmlNewChild(crossover, nullptr, (xmlChar*)("id_string"), nullptr);
    xmlNodeSetContent(field, (xmlChar*)m_id_string.c_str());

    return crossover;
}

auto operator<<(std::ostream& output, const Crossover& crossover) -> std::ostream&
{
    output << _("Crossover type:") << crossover.m_type << "\n"
           << "Id: " << crossover.m_id << "\n---Nets----\n";

    for (auto const& network : crossover.m_networks)
    {
        output << network;
    }
    return output;
}

auto Crossover::get_id_string() const -> std::string const& { return m_id_string; }

void Crossover::set_id_string(std::string id_string) { m_id_string = std::move(id_string); }
