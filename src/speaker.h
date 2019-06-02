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

#include <iostream>
#include <map>
#include <string>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "gspeakersobject.h"

/* Speaker types */
#define SPEAKER_TYPE_BASS 1
#define SPEAKER_TYPE_MIDRANGE 2
#define SPEAKER_TYPE_TWEETER 4

/*
 * This class contains data for a speaker
 */
class Speaker : public GSpeakersObject {
public:
  /* Default values from Vifa P21WO-20-08 */
  Speaker(std::string id_string = "Vifa P21-20-08",
          int type = SPEAKER_TYPE_BASS | SPEAKER_TYPE_MIDRANGE, double qts = 0.33, double vas = 113,
          double fs = 28, double rdc = 5.6, double lvc = 0.9, double qms = 1.6, double qes = 0.41,
          double imp = 8, double sens = 91, std::string freq_resp_filename = "",
          std::string imp_resp_filename = "", double mmd = 0.04, double ad = 0.0837, double bl = 11,
          double rms = 1.7, double cms = 0.0012);

  /* Construct a speaker from an xml node, throws an exception on error in xml */
  Speaker(xmlNodePtr parent);

  /* Convert data for a part to an xml node, throws GSpeakersException on failure */
  xmlNodePtr to_xml_node(xmlNodePtr parent); // Maybe this one should throw an exception

  /* Print part data to stdout */
  friend std::ostream& operator<<(std::ostream& o, const Speaker& speaker);

  void set_qts(double qts);
  void set_vas(double vas);
  void set_fs(double fs);
  void set_rdc(double rdc);
  void set_lvc(double lvc);
  void set_qms(double qms);
  void set_qes(double qes);
  void set_imp(double imp);
  void set_sens(double sens);
  void set_freq_resp_filename(std::string filename);
  void set_freq_resp(std::map<double, double> freq_resp);
  void set_imp_resp_filename(std::string filename);
  void set_imp_resp(std::map<double, double> imp_resp);
  void set_id_string(std::string id_string);
  void set_mmd(double mmd);
  void set_ad(double ad);
  void set_bl(double bl);
  void set_rms(double rms);
  void set_cms(double cms);

  double get_qts() const;
  double get_vas() const;
  double get_fs() const;
  double get_rdc() const;
  double get_lvc() const;
  double get_qms() const;
  double get_qes() const;
  double get_imp() const;
  double get_sens() const;
  std::string get_freq_resp_filename() const;
  std::map<double, double> const* get_freq_resp() const;
  std::string get_imp_resp_filename() const;
  std::map<double, double> const* get_imp_resp() const;
  std::string const& get_id_string() const;
  double get_mmd() const;
  double get_ad() const;
  double get_bl() const;
  double get_rms() const;
  double get_cms() const;

protected:
  double m_qts;  /// total Q
  double m_vas;  /// equivalent volume
  double m_fs;   /// free air resonanse frequency
  double m_rdc;  /// voice coil resistance
  double m_lvc;  /// voice coil inductance (mH)
  double m_qms;  /// mechanical Q
  double m_qes;  /// electrical Q
  double m_imp;  /// nominal impedance
  double m_sens; /// sensitivity

  double m_mmd; /// cone mass, kg
  double m_ad;  /// effective radius, m
  double m_bl;  /// force factor
  double m_rms; /// suspension mechanical resistance, Ns/m
  double m_cms; /// suspension compliance, m/N

  std::string m_freq_resp_filename;
  std::map<double, double> m_freq_resp;
  std::string m_imp_resp_filename;
  std::map<double, double> m_imp_resp;

  std::string m_id_string;

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
  void parse_mmd(xmlNodePtr node);
  void parse_ad(xmlNodePtr node);
  void parse_bl(xmlNodePtr node);
  void parse_rms(xmlNodePtr node);
  void parse_cms(xmlNodePtr node);
};

#endif
