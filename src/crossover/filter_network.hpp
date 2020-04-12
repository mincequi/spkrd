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

#pragma once

#include "common.h"
#include "gspeakersobject.h"
#include "passive_component.hpp"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <vector>

/* filter_network types */
constexpr auto NET_TYPE_LOWPASS = 1;
constexpr auto NET_TYPE_HIGHPASS = 2;
constexpr auto NET_TYPE_BANDPASS = 3;
constexpr auto NET_NOT_PRESENT = 0;

/* Filter # */
constexpr auto NET_ORDER_1ST = 1;
constexpr auto NET_ORDER_2ND = 2;
constexpr auto NET_ORDER_3RD = 3;
constexpr auto NET_ORDER_4TH = 4;

/* filter family */
constexpr auto NET_BESSEL = 1;
constexpr auto NET_BUTTERWORTH = 2;
constexpr auto NET_CHEBYCHEV = 3;
constexpr auto NET_LINKWITZRILEY = 4;
constexpr auto NET_GAUSSIAN = 5;
constexpr auto NET_LEGENDRE = 6;
constexpr auto NET_LINEARPHASE = 7;

/// filter_network is a part of a crossover: one part of the crossover we want to simulate in SPICE
/// For example: Lowpass filter with impedance correction network or
///              highpass filter with damping network.
class filter_network : public GSpeakersObject
{
public:
    /// Construct new net object
    /// type = NET_TYPE_LOWPASS                        // lowpass filter
    /// type = NET_TYPE_HIGHPASS                       // highpass filter
    /// type = NET_TYPE_LOWPASS | NET_TYPE_HIGHPASS    // bandpass filter
    filter_network(int type = NET_TYPE_LOWPASS,
                   int lowpass_order = NET_ORDER_1ST,
                   int highpass_order = NET_NOT_PRESENT,
                   bool has_imp_corr = false,
                   bool has_damp = false,
                   bool has_res = false,
                   int family = NET_BUTTERWORTH,
                   int adv_imp_model = 0,
                   bool inv_pol = false);

    /// Construct a part from an xml node or create_from_xml(xmlNodePtr)
    filter_network(xmlNodePtr parent);

    /// Convert data for a part to an xml node, throws std::runtime_error on failure
    /// Maybe this one should throw an exception
    auto to_xml_node(xmlNodePtr parent) -> xmlNodePtr;

    /// Print part data to stdout
    friend auto operator<<(std::ostream& o, filter_network const& net) -> std::ostream&;

    auto to_SPICE(driver const& s, bool use_gnucap = false) -> std::string;

    auto parts() -> std::vector<passive_component>& { return m_parts; }

    auto parts() const -> std::vector<passive_component> const& { return m_parts; }

    auto get_highpass_order() const -> int { return m_highpass_order; }

    auto get_lowpass_order() const -> int { return m_lowpass_order; }

    auto get_has_imp_corr() const -> bool { return m_has_imp_corr; }

    auto get_has_damp() const -> bool { return m_has_damp; }

    auto get_has_res() const -> bool { return m_has_res; }

    auto get_lowpass_family() const -> int { return m_lowpass_family; }

    auto get_highpass_family() const -> int { return m_highpass_family; }

    auto get_speaker() const -> std::string const& { return m_speaker; }

    auto get_adv_imp_model() const -> int { return m_adv_imp_model; }

    auto get_inv_pot() const -> bool { return m_inv_pol; }

    auto get_imp_corr_R() -> passive_component& { return m_imp_corr_R; }

    auto get_imp_corr_C() -> passive_component& { return m_imp_corr_C; }

    auto get_damp_R1() -> passive_component& { return m_damp_R1; }

    auto get_damp_R2() -> passive_component& { return m_damp_R2; }

    auto get_res_R() -> passive_component& { return m_res_R; }

    auto get_res_C() -> passive_component& { return m_res_C; }

    auto get_res_L() -> passive_component& { return m_res_L; }

    void set_highpass_order(int order);

    void set_lowpass_order(int order);

    void set_has_imp_corr(bool has_imp_corr);

    void set_has_damp(bool has_damp);

    void set_imp_corr_R(passive_component p) { m_imp_corr_R = std::move(p); }

    void set_imp_corr_C(passive_component p) { m_imp_corr_C = std::move(p); }

    void set_damp_R1(passive_component p) { m_damp_R1 = std::move(p); }

    void set_damp_R2(passive_component p) { m_damp_R2 = std::move(p); }

    void set_res_R(passive_component p) { m_res_R = std::move(p); }

    void set_res_C(passive_component p) { m_res_C = std::move(p); }

    void set_res_L(passive_component p) { m_res_L = std::move(p); }

    void set_lowpass_family(int family) { m_lowpass_family = family; }

    void set_highpass_family(int family) { m_highpass_family = family; }

    void set_speaker(std::string speaker) { m_speaker = std::move(speaker); }

    void set_adv_imp_model(int model) { m_adv_imp_model = model; }

    void set_inv_pol(bool pol) { m_inv_pol = pol; }

protected:
    int m_highpass_order;
    int m_lowpass_order;

    bool m_has_imp_corr;
    bool m_has_damp;
    bool m_has_res;

    passive_component m_imp_corr_R;
    passive_component m_imp_corr_C;
    passive_component m_damp_R1;
    passive_component m_damp_R2;
    passive_component m_res_R;
    passive_component m_res_C;
    passive_component m_res_L;

    std::vector<passive_component> m_parts;

    int m_lowpass_family;
    int m_highpass_family;

    std::string m_speaker;

    int m_adv_imp_model;
    bool m_inv_pol;

private:
    /// This function will set a new order in one subfilter
    /// It will also remove or add parts to fit the new order of the filter
    /// new order: the new order for selected filter, NET_ORDER_1ST, ...
    /// which filter: filter link to work on, NET_TYPE_LOWPASS, NET_TYPE_HIGHPASS
    void setup_net_by_order(int new_order, int which_net);

    void parse_type(xmlNodePtr node);
    void parse_highpass_order(xmlNodePtr node);
    void parse_lowpass_order(xmlNodePtr node);
    void parse_has_imp_corr(xmlNodePtr node);
    void parse_has_damp(xmlNodePtr node);
    void parse_has_res(xmlNodePtr node);
    void parse_parts(xmlNodePtr node);
    void parse_lowpass_family(xmlNodePtr node);
    void parse_highpass_family(xmlNodePtr node);
    void parse_speaker(xmlNodePtr node);
    void parse_adv_imp_model(xmlNodePtr node);
    void parse_inv_pol(xmlNodePtr node);
};
