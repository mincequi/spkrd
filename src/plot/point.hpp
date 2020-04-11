
#pragma once

namespace gspk
{
/// Point class to exchange plot coordinates between classes.
/// I need a double on the y-axis since decimal values are required
/// in the conversion to axis mappes coordinates
class point
{
public:
    point() = default;

    point(int x, double y) : m_x(x), m_y(y) {}

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

}
