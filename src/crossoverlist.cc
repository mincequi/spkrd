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

#include "crossoverlist.h"

#include <glib.h>

CrossoverList::CrossoverList(std::string filename) {

  xmlDocPtr doc = xmlParseFile(filename.c_str());
  if (doc != NULL) {
    xmlNodePtr node = xmlDocGetRootElement(doc);
    if ((node != NULL) && (g_ascii_strcasecmp((char*)node->name, "crossoverlist") == 0)) {
      if (node->children) {
        xmlNodePtr children = node->children;
        while (children != NULL) {
          try {
            m_crossover_list.push_back(Crossover(children));
          } catch (GSpeakersException const& e) {
            throw e;
          }
          children = children->next;
        }
      }
    } else {
      throw GSpeakersException(_("CrossoverList: crossoverlist node not found"));
    }
  } else {
    throw GSpeakersException(_("CrossoverList: Xml file not found"));
  }
}

void CrossoverList::clear() {
  m_crossover_list.erase(m_crossover_list.begin(), m_crossover_list.end());
}

void CrossoverList::to_xml(std::string filename) {

  xmlDocPtr doc = xmlNewDoc((xmlChar*)("1.0"));

  xmlNodePtr node = xmlNewDocNode(doc, NULL, (xmlChar*)("crossoverlist"), NULL);
  xmlDocSetRootElement(doc, node);

  /* Iterate through all crossovers */
  for (auto from = m_crossover_list.begin(); from != m_crossover_list.end(); ++from) {
    ((Crossover)(*from)).to_xml_node(node);
  }

  /* Save xml file */
  if (xmlSaveFile(filename.c_str(), doc) == -1) {
    throw GSpeakersException(_("CrossoverList: Could not save to ") + filename);
  }
}

std::ostream& operator<<(std::ostream& o, const CrossoverList& crossover_list) {
  o << "Crossover List" << std::endl;

  for (auto from = crossover_list.m_crossover_list.begin();
       from != crossover_list.m_crossover_list.end(); ++from) {
    o << *from;
  }
  return o;
}

std::vector<Crossover>* CrossoverList::crossover_list() { return &m_crossover_list; }
