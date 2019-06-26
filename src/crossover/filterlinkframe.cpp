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

#include "filterlinkframe.h"

#include "common.h"
#include "plot.hpp"

#include <gtkmm/messagedialog.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

FilterLinkFrame::FilterLinkFrame(filter_network* net,
                                 const std::string& description,
                                 driver_list* driver_list)
    : Gtk::Frame(""),
      m_vbox(Gtk::ORIENTATION_VERTICAL),
      m_lower_co_freq_digits(Gtk::Adjustment::create(2000, 1, 20000, 1, 100)),
      m_higher_co_freq_digits(Gtk::Adjustment::create(2000, 1, 20000, 1, 100)),
      m_inv_pol_checkbutton(_("Invert polarity"), false),
      m_dampening_digits(Gtk::Adjustment::create(0, 0, 100, 1, 5.0)),
      m_damp_spinbutton(m_dampening_digits),
      m_imp_corr_checkbutton(_("Impedance correction")),
      m_adv_imp_model_checkbutton(_("Use advance driver impendance model")),
      m_net(net),
      m_description(description),
      m_speaker_list(driver_list)
{
    set_border_width(2);
    set_shadow_type(Gtk::SHADOW_NONE);

    m_label.set_markup("<b>" + description + "</b>");
    set_label_widget(m_label);

    m_vbox.set_border_width(12);

    this->initialise_speaker_combobox();

    m_vbox.pack_start(m_imp_corr_checkbutton);
    m_imp_corr_checkbutton.set_active(m_net->get_has_imp_corr());

    m_vbox.pack_start(m_adv_imp_model_checkbutton);
    m_adv_imp_model_checkbutton.set_active(m_net->get_adv_imp_model() == 1);

    auto hbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
    m_vbox.pack_start(*hbox);
    hbox->pack_start(*Gtk::manage(new Gtk::Label(_("Damping: "))));
    hbox->pack_start(m_damp_spinbutton);
    hbox->pack_start(*Gtk::manage(new Gtk::Label("dB")));

    if (m_net->get_has_damp())
    {
        this->initialise_dampening();
    }
    if (net->get_type() == NET_TYPE_HIGHPASS)
    {
        this->initialise_highpass_filter();
    }
    if (m_net->get_type() == NET_TYPE_LOWPASS)
    {
        this->initialise_lowpass_filter();
    }

    add(m_vbox);
    show_all();

    this->connect_signals();

    g_settings.defaultValueString("SPICECmdLine", "gnucap");

    m_init = false;
    m_enable_edit = true;
}

void FilterLinkFrame::initialise_dampening()
{
    // Set damp value in dB here
    auto const r_ser = m_net->get_damp_R1().get_value();

    driver speaker;

    if (m_speaker_list)
    {
        speaker = m_speaker_list->get_by_id_string(m_speaker_combo.get_active_text());
    }

    m_damp_spinbutton.set_value(std::round(
        20
        * std::log10(r_ser
                         / (g_settings.getValueBool("UseDriverImpedance") ? speaker.get_imp()
                                                                          : speaker.get_rdc())
                     + 1.0)));
}

void FilterLinkFrame::initialise_speaker_combobox()
{
    std::string const& speaker_name = m_net->get_speaker();

    m_speaker_combo.remove_all();

    if (!speaker_name.empty())
    {
        m_speaker_combo.append(speaker_name);
    }

    if (m_speaker_list != nullptr)
    {
        for (auto& iter : m_speaker_list->data())
        {
            // TODO: only insert speakers appropriate for this particular crossover
            if (speaker_name != iter.get_id_string())
            {
                m_speaker_combo.append(iter.get_id_string());
            }
        }
    }
    m_speaker_combo.set_active(0);
    m_vbox.pack_start(m_speaker_combo);
}

void FilterLinkFrame::initialise_highpass_filter()
{
    auto frame = Gtk::manage(new Gtk::Frame(""));

    frame->set_border_width(2);
    frame->set_shadow_type(Gtk::SHADOW_NONE);

    {
        auto label = Gtk::manage(new Gtk::Label());
        label->set_markup("<b>" + Glib::ustring(_("Highpass")) + "</b>");
        frame->set_label_widget(*label);
    }

    m_vbox.pack_start(*frame);
    auto vbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    vbox->set_border_width(12);
    frame->add(*vbox);

    /* Setup menus */
    m_higher_order_combo = Gtk::manage(new Gtk::ComboBoxText());
    m_higher_order_combo->append("1");
    m_higher_order_combo->append("2");
    m_higher_order_combo->append("3");
    m_higher_order_combo->append("4");
    m_higher_order_combo->set_active(m_net->get_highpass_order() - 1);

    m_higher_type_combo = Gtk::manage(new Gtk::ComboBoxText());

    on_order_selected(m_higher_order_combo, m_higher_type_combo);

    /* menus ready */
    auto hbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
    vbox->pack_start(*hbox);
    hbox->pack_start(*Gtk::manage(new Gtk::Label(_("Order: "), Gtk::ALIGN_START)));
    hbox->pack_start(*m_higher_order_combo);
    vbox->pack_start(*m_higher_type_combo);

    hbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
    hbox->pack_start(*Gtk::manage(new Gtk::Label(_("Cutoff: "))));
    m_higher_co_freq_spinbutton = Gtk::manage(new Gtk::SpinButton(m_higher_co_freq_digits));
    hbox->pack_start(*m_higher_co_freq_spinbutton);
    {
        auto label = Gtk::manage(new Gtk::Label(" Hz"));
        label->set_alignment(Gtk::ALIGN_START);
        hbox->pack_start(*label);
    }
    vbox->pack_start(*hbox);

    this->set_family(m_higher_type_combo, m_net->get_highpass_order(), m_net->get_highpass_family());
}

