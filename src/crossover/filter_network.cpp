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

#include "filter_network.hpp"
#include "driver.hpp"

#include <glibmm.h>

#include <fstream>
#include <sstream>
#include <utility>

filter_network::filter_network(int type,
                               int lowpass_order,
                               int highpass_order,
                               bool has_imp_corr,
                               bool has_damp,
                               bool has_res,
                               int family,
                               int adv_imp_model,
                               bool inv_pol)
    : gspkObject(type),
      m_highpass_order(highpass_order),
      m_lowpass_order(lowpass_order),
      m_has_imp_corr(has_imp_corr),
      m_has_damp(has_damp),
      m_has_res(has_res),
      m_lowpass_family(family),
      m_highpass_family(family),
      m_adv_imp_model(adv_imp_model),
      m_inv_pol(inv_pol)
{
    // Init lowpass filter if present
    if (m_type == NET_TYPE_LOWPASS)
    {
        switch (lowpass_order)
        {
            case NET_ORDER_1ST:
                m_parts.emplace_back(PART_TYPE_INDUCTOR);
                break;
            case NET_ORDER_2ND:
                m_parts.emplace_back(PART_TYPE_INDUCTOR);
                m_parts.emplace_back(PART_TYPE_CAPACITOR);
                break;
            case NET_ORDER_3RD:
                m_parts.emplace_back(PART_TYPE_INDUCTOR);
                m_parts.emplace_back(PART_TYPE_CAPACITOR);
                m_parts.emplace_back(PART_TYPE_INDUCTOR);
                break;
            case NET_ORDER_4TH:
                m_parts.emplace_back(PART_TYPE_INDUCTOR);
                m_parts.emplace_back(PART_TYPE_CAPACITOR);
                m_parts.emplace_back(PART_TYPE_INDUCTOR);
                m_parts.emplace_back(PART_TYPE_CAPACITOR);
                break;
        }
    }

    // Init highpass filter if present
    if (m_type == NET_TYPE_HIGHPASS)
    {
        switch (highpass_order)
        {
            case NET_ORDER_1ST:
                m_parts.emplace_back(PART_TYPE_CAPACITOR);
                break;
            case NET_ORDER_2ND:
                m_parts.emplace_back(PART_TYPE_CAPACITOR);
                m_parts.emplace_back(PART_TYPE_INDUCTOR);
                break;
            case NET_ORDER_3RD:
                m_parts.emplace_back(PART_TYPE_CAPACITOR);
                m_parts.emplace_back(PART_TYPE_INDUCTOR);
                m_parts.emplace_back(PART_TYPE_CAPACITOR);
                break;
            case NET_ORDER_4TH:
                m_parts.emplace_back(PART_TYPE_CAPACITOR);
                m_parts.emplace_back(PART_TYPE_INDUCTOR);
                m_parts.emplace_back(PART_TYPE_CAPACITOR);
                m_parts.emplace_back(PART_TYPE_INDUCTOR);
                break;
        }
    }
    if (has_imp_corr)
    {
        m_imp_corr_R = passive_component(PART_TYPE_RESISTOR, 5.6, "");
        m_imp_corr_C = passive_component(PART_TYPE_CAPACITOR);
    }
    if (has_damp)
    {
        m_damp_R1 = passive_component(PART_TYPE_RESISTOR, 1, "");
        m_damp_R2 = passive_component(PART_TYPE_RESISTOR, 1, "");
    }
    if (has_res)
    {
        m_res_R = passive_component(PART_TYPE_RESISTOR, 1, "");
        m_res_C = passive_component(PART_TYPE_CAPACITOR, 1, "u");
        m_res_L = passive_component(PART_TYPE_INDUCTOR, 1, "m");
    }
}

filter_network::filter_network(xmlNodePtr parent)
{
    if (parent != nullptr && (g_ascii_strncasecmp((char*)parent->name, "net", 3) == 0))
    {
        try
        {
            parse_type(parent->children);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("filter_network: net node expected"));
    }
}

