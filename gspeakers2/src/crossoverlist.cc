/*
  crossoverlist Copyright (C) 2002 Daniel Sundberg

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
#include "crossoverlist.h"

CrossoverList::CrossoverList()
{

}

CrossoverList::CrossoverList(string filename)
{
  xmlDocPtr doc;
  xmlNodePtr node, children;
  
  doc = xmlParseFile( filename.c_str() );
  if (doc != NULL) {
    node = xmlDocGetRootElement(doc);
    if ((node != NULL) && (g_strcasecmp( (char *)node->name, "crossoverlist" ) == 0)) {
      if (node->children) {
        children = node->children;
        while (children != NULL) {
          try {
            m_crossover_list.push_back(Crossover(children));
          } catch (GSpeakersException e) {
            throw e;
          }
          children = children->next;
        }
      }
    } else {
      throw GSpeakersException("CrossoverList: crossoverlist node not found");
    }
  } else {
    throw GSpeakersException("CrossoverList: Xml file not found");
  }
}

void CrossoverList::clear() 
{
  m_crossover_list.erase(m_crossover_list.begin(), m_crossover_list.end());
}

void CrossoverList::to_xml(string filename)
{
  xmlDocPtr doc;
  xmlNodePtr node;
  
  doc = xmlNewDoc( (xmlChar *)("1.0") );
  
  node = xmlNewDocNode( doc, NULL, (xmlChar *)("crossoverlist"), NULL );
  xmlDocSetRootElement(doc, node);

  /* Iterate through all crossovers */
  for(
    vector<Crossover>::iterator from = m_crossover_list.begin();
    from != m_crossover_list.end();
    ++from)
  { 
    ((Crossover)(*from)).to_xml_node(node);
  }

  /* Save xml file */
  xmlSaveFile(filename.c_str(), doc);
}


ostream& operator<< (ostream& o, const CrossoverList& crossover_list)
{
  o << "Crossover List" << endl;

  for(
    vector<Crossover>::iterator from = ((vector<Crossover>)(crossover_list.m_crossover_list)).begin();
    from != ((vector<Crossover>)(crossover_list.m_crossover_list)).end();
    ++from)
  {
    o << *from;
  }
  
  return o;
}

vector<Crossover> *CrossoverList::crossover_list()
{
  return &m_crossover_list;
}
