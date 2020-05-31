
#pragma once

#include <cstdint>
#include <vector>
#include <algorithm>

namespace spkrd
{
/// Point class to exchange plot coordinates between classes.
/// I need a double on the y-axis since decimal values are required
/// in the conversion to axis mapped coordinates
class point
{
public:
    explicit point(std::int32_t x, double y) : m_x(x), m_y(y) {}

    auto get_x() const -> int { return m_x; }

    auto get_y() const -> double { return m_y; }

    void set_x(int x) { m_x = x; }

    void set_y(double y) { m_y = y; }

private:
    std::int32_t m_x;
    double m_y;
};

struct comparison
{
    auto operator()(const point& left, const point& right) const noexcept -> bool
    {
        return left.get_x() <= right.get_x();
    }

    auto operator()(const point& left, int right) const noexcept -> bool
    {
        return left.get_x() <= right;
    }

    auto operator()(int left, const point& right) const noexcept -> bool
    {
        return left <= right.get_x();
    }
};

inline auto lerp(std::vector<point> const& freq_resp_points, double const x) -> double
{
    auto first_i = std::lower_bound(begin(freq_resp_points),
                                    end(freq_resp_points),
                                    x,
                                    comparison{});

    if (first_i != begin(freq_resp_points))
    {
        std::advance(first_i, -1);
    }

    auto second_i = std::upper_bound(begin(freq_resp_points),
                                     end(freq_resp_points),
                                     x,
                                     comparison{});

    auto const x0 = first_i->get_x();
    auto const x1 = second_i->get_x();

    if (x0 == x1)
    {
        return first_i->get_y();
    }

    auto const y0 = first_i->get_y();
    auto const y1 = second_i->get_y();

    return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
}
}