void FilterLinkFrame::initialise_lowpass_filter()
{
    auto frame = Gtk::manage(new Gtk::Frame(""));
    frame->set_border_width(2);
    frame->set_shadow_type(Gtk::SHADOW_NONE);
    m_vbox.pack_start(*frame);

    {
        auto label = Gtk::manage(new Gtk::Label());
        label->set_markup("<b>" + Glib::ustring(_("Lowpass")) + "</b>");
        frame->set_label_widget(*label);
    }

    auto vbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    vbox->set_border_width(12);
    frame->add(*vbox);

    // Setup menus
    m_lower_order_combo = Gtk::manage(new Gtk::ComboBoxText());
    m_lower_order_combo->append("1");
    m_lower_order_combo->append("2");
    m_lower_order_combo->append("3");
    m_lower_order_combo->append("4");
    m_lower_order_combo->set_active(m_net->get_lowpass_order() - 1);

    m_lower_type_combo = Gtk::manage(new Gtk::ComboBoxText());

    on_order_selected(m_lower_order_combo, m_lower_type_combo);

    /* menus ready */
    auto hbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
    vbox->pack_start(*hbox);
    hbox->pack_start(*Gtk::manage(new Gtk::Label(_("Order: "))));
    hbox->pack_start(*m_lower_order_combo);
    vbox->pack_start(*m_lower_type_combo);

    hbox = Gtk::manage(new Gtk::HBox());
    hbox->pack_start(*Gtk::manage(new Gtk::Label(_("Cutoff: "))));

    m_lower_co_freq_spinbutton = Gtk::manage(new Gtk::SpinButton(m_lower_co_freq_digits));

    hbox->pack_start(*m_lower_co_freq_spinbutton);
    hbox->pack_start(*Gtk::manage(new Gtk::Label(_("Hz"))));

    vbox->pack_start(*hbox);

    this->set_family(m_lower_type_combo, m_net->get_lowpass_order(), m_net->get_lowpass_family());
}

void FilterLinkFrame::connect_signals()
{
    m_speaker_combo.signal_changed().connect(sigc::mem_fun(*this, &FilterLinkFrame::on_param_changed));

    if ((m_net->get_type() & NET_TYPE_LOWPASS) != 0)
    {
        m_lower_order_combo->signal_changed().connect(
            sigc::bind(sigc::mem_fun(*this, &FilterLinkFrame::on_order_selected),
                       m_lower_order_combo,
                       m_lower_type_combo));
        m_lower_type_combo->signal_changed().connect(
            sigc::mem_fun(*this, &FilterLinkFrame::on_param_changed));
        m_lower_order_combo->signal_changed().connect(
            sigc::mem_fun(*this, &FilterLinkFrame::on_param_changed));
        m_lower_co_freq_spinbutton->signal_value_changed().connect(
            sigc::mem_fun(*this, &FilterLinkFrame::on_param_changed));
    }
    if ((m_net->get_type() & NET_TYPE_HIGHPASS) != 0)
    {
        m_higher_order_combo->signal_changed().connect(
            sigc::bind(sigc::mem_fun(*this, &FilterLinkFrame::on_order_selected),
                       m_higher_order_combo,
                       m_higher_type_combo));
        m_higher_order_combo->signal_changed().connect(
            sigc::mem_fun(*this, &FilterLinkFrame::on_param_changed));
        m_higher_type_combo->signal_changed().connect(
            sigc::mem_fun(*this, &FilterLinkFrame::on_param_changed));
        m_higher_co_freq_spinbutton->signal_value_changed().connect(
            sigc::mem_fun(*this, &FilterLinkFrame::on_param_changed));
    }

    m_imp_corr_checkbutton.signal_toggled().connect(
        sigc::mem_fun(*this, &FilterLinkFrame::on_param_changed));

    m_damp_spinbutton.signal_value_changed().connect(
        sigc::mem_fun(*this, &FilterLinkFrame::on_param_changed));

    m_adv_imp_model_checkbutton.signal_toggled().connect(
        sigc::mem_fun(*this, &FilterLinkFrame::on_param_changed));

    signal_net_modified_by_user.connect(sigc::mem_fun(*this, &FilterLinkFrame::on_net_updated));
    signal_plot_crossover.connect(sigc::mem_fun(*this, &FilterLinkFrame::on_clear_and_plot));

    signal_speakerlist_loaded.connect(sigc::mem_fun(*this, &FilterLinkFrame::on_speakerlist_loaded));
    g_settings.settings_changed.connect(sigc::mem_fun(*this, &FilterLinkFrame::on_settings_changed));
}

