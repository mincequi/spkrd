/* $Id$
 *
 * Copyright (C) 2001 Barnaby Gray <barnaby@beedesign.co.uk>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "settings.h"
#include "../config.h"

#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "sstream_fix.h"
#include <fstream>
#include <iostream>

using std::endl;
using std::ifstream;
using std::istringstream;
using std::map;
using std::ofstream;
using std::ostringstream;
using std::runtime_error;

Settings::Settings() { defaultSettings(); }

Settings::~Settings() {}

void Settings::load(const string& filename) noexcept(false) {

  ifstream inf(filename.c_str());
  if (!inf) {
    ostringstream ostr;
    ostr << _("Settings::load: Could not open file for reading: ") << filename;
    throw runtime_error(ostr.str());
  }

  while (inf) {
    string k, t, v;
    inf >> k >> t;
    if (t == "=") {
      while (inf.peek() == ' ')
        inf.ignore();
      getline(inf, v);

      m_map[k] = Unescape(v);
    }
  }
  m_filename = filename;
}

void Settings::save(const string& filename) noexcept(false) {
  /* save to a temporary file first, then switch over, that way we
     should behave better on systems run by muppets have let their
     filesystem run down to no space left  */
  string tempfilename = filename + ".temp";

  ofstream of(tempfilename.c_str(), std::ios::out | std::ios::trunc);
  if (!of) {
    ostringstream ostr;
    ostr << _("Settings::save: Could not open temporary settings file to save to: ")
         << tempfilename;
    throw runtime_error(ostr.str());
  }

  map<const string, string>::iterator curr = m_map.begin();
  while (curr != m_map.end()) {
    of << (*curr).first << " = " << Escape((*curr).second) << endl;
    if (!of) {
      of.close();
      unlink(tempfilename.c_str());
      ostringstream ostr;
      ostr << _("Settings::save: Failed writing to temporary file: ") << tempfilename;
      throw runtime_error(ostr.str());
    }
    ++curr;
  }
  of.close();

  // swap over temporary file to real file
#ifdef TARGET_WIN32
  remove(filename.c_str());
#endif
  if (rename(tempfilename.c_str(), filename.c_str()) == -1) {
    ostringstream ostr;
    ostr << _("Settings::save: Failed renaming temporary file to replace old file: ") << filename;
    throw runtime_error(ostr.str());
  }
  m_filename = filename;
}

void Settings::save() noexcept(false) {

  /* save to a temporary file first, then switch over, that way we
     should behave better on systems run by muppets have let their
     filesystem run down to no space left  */
  string tempfilename = m_filename + ".temp";

  ofstream of(tempfilename.c_str(), std::ios::out | std::ios::trunc);
  if (!of) {
    ostringstream ostr;
    ostr << _("Settings::save: Could not open temporary settings file to save to: ")
         << tempfilename;
    throw runtime_error(ostr.str());
  }

  map<const string, string>::iterator curr = m_map.begin();
  while (curr != m_map.end()) {
    of << (*curr).first << " = " << Escape((*curr).second) << endl;
    if (!of) {
      of.close();
      unlink(tempfilename.c_str());
      ostringstream ostr;
      ostr << _("Settings::save: Failed writing to temporary file: ") << tempfilename;
      throw runtime_error(ostr.str());
    }
    ++curr;
  }
  of.close();

  // swap over temporary file to real file
#ifdef TARGET_WIN32
  remove(m_filename.c_str());
#endif
  if (rename(tempfilename.c_str(), m_filename.c_str()) == -1) {
    ostringstream ostr;
    ostr << _("Settings::save: Failed renaming temporary file to replace old file: ") << m_filename;
    throw runtime_error(ostr.str());
  }
}

string Settings::Escape(const string& s) {
  ostringstream ostr;
  string::const_iterator curr = s.begin();
  while (curr != s.end()) {
    switch (*curr) {
    case '\n':
      ostr << "\\n";
      break;
    case '\r':
      ostr << "\\r";
      break;
    case '\\':
      ostr << "\\\\";
      break;
    default:
      ostr << (*curr);
    }
    ++curr;
  }

  return ostr.str();
}

