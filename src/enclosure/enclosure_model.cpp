

#include "enclosure_model.hpp"

#include "unit_conversion.hpp"

#include <cassert>
#include <cmath>

namespace gspk
{
namespace sealed
{
auto volume(double const Qtc, double const equivalent_volume, double const Qts) noexcept
    -> double
{
    assert(equivalent_volume > 0.0);
    assert(Qtc > 0.0);
    assert(Qts > 0.0);

    return equivalent_volume / (std::pow(Qtc / Qts, 2) - 1.0);
}

auto resonance_frequency(double const Qtc,
                         double const driver_resonance_frequency,
                         double const Qts) noexcept -> double
{
    assert(Qtc > 0.0);
    assert(Qts > 0.0);
    assert(driver_resonance_frequency > 0.0);

    return Qtc * driver_resonance_frequency / Qts;
}

auto minus_3dB_rolloff(double const Qtc, double const resonance_frequency) noexcept -> double
{
    assert(Qtc > 0);
    assert(resonance_frequency > 0);

    return resonance_frequency
           * std::sqrt((1.0 / std::pow(Qtc, 2) - 2.0
                        + std::sqrt(std::pow(1.0 / std::pow(Qtc, 2) - 2.0, 2) + 4.0))
                       / 2.0);
}

auto frequency_response(double const Qtc,
                        double const frequency,
                        double const enclosure_resonance_frequency) noexcept -> double
{
    assert(Qtc >= 0.0);
    assert(enclosure_resonance_frequency >= 0.0);
    assert(frequency >= 0.0);

    auto const& f = frequency;
    auto const& fc = enclosure_resonance_frequency;

    return 10.0
           * std::log10(std::pow(f / fc, 2)
                        / std::sqrt(std::pow(f / fc, 4)
                                    + std::pow(f / fc, 2) * (std::pow(Qtc, -2) - 2.0)
                                    + 1.0));
}
}

namespace ported
{
auto port_length(double const port_radius,
                 double const enclosure_volume,
                 double const tuning_frequency) noexcept -> double
{
    // The formula has the constants written in freedom units. We can do the
    // conversion with a few calls to some functions (should all be inlined and
    // optimised away) and then convert back to units that make sense.
    return inch_to_cm(
        1.463E7 * std::pow(cm_to_inch(port_radius), 2)
            / (std::pow(tuning_frequency, 2) * litre_to_cubic_inches(enclosure_volume))
        - 1.463 * cm_to_inch(port_radius));
}

auto minimum_port_diameter(double const cone_displacement_volume,
                           double const tuning_frequency) noexcept -> double
{
    return 100.0
           * std::sqrt(411.25 * cone_displacement_volume / std::sqrt(tuning_frequency));
}

auto minimum_port_diameter_conservative(double const cone_displacement_volume,
                                        double const tuning_frequency) noexcept -> double
{
    return 100.0 * std::sqrt(tuning_frequency * cone_displacement_volume);
}

}

}
