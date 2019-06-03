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

  xmlDocPtr doc = xmlParseFile(filename.c_str());
  if (doc != nullptr) {
    xmlNodePtr node = xmlDocGetRootElement(doc);
    if ((node != nullptr) && (g_ascii_strcasecmp((char*)node->name, "speakerlist") == 0)) {
      if (node->children != nullptr) {
        xmlNodePtr children = node->children;
        while (children != nullptr) {
          try {
            m_speaker_list.emplace_back(children);
          } catch (std::runtime_error const& e) {
            throw e;
          }
          children = children->next;
        }
      }
    } else {
      throw std::runtime_error(_("SpeakerList: speakerlist node not found"));
    }
  } else {
    throw std::runtime_error(_("SpeakerList: Xml file not found"));
  }
}

void SpeakerList::clear() { m_speaker_list.erase(m_speaker_list.begin(), m_speaker_list.end()); }

void SpeakerList::to_xml(const std::string& filename) {
  xmlDocPtr doc;
  xmlNodePtr node;

  doc = xmlNewDoc((xmlChar*)("1.0"));

  node = xmlNewDocNode(doc, nullptr, (xmlChar*)("speakerlist"), nullptr);
  xmlDocSetRootElement(doc, node);

  // Iterate through all speakers
  for (auto& from : m_speaker_list) {
    ((Speaker)from).to_xml_node(node);
  }

  // Save xml file
  if (xmlSaveFile(filename.c_str(), doc) == -1) {
    throw std::runtime_error(_("SpeakerList: Could not save to ") + filename);
  }
}

std::ostream& operator<<(std::ostream& output, const SpeakerList& speaker_list) {

  output << _("Speaker List") << "\n";

  for (auto& from : speaker_list.m_speaker_list) {
    output << from;
  }
  return output;
}

std::vector<Speaker>& SpeakerList::speaker_list() { return m_speaker_list; }

std::vector<Speaker> const& SpeakerList::speaker_list() const { return m_speaker_list; }

Speaker SpeakerList::get_speaker_by_id_string(std::string const& id_string) {

  auto const location =
      std::find_if(begin(m_speaker_list), end(m_speaker_list), [&id_string](auto const& speaker) {
        return speaker.get_id_string() == id_string;
      });

  if (location != end(m_speaker_list)) {
    return *location;
  }

  if (!m_speaker_list.empty()) {
    return m_speaker_list[0];
  }

  return Speaker();
}