FilterLinkFrame::~FilterLinkFrame() = default;

void FilterLinkFrame::on_order_selected(Gtk::ComboBoxText const* order_box,
                                        Gtk::ComboBoxText* type_box)
{
    if (type_box->get_active())
    {
        // Block the signal from being emitted until the end of the function
        // otherwise on_param_changed() will be called prematurely
        m_enable_edit = false;
        type_box->remove_all();
    }

    switch (order_box->get_active_row_number() + 1)
    {
        case NET_ORDER_1ST:
            std::puts("first order");
            type_box->append("Butterworth");
            break;
        case NET_ORDER_2ND:
            std::puts("second order");
            type_box->append("Bessel");
            type_box->append("Butterworth");
            type_box->append("Chebychev");
            type_box->append("Linkwitz-Riley");
            break;
        case NET_ORDER_3RD:
            std::puts("third order");
            type_box->append("Bessel");
            type_box->append("Butterworth");
            break;
        case NET_ORDER_4TH:
            std::puts("fourth order");
            type_box->append("Bessel");
            type_box->append("Butterworth");
            type_box->append("Gaussian");
            type_box->append("Legendre");
            type_box->append("Linear-Phase");
            type_box->append("Linkwitz-Riley");
            break;
    }
    type_box->set_active(0);

    m_enable_edit = true;

    std::puts("returning from on_order_selected");
}

void FilterLinkFrame::on_settings_changed(const std::string& setting)
{
    if (setting == "DisableFilterAmp")
    {
        on_param_changed();
    }
}

