
#pragma once

namespace spkrd
{
/* filter_network types */
constexpr auto NET_TYPE_LOWPASS = 1;
constexpr auto NET_TYPE_HIGHPASS = 2;
constexpr auto NET_TYPE_BANDPASS = 3;
constexpr auto NET_NOT_PRESENT = 0;

/* Filter # */
constexpr auto NET_ORDER_1ST = 1;
constexpr auto NET_ORDER_2ND = 2;
constexpr auto NET_ORDER_3RD = 3;
constexpr auto NET_ORDER_4TH = 4;

/* filter family */
constexpr auto NET_BESSEL = 1;
constexpr auto NET_BUTTERWORTH = 2;
constexpr auto NET_CHEBYCHEV = 3;
constexpr auto NET_LINKWITZRILEY = 4;
constexpr auto NET_GAUSSIAN = 5;
constexpr auto NET_LEGENDRE = 6;
constexpr auto NET_LINEARPHASE = 7;

}
