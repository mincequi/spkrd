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

#include <glib/gstrfuncs.h>
#include <sstream>
#include "speaker.h"
#include "common.h"

Speaker::Speaker(string id_string, int type, double qts, double vas, double fs, double rdc, 
                 double lvc, double qms, double qes, double imp, double sens, 
                 string freq_resp_filename, string imp_resp_filename) : GSpeakersObject()
{
  m_id_string = id_string;
  m_type = type;
  m_qts = qts;
  m_vas = vas;
  m_fs = fs;
  m_rdc = rdc;
  m_lvc = lvc;
  m_qms = qms;
  m_qes = qes;
  m_imp = imp;
  m_sens = sens;
  
  m_freq_resp_filename = freq_resp_filename;
  m_imp_resp_filename = imp_resp_filename;
}

Speaker::Speaker(xmlNodePtr parent)
{
  if (( parent != NULL ) && ( string( (char *)parent->name) == string( "speaker" ))) {
    try {
      parse_id_string(parent->children);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Speaker: speaker node not found");
  }
}
  
xmlNodePtr Speaker::to_xml_node(xmlNodePtr parent)
{
  xmlNodePtr speaker, child;
  
  speaker = xmlNewChild( parent, NULL, (xmlChar *)("speaker"), NULL );

  child = xmlNewChild( speaker, NULL, (xmlChar *)("id_string"), NULL );
  xmlNodeSetContent( child, (xmlChar *)m_id_string.c_str());

  child = xmlNewChild( speaker, NULL, (xmlChar *)("type"), NULL );
  xmlNodeSetContent( child, (xmlChar *)g_strdup_printf("%d", m_type));

  child = xmlNewChild( speaker, NULL, (xmlChar *)("qts"), NULL );
  xmlNodeSetContent( child, (xmlChar *)g_strdup_printf("%f", m_qts));

  child = xmlNewChild( speaker, NULL, (xmlChar *)("vas"), NULL );
  xmlNodeSetContent( child, (xmlChar *)g_strdup_printf("%f", m_vas));

  child = xmlNewChild( speaker, NULL, (xmlChar *)("fs"), NULL );
  xmlNodeSetContent( child, (xmlChar *)g_strdup_printf("%f", m_fs));
  
  child = xmlNewChild( speaker, NULL, (xmlChar *)("rdc"), NULL );
  xmlNodeSetContent( child, (xmlChar *)g_strdup_printf("%f", m_rdc));

  child = xmlNewChild( speaker, NULL, (xmlChar *)("lvc"), NULL );
  xmlNodeSetContent( child, (xmlChar *)g_strdup_printf("%f", m_lvc));

  child = xmlNewChild( speaker, NULL, (xmlChar *)("qms"), NULL );
  xmlNodeSetContent( child, (xmlChar *)g_strdup_printf("%f", m_qms));

  child = xmlNewChild( speaker, NULL, (xmlChar *)("qes"), NULL );
  xmlNodeSetContent( child, (xmlChar *)g_strdup_printf("%f", m_qes));

  child = xmlNewChild( speaker, NULL, (xmlChar *)("imp"), NULL );
  xmlNodeSetContent( child, (xmlChar *)g_strdup_printf("%f", m_imp));

  child = xmlNewChild( speaker, NULL, (xmlChar *)("sens"), NULL );
  xmlNodeSetContent( child, (xmlChar *)g_strdup_printf("%f", m_sens));

  child = xmlNewChild( speaker, NULL, (xmlChar *)("freq_resp_filename"), NULL );
  xmlNodeSetContent( child, (xmlChar *)m_freq_resp_filename.c_str());

  child = xmlNewChild( speaker, NULL, (xmlChar *)("imp_resp_filename"), NULL );
  xmlNodeSetContent( child, (xmlChar *)m_imp_resp_filename.c_str());

  return speaker;
}

ostream& operator<< (ostream& o, const Speaker& speaker)
{
  o << speaker.m_id_string << endl <<
       "Id:   " << speaker.m_id << endl <<
       "Type: " << speaker.m_type << endl <<
       "Qts:  " << speaker.m_qts << endl << 
       "Vas:  " << speaker.m_vas << endl <<
       "Fs:   " << speaker.m_fs << endl;
  return o;
}

void Speaker::set_qts(double qts)
{
  m_qts = qts;
}

void Speaker::set_vas(double vas)
{
  m_vas = vas;
}

void Speaker::set_fs(double fs)
{
  m_fs = fs;
}

void Speaker::set_rdc(double rdc)
{
  m_rdc = rdc;
}

void Speaker::set_lvc(double lvc)
{
  m_lvc = lvc;
}

void Speaker::set_qms(double qms)
{
  m_qms = qms;
}

void Speaker::set_qes(double qes)
{
  m_qes = qes;
}

void Speaker::set_imp(double imp)
{
  m_imp = imp;
}

void Speaker::set_sens(double sens)
{
  m_sens = sens;
}

void Speaker::set_freq_resp_filename(string filename)
{
  m_freq_resp_filename = filename;
}

void Speaker::set_freq_resp(map<double, double> freq_resp)
{
  m_freq_resp = freq_resp;
}

void Speaker::set_imp_resp_filename(string filename)
{
  m_imp_resp_filename = filename;
}

void Speaker::set_imp_resp(map<double, double> imp_resp)
{
  m_imp_resp = imp_resp;
}

void Speaker::set_id_string(string id_string)
{
  m_id_string = id_string;
}
  
double Speaker::get_qts()
{
  return m_qts;
}

double Speaker::get_vas()
{
  return m_vas;
}

double Speaker::get_fs()
{
  return m_fs;
}

double Speaker::get_rdc()
{
  return m_rdc;
}

double Speaker::get_lvc()
{
  return m_lvc;
}

double Speaker::get_qms()
{
  return m_qms;
}

double Speaker::get_qes()
{
  return m_qes;
}

double Speaker::get_imp()
{
  return m_imp;
}

double Speaker::get_sens()
{
  return m_sens;
}

string Speaker::get_freq_resp_filename()
{
  return m_freq_resp_filename;
}

map<double, double> *Speaker::get_freq_resp()
{
  return &m_freq_resp;
}

string Speaker::get_imp_resp_filename()
{
  return m_imp_resp_filename;
}

map<double, double> *Speaker::get_imp_resp()
{
  return &m_imp_resp;
}

string Speaker::get_id_string()
{
  return m_id_string;
}

void Speaker::parse_id_string(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "id_string" ))) {
    m_id_string = string((char *)xmlNodeGetContent(node));
    try {
      parse_type(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Speaker: id_string node not found");
  }

}


void Speaker::parse_type(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "type" ))) {
    istringstream((char *)xmlNodeGetContent(node)) >> m_type;
    try {
      parse_qts(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Speaker: type node not found");
  }
  
}

void Speaker::parse_qts(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "qts" ))) {
    istringstream((char *)xmlNodeGetContent(node)) >> m_qts;
    try {
      parse_vas(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Speaker: qts node not found");
  }

}

void Speaker::parse_vas(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "vas" ))) {
    istringstream((char *)xmlNodeGetContent(node)) >> m_vas;
    try {
      parse_fs(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Speaker: vas node not found");
  }

}

void Speaker::parse_fs(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "fs" ))) {
    istringstream((char *)xmlNodeGetContent(node)) >> m_fs;
    try {
      parse_rdc(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Speaker: fs node not found");
  }

}

void Speaker::parse_rdc(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "rdc" ))) {
    istringstream((char *)xmlNodeGetContent(node)) >> m_rdc;
    try {
      parse_lcv(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Speaker: rdc node not found");
  }

}

void Speaker::parse_lcv(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "lvc" ))) {
    istringstream((char *)xmlNodeGetContent(node)) >> m_lvc;
    try {
      parse_qms(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Speaker: lvc node not found");
  }

}

void Speaker::parse_qms(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "qms" ))) {
    istringstream((char *)xmlNodeGetContent(node)) >> m_qms;
    try {
      parse_qes(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Speaker: qms node not found");
  }

}

void Speaker::parse_qes(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "qes" ))) {
    istringstream((char *)xmlNodeGetContent(node)) >> m_qes;
    try {
      parse_imp(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Speaker: qes node not found");
  }

}

void Speaker::parse_imp(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "imp" ))) {
    istringstream((char *)xmlNodeGetContent(node)) >> m_imp;
    try {
      parse_sens(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Speaker: imp node not found");
  }

}

void Speaker::parse_sens(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "sens" ))) {
    istringstream((char *)xmlNodeGetContent(node)) >> m_sens;
    try {
      parse_freq_resp_filename(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Speaker: sens node not found");
  }

}

void Speaker::parse_freq_resp_filename(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "freq_resp_filename" ))) {
    m_freq_resp_filename = string((char *)xmlNodeGetContent(node));
    try {
      parse_imp_resp_filename(node->next);
    } catch (GSpeakersException e) {
      throw e;
    }
  } else {
    throw GSpeakersException("Speaker: freq_resp_filename node not found");
  }

}

void Speaker::parse_imp_resp_filename(xmlNodePtr node)
{
  if (( node != NULL ) && ( string( (char *)node->name) == string( "imp_resp_filename" ))) {
    m_imp_resp_filename = string((char *)xmlNodeGetContent(node));
  } else {
    throw GSpeakersException("Speaker: imp_resp_filename node not found");
  }

}
