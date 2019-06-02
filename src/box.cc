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

#include "box.h"
#include "common.h"

#include <glib.h>

#include <sstream>

Box::Box(std::string id_string, int type, double vb1, double fb1, double vb2, double fb2,
         std::string speaker)
    : GSpeakersObject(), m_vb1(vb1), m_fb1(fb1), m_vb2(vb2), m_fb2(fb2), m_speaker(speaker) {
  m_id_string = id_string;
  m_type = type;
}

Box::Box(xmlNodePtr parent) : GSpeakersObject() {
  if ((parent != NULL) && (std::string((char*)parent->name) == std::string("box"))) {
    try {
      parse_id_string(parent->children);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Box: box node not found"));
  }
}

xmlNodePtr Box::to_xml_node(xmlNodePtr parent) {
  xmlNodePtr box, child;
  gchar* buffer = new char[8];

  box = xmlNewChild(parent, NULL, (xmlChar*)("box"), NULL);

  child = xmlNewChild(box, NULL, (xmlChar*)("id_string"), NULL);
  xmlNodeSetContent(child, (xmlChar*)m_id_string.c_str());
  child = xmlNewChild(box, NULL, (xmlChar*)("type"), NULL);
  xmlNodeSetContent(child, (xmlChar*)g_strdup_printf("%d", m_type));
  child = xmlNewChild(box, NULL, (xmlChar*)("vb1"), NULL);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_vb1));
  child = xmlNewChild(box, NULL, (xmlChar*)("fb1"), NULL);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_fb1));
  child = xmlNewChild(box, NULL, (xmlChar*)("vb2"), NULL);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_vb2));
  child = xmlNewChild(box, NULL, (xmlChar*)("fb2"), NULL);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_fb2));
  child = xmlNewChild(box, NULL, (xmlChar*)("speaker"), NULL);
  xmlNodeSetContent(child, (xmlChar*)m_speaker.c_str());

  return box;
}

std::ostream& operator<<(std::ostream& o, const Box& box) {
  o << _("Box") << std::endl
    << _("Id:        ") << box.m_id << std::endl
    << _("Id-string: ") << box.m_id_string << std::endl
    << _("Type:      ") << box.m_type << std::endl
    << _("Vb1:       ") << box.m_vb1 << std::endl
    << _("Fb1:       ") << box.m_fb1 << std::endl
    << _("Speaker:   ") << box.m_speaker << std::endl;
  return o;
}

void Box::set_id_string(std::string id_string) { m_id_string = id_string; }

void Box::set_vb1(double vb1) { m_vb1 = vb1; }

void Box::set_fb1(double fb1) { m_fb1 = fb1; }

void Box::set_vb2(double vb2) { m_vb2 = vb2; }

void Box::set_fb2(double fb2) { m_fb2 = fb2; }

void Box::set_speaker(const std::string& speaker) { m_speaker = speaker; }

std::string const& Box::get_id_string() const { return m_id_string; }

double Box::get_vb1() const { return m_vb1; }

double Box::get_fb1() const { return m_fb1; }

double Box::get_vb2() const { return m_vb2; }

double Box::get_fb2() const { return m_fb2; }

const std::string& Box::get_speaker() const { return m_speaker; }

void Box::parse_id_string(xmlNodePtr node) {
  if ((node != NULL) && (std::string((char*)node->name) == std::string("id_string"))) {
    m_id_string = std::string((char*)xmlNodeGetContent(node));
    try {
      parse_type(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Box: id_string node not found"));
  }
}

void Box::parse_type(xmlNodePtr node) {
  if ((node != NULL) && (std::string((char*)node->name) == std::string("type"))) {
    std::istringstream((char*)xmlNodeGetContent(node)) >> m_type;
    try {
      parse_vb1(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Box: type node not found"));
  }
}

void Box::parse_vb1(xmlNodePtr node) {
  if ((node != NULL) && (std::string((char*)node->name) == std::string("vb1"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_vb1;
    m_vb1 = g_ascii_strtod((gchar*)xmlNodeGetContent(node), NULL);
    try {
      parse_fb1(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Box: vb1 node not found"));
  }
}

void Box::parse_fb1(xmlNodePtr node) {
  if ((node != NULL) && (std::string((char*)node->name) == std::string("fb1"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_fb1;
    m_fb1 = g_ascii_strtod((gchar*)xmlNodeGetContent(node), NULL);
    try {
      parse_vb2(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Box: fb1 node not found"));
  }
}

void Box::parse_vb2(xmlNodePtr node) {
  if ((node != NULL) && (std::string((char*)node->name) == std::string("vb2"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_vb2;
    m_vb2 = g_ascii_strtod((gchar*)xmlNodeGetContent(node), NULL);
    try {
      parse_fb2(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Box: vb2 node not found"));
  }
}

void Box::parse_fb2(xmlNodePtr node) {
  if ((node != NULL) && (std::string((char*)node->name) == std::string("fb2"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_fb2;
    m_fb2 = g_ascii_strtod((gchar*)xmlNodeGetContent(node), NULL);
    try {
      parse_speaker(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Box: fb2 node not found"));
  }
}

void Box::parse_speaker(xmlNodePtr node) {
  if ((node != NULL) && (std::string((char*)node->name) == std::string("speaker"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_fb2;
    m_speaker = std::string((char*)xmlNodeGetContent(node));
  } else {
    throw GSpeakersException(_("Box: speaker node not found"));
  }
}
