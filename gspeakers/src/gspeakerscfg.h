/* 
 * gspeakerscfg.h
 *
 * Copyright (C) 2001 Daniel Sundberg <dss@home.se>
 *
 * http://sumpan.campus.luth.se/software/gspeakers
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#ifndef __GSPEAKERSCFG_H
#define __GSPEAKERSCFG_H

#include <string>
#include "cconfig.h"

class GSpeakersCFG : public CConfig {
 public:
  GSpeakersCFG();
  string get_font();
  int get_line_style();
  int get_line_size();
  
  void set_font( string newfont );
 private:
};

#endif
