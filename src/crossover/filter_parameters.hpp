
#pragma once

#include <vector>

namespace spkrd
{
namespace filter
{
/// Numerical coefficients for the filter principles
/// name = NET_BESSEL, ...,
/// order = NET_ORDER_1ST, ...,
/// type = NET_TYPE_LOWPASS, NET_TYPE_HIGHPASS
auto coefficients(int name, int order, int type) noexcept(false) -> std::vector<double>;
}
}
