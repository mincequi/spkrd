
#include "filter_parameters.hpp"

#include "filter_type.hpp"

#include <stdexcept>

namespace spkrd
{
namespace filter
{
auto coefficients(int name, int order, int type) -> std::vector<double>
{
    std::vector<double> parameters;
    switch (order)
    {
        case NET_ORDER_1ST:
        {
            switch (type)
            {
                case NET_TYPE_LOWPASS:
                    parameters.emplace_back(6.28);
                    break;
                case NET_TYPE_HIGHPASS:
                    parameters.emplace_back(0.159);
                    break;
            }
            break;
        }
        case NET_ORDER_2ND:
        {
            switch (name)
            {
                case NET_BESSEL:
                {
                    switch (type)
                    {
                        case NET_TYPE_LOWPASS:
                            parameters.emplace_back(0.2756);
                            parameters.emplace_back(0.0912);
                            break;
                        case NET_TYPE_HIGHPASS:
                            parameters.emplace_back(0.0912);
                            parameters.emplace_back(0.2756);
                            break;
                    }
                    break;
                }
                case NET_BUTTERWORTH:
                {
                    switch (type)
                    {
                        case NET_TYPE_LOWPASS:
                            parameters.emplace_back(0.2251);
                            parameters.emplace_back(0.1125);
                            break;
                        case NET_TYPE_HIGHPASS:
                            parameters.emplace_back(0.1125);
                            parameters.emplace_back(0.2251);
                            break;
                    }
                    break;
                }
                case NET_CHEBYCHEV:
                {
                    switch (type)
                    {
                        case NET_TYPE_LOWPASS:
                            parameters.emplace_back(0.1592);
                            parameters.emplace_back(0.1592);
                            break;
                        case NET_TYPE_HIGHPASS:
                            parameters.emplace_back(0.1592);
                            parameters.emplace_back(0.1592);
                            break;
                    }
                    break;
                }
                case NET_LINKWITZRILEY:
                {
                    switch (type)
                    {
                        case NET_TYPE_LOWPASS:
                            parameters.emplace_back(0.3183);
                            parameters.emplace_back(0.0796);
                            break;
                        case NET_TYPE_HIGHPASS:
                            parameters.emplace_back(0.3183);
                            parameters.emplace_back(0.0796);
                            break;
                    }
                    break;
                }
            }
            break;
        }
        case NET_ORDER_3RD:
        {
            switch (name)
            {
                case NET_BESSEL:
                {
                    switch (type)
                    {
                        case NET_TYPE_LOWPASS:
                            parameters.emplace_back(0.3294);
                            parameters.emplace_back(0.1897);
                            parameters.emplace_back(0.06592);
                            break;
                        case NET_TYPE_HIGHPASS:
                            parameters.emplace_back(0.07911);
                            parameters.emplace_back(0.1317);
                            parameters.emplace_back(0.3953);
                            break;
                    }
                    break;
                }
                case NET_BUTTERWORTH:
                {
                    switch (type)
                    {
                        case NET_TYPE_LOWPASS:
                            parameters.emplace_back(0.2387);
                            parameters.emplace_back(0.2122);
                            parameters.emplace_back(0.0796);
                            break;
                        case NET_TYPE_HIGHPASS:
                            parameters.emplace_back(0.1061);
                            parameters.emplace_back(0.1194);
                            parameters.emplace_back(0.3183);
                            break;
                    }
                    break;
                }
            }
            break;
        }
        case NET_ORDER_4TH:
        {
            switch (name)
            {
                case NET_BESSEL:
                {
                    switch (type)
                    {
                        case NET_TYPE_LOWPASS:
                            parameters.emplace_back(0.3583);
                            parameters.emplace_back(0.2336);
                            parameters.emplace_back(0.1463);
                            parameters.emplace_back(0.0504);
                            break;
                        case NET_TYPE_HIGHPASS:
                            parameters.emplace_back(0.0702);
                            parameters.emplace_back(0.0862);
                            parameters.emplace_back(0.0719);
                            parameters.emplace_back(0.4983);
                            break;
                    }
                    break;
                }
                case NET_BUTTERWORTH:
                {
                    switch (type)
                    {
                        case NET_TYPE_LOWPASS:
                            parameters.emplace_back(0.2437);
                            parameters.emplace_back(0.2509);
                            parameters.emplace_back(0.1723);
                            parameters.emplace_back(0.0609);
                            break;
                        case NET_TYPE_HIGHPASS:
                            parameters.emplace_back(0.1040);
                            parameters.emplace_back(0.1009);
                            parameters.emplace_back(0.1470);
                            parameters.emplace_back(0.4159);
                            break;
                    }
                    break;
                }
                case NET_GAUSSIAN:
                {
                    switch (type)
                    {
                        case NET_TYPE_LOWPASS:
                            parameters.emplace_back(0.3253);
                            parameters.emplace_back(0.2235);
                            parameters.emplace_back(0.1674);
                            parameters.emplace_back(0.0768);
                            break;
                        case NET_TYPE_HIGHPASS:
                            parameters.emplace_back(0.0767);
                            parameters.emplace_back(0.1116);
                            parameters.emplace_back(0.1491);
                            parameters.emplace_back(0.3251);
                            break;
                    }
                    break;
                }
                case NET_LEGENDRE:
                {
                    switch (type)
                    {
                        case NET_TYPE_LOWPASS:
                            parameters.emplace_back(0.2294);
                            parameters.emplace_back(0.2365);
                            parameters.emplace_back(0.2034);
                            parameters.emplace_back(0.0910);
                            break;
                        case NET_TYPE_HIGHPASS:
                            parameters.emplace_back(0.1104);
                            parameters.emplace_back(0.1073);
                            parameters.emplace_back(0.1246);
                            parameters.emplace_back(0.2783);
                            break;
                    }
                }
                case NET_LINEARPHASE:
                {
                    switch (type)
                    {
                        case NET_TYPE_LOWPASS:
                            parameters.emplace_back(0.3285);
                            parameters.emplace_back(0.2255);
                            parameters.emplace_back(0.1578);
                            parameters.emplace_back(0.0632);
                            break;
                        case NET_TYPE_HIGHPASS:
                            parameters.emplace_back(0.0741);
                            parameters.emplace_back(0.1079);
                            parameters.emplace_back(0.1524);
                            parameters.emplace_back(0.3853);
                            break;
                    }
                    break;
                }
                case NET_LINKWITZRILEY:
                {
                    switch (type)
                    {
                        case NET_TYPE_LOWPASS:
                            parameters.emplace_back(0.3000);
                            parameters.emplace_back(0.2533);
                            parameters.emplace_back(0.1500);
                            parameters.emplace_back(0.0563);
                            break;
                        case NET_TYPE_HIGHPASS:
                            parameters.emplace_back(0.0844);
                            parameters.emplace_back(0.1000);
                            parameters.emplace_back(0.1688);
                            parameters.emplace_back(0.4501);
                            break;
                    }
                    break;
                }
            }
            break;
        }
    }

    if (parameters.empty())
    {
        throw std::runtime_error("Filter parameters was not correctly populated");
    }
    return parameters;
}
}
}
