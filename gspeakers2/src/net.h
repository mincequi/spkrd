/*
  net Copyright (C) 2002 Daniel Sundberg

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

#ifndef __GFILTER_NET_H
#define __GFILTER_NET_H

#include <vector>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include "gspeakersobject.h"
#include "part.h"
#include "common.h"

/* Net types */
#define NET_TYPE_LOWPASS  1
#define NET_TYPE_HIGHPASS 2
#define NET_TYPE_BANDPASS 3
#define NET_NOT_PRESENT   0

/* Filter # */
#define NET_ORDER_1ST 1
#define NET_ORDER_2ND 2
#define NET_ORDER_3RD 3
#define NET_ORDER_4TH 4

/* filter family */
#define NET_BESSEL        1
#define NET_BUTTERWORTH   2
#define NET_CHEBYCHEV     3
#define NET_LINKWITZRILEY 4
#define NET_GAUSSIAN      5
#define NET_LEGENDRE      6
#define NET_LINEARPHASE   7

using namespace std;

/*
 * Net is a part of a crossover: one part of the crossover we want to simulate in SPICE
 *
 * For example: Lowpassfilter with impedance correction network or highpassfilter with
 *              damping network.
 *
 * TODO: Use c++ streams for input in Part(xmlNodePtr)
 */
class Net : public GSpeakersObject
{
public:
  /*
   * Construct new net object
   *
   * type = NET_TYPE_LOWPASS                        // lowpass filter
   * type = NET_TYPE_HIGHPASS                       // highpass filter
   * type = NET_TYPE_LOWPASS | NET_TYPE_HIGHPASS    // bandpass filter
   */
  Net(int type = NET_TYPE_LOWPASS, int lowpass_order = NET_ORDER_1ST, int highpass_order = NET_NOT_PRESENT, 
      bool has_imp_corr = false, bool has_damp = false, bool has_res = false, int family = NET_BUTTERWORTH);

  /* Construct a part from an xml node */
  Net(xmlNodePtr parent); //or create_from_xml(xmlNodePtr); depending on excetions and stuff like that
  
  /* Convert data for a part to an xml node, throws GSpeakersException on failure */
  xmlNodePtr to_xml_node(xmlNodePtr parent);   // Maybe this one should throw an exception

  /* Print part data to stdout */
  friend std::ostream& operator<< (std::ostream& o, const Net& net);

  /* Maybe we should add a toSPICE function here */
  string to_SPICE(Speaker& s);

  /* Use this to get the parts or add part to the net */
  vector<Part> *parts();

  int get_highpass_order();
  int get_lowpass_order();
  bool get_has_imp_corr();
  bool get_has_damp();
  bool get_has_res();
  int get_lowpass_family();
  int get_highpass_family();

  /* 
   * We return parts by ref to so that we not copy these parts.
   * For example, if we have:
   * Net n = net;
   * n.get_imp_corr_R().set_value(0.2);
   *
   * we don't access the part stored in n, we get a copy of that 
   * part that we modify, but if we return a ref we modify the 
   * part stored in Net
   */
  Part& get_imp_corr_R();
  Part& get_imp_corr_C();
  Part& get_damp_R1();
  Part& get_damp_R2();
  Part& get_res_R();
  Part& get_res_C();
  Part& get_res_L();
  
  void set_highpass_order(int order);
  void set_lowpass_order(int order);
  void set_has_imp_corr(bool has_imp_corr);
  void set_has_damp(bool has_damp);

  /* Here we don't want to use the ref since we want this class to have 
   * a unique copy of the part.
   */
  void set_imp_corr_R(Part p);
  void set_imp_corr_C(Part p);
  void set_damp_R1(Part p);
  void set_damp_R2(Part p);
  void set_res_R(Part p);
  void set_res_C(Part p);
  void set_res_L(Part p);
  
  void set_lowpass_family(int family);
  void set_highpass_family(int family);

protected:
  /* Member variables */
  // int m_id; /* from GSpeakersObject */
  // int m_type; /* from GSpeakersObject, we don't really need this one here... */
  int m_highpass_order;
  int m_lowpass_order;
  bool m_has_imp_corr;
  bool m_has_damp;
  bool m_has_res;
  Part m_imp_corr_R;
  Part m_imp_corr_C;
  Part m_damp_R1;
  Part m_damp_R2;
  Part m_res_R;
  Part m_res_C;
  Part m_res_L;
  vector<Part> m_parts;
  int m_lowpass_family;
  int m_highpass_family;

private:
  
  /*
   * This function will set a new order in one subfilter
   * It will also remove or add parts to fit the new order of the filter
   * 
   * new order: the new order for selected filter, NET_ORDER_1ST, ...
   * which filter: filter link to work on, NET_TYPE_LOWPASS, NET_TYPE_HIGHPASS
   *
   */
  void setup_net_by_order(int new_order, int which_net);
  
  /* xml parsing functions */
  void parse_type(xmlNodePtr node);
  void parse_highpass_order(xmlNodePtr node);
  void parse_lowpass_order(xmlNodePtr node);
  void parse_has_imp_corr(xmlNodePtr node);
  void parse_has_damp(xmlNodePtr node);
  void parse_has_res(xmlNodePtr node);
  void parse_parts(xmlNodePtr node);
  void parse_lowpass_family(xmlNodePtr node);
  void parse_highpass_family(xmlNodePtr node);
  
};

#endif
