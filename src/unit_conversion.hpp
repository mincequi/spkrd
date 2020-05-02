
#pragma once

namespace gspk
{
inline auto constexpr inch_to_cm(double const inches) -> double { return inches * 2.54; }

inline auto constexpr cm_to_inch(double const centimetres) -> double
{
    return centimetres / 2.54;
}

inline auto constexpr litre_to_cubic_inches(double const litres) -> double
{
    return litres * 61.0237;
}
}