void filter_network::parse_type(xmlNodePtr node)
{
    if (node != nullptr && (g_ascii_strncasecmp((char*)node->name, "type", 4) == 0))
    {
        std::istringstream((char*)xmlNodeGetContent(node)) >> m_type;
        try
        {
            parse_lowpass_order(node->next);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("filter_network: type node expected"));
    }
}

void filter_network::parse_lowpass_order(xmlNodePtr node)
{
    if ((node != nullptr)
        && (g_ascii_strncasecmp((char*)node->name, "lowpass_order", 13) == 0))
    {
        std::istringstream((char*)xmlNodeGetContent(node)) >> m_lowpass_order;
        try
        {
            parse_highpass_order(node->next);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("filter_network: lowpass_order node expected"));
    }
}

void filter_network::parse_highpass_order(xmlNodePtr node)
{
    if ((node != nullptr)
        && (g_ascii_strncasecmp((char*)node->name, "highpass_order", 14) == 0))
    {
        std::istringstream((char*)xmlNodeGetContent(node)) >> m_highpass_order;
        try
        {
            parse_has_imp_corr(node->next);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("filter_network: highpass_order node expected"));
    }
}

void filter_network::parse_has_imp_corr(xmlNodePtr node)
{
    if ((node != nullptr)
        && (g_ascii_strncasecmp((char*)node->name, "has_imp_corr", 12) == 0))
    {
        m_has_imp_corr = g_ascii_strncasecmp((char*)xmlNodeGetContent(node), "1", 1) == 0;
        try
        {
            parse_has_damp(node->next);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("filter_network: has_imp_corr node expected"));
    }
}

void filter_network::parse_has_damp(xmlNodePtr node)
{
    if ((node != nullptr) && (g_ascii_strncasecmp((char*)node->name, "has_damp", 8) == 0))
    {
        m_has_damp = g_ascii_strncasecmp((char*)xmlNodeGetContent(node), "1", 1) == 0;

        try
        {
            parse_has_res(node->next);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("filter_network: has_damp node expected"));
    }
}

void filter_network::parse_has_res(xmlNodePtr node)
{
    if ((node != nullptr) && (g_ascii_strncasecmp((char*)node->name, "has_res", 7) == 0))
    {
        m_has_res = g_ascii_strncasecmp((char*)xmlNodeGetContent(node), "1", 1) == 0;

        try
        {
            parse_parts(node->next);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("filter_network: has_res node expected"));
    }
}

void filter_network::parse_parts(xmlNodePtr node)
{
    if ((node != nullptr) && (g_ascii_strncasecmp((char*)node->name, "parts", 5) == 0))
    {
        // Iterate over node->children, first comes the imp corr and damping network
        xmlNodePtr child = node->children;

        // Impedance correction parts
        if (m_has_imp_corr)
        {
            try
            {
                m_imp_corr_R = passive_component(child);
            }
            catch (std::runtime_error const& error)
            {
                throw error;
            }
            child = child->next;
            try
            {
                m_imp_corr_C = passive_component(child);
            }
            catch (std::runtime_error const& error)
            {
                throw error;
            }
            child = child->next;
        }
        /* Damping network parts */
        if (m_has_damp)
        {
            try
            {
                m_damp_R1 = passive_component(child);
            }
            catch (std::runtime_error const& error)
            {
                throw error;
            }
            child = child->next;
            try
            {
                m_damp_R2 = passive_component(child);
            }
            catch (std::runtime_error const& error)
            {
                throw error;
            }
            child = child->next;
        }
        // Resonance circuit
        if (m_has_res)
        {
            try
            {
                m_res_R = passive_component(child);
            }
            catch (std::runtime_error const& error)
            {
                throw error;
            }
            child = child->next;
            try
            {
                m_res_C = passive_component(child);
            }
            catch (std::runtime_error const& error)
            {
                throw error;
            }
            child = child->next;
            try
            {
                m_res_L = passive_component(child);
            }
            catch (std::runtime_error const& error)
            {
                throw error;
            }
            child = child->next;
        }

        // Other parts
        while (child != nullptr)
        {
            try
            {
                m_parts.emplace_back(child);
            }
            catch (std::runtime_error const& error)
            {
                throw error;
            }
            child = child->next;
        }
        try
        {
            parse_lowpass_family(node->next);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("filter_network: parts node expected"));
    }
}

