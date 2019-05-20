/*
  $Id$

  freqrespeditor Copyright (C) 2002 Daniel Sundberg

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

#include "freqrespeditor.h"
#include <fstream>
#include <gtkmm/fileselection.h>
#include <gtkmm/frame.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/stock.h>
#include <stdio.h>

using namespace sigc;

FreqRespEditor::FreqRespEditor(string filename)
    : m_table(15, 4, false), m_save_button(Gtk::Stock::SAVE), m_saveas_button(Gtk::Stock::SAVE_AS),
      m_close_button(Gtk::Stock::CLOSE) {
  m_filename = filename;
  set_modal();

  Gtk::Frame* frame = manage(new Gtk::Frame(""));
  Gtk::VBox* frame_vbox = manage(new Gtk::VBox());
  frame->set_border_width(5);
  frame->set_shadow_type(Gtk::SHADOW_NONE);
  static_cast<Gtk::Label*>(frame->get_label_widget())
      ->set_markup("<b>" + Glib::ustring(_("Frequency response for selected driver")) + "</b>");

  // get_vbox()->pack_start(*manage(new Gtk::Label(_("Enter the freq response dB magnitude, this is
  // not intended to provide an "
  //                                                "exact estimation of the total frequency
  //                                                response."))));
  // get_vbox()->set_border_width(10);
  get_vbox()->pack_start(*frame);
  m_table.set_spacings(5);
  frame->add(*frame_vbox);
  frame_vbox->set_border_width(12);
  frame_vbox->pack_start(*manage(new Gtk::Label(
      Glib::ustring(_("Enter the freq response dB magnitude, this is not intended to provide an")) +
      "\n" + Glib::ustring(_("exact estimation of the total frequency response.")))));
  frame_vbox->set_spacing(12);
  frame_vbox->pack_start(m_table);
  // m_table.set_border_width(12);

  for (int i = 0; i < 30; i++) {
    dbmag_entries.push_back(manage(new Gtk::Entry()));
  }

  for (int j = 0; j < 15; j++) {
    m_table.attach(*dbmag_entries[2 * j], 1, 2, j, j + 1);
    m_table.attach(*dbmag_entries[2 * j + 1], 3, 4, j, j + 1);
  }

  m_table.attach(*manage(new Gtk::Label("20.0 Hz: ", Gtk::ALIGN_RIGHT)), 0, 1, 0, 1);
  m_table.attach(*manage(new Gtk::Label("25.2 Hz: ", Gtk::ALIGN_RIGHT)), 2, 3, 0, 1);
  m_table.attach(*manage(new Gtk::Label("31.7 Hz: ", Gtk::ALIGN_RIGHT)), 0, 1, 1, 2);
  m_table.attach(*manage(new Gtk::Label("39.9 Hz: ", Gtk::ALIGN_RIGHT)), 2, 3, 1, 2);
  m_table.attach(*manage(new Gtk::Label("50.2 Hz: ", Gtk::ALIGN_RIGHT)), 0, 1, 2, 3);
  m_table.attach(*manage(new Gtk::Label("63.2 Hz: ", Gtk::ALIGN_RIGHT)), 2, 3, 2, 3);
  m_table.attach(*manage(new Gtk::Label("79.6 Hz: ", Gtk::ALIGN_RIGHT)), 0, 1, 3, 4);
  m_table.attach(*manage(new Gtk::Label("100 Hz: ", Gtk::ALIGN_RIGHT)), 2, 3, 3, 4);
  m_table.attach(*manage(new Gtk::Label("126 Hz: ", Gtk::ALIGN_RIGHT)), 0, 1, 4, 5);
  m_table.attach(*manage(new Gtk::Label("159 Hz: ", Gtk::ALIGN_RIGHT)), 2, 3, 4, 5);
  m_table.attach(*manage(new Gtk::Label("200 Hz: ", Gtk::ALIGN_RIGHT)), 0, 1, 5, 6);
  m_table.attach(*manage(new Gtk::Label("252 Hz: ", Gtk::ALIGN_RIGHT)), 2, 3, 5, 6);
  m_table.attach(*manage(new Gtk::Label("317 Hz: ", Gtk::ALIGN_RIGHT)), 0, 1, 6, 7);
  m_table.attach(*manage(new Gtk::Label("399 Hz: ", Gtk::ALIGN_RIGHT)), 2, 3, 6, 7);
  m_table.attach(*manage(new Gtk::Label("502 Hz: ", Gtk::ALIGN_RIGHT)), 0, 1, 7, 8);
  m_table.attach(*manage(new Gtk::Label("632 Hz: ", Gtk::ALIGN_RIGHT)), 2, 3, 7, 8);
  m_table.attach(*manage(new Gtk::Label("796 Hz: ", Gtk::ALIGN_RIGHT)), 0, 1, 8, 9);
  m_table.attach(*manage(new Gtk::Label("1.00 kHz: ", Gtk::ALIGN_RIGHT)), 2, 3, 8, 9);
  m_table.attach(*manage(new Gtk::Label("1.26 kHz: ", Gtk::ALIGN_RIGHT)), 0, 1, 9, 10);
  m_table.attach(*manage(new Gtk::Label("1.59 kHz: ", Gtk::ALIGN_RIGHT)), 2, 3, 9, 10);
  m_table.attach(*manage(new Gtk::Label("2.00 kHz: ", Gtk::ALIGN_RIGHT)), 0, 1, 10, 11);
  m_table.attach(*manage(new Gtk::Label("2.52 kHz: ", Gtk::ALIGN_RIGHT)), 2, 3, 10, 11);
  m_table.attach(*manage(new Gtk::Label("3.17 kHz: ", Gtk::ALIGN_RIGHT)), 0, 1, 11, 12);
  m_table.attach(*manage(new Gtk::Label("3.99 kHz: ", Gtk::ALIGN_RIGHT)), 2, 3, 11, 12);
  m_table.attach(*manage(new Gtk::Label("5.02 kHz: ", Gtk::ALIGN_RIGHT)), 0, 1, 12, 13);
  m_table.attach(*manage(new Gtk::Label("6.32 kHz: ", Gtk::ALIGN_RIGHT)), 2, 3, 12, 13);
  m_table.attach(*manage(new Gtk::Label("7.96 kHz: ", Gtk::ALIGN_RIGHT)), 0, 1, 13, 14);
  m_table.attach(*manage(new Gtk::Label("10.0 kHz: ", Gtk::ALIGN_RIGHT)), 2, 3, 13, 14);
  m_table.attach(*manage(new Gtk::Label("15.9 kHz: ", Gtk::ALIGN_RIGHT)), 0, 1, 14, 15);
  m_table.attach(*manage(new Gtk::Label("20.0 kHz: ", Gtk::ALIGN_RIGHT)), 2, 3, 14, 15);

  get_action_area()->pack_start(m_save_button);
  get_action_area()->pack_start(m_saveas_button);
  get_action_area()->pack_start(m_close_button);

  m_saveas_button.signal_clicked().connect(mem_fun(*this, &FreqRespEditor::on_save_as));
  m_save_button.signal_clicked().connect(mem_fun(*this, &FreqRespEditor::on_save));
  m_close_button.signal_clicked().connect(mem_fun(*this, &FreqRespEditor::on_close));
  m_save_button.set_sensitive(false);

  if (m_filename != "") {
    /* Open file and fill in the values */
    ifstream fin(m_filename.c_str());
    if (fin.good()) {
      for (int i = 0; i < 30; i++) {
        char* buffer = new char[100];
        fin.getline(buffer, 100, '\n');

        float f1, f2;
        // sscanf(buffer, "%f,%f", &f1, &f2);

        char* substr_ptr = strtok(buffer, ",");
        f1 = g_ascii_strtod(substr_ptr, NULL);
        substr_ptr = strtok(NULL, ",");
        f2 = g_ascii_strtod(substr_ptr, NULL);

        dbmag_entries[i]->set_text(GSpeakers::double_to_ustring((double)f2, 2, 1));
        dbmag_entries[i]->signal_changed().connect(
            bind<bool>(mem_fun(m_save_button, &Gtk::Button::set_sensitive), true));

        delete buffer;
      }
    }
  }

  show_all();
}

