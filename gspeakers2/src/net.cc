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

#include <glib/gstrfuncs.h>
#include <sstream>
#include <fstream>
#include "net.h"
#include "../config.h"
#include "speaker.h"

Net::Net(int type, int lowpass_order, int highpass_order, bool has_imp_corr, 
         bool has_damp, bool has_res, int family) : GSpeakersObject()
{
  m_type = type;
  m_highpass_order = highpass_order;
  m_lowpass_order = lowpass_order;
  m_has_imp_corr = has_imp_corr;
  m_has_damp = has_damp;
  m_has_res = has_res;
  m_lowpass_family = family;
  m_highpass_family = family;
  m_speaker = "";
  
  /* Init lowpass filter if present */
  if (m_type & NET_TYPE_LOWPASS) {
    //cout << "net #" << get_id() << ": adding lowpass filter" << endl;
    switch (lowpass_order) {
      case NET_ORDER_1ST:
        m_parts.push_back(Part(PART_TYPE_INDUCTOR));
        break;
      case NET_ORDER_2ND:
        m_parts.push_back(Part(PART_TYPE_INDUCTOR));
        m_parts.push_back(Part(PART_TYPE_CAPACITOR));
        break;
      case NET_ORDER_3RD:
        m_parts.push_back(Part(PART_TYPE_INDUCTOR));
        m_parts.push_back(Part(PART_TYPE_CAPACITOR));
        m_parts.push_back(Part(PART_TYPE_INDUCTOR));
        break;
      case NET_ORDER_4TH:
        m_parts.push_back(Part(PART_TYPE_INDUCTOR));
        m_parts.push_back(Part(PART_TYPE_CAPACITOR));
        m_parts.push_back(Part(PART_TYPE_INDUCTOR));
        m_parts.push_back(Part(PART_TYPE_CAPACITOR));
        break;
    }
  }
  
  /* Init highpass filter if present */
  if (m_type & NET_TYPE_HIGHPASS) {
    //cout << "net #" << get_id() << ": adding highpass filter" << endl;  
    switch (highpass_order) {
      case NET_ORDER_1ST:
        m_parts.push_back(Part(PART_TYPE_CAPACITOR));      
        break;
      case NET_ORDER_2ND:
        m_parts.push_back(Part(PART_TYPE_CAPACITOR));      
        m_parts.push_back(Part(PART_TYPE_INDUCTOR));      
        break;
      case NET_ORDER_3RD:
        m_parts.push_back(Part(PART_TYPE_CAPACITOR));      
        m_parts.push_back(Part(PART_TYPE_INDUCTOR));      
        m_parts.push_back(Part(PART_TYPE_CAPACITOR));            
        break;
      case NET_ORDER_4TH:
        m_parts.push_back(Part(PART_TYPE_CAPACITOR));      
        m_parts.push_back(Part(PART_TYPE_INDUCTOR));      
        m_parts.push_back(Part(PART_TYPE_CAPACITOR));            
        m_parts.push_back(Part(PART_TYPE_INDUCTOR));            
        break;
    }
  }
  if (has_imp_corr == true)
  {
    //cout << "net #" << get_id() << ": adding impedance correction network" << endl;    
    m_imp_corr_R = Part(PART_TYPE_RESISTOR, 5.6, "");
    m_imp_corr_C = Part(PART_TYPE_CAPACITOR);
  }
  if (has_damp == true)
  {
    //cout << "net #" << get_id() << ": adding damping network" << endl;
    m_damp_R1 = Part(PART_TYPE_RESISTOR, 1, "");
    m_damp_R2 = Part(PART_TYPE_RESISTOR, 1, "");
  }
  if (has_res == true)
  {
    //cout << "net #" << get_id() << ": adding resonanse cicuit" << endl;
    m_res_R = Part(PART_TYPE_RESISTOR, 1, "");
    m_res_C = Part(PART_TYPE_CAPACITOR, 1, "u");
    m_res_L = Part(PART_TYPE_INDUCTOR, 1, "m");
  }
}