void filter_network::parse_lowpass_family(xmlNodePtr node)
{
    if ((node != nullptr)
        && (g_ascii_strncasecmp((char*)node->name, "lowpass_family", 14) == 0))
    {
        std::istringstream((char*)xmlNodeGetContent(node)) >> m_lowpass_family;
        try
        {
            parse_highpass_family(node->next);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("filter_network::parse_lowpass_family: lowpass_family "
                                   "node "
                                   "expected"));
    }
}

void filter_network::parse_highpass_family(xmlNodePtr node)
{
    if ((node != nullptr)
        && (g_ascii_strncasecmp((char*)node->name, "highpass_family", 15) == 0))
    {
        std::istringstream((char*)xmlNodeGetContent(node)) >> m_highpass_family;
        try
        {
            parse_speaker(node->next);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("filter_network::parse_highpass_family: "
                                   "highpass_family node "
                                   "expected"));
    }
}

void filter_network::parse_speaker(xmlNodePtr node)
{
    if ((node != nullptr) && (g_ascii_strncasecmp((char*)node->name, "speaker", 7) == 0))
    {
        m_speaker = std::string((char*)xmlNodeGetContent(node));
        try
        {
            parse_adv_imp_model(node->next);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("filter_network::parse_speaker: speaker node "
                                   "expected"));
    }
}

void filter_network::parse_adv_imp_model(xmlNodePtr node)
{
    if ((node != nullptr)
        && (g_ascii_strncasecmp((char*)node->name, "adv_imp_model", 13) == 0))
    {
        std::istringstream((char*)xmlNodeGetContent(node)) >> m_adv_imp_model;
        try
        {
            parse_inv_pol(node->next);
        }
        catch (std::runtime_error const& error)
        {
            throw error;
        }
    }
    else
    {
        throw std::runtime_error(_("filter_network::parse_adv_imp_model: adv_imp_model "
                                   "node "
                                   "expected"));
    }
}

void filter_network::parse_inv_pol(xmlNodePtr node)
{
    if ((node != nullptr) && (g_ascii_strncasecmp((char*)node->name, "inv_pol", 7) == 0))
    {
        std::istringstream((char*)xmlNodeGetContent(node)) >> m_inv_pol;
    }
    else
    {
        throw std::runtime_error(_("filter_network::parse_inv_pol: inv_pol node "
                                   "expected"));
    }
}

