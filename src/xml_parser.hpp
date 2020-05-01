
#pragma once

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <string>

namespace gspk
{
auto parse_double(xmlNodePtr const node, std::string const& label) noexcept(false)
    -> double;

auto parse_int(xmlNodePtr const node, std::string const& label) noexcept(false) -> int;

auto parse_string(xmlNodePtr const node, std::string const& label) noexcept(false)
    -> std::string;

void check_name(xmlNodePtr const node, std::string const& label) noexcept(false);
}
