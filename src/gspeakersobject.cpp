/*
  gspeakers_object Copyright (C) 2002 Daniel Sundberg

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

#include "gspeakersobject.h"

gspkObject::gspkObject() : sigc::trackable()
{
    static int next_id = 0;

    m_id = next_id++;
}

gspkObject::gspkObject(int type) : gspkObject() { m_type = type; }

auto gspkObject::get_id() const -> int { return m_id; }

void gspkObject::set_type(int type) { m_type = type; }

auto gspkObject::get_type() const -> int { return m_type; }