auto filter_network::to_xml_node(xmlNodePtr parent) -> xmlNodePtr
{
    xmlNodePtr net = xmlNewChild(parent, nullptr, (xmlChar*)("net"), nullptr);
    xmlNodePtr field = xmlNewChild(net, nullptr, (xmlChar*)("type"), nullptr);

    xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_type));
    field = xmlNewChild(net, nullptr, (xmlChar*)("lowpass_order"), nullptr);
    xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_lowpass_order));
    field = xmlNewChild(net, nullptr, (xmlChar*)("highpass_order"), nullptr);
    xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_highpass_order));

    field = xmlNewChild(net, nullptr, (xmlChar*)("has_imp_corr"), nullptr);
    xmlNodeSetContent(field,
                      (xmlChar*)g_strdup_printf("%d", static_cast<int>(m_has_imp_corr)));
    field = xmlNewChild(net, nullptr, (xmlChar*)("has_damp"), nullptr);
    xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", static_cast<int>(m_has_damp)));
    field = xmlNewChild(net, nullptr, (xmlChar*)("has_res"), nullptr);
    xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", static_cast<int>(m_has_res)));
    field = xmlNewChild(net, nullptr, (xmlChar*)("parts"), nullptr);

    // Insert impedance correction and damping network first in parts section
    if (m_has_imp_corr)
    {
        m_imp_corr_R.to_xml_node(field);
        m_imp_corr_C.to_xml_node(field);
    }
    if (m_has_damp)
    {
        m_damp_R1.to_xml_node(field);
        m_damp_R2.to_xml_node(field);
    }
    if (m_has_res)
    {
        m_res_R.to_xml_node(field);
        m_res_C.to_xml_node(field);
        m_res_L.to_xml_node(field);
    }

    for (auto& m_part : m_parts)
    {
        m_part.to_xml_node(field);
    }
    field = xmlNewChild(net, nullptr, (xmlChar*)("lowpass_family"), nullptr);
    xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_lowpass_family));
    field = xmlNewChild(net, nullptr, (xmlChar*)("highpass_family"), nullptr);
    xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_highpass_family));
    field = xmlNewChild(net, nullptr, (xmlChar*)("speaker"), nullptr);
    xmlNodeSetContent(field, (xmlChar*)m_speaker.c_str());
    field = xmlNewChild(net, nullptr, (xmlChar*)("adv_imp_model"), nullptr);
    xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_adv_imp_model));
    field = xmlNewChild(net, nullptr, (xmlChar*)("inv_pol"), nullptr);
    xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", static_cast<int>(m_inv_pol)));

    return net;
}

