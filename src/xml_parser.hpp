
#pragma once

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <string>

namespace gspk
{
auto parse_double(xmlNodePtr node, std::string const& label) -> double;

auto parse_int(xmlNodePtr node, std::string const& label) -> int;

auto parse_string(xmlNodePtr node, std::string const& label) -> std::string;
}