void FilterLinkFrame::on_param_changed()
{
    if (m_enable_edit)
    {
        std::puts("FilterLinkFrame::on_param_changed");

        m_enable_edit = false;

        driver speaker;

        if (m_speaker_list != nullptr)
        {
            speaker = m_speaker_list->get_by_id_string(m_speaker_combo.get_active_text());
        }

        m_net->set_speaker(speaker.get_id_string());

        int index = 0;
        std::vector<double> num_params;

        if (m_net->get_type() == NET_TYPE_LOWPASS)
        {
            m_net->set_lowpass_order(m_lower_order_combo->get_active_row_number() + 1);
            double const cutoff = m_lower_co_freq_spinbutton->get_value();

            switch (m_net->get_lowpass_order())
            {
                case NET_ORDER_1ST:
                    num_params = get_filter_params(NET_BUTTERWORTH, NET_ORDER_1ST, NET_TYPE_LOWPASS);
                    m_net->parts()[index].set_value((speaker.get_rdc() / (num_params[0] * cutoff))
                                                    * 1000);
                    m_net->parts()[index++].set_unit("m");
                    m_net->set_lowpass_family(NET_BUTTERWORTH);
                    break;
                case NET_ORDER_2ND:
                    switch (m_lower_type_combo->get_active_row_number())
                    {
                        case 0: // bessel
                            num_params = get_filter_params(NET_BESSEL, NET_ORDER_2ND, NET_TYPE_LOWPASS);
                            m_net->set_lowpass_family(NET_BESSEL);
                            break;
                        case 1: // butterworth
                            num_params = get_filter_params(NET_BUTTERWORTH,
                                                           NET_ORDER_2ND,
                                                           NET_TYPE_LOWPASS);
                            m_net->set_lowpass_family(NET_BUTTERWORTH);
                            break;
                        case 2: // chebychev
                            num_params = get_filter_params(NET_CHEBYCHEV,
                                                           NET_ORDER_2ND,
                                                           NET_TYPE_LOWPASS);
                            m_net->set_lowpass_family(NET_CHEBYCHEV);
                            break;
                        case 3: // linkwitz-riley
                            num_params = get_filter_params(NET_LINKWITZRILEY,
                                                           NET_ORDER_2ND,
                                                           NET_TYPE_LOWPASS);
                            m_net->set_lowpass_family(NET_LINKWITZRILEY);
                            break;
                    }
                    /* inductor */
                    m_net->parts()[index].set_value(speaker.get_rdc() * num_params[0] / cutoff * 1000);
                    m_net->parts()[index++].set_unit("m");
                    /* capacitor */
                    m_net->parts()[index].set_value(num_params[1] / (speaker.get_rdc() * cutoff)
                                                    * 1000000);
                    m_net->parts()[index++].set_unit("u");
                    break;
                case NET_ORDER_3RD:
                    switch (m_lower_type_combo->get_active_row_number())
                    {
                        case 0: // bessel
                            num_params = get_filter_params(NET_BESSEL, NET_ORDER_3RD, NET_TYPE_LOWPASS);
                            m_net->set_lowpass_family(NET_BESSEL);
                            break;
                        case 1: // butterworth
                            num_params = get_filter_params(NET_BUTTERWORTH,
                                                           NET_ORDER_3RD,
                                                           NET_TYPE_LOWPASS);
                            m_net->set_lowpass_family(NET_BUTTERWORTH);
                            break;
                    }
                    /* inductor */
                    m_net->parts()[index].set_value(speaker.get_rdc() * num_params[0] / cutoff * 1000);
                    m_net->parts()[index++].set_unit("m");
                    /* capacitor */
                    m_net->parts()[index].set_value(num_params[1] / (speaker.get_rdc() * cutoff)
                                                    * 1000000);
                    m_net->parts()[index++].set_unit("u");
                    /* inductor */
                    m_net->parts()[index].set_value(speaker.get_rdc() * num_params[2] / cutoff * 1000);
                    m_net->parts()[index++].set_unit("m");
                    break;
                case NET_ORDER_4TH:
                    switch (m_lower_type_combo->get_active_row_number())
                    {
                        case 0: // bessel
                            num_params = get_filter_params(NET_BESSEL, NET_ORDER_4TH, NET_TYPE_LOWPASS);
                            m_net->set_lowpass_family(NET_BESSEL);
                            break;
                        case 1: // butterworth
                            num_params = get_filter_params(NET_BUTTERWORTH,
                                                           NET_ORDER_4TH,
                                                           NET_TYPE_LOWPASS);
                            m_net->set_lowpass_family(NET_BUTTERWORTH);
                            break;
                        case 2: // gaussian
                            num_params = get_filter_params(NET_GAUSSIAN,
                                                           NET_ORDER_4TH,
                                                           NET_TYPE_LOWPASS);
                            m_net->set_lowpass_family(NET_GAUSSIAN);
                            break;
                        case 3: // legendre
                            num_params = get_filter_params(NET_LEGENDRE,
                                                           NET_ORDER_4TH,
                                                           NET_TYPE_LOWPASS);
                            m_net->set_lowpass_family(NET_LEGENDRE);
                            break;
                        case 4: // liner-phase
                            num_params = get_filter_params(NET_LINEARPHASE,
                                                           NET_ORDER_4TH,
                                                           NET_TYPE_LOWPASS);
                            m_net->set_lowpass_family(NET_LINEARPHASE);
                            break;
                        case 5: // linkwitz-riley
                            num_params = get_filter_params(NET_LINKWITZRILEY,
                                                           NET_ORDER_4TH,
                                                           NET_TYPE_LOWPASS);
                            m_net->set_lowpass_family(NET_LINKWITZRILEY);
                            break;
                    }
                    // inductor
                    m_net->parts()[index].set_value(speaker.get_rdc() * num_params[0] / cutoff * 1000);
                    m_net->parts()[index++].set_unit("m");
                    /* capacitor */
                    m_net->parts()[index].set_value(num_params[1] / (speaker.get_rdc() * cutoff)
                                                    * 1000000);
                    m_net->parts()[index++].set_unit("u");
                    /* inductor */
                    m_net->parts()[index].set_value(speaker.get_rdc() * num_params[2] / cutoff * 1000);
                    m_net->parts()[index++].set_unit("m");
                    /* capacitor */
                    m_net->parts()[index].set_value(num_params[3] / (speaker.get_rdc() * cutoff)
                                                    * 1000000);
                    m_net->parts()[index++].set_unit("u");
                    break;
            }
        }
        if (m_net->get_type() == NET_TYPE_HIGHPASS)
        {
            m_net->set_highpass_order(m_higher_order_combo->get_active_row_number() + 1);

            double cutoff = m_higher_co_freq_spinbutton->get_value();

            switch (m_net->get_highpass_order())
            {
                case NET_ORDER_1ST:
                    num_params = get_filter_params(NET_BUTTERWORTH, NET_ORDER_1ST, NET_TYPE_HIGHPASS);
                    m_net->parts()[index].set_value(num_params[0] / (speaker.get_rdc() * cutoff)
                                                    * 1000000);
                    m_net->parts()[index++].set_unit("u");
                    m_net->set_highpass_family(NET_BUTTERWORTH);
                    break;
                case NET_ORDER_2ND:
                    switch (m_higher_type_combo->get_active_row_number())
                    {
                        case 0: // bessel
                            num_params = get_filter_params(NET_BESSEL,
                                                           NET_ORDER_2ND,
                                                           NET_TYPE_HIGHPASS);
                            m_net->set_highpass_family(NET_BESSEL);
                            break;
                        case 1: // butterworth
                            num_params = get_filter_params(NET_BUTTERWORTH,
                                                           NET_ORDER_2ND,
                                                           NET_TYPE_HIGHPASS);
                            m_net->set_highpass_family(NET_BUTTERWORTH);
                            break;
                        case 2: // chebychev
                            num_params = get_filter_params(NET_CHEBYCHEV,
                                                           NET_ORDER_2ND,
                                                           NET_TYPE_HIGHPASS);
                            m_net->set_highpass_family(NET_CHEBYCHEV);
                            break;
                        case 3: // linkwitz-riley
                            num_params = get_filter_params(NET_LINKWITZRILEY,
                                                           NET_ORDER_2ND,
                                                           NET_TYPE_HIGHPASS);
                            m_net->set_highpass_family(NET_LINKWITZRILEY);
                            break;
                    }
                    /* capacitor */
                    m_net->parts()[index].set_value(num_params[0] / (speaker.get_rdc() * cutoff)
                                                    * 1000000);
                    m_net->parts()[index++].set_unit("u");
                    /* inductor */
                    m_net->parts()[index].set_value(num_params[1] * speaker.get_rdc() / cutoff * 1000);
                    m_net->parts()[index++].set_unit("m");
                    break;
                case NET_ORDER_3RD:
                    switch (m_higher_type_combo->get_active_row_number())
                    {
                        case 0: // bessel
                            num_params = get_filter_params(NET_BESSEL,
                                                           NET_ORDER_3RD,
                                                           NET_TYPE_HIGHPASS);
                            m_net->set_highpass_family(NET_BESSEL);
                            break;
                        case 1: // butterworth
                            num_params = get_filter_params(NET_BUTTERWORTH,
                                                           NET_ORDER_3RD,
                                                           NET_TYPE_HIGHPASS);
                            m_net->set_highpass_family(NET_BUTTERWORTH);
                            break;
                    }
                    /* capacitor */
                    m_net->parts()[index].set_value(num_params[0] / (speaker.get_rdc() * cutoff)
                                                    * 1000000);
                    m_net->parts()[index++].set_unit("u");
                    /* inductor */
                    m_net->parts()[index].set_value(num_params[1] * speaker.get_rdc() / cutoff * 1000);
                    m_net->parts()[index++].set_unit("m");
                    /* capacitor */
                    m_net->parts()[index].set_value(num_params[2] / (speaker.get_rdc() * cutoff)
                                                    * 1000000);
                    m_net->parts()[index++].set_unit("u");
                    break;
                case NET_ORDER_4TH:
                    switch (m_higher_type_combo->get_active_row_number())
                    {
                        case 0: // bessel
                            num_params = get_filter_params(NET_BESSEL,
                                                           NET_ORDER_4TH,
                                                           NET_TYPE_HIGHPASS);
                            m_net->set_highpass_family(NET_BESSEL);
                            break;
                        case 1: // butterworth
                            num_params = get_filter_params(NET_BUTTERWORTH,
                                                           NET_ORDER_4TH,
                                                           NET_TYPE_HIGHPASS);
                            m_net->set_highpass_family(NET_BUTTERWORTH);
                            break;
                        case 2: // gaussian
                            num_params = get_filter_params(NET_GAUSSIAN,
                                                           NET_ORDER_4TH,
                                                           NET_TYPE_HIGHPASS);
                            m_net->set_highpass_family(NET_GAUSSIAN);
                            break;
                        case 3: // legendre
                            num_params = get_filter_params(NET_LEGENDRE,
                                                           NET_ORDER_4TH,
                                                           NET_TYPE_HIGHPASS);
                            m_net->set_highpass_family(NET_LEGENDRE);
                            break;
                        case 4: // liner-phase
                            num_params = get_filter_params(NET_LINEARPHASE,
                                                           NET_ORDER_4TH,
                                                           NET_TYPE_HIGHPASS);
                            m_net->set_highpass_family(NET_LINEARPHASE);
                            break;
                        case 5: // linkwitz-riley
                            num_params = get_filter_params(NET_LINKWITZRILEY,
                                                           NET_ORDER_4TH,
                                                           NET_TYPE_HIGHPASS);
                            m_net->set_highpass_family(NET_LINKWITZRILEY);
                            break;
                    }

                    /* capacitor */
                    m_net->parts()[index].set_value((num_params[0] / (speaker.get_rdc() * cutoff))
                                                    * 1000000);
                    m_net->parts()[index++].set_unit("u");
                    /* inductor */
                    m_net->parts()[index].set_value((num_params[1] * speaker.get_rdc() / cutoff)
                                                    * 1000);
                    m_net->parts()[index++].set_unit("m");
                    /* capacitor */
                    m_net->parts()[index].set_value((num_params[2] / (speaker.get_rdc() * cutoff))
                                                    * 1000000);
                    m_net->parts()[index++].set_unit("u");
                    /* inductor */
                    m_net->parts()[index].set_value((num_params[3] * speaker.get_rdc() / cutoff)
                                                    * 1000);
                    m_net->parts()[index++].set_unit("m");
                    break;
            }
        }

        m_net->set_has_imp_corr(m_imp_corr_checkbutton.get_active());

        if (m_imp_corr_checkbutton.get_active())
        {
            // calc imp corr here
            m_net->get_imp_corr_C().set_value(speaker.get_lvc() / 1000
                                              / std::pow(speaker.get_rdc(), 2) * 1000000);
            m_net->get_imp_corr_C().set_unit("u");
            m_net->get_imp_corr_R().set_value(speaker.get_rdc());
        }

        m_net->set_adv_imp_model(m_adv_imp_model_checkbutton.get_active());

        m_net->set_has_damp(m_damp_spinbutton.get_value_as_int());

        if (m_damp_spinbutton.get_value_as_int())
        {
            // Calculate resistors for damping network
            auto const r_ser = speaker.get_rdc()
                               * (std::pow(10, m_damp_spinbutton.get_value() / 20) - 1);
            auto const r_par = speaker.get_rdc() + std::pow(speaker.get_rdc(), 2) / r_ser;
            m_net->get_damp_R2().set_value(r_ser);
            m_net->get_damp_R1().set_value(r_par);
        }

        signal_net_modified_by_wizard();

        if (g_settings.getValueBool("AutoUpdateFilterPlots"))
        {
            on_plot_crossover();
        }
        m_enable_edit = true;
    }
}

