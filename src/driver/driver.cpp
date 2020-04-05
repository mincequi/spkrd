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

driver::driver(xmlNodePtr parent)
{
    if (parent == nullptr || std::string((char*)parent->name) != "speaker")
    {
        throw std::runtime_error(_("driver: speaker node not found"));
    }

    auto node = parent->children;

    m_id_string = parse_string(node, "id_string");
    node = node->next;

    m_type = parse_int(node, "type");
    node = node->next;

    m_qts = parse_double(node, "qts");
    node = node->next;

    m_vas = parse_double(node, "vas");
    node = node->next;

    m_fs = parse_double(node, "fs");
    node = node->next;

    m_rdc = parse_double(node, "rdc");
    node = node->next;

    m_lvc = parse_double(node, "lvc");
    node = node->next;

    m_qms = parse_double(node, "qms");
    node = node->next;

    m_qes = parse_double(node, "qes");
    node = node->next;

    m_imp = parse_double(node, "imp");
    node = node->next;

    m_sens = parse_double(node, "sens");
    node = node->next;

    m_freq_resp_filename = parse_string(node, "freq_resp_filename");
    node = node->next;

    m_imp_resp_filename = parse_string(node, "imp_resp_filename");
    node = node->next;

    m_mmd = parse_double(node, "mmd");
    node = node->next;

    m_ad = parse_double(node, "ad");
    node = node->next;

    m_bl = parse_double(node, "bl");
    node = node->next;

    m_rms = parse_double(node, "rms");
    node = node->next;

    m_cms = parse_double(node, "cms");
    node = node->next;
}

auto driver::to_xml_node(xmlNodePtr parent) -> xmlNodePtr
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

auto operator<<(std::ostream& output, driver const& speaker) -> std::ostream&
{
    output << speaker.m_id_string << "\n"
           << _("Id:   ") << speaker.m_id << "\n"
           << _("Type: ") << speaker.m_type << "\n"
           << _("Qts:  ") << speaker.m_qts << "\n"
           << _("Vas:  ") << speaker.m_vas << "\n"
           << _("Fs:   ") << speaker.m_fs << "\n";
    return output;
}
