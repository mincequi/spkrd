/*
  speaker Copyright (C) 2002 Daniel Sundberg

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

#ifndef __GFILTER_SPEAKER
#define __GFILTER_SPEAKER

#include <string>
#include <iostream>
#include <map>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "gspeakersobject.h"

/* Speaker types */
#define SPEAKER_TYPE_BASS     1
#define SPEAKER_TYPE_MIDRANGE 2
#define SPEAKER_TYPE_TWEETER  4

using namespace std;

/* 
 * This class contains data for a speaker
 */
class Speaker : public GSpeakersObject
{
public:
  /* Default values from Vifa P21WO-20-08 */
  Speaker(string id_string = "Vifa P21-20-08", int type = SPEAKER_TYPE_BASS | SPEAKER_TYPE_MIDRANGE, 
          double qts = 0.33, double vas = 113, double fs = 28, double rdc = 5.6, double lvc = 0.9, 
          double qms = 1.6, double qes = 0.41, double imp = 8, double sens = 91, 
          string freq_resp_filename = "", string imp_resp_filename = "");
  
  /* Construct a speaker from an xml node, throws an exception on error in xml */
  Speaker(xmlNodePtr parent);
  
  /* Convert data for a part to an xml node, throws GSpeakersException on failure */
  xmlNodePtr to_xml_node(xmlNodePtr parent);   // Maybe this one should throw an exception

  /* Print part data to stdout */
  friend ostream& operator<< (ostream& o, const Speaker& speaker);

  void set_qts(double qts);
  void set_vas(double vas);
  void set_fs(double fs);
  void set_rdc(double rdc);
  void set_lvc(double lvc);
  void set_qms(double qms);
  void set_qes(double qes);
  void set_imp(double imp);
  void set_sens(double sens);
  void set_freq_resp_filename(string filename);
  void set_freq_resp(map<double, double> freq_resp);
  void set_imp_resp_filename(string filename);
  void set_imp_resp(map<double, double> imp_resp);
  void set_id_string(string id_string);
  
  double get_qts();
  double get_vas();
  double get_fs();
  double get_rdc();
  double get_lvc();
  double get_qms();
  double get_qes();
  double get_imp();
  double get_sens();
  string get_freq_resp_filename();
  map<double, double> *get_freq_resp();
  string get_imp_resp_filename();
  map<double, double> *get_imp_resp();
  string get_id_string();
  
protected:
  double m_qts;     // total Q
  double m_vas;     
  double m_fs;      // free air resonanse frequency
  double m_rdc;     // voice coil resistance
  double m_lvc;     // voice coil inductance (mH)
  double m_qms;     // mechanical Q
  double m_qes;     // electrical Q
  double m_imp;     // nominal impedance
  double m_sens;    // sensitivity
  
  string m_freq_resp_filename;
  map<double, double> m_freq_resp;
  string m_imp_resp_filename;
  map<double, double> m_imp_resp;
  
  string m_id_string;

private:
  /* xml parsing helper functions */
  void parse_type(xmlNodePtr node);
  void parse_qts(xmlNodePtr node);
  void parse_vas(xmlNodePtr node);
  void parse_fs(xmlNodePtr node);
  void parse_rdc(xmlNodePtr node);
  void parse_lcv(xmlNodePtr node);
  void parse_qms(xmlNodePtr node);
  void parse_qes(xmlNodePtr node);
  void parse_imp(xmlNodePtr node);
  void parse_sens(xmlNodePtr node);
  void parse_freq_resp_filename(xmlNodePtr node);
  void parse_imp_resp_filename(xmlNodePtr node);
  void parse_id_string(xmlNodePtr node);

};

#endif
