/*
  $Id$

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

#include "net.h"

#include "speaker.h"
#include <fstream>
#include <glib.h>
#include <sstream>

Net::Net(int type, int lowpass_order, int highpass_order, bool has_imp_corr, bool has_damp,
         bool has_res, int family, int adv_imp_model, bool inv_pol)
    : GSpeakersObject() {
  m_type = type;
  m_highpass_order = highpass_order;
  m_lowpass_order = lowpass_order;
  m_has_imp_corr = has_imp_corr;
  m_has_damp = has_damp;
  m_has_res = has_res;
  m_lowpass_family = family;
  m_highpass_family = family;
  m_speaker = "";
  m_adv_imp_model = adv_imp_model;
  m_inv_pol = inv_pol;

  /* Init lowpass filter if present */
  if (m_type & NET_TYPE_LOWPASS) {
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
  if (has_imp_corr == true) {
    m_imp_corr_R = Part(PART_TYPE_RESISTOR, 5.6, "");
    m_imp_corr_C = Part(PART_TYPE_CAPACITOR);
  }
  if (has_damp == true) {
    m_damp_R1 = Part(PART_TYPE_RESISTOR, 1, "");
    m_damp_R2 = Part(PART_TYPE_RESISTOR, 1, "");
  }
  if (has_res == true) {
    m_res_R = Part(PART_TYPE_RESISTOR, 1, "");
    m_res_C = Part(PART_TYPE_CAPACITOR, 1, "u");
    m_res_L = Part(PART_TYPE_INDUCTOR, 1, "m");
  }
}

