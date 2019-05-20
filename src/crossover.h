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

#ifndef __GFILTER_CROSSOVER_H
#define __GFILTER_CROSSOVER_H

#include "common.h"
#include "gspeakersobject.h"
#include "net.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <iostream>
#include <vector>

/* You should be able to use one or more of the type properties */
#define CROSSOVER_TYPE_LOWPASS 1
#define CROSSOVER_TYPE_SUBSONIC 2
#define CROSSOVER_TYPE_HIGHPASS 4
#define CROSSOVER_TYPE_TWOWAY 8
/* Two and a half way filter will be CROSSOVER_TYPE_LOWPASS | CROSSOVER_TYPE_TWOWAY */
#define CROSSOVER_TYPE_THREEWAY 16
#define CROSSOVER_TYPE_FOURWAY 32
/* Why would anyone use my program to develop something more adveanced
   than a loudspeaker with four elements...? */

class Crossover : public GSpeakersObject {
public:
  Crossover(int type = CROSSOVER_TYPE_TWOWAY, std::string id_string = "Crossover");

  /* Construct a part from an xml node */
  Crossover(xmlNodePtr parent);

  /* Convert data for a part to an xml node, throws GSpeakersException on failure */
  xmlNodePtr to_xml_node(xmlNodePtr parent);

  /* Print part data to stdout */
  friend std::ostream& operator<<(std::ostream& o, const Crossover& crossover);

  /* Use this to get the parts or add part to the net */
  std::vector<Net>* networks();

  /* return id_string for this crossover */
  std::string get_id_string();
  void set_id_string(std::string id_string);

protected:
  /* Member variables */
  std::vector<Net> m_networks;
  // int m_id; /* from GSpeakersObject */
  // int m_type; /* from GSpeakersObject, we don't really need this one here... */
  std::string m_id_string;

  /* Member functions, used to parse xml */
  void parse_type(xmlNodePtr node);
  void parse_networks(xmlNodePtr node);
  void parse_id_string(xmlNodePtr node);
};

#endif