auto filter_network::to_SPICE(driver const& s, bool use_gnucap) -> std::string
{
    std::string tmp_dir = Glib::get_tmp_dir();
#ifdef TARGET_WIN32
    std::string tmp_file = tmp_dir + "\\net" + gspk::int_to_ustring(m_id) + ".tmp";
#else
    std::string tmp_file = tmp_dir + "/net" + gspk::int_to_ustring(m_id) + ".tmp";
#endif

    int node_counter = 0;
    int part_index = 0;
    int next_node_cnt_inc = 1;
    auto* buffer = new gchar[8];

    std::ofstream of(tmp_file.c_str());
    if (of.good())
    {
        of << "Crossover network SPICE code generated by gspk " << std::string(VERSION)
           << "\n";
        of << "vamp " << ++node_counter << " " << 0 << " dc 0 ac 1"
           << "\n";
        if (m_lowpass_order > 0)
        {
            switch (m_lowpass_order)
            {
                case NET_ORDER_1ST:
                    node_counter++;
                    of << "L" << m_parts[part_index].get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    break;
                case NET_ORDER_2ND:
                    node_counter++;
                    of << "L" << m_parts[part_index].get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    of << "C" << m_parts[part_index].get_id() << " " << node_counter << " "
                       << 0 << " " << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    break;
                case NET_ORDER_3RD:
                    node_counter++;
                    of << "L" << m_parts[part_index].get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    of << "C" << m_parts[part_index].get_id() << " " << node_counter << " "
                       << 0 << " " << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    node_counter++;
                    of << "L" << m_parts[part_index].get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    break;
                case NET_ORDER_4TH:
                    node_counter++;
                    of << "L" << m_parts[part_index].get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    of << "C" << m_parts[part_index].get_id() << " " << node_counter << " "
                       << 0 << " " << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    node_counter++;
                    of << "L" << m_parts[part_index].get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    of << "C" << m_parts[part_index].get_id() << " " << node_counter << " "
                       << 0 << " " << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    break;
            }
        }
        if (m_highpass_order > 0)
        {
            switch (m_highpass_order)
            {
                case NET_ORDER_1ST:
                    node_counter++;
                    of << "C" << m_parts[part_index].get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    break;
                case NET_ORDER_2ND:
                    node_counter++;
                    of << "C" << m_parts[part_index].get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    of << "L" << m_parts[part_index].get_id() << " " << node_counter << " "
                       << 0 << " " << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    break;
                case NET_ORDER_3RD:
                    node_counter++;
                    of << "C" << m_parts[part_index].get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    of << "L" << m_parts[part_index].get_id() << " " << node_counter << " "
                       << 0 << " " << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    node_counter++;
                    of << "C" << m_parts[part_index].get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    break;
                case NET_ORDER_4TH:
                    node_counter++;
                    of << "C" << m_parts[part_index].get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    of << "L" << m_parts[part_index].get_id() << " " << node_counter << " "
                       << 0 << " " << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    node_counter++;
                    of << "C" << m_parts[part_index].get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    of << "L" << m_parts[part_index].get_id() << " " << node_counter << " "
                       << 0 << " " << std::to_string(m_parts[part_index].get_value())
                       << m_parts[part_index].get_unit() << "\n";
                    part_index++;
                    break;
            }
        }

        if (m_has_imp_corr)
        {
            of << "R" << m_imp_corr_R.get_id() << " " << node_counter << " "
               << node_counter + 1 << " " << std::to_string(m_imp_corr_R.get_value())
               << "\n";
            of << "C" << m_imp_corr_C.get_id() << " " << node_counter + 1 << " " << 0
               << " " << std::to_string(m_imp_corr_C.get_value())
               << m_imp_corr_C.get_unit() << "\n";
            next_node_cnt_inc = 2;
        }
        else
        {
            next_node_cnt_inc = 1;
        }
        if (m_has_damp)
        {
            of << "R" << m_damp_R1.get_id() << " " << node_counter << " " << 0 << " "
               << std::to_string(m_damp_R1.get_value()) << "\n";
            of << "R" << m_damp_R2.get_id() << " " << node_counter << " "
               << node_counter + next_node_cnt_inc << " "
               << std::to_string(m_damp_R2.get_value()) << "\n";
            node_counter = node_counter + next_node_cnt_inc;
            next_node_cnt_inc = 1;
        }

        int spk_node = node_counter;

        // insert speaker resistance, TODO: insert speaker impedance table
        if (m_adv_imp_model == 1)
        {
            // Complex model
            double const cmes = s.get_mmd() / (s.get_bl() * s.get_bl()) * 1'000'000;
            double const lces = s.get_bl() * s.get_bl() * s.get_cms() * 1'000;
            double const res = s.get_bl() * s.get_bl() / s.get_rms();

            // air density kg/m^3
            constexpr double po = 1.18;

            double const cmef = 8 * po * s.get_ad() * s.get_ad() * s.get_ad()
                                / (3 * s.get_bl() * s.get_bl()) * 1'000'000;

            of << "R" << s.get_id() << " " << node_counter << " "
               << node_counter + next_node_cnt_inc << " " << s.get_rdc() << "\n";
            node_counter = node_counter + next_node_cnt_inc;
            of << "L" << s.get_id() << " " << node_counter << " " << node_counter + 1
               << " " << s.get_lvc() << "mH"
               << "\n";
            node_counter = node_counter + 1;
            of << "lces " << node_counter << " " << 0 << " " << std::to_string(lces)
               << "mH\n";
            of << "cmes " << node_counter << " " << 0 << " " << std::to_string(cmes)
               << "uF\n";
            of << "res " << node_counter << " " << 0 << " " << std::to_string(res) << "\n";
            of << "cmef " << node_counter << " " << 0 << " " << std::to_string(cmef)
               << "uF\n";
        }
        else
        {
            // simple model, model speaker as resistor
            of << "R" << s.get_id() << " " << node_counter << " " << 0 << " "
               << std::to_string(s.get_rdc()) << "\n";
        }

        if (use_gnucap)
        {
            of << ".print ac vdb(" << spk_node << ")"
               << "\n";
            of << ".ac DEC 50 20 20k"
               << "\n";
        }
        else
        {
            of << ".ac DEC 50 20 20k"
               << "\n";
            of << ".print ac db(v(" << spk_node << "))"
               << "\n";
        }
        of << ".end"
           << "\n";
        of.close();
    }
    else
    {
        throw std::runtime_error("filter_network::to_SPICE: could not write " + tmp_file);
    }
    return tmp_file;
}

