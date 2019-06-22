/* plot.hpp
 *
 * $Id$
 *
 * Copyright (C) 2001-2002 Daniel Sundberg <dss@home.se>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#ifndef __GSPEAKERS_PLOT_H
#define __GSPEAKERS_PLOT_H

#include <gdkmm/color.h>
#include <gtkmm/drawingarea.h>

#include <string>
#include <vector>

/// This is max and min y-axis-values
constexpr auto MAX_NEG_VALUE = -40;
constexpr auto MAX_POS_VALUE = 10;
/// 30 is a nice little space to have between the graph and the window-border
constexpr auto BOX_FRAME_SIZE = 30;
/// We want vertical lines on every 5 dB
constexpr auto N_VERTICAL_LINES = 2 * (-MAX_NEG_VALUE + MAX_POS_VALUE) / 10 - 1;
/// Upper frequency limit
constexpr auto UPPER_LIMIT = 1000;

/// Point class to exchange plot coordinates between classes.
/// I need a double on the y-axis since decimal values are required
/// in the conversion to axis mappes coordinates
namespace GSpeakers
{
class Point
{
public:
    Point() = default;

    Point(int x, double y) : m_x(x), m_y(y) {}

    int get_x() const { return m_x; }

    double get_y() const { return m_y; }

    void set_x(int x) { m_x = x; }

    void set_y(double y) { m_y = y; }

private:
    int m_x;
    double m_y;
};

struct comparison
{
    bool operator()(const Point& left, const Point& right) const noexcept
    {
        return left.get_x() <= right.get_x();
    }

    bool operator()(const Point& left, int right) const noexcept { return left.get_x() <= right; }

    bool operator()(int left, const Point& right) const noexcept { return left <= right.get_x(); }
};

} // namespace GSpeakers

/// This is a class that can draw plots. It has add_plot, remove_plot
/// and a few other methods you can use. It has logarithmic x-axis and
/// a flat y-axis. The x-axis is supposed to be used for frequency and
/// the y-axis can be used for something linear, for example dB.
class plot : public Gtk::DrawingArea
{
public:
    /// lower_x: x axis lower limit
    /// upper_x: x axis upper limit
    /// lower_y: y axis lower limit
    /// upper_y: y axis upper limit
    /// logx:    logarithmic x axis
    /// y_zero_freq: draw a thicker horizontal line on this y-value
    plot(int lower_x = 20,
         int upper_x = 1000,
         int lower_y = -40,
         int upper_y = 10,
         bool logx = true,
         int y_zero_freq = 0,
         bool enable_sec_scale = false);

    int add_plot(std::vector<GSpeakers::Point>& points, Gdk::Color& colour);

    void remove_plot(int n);

    void hide_plot(int n);

    void remove_all_plots();

    void redraw(Cairo::RefPtr<Cairo::Context> const& context);

    void set_font(const std::string& font);

    void select_plot(int index);

    void replace_plot(int index, std::vector<GSpeakers::Point>& p, Gdk::Color& ref_color);

    void set_y_label(std::string const& text);

    void set_y_label2(std::string const& text);

protected:
    bool on_draw(Cairo::RefPtr<Cairo::Context> const& context) override;

    /// Copy the area that needs to be updated from the pixmap
    /// to the window
    [[deprecated]] bool on_expose_event(GdkEventExpose* event);

    // void on_show();

    [[deprecated]] bool on_configure_event(GdkEventConfigure* event) override;

protected:
    /// Y axis magnitude points for the plots (dbmag)
    std::vector<std::vector<GSpeakers::Point>> m_points;
    /// Plot line colours
    std::vector<Gdk::Color> m_colors;
    /// Visible plots
    std::vector<bool> m_visible_plots;

private:
    Glib::ustring int_to_ustring3(int d);

    /// Draw vertical lines in a logarithmic plot
    void draw_log_grid(Cairo::RefPtr<Cairo::Context> const& context);
    /// Draw vertical lines in a linear plot
    void draw_linear_grid(Cairo::RefPtr<Cairo::Context> const& context);
    /// Draw a text box in a Cairo context using Pango
    void draw_text_box(Cairo::RefPtr<Cairo::Context> const& context,
                       std::string const& text,
                       int const center_x,
                       int const center_y);

private:
    int m_linesize;
    int m_lower_x;
    int m_upper_x;
    int m_lower_y;
    int m_upper_y;
    int m_y_zero_freq;
    int m_enable_sec_scale;

    bool m_logx;
    bool m_visible;
    int m_selected_plot;

    std::string m_y_label1, m_y_label2;

    Gdk::Color black, white;
};

#endif