string Settings::Unescape(const string& s) {
  ostringstream ostr;
  string::const_iterator curr = s.begin();
  while (curr != s.end()) {
    if (*curr == '\\') {
      ++curr;
      if (curr != s.end()) {
        switch (*curr) {
        case 'n':
          ostr << "\n";
          break;
        case 'r':
          ostr << "\r";
          break;
        case '\\':
          ostr << "\\";
          break;
        }
      } else {
        ostr << "\\";
      }
    } else {
      ostr << (*curr);
    }
    ++curr;
  }

  return ostr.str();
}

string Settings::getValueString(const string& k) {
  if (exists(k))
    return m_map[k];
  else
    return string();
}

int Settings::getValueInt(const string& k) {
  int ret = 0;
  if (exists(k)) {
    istringstream istr(m_map[k]);
    istr >> ret;
  }
  return ret;
}

unsigned int Settings::getValueUnsignedInt(const string& k) {
  unsigned int v = 0;
  if (exists(k)) {
    istringstream istr(m_map[k]);
    istr >> v;
  }
  return v;
}

unsigned short Settings::getValueUnsignedShort(const string& k) {
  unsigned short v = 0;
  if (exists(k)) {
    istringstream istr(m_map[k]);
    istr >> v;
  }
  return v;
}

unsigned char Settings::getValueUnsignedChar(const string& k) {
  unsigned char v = 0;
  if (exists(k)) {
    unsigned int vi = 0;
    istringstream istr(m_map[k]);
    istr >> vi;
    v = (unsigned char)vi;
  }
  return v;
}

bool Settings::getValueBool(const string& k) { return !(getValueUnsignedShort(k) == 0); }

void Settings::defaultValueUnsignedInt(const string& k, unsigned int dflt, unsigned int lower,
                                       unsigned int upper) {
  unsigned int v;
  if (exists(k)) {
    v = getValueUnsignedInt(k);
    if (v < lower)
      v = lower;
    if (v > upper)
      v = upper;
  } else {
    v = dflt;
  }
  setValue(k, v);
}

void Settings::defaultValueUnsignedShort(const string& k, unsigned short dflt, unsigned short lower,
                                         unsigned short upper) {
  unsigned short v;
  if (exists(k)) {
    v = getValueUnsignedShort(k);
    if (v < lower)
      v = lower;
    if (v > upper)
      v = upper;
  } else {
    v = dflt;
  }
  setValue(k, v);
}

void Settings::defaultValueUnsignedChar(const string& k, unsigned char dflt, unsigned char lower,
                                        unsigned char upper) {
  unsigned char v;
  if (exists(k)) {
    v = getValueUnsignedChar(k);
    if (v < lower)
      v = lower;
    if (v > upper)
      v = upper;
  } else {
    v = dflt;
  }
  setValue(k, v);
}

void Settings::defaultValueBool(const string& k, bool dflt) {
  if (!exists(k))
    setValue(k, dflt);
}

void Settings::defaultValueString(const string& k, const string& dflt) {
  if (!exists(k))
    setValue(k, dflt);
}

void Settings::setValue(const string& k, const string& v) {
  m_map[k] = v;
  settings_changed.emit(k);
}

void Settings::setValue(const string& k, int v) {
  ostringstream ostr;
  ostr << v;
  if (m_map[k] != ostr.str()) {
    m_map[k] = ostr.str();
    settings_changed.emit(k);
  }
}

void Settings::setValue(const string& k, unsigned int v) {
  ostringstream ostr;
  ostr << v;
  if (m_map[k] != ostr.str()) {
    m_map[k] = ostr.str();
    settings_changed.emit(k);
  }
}

void Settings::setValue(const string& k, unsigned short v) {
  ostringstream ostr;
  ostr << v;
  m_map[k] = ostr.str();
  settings_changed.emit(k);
}

void Settings::setValue(const string& k, unsigned char v) {
  ostringstream ostr;
  ostr << (unsigned int)v;
  if (m_map[k] != ostr.str()) {
    m_map[k] = ostr.str();
    settings_changed.emit(k);
  }
}

void Settings::setValue(const string& k, bool v) {
  if (m_map[k] != (v ? "1" : "0")) {
    m_map[k] = (v ? "1" : "0");
    settings_changed.emit(k);
  }
}

bool Settings::exists(const string& k) { return (m_map.count(k) != 0); }

void Settings::defaultSettings() {
  // virtual
}
