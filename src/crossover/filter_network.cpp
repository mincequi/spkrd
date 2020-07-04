/*
  $Id$

  net Copyright (C) 2002 Daniel Sundberg

  This program is free software; you can redistribute it and/or modify
  it under the terms output the GNU General Public License version 2
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty output
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy output the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "filter_network.hpp"
#include "driver.hpp"
#include "xml_parser.hpp"

#include <glibmm.h>

#include <fstream>
#include <sstream>
#include <utility>

namespace spkrd
{
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
      m_has_attenuation(has_damp),
      m_has_res(has_res),
      m_lowpass_family(family),
      m_highpass_family(family),
      m_advanced_impedance_model(adv_imp_model),
      m_invert_polarity(inv_pol)
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
        m_damp_R1 = passive_component(PART_TYPE_RESISTOR, 1.0, "");
        m_damp_R2 = passive_component(PART_TYPE_RESISTOR, 1.0, "");
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
    check_name(parent, "net");

    auto* child = parent->children;

    for (auto const& label : {"type",
                              "lowpass_order",
                              "highpass_order",
                              "has_imp_corr",
                              "has_damp",
                              "has_res",
                              "parts",
                              "lowpass_family",
                              "highpass_family",
                              "speaker",
                              "adv_imp_model",
                              "inv_pol"})
    {
        check_name(child, label);
        child = child->next;
    }

    child = parent->children;
    m_type = parse_int(child, "type");

    child = child->next;
    m_lowpass_order = parse_int(child, "lowpass_order");

    child = child->next;
    m_highpass_order = parse_int(child, "highpass_order");

    child = child->next;
    m_has_imp_corr = (parse_int(child, "has_imp_corr") != 0);

    child = child->next;
    m_has_attenuation = (parse_int(child, "has_damp") != 0);

    child = child->next;
    m_has_res = (parse_int(child, "has_res") != 0);

    child = child->next;
    // Parse the parts from the xml file
    {
        auto* subchild = child->children;

        if (m_has_imp_corr)
        {
            m_imp_corr_R = passive_component(subchild);
            subchild = subchild->next;

            m_imp_corr_C = passive_component(subchild);
            subchild = subchild->next;
        }
        if (m_has_attenuation)
        {
            m_damp_R1 = passive_component(subchild);
            subchild = subchild->next;

            m_damp_R2 = passive_component(subchild);
            subchild = subchild->next;
        }
        if (m_has_res)
        {
            m_res_R = passive_component(subchild);
            subchild = subchild->next;

            m_res_C = passive_component(subchild);
            subchild = subchild->next;

            m_res_L = passive_component(subchild);
            subchild = subchild->next;
        }
        while (subchild != nullptr)
        {
            m_parts.emplace_back(subchild);
            subchild = subchild->next;
        }
    }

    child = child->next;
    m_lowpass_family = parse_int(child, "lowpass_family");

    child = child->next;
    m_highpass_family = parse_int(child, "highpass_family");

    child = child->next;
    m_speaker = parse_string(child, "speaker");

    child = child->next;
    m_advanced_impedance_model = parse_int(child, "adv_imp_model");

    child = child->next;
    m_invert_polarity = (parse_int(child, "inv_pol") != 0);
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
    xmlNodeSetContent(field,
                      (xmlChar*)g_strdup_printf("%d", static_cast<int>(m_has_attenuation)));
    field = xmlNewChild(net, nullptr, (xmlChar*)("has_res"), nullptr);
    xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", static_cast<int>(m_has_res)));
    field = xmlNewChild(net, nullptr, (xmlChar*)("parts"), nullptr);

    // Insert impedance correction and damping network first in parts section
    if (m_has_imp_corr)
    {
        m_imp_corr_R.to_xml_node(field);
        m_imp_corr_C.to_xml_node(field);
    }
    if (m_has_attenuation)
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
    xmlNodeSetContent(field, (xmlChar*)g_strdup_printf("%d", m_advanced_impedance_model));
    field = xmlNewChild(net, nullptr, (xmlChar*)("inv_pol"), nullptr);
    xmlNodeSetContent(field,
                      (xmlChar*)g_strdup_printf("%d", static_cast<int>(m_invert_polarity)));

    return net;
}

auto filter_network::to_SPICE(driver const& current_driver, bool use_gnucap) -> std::string
{
    std::string tmp_dir = Glib::get_tmp_dir();

    std::string tmp_file = tmp_dir +
#ifdef TARGET_WIN32
                           "\\net"
#else
                           "/net"
#endif
                           + int_to_ustring(m_id) + ".tmp";

    std::ofstream output(tmp_file.c_str());

    if (!output.good())
    {
        throw std::runtime_error("filter_network::to_SPICE: could not write " + tmp_file);
    }

    std::int32_t node_counter = 0;
    std::int32_t part_index = 0;
    std::int32_t next_node_cnt_inc = 1;

    output << "Crossover network SPICE code generated by gspk " << std::string(VERSION)
           << "\n";
    output << "vamp " << ++node_counter << " " << 0 << " dc 0 ac 1\n";

    if (m_lowpass_order > 0)
    {
        switch (m_lowpass_order)
        {
            case NET_ORDER_1ST:
            {
                ++node_counter;

                output << "L" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;

                break;
            }
            case NET_ORDER_2ND:
            {
                ++node_counter;

                output << "L" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;

                output << "C" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << 0 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;

                break;
            }
            case NET_ORDER_3RD:
            {
                ++node_counter;

                output << "L" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;

                output << "C" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << 0 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;
                ++node_counter;

                output << "L" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;

                break;
            }
            case NET_ORDER_4TH:
            {
                ++node_counter;

                output << "L" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;

                output << "C" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << 0 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;
                ++node_counter;

                output << "L" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;

                output << "C" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << 0 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;

                break;
            }
        }
    }
    if (m_highpass_order > 0)
    {
        switch (m_highpass_order)
        {
            case NET_ORDER_1ST:
            {
                ++node_counter;

                output << "C" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;

                break;
            }
            case NET_ORDER_2ND:
            {
                ++node_counter;

                output << "C" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;

                output << "L" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << 0 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;

                break;
            }
            case NET_ORDER_3RD:
            {
                ++node_counter;

                output << "C" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;

                output << "L" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << 0 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;
                ++node_counter;

                output << "C" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;

                break;
            }
            case NET_ORDER_4TH:
            {
                ++node_counter;

                output << "C" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;

                output << "L" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << 0 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;
                ++node_counter;

                output << "C" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << node_counter - 1 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;

                output << "L" << m_parts.at(part_index).get_id() << " " << node_counter
                       << " " << 0 << " "
                       << std::to_string(m_parts.at(part_index).get_value())
                       << m_parts.at(part_index).get_unit() << "\n";

                ++part_index;
                break;
            }
        }
    }

    next_node_cnt_inc = 1 + m_has_imp_corr;

    if (m_has_imp_corr)
    {
        output << "R" << m_imp_corr_R.get_id() << " " << node_counter << " "
               << node_counter + 1 << " " << std::to_string(m_imp_corr_R.get_value())
               << "\n";
        output << "C" << m_imp_corr_C.get_id() << " " << node_counter + 1 << " " << 0
               << " " << std::to_string(m_imp_corr_C.get_value())
               << m_imp_corr_C.get_unit() << "\n";
    }

    if (m_has_attenuation)
    {
        output << "R" << m_damp_R1.get_id() << " " << node_counter << " " << 0 << " "
               << std::to_string(m_damp_R1.get_value()) << "\n";
        output << "R" << m_damp_R2.get_id() << " " << node_counter << " "
               << node_counter + next_node_cnt_inc << " "
               << std::to_string(m_damp_R2.get_value()) << "\n";

        node_counter += next_node_cnt_inc;
        next_node_cnt_inc = 1;
    }

    auto const driver_node = node_counter;

    // Insert speaker resistance, TODO: insert speaker impedance table
    if (m_advanced_impedance_model == 1)
    {
        auto const positive_node = m_invert_polarity ? 0 : driver_node;
        auto const negative_node = m_invert_polarity ? driver_node : 0;

        // + o--- R ----o----L----o-----o------o-----o-----o
        //        (Voice coil)          |      |     |     |
        //                              |      |     |     |
        //                              L      C     R     C
        //                              |      |     |     |
        //                              |      |     |     |
        // - o--------------------------o------o-----o-----o

        // Voice coil resistance
        output << "R" << current_driver.get_id() << " " << positive_node << " "
               << node_counter + next_node_cnt_inc << " " << current_driver.get_rdc()
               << '\n';

        node_counter += next_node_cnt_inc;

        // Voice coil inductance
        output << "L" << current_driver.get_id() << " " << node_counter << " "
               << node_counter + 1 << " " << current_driver.get_lvc() << "mH" << '\n';

        ++node_counter;

        // Air density at 25 Celsius [kg/m^3]
        constexpr auto air_density = 1.184;

        auto const force_factor = current_driver.get_bl();

        auto const air_capacitance = 8 * air_density * std::pow(current_driver.get_ad(), 3)
                                     / (3 * std::pow(force_factor, 2)) * 1'000'000;

        // Moving mass capacitance
        auto const moving_mass_capacitance = current_driver.get_mmd()
                                             / std::pow(force_factor, 2) * 1'000'000;

        // Cone suspension
        auto const suspension_inductance = std::pow(force_factor, 2)
                                           * current_driver.get_cms() * 1'000;
        auto const suspension_resistance = std::pow(force_factor, 2)
                                           / current_driver.get_rms();

        output << "lces " << node_counter << " " << negative_node << " "
               << std::to_string(suspension_inductance) << "mH\n";
        output << "cmems " << node_counter << " " << negative_node << " "
               << std::to_string(moving_mass_capacitance) << "uF\n";
        output << "res " << node_counter << " " << negative_node << " "
               << std::to_string(suspension_resistance) << '\n';
        output << "cmef " << node_counter << " " << negative_node << " "
               << std::to_string(air_capacitance) << "uF\n";
    }
    else
    {
        // simple model, model speaker as resistor
        output << "R" << current_driver.get_id() << " "
               << (m_invert_polarity ? 0 : node_counter) << " "
               << (m_invert_polarity ? node_counter : 0) << " "
               << std::to_string(current_driver.get_rdc()) << '\n';
    }

    if (use_gnucap)
    {
        // Output the voltage magnitude in dB and the phase
        output << ".print ac vdb(" << driver_node << ") vp(" << driver_node << ")\n";
        output << ".ac DEC 100 20 20k\n";
    }
    else
    {
        output << ".ac DEC 100 20 20k\n";
        output << ".print ac db(v(" << driver_node << "))\n";
    }
    output << ".end\n";

    output.close();

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
           << _("Has damping : ") << net.m_has_attenuation << "\n"
           << _("Has resonance circuit: ") << net.m_has_res << "\n";
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
    if (order < 0 || order > 4)
    {
        throw std::invalid_argument("High pass order must be 0 to 4");
    }

    this->setup_net_by_order(order, NET_TYPE_HIGHPASS);
    m_highpass_order = order;
    m_type = m_highpass_order > 0 ? m_type | NET_TYPE_HIGHPASS
                                  : m_type & ~NET_TYPE_HIGHPASS;
}

void filter_network::set_lowpass_order(int order)
{
    if (order < 0 || order > 4)
    {
        throw std::invalid_argument("Low pass order must be 0 to 4");
    }

    this->setup_net_by_order(order, NET_TYPE_LOWPASS);
    m_lowpass_order = order;
    m_type = m_lowpass_order > 0 ? m_type | NET_TYPE_LOWPASS : m_type & ~NET_TYPE_LOWPASS;
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

void filter_network::set_has_damp(bool has_attenuation)
{
    m_has_attenuation = has_attenuation;

    if (m_has_attenuation)
    {
        m_damp_R1 = passive_component(PART_TYPE_RESISTOR, 1.0, "");
        m_damp_R2 = passive_component(PART_TYPE_RESISTOR, 1.0, "");
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
            // increase filter order: add parts to the lowpass part output the net
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
}