void FilterLinkFrame::on_net_updated(filter_network* net)
{
    if (m_net->get_id() == net->get_id())
    {
#ifndef NDEBUG
        std::puts("FilterLinkFrame::on_net_updated");
#endif

        if (g_settings.getValueBool("AutoUpdateFilterPlots"))
        {
            on_plot_crossover();
        }
    }
}

void FilterLinkFrame::on_clear_and_plot()
{
    my_filter_plot_index = -1;
    on_plot_crossover();
}

void FilterLinkFrame::on_speakerlist_loaded(driver_list* driver_list)
{
#ifndef NDEBUG
    std::puts("FilterLinkFrame::on_speakerlist_loaded");
#endif
    m_speaker_list = driver_list;

    std::string const& speaker_name = m_net->get_speaker();

    // Setup the speaker combo box
    bool speaker_is_in_speakerlist = false;

    m_speaker_combo.remove_all();

    if (m_speaker_list)
    {
        for (auto& iter : m_speaker_list->data())
        {
            /* TODO: only insert speakers appropriate for this particular crossover */
            if (speaker_name != iter.get_id_string())
            {
                m_speaker_combo.append(iter.get_id_string());
            }
            else
            {
                speaker_is_in_speakerlist = true;
            }
        }
    }
    if (speaker_is_in_speakerlist)
    {
        m_speaker_combo.prepend(speaker_name);
        m_speaker_combo.set_active(0);
    }
}

