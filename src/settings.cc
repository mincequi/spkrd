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

#include "common.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <unistd.h>

Settings::Settings() { defaultSettings(); }

void Settings::load(const std::string& filename) noexcept(false) {

  std::ifstream inf(filename.c_str());
  if (!inf) {
    std::ostringstream ostr;
    ostr << _("Settings::load: Could not open file for reading: ") << filename;
    throw std::runtime_error(ostr.str());
  }

  while (inf) {
    std::string k, t, v;
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

void Settings::save(const std::string& filename) noexcept(false) {
  /* save to a temporary file first, then switch over, that way we
     should behave better on systems run by muppets have let their
     filesystem run down to no space left  */
  std::string tempfilename = filename + ".temp";

  std::ofstream of(tempfilename.c_str(), std::ios::out | std::ios::trunc);
  if (!of) {
    std::ostringstream ostr;
    ostr << _("Settings::save: Could not open temporary settings file to save to: ")
         << tempfilename;
    throw std::runtime_error(ostr.str());
  }

  auto curr = m_map.begin();
  while (curr != m_map.end()) {
    of << (*curr).first << " = " << Escape((*curr).second) << std::endl;
    if (!of) {
      of.close();
      unlink(tempfilename.c_str());
      std::ostringstream ostr;
      ostr << _("Settings::save: Failed writing to temporary file: ") << tempfilename;
      throw std::runtime_error(ostr.str());
    }
    ++curr;
  }
  of.close();

  // swap over temporary file to real file
#ifdef TARGET_WIN32
  remove(filename.c_str());
#endif
  if (rename(tempfilename.c_str(), filename.c_str()) == -1) {
    std::ostringstream ostr;
    ostr << _("Settings::save: Failed renaming temporary file to replace old file: ") << filename;
    throw std::runtime_error(ostr.str());
  }
  m_filename = filename;
}

void Settings::save() noexcept(false) {

  /* save to a temporary file first, then switch over, that way we
     should behave better on systems run by muppets have let their
     filesystem run down to no space left  */
  std::string tempfilename = m_filename + ".temp";

  std::ofstream of(tempfilename.c_str(), std::ios::out | std::ios::trunc);
  if (!of) {
    std::ostringstream ostr;
    ostr << _("Settings::save: Could not open temporary settings file to save to: ")
         << tempfilename;
    throw std::runtime_error(ostr.str());
  }

  auto curr = m_map.begin();
  while (curr != m_map.end()) {
    of << (*curr).first << " = " << Escape((*curr).second) << std::endl;
    if (!of) {
      of.close();
      unlink(tempfilename.c_str());
      std::ostringstream ostr;
      ostr << _("Settings::save: Failed writing to temporary file: ") << tempfilename;
      throw std::runtime_error(ostr.str());
    }
    ++curr;
  }
  of.close();

  // swap over temporary file to real file
#ifdef TARGET_WIN32
  remove(m_filename.c_str());
#endif
  if (rename(tempfilename.c_str(), m_filename.c_str()) == -1) {
    std::ostringstream ostr;
    ostr << _("Settings::save: Failed renaming temporary file to replace old file: ") << m_filename;
    throw std::runtime_error(ostr.str());
  }
}

std::string Settings::Escape(const std::string& s) {
  std::ostringstream ostr;
  std::string::const_iterator curr = s.begin();
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

std::string Settings::Unescape(const std::string& s) {
  std::ostringstream ostr;
  std::string::const_iterator curr = s.begin();
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

std::string Settings::getValueString(const std::string& k) {
  if (exists(k))
    return m_map[k];
  else
    return std::string();
}

int Settings::getValueInt(const std::string& k) {
  int ret = 0;
  if (exists(k)) {
    std::istringstream istr(m_map[k]);
    istr >> ret;
  }
  return ret;
}

unsigned int Settings::getValueUnsignedInt(const std::string& k) {
  unsigned int v = 0;
  if (exists(k)) {
    std::istringstream istr(m_map[k]);
    istr >> v;
  }
  return v;
}

unsigned short Settings::getValueUnsignedShort(const std::string& k) {
  unsigned short v = 0;
  if (exists(k)) {
    std::istringstream istr(m_map[k]);
    istr >> v;
  }
  return v;
}

unsigned char Settings::getValueUnsignedChar(const std::string& k) {
  unsigned char v = 0;
  if (exists(k)) {
    unsigned int vi = 0;
    std::istringstream istr(m_map[k]);
    istr >> vi;
    v = (unsigned char)vi;
  }
  return v;
}

bool Settings::getValueBool(const std::string& k) { return !(getValueUnsignedShort(k) == 0); }

void Settings::defaultValueUnsignedInt(const std::string& k, unsigned int dflt, unsigned int lower,
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

void Settings::defaultValueUnsignedShort(const std::string& k, unsigned short dflt,
                                         unsigned short lower, unsigned short upper) {
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

void Settings::defaultValueUnsignedChar(const std::string& k, unsigned char dflt,
                                        unsigned char lower, unsigned char upper) {
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

void Settings::defaultValueBool(const std::string& k, bool dflt) {
  if (!exists(k))
    setValue(k, dflt);
}

void Settings::defaultValueString(const std::string& k, const std::string& dflt) {
  if (!exists(k))
    setValue(k, dflt);
}

void Settings::setValue(const std::string& k, const std::string& v) {
  m_map[k] = v;
  settings_changed.emit(k);
}

void Settings::setValue(const std::string& k, int v) {
  std::ostringstream ostr;
  ostr << v;
  if (m_map[k] != ostr.str()) {
    m_map[k] = ostr.str();
    settings_changed.emit(k);
  }
}

void Settings::setValue(const std::string& k, unsigned int v) {
  std::ostringstream ostr;
  ostr << v;
  if (m_map[k] != ostr.str()) {
    m_map[k] = ostr.str();
    settings_changed.emit(k);
  }
}

void Settings::setValue(const std::string& k, unsigned short v) {
  std::ostringstream ostr;
  ostr << v;
  m_map[k] = ostr.str();
  settings_changed.emit(k);
}

void Settings::setValue(const std::string& k, unsigned char v) {
  std::ostringstream ostr;
  ostr << (unsigned int)v;
  if (m_map[k] != ostr.str()) {
    m_map[k] = ostr.str();
    settings_changed.emit(k);
  }
}

void Settings::setValue(const std::string& k, bool v) {
  if (m_map[k] != (v ? "1" : "0")) {
    m_map[k] = (v ? "1" : "0");
    settings_changed.emit(k);
  }
}

bool Settings::exists(const std::string& k) { return (m_map.count(k) != 0); }

void Settings::defaultSettings() {
  // virtual
}
