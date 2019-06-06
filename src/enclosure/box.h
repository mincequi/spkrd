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

#include "gspeakersobject.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <iosfwd>
#include <map>
#include <string>

constexpr auto BOX_TYPE_SEALED = 1;
constexpr auto BOX_TYPE_PORTED = 2;

class Box : public GSpeakersObject {
public:
  Box(std::string id_string = "", int type = BOX_TYPE_SEALED, double vb1 = 20, double fb1 = 40,
      double vb2 = 0, double fb2 = 0, std::string speaker = "");

  /// Construct from an xmlNode
  Box(xmlNodePtr parent);

  /* Convert data for a part to an xml node, throws std::runtime_error on failure */
  xmlNodePtr to_xml_node(xmlNodePtr parent);

  /* Print part data to stdout */
  friend std::ostream& operator<<(std::ostream& o, const Box& box);

  void set_id_string(std::string id_string);
  void set_vb1(double vb1);
  void set_fb1(double fb1);
  void set_vb2(double vb2);
  void set_fb2(double fb2);
  void set_speaker(const std::string& speaker);

  std::string const& get_id_string() const;
  double get_vb1() const;
  double get_fb1() const;
  double get_vb2() const;
  double get_fb2() const;
  const std::string& get_speaker() const;

protected:
  std::string m_id_string;
  double m_vb1;
  double m_fb1;
  double m_vb2;
  double m_fb2;
  std::string m_speaker;

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
