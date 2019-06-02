/*
  speakerlist Copyright (C) 2002 Daniel Sundberg

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

#include "speakerlist.h"
#include "common.h"

#include <glib.h>

SpeakerList::SpeakerList(const std::string& filename) {
  xmlDocPtr doc;
  xmlNodePtr node, children;

  doc = xmlParseFile(filename.c_str());
  if (doc != nullptr) {
    node = xmlDocGetRootElement(doc);
    if ((node != nullptr) && (g_ascii_strcasecmp((char*)node->name, "speakerlist") == 0)) {
      if (node->children != nullptr) {
        children = node->children;
        while (children != nullptr) {
          try {
            m_speaker_list.emplace_back(children);
          } catch (GSpeakersException const& e) {
            throw e;
          }
          children = children->next;
        }
      }
    } else {
      throw GSpeakersException(_("SpeakerList: speakerlist node not found"));
    }
  } else {
    throw GSpeakersException(_("SpeakerList: Xml file not found"));
  }
}

void SpeakerList::clear() { m_speaker_list.erase(m_speaker_list.begin(), m_speaker_list.end()); }

void SpeakerList::to_xml(const std::string& filename) {
  xmlDocPtr doc;
  xmlNodePtr node;

  doc = xmlNewDoc((xmlChar*)("1.0"));

  node = xmlNewDocNode(doc, nullptr, (xmlChar*)("speakerlist"), nullptr);
  xmlDocSetRootElement(doc, node);

  /* Iterate through all speakers */
  for (auto& from : m_speaker_list) {
    ((Speaker)from).to_xml_node(node);
  }

  /* Save xml file */
  if (xmlSaveFile(filename.c_str(), doc) == -1) {
    throw GSpeakersException(_("SpeakerList: Could not save to ") + filename);
  }
}

std::ostream& operator<<(std::ostream& o, const SpeakerList& speaker_list) {
  o << _("Speaker List") << std::endl;

  for (auto& from : ((std::vector<Speaker>)(speaker_list.m_speaker_list))) {
    o << from;
  }

  return o;
}

std::vector<Speaker>& SpeakerList::speaker_list() { return m_speaker_list; }

std::vector<Speaker> const& SpeakerList::speaker_list() const { return m_speaker_list; }

Speaker SpeakerList::get_speaker_by_id_string(const std::string& id_string) {
  for (auto& from : m_speaker_list) {
    if (from.get_id_string() == id_string) {
      return from;
    }
  }
  if (!m_speaker_list.empty()) {
    return m_speaker_list[0];
  } else {
    return Speaker();
  }
}