auto operator<<(std::ostream& output, filter_network const& net) -> std::ostream&
{
    output << _("***filter_network*** ******") << "\n"
           << _("Id:       ") << net.m_id << "\n"
           << _("Type:     ") << net.m_type << "\n"
           << _("Highpass #") << net.m_highpass_order << "\n"
           << _("Lowpass  #") << net.m_lowpass_order << "\n"
           << _("Has imp corr: ") << net.m_has_imp_corr << "\n"
           << _("Has damping : ") << net.m_has_damp << "\n"
           << _("Has resonanse circuit: ") << net.m_has_res << "\n";
    output << _("Parts:") << "\n";

    // Print every part in this net
    for (auto const& from : net.m_parts)
    {
        output << from;
    }
    return output << _("********* ******") << "\n";
}

void filter_network::set_highpass_order(int order)
{
    if (order >= 0 && order <= 4)
    {
        setup_net_by_order(order, NET_TYPE_HIGHPASS);
        m_highpass_order = order;
        m_type = order > 0 ? m_type | NET_TYPE_HIGHPASS : m_type & ~NET_TYPE_HIGHPASS;
    }
}

void filter_network::set_lowpass_order(int order)
{
    if (order >= 0 && order <= 4)
    {
        setup_net_by_order(order, NET_TYPE_LOWPASS);
        m_lowpass_order = order;
        m_type = order > 0 ? m_type | NET_TYPE_LOWPASS : m_type & ~NET_TYPE_LOWPASS;
    }
}

void filter_network::set_has_imp_corr(bool has_impedance_correction)
{
    m_has_imp_corr = has_impedance_correction;
    if (has_impedance_correction)
    {
        m_imp_corr_R = passive_component(PART_TYPE_RESISTOR, 5.6, "");
        m_imp_corr_C = passive_component(PART_TYPE_CAPACITOR);
    }
}

void filter_network::set_has_damp(bool has_dampening)
{
    m_has_damp = has_dampening;
    if (has_dampening)
    {
        m_damp_R1 = passive_component(PART_TYPE_RESISTOR, 1, "");
        m_damp_R2 = passive_component(PART_TYPE_RESISTOR, 1, "");
    }
}

