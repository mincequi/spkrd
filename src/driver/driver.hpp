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

#pragma once

#include "gspeakersobject.h"

#include <libxml/tree.h>

#include <iosfwd>
#include <string>

/// driver types
constexpr auto SPEAKER_TYPE_BASS = 1;
constexpr auto SPEAKER_TYPE_MIDRANGE = 2;
constexpr auto SPEAKER_TYPE_TWEETER = 4;

/// \return True is the driver has a bass frequency response
constexpr auto is_bass_driver(int const value) -> bool
{
    return (value & SPEAKER_TYPE_BASS) != 0;
}
/// \return True is the driver has a midrange frequency response
constexpr auto is_midrange_driver(int const value) -> bool
{
    return (value & SPEAKER_TYPE_MIDRANGE) != 0;
}
/// \return True is the driver has a high frequency response
constexpr auto is_tweeter_driver(int const value) -> bool
{
    return (value & SPEAKER_TYPE_TWEETER) != 0;
}

/// This class contains data for a speaker
class driver : public gspkObject
{
public:
    /// Default values from Vifa P21WO-20-08
    driver(std::string id_string = "Vifa P21-20-08",
           int type = SPEAKER_TYPE_BASS | SPEAKER_TYPE_MIDRANGE,
           double qts = 0.33,
           double vas = 113,
           double fs = 28,
           double rdc = 5.6,
           double lvc = 0.9,
           double qms = 1.6,
           double qes = 0.41,
           double imp = 8,
           double sens = 91,
           std::string freq_resp_filename = "",
           std::string imp_resp_filename = "",
           double mmd = 0.04,
           double ad = 0.0837,
           double bl = 11,
           double rms = 1.7,
           double cms = 0.0012);

    /// Construct a speaker from an xml node, throws an exception on error in xml
    driver(xmlNodePtr parent) noexcept(false);

    /// Convert data for a part to an xml node, throws std::runtime_error on failure
    auto to_xml_node(xmlNodePtr parent) noexcept(false) -> xmlNodePtr;

    /// Print part data to stdout
    friend auto operator<<(std::ostream& output, driver const& speaker) -> std::ostream&;

    void set_qts(double const qts) { m_qts = qts; }

    void set_vas(double const vas) { m_vas = vas; }

    void set_fs(double const fs) { m_fs = fs; }

    void set_rdc(double const rdc) { m_rdc = rdc; }

    void set_lvc(double const lvc) { m_lvc = lvc; }

    void set_qms(double const qms) { m_qms = qms; }

    void set_qes(double const qes) { m_qes = qes; }

    void set_imp(double const imp) { m_imp = imp; }

    void set_sens(double const sens) { m_sens = sens; }

    void set_freq_resp_filename(std::string filename)
    {
        m_freq_resp_filename = std::move(filename);
    }

    void set_imp_resp_filename(std::string filename) { m_imp_resp_filename = std::move(filename); }

    void set_id_string(std::string id_string) { m_id_string = std::move(id_string); }

    void set_mmd(double const mmd) { m_mmd = mmd; }

    void set_ad(double const ad) { m_ad = ad; }

    void set_bl(double const bl) { m_bl = bl; }

    void set_rms(double const rms) { m_rms = rms; }

    void set_cms(double const cms) { m_cms = cms; }

    auto get_qts() const -> double { return m_qts; }

    auto get_vas() const -> double { return m_vas; }

    auto get_fs() const -> double { return m_fs; }

    auto get_rdc() const -> double { return m_rdc; }

    auto get_lvc() const -> double { return m_lvc; }

    auto get_qms() const -> double { return m_qms; }

    auto get_qes() const -> double { return m_qes; }

    auto get_imp() const -> double { return m_imp; }

    auto get_sens() const -> double { return m_sens; }

    auto get_freq_resp_filename() const -> std::string { return m_freq_resp_filename; }

    auto get_imp_resp_filename() const -> std::string { return m_imp_resp_filename; }

    auto get_id_string() const -> std::string const& { return m_id_string; }

    auto get_mmd() const -> double { return m_mmd; }

    auto get_ad() const -> double { return m_ad; }

    auto get_bl() const -> double { return m_bl; }

    auto get_rms() const -> double { return m_rms; }

    auto get_cms() const -> double { return m_cms; }

protected:
    /// total Q
    double m_qts;
    /// mechanical Q
    double m_qms;
    /// electrical Q
    double m_qes;
    /// equivalent volume
    double m_vas;
    /// free air resonant frequency
    double m_fs;
    /// voice coil resistance
    double m_rdc;
    /// voice coil inductance (mH)
    double m_lvc;
    /// nominal impedance
    double m_imp;
    /// sensitivity
    double m_sens;
    /// cone mass, kg
    double m_mmd;
    /// effective radius, m
    double m_ad;
    /// force factor
    double m_bl;
    /// suspension mechanical resistance, Ns/m
    double m_rms;
    /// suspension compliance, m/N
    double m_cms;

    std::string m_freq_resp_filename;
    std::string m_imp_resp_filename;

    std::string m_id_string;
};
