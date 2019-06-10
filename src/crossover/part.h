/*
  part Copyright (C) 2002 Daniel Sundberg

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

#ifndef __GFILTER_PART_H
#define __GFILTER_PART_H

#include "gspeakersobject.h"
#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>

/* Part types */
constexpr auto PART_TYPE_CAPACITOR = 1;
constexpr auto PART_TYPE_INDUCTOR = 2;
constexpr auto PART_TYPE_RESISTOR = 3;

/*
 * Part is an electronic component, for example a inductor or capacitor
 */
class Part : public GSpeakersObject {
public:
  /* Create a new part
   *
   * type:          part type (capacitor, inductor or resistor)
   * value:         part value, for example 4.7, 3.3, 2.2
   * unit:          unit for the value, for example m for a real value of 4.7mH
   *                 this is the unit used in SPICE siumaltions, for example M == m
   *                 and 10^6 == meg
   */
  Part(int type = PART_TYPE_CAPACITOR, double value = 1.0, std::string unit = "m");

  /* Create new part from an xml node
   * This functions throws an exception if parsing of the xml fails
   */
  Part(xmlNodePtr fields) noexcept(false);

  /* Convert data for a part to an xml node, throws std::runtime_error on failure */
  xmlNodePtr to_xml_node(xmlNodePtr parent); // Maybe this one should throw an exception

  /* Print part data to stdout */
  friend std::ostream& operator<<(std::ostream& o, const Part& part);

  void set_value(double value);

  void set_unit(std::string unit);

  double get_value() const;

  std::string const& get_unit() const;

  /* Signal handlers */
  void on_part_value_changed(int id, double new_value);
  void on_part_unit_changed(int id, std::string new_unit);
  void on_part_type_changed(int id, int new_type);

protected:
  /* xml parsing */
  void parse_type(xmlNodePtr node);
  void parse_value(xmlNodePtr node);
  void parse_unit(xmlNodePtr node);

protected:
  /// Part value, for example 4.7 (used in SPICE)
  double m_value;
  /// Value unit, for example u (used in SPICE)
  std::string m_unit;
};

#endif