void FilterLinkFrame::on_plot_crossover()
{
    std::puts("DEBUG: plotting cross-over");

    // Create spice code for this net
    driver speaker;
    if (m_speaker_list)
    {
        speaker = m_speaker_list->get_by_id_string(m_speaker_combo.get_active_text());
    }

    std::string spice_filename;
    try
    {
        spice_filename = m_net->to_SPICE(speaker, g_settings.getValueBool("SPICEUseGNUCAP"));
    }
    catch (std::runtime_error const& e)
    {
        Gtk::MessageDialog(_("FilterLinkFrame::on_plot_crossover: ERROR: ") + Glib::ustring(e.what()),
                           false,
                           Gtk::MESSAGE_ERROR,
                           Gtk::BUTTONS_OK,
                           true)
            .run();
    }

    // run spice with created file
    std::string cmd = g_settings.getValueString("SPICECmdLine");

    if (g_settings.getValueBool("SPICEUseNGSPICE") || g_settings.getValueBool("SPICEUseGNUCAP"))
    {
        cmd += " -b " + spice_filename + " > " + spice_filename + ".out";
    }
    else
    {
        cmd += " -b -o " + spice_filename + ".out " + spice_filename;
    }
#ifndef NDEBUG
    std::cout << "FilterLinkFrame::on_plot_crossover: running SPICE with \"" + cmd + "\"\n";
#endif

    system(cmd.c_str());

#ifndef NDEBUG
    std::cout << "FilterLinkFrame::on_plot_crossover: SPICE done\n";
#endif

    /* extract spice output into a vector */
    std::string spice_output_file = spice_filename + ".out";
    std::ifstream fin(spice_output_file.c_str());

    if (fin.good())
    {
        bool output = false;
        points.clear();
        while (!fin.eof())
        {
            std::vector<char> buffer(100);
            fin.getline(buffer.data(), 100, '\n');
            if (g_settings.getValueBool("SPICEUseGNUCAP"))
            {
                if (buffer[0] == ' ')
                {
                    output = true;
                }
            }
            else
            {
                if (buffer[0] == '0')
                {
                    output = true;
                }
            }

            if (output)
            {
                if (g_settings.getValueBool("SPICEUseGNUCAP"))
                {
                    float const f_id = atof(buffer.data());
                    if (f_id != 0)
                    {
                        /* Check if we got a freq more than 10kHz */
                        char* substr_ptr = strstr(buffer.data(), "K");
                        float const freq = substr_ptr != nullptr ? f_id * 1000 : f_id;

                        substr_ptr = strtok(buffer.data(), " ");
                        substr_ptr = strtok(nullptr, " ");
                        float const db = g_ascii_strtod(substr_ptr, nullptr);
                        points.emplace_back(std::round(freq), db);
                    }
                }
                else
                {
                    if ((buffer[0] >= '0') && (buffer[0] <= '9'))
                    {
                        strtok(buffer.data(), "\t");
                        char* substr_ptr = strtok(nullptr, "\t");

                        float const freq = g_ascii_strtod(substr_ptr, nullptr);

                        substr_ptr = strtok(nullptr, "\t");

                        float const db = g_ascii_strtod(substr_ptr, nullptr);
                        points.emplace_back(std::round(freq), db);
                    }
                }
            }
        }
        /* send the spice data to the plot */
        /* TODO: improve color handling here */
        Gdk::Color c;
        if ((m_net->get_type() & NET_TYPE_LOWPASS) != 0)
        {
            c = Gdk::Color("blue");
        }
        else if ((m_net->get_type() & NET_TYPE_HIGHPASS) != 0)
        {
            c = Gdk::Color("red");
        }
        else if ((m_net->get_type() & NET_TYPE_BANDPASS) != 0)
        {
            c = Gdk::Color("darkgreen");
        }
        signal_add_crossover_plot(points, c, &my_filter_plot_index, m_net);

        if (m_enable_edit)
        {
            m_enable_edit = false;
            if ((m_net->get_type() & NET_TYPE_LOWPASS) != 0)
            {
                auto const location = std::find_if(rbegin(points), rend(points), [&](auto const& point) {
                    return point.get_y() > (-3 - m_damp_spinbutton.get_value());
                });

                auto const index = location == rend(points)
                                       ? 0ul
                                       : std::distance(location, rend(points)) - 1;

                points[index + 1].set_y(points[index + 1].get_y() + m_damp_spinbutton.get_value());
                points[index].set_y(points[index].get_y() + m_damp_spinbutton.get_value());

                double ydiff = points[index + 1].get_y() - points[index].get_y();
                int xdiff = points[index + 1].get_x() - points[index].get_x();
                double ytodbdiff = points[index].get_y() + 3;
                m_lower_co_freq_spinbutton->set_value((ytodbdiff / ydiff) * xdiff
                                                      + points[index + 1].get_x());
            }
            if ((m_net->get_type() & NET_TYPE_HIGHPASS) != 0)
            {
                auto const location = std::find_if(begin(points), end(points), [&](auto const& point) {
                    return point.get_y() >= (-3 - m_damp_spinbutton.get_value());
                });

                auto const index = std::distance(begin(points), location);

                points[index - 1].set_y(points[index - 1].get_y() + m_damp_spinbutton.get_value());
                points[index].set_y(points[index].get_y() + m_damp_spinbutton.get_value());

                double ydiff = points[index - 1].get_y() - points[index].get_y();
                int xdiff = points[index].get_x() - points[index - 1].get_x();
                double ytodbdiff = points[index].get_y() + 3;
                m_higher_co_freq_spinbutton->set_value(ytodbdiff / ydiff * xdiff
                                                       + points[index].get_x());
            }
            m_enable_edit = true;
        }
    }
}

