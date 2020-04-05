/*
  $Id$

  frequency_response_editor Copyright (C) 2002 Daniel Sundberg

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

#include "frequency_response_editor.hpp"

#include <gtkmm/filechooserdialog.h>
#include <gtkmm/frame.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/stock.h>

#include <cstdio>
#include <fstream>
#include <utility>

frequency_response_editor::frequency_response_editor(std::string filename)
    : m_table(15, 4, false),
      dbmag_entries(30),
      m_save_button(Gtk::Stock::SAVE),
      m_saveas_button(Gtk::Stock::SAVE_AS),
      m_close_button(Gtk::Stock::CLOSE),
      m_filename(std::move(filename))
{
    set_modal();

    m_label.set_markup("<b>" + Glib::ustring(_("Frequency response for selected driver")) + "</b>");

    auto frame = Gtk::make_managed<Gtk::Frame>();
    auto frame_vbox = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL);

    m_save_button.set_border_width(10);
    m_saveas_button.set_border_width(10);
    m_close_button.set_border_width(10);

    frame->set_border_width(10);
    frame->set_shadow_type(Gtk::SHADOW_NONE);
    frame->set_label_widget(m_label);

    get_vbox()->pack_start(*frame);
    m_table.set_spacings(5);

    frame->add(*frame_vbox);
    frame_vbox->set_border_width(10);
    frame_vbox->pack_start(*Gtk::make_managed<Gtk::Label>(
        Glib::ustring(_("Enter the freq response dB magnitude, this is not intended to provide an"))
        + "\n" + Glib::ustring(_("exact estimation of the total frequency response."))));
    frame_vbox->set_spacing(12);
    frame_vbox->pack_start(m_table);

    std::generate(begin(dbmag_entries), end(dbmag_entries), []() {
        return Gtk::make_managed<Gtk::Entry>();
    });

    for (int j = 0; j < 15; j++)
    {
        m_table.attach(*dbmag_entries[2 * j], 1, 2, j, j + 1);
        m_table.attach(*dbmag_entries[2 * j + 1], 3, 4, j, j + 1);
    }

    m_table.attach(*Gtk::manage(new Gtk::Label("20.0 Hz: ", Gtk::ALIGN_END)), 0, 1, 0, 1);
    m_table.attach(*Gtk::manage(new Gtk::Label("25.2 Hz: ", Gtk::ALIGN_END)), 2, 3, 0, 1);
    m_table.attach(*Gtk::manage(new Gtk::Label("31.7 Hz: ", Gtk::ALIGN_END)), 0, 1, 1, 2);
    m_table.attach(*Gtk::manage(new Gtk::Label("39.9 Hz: ", Gtk::ALIGN_END)), 2, 3, 1, 2);
    m_table.attach(*Gtk::manage(new Gtk::Label("50.2 Hz: ", Gtk::ALIGN_END)), 0, 1, 2, 3);
    m_table.attach(*Gtk::manage(new Gtk::Label("63.2 Hz: ", Gtk::ALIGN_END)), 2, 3, 2, 3);
    m_table.attach(*Gtk::manage(new Gtk::Label("79.6 Hz: ", Gtk::ALIGN_END)), 0, 1, 3, 4);
    m_table.attach(*Gtk::manage(new Gtk::Label("100 Hz: ", Gtk::ALIGN_END)), 2, 3, 3, 4);
    m_table.attach(*Gtk::manage(new Gtk::Label("126 Hz: ", Gtk::ALIGN_END)), 0, 1, 4, 5);
    m_table.attach(*Gtk::manage(new Gtk::Label("159 Hz: ", Gtk::ALIGN_END)), 2, 3, 4, 5);
    m_table.attach(*Gtk::manage(new Gtk::Label("200 Hz: ", Gtk::ALIGN_END)), 0, 1, 5, 6);
    m_table.attach(*Gtk::manage(new Gtk::Label("252 Hz: ", Gtk::ALIGN_END)), 2, 3, 5, 6);
    m_table.attach(*Gtk::manage(new Gtk::Label("317 Hz: ", Gtk::ALIGN_END)), 0, 1, 6, 7);
    m_table.attach(*Gtk::manage(new Gtk::Label("399 Hz: ", Gtk::ALIGN_END)), 2, 3, 6, 7);
    m_table.attach(*Gtk::manage(new Gtk::Label("502 Hz: ", Gtk::ALIGN_END)), 0, 1, 7, 8);
    m_table.attach(*Gtk::manage(new Gtk::Label("632 Hz: ", Gtk::ALIGN_END)), 2, 3, 7, 8);
    m_table.attach(*Gtk::manage(new Gtk::Label("796 Hz: ", Gtk::ALIGN_END)), 0, 1, 8, 9);
    m_table.attach(*Gtk::manage(new Gtk::Label("1.00 kHz: ", Gtk::ALIGN_END)), 2, 3, 8, 9);
    m_table.attach(*Gtk::manage(new Gtk::Label("1.26 kHz: ", Gtk::ALIGN_END)), 0, 1, 9, 10);
    m_table.attach(*Gtk::manage(new Gtk::Label("1.59 kHz: ", Gtk::ALIGN_END)), 2, 3, 9, 10);
    m_table.attach(*Gtk::manage(new Gtk::Label("2.00 kHz: ", Gtk::ALIGN_END)), 0, 1, 10, 11);
    m_table.attach(*Gtk::manage(new Gtk::Label("2.52 kHz: ", Gtk::ALIGN_END)), 2, 3, 10, 11);
    m_table.attach(*Gtk::manage(new Gtk::Label("3.17 kHz: ", Gtk::ALIGN_END)), 0, 1, 11, 12);
    m_table.attach(*Gtk::manage(new Gtk::Label("3.99 kHz: ", Gtk::ALIGN_END)), 2, 3, 11, 12);
    m_table.attach(*Gtk::manage(new Gtk::Label("5.02 kHz: ", Gtk::ALIGN_END)), 0, 1, 12, 13);
    m_table.attach(*Gtk::manage(new Gtk::Label("6.32 kHz: ", Gtk::ALIGN_END)), 2, 3, 12, 13);
    m_table.attach(*Gtk::manage(new Gtk::Label("7.96 kHz: ", Gtk::ALIGN_END)), 0, 1, 13, 14);
    m_table.attach(*Gtk::manage(new Gtk::Label("10.0 kHz: ", Gtk::ALIGN_END)), 2, 3, 13, 14);
    m_table.attach(*Gtk::manage(new Gtk::Label("15.9 kHz: ", Gtk::ALIGN_END)), 0, 1, 14, 15);
    m_table.attach(*Gtk::manage(new Gtk::Label("20.0 kHz: ", Gtk::ALIGN_END)), 2, 3, 14, 15);

    get_action_area()->pack_start(m_save_button);
    get_action_area()->pack_start(m_saveas_button);
    get_action_area()->pack_start(m_close_button);

    m_saveas_button.signal_clicked().connect(
        sigc::mem_fun(*this, &frequency_response_editor::on_save_as));
    m_save_button.signal_clicked().connect(sigc::mem_fun(*this, &frequency_response_editor::on_save));
    m_close_button.signal_clicked().connect(
        sigc::mem_fun(*this, &frequency_response_editor::on_close));
    m_save_button.set_sensitive(false);

    if (!m_filename.empty())
    {
        // Open file and fill in the values
        std::ifstream file_in(m_filename.c_str());

        if (file_in.good())
        {
            for (int i = 0; i < 30; i++)
            {
                std::vector<char> buffer(100);
                file_in.getline(buffer.data(), 100, '\n');

                char* substr_ptr = strtok(buffer.data(), ",");
                g_ascii_strtod(substr_ptr, nullptr);
                substr_ptr = strtok(nullptr, ",");
                auto const f2 = g_ascii_strtod(substr_ptr, nullptr);

                dbmag_entries[i]->set_text(GSpeakers::double_to_ustring(f2, 2, 1));
                dbmag_entries[i]->signal_changed().connect(
                    sigc::bind(sigc::mem_fun(m_save_button, &Gtk::Button::set_sensitive), true));
            }
        }
    }
    show_all();
}

void frequency_response_editor::on_save()
{
#ifndef NDEBUG
    std::puts("frequency_response_editor::on_save");
#endif
    std::vector<double> const& v = get_x_vector();

    std::ofstream of(m_filename.c_str());
    if (of.good())
    {
        std::array<char, 8> buffer;
        for (int j = 0; j < 15; j++)
        {
            of << v[2 * j] << ","
               << g_ascii_dtostr(buffer.data(),
                                 8,
                                 g_ascii_strtod(dbmag_entries[2 * j]->get_text().c_str(), nullptr))
               << "\n";
            of << v[2 * j + 1] << ","
               << g_ascii_dtostr(buffer.data(),
                                 8,
                                 g_ascii_strtod(dbmag_entries[2 * j + 1]->get_text().c_str(), nullptr))
               << "\n";
        }
        of.close();
        for (int i = 0; i < 30; i++)
        {
            dbmag_entries[i]->signal_changed().connect(
                sigc::bind(sigc::mem_fun(m_save_button, &Gtk::Button::set_sensitive), true));
        }
    }
    else
    {
        Gtk::MessageDialog(*this,
                           std::string(_("Could not open file: ")) + m_filename,
                           false,
                           Gtk::MESSAGE_ERROR,
                           Gtk::BUTTONS_OK,
                           true)
            .run();
    }
    m_save_button.set_sensitive(false);
}

void frequency_response_editor::on_save_as()
{
    std::puts("frequency_response_editor::on_save_as");

    Gtk::FileChooserDialog dialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);

    // Add response buttons the the dialog:
    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Open", Gtk::RESPONSE_OK);

    // Show the dialog and wait for a user response
    switch (dialog.run())
    {
        case Gtk::RESPONSE_OK:
        {
            m_filename = dialog.get_filename();
            on_save();
            break;
        }
    }
}

void frequency_response_editor::on_close()
{
#ifndef NDEBUG
    std::puts("frequency_response_editor::on_close");
#endif
    response(0);
    hide();
}

auto frequency_response_editor::get_x_vector() -> std::vector<double>
{
    return {20.0, 25.2, 31.7, 39.9, 50.2, 63.2, 79.6, 100,   126,   159,
            200,  252,  317,  399,  502,  632,  796,  1000,  1260,  1590,
            2000, 2520, 3170, 3990, 5020, 6320, 7960, 10000, 15900, 20000};
}
