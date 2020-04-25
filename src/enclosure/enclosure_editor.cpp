/*
  $Id$

  boxeditor Copyright (C) 2002 Daniel Sundberg

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

#include "enclosure_editor.hpp"

#include "enclosure_model.hpp"
#include "common.h"
#include "plot.hpp"

#include <cmath>
#include <iostream>

constexpr auto SEALED_SELECTED = 0;
constexpr auto PORTED_SELECTED = 1;

constexpr auto ID_STRING_ENTRY_CHANGED = 2;

constexpr auto VB1_ENTRY_CHANGED = 3;
constexpr auto FB1_ENTRY_CHANGED = 4;
constexpr auto VB2_ENTRY_CHANGED = 5;
constexpr auto FB2_ENTRY_CHANGED = 6;

enclosure_editor::enclosure_editor()
    : Gtk::Frame(_("Enclosure editor")),
      m_vbox(Gtk::ORIENTATION_VERTICAL),
      m_hbox(Gtk::ORIENTATION_HORIZONTAL),
      m_speaker_vas_label("", Gtk::ALIGN_START),
      m_speaker_fs_label("", Gtk::ALIGN_START),
      m_efficiency_bandwidth_product_label("", Gtk::ALIGN_START)
{
    m_vbox.pack_start(m_grid);

    set_border_width(2);
    add(m_vbox);
    m_vbox.set_border_width(12);
    m_hbox.set_spacing(5);
    set_shadow_type(Gtk::SHADOW_NONE);
    m_frame_label.set_markup("<b>" + Glib::ustring(_("Enclosure editor")) + "</b>");
    set_label_widget(m_frame_label);

    m_vb1_entry.set_width_chars(10);
    m_fb1_entry.set_width_chars(10);
    m_id_string_entry.set_width_chars(10);

    m_box_damping_combo.append("Perfect transient (0.5)");
    m_box_damping_combo.append("Bessel (D2 - 0.577)");
    m_box_damping_combo.append("Butterworth (B2 - 0.707)");
    m_box_damping_combo.append("Chebychev (C2 - 0.8)");
    m_box_damping_combo.append("Chebychev (C2 - 0.9)");
    m_box_damping_combo.append("Chebychev (C2 - 1.0)");
    m_box_damping_combo.append("Chebychev (C2 - 1.1)");
    m_box_damping_combo.append("Chebychev (C2 - 1.2)");

    m_box_damping_combo.set_active(2);
    m_system_damping = 1.0 / std::sqrt(2.0);

    m_box_type_combo.append("Sealed");
    m_box_type_combo.append("Ported");

    this->arrange_grid();

    m_vb1_entry.set_tooltip_text(_("Enclosure volume"));

    this->connect_signals();

    show_all();
}

void enclosure_editor::arrange_grid()
{
    m_grid.set_row_spacing(10);
    m_grid.set_column_spacing(10);

    // Indices for grid are given as x, y

    // Left column

    m_grid.attach(*Gtk::manage(new Gtk::Label(_("Name:"), Gtk::ALIGN_START)), 0, 0);
    m_grid.attach(m_id_string_entry, 1, 0);

    m_grid.attach(*Gtk::manage(new Gtk::Label(_("Woofer:"), Gtk::ALIGN_START)), 0, 1);
    m_grid.attach(m_bass_speaker_combo, 1, 1);

    m_grid.attach(*Gtk::manage(new Gtk::Label(_("Type:"), Gtk::ALIGN_START)), 0, 2);
    m_grid.attach(m_box_type_combo, 1, 2);

    m_grid.attach(*Gtk::manage(new Gtk::Label(_("Alignment:"), Gtk::ALIGN_START)), 0, 3);
    m_grid.attach(m_box_damping_combo, 1, 3);

    // Right column

    m_grid.attach(*Gtk::manage(new Gtk::Label(_("EBP:"), Gtk::ALIGN_END)), 2, 1);
    m_grid.attach(m_efficiency_bandwidth_product_label, 3, 1);

    m_grid.attach(*Gtk::manage(new Gtk::Label(_("Resonance\nfrequency:"), Gtk::ALIGN_END)),
                  2,
                  2);
    m_grid.attach(m_fb1_entry, 3, 2);
    m_grid.attach(*Gtk::manage(new Gtk::Label(_("Hz"), Gtk::ALIGN_START)), 4, 2);

    m_grid.attach(*Gtk::manage(new Gtk::Label(_("Volume:"), Gtk::ALIGN_END)), 2, 3);
    m_grid.attach(m_vb1_entry, 3, 3);
    m_grid.attach(*Gtk::manage(new Gtk::Label(_("L"), Gtk::ALIGN_START)), 4, 3);
}

void enclosure_editor::connect_signals()
{
    m_bass_speaker_combo.signal_changed().connect(
        sigc::mem_fun(*this, &enclosure_editor::on_combo_entry_changed));

    m_box_damping_combo.signal_changed().connect(
        sigc::mem_fun(*this, &enclosure_editor::on_alignment_changed));

    m_id_string_entry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &enclosure_editor::on_box_data_changed),
                   ID_STRING_ENTRY_CHANGED));
    m_vb1_entry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &enclosure_editor::on_box_data_changed),
                   VB1_ENTRY_CHANGED));
    m_fb1_entry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &enclosure_editor::on_box_data_changed),
                   FB1_ENTRY_CHANGED));

    signal_speakerlist_loaded.connect(
        sigc::mem_fun(*this, &enclosure_editor::on_speaker_list_loaded));

    // Setup option menu
    m_box_type_combo.signal_changed().connect(
        sigc::mem_fun(*this, &enclosure_editor::on_enclosure_changed));

    signal_box_selected.connect(sigc::mem_fun(*this, &enclosure_editor::on_box_selected));

    // On enter presses in vb entry we should move focus to fb entry
    m_vb1_entry.signal_activate().connect(
        sigc::mem_fun(*this, &enclosure_editor::on_vb1_entry_activated));
    m_fb1_entry.signal_activate().connect(
        sigc::mem_fun(*this, &enclosure_editor::on_append_to_boxlist_clicked));
}

void enclosure_editor::on_vb1_entry_activated()
{
    switch (m_box->get_type())
    {
        case BOX_TYPE_SEALED:
            on_append_to_boxlist_clicked();
            break;
        case BOX_TYPE_PORTED:
            m_fb1_entry.grab_focus();
            break;
    }
}

void enclosure_editor::append_and_plot()
{
    on_append_to_boxlist_clicked();
    on_append_to_plot_clicked();
    m_vb1_entry.grab_focus();
}

void enclosure_editor::on_optimize_button_clicked()
{
    if (!m_disable_signals)
    {
        m_disable_signals = true;
        switch (m_box_type_combo.get_active_row_number() + 1)
        {
            case BOX_TYPE_SEALED:
            {
                using namespace gspk;

                m_box->set_fb1(sealed::resonance_frequency(m_system_damping,
                                                           m_current_speaker.get_fs(),
                                                           m_current_speaker.get_qts()));

                m_box->set_vb1(sealed::volume(m_system_damping,
                                              m_current_speaker.get_vas(),
                                              m_current_speaker.get_qts()));
                break;
            }
            case BOX_TYPE_PORTED:
                // Ported box
                // vb=20*vas*qts^3.3;
                // fb=fs*(vas/vb)^0.31;
                m_box->set_vb1(20.0 * m_current_speaker.get_vas()
                               * std::pow(m_current_speaker.get_qts(), 3.3));
                m_box->set_fb1(
                    m_current_speaker.get_fs()
                    * std::pow(m_current_speaker.get_vas() / m_box->get_vb1(), 0.31));
                break;
        }
        m_vb1_entry.set_text(gspk::double_to_ustring(m_box->get_vb1(), 2, 1));
        m_fb1_entry.set_text(gspk::double_to_ustring(m_box->get_fb1(), 2, 1));
    }
    signal_box_modified(m_box);
    m_disable_signals = false;
}

void enclosure_editor::on_append_to_plot_clicked()
{
    m_box = new enclosure();

    m_box->set_id_string(m_id_string_entry.get_text());
    m_box->set_vb1(std::atof(m_vb1_entry.get_text().c_str()));
    m_box->set_fb1(std::atof(m_fb1_entry.get_text().c_str()));
    m_box->set_type(m_box_type_combo.get_active_row_number() + 1);

    Gdk::Color color(m_color_list.get_color_string());

    // Calculate the frequency response graph for current enclosure and the current speaker
    std::vector<gspk::point> points;

    using namespace gspk;

    switch (m_box->get_type())
    {
        case BOX_TYPE_PORTED:
            for (int f = 10; f < 1000; f++)
            {
                auto const A = std::pow(m_box->get_fb1() / m_current_speaker.get_fs(), 2);
                auto const B = A / m_current_speaker.get_qts()
                               + m_box->get_fb1()
                                     / (7.0 * m_current_speaker.get_fs()
                                        * m_current_speaker.get_qts());
                auto const C = 1.0 + A + m_current_speaker.get_vas() / m_box->get_vb1()
                               + m_box->get_fb1()
                                     / (7.0 * m_current_speaker.get_fs()
                                        * m_current_speaker.get_qts());
                auto const D = 1.0 / m_current_speaker.get_qts()
                               + m_box->get_fb1() / (7.0 * m_current_speaker.get_fs());
                auto const fn2 = std::pow(f / m_current_speaker.get_fs(), 2);
                auto const fn4 = std::pow(fn2, 2);

                auto const y = (10
                                * std::log10(std::pow(fn4, 2)
                                             / (std::pow(fn4 - C * fn2 + A, 2)
                                                + fn2 * std::pow(D * fn2 - B, 2))));
                points.emplace_back(f, y);
            }
            break;
        case BOX_TYPE_SEALED:

            for (int f = 10; f < 1000; ++f)
            {
                points.emplace_back(f,
                                    sealed::frequency_response(m_system_damping,
                                                               f,
                                                               m_box->get_fb1()));
            }
            break;
    }
    signal_add_box_plot(points, color);
    signal_add_plot(m_box, &m_current_speaker, color);
}

void enclosure_editor::on_calc_port_clicked() {}

void enclosure_editor::on_append_to_boxlist_clicked() { signal_add_to_boxlist(m_box); }

void enclosure_editor::on_box_selected(enclosure* b)
{
    if (m_disable_signals)
    {
        return;
    }

    m_disable_signals = true;

    if (b != nullptr)
    {
        m_box = b;
        m_id_string_entry.set_text(b->get_id_string());
        m_vb1_entry.set_text(gspk::double_to_ustring(b->get_vb1(), 2, 1));
        m_fb1_entry.set_text(gspk::double_to_ustring(b->get_fb1(), 2, 1));

        // Set combo to proper speaker
        if (speaker_list_is_loaded)
        {
            m_current_speaker = m_speaker_list->get_by_id_string(b->get_speaker());

            // Remove all the previous entries and populate again
            m_bass_speaker_combo.remove_all();

            for (auto const& speaker : m_speaker_list->data())
            {
                if (is_bass_driver(speaker.get_type())
                    && m_current_speaker.get_id_string() != speaker.get_id_string())
                {
                    m_bass_speaker_combo.append(speaker.get_id_string());
                }
            }
            m_bass_speaker_combo.prepend(m_current_speaker.get_id_string());
            m_bass_speaker_combo.set_active(0);
        }
        // set state of option menu here
        // enclosure type is 1, 2, 3...therefor the '-1'
        m_box_type_combo.set_active(b->get_type() - 1);

        m_fb1_entry.set_sensitive(m_box->get_type() != BOX_TYPE_SEALED);

        if (m_box->get_type() == BOX_TYPE_SEALED)
        {
            m_box->set_fb1(gspk::sealed::resonance_frequency(m_system_damping,
                                                             m_current_speaker.get_fs(),
                                                             m_current_speaker.get_qts()));

            m_fb1_entry.set_text(gspk::double_to_ustring(m_box->get_fb1(), 2, 1));
        }
    }
    else
    {
        b = new Box();
    }
    m_disable_signals = false;
}

void enclosure_editor::on_speaker_list_loaded(driver_list* driver_list)
{
    if (m_disable_signals)
    {
        return;
    }
    m_disable_signals = true;
    m_speaker_list = driver_list;

    m_bass_speaker_combo.remove_all();

    // If we have got a selected box, insert the items with the driver
    // belonging to the current speaker at the top position, if we haven't
    // got a selected box: insert all drivers and don't care about the order
    if (m_box != nullptr)
    {
        for (auto const& speaker : m_speaker_list->data())
        {
            if (is_bass_driver(speaker.get_type()) && m_box->get_speaker() != speaker.get_id_string())
            {
                m_bass_speaker_combo.append(speaker.get_id_string());
            }
        }
        m_bass_speaker_combo.prepend(m_box->get_speaker());
    }
    else
    {
        for (auto const& speaker : m_speaker_list->data())
        {
            if (is_bass_driver(speaker.get_type()))
            {
                m_bass_speaker_combo.append(speaker.get_id_string());
            }
        }
    }

    m_bass_speaker_combo.set_active(m_speaker_list->data().empty() ? -1 : 0);

    speaker_list_is_loaded = true;
    m_disable_signals = false;
}

void enclosure_editor::on_combo_entry_changed()
{
    std::cout << "enclosure_editor: combo entry changed: "
              << m_bass_speaker_combo.get_active_text() << "\n";

    if (m_disable_signals)
    {
        return;
    }

    m_disable_signals = true;

    // Search for the new entry string in the driver_list
    m_current_speaker = m_speaker_list->get_by_id_string(m_bass_speaker_combo.get_active_text());

    // maybe set_markup here?
    m_speaker_vas_label.set_text(gspk::double_to_ustring(m_current_speaker.get_vas(), 2, 1));
    m_speaker_fs_label.set_text(gspk::double_to_ustring(m_current_speaker.get_fs(), 2, 1));

    {
        auto const ebp = m_current_speaker.get_fs() / m_current_speaker.get_qes();
        m_efficiency_bandwidth_product_label.set_text(gspk::double_to_ustring(ebp, 2, 1));

        // Select the best enclosure based on the efficiency bandwidth product
        if (ebp <= 50.0)
        {
            m_box_type_combo.set_active(BOX_TYPE_SEALED - 1);
        }
        else if (ebp >= 100.0)
        {
            m_box_type_combo.set_active(BOX_TYPE_PORTED - 1);
        }
    }

    m_box->set_speaker(m_bass_speaker_combo.get_active_text());

    signal_box_modified(m_box);

    m_disable_signals = false;
}

void enclosure_editor::on_alignment_changed()
{
    std::cout << "enclosure_editor: combo entry changed: "
              << m_box_damping_combo.get_active_text() << '\n';

    if (m_disable_signals)
    {
        return;
    }

    m_disable_signals = true;

    auto const alignment_index = m_box_damping_combo.get_active_row_number();

    switch (alignment_index)
    {
        case static_cast<int>(gspk::sealed::RESPONSE::PERFECT_TRANSIENT):
            m_system_damping = 0.5;
            break;
        case static_cast<int>(gspk::sealed::RESPONSE::BESSEL):
            m_system_damping = std::sqrt(1.0 / 3.0);
            break;
        case static_cast<int>(gspk::sealed::RESPONSE::BUTTERWORTH):
            m_system_damping = std::sqrt(1.0 / 2.0);
            break;
        case static_cast<int>(gspk::sealed::RESPONSE::CHEBYCHEV_08):
            m_system_damping = 0.8;
            break;
        case static_cast<int>(gspk::sealed::RESPONSE::CHEBYCHEV_09):
            m_system_damping = 0.9;
            break;
        case static_cast<int>(gspk::sealed::RESPONSE::CHEBYCHEV_10):
            m_system_damping = 1.0;
            break;
        case static_cast<int>(gspk::sealed::RESPONSE::CHEBYCHEV_11):
            m_system_damping = 1.1;
            break;
        case static_cast<int>(gspk::sealed::RESPONSE::CHEBYCHEV_12):
            m_system_damping = 1.2;
            break;
    }
    m_disable_signals = false;

    this->on_optimize_button_clicked();
}

void enclosure_editor::on_enclosure_changed()
{
    if (m_disable_signals)
    {
        return;
    }
    m_disable_signals = true;

    switch (m_box_type_combo.get_active_row_number())
    {
        case SEALED_SELECTED:
        {
            using namespace gspk;

            m_box->set_type(BOX_TYPE_SEALED);
            m_fb1_entry.set_sensitive(false);

            m_box->set_fb1(sealed::resonance_frequency(m_system_damping,
                                                       m_current_speaker.get_fs(),
                                                       m_current_speaker.get_qts()));

            m_fb1_entry.set_text(gspk::double_to_ustring(m_box->get_fb1(), 2, 1));
            break;
        }
        case PORTED_SELECTED:
            m_box->set_type(BOX_TYPE_PORTED);
            m_fb1_entry.set_sensitive(true);
            break;
    }
    signal_box_selected(m_box);
    signal_box_modified(m_box);

    m_disable_signals = false;
}

void enclosure_editor::on_box_data_changed(int i)
{
    if (m_disable_signals)
    {
        return;
    }

    m_disable_signals = true;

    switch (i)
    {
        case ID_STRING_ENTRY_CHANGED:
            m_box->set_id_string(m_id_string_entry.get_text());
            break;
        case VB1_ENTRY_CHANGED:
            m_box->set_vb1(std::atof(m_vb1_entry.get_text().c_str()));
            break;
        case FB1_ENTRY_CHANGED:
            m_box->set_fb1(std::atof(m_fb1_entry.get_text().c_str()));
            break;
        case VB2_ENTRY_CHANGED:
            m_box->set_vb2(std::atof(m_vb2_entry.get_text().c_str()));
            break;
        case FB2_ENTRY_CHANGED:
            m_box->set_fb2(std::atof(m_fb2_entry.get_text().c_str()));
            break;
    }
    signal_box_selected(m_box);
    signal_box_modified(m_box);

    m_disable_signals = false;
}