Net::Net(xmlNodePtr parent) 
{
  if ((parent != NULL) && (g_strcasecmp( (char *)parent->name, "net" ) == 0)) {  
    try {
      parse_type(parent->children);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Net: net node expected");
  }
  

}

void Net::parse_type(xmlNodePtr node) 
{
  if ((node != NULL) && (g_strcasecmp( (char *)node->name, "type" ) == 0)) {  
    istringstream((char *)xmlNodeGetContent(node)) >> m_type;
    try {
      parse_lowpass_order(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Net: type node expected");
  }
}

void Net::parse_lowpass_order(xmlNodePtr node)
{
  if ((node != NULL) && (g_strcasecmp( (char *)node->name, "lowpass_order" ) == 0)) {  
    istringstream((char *)xmlNodeGetContent(node)) >> m_lowpass_order;
    try {
      parse_highpass_order(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Net: lowpass_order node expected");
  }
}

void Net::parse_highpass_order(xmlNodePtr node)
{
  if ((node != NULL) && (g_strcasecmp( (char *)node->name, "highpass_order" ) == 0)) {  
    istringstream((char *)xmlNodeGetContent(node)) >> m_highpass_order;
    try {
      parse_has_imp_corr(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Net: highpass_order node expected");
  }
  
}

void Net::parse_has_imp_corr(xmlNodePtr node)
{
  if ((node != NULL) && (g_strcasecmp( (char *)node->name, "has_imp_corr" ) == 0)) {  
    if (g_strcasecmp( (char *)xmlNodeGetContent(node), "1" ) == 0) {
      m_has_imp_corr = true;
    } else {
      m_has_imp_corr = false;
    }
    
    try {
      parse_has_damp(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Net: has_imp_corr node expected");
  }

}

void Net::parse_has_damp(xmlNodePtr node)
{
  if ((node != NULL) && (g_strcasecmp( (char *)node->name, "has_damp" ) == 0)) {  

    if (g_strcasecmp( (char *)xmlNodeGetContent(node), "1" ) == 0) {
      m_has_damp = true;
    } else {
      m_has_damp = false;
    }

    try {
      parse_has_res(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Net: has_damp node expected");
  }

}

void Net::parse_has_res(xmlNodePtr node)
{
  if ((node != NULL) && (g_strcasecmp( (char *)node->name, "has_res" ) == 0)) {  

    if (g_strcasecmp( (char *)xmlNodeGetContent(node), "1" ) == 0) {
      m_has_res = true;
    } else {
      m_has_res = false;
    }

    try {
      parse_parts(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Net: has_res node expected");
  }

}


void Net::parse_parts(xmlNodePtr node)
{
  xmlNodePtr child;
  if ((node != NULL) && (g_strcasecmp( (char *)node->name, "parts" ) == 0)) {  
    /* Iterate over node->children, first comes the imp corr and damping network */
    child = node->children;
    /* Impedance correction parts */
    if (m_has_imp_corr == true) {
      //cout << "Parsing for impedance correction" << endl;   
      try {
        m_imp_corr_R = Part(child);
      } catch (GSpeakersException e) {
        throw e;
      } 
      child = child->next;
      try {
        m_imp_corr_C = Part(child);
      } catch (GSpeakersException e) {
        throw e;
      } 
      child = child->next;
    }
    /* Damping network parts */
    if (m_has_damp == true) {
      //cout << "Parsing for damping network" << endl;
      try {
        m_damp_R1 = Part(child);
      } catch (GSpeakersException e) {
        throw e;
      } 
      child = child->next;
      try {
        m_damp_R2 = Part(child);
      } catch (GSpeakersException e) {
        throw e;
      } 
      child = child->next;
    }
    /* Resonanse circuit */
    if (m_has_res == true) {
      //cout << "Parsing for resonanse circuit" << endl;
      try {
        m_res_R = Part(child);
      } catch (GSpeakersException e) {
        throw e;
      } 
      child = child->next;
      try {
        m_res_C = Part(child);
      } catch (GSpeakersException e) {
        throw e;
      } 
      child = child->next;
      try {
        m_res_L = Part(child);
      } catch (GSpeakersException e) {
        throw e;
      } 
      child = child->next;
    }
    
    /* Other parts */
    while (child != NULL) {
      //cout << "Parsing for an additional part" << endl;
      try {
        m_parts.push_back(Part(child));
      } catch (GSpeakersException e) {
        throw e;
      }
      child = child->next;
    }
    //cout << "Found #" << m_parts.size() << " parts" << endl;
    try {
      parse_lowpass_family(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }

  } else {
    throw GSpeakersException("Net: parts node expected");
  }
}

void Net::parse_lowpass_family(xmlNodePtr node)
{
  if ((node != NULL) && (g_strcasecmp( (char *)node->name, "lowpass_family" ) == 0)) {  
    istringstream((char *)xmlNodeGetContent(node)) >> m_lowpass_family;
    //m_family = 1;
    try {
      parse_highpass_family(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Net::parse_lowpass_family: lowpass_family node expected");
  }

}

void Net::parse_highpass_family(xmlNodePtr node)
{
  if ((node != NULL) && (g_strcasecmp( (char *)node->name, "highpass_family" ) == 0)) {  
    istringstream((char *)xmlNodeGetContent(node)) >> m_highpass_family;
    //m_family = 1;
    try {
      parse_speaker(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
    
  } else {
    throw GSpeakersException("Net::parse_highpass_family: highpass_family node expected");
  }

}

void Net::parse_speaker(xmlNodePtr node)
{
  if ((node != NULL) && (g_strcasecmp( (char *)node->name, "speaker" ) == 0)) {  
    m_speaker = string((char *)xmlNodeGetContent(node));
    //m_family = 1;
  } else {
    throw GSpeakersException("Net::parse_highpass_family: speaker node expected");
  }
}


xmlNodePtr Net::to_xml_node(xmlNodePtr parent) 
{
  xmlNodePtr net, field;
  
  net = xmlNewChild( parent, NULL, (xmlChar *)("net"), NULL );

  field = xmlNewChild( net, NULL, (xmlChar *)("type"), NULL );
  xmlNodeSetContent( field, (xmlChar *)g_strdup_printf("%d", m_type));
  field = xmlNewChild( net, NULL, (xmlChar *)("lowpass_order"), NULL );
  xmlNodeSetContent( field, (xmlChar *)g_strdup_printf("%d", m_lowpass_order));
  field = xmlNewChild( net, NULL, (xmlChar *)("highpass_order"), NULL );
  xmlNodeSetContent( field, (xmlChar *)g_strdup_printf("%d", m_highpass_order));

  field = xmlNewChild( net, NULL, (xmlChar *)("has_imp_corr"), NULL );
  xmlNodeSetContent( field, (xmlChar *)g_strdup_printf("%d", m_has_imp_corr));
  field = xmlNewChild( net, NULL, (xmlChar *)("has_damp"), NULL );
  xmlNodeSetContent( field, (xmlChar *)g_strdup_printf("%d", m_has_damp));
  field = xmlNewChild( net, NULL, (xmlChar *)("has_res"), NULL );
  xmlNodeSetContent( field, (xmlChar *)g_strdup_printf("%d", m_has_res));
  field = xmlNewChild( net, NULL, (xmlChar *)("parts"), NULL );  
  /* Insert impedance correction and damping network first in parts section */
  if (m_has_imp_corr == true) {
    m_imp_corr_R.to_xml_node(field);
    m_imp_corr_C.to_xml_node(field);    
  }
  if (m_has_damp == true) {
    m_damp_R1.to_xml_node(field);
    m_damp_R2.to_xml_node(field);    
  }
  if (m_has_res == true) {
    m_res_R.to_xml_node(field);
    m_res_C.to_xml_node(field);    
    m_res_L.to_xml_node(field);    
  }

  for(
    vector<Part>::iterator from = m_parts.begin();
    from != m_parts.end();
    ++from)
  {
    ((Part)(*from)).to_xml_node(field);
  }
  field = xmlNewChild( net, NULL, (xmlChar *)("lowpass_family"), NULL );
  xmlNodeSetContent( field, (xmlChar *)g_strdup_printf("%d", m_lowpass_family));
  field = xmlNewChild( net, NULL, (xmlChar *)("highpass_family"), NULL );
  xmlNodeSetContent( field, (xmlChar *)g_strdup_printf("%d", m_highpass_family));
  field = xmlNewChild( net, NULL, (xmlChar *)("speaker"), NULL );
  xmlNodeSetContent( field, (xmlChar *)m_speaker.c_str());
  
  return net;
}

string Net::to_SPICE(Speaker& s)
{
  string tmp_dir = Glib::get_tmp_dir();
  string tmp_file = tmp_dir + "/net" + GSpeakers::int_to_ustring(m_id) + ".tmp";
  
  int node_counter = 0;
  int part_index = 0;
  int next_node_cnt_inc = 1;
  
  ofstream of(tmp_file.c_str());
  if (of.good()) {
    of << "Crossover network SPICE code generated by GSpeakers-" << VERSION << endl;
    of << "vamp " << ++node_counter << " " << 0 <<  " ac" << endl;
    if (m_lowpass_order > 0) {
      switch (m_lowpass_order) {
        case NET_ORDER_1ST:
          of << "L" << m_parts[part_index].get_id() << " " << ++node_counter << " " << node_counter - 1 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
          break;
        case NET_ORDER_2ND:
          of << "L" << m_parts[part_index].get_id() << " " << ++node_counter << " " << node_counter - 1 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
          of << "C" << m_parts[part_index].get_id() << " " << node_counter << " " << 0 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
          break;
        case NET_ORDER_3RD:
          of << "L" << m_parts[part_index].get_id() << " " << ++node_counter << " " << node_counter - 1 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
          of << "C" << m_parts[part_index].get_id() << " " << node_counter << " " << 0 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
          of << "L" << m_parts[part_index].get_id() << " " << ++node_counter << " " << node_counter - 1 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
          break;
        case NET_ORDER_4TH:
          of << "L" << m_parts[part_index].get_id() << " " << ++node_counter << " " << node_counter - 1 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
          of << "C" << m_parts[part_index].get_id() << " " << node_counter << " " << 0 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
          of << "L" << m_parts[part_index].get_id() << " " << ++node_counter << " " << node_counter - 1 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
          of << "C" << m_parts[part_index].get_id() << " " << node_counter << " " << 0 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
          break;
      }
    }
    if (m_highpass_order > 0) {
      switch (m_highpass_order) {
        case NET_ORDER_1ST:
          of << "C" << m_parts[part_index].get_id() << " " << ++node_counter << " " << node_counter - 1 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
          
          break;
        case NET_ORDER_2ND:
          of << "C" << m_parts[part_index].get_id() << " " << ++node_counter << " " << node_counter - 1 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;

          of << "L" << m_parts[part_index].get_id() << " " << node_counter << " " << 0 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
        
          break;
        case NET_ORDER_3RD:
          of << "C" << m_parts[part_index].get_id() << " " << ++node_counter << " " << node_counter - 1 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;

          of << "L" << m_parts[part_index].get_id() << " " << node_counter << " " << 0 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
          of << "C" << m_parts[part_index].get_id() << " " << ++node_counter << " " << node_counter - 1 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
        
          break;
        case NET_ORDER_4TH:
          of << "C" << m_parts[part_index].get_id() << " " << ++node_counter << " " << node_counter - 1 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;

          of << "L" << m_parts[part_index].get_id() << " " << node_counter << " " << 0 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
          of << "C" << m_parts[part_index].get_id() << " " << ++node_counter << " " << node_counter - 1 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
          of << "L" << m_parts[part_index].get_id() << " " << node_counter << " " << 0 << " " << 
                m_parts[part_index].get_value() << m_parts[part_index].get_unit() << endl;
          part_index++;
        
          break;
      }
    }
    if (m_has_imp_corr == true) {
      of << "R" << m_imp_corr_R.get_id() << " " << node_counter << " " << node_counter + 1 << " " << 
            m_imp_corr_C.get_value() << endl;
      of << "C" << m_imp_corr_C.get_id() << " " << node_counter + 1 << " " << 0 << " " << 
            m_imp_corr_C.get_value() << m_imp_corr_C.get_unit() << endl;
      next_node_cnt_inc = 2;
    } else {
      next_node_cnt_inc = 1;
    }
    if (m_has_damp == true) {
      of << "R" << m_damp_R1.get_id() << " " << node_counter << " " << 0 << " " << 
            m_damp_R1.get_value() << endl;
      of << "R" << m_damp_R2.get_id() << " " << node_counter << " " << node_counter + next_node_cnt_inc << " " << 
            m_damp_R2.get_value() << endl;
      node_counter = node_counter + next_node_cnt_inc;
    
    }
    /* insert speaker resistance, TODO: insert speaker impedance table */
    of << "R" << s.get_id() << " " << node_counter << " " << 0 << " " << s.get_rdc() << endl;
    of << ".ac DEC 10 20 20k" << endl;
    of << ".print ac db(v(" << node_counter << "))" << endl;
    of << ".end" << endl;
    of.close();
  } else {
    throw GSpeakersException("Net::to_SPICE: could not write " + tmp_file);
  }
  return tmp_file;
}

std::ostream& operator<< (std::ostream& o, const Net& net)
{
  o << "***Net*** ******" << endl <<
       "Id:       " << net.m_id << endl << 
       "Type:     " << net.m_type << endl <<
       "Highpass #" << net.m_highpass_order << endl <<
       "Lowpass  #" << net.m_lowpass_order << endl <<
       "Has imp corr: " << net.m_has_imp_corr << endl <<
       "Has damping : " << net.m_has_damp << endl <<
       "Has resonanse circuit: " << net.m_has_res << endl;
  o << "Parts:" << endl;
  
  /* Print every part in this net */
  for (vector<Part>::iterator from = ((vector<Part>)(net.m_parts)).begin();
       from != ((vector<Part>)(net.m_parts)).end();
       ++from)
  {
    o << *from;
  }
  return o << "********* ******" << endl;
}

vector<Part> *Net::parts()
{
  
  return &m_parts;
}

string Net::get_speaker() 
{
  return m_speaker;
}   

int Net::get_lowpass_family()
{
  return m_lowpass_family;
}

int Net::get_highpass_family()
{
  return m_highpass_family;
}

int Net::get_highpass_order()
{
  return m_highpass_order;
}

int Net::get_lowpass_order()
{
  return m_lowpass_order;
}

bool Net::get_has_imp_corr()
{
  return m_has_imp_corr;
}

bool Net::get_has_damp()
{
  return m_has_damp;
}

bool Net::get_has_res()
{
  return m_has_res;
}

Part& Net::get_imp_corr_R()
{
  return m_imp_corr_R;
}

Part& Net::get_imp_corr_C()
{
  return m_imp_corr_C;
}

Part& Net::get_damp_R1()
{
  return m_damp_R1;
}

Part& Net::get_damp_R2()
{
  return m_damp_R2;
}

Part& Net::get_res_R()
{
  return m_res_R;
}

Part& Net::get_res_C()
{
  return m_res_C;
}

Part& Net::get_res_L()
{
  return m_res_L;
}



void Net::set_highpass_order(int order)
{
  if ((order >= 0) && (order <= 4)) {
    setup_net_by_order(order, NET_TYPE_HIGHPASS);
    m_highpass_order = order;
    if (order > 0) {
      m_type = m_type | NET_TYPE_HIGHPASS;
    } else {
      m_type = m_type & ~NET_TYPE_HIGHPASS;
    }
    //cout << "------net type: " << m_type << endl;
  }
}

void Net::set_lowpass_order(int order)
{
  if ((order >= 0) && (order <= 4)) {  
    setup_net_by_order(order, NET_TYPE_LOWPASS);
    m_lowpass_order = order;
    if (order > 0) {
      m_type = m_type | NET_TYPE_LOWPASS;
    } else {
      m_type = m_type & ~NET_TYPE_LOWPASS;
    }
    //cout << "------net type: " << m_type << endl;
  }
}

void Net::set_has_imp_corr(bool has_imp_corr)
{
  m_has_imp_corr = has_imp_corr;
  if (has_imp_corr == true)
  {
    //cout << "net #" << get_id() << ": adding impedance correction network" << endl;    
    m_imp_corr_R = Part(PART_TYPE_RESISTOR, 5.6, "");
    m_imp_corr_C = Part(PART_TYPE_CAPACITOR);
  }

}

void Net::set_has_damp(bool has_damp)
{
  m_has_damp = has_damp;
  if (has_damp == true)
  {
    //cout << "net #" << get_id() << ": adding damping network" << endl;
    m_damp_R1 = Part(PART_TYPE_RESISTOR, 1, "");
    m_damp_R2 = Part(PART_TYPE_RESISTOR, 1, "");
  }

}

void Net::set_speaker(string speaker)
{
  m_speaker = speaker;
}

void Net::set_lowpass_family(int family) 
{
  m_lowpass_family = family;
}

void Net::set_highpass_family(int family) 
{
  m_highpass_family = family;
}

void Net::set_imp_corr_R(Part p)
{
  m_imp_corr_R = p;
}

void Net::set_imp_corr_C(Part p)
{
  m_imp_corr_C = p;
}

void Net::set_damp_R1(Part p)
{
  m_damp_R1 = p;
}

void Net::set_damp_R2(Part p)
{
  m_damp_R2 = p;
}

void Net::set_res_R(Part p)
{
  m_res_R = p;
}
void Net::set_res_C(Part p)
{
  m_res_C = p;
}
void Net::set_res_L(Part p)
{
  m_res_L = p;
}


/*
 * This function will set a new order in one subfilter
 * It will also remove or add parts to fit the new order of the filter
 * 
 */
void Net::setup_net_by_order(int new_order, int which_net) {
  vector<Part>::iterator iter = m_parts.begin();
  
  if (which_net == NET_TYPE_LOWPASS) {
    /* Find out how much we should increase (or decrease) filter order */
    int diff = new_order - m_lowpass_order;  
    if (diff > 0) {  // if (new_order > m_lowpass_order
      /* increase filter order: add parts to the lwopass part of the net */
      switch (diff) {
        case 1:  // add one part
          switch (m_lowpass_order) {
            /* We will add the same part to a 0th order filter as we would add to a 2nd order filter */
            case NET_NOT_PRESENT:
            case NET_ORDER_2ND:
              m_parts.insert(iter + m_lowpass_order, Part(PART_TYPE_INDUCTOR));
              break;
            case NET_ORDER_1ST:
            case NET_ORDER_3RD:
              m_parts.insert(iter + m_lowpass_order, Part(PART_TYPE_CAPACITOR));
              break;
              /* Also we can't add 1 part to a 4th order filter so we don't use its identifier here */
            }
           break;
        case 2:
          switch (m_lowpass_order) {
            case NET_NOT_PRESENT:
            case NET_ORDER_2ND:
              /* Since we're inserting the parts...last one comes first in the vector, we insert them
                 in reversed order */
              iter = m_parts.insert(iter + m_lowpass_order, Part(PART_TYPE_CAPACITOR));                
              iter = m_parts.insert(iter, Part(PART_TYPE_INDUCTOR));
              break;    
            case NET_ORDER_1ST:
              iter = m_parts.insert(iter + m_lowpass_order, Part(PART_TYPE_INDUCTOR));                
              iter = m_parts.insert(iter, Part(PART_TYPE_CAPACITOR));
              break;
            
           }
           break;
         case 3:
           switch (m_lowpass_order) {
             case NET_NOT_PRESENT:
               iter = m_parts.insert(iter + m_lowpass_order, Part(PART_TYPE_INDUCTOR));
               iter = m_parts.insert(iter, Part(PART_TYPE_CAPACITOR));                
               iter = m_parts.insert(iter, Part(PART_TYPE_INDUCTOR));        
               break;
             case NET_ORDER_1ST:
               iter = m_parts.insert(iter + m_lowpass_order, Part(PART_TYPE_CAPACITOR));                
               iter = m_parts.insert(iter, Part(PART_TYPE_INDUCTOR));        
               iter = m_parts.insert(iter, Part(PART_TYPE_CAPACITOR));                              
               break;
           }
           break;
         case 4:
           if (m_lowpass_order == NET_NOT_PRESENT) 
           {
             iter = m_parts.insert(iter + m_lowpass_order, Part(PART_TYPE_CAPACITOR));                              
             iter = m_parts.insert(iter, Part(PART_TYPE_INDUCTOR));
             iter = m_parts.insert(iter, Part(PART_TYPE_CAPACITOR));                
             iter = m_parts.insert(iter, Part(PART_TYPE_INDUCTOR));        
           }
         break;
      }
    } else if (diff < 0) {
      /* Remove parts, remove all parts from the new filter order index up to the old last lowpass part */
      m_parts.erase(iter + new_order, iter + m_lowpass_order);
    }
  } else {
    int diff = new_order - m_highpass_order;  
    if (diff > 0) {  // if (new_order > m_lowpass_order
      /* add parts */
      switch (diff) {
        case 1:
          switch (m_highpass_order) {
            case NET_NOT_PRESENT:
            case NET_ORDER_2ND:
              /* Here we use push_back since we're always adding parts to the end of the m_parts vector */
              m_parts.push_back(Part(PART_TYPE_CAPACITOR));
              break;
            case NET_ORDER_1ST:
            case NET_ORDER_3RD:
              m_parts.push_back(Part(PART_TYPE_INDUCTOR));
              break;
            }
           break;
        case 2:

          switch (m_highpass_order) {
            case NET_NOT_PRESENT:
            case NET_ORDER_2ND:
              m_parts.push_back(Part(PART_TYPE_CAPACITOR));                
              m_parts.push_back(Part(PART_TYPE_INDUCTOR));
              break;    
            case NET_ORDER_1ST:
              m_parts.push_back(Part(PART_TYPE_INDUCTOR));
              m_parts.push_back(Part(PART_TYPE_CAPACITOR));
              break;
           }
           break;
         case 3:
           switch (m_highpass_order) {
             case NET_NOT_PRESENT:
               m_parts.push_back(Part(PART_TYPE_CAPACITOR));                
               m_parts.push_back(Part(PART_TYPE_INDUCTOR));        
               m_parts.push_back(Part(PART_TYPE_CAPACITOR));                              
              break;
             case NET_ORDER_1ST:
               m_parts.push_back(Part(PART_TYPE_INDUCTOR));
               m_parts.push_back(Part(PART_TYPE_CAPACITOR));                
               m_parts.push_back(Part(PART_TYPE_INDUCTOR));        
               break;
           }
           break;
         case 4:
           if (m_highpass_order == NET_NOT_PRESENT) {
             m_parts.push_back(Part(PART_TYPE_CAPACITOR));                
             m_parts.push_back(Part(PART_TYPE_INDUCTOR));        
             m_parts.push_back(Part(PART_TYPE_CAPACITOR));                              
             m_parts.push_back(Part(PART_TYPE_INDUCTOR));
           }
         break;
      }
    } else if (diff < 0) {
      /* Remove parts */
      m_parts.erase(iter + m_lowpass_order + new_order, iter + m_lowpass_order + m_highpass_order);
    }
  
  }
}
