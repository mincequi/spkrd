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

#ifndef __GFILTER_SPEAKERLIST
#define __GFILTER_SPEAKERLIST

#include "gspeakersobject.h"
#include "speaker.h"
#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>
#include <vector>

/*
 * This is a list to store all speakers in
 */
class SpeakerList {
public:
  /* Default constructor */
  SpeakerList();

  /* Construct a part from an xml file */
  SpeakerList(string filename);

  /* Convert data for a part to an xml node, throws GSpeakersException on failure */
  void to_xml(string filename); // Maybe this one should throw an exception

  /* Print part data to stdout */
  friend ostream& operator<<(ostream& o, const SpeakerList& speaker_list);

  vector<Speaker>* speaker_list();

  Speaker get_speaker_by_id_string(string id_string);

  /* Remove all items from the speakerlist */
  void clear();

protected:
  /* Member variables */
  vector<Speaker> m_speaker_list;
};

#endif
