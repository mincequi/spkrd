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

#include "speaker.h"
#include "common.h"
#include <glib.h>
#include <sstream>
#include <utility>

Speaker::Speaker(std::string id_string, int type, double qts, double vas, double fs, double rdc,
                 double lvc, double qms, double qes, double imp, double sens,
                 std::string freq_resp_filename, std::string imp_resp_filename, double mmd,
                 double ad, double bl, double rms, double cms)
    : GSpeakersObject() {
  m_id_string = std::move(id_string);
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
  m_mmd = mmd;
  m_ad = ad;
  m_bl = bl;
  m_rms = rms;
  m_cms = cms;

  m_freq_resp_filename = std::move(freq_resp_filename);
  m_imp_resp_filename = std::move(imp_resp_filename);
}

Speaker::Speaker(xmlNodePtr parent) {
  if ((parent != nullptr) && (std::string((char*)parent->name) == std::string("speaker"))) {
    try {
      parse_id_string(parent->children);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: speaker node not found"));
  }
}

xmlNodePtr Speaker::to_xml_node(xmlNodePtr parent) {
  xmlNodePtr speaker, child;
  auto* buffer = new gchar[10];

  speaker = xmlNewChild(parent, nullptr, (xmlChar*)("speaker"), nullptr);

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("id_string"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)m_id_string.c_str());

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("type"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)g_strdup_printf("%d", m_type));

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("qts"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_qts));

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("vas"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_vas));

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("fs"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_fs));

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("rdc"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_rdc));

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("lvc"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_lvc));

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("qms"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_qms));

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("qes"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_qes));

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("imp"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_imp));

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("sens"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_sens));

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("freq_resp_filename"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)m_freq_resp_filename.c_str());

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("imp_resp_filename"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)m_imp_resp_filename.c_str());

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("mmd"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_mmd));

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("ad"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_ad));

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("bl"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_bl));

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("rms"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_rms));

  child = xmlNewChild(speaker, nullptr, (xmlChar*)("cms"), nullptr);
  xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer, 8, m_cms));

  delete buffer;

  return speaker;
}

std::ostream& operator<<(std::ostream& o, const Speaker& speaker) {
  o << speaker.m_id_string << std::endl
    << _("Id:   ") << speaker.m_id << std::endl
    << _("Type: ") << speaker.m_type << std::endl
    << _("Qts:  ") << speaker.m_qts << std::endl
    << _("Vas:  ") << speaker.m_vas << std::endl
    << _("Fs:   ") << speaker.m_fs << std::endl;
  return o;
}

void Speaker::set_qts(double qts) { m_qts = qts; }

void Speaker::set_vas(double vas) { m_vas = vas; }

void Speaker::set_fs(double fs) { m_fs = fs; }

void Speaker::set_rdc(double rdc) { m_rdc = rdc; }

void Speaker::set_lvc(double lvc) { m_lvc = lvc; }

void Speaker::set_qms(double qms) { m_qms = qms; }

void Speaker::set_qes(double qes) { m_qes = qes; }

void Speaker::set_imp(double imp) { m_imp = imp; }

void Speaker::set_sens(double sens) { m_sens = sens; }

void Speaker::set_freq_resp_filename(std::string filename) {
  m_freq_resp_filename = std::move(filename);
}

void Speaker::set_freq_resp(std::map<double, double> freq_resp) {
  m_freq_resp = std::move(freq_resp);
}

void Speaker::set_imp_resp_filename(std::string filename) {
  m_imp_resp_filename = std::move(filename);
}

void Speaker::set_imp_resp(std::map<double, double> imp_resp) { m_imp_resp = std::move(imp_resp); }

void Speaker::set_id_string(std::string id_string) { m_id_string = std::move(id_string); }

void Speaker::set_mmd(double mmd) { m_mmd = mmd; }

void Speaker::set_ad(double ad) { m_ad = ad; }

void Speaker::set_bl(double bl) { m_bl = bl; }

void Speaker::set_rms(double rms) { m_rms = rms; }

void Speaker::set_cms(double cms) { m_cms = cms; }

double Speaker::get_qts() const { return m_qts; }

double Speaker::get_vas() const { return m_vas; }

double Speaker::get_fs() const { return m_fs; }

double Speaker::get_rdc() const { return m_rdc; }

double Speaker::get_lvc() const { return m_lvc; }

double Speaker::get_qms() const { return m_qms; }

double Speaker::get_qes() const { return m_qes; }

double Speaker::get_imp() const { return m_imp; }

double Speaker::get_sens() const { return m_sens; }

std::string Speaker::get_freq_resp_filename() const { return m_freq_resp_filename; }

std::map<double, double> const* Speaker::get_freq_resp() const { return &m_freq_resp; }

