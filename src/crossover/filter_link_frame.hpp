/*
  $Id$

  filterlinkframe Copyright (C) 2002 Daniel Sundberg

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

#pragma once

#include "crossover.hpp"
#include "plot.hpp"
#include "driver_list.hpp"

#include <gtkmm/box.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/frame.h>
#include <gtkmm/spinbutton.h>

namespace spkrd
{
inline auto is_driver_and_filter_matched(driver const& driver,
                                         filter_network const& network) -> bool
{
    return (is_tweeter_driver(driver.get_type())
            && ((network.get_type() & NET_TYPE_HIGHPASS) != 0))
           || (is_bass_driver(driver.get_type())
               && ((network.get_type() & NET_TYPE_LOWPASS) != 0));
}

/// filter_link_frame holds the information for one driver in the interface
class filter_link_frame : public Gtk::Frame
{
public:
    filter_link_frame(filter_network* network,
                      std::string const& description,
                      std::shared_ptr<driver_list const> const& drivers);

    ~filter_link_frame() override;

private:
    void on_order_selected(Gtk::ComboBoxText const* order_box, Gtk::ComboBoxText* type_box);

    void on_attenutation_changed();

    void on_param_changed();

    void on_net_updated(filter_network* net);

    void on_plot_crossover();

    void on_clear_and_plot();

    void on_drivers_loaded(std::shared_ptr<driver_list const> const& drivers);

    void on_settings_changed(std::string const& s);

    void on_impedance_correction_changed(driver const& speaker);

    void on_update_attenutation(driver const& speaker);

private:
    void connect_signals();

    void initialise_speaker_combobox();

    void initialise_damping();

    void initialise_highpass_filter();

    void initialise_lowpass_filter();

    void perform_spice_simulation();

    auto plot_line_colour() const -> Gdk::Color;

    /// Update the lowpass network. Note this must be called before
    /// update_highpass_network
    auto update_lowpass_network(std::int32_t index, driver& speaker) -> std::int32_t;

    void update_highpass_network(std::int32_t index, driver& speaker);

private:
    void set_family(Gtk::ComboBoxText* option_menu, int order, int family);

private:
    Gtk::Label m_label;
    Gtk::Box m_vbox;
    Gtk::ComboBoxText m_speaker_combo;

    /* For lowpass filter */
    Gtk::ComboBoxText* m_lower_order_combo;
    Gtk::ComboBoxText* m_lower_type_combo;
    Glib::RefPtr<Gtk::Adjustment> m_lower_co_freq_digits;
    Gtk::SpinButton* m_lower_co_freq_spinbutton;

    /* For highpass filter */
    Gtk::ComboBoxText* m_higher_order_combo;
    Gtk::ComboBoxText* m_higher_type_combo;
    Glib::RefPtr<Gtk::Adjustment> m_higher_co_freq_digits;
    Gtk::SpinButton* m_higher_co_freq_spinbutton;

    /* For both */
    Gtk::CheckButton m_inv_pol_checkbutton;
    Glib::RefPtr<Gtk::Adjustment> m_dampening_digits;
    Gtk::SpinButton m_damp_spinbutton;
    Gtk::CheckButton m_imp_corr_checkbutton;
    Gtk::CheckButton m_adv_imp_model_checkbutton;

    filter_network* m_network;
    std::string m_description;
    std::shared_ptr<driver_list const> m_drivers;
    bool m_enable_edit{false};

    std::int32_t m_filter_plot_index{-1};
    std::vector<point> m_points;
};
}
