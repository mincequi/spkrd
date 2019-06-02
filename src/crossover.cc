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

#include "crossover.h"
#include <glib.h>
#include <sstream>
#include <utility>

Crossover::Crossover(int type, std::string id_string)
    : GSpeakersObject(), m_id_string(std::move(id_string)) {
  m_type = type;

  if ((m_type & CROSSOVER_TYPE_SUBSONIC) != 0) {
    m_networks.emplace_back(NET_TYPE_HIGHPASS, NET_NOT_PRESENT, NET_ORDER_1ST);
  }
  if ((m_type & CROSSOVER_TYPE_LOWPASS) != 0) {
    m_networks.emplace_back(NET_TYPE_LOWPASS, NET_ORDER_1ST, NET_NOT_PRESENT);
  }
  if ((m_type & CROSSOVER_TYPE_HIGHPASS) != 0) {
    m_networks.emplace_back(NET_TYPE_HIGHPASS, NET_NOT_PRESENT, NET_ORDER_1ST);
  }
  if ((m_type & CROSSOVER_TYPE_TWOWAY) != 0) {
    m_networks.emplace_back(NET_TYPE_LOWPASS, NET_ORDER_1ST, NET_NOT_PRESENT);
    m_networks.emplace_back(NET_TYPE_HIGHPASS, NET_NOT_PRESENT, NET_ORDER_1ST);
  }
  if ((m_type & CROSSOVER_TYPE_THREEWAY) != 0) {
    m_networks.emplace_back(NET_TYPE_LOWPASS, NET_ORDER_1ST, NET_NOT_PRESENT);
    m_networks.emplace_back(NET_TYPE_LOWPASS | NET_TYPE_HIGHPASS, NET_ORDER_1ST, NET_ORDER_1ST);
    m_networks.emplace_back(NET_TYPE_HIGHPASS, NET_NOT_PRESENT, NET_ORDER_1ST);
  }
  if ((m_type & CROSSOVER_TYPE_FOURWAY) != 0) {
    m_networks.emplace_back(NET_TYPE_LOWPASS, NET_ORDER_1ST, NET_NOT_PRESENT);
    m_networks.emplace_back(NET_TYPE_LOWPASS | NET_TYPE_HIGHPASS, NET_ORDER_1ST, NET_ORDER_1ST);
    m_networks.emplace_back(NET_TYPE_LOWPASS | NET_TYPE_HIGHPASS, NET_ORDER_1ST, NET_ORDER_1ST);
    m_networks.emplace_back(NET_TYPE_HIGHPASS, NET_NOT_PRESENT, NET_ORDER_1ST);
  }
}

Crossover::Crossover(xmlNodePtr parent) {
  if ((parent != nullptr) && (g_ascii_strncasecmp((char*)parent->name, "crossover", 9) == 0)) {
    try {
      parse_type(parent->children);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Crossover: crossover node expected"));
  }
}

void Crossover::parse_type(xmlNodePtr node) {
  if ((node != nullptr) && (g_ascii_strncasecmp((char*)node->name, "type", 4) == 0)) {
    std::istringstream((char*)xmlNodeGetContent(node)) >> m_type;
    try {
      parse_networks(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Crossover: type node expected"));
  }
}

void Crossover::parse_networks(xmlNodePtr node) {
  xmlNodePtr child;

  if ((node != nullptr) && (g_ascii_strncasecmp((char*)node->name, "networks", 8) == 0)) {
    child = node->children;
    while (child != nullptr) {
      try {
        m_networks.emplace_back(child);
      } catch (GSpeakersException const& e) {
        throw e;
      }
      child = child->next;
    }
    try {
      parse_id_string(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  }
}

void Crossover::parse_id_string(xmlNodePtr node) {
  if ((node != nullptr) && (g_ascii_strncasecmp((char*)node->name, "id_string", 9) == 0)) {
    m_id_string = std::string((char*)xmlNodeGetContent(node));
  } else {
    throw GSpeakersException(_("Crossover: id_string node expected"));
  }
}

xmlNodePtr Crossover::to_xml_node(xmlNodePtr parent) {

  xmlNodePtr crossover, field;

  crossover = xmlNewChild(parent, nullptr, (xmlChar*)("crossover"), nullptr);

  field = xmlNewChild(crossover, nullptr, (xmlChar*)("type"), nullptr);
  xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_type));
  field = xmlNewChild(crossover, nullptr, (xmlChar*)("networks"), nullptr);

  for (auto& network : m_networks) {
    network.to_xml_node(field);
  }
  field = xmlNewChild(crossover, nullptr, (xmlChar*)("id_string"), nullptr);
  xmlNodeSetContent(field, (xmlChar*)m_id_string.c_str());

  return crossover;
}

std::ostream& operator<<(std::ostream& o, const Crossover& crossover) {
  o << _("Crossover type:") << crossover.m_type << std::endl
    << "Id: " << crossover.m_id << std::endl
    << "---Nets----" << std::endl;
  for (auto const& network : crossover.m_networks) {
    o << network;
  }
  return o;
}

std::vector<Net>* Crossover::networks() { return &m_networks; }

std::string const& Crossover::get_id_string() const { return m_id_string; }

void Crossover::set_id_string(std::string id_string) { m_id_string = std::move(id_string); }
