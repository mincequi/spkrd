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

#include <glib/gstrfuncs.h>
#include <sstream>
#include "box.h"
#include "common.h"

Box::Box(string id_string, int type, double vb1, double fb1, double vb2, double fb2) : GSpeakersObject()
{
  m_id_string = id_string;
  m_type = type;
  m_vb1 = vb1;
  m_fb1 = fb1;
  m_vb2 = vb2;
  m_fb2 = fb2;
}

Box::Box(xmlNodePtr parent) : GSpeakersObject()
{
  if (( parent != NULL ) && ( string( (char *)parent->name) == string( "box" ))) {
    try {
      parse_id_string(parent->children);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Box: box node not found"));
  }
}
  
xmlNodePtr Box::to_xml_node(xmlNodePtr parent)
{
  xmlNodePtr box, child;
  
  box = xmlNewChild( parent, NULL, (xmlChar *)("box"), NULL );

  child = xmlNewChild( box, NULL, (xmlChar *)("id_string"), NULL );
  xmlNodeSetContent( child, (xmlChar *)m_id_string.c_str());
  child = xmlNewChild( box, NULL, (xmlChar *)("type"), NULL );
  xmlNodeSetContent( child, (xmlChar *)g_strdup_printf("%d", m_type));
  child = xmlNewChild( box, NULL, (xmlChar *)("vb1"), NULL );
  xmlNodeSetContent( child, (xmlChar *)g_strdup_printf("%f", m_vb1));
  child = xmlNewChild( box, NULL, (xmlChar *)("fb1"), NULL );
  xmlNodeSetContent( child, (xmlChar *)g_strdup_printf("%f", m_fb1));
  child = xmlNewChild( box, NULL, (xmlChar *)("vb2"), NULL );
  xmlNodeSetContent( child, (xmlChar *)g_strdup_printf("%f", m_vb2));
  child = xmlNewChild( box, NULL, (xmlChar *)("fb2"), NULL );
  xmlNodeSetContent( child, (xmlChar *)g_strdup_printf("%f", m_fb2));

  return box;
}

ostream& operator<< (ostream& o, const Box& box)
{
  o << _("Box") << endl <<
       _("Id:        ") << box.m_id << endl <<
       _("Id-string: ") << box.m_id_string << endl <<
       _("Type:      ") << box.m_type << endl <<
       _("Vb1:       ") << box.m_vb1 << endl <<
       _("Fb1:       ") << box.m_fb1 << endl;
  return o;
}

void Box::set_id_string(string id_string)
{
  m_id_string = id_string;
}

void Box::set_vb1(double vb1)
{
  m_vb1 = vb1;
}

void Box::set_fb1(double fb1)
{
  m_fb1 = fb1;
}

void Box::set_vb2(double vb2)
{
  m_vb2 = vb2;
}

void Box::set_fb2(double fb2)
{
  m_fb2 = fb2;
}

string Box::get_id_string()
{
  return m_id_string;
}

double Box::get_vb1()
{
  return m_vb1;
}

double Box::get_fb1()
{
  return m_fb1;
}

double Box::get_vb2()
{
  return m_vb2;
}

double Box::get_fb2()
{
  return m_fb2;
}
  
void Box::parse_id_string(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "id_string" ))) {
    m_id_string = string((char *)xmlNodeGetContent(node));
    try {
      parse_type(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Box: id_string node not found"));
  }
}

void Box::parse_type(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "type" ))) {
    istringstream((char *)xmlNodeGetContent(node)) >> m_type;
    try {
      parse_vb1(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Box: type node not found"));
  }
}

void Box::parse_vb1(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "vb1" ))) {
    istringstream((char *)xmlNodeGetContent(node)) >> m_vb1;
    try {
      parse_fb1(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Box: vb1 node not found"));
  }
}

void Box::parse_fb1(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "fb1" ))) {
    istringstream((char *)xmlNodeGetContent(node)) >> m_fb1;
    try {
      parse_vb2(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Box: fb1 node not found"));
  }
}

void Box::parse_vb2(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "vb2" ))) {
    istringstream((char *)xmlNodeGetContent(node)) >> m_vb2;
    try {
      parse_fb2(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Box: vb2 node not found"));
  }
}

void Box::parse_fb2(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "fb2" ))) {
    istringstream((char *)xmlNodeGetContent(node)) >> m_fb2;
  } else {
    throw GSpeakersException(_("Box: fb2 node not found"));
  }
}
