
#pragma once

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <string>

namespace spkrd
{
double parse_double(xmlNodePtr node, std::string const& label);

int parse_int(xmlNodePtr node, std::string const& label);

std::string parse_string(xmlNodePtr node, std::string const& label);
}