std::vector<double> FilterLinkFrame::get_filter_params(int net_name_type, int net_order, int net_type)
{
    std::vector<double> nums;
    switch (net_order)
    {
        case NET_ORDER_1ST:
            switch (net_type)
            {
                case NET_TYPE_LOWPASS:
                    nums.push_back(6.28);
                    break;
                case NET_TYPE_HIGHPASS:
                    nums.push_back(0.159);
                    break;
            }
            break;
        case NET_ORDER_2ND:
            switch (net_name_type)
            {
                case NET_BESSEL:
                    switch (net_type)
                    {
                        case NET_TYPE_LOWPASS:
                            nums.push_back(0.2756);
                            nums.push_back(0.0912);
                            break;
                        case NET_TYPE_HIGHPASS:
                            nums.push_back(0.0912);
                            nums.push_back(0.2756);
                            break;
                    }
                    break;
                case NET_BUTTERWORTH:
                    switch (net_type)
                    {
                        case NET_TYPE_LOWPASS:
                            nums.push_back(0.2251);
                            nums.push_back(0.1125);
                            break;
                        case NET_TYPE_HIGHPASS:
                            nums.push_back(0.1125);
                            nums.push_back(0.2251);
                            break;
                    }
                    break;
                case NET_CHEBYCHEV:
                    switch (net_type)
                    {
                        case NET_TYPE_LOWPASS:
                            nums.push_back(0.1592);
                            nums.push_back(0.1592);
                            break;
                        case NET_TYPE_HIGHPASS:
                            nums.push_back(0.1592);
                            nums.push_back(0.1592);
                            break;
                    }
                    break;
                case NET_LINKWITZRILEY:
                    switch (net_type)
                    {
                        case NET_TYPE_LOWPASS:
                            nums.push_back(0.3183);
                            nums.push_back(0.0796);
                            break;
                        case NET_TYPE_HIGHPASS:
                            nums.push_back(0.3183);
                            nums.push_back(0.0796);
                            break;
                    }
                    break;
            }
            break;
        case NET_ORDER_3RD:
            switch (net_name_type)
            {
                case NET_BESSEL:
                    switch (net_type)
                    {
                        case NET_TYPE_LOWPASS:
                            nums.push_back(0.3294);
                            nums.push_back(0.1897);
                            nums.push_back(0.06592);
                            break;
                        case NET_TYPE_HIGHPASS:
                            nums.push_back(0.07911);
                            nums.push_back(0.1317);
                            nums.push_back(0.3953);
                            break;
                    }
                    break;
                case NET_BUTTERWORTH:
                    switch (net_type)
                    {
                        case NET_TYPE_LOWPASS:
                            nums.push_back(0.2387);
                            nums.push_back(0.2122);
                            nums.push_back(0.0796);
                            break;
                        case NET_TYPE_HIGHPASS:
                            nums.push_back(0.1061);
                            nums.push_back(0.1194);
                            nums.push_back(0.3183);
                            break;
                    }
                    break;
            }
            break;
        case NET_ORDER_4TH:
            switch (net_name_type)
            {
                case NET_BESSEL:
                    switch (net_type)
                    {
                        case NET_TYPE_LOWPASS:
                            nums.push_back(0.3583);
                            nums.push_back(0.2336);
                            nums.push_back(0.1463);
                            nums.push_back(0.0504);
                            break;
                        case NET_TYPE_HIGHPASS:
                            nums.push_back(0.0702);
                            nums.push_back(0.0862);
                            nums.push_back(0.0719);
                            nums.push_back(0.4983);
                            break;
                    }
                    break;
                case NET_BUTTERWORTH:
                    switch (net_type)
                    {
                        case NET_TYPE_LOWPASS:
                            nums.push_back(0.2437);
                            nums.push_back(0.2509);
                            nums.push_back(0.1723);
                            nums.push_back(0.0609);
                            break;
                        case NET_TYPE_HIGHPASS:
                            nums.push_back(0.1040);
                            nums.push_back(0.1009);
                            nums.push_back(0.1470);
                            nums.push_back(0.4159);
                            break;
                    }
                    break;
                case NET_GAUSSIAN:
                    switch (net_type)
                    {
                        case NET_TYPE_LOWPASS:
                            nums.push_back(0.3253);
                            nums.push_back(0.2235);
                            nums.push_back(0.1674);
                            nums.push_back(0.0768);
                            break;
                        case NET_TYPE_HIGHPASS:
                            nums.push_back(0.0767);
                            nums.push_back(0.1116);
                            nums.push_back(0.1491);
                            nums.push_back(0.3251);
                            break;
                    }
                    break;
                case NET_LEGENDRE:
                    switch (net_type)
                    {
                        case NET_TYPE_LOWPASS:
                            nums.push_back(0.2294);
                            nums.push_back(0.2365);
                            nums.push_back(0.2034);
                            nums.push_back(0.0910);
                            break;
                        case NET_TYPE_HIGHPASS:
                            nums.push_back(0.1104);
                            nums.push_back(0.1073);
                            nums.push_back(0.1246);
                            nums.push_back(0.2783);
                            break;
                    }
                case NET_LINEARPHASE:
                    switch (net_type)
                    {
                        case NET_TYPE_LOWPASS:
                            nums.push_back(0.3285);
                            nums.push_back(0.2255);
                            nums.push_back(0.1578);
                            nums.push_back(0.0632);
                            break;
                        case NET_TYPE_HIGHPASS:
                            nums.push_back(0.0741);
                            nums.push_back(0.1079);
                            nums.push_back(0.1524);
                            nums.push_back(0.3853);
                            break;
                    }
                    break;
                case NET_LINKWITZRILEY:
                    switch (net_type)
                    {
                        case NET_TYPE_LOWPASS:
                            nums.push_back(0.3000);
                            nums.push_back(0.2533);
                            nums.push_back(0.1500);
                            nums.push_back(0.0563);
                            break;
                        case NET_TYPE_HIGHPASS:
                            nums.push_back(0.0844);
                            nums.push_back(0.1000);
                            nums.push_back(0.1688);
                            nums.push_back(0.4501);
                            break;
                    }
                    break;
            }
            break;
    }

    if (nums.empty())
    {
        throw std::runtime_error("Filter parameters was not correctly populated");
    }
    return nums;
}

