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
#include <sigc++/sigc++.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <glib/gstrfuncs.h>

using namespace sigc;
using namespace std;

Part::Part(int type, double value, string unit) : GSpeakersObject()
{
  m_type  = type;
  m_value = value;
  m_unit  = unit;
}
  
Part::Part(xmlNodePtr parent) : GSpeakersObject()
{   
  if (( parent != NULL ) && ( string( (char *)parent->name) == string( "part" ))) {
    try {
      parse_type(parent->children);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Part: part node not found"));
  }
}

void Part::parse_type(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "type" ))) {
    istringstream((char *)xmlNodeGetContent(node)) >> m_type;
    try {
      parse_value(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Part: type node not found"));
  }
}

void Part::parse_value(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "value" ))) {
    //istringstream((char *)xmlNodeGetContent(node)) >> m_value;
    m_value = g_ascii_strtod((gchar *)xmlNodeGetContent(node), NULL);
    try {
      parse_unit(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Part: value node not found"));
  }

}

void Part::parse_unit(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "unit" ))) {
    m_unit = string((char *)xmlNodeGetContent(node));
  } else {
    throw GSpeakersException(_("Part: unit node not found"));
  }
}



xmlNodePtr Part::to_xml_node(xmlNodePtr parent)
{
  xmlNodePtr part, field;
  gchar *buffer = new gchar[8];
  
  part = xmlNewChild( parent, NULL, (xmlChar *)("part"), NULL );
  field = xmlNewChild( part, NULL, (xmlChar *)("type"), NULL );
  xmlNodeSetContent( field, (xmlChar *)g_strdup_printf("%d", m_type));
  field = xmlNewChild( part, NULL, (xmlChar *)("value"), NULL );
  xmlNodeSetContent( field, (xmlChar *)g_ascii_dtostr(buffer, 8, m_value));
  field = xmlNewChild( part, NULL, (xmlChar *)("unit"), NULL );
  xmlNodeSetContent( field, (xmlChar *)(m_unit.c_str()));

  return part;
}

void Part::set_value(double value) 
{

  m_value = value;
  //cout << "Part.id: " << get_id() << "...value set to " << m_value << endl;
}

void Part::set_unit(string unit) 
{
  m_unit = unit;
}

double Part::get_value() 
{
  return m_value;
}

string Part::get_unit() 
{
  return m_unit;
}

void Part::on_part_value_changed(int id, double new_value)
{
  if (id == get_id()) {
    //cout << "Signal emitted in: " << get_id() << endl;
    m_value = new_value;
  }
}

void Part::on_part_unit_changed(int id, std::string new_unit)
{
  if (id == get_id()) {
    //cout << "Signal emitted in: " << get_id() << endl;
    m_unit = new_unit;
  }
}

void Part::on_part_type_changed(int id, int new_type)
{
  if (id == get_id()) {
    //cout << "Signal emitted in: " << get_id() << endl;
    m_type = new_type;
  }
}

std::ostream& operator<< (std::ostream& o, const Part& part) 
{
  return o << _("***Part***") << endl <<
              _("Id   : ") << part.m_id << endl <<
              _("Type : ") << part.m_type << endl <<
              _("Value: ") << part.m_value << endl <<
              _("Unit : ") << part.m_unit << endl;
}
