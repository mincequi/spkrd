
#pragma once

namespace spkrd
{
namespace sealed
{
/// Compute the volume of a sealed enclsoure.
auto volume(double const Qtc, double const equivalent_volume, double const Qts) noexcept
    -> double;

/// Compute the resonance frequency of the sealed enclosure for a given
/// alignment, driver resonance frequency and damping
/// \return Resonance frequency (Hz)
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

namespace ported
{
/// Compute the length of the port
/// \param port_radius Port radius in cm
/// \param enclosure_volume Enclosure volume in litres
/// \param tuning_frequency Enclosure tuning frequency in Hz
/// \return Vent length in cm
auto port_length(double const port_radius,
                 double const enclosure_volume,
                 double const tuning_frequency) noexcept -> double;

/// Compute the length of the port
/// \param cone_displacement_volume Cone displacement volume in litres
/// \param tuning_frequency Enclosure tuning frequency in Hz
/// \return Vent diameter in cm
auto minimum_port_diameter(double const cone_displacement_volume,
                           double const tuning_frequency) noexcept -> double;

/// Compute the length of the port
/// \param cone_displacement_volume Cone displacement volume in cubic metres
/// \param tuning_frequency Enclosure tuning frequency in Hz
/// \return Vent diameter in cm
auto minimum_port_diameter_conservative(double const cone_displacement_volume,
                                        double const tuning_frequency) noexcept -> double;

}

}