void FreqRespEditor::on_save() {
#ifdef OUTPUT_DEBUG
  cout << "FreqRespEditor::on_save" << endl;
#endif
  vector<double> v = get_x_vector();
  ofstream of(m_filename.c_str());
  if (of.good()) {
    gchar* buffer = new char[8];
    for (int j = 0; j < 15; j++) {
      of << v[2 * j] << ","
         << g_ascii_dtostr(buffer, 8,
                           g_ascii_strtod(dbmag_entries[2 * j]->get_text().c_str(), NULL))
         << endl;
      of << v[2 * j + 1] << ","
         << g_ascii_dtostr(buffer, 8,
                           g_ascii_strtod(dbmag_entries[2 * j + 1]->get_text().c_str(), NULL))
         << endl;
    }
    delete buffer;
    of.close();
    for (int i = 0; i < 30; i++) {
      dbmag_entries[i]->signal_changed().connect(
          bind<bool>(mem_fun(m_save_button, &Gtk::Button::set_sensitive), true));
    }

  } else {
    Gtk::MessageDialog* d =
        new Gtk::MessageDialog(*this, string(_("Could not open file: ")) + m_filename, false,
                               Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
    d->run();
    delete d;
  }
  m_save_button.set_sensitive(false);
}

void FreqRespEditor::on_save_as() {
#ifdef OUTPUT_DEBUG
  cout << "FreqRespEditor::on_save_as" << endl;
#endif
  Gtk::FileSelection* f = new Gtk::FileSelection(_("Enter filename..."));
  f->set_modal();
  /* -5 == ok button clicked */
  if (f->run() == -5) {
    m_filename = f->get_filename();
    on_save();
  }
  f->hide();
  delete f;
}

void FreqRespEditor::on_close() {
#ifdef OUTPUT_DEBUG
  cout << "FreqRespEditor::on_close" << endl;
#endif
  response(0);
  hide();
}

FreqRespEditor::~FreqRespEditor() {
#ifdef OUTPUT_DEBUG
  cout << "FreqRespEditor::~FreqRespEditor" << endl;
#endif
}

vector<double> FreqRespEditor::get_x_vector() {
  vector<double> v;
  v.push_back(20.0);
  v.push_back(25.2);
  v.push_back(31.7);
  v.push_back(39.9);
  v.push_back(50.2);
  v.push_back(63.2);
  v.push_back(79.6);
  v.push_back(100);
  v.push_back(126);
  v.push_back(159);
  v.push_back(200);
  v.push_back(252);
  v.push_back(317);
  v.push_back(399);
  v.push_back(502);
  v.push_back(632);
  v.push_back(796);
  v.push_back(1000);
  v.push_back(1260);
  v.push_back(1590);
  v.push_back(2000);
  v.push_back(2520);
  v.push_back(3170);
  v.push_back(3990);
  v.push_back(5020);
  v.push_back(6320);
  v.push_back(7960);
  v.push_back(10000);
  v.push_back(15900);
  v.push_back(20000);
  return v;
}
