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

#ifndef __GFILTER_BOX
#define __GFILTER_BOX

#include <string>
#include <iostream>
#include <map>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "gspeakersobject.h"

#define BOX_TYPE_SEALED 1
#define BOX_TYPE_PORTED 2

using namespace std;

class Box : public GSpeakersObject
{
public:
  Box(string id_string = "", int type = BOX_TYPE_SEALED, double vb1 = 20, double fb1 = 40, 
      double vb2 = 0, double fb2 = 0, string speaker = "");
  /* Construct box object from an xmlNode */
  Box(xmlNodePtr parent);
  
  /* Convert data for a part to an xml node, throws GSpeakersException on failure */
  xmlNodePtr to_xml_node(xmlNodePtr parent);

  /* Print part data to stdout */
  friend ostream& operator<< (ostream& o, const Box& box);

  void set_id_string(string id_string);
  void set_vb1(double vb1);
  void set_fb1(double fb1);
  void set_vb2(double vb2);
  void set_fb2(double fb2);
  void set_speaker(const string& speaker);
  
  string get_id_string();
  double get_vb1();
  double get_fb1();
  double get_vb2();
  double get_fb2();
  const string& get_speaker();
  
protected:
  string m_id_string;
  double m_vb1;
  double m_fb1;
  double m_vb2;
  double m_fb2;
  string m_speaker;

private:
  /* various helper functions for the xml parsing routine */
  void parse_id_string(xmlNodePtr node);
  void parse_type(xmlNodePtr node);
  void parse_vb1(xmlNodePtr node);
  void parse_fb1(xmlNodePtr node);
  void parse_vb2(xmlNodePtr node);
  void parse_fb2(xmlNodePtr node);
  void parse_speaker(xmlNodePtr node);
};

#endif