void FilterLinkFrame::set_family(Gtk::ComboBoxText* option_menu, int order, int family)
{
#ifndef NDEBUG
    std::cout << "FilterLinkFrame::set_family: order = " << order << ", family = " << family << "\n";
#endif
    switch (order)
    {
        case NET_ORDER_2ND:
            switch (family)
            {
                case NET_BESSEL:
                    option_menu->set_active(0);
                    break;
                case NET_BUTTERWORTH:
                    option_menu->set_active(1);
                    break;
                case NET_CHEBYCHEV:
                    option_menu->set_active(2);
                    break;
                case NET_LINKWITZRILEY:
                    option_menu->set_active(3);
                    break;
            }
            break;
        case NET_ORDER_3RD:
            switch (family)
            {
                case NET_BUTTERWORTH:
                    option_menu->set_active(1);
                    break;
                case NET_BESSEL:
                    option_menu->set_active(0);
                    break;
            }
            break;
        case NET_ORDER_4TH:
            switch (family)
            {
                case NET_BESSEL:
                    option_menu->set_active(0);
                    break;
                case NET_BUTTERWORTH:
                    option_menu->set_active(1);
                    break;
                case NET_GAUSSIAN:
                    option_menu->set_active(2);
                    break;
                case NET_LEGENDRE:
                    option_menu->set_active(3);
                    break;
                case NET_LINEARPHASE:
                    option_menu->set_active(4);
                    break;
                case NET_LINKWITZRILEY:
                    option_menu->set_active(5);
                    break;
            }
            break;
    }
}
