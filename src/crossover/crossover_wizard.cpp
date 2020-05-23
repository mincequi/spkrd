/*
  crossover_wizard Copyright (C) 2002 Daniel Sundberg

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License version 2
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "crossover_wizard.hpp"

#include "signal.hpp"
#include "common.h"

namespace spkrd
{
crossover_wizard::crossover_wizard()
    : Gtk::ScrolledWindow(), m_vbox(Gtk::ORIENTATION_VERTICAL), m_drivers{nullptr}
{
    signal_crossover_selected.connect(
        sigc::mem_fun(*this, &crossover_wizard::on_crossover_selected));

    set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    set_border_width(2);

    m_vbox.set_valign(Gtk::ALIGN_START);
    add(m_vbox);

    signal_drivers_loaded.connect(
        sigc::mem_fun(*this, &crossover_wizard::on_drivers_loaded));

    show_all();
}

void crossover_wizard::on_crossover_selected(Crossover* crossover)
{
    // Remove all previous filter settings
    for (auto* children : m_vbox.get_children())
    {
        m_vbox.remove(*children);
        // Delete the memory as this is no longer handled
        // by the widget
        delete children;
    }

    int index = 0;

    for (auto& network : crossover->networks())
    {
        auto const type = crossover->get_type();

        // The following code is here because we want pretty labels for what
        // was each subfilter is there for
        if (type == CROSSOVER_TYPE_TWOWAY)
        {
            auto const description = index == 0 ? _("Woofer/midrange filter")
                                                : _("Tweeter filter");

            m_vbox.pack_start(
                *Gtk::make_managed<filter_link_frame>(&network, description, m_drivers));
        }
        else if (type == CROSSOVER_TYPE_THREEWAY)
        {
            auto const description = index == 0 ? _("Woofer filter")
                                                : index == 1 ? _("Midrange filter")
                                                             : _("Tweeter filter");

            m_vbox.pack_start(
                *Gtk::make_managed<filter_link_frame>(&network, description, m_drivers));
        }
        else if (type == (CROSSOVER_TYPE_TWOWAY | CROSSOVER_TYPE_LOWPASS))
        {
            auto const description = index == 0 ? _("Woofer filter")
                                                : index == 1 ? _("Woofer/midrange filter")
                                                             : _("Tweeter filter");

            m_vbox.pack_start(
                *Gtk::make_managed<filter_link_frame>(&network, description, m_drivers));
        }
        else
        {
            m_vbox.pack_start(
                *Gtk::make_managed<filter_link_frame>(&network, _("filter"), m_drivers));
        }
        ++index;
    }
}

void crossover_wizard::on_drivers_loaded(std::shared_ptr<driver_list const> const& drivers)
{
    m_drivers = drivers;
}
}