void filter_network::setup_net_by_order(int new_order, int which_net)
{
    auto iter = m_parts.begin();

    if (which_net == NET_TYPE_LOWPASS)
    {
        // Find out how much we should increase (or decrease) filter order
        int diff = new_order - m_lowpass_order;
        if (diff > 0)
        {
            // increase filter order: add parts to the lowpass part of the net
            switch (diff)
            {
                case 1: // add one part
                    switch (m_lowpass_order)
                    {
                        /* We will add the same part to a 0th order filter as we would add to a 2nd order filter */
                        case NET_NOT_PRESENT:
                        case NET_ORDER_2ND:
                            m_parts.insert(iter + m_lowpass_order,
                                           passive_component(PART_TYPE_INDUCTOR));
                            break;
                        case NET_ORDER_1ST:
                        case NET_ORDER_3RD:
                            m_parts.insert(iter + m_lowpass_order,
                                           passive_component(PART_TYPE_CAPACITOR));
                            break;
                            /* Also we can't add 1 part to a 4th order filter so we don't use its identifier here */
                    }
                    break;
                case 2:
                    switch (m_lowpass_order)
                    {
                        case NET_NOT_PRESENT:
                        case NET_ORDER_2ND:
                            /* Since we're inserting the parts...last one comes first in
                               the std::vector, we insert them in reversed order */
                            iter = m_parts.insert(iter + m_lowpass_order,
                                                  passive_component(PART_TYPE_CAPACITOR));
                            iter = m_parts.insert(iter,
                                                  passive_component(PART_TYPE_INDUCTOR));
                            break;
                        case NET_ORDER_1ST:
                            iter = m_parts.insert(iter + m_lowpass_order,
                                                  passive_component(PART_TYPE_INDUCTOR));
                            iter = m_parts.insert(iter,
                                                  passive_component(PART_TYPE_CAPACITOR));
                            break;
                    }
                    break;
                case 3:
                    switch (m_lowpass_order)
                    {
                        case NET_NOT_PRESENT:
                            iter = m_parts.insert(iter + m_lowpass_order,
                                                  passive_component(PART_TYPE_INDUCTOR));
                            iter = m_parts.insert(iter,
                                                  passive_component(PART_TYPE_CAPACITOR));
                            iter = m_parts.insert(iter,
                                                  passive_component(PART_TYPE_INDUCTOR));
                            break;
                        case NET_ORDER_1ST:
                            iter = m_parts.insert(iter + m_lowpass_order,
                                                  passive_component(PART_TYPE_CAPACITOR));
                            iter = m_parts.insert(iter,
                                                  passive_component(PART_TYPE_INDUCTOR));
                            iter = m_parts.insert(iter,
                                                  passive_component(PART_TYPE_CAPACITOR));
                            break;
                    }
                    break;
                case 4:
                    if (m_lowpass_order == NET_NOT_PRESENT)
                    {
                        iter = m_parts.insert(iter + m_lowpass_order,
                                              passive_component(PART_TYPE_CAPACITOR));
                        iter = m_parts.insert(iter, passive_component(PART_TYPE_INDUCTOR));
                        iter = m_parts.insert(iter, passive_component(PART_TYPE_CAPACITOR));
                        iter = m_parts.insert(iter, passive_component(PART_TYPE_INDUCTOR));
                    }
                    break;
            }
        }
        else if (diff < 0)
        {
            // Remove all parts from the new filter order index up to the old last
            // lowpass part
            m_parts.erase(iter + new_order, iter + m_lowpass_order);
        }
    }
    else
    {
        auto const difference = new_order - m_highpass_order;
        if (difference > 0)
        {
            switch (difference)
            {
                case 1:
                    switch (m_highpass_order)
                    {
                        case NET_NOT_PRESENT:
                        case NET_ORDER_2ND:
                            m_parts.emplace_back(PART_TYPE_CAPACITOR);
                            break;
                        case NET_ORDER_1ST:
                        case NET_ORDER_3RD:
                            m_parts.emplace_back(PART_TYPE_INDUCTOR);
                            break;
                    }
                    break;
                case 2:
                    switch (m_highpass_order)
                    {
                        case NET_NOT_PRESENT:
                        case NET_ORDER_2ND:
                            m_parts.emplace_back(PART_TYPE_CAPACITOR);
                            m_parts.emplace_back(PART_TYPE_INDUCTOR);
                            break;
                        case NET_ORDER_1ST:
                            m_parts.emplace_back(PART_TYPE_INDUCTOR);
                            m_parts.emplace_back(PART_TYPE_CAPACITOR);
                            break;
                    }
                    break;
                case 3:
                    switch (m_highpass_order)
                    {
                        case NET_NOT_PRESENT:
                            m_parts.emplace_back(PART_TYPE_CAPACITOR);
                            m_parts.emplace_back(PART_TYPE_INDUCTOR);
                            m_parts.emplace_back(PART_TYPE_CAPACITOR);
                            break;
                        case NET_ORDER_1ST:
                            m_parts.emplace_back(PART_TYPE_INDUCTOR);
                            m_parts.emplace_back(PART_TYPE_CAPACITOR);
                            m_parts.emplace_back(PART_TYPE_INDUCTOR);
                            break;
                    }
                    break;
                case 4:
                    if (m_highpass_order == NET_NOT_PRESENT)
                    {
                        m_parts.emplace_back(PART_TYPE_CAPACITOR);
                        m_parts.emplace_back(PART_TYPE_INDUCTOR);
                        m_parts.emplace_back(PART_TYPE_CAPACITOR);
                        m_parts.emplace_back(PART_TYPE_INDUCTOR);
                    }
                    break;
            }
        }
        else if (difference < 0)
        {
            // Remove parts
            m_parts.erase(iter + m_lowpass_order + new_order,
                          iter + m_lowpass_order + m_highpass_order);
        }
    }
}