Net::Net(xmlNodePtr parent) {
  if ((parent != NULL) && (g_ascii_strncasecmp((char*)parent->name, "net", 3) == 0)) {
    try {
      parse_type(parent->children);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Net: net node expected"));
  }
}

void Net::parse_type(xmlNodePtr node) {
  if ((node != NULL) && (g_ascii_strncasecmp((char*)node->name, "type", 4) == 0)) {
    std::istringstream((char*)xmlNodeGetContent(node)) >> m_type;
    try {
      parse_lowpass_order(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Net: type node expected"));
  }
}

void Net::parse_lowpass_order(xmlNodePtr node) {
  if ((node != NULL) && (g_ascii_strncasecmp((char*)node->name, "lowpass_order", 13) == 0)) {
    std::istringstream((char*)xmlNodeGetContent(node)) >> m_lowpass_order;
    try {
      parse_highpass_order(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Net: lowpass_order node expected"));
  }
}

void Net::parse_highpass_order(xmlNodePtr node) {
  if ((node != NULL) && (g_ascii_strncasecmp((char*)node->name, "highpass_order", 14) == 0)) {
    std::istringstream((char*)xmlNodeGetContent(node)) >> m_highpass_order;
    try {
      parse_has_imp_corr(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Net: highpass_order node expected"));
  }
}

void Net::parse_has_imp_corr(xmlNodePtr node) {
  if ((node != NULL) && (g_ascii_strncasecmp((char*)node->name, "has_imp_corr", 12) == 0)) {
    if (g_ascii_strncasecmp((char*)xmlNodeGetContent(node), "1", 1) == 0) {
      m_has_imp_corr = true;
    } else {
      m_has_imp_corr = false;
    }
    try {
      parse_has_damp(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Net: has_imp_corr node expected"));
  }
}

void Net::parse_has_damp(xmlNodePtr node) {
  if ((node != NULL) && (g_ascii_strncasecmp((char*)node->name, "has_damp", 8) == 0)) {
    if (g_ascii_strncasecmp((char*)xmlNodeGetContent(node), "1", 1) == 0) {
      m_has_damp = true;
    } else {
      m_has_damp = false;
    }
    try {
      parse_has_res(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Net: has_damp node expected"));
  }
}

void Net::parse_has_res(xmlNodePtr node) {
  if ((node != NULL) && (g_ascii_strncasecmp((char*)node->name, "has_res", 7) == 0)) {
    if (g_ascii_strncasecmp((char*)xmlNodeGetContent(node), "1", 1) == 0) {
      m_has_res = true;
    } else {
      m_has_res = false;
    }
    try {
      parse_parts(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Net: has_res node expected"));
  }
}

void Net::parse_parts(xmlNodePtr node) {
  xmlNodePtr child;
  if ((node != NULL) && (g_ascii_strncasecmp((char*)node->name, "parts", 5) == 0)) {
    /* Iterate over node->children, first comes the imp corr and damping network */
    child = node->children;
    /* Impedance correction parts */
    if (m_has_imp_corr == true) {
      try {
        m_imp_corr_R = Part(child);
      } catch (GSpeakersException const& e) {
        throw e;
      }
      child = child->next;
      try {
        m_imp_corr_C = Part(child);
      } catch (GSpeakersException const& e) {
        throw e;
      }
      child = child->next;
    }
    /* Damping network parts */
    if (m_has_damp == true) {
      try {
        m_damp_R1 = Part(child);
      } catch (GSpeakersException const& e) {
        throw e;
      }
      child = child->next;
      try {
        m_damp_R2 = Part(child);
      } catch (GSpeakersException const& e) {
        throw e;
      }
      child = child->next;
    }
    /* Resonanse circuit */
    if (m_has_res == true) {
      try {
        m_res_R = Part(child);
      } catch (GSpeakersException const& e) {
        throw e;
      }
      child = child->next;
      try {
        m_res_C = Part(child);
      } catch (GSpeakersException const& e) {
        throw e;
      }
      child = child->next;
      try {
        m_res_L = Part(child);
      } catch (GSpeakersException const& e) {
        throw e;
      }
      child = child->next;
    }

    /* Other parts */
    while (child != NULL) {
      try {
        m_parts.push_back(Part(child));
      } catch (GSpeakersException const& e) {
        throw e;
      }
      child = child->next;
    }
    try {
      parse_lowpass_family(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Net: parts node expected"));
  }
}

void Net::parse_lowpass_family(xmlNodePtr node) {
  if ((node != NULL) && (g_ascii_strncasecmp((char*)node->name, "lowpass_family", 14) == 0)) {
    std::istringstream((char*)xmlNodeGetContent(node)) >> m_lowpass_family;
    try {
      parse_highpass_family(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Net::parse_lowpass_family: lowpass_family node expected"));
  }
}

void Net::parse_highpass_family(xmlNodePtr node) {
  if ((node != NULL) && (g_ascii_strncasecmp((char*)node->name, "highpass_family", 15) == 0)) {
    std::istringstream((char*)xmlNodeGetContent(node)) >> m_highpass_family;
    try {
      parse_speaker(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Net::parse_highpass_family: highpass_family node expected"));
  }
}

void Net::parse_speaker(xmlNodePtr node) {
  if ((node != NULL) && (g_ascii_strncasecmp((char*)node->name, "speaker", 7) == 0)) {
    m_speaker = std::string((char*)xmlNodeGetContent(node));
    try {
      parse_adv_imp_model(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Net::parse_speaker: speaker node expected"));
  }
}

void Net::parse_adv_imp_model(xmlNodePtr node) {
  if ((node != NULL) && (g_ascii_strncasecmp((char*)node->name, "adv_imp_model", 13) == 0)) {
    std::istringstream((char*)xmlNodeGetContent(node)) >> m_adv_imp_model;
    try {
      parse_inv_pol(node->next);
    } catch (GSpeakersException const& e) {
      throw e;
    }
  } else {
    throw GSpeakersException(_("Net::parse_adv_imp_model: adv_imp_model node expected"));
  }
}

void Net::parse_inv_pol(xmlNodePtr node) {
  if ((node != NULL) && (g_ascii_strncasecmp((char*)node->name, "inv_pol", 7) == 0)) {
    std::istringstream((char*)xmlNodeGetContent(node)) >> m_inv_pol;
  } else {
    throw GSpeakersException(_("Net::parse_inv_pol: inv_pol node expected"));
  }
}

xmlNodePtr Net::to_xml_node(xmlNodePtr parent) {
  xmlNodePtr net, field;

  net = xmlNewChild(parent, NULL, (xmlChar*)("net"), NULL);

  field = xmlNewChild(net, NULL, (xmlChar*)("type"), NULL);
  xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_type));
  field = xmlNewChild(net, NULL, (xmlChar*)("lowpass_order"), NULL);
  xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_lowpass_order));
  field = xmlNewChild(net, NULL, (xmlChar*)("highpass_order"), NULL);
  xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_highpass_order));

  field = xmlNewChild(net, NULL, (xmlChar*)("has_imp_corr"), NULL);
  xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_has_imp_corr));
  field = xmlNewChild(net, NULL, (xmlChar*)("has_damp"), NULL);
  xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_has_damp));
  field = xmlNewChild(net, NULL, (xmlChar*)("has_res"), NULL);
  xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_has_res));
  field = xmlNewChild(net, NULL, (xmlChar*)("parts"), NULL);

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

  for (std::vector<Part>::iterator from = m_parts.begin(); from != m_parts.end(); ++from) {
    ((Part)(*from)).to_xml_node(field);
  }
  field = xmlNewChild(net, NULL, (xmlChar*)("lowpass_family"), NULL);
  xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_lowpass_family));
  field = xmlNewChild(net, NULL, (xmlChar*)("highpass_family"), NULL);
  xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_highpass_family));
  field = xmlNewChild(net, NULL, (xmlChar*)("speaker"), NULL);
  xmlNodeSetContent(field, (xmlChar*)m_speaker.c_str());
  field = xmlNewChild(net, NULL, (xmlChar*)("adv_imp_model"), NULL);
  xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_adv_imp_model));
  field = xmlNewChild(net, NULL, (xmlChar*)("inv_pol"), NULL);
  xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_inv_pol));

  return net;
}

std::string Net::to_SPICE(Speaker& s, bool use_gnucap) {
  std::string tmp_dir = Glib::get_tmp_dir();
#ifdef TARGET_WIN32
  std::string tmp_file = tmp_dir + "\\net" + GSpeakers::int_to_ustring(m_id) + ".tmp";
#else
  std::string tmp_file = tmp_dir + "/net" + GSpeakers::int_to_ustring(m_id) + ".tmp";
#endif

  int node_counter = 0;
  int part_index = 0;
  int next_node_cnt_inc = 1;
  gchar* buffer = new gchar[8];

  std::ofstream of(tmp_file.c_str());
  if (of.good()) {
    of << "Crossover network SPICE code generated by GSpeakers Redux-" << VERSION << std::endl;
    of << "vamp " << ++node_counter << " " << 0 << " dc 0 ac 1" << std::endl;
    if (m_lowpass_order > 0) {
      switch (m_lowpass_order) {
      case NET_ORDER_1ST:
        node_counter++;
        of << "L" << m_parts[part_index].get_id() << " " << node_counter << " " << node_counter - 1
           << " " << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        break;
      case NET_ORDER_2ND:
        node_counter++;
        of << "L" << m_parts[part_index].get_id() << " " << node_counter << " " << node_counter - 1
           << " " << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        of << "C" << m_parts[part_index].get_id() << " " << node_counter << " " << 0 << " "
           << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        break;
      case NET_ORDER_3RD:
        node_counter++;
        of << "L" << m_parts[part_index].get_id() << " " << node_counter << " " << node_counter - 1
           << " " << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        of << "C" << m_parts[part_index].get_id() << " " << node_counter << " " << 0 << " "
           << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        node_counter++;
        of << "L" << m_parts[part_index].get_id() << " " << node_counter << " " << node_counter - 1
           << " " << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        break;
      case NET_ORDER_4TH:
        node_counter++;
        of << "L" << m_parts[part_index].get_id() << " " << node_counter << " " << node_counter - 1
           << " " << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        of << "C" << m_parts[part_index].get_id() << " " << node_counter << " " << 0 << " "
           << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        node_counter++;
        of << "L" << m_parts[part_index].get_id() << " " << node_counter << " " << node_counter - 1
           << " " << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        of << "C" << m_parts[part_index].get_id() << " " << node_counter << " " << 0 << " "
           << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        break;
      }
    }
    if (m_highpass_order > 0) {
      switch (m_highpass_order) {
      case NET_ORDER_1ST:
        node_counter++;
        of << "C" << m_parts[part_index].get_id() << " " << node_counter << " " << node_counter - 1
           << " " << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        break;
      case NET_ORDER_2ND:
        node_counter++;
        of << "C" << m_parts[part_index].get_id() << " " << node_counter << " " << node_counter - 1
           << " " << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        of << "L" << m_parts[part_index].get_id() << " " << node_counter << " " << 0 << " "
           << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        break;
      case NET_ORDER_3RD:
        node_counter++;
        of << "C" << m_parts[part_index].get_id() << " " << node_counter << " " << node_counter - 1
           << " " << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        of << "L" << m_parts[part_index].get_id() << " " << node_counter << " " << 0 << " "
           << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        node_counter++;
        of << "C" << m_parts[part_index].get_id() << " " << node_counter << " " << node_counter - 1
           << " " << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        break;
      case NET_ORDER_4TH:
        node_counter++;
        of << "C" << m_parts[part_index].get_id() << " " << node_counter << " " << node_counter - 1
           << " " << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        of << "L" << m_parts[part_index].get_id() << " " << node_counter << " " << 0 << " "
           << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        node_counter++;
        of << "C" << m_parts[part_index].get_id() << " " << node_counter << " " << node_counter - 1
           << " " << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        of << "L" << m_parts[part_index].get_id() << " " << node_counter << " " << 0 << " "
           << g_ascii_dtostr(buffer, 8, m_parts[part_index].get_value())
           << m_parts[part_index].get_unit() << std::endl;
        part_index++;
        break;
      }
    }

    if (m_has_imp_corr == true) {
      of << "R" << m_imp_corr_R.get_id() << " " << node_counter << " " << node_counter + 1 << " "
         << g_ascii_dtostr(buffer, 8, m_imp_corr_R.get_value()) << std::endl;
      of << "C" << m_imp_corr_C.get_id() << " " << node_counter + 1 << " " << 0 << " "
         << g_ascii_dtostr(buffer, 8, m_imp_corr_C.get_value()) << m_imp_corr_C.get_unit()
         << std::endl;
      next_node_cnt_inc = 2;
    } else {
      next_node_cnt_inc = 1;
    }
    if (m_has_damp == true) {
      of << "R" << m_damp_R1.get_id() << " " << node_counter << " " << 0 << " "
         << g_ascii_dtostr(buffer, 8, m_damp_R1.get_value()) << std::endl;
      of << "R" << m_damp_R2.get_id() << " " << node_counter << " "
         << node_counter + next_node_cnt_inc << " "
         << g_ascii_dtostr(buffer, 8, m_damp_R2.get_value()) << std::endl;
      node_counter = node_counter + next_node_cnt_inc;
      next_node_cnt_inc = 1;
    }

    int spk_node = node_counter;

    /* insert speaker resistance, TODO: insert speaker impedance table */
    if (m_adv_imp_model == 1) {
      /* Complex model */
      double cmes = s.get_mmd() / (s.get_bl() * s.get_bl()) * 1000000;
      double lces = s.get_bl() * s.get_bl() * s.get_cms() * 1000;
      double res = s.get_bl() * s.get_bl() / s.get_rms();
      double po = 1.18; // air density kg/m^3
      double cmef =
          8 * po * s.get_ad() * s.get_ad() * s.get_ad() / (3 * s.get_bl() * s.get_bl()) * 1000000;
      of << "R" << s.get_id() << " " << node_counter << " " << node_counter + next_node_cnt_inc
         << " " << s.get_rdc() << std::endl;
      node_counter = node_counter + next_node_cnt_inc;
      of << "L" << s.get_id() << " " << node_counter << " " << node_counter + 1 << " "
         << s.get_lvc() << "mH" << std::endl;
      node_counter = node_counter + 1;
      of << "lces " << node_counter << " " << 0 << " " << g_ascii_dtostr(buffer, 8, lces) << "mH"
         << std::endl;
      of << "cmes " << node_counter << " " << 0 << " " << g_ascii_dtostr(buffer, 8, cmes) << "uF"
         << std::endl;
      of << "res " << node_counter << " " << 0 << " " << g_ascii_dtostr(buffer, 8, res)
         << std::endl;
      of << "cmef " << node_counter << " " << 0 << " " << g_ascii_dtostr(buffer, 8, cmef) << "uF"
         << std::endl;
    } else {
      /* simple model, model speaker as resistor */
      of << "R" << s.get_id() << " " << node_counter << " " << 0 << " "
         << g_ascii_dtostr(buffer, 8, s.get_rdc()) << std::endl;
    }

    if (use_gnucap == true) {
      of << ".print ac vdb(" << spk_node << ")" << std::endl;
      of << ".ac DEC 50 20 20k" << std::endl;
    } else {
      of << ".ac DEC 50 20 20k" << std::endl;
      of << ".print ac db(v(" << spk_node << "))" << std::endl;
    }
    of << ".end" << std::endl;
    of.close();
  } else {
    throw GSpeakersException("Net::to_SPICE: could not write " + tmp_file);
  }
  return tmp_file;
}

std::ostream& operator<<(std::ostream& o, const Net& net) {
  o << _("***Net*** ******") << std::endl
    << _("Id:       ") << net.m_id << std::endl
    << _("Type:     ") << net.m_type << std::endl
    << _("Highpass #") << net.m_highpass_order << std::endl
    << _("Lowpass  #") << net.m_lowpass_order << std::endl
    << _("Has imp corr: ") << net.m_has_imp_corr << std::endl
    << _("Has damping : ") << net.m_has_damp << std::endl
    << _("Has resonanse circuit: ") << net.m_has_res << std::endl;
  o << _("Parts:") << std::endl;

  /* Print every part in this net */
  for (std::vector<Part>::iterator from = ((std::vector<Part>)(net.m_parts)).begin();
       from != ((std::vector<Part>)(net.m_parts)).end(); ++from) {
    o << *from;
  }
  return o << _("********* ******") << std::endl;
}

std::vector<Part>* Net::parts() { return &m_parts; }

std::string Net::get_speaker() { return m_speaker; }

int Net::get_lowpass_family() { return m_lowpass_family; }

int Net::get_highpass_family() { return m_highpass_family; }

int Net::get_highpass_order() { return m_highpass_order; }

int Net::get_lowpass_order() { return m_lowpass_order; }

bool Net::get_has_imp_corr() { return m_has_imp_corr; }

bool Net::get_has_damp() { return m_has_damp; }

bool Net::get_has_res() { return m_has_res; }

Part& Net::get_imp_corr_R() { return m_imp_corr_R; }

Part& Net::get_imp_corr_C() { return m_imp_corr_C; }

Part& Net::get_damp_R1() { return m_damp_R1; }

Part& Net::get_damp_R2() { return m_damp_R2; }

Part& Net::get_res_R() { return m_res_R; }

Part& Net::get_res_C() { return m_res_C; }

Part& Net::get_res_L() { return m_res_L; }

int Net::get_adv_imp_model() { return m_adv_imp_model; }

bool Net::get_inv_pot() { return m_inv_pol; }

void Net::set_highpass_order(int order) {
  if ((order >= 0) && (order <= 4)) {
    setup_net_by_order(order, NET_TYPE_HIGHPASS);
    m_highpass_order = order;
    if (order > 0) {
      m_type = m_type | NET_TYPE_HIGHPASS;
    } else {
      m_type = m_type & ~NET_TYPE_HIGHPASS;
    }
  }
}

void Net::set_lowpass_order(int order) {
  if ((order >= 0) && (order <= 4)) {
    setup_net_by_order(order, NET_TYPE_LOWPASS);
    m_lowpass_order = order;
    if (order > 0) {
      m_type = m_type | NET_TYPE_LOWPASS;
    } else {
      m_type = m_type & ~NET_TYPE_LOWPASS;
    }
  }
}

void Net::set_has_imp_corr(bool has_imp_corr) {
  m_has_imp_corr = has_imp_corr;
  if (has_imp_corr == true) {
    m_imp_corr_R = Part(PART_TYPE_RESISTOR, 5.6, "");
    m_imp_corr_C = Part(PART_TYPE_CAPACITOR);
  }
}

void Net::set_has_damp(bool has_damp) {
  m_has_damp = has_damp;
  if (has_damp == true) {
    m_damp_R1 = Part(PART_TYPE_RESISTOR, 1, "");
    m_damp_R2 = Part(PART_TYPE_RESISTOR, 1, "");
  }
}

void Net::set_speaker(std::string speaker) { m_speaker = speaker; }

void Net::set_lowpass_family(int family) { m_lowpass_family = family; }

void Net::set_highpass_family(int family) { m_highpass_family = family; }

void Net::set_imp_corr_R(Part p) { m_imp_corr_R = p; }

void Net::set_imp_corr_C(Part p) { m_imp_corr_C = p; }

void Net::set_damp_R1(Part p) { m_damp_R1 = p; }

void Net::set_damp_R2(Part p) { m_damp_R2 = p; }

void Net::set_res_R(Part p) { m_res_R = p; }
void Net::set_res_C(Part p) { m_res_C = p; }
void Net::set_res_L(Part p) { m_res_L = p; }

void Net::set_adv_imp_model(int model) { m_adv_imp_model = model; }

void Net::set_inv_pol(bool pol) { m_inv_pol = pol; }

/*
 * This function will set a new order in one subfilter
 * It will also remove or add parts to fit the new order of the filter
 *
 */
void Net::setup_net_by_order(int new_order, int which_net) {
  std::vector<Part>::iterator iter = m_parts.begin();

  if (which_net == NET_TYPE_LOWPASS) {
    /* Find out how much we should increase (or decrease) filter order */
    int diff = new_order - m_lowpass_order;
    if (diff > 0) { // if (new_order > m_lowpass_order
      /* increase filter order: add parts to the lwopass part of the net */
      switch (diff) {
      case 1: // add one part
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
          /* Since we're inserting the parts...last one comes first in thestd::vector, we insert
             them in reversed order */
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
        if (m_lowpass_order == NET_NOT_PRESENT) {
          iter = m_parts.insert(iter + m_lowpass_order, Part(PART_TYPE_CAPACITOR));
          iter = m_parts.insert(iter, Part(PART_TYPE_INDUCTOR));
          iter = m_parts.insert(iter, Part(PART_TYPE_CAPACITOR));
          iter = m_parts.insert(iter, Part(PART_TYPE_INDUCTOR));
        }
        break;
      }
    } else if (diff < 0) {
      /* Remove parts, remove all parts from the new filter order index up to the old last lowpass
       * part */
      m_parts.erase(iter + new_order, iter + m_lowpass_order);
    }
  } else {
    int diff = new_order - m_highpass_order;
    if (diff > 0) { // if (new_order > m_lowpass_order
      /* add parts */
      switch (diff) {
      case 1:
        switch (m_highpass_order) {
        case NET_NOT_PRESENT:
        case NET_ORDER_2ND:
          /* Here we use push_back since we're always adding parts to the end of the
           * m_partsstd::vector
           */
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