std::string Speaker::get_imp_resp_filename() const { return m_imp_resp_filename; }

std::map<double, double> const* Speaker::get_imp_resp() const { return &m_imp_resp; }

std::string const& Speaker::get_id_string() const { return m_id_string; }

double Speaker::get_mmd() const { return m_mmd; }

double Speaker::get_ad() const { return m_ad; }

double Speaker::get_bl() const { return m_bl; }

double Speaker::get_rms() const { return m_rms; }

double Speaker::get_cms() const { return m_cms; }

void Speaker::parse_id_string(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("id_string"))) {
    m_id_string = std::string((char*)xmlNodeGetContent(node));
    try {
      parse_type(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: id_string node not found"));
  }
}

void Speaker::parse_type(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("type"))) {
    std::istringstream((char*)xmlNodeGetContent(node)) >> m_type;
    try {
      parse_qts(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: type node not found"));
  }
}

void Speaker::parse_qts(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("qts"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_qts;
    m_qts = g_ascii_strtod((char*)xmlNodeGetContent(node), nullptr);
    try {
      parse_vas(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: qts node not found"));
  }
}

void Speaker::parse_vas(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("vas"))) {
    m_vas = g_ascii_strtod((char*)xmlNodeGetContent(node), nullptr);
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_vas;
    try {
      parse_fs(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: vas node not found"));
  }
}

void Speaker::parse_fs(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("fs"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_fs;
    m_fs = g_ascii_strtod((char*)xmlNodeGetContent(node), nullptr);
    try {
      parse_rdc(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: fs node not found"));
  }
}

void Speaker::parse_rdc(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("rdc"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_rdc;
    m_rdc = g_ascii_strtod((char*)xmlNodeGetContent(node), nullptr);
    try {
      parse_lcv(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: rdc node not found"));
  }
}

void Speaker::parse_lcv(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("lvc"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_lvc;
    m_lvc = g_ascii_strtod((char*)xmlNodeGetContent(node), nullptr);
    try {
      parse_qms(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: lvc node not found"));
  }
}

void Speaker::parse_qms(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("qms"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_qms;
    m_qms = g_ascii_strtod((char*)xmlNodeGetContent(node), nullptr);
    try {
      parse_qes(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: qms node not found"));
  }
}

void Speaker::parse_qes(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("qes"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_qes;
    m_qes = g_ascii_strtod((char*)xmlNodeGetContent(node), nullptr);
    try {
      parse_imp(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: qes node not found"));
  }
}

void Speaker::parse_imp(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("imp"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_imp;
    m_imp = g_ascii_strtod((char*)xmlNodeGetContent(node), nullptr);
    try {
      parse_sens(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: imp node not found"));
  }
}

void Speaker::parse_sens(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("sens"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_sens;
    m_sens = g_ascii_strtod((char*)xmlNodeGetContent(node), nullptr);
    try {
      parse_freq_resp_filename(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: sens node not found"));
  }
}

void Speaker::parse_freq_resp_filename(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("freq_resp_filename"))) {
    m_freq_resp_filename = std::string((char*)xmlNodeGetContent(node));
    try {
      parse_imp_resp_filename(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: freq_resp_filename node not found"));
  }
}

void Speaker::parse_imp_resp_filename(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("imp_resp_filename"))) {
    m_imp_resp_filename = std::string((char*)xmlNodeGetContent(node));
    try {
      parse_mmd(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: imp_resp_filename node not found"));
  }
}

void Speaker::parse_mmd(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("mmd"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_mmd;
    m_mmd = g_ascii_strtod((char*)xmlNodeGetContent(node), nullptr);
    try {
      parse_ad(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: mmd node not found"));
  }
}

void Speaker::parse_ad(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("ad"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_ad;
    m_ad = g_ascii_strtod((char*)xmlNodeGetContent(node), nullptr);
    try {
      parse_bl(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: ad node not found"));
  }
}

void Speaker::parse_bl(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("bl"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_bl;
    m_bl = g_ascii_strtod((char*)xmlNodeGetContent(node), nullptr);
    try {
      parse_rms(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: bl node not found"));
  }
}

void Speaker::parse_rms(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("rms"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_rms;
    m_rms = g_ascii_strtod((char*)xmlNodeGetContent(node), nullptr);
    try {
      parse_cms(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Speaker: rms node not found"));
  }
}

void Speaker::parse_cms(xmlNodePtr node) {
  if ((node != nullptr) && (std::string((char*)node->name) == std::string("cms"))) {
    // std::istringstream((char *)xmlNodeGetContent(node)) >> m_cms;
    m_cms = g_ascii_strtod((char*)xmlNodeGetContent(node), nullptr);
  } else {
    throw GSpeakersException(_("Speaker: mmd node not found"));
  }
}
