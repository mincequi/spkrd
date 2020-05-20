
#pragma once

#include <vector>
#include <algorithm>

namespace gspk
{
/// Point class to exchange plot coordinates between classes.
/// I need a double on the y-axis since decimal values are required
/// in the conversion to axis mapped coordinates
class point
{
public:
    explicit point(int x, double y) : m_x(x), m_y(y) {}

    auto get_x() const -> int { return m_x; }

    auto get_y() const -> double { return m_y; }

    void set_x(int x) { m_x = x; }

    void set_y(double y) { m_y = y; }

private:
    int m_x;
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

inline auto lerp(std::vector<point> const& freq_resp_points, double x) -> double
{
    auto first_i = std::lower_bound(freq_resp_points.begin(),
                                    freq_resp_points.end(),
                                    x,
                                    gspk::comparison{});

    if (first_i != begin(freq_resp_points))
    {
        std::advance(first_i, -1);
    }

    auto second_i = std::upper_bound(begin(freq_resp_points),
                                     end(freq_resp_points),
                                     x,
                                     gspk::comparison{});

    double x0 = first_i->get_x();
    double x1 = second_i->get_x();

    if (x0 == x1)
    {
        return first_i->get_y();
    }

    double y0 = first_i->get_y();
    double y1 = second_i->get_y();

    return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
}

}
