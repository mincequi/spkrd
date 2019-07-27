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
#include <string>

constexpr auto BOX_TYPE_SEALED = 1;
constexpr auto BOX_TYPE_PORTED = 2;

class enclosure : public GSpeakersObject
{
public:
    enclosure(std::string id_string = "",
              int type = BOX_TYPE_SEALED,
              double vb1 = 20,
              double fb1 = 40,
              double vb2 = 0,
              double fb2 = 0,
              std::string speaker = "");

    /// Construct from an xmlNode
    enclosure(xmlNodePtr parent);

    /// Convert data for a part to an xml node, throws std::runtime_error on failure
    xmlNodePtr to_xml_node(xmlNodePtr parent);

    /// Print part data to stdout
    friend std::ostream& operator<<(std::ostream& o, const enclosure& box);

    void set_id_string(std::string id_string) { m_id_string = std::move(id_string); }

    void set_vb1(double vb1) { m_vb1 = vb1; }

    void set_fb1(double fb1) { m_fb1 = fb1; }

    void set_vb2(double vb2) { m_vb2 = vb2; }

    void set_fb2(double fb2) { m_fb2 = fb2; }

    void set_driver(const std::string& speaker) { m_speaker = speaker; }

    std::string const& get_id_string() const { return m_id_string; }

    double get_vb1() const { return m_vb1; }

    double get_fb1() const { return m_fb1; }

    double get_vb2() const { return m_vb2; }

    double get_fb2() const { return m_fb2; }

    const std::string& get_driver() const { return m_speaker; }

protected:
    std::string m_id_string;
    double m_vb1;
    double m_fb1;
    double m_vb2;
    double m_fb2;
    std::string m_speaker;
};

#endif
