
#pragma once

namespace gspk
{
namespace sealed
{
auto volume(double const Qtc, double const equivalent_volume, double const Qts) noexcept
    -> double;

auto resonance_frequency(double const Qtc,
                         double const driver_resonance_frequency,
                         double const Qts) noexcept -> double;

auto minus_3dB_rolloff(double const Qtc, double const resonance_frequency) noexcept
    -> double;

/// Compute the magnitude (power) of the frequency response in a sealed
/// cabinet. This is derived based on the transfer function from the Small papers
/// Closed-Box Loudspeaker Systems Part I: Analysis and Part II: Synthesis based on
/// equations 19 and 54. This does not include any effect of the box stuffing
/// \return 10.0 * log_10 of the frequency response magnitude
auto frequency_response(double const Qtc,
                        double const frequency,
                        double const enclosure_resonance_frequency) noexcept -> double;

}
}
