/*
  $Id$

  boxlist Copyright (C) 2002 Daniel Sundberg

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

#ifndef __GFILTER_BOXLIST
#define __GFILTER_BOXLIST

#include "box.h"
#include "gspeakersobject.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <iosfwd>
#include <string>
#include <vector>

/*
 * This is a class that handles a list of boxes
 */
class BoxList {
public:
  BoxList();

  /* Construct a part from an xml file */
  BoxList(std::string filename);

  /* Convert data for a part to an xml node, throws GSpeakersException on failure */
  void to_xml(std::string filename);

  /* Print part data to stdout */
  friend std::ostream& operator<<(std::ostream& o, const BoxList& box_list);

  /* Return the box_list vector */
  std::vector<Box>* box_list();

  /* Clear all items in the list */
  void clear();

protected:
  /* Member variables */
  std::vector<Box> m_box_list;
};

#endif
