/*
**  CConfig v0.11
 **  Copyright (C) 2000-2001 Daniel Sundberg
 **
 **  This program is free software; you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation; either version 2 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program; if not, write to the Free Software
 **  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 **
 ** cconfig.h
 ** 
 ** Made by:    Daniel Sundberg
 ** E-Mail:     dansun-8@student.luth.se
 ** Homepage:   http://sumpan.campus.luth.se
 ** 
 ** Started on  Mon Jul  3 20:00:00 2000 Daniel Sundberg 
 ** Last update Tue Mar 27 20:34:00 2001 Daniel Sundberg
 */

#ifndef __CCONFIG_H
#define __CCONFIG_H

#include <gtk/gtk.h>

#define MAX_CONFIG_FILENAME_LENGTH 50
#define MAX_CONFIG_KEYS 50

class CConfig {
private:
  /* Store key first, then data */
  GList *confKeyList;
  char *fileName;
  void write(void);
public:
  CConfig(char *configFileName);
  ~CConfig();
  void writeConfigKey(char *key, char *data);
  char *getConfigKey(char *key);
  int getNKeys(void);
  void rmConfigKey(char *key);
  void rmConfigKeyByData(char *data);
  void printCfg(void);
};

#endif
