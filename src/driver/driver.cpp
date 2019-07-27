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

#include "driver.hpp"
#include "common.h"

#include "xml_parser.hpp"

#include <libxml/parser.h>

#include "nlohmann/json.hpp"

#include <utility>

using namespace spkrd;

driver::driver(std::string id_string,
               int type,
               double qts,
               double vas,
               double fs,
               double rdc,
               double lvc,
               double qms,
               double qes,
               double imp,
               double sens,
               std::string freq_resp_filename,
               std::string imp_resp_filename,
               double mmd,
               double ad,
               double bl,
               double rms,
               double cms)
    : GSpeakersObject(),
      m_qts(qts),
      m_qms(qms),
      m_qes(qes),
      m_vas(vas),
      m_fs(fs),
      m_rdc(rdc),
      m_lvc(lvc),
      m_imp(imp),
      m_sens(sens),
      m_mmd(mmd),
      m_ad(ad),
      m_bl(bl),
      m_rms(rms),
      m_cms(cms),
      m_freq_resp_filename(std::move(freq_resp_filename)),
      m_imp_resp_filename(std::move(imp_resp_filename)),
      m_id_string(std::move(id_string))
{
    m_type = type;
}

driver::driver(nlohmann::json const& driver_data)
{
    auto error_message = [](std::string const& field) {
        return "A " + field + " was not provided for the driver";
    };

    std::vector<std::string> fields = {"name",
                                       "type",
                                       "qts",
                                       "vas",
                                       "fs",
                                       "rdc",
                                       "lvc",
                                       "qms",
                                       "qes",
                                       "imp",
                                       "sens",
                                       "mmd",
                                       "ad",
                                       "bl",
                                       "rms",
                                       "cms"};

    for (auto const& field : fields)
    {
        if (driver_data.find(field) == end(driver_data))
        {
            throw std::runtime_error(error_message(field));
        }
    }

    m_id_string = driver_data["name"].get<std::string>();

    if (driver_data.find("frequency_response_file") != end(driver_data))
    {
        m_freq_resp_filename = driver_data["frequency_response_file"].get<std::string>();
    }

    if (driver_data.find("impedence_response_file") != end(driver_data))
    {
        m_imp_resp_filename = driver_data["impedence_response_file"].get<std::string>();
    }

    m_type = driver_data["type"].get<std::int32_t>();

    m_qts = driver_data["qts"].get<double>();
    m_vas = driver_data["vas"].get<double>();
    m_fs = driver_data["fs"].get<double>();
    m_rdc = driver_data["rdc"].get<double>();
    m_lvc = driver_data["lvc"].get<double>();
    m_qms = driver_data["qms"].get<double>();
    m_qes = driver_data["qes"].get<double>();
    m_imp = driver_data["imp"].get<double>();
    m_sens = driver_data["sens"].get<double>();
    m_mmd = driver_data["mmd"].get<double>();
    m_ad = driver_data["ad"].get<double>();
    m_bl = driver_data["bl"].get<double>();
    m_rms = driver_data["rms"].get<double>();
    m_cms = driver_data["cms"].get<double>();
}

xmlNodePtr driver::to_xml_node(xmlNodePtr parent)
{
    std::array<gchar, 10> buffer;

    xmlNodePtr speaker = xmlNewChild(parent, nullptr, (xmlChar*)("speaker"), nullptr);

    xmlNodePtr child = xmlNewChild(speaker, nullptr, (xmlChar*)("id_string"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)m_id_string.c_str());

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("type"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_strdup_printf("%d", m_type));

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("qts"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_qts));

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("vas"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_vas));

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("fs"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_fs));

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("rdc"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_rdc));

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("lvc"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_lvc));

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("qms"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_qms));

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("qes"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_qes));

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("imp"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_imp));

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("sens"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_sens));

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("freq_resp_filename"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)m_freq_resp_filename.c_str());

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("imp_resp_filename"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)m_imp_resp_filename.c_str());

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("mmd"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_mmd));

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("ad"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_ad));

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("bl"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_bl));

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("rms"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_rms));

    child = xmlNewChild(speaker, nullptr, (xmlChar*)("cms"), nullptr);
    xmlNodeSetContent(child, (xmlChar*)g_ascii_dtostr(buffer.data(), 8, m_cms));

    return speaker;
}

std::ostream& operator<<(std::ostream& output, const driver& speaker)
{
    output << speaker.m_id_string << "\n"
           << _("Id:   ") << speaker.m_id << "\n"
           << _("Type: ") << speaker.m_type << "\n"
           << _("Qts:  ") << speaker.m_qts << "\n"
           << _("Vas:  ") << speaker.m_vas << "\n"
           << _("Fs:   ") << speaker.m_fs << "\n";
    return output;
}
