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

#include <glib/gstrfuncs.h>
#include "boxlist.h"
#include "common.h"

BoxList::BoxList()
{

}

BoxList::BoxList(string filename)
{
  xmlDocPtr doc;
  xmlNodePtr node, children;
  
  doc = xmlParseFile( filename.c_str() );
  if (doc != NULL) {
    node = xmlDocGetRootElement(doc);
    if ((node != NULL) && (g_strcasecmp( (char *)node->name, "boxlist" ) == 0)) {
      if (node->children) {
        children = node->children;
        while (children != NULL) {
          try {
            m_box_list.push_back(Box(children));
          } catch (GSpeakersException e) {
            throw e;
          }
          children = children->next;
        }
      }
    } else {
      throw GSpeakersException("BoxList: boxlist node not found");
    }
  } else {
    throw GSpeakersException("BoxList: Xml file not found");
  }
}
  
void BoxList::to_xml(string filename)
{
  xmlDocPtr doc;
  xmlNodePtr node;
  
  doc = xmlNewDoc( (xmlChar *)("1.0") );
  
  node = xmlNewDocNode( doc, NULL, (xmlChar *)("boxlist"), NULL );
  xmlDocSetRootElement(doc, node);

  /* Iterate through all boxes */
  for(
    vector<Box>::iterator from = m_box_list.begin();
    from != m_box_list.end();
    ++from)
  { 
    ((Box)(*from)).to_xml_node(node);
  }

  /* Save xml file */
  if (xmlSaveFile(filename.c_str(), doc) == -1) {
    throw GSpeakersException("BoxList: Could not save to " + filename);
  }
}


ostream& operator<< (ostream& o, const BoxList& box_list)
{
  o << "Box List" << endl;

  for(
    vector<Box>::iterator from = ((vector<Box>)(box_list.m_box_list)).begin();
    from != ((vector<Box>)(box_list.m_box_list)).end();
    ++from)
  {
    o << *from;
  }
  
  return o;
}

vector<Box> *BoxList::box_list()
{
  return &m_box_list;
}

void BoxList::clear()
{
  m_box_list.erase(m_box_list.begin(), m_box_list.end());
}
