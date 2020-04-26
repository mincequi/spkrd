
#include "xml_parser.hpp"

#include "common.h"

#include <stdexcept>
#include <sstream>

namespace gspk
{
auto parse_double(xmlNodePtr node, std::string const& label) -> double
{
    double value = 0.0;

    if (node != nullptr && std::string((char*)node->name) == label)
    {
        std::istringstream((char*)xmlNodeGetContent(node)) >> value;
    }
    else
    {
        throw std::runtime_error(label + _(" node not found"));
    }
    return value;
}

auto parse_int(xmlNodePtr node, std::string const& label) -> int
{
    int value = 0;

    if (node != nullptr && std::string((char*)node->name) == label)
    {
        std::istringstream((char*)xmlNodeGetContent(node)) >> value;
    }
    else
    {
        throw std::runtime_error(label + _(" node not found"));
    }
    return value;
}

auto parse_string(xmlNodePtr const node, std::string const& label) -> std::string
{
    std::string value;

    if (node != nullptr && std::string((char*)node->name) == label)
    {
        value = std::string((char*)xmlNodeGetContent(node));
    }
    else
    {
        throw std::runtime_error(label + _(" node not found"));
    }
    return value;
}
}
