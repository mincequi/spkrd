
#pragma once

namespace gspk
{
namespace sealed
{
auto volume(double const Qtc, double const equivalent_volume, double const Qts) noexcept -> double;

auto resonance_frequency(double const Qtc,
                         double const driver_resonance_frequency,
                         double const Qts) noexcept -> double;

auto minus_3dB_rolloff(double const Qtc, double const resonant_frequency) noexcept -> double;
}
}
