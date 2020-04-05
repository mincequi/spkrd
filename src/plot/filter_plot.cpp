/* gspeakersboxplot
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

#include "filter_plot.hpp"

#include "common.h"

filter_plot::filter_plot() : m_plot(1, 20000)
{
    add(m_plot);

    signal_add_crossover_plot.connect(sigc::mem_fun(*this, &filter_plot::on_add_plot));
    signal_crossover_selected.connect(sigc::mem_fun(*this, &filter_plot::on_crossover_selected));

    m_plot.set_y_label(_("Magnitude / dB"));

    show_all();
}

auto filter_plot::on_add_plot(std::vector<GSpeakers::Point> const& points,
                              Gdk::Color const& color,
                              int& i,
                              Net* n) -> int
{
    if (i == -1)
    {
        i = m_plot.add_plot(points, color);
    }
    else
    {
        m_plot.replace_plot(i, points, color);
    }
    m_plot.select_plot(-1);
    return 0;
}

void filter_plot::clear() { m_plot.remove_all_plots(); }

void filter_plot::on_crossover_selected(Crossover*) { clear(); }
