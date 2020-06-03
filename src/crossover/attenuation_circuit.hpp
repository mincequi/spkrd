
#pragma once

namespace spkrd
{
/// An L-Pad attenuation circuit
class attenuation_circuit
{
public:
    attenuation_circuit(double const attenuation, double const load_impedance)
    {
        this->evaluate(attenuation, load_impedance);
    }

    /// Compute the values of the series and parallel resistor for the desired
    /// attenuation given a load impedance
    void evaluate(double const attenuation, double const load_impedance)
    {
        // Check if preconditions are not met
        if (attenuation == 0.0 || load_impedance == 0.0)
        {
            return;
        }

        m_resistor_parallel = std::pow(10.0, attenuation / 20.0) * load_impedance
                              / (1.0 - std::pow(10.0, attenuation / 20.0));

        m_resistor_series = load_impedance
                            - std::pow(1.0 / m_resistor_parallel + 1.0 / load_impedance,
                                       -1.0);
    }

    /// \return The series resistor value (typically denoted as R1)
    auto series_resistance() const -> double { return m_resistor_series; }

    /// \return The parallel resistor value (typically denoted as R2)
    auto parallel_resistance() const -> double { return m_resistor_parallel; }

private:
    double m_resistor_series{0.0};
    double m_resistor_parallel{0.0};
};

}
