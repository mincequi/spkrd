
#pragma once

#include <cmath>

namespace spkrd
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

inline auto dB_to_magnitude(double const dB) -> double { return std::pow(10, dB / 10.0); }

inline auto magnitude_to_dB(double const magnitude) -> double
{
    return 10.0 * std::log10(magnitude);
}

}
