/*
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

#include <math.h>
#include "boxeditor.h"
#include "common.h"
#include "gspeakerscolor.h"
#include "gspeakersplot.h"

#define SEALED_SELECTED         0
#define PORTED_SELECTED         1
#define ID_STRING_ENTRY_CHANGED 2
#define VB1_ENTRY_CHANGED       3
#define FB1_ENTRY_CHANGED       4
#define VB2_ENTRY_CHANGED       5
#define FB2_ENTRY_CHANGED       6

BoxEditor::BoxEditor() :
  m_table(6, 5, true),
  m_vbox(),
  m_hbox(),
  m_bass_speaker_combo(),
  m_box_type_optionmenu(),
  m_optimize_button("Optimize"), 
  m_append_button("Append to list"), 
  m_plot_button("Plot"),
  m_calc_port_button("Calc port..."),
  m_option_menu()
{
  disable_signals = false;
  set_title("Boxeditor");
  m_vbox.pack_start(m_table);
  //m_vbox.pack_start(m_hbox);
  add(m_vbox);
  set_border_width(8);
  m_hbox.set_spacing(5);

  m_speaker_qts_entry.set_width_chars(10);
  m_speaker_vas_entry.set_width_chars(10);
  m_speaker_fs_entry.set_width_chars(5);
  m_speaker_qts_entry.set_editable(false);
  m_speaker_vas_entry.set_editable(false);
  m_speaker_fs_entry.set_editable(false);
  
  m_vb1_entry.set_width_chars(10);
  m_fb1_entry.set_width_chars(10);
  m_id_string_entry.set_width_chars(10);
  
  m_table.set_spacings(4);
  m_table.attach(*manage(new Gtk::Label("Bass speaker: ")), 0, 1, 0, 1);
  m_table.attach(m_bass_speaker_combo, 1, 5, 0, 1);
  
  m_table.attach(*manage(new Gtk::Label("Qts: ")), 0, 1, 1, 2);
  m_table.attach(m_speaker_qts_entry, 1, 2, 1, 2);   
  m_table.attach(*manage(new Gtk::Label("Vas: ")), 3, 4, 1, 2);
  m_table.attach(m_speaker_vas_entry, 4, 5, 1, 2);   
  
  m_table.attach(*manage(new Gtk::Label("Fs: ")), 0, 1, 2, 3);
  m_table.attach(m_speaker_fs_entry, 1, 2, 2, 3);      

  m_table.attach(*manage(new Gtk::Label("Id string: ")), 0, 1, 3, 4);
  m_table.attach(m_id_string_entry, 1, 3, 3, 4);
  m_table.attach(*manage(new Gtk::Label("  Type: ")), 3, 4, 3, 4);
  m_table.attach(m_box_type_optionmenu, 4, 5, 3, 4);
  
  m_table.attach(*manage(new Gtk::Label("Vb1: ")), 0, 1, 4, 5);
  m_table.attach(m_vb1_entry, 1, 2, 4, 5);
  m_table.attach(*manage(new Gtk::Label("  Fb1: ")), 2, 3, 4, 5);
  m_table.attach(m_fb1_entry, 3, 4, 4, 5);
  

  m_table.attach(m_optimize_button, 0, 1, 5, 6);
  m_table.attach(m_append_button, 1, 2, 5, 6);
  m_table.attach(m_plot_button, 2, 3, 5, 6);
  //m_table.attach(m_calc_port_button, 3, 4, 3, 4);

  m_bass_speaker_combo.get_entry()->set_editable(false);
  m_bass_speaker_combo.get_entry()->signal_changed().connect(slot(*this, &BoxEditor::on_combo_entry_changed));

  m_id_string_entry.signal_changed().connect(bind<int>(slot(*this, &BoxEditor::on_box_data_changed), ID_STRING_ENTRY_CHANGED));
  m_vb1_entry.signal_changed().connect(bind<int>(slot(*this, &BoxEditor::on_box_data_changed), VB1_ENTRY_CHANGED));
  m_fb1_entry.signal_changed().connect(bind<int>(slot(*this, &BoxEditor::on_box_data_changed), FB1_ENTRY_CHANGED));

  m_optimize_button.signal_clicked().connect(slot(*this, &BoxEditor::on_optimize_button_clicked));
  m_append_button.signal_clicked().connect(slot(*this, &BoxEditor::on_append_to_boxlist_clicked));
  m_plot_button.signal_clicked().connect(slot(*this, &BoxEditor::on_append_to_plot_clicked));

  //m_hbox.pack_start(m_optimize_button);
  //m_hbox.pack_start(m_append_button);
  //m_hbox.pack_start(m_plot_button);
  //m_hbox.pack_start(m_calc_port_button);
  
  signal_speakerlist_loaded.connect(slot(*this, &BoxEditor::on_speaker_list_loaded));
  
  /* Setup option menu */
  Gtk::Menu::MenuList& menulist = m_option_menu.items();
  menulist.push_back( Gtk::Menu_Helpers::MenuElem("Sealed", bind<int>(slot(*this, &BoxEditor::on_box_data_changed), SEALED_SELECTED) ) ); 
  menulist.push_back( Gtk::Menu_Helpers::MenuElem("Ported", bind<int>(slot(*this, &BoxEditor::on_box_data_changed), PORTED_SELECTED) ) );
  m_box_type_optionmenu.set_menu(m_option_menu);
  
  //m_box = new Box();
  signal_box_selected.connect(slot(*this, &BoxEditor::on_box_selected));
  //on_box_selected(m_box);
  
  show_all();
}

BoxEditor::~BoxEditor()
{

}

/*
 * This function executes when you have clicked the optimize button.
 * 
 */
void BoxEditor::on_optimize_button_clicked()
{
  /* Ported box */
  //  vb=20*vas*qts^3.3;
  //  fb=fs*(vas/vb)^0.31;

  /* Sealed box */
  //  qr=(1/qts)/(1/0.707-0.1);
  //  fb=qr*fs;
  //  vr=qr^2-1;
  //  vb=vas/vr;
  if (disable_signals == false) {
    disable_signals = true;
    
    double qr, vr;
    switch ( m_box_type_optionmenu.get_history() + 1 ) {
    case BOX_TYPE_PORTED:
      m_box->set_vb1( 20 * m_current_speaker.get_vas() * pow( m_current_speaker.get_qts(), 3.3 ) );
      m_box->set_fb1( m_current_speaker.get_fs() * pow( m_current_speaker.get_vas() / m_box->get_vb1() , 0.31) );
      break;
    case BOX_TYPE_SEALED:
      qr = ( 1 / m_current_speaker.get_qts() ) / ( 1 / 0.707 - 0.1 );
      m_box->set_fb1( qr * m_current_speaker.get_fs() );
      vr = pow( qr, 2 ) - 1;
      m_box->set_vb1( m_current_speaker.get_vas() / vr );
      break;
    }
    char *str = NULL;
    GString *buffer = g_string_new(str);
  
    g_string_printf(buffer, "%f", m_box->get_vb1());
    m_vb1_entry.set_text( Glib::ustring(buffer->str) );
    
    g_string_printf(buffer, "%f", m_box->get_fb1());
    m_fb1_entry.set_text( Glib::ustring(buffer->str) );
  }
  signal_box_modified(m_box);
  disable_signals = false;
}

/*
 * This function executes when you have clicked the plot button
 *
 */
void BoxEditor::on_append_to_plot_clicked()
{
  cout << "BoxEditor: add plot" << endl;
  m_box = new Box();
  m_box->set_id_string(m_id_string_entry.get_text());
  m_box->set_vb1(atof(m_vb1_entry.get_text().c_str()));
  m_box->set_fb1(atof(m_fb1_entry.get_text().c_str()));
  m_box->set_type(m_box_type_optionmenu.get_history() + 1);

  string str = m_color_list.get_color_string();
  cout << "color: " << str << endl;
  Gdk::Color color = Gdk::Color(str);

  /* Calculate the frequency response graph */
  vector<GSpeakers::Point> points;
  double A, B, C, D, fn2, fn4, fr, vr, qr, qb;
  
  /* Calculate the frequency response for current enclosure and the current speaker */
  GSpeakers::Point p;
  switch ( m_box->get_type() ) {
  case BOX_TYPE_PORTED:
    for (int f = 10; f < 1000; f++) {
      p.set_x(f);
      A = pow( ( m_box->get_fb1() / m_current_speaker.get_fs() ), 2 );
      B = A / m_current_speaker.get_qts() + m_box->get_fb1() / ( 7 * m_current_speaker.get_fs() * m_current_speaker.get_qts() );
      C = 1 + A + ( m_current_speaker.get_vas() / m_box->get_vb1() ) + m_box->get_fb1() / 
                  ( 7 * m_current_speaker.get_fs() * m_current_speaker.get_qts() );
      D = 1 / m_current_speaker.get_qts() + m_box->get_fb1() / ( 7 * m_current_speaker.get_fs() );
      fn2 = pow( ( f / m_current_speaker.get_fs() ), 2 );
      fn4 = pow( fn2, 2 );
      p.set_y(10 * log10( pow( fn4, 2 ) / ( pow( ( fn4 - C * fn2 + A ), 2 ) + 
                        fn2 * pow( ( D * fn2 - B), 2 ) ) ));
      points.push_back(p);
    }
    break;
  case BOX_TYPE_SEALED:
    for (int f = 10; f < 1000; f++) {
      p.set_x(f);
      fr = pow( ( f / m_box->get_fb1() ), 2 );
      vr = m_current_speaker.get_vas() / m_box->get_vb1();
      qr = sqrt( vr + 1 );
      qb = 1 / ( ( 1 / m_current_speaker.get_qts() ) / qr + 0.1 );
      p.set_y(10 * log10( pow( fr, 2 ) / ( pow( ( fr - 1 ), 2 ) + fr / ( pow( qb, 2 ) ) ) ));
      points.push_back(p);
    }
    break;
  }
  signal_add_box_plot(points, color);

  signal_add_plot(m_box, &m_current_speaker, color);
  
}

void BoxEditor::on_calc_port_clicked()
{

}


void BoxEditor::on_append_to_boxlist_clicked()
{
  cout << "BoxEditor: append to boxlist" << endl;
  signal_add_to_boxlist(m_box);
}




void BoxEditor::on_box_selected(Box *b)
{
  if (disable_signals == false) {
    disable_signals = true;
    cout << "Boxeditor: on box selected" << endl;
    if (b != NULL) {
      m_box = b;
      m_id_string_entry.set_text(b->get_id_string());
      
      char *str = NULL;
      GString *buffer = g_string_new(str);
      g_string_printf(buffer, "%f", b->get_vb1());
      m_vb1_entry.set_text(string(buffer->str));
      g_string_printf(buffer, "%f", b->get_fb1());
      m_fb1_entry.set_text(string(buffer->str));
      
      /* set state of option menu here */
      /* Box type is 1, 2, 3...therefor the '-1' */
      m_box_type_optionmenu.set_history(b->get_type() - 1);
      if (m_box->get_type() == BOX_TYPE_SEALED) {
        m_fb1_entry.set_sensitive(false);
        double qr = ( 1 / m_current_speaker.get_qts() ) / ( 1 / 0.707 - 0.1 );
        m_box->set_fb1( qr * m_current_speaker.get_fs() );
        buffer = g_string_new(str);
        g_string_printf(buffer, "%f", m_box->get_fb1());
        m_fb1_entry.set_text( Glib::ustring(buffer->str) );

      } else {
        m_fb1_entry.set_sensitive(true);
      }
      
    } else {
      cout << "Box ptr = NULL" << endl;
      b = new Box(); // Maybe we don't really need this one */
    }
    disable_signals = false;
  }
}

void BoxEditor::on_speaker_list_loaded(string speaker_list_filename)
{
  cout << "boxeditor: " << speaker_list_filename << " loaded" << endl;
  m_speaker_list = SpeakerList(speaker_list_filename);
  vector<Glib::ustring> popdown_strings;
  
  for (
    vector<Speaker>::iterator from = m_speaker_list.speaker_list()->begin();
    from != m_speaker_list.speaker_list()->end();
    ++from)
  {
    if ((*from).get_type() & SPEAKER_TYPE_BASS) {
      popdown_strings.push_back((*from).get_id_string());
    }
  }
  m_bass_speaker_combo.set_popdown_strings(popdown_strings);
}

void BoxEditor::on_combo_entry_changed() 
{
  cout << "BoxEditor: combo entry changed: " << m_bass_speaker_combo.get_entry()->get_text() << endl;
  /* Search for the new entry string in the SpeakerList */
  m_current_speaker = m_speaker_list.get_speaker_by_id_string(m_bass_speaker_combo.get_entry()->get_text());
  m_speaker_qts_entry.set_text(GSpeakers::double_to_ustring(m_current_speaker.get_qts()));
  m_speaker_vas_entry.set_text(GSpeakers::double_to_ustring(m_current_speaker.get_vas()));
  m_speaker_fs_entry.set_text(GSpeakers::double_to_ustring(m_current_speaker.get_fs()));
  //cout << m_current_speaker << endl;
}

void BoxEditor::on_box_data_changed(int i)
{
  if (disable_signals == false) {
    disable_signals = true;
    cout << "changed: ";
    double qr;
    char *str = NULL;
    GString *buffer;
  
    switch (i) {
      case SEALED_SELECTED:
        cout << "Sealed enclosure" << endl;
        m_box->set_type(BOX_TYPE_SEALED);
        m_fb1_entry.set_sensitive(false);
        buffer = g_string_new(str);
        qr = ( 1 / m_current_speaker.get_qts() ) / ( 1 / 0.707 - 0.1 );
        m_box->set_fb1( qr * m_current_speaker.get_fs() );
        g_string_printf(buffer, "%f", m_box->get_fb1());
        m_fb1_entry.set_text( Glib::ustring(buffer->str) );

        break;
      case PORTED_SELECTED:
        cout << "ported enclosure" << endl;
        m_box->set_type(BOX_TYPE_PORTED);
        m_fb1_entry.set_sensitive(true);
        break;
      case ID_STRING_ENTRY_CHANGED:
        m_box->set_id_string(m_id_string_entry.get_text());
        cout << "id string entry" << endl;
        break;
      case VB1_ENTRY_CHANGED:
        cout << "vb1 entry changed" << endl;
        m_box->set_vb1(atof(m_vb1_entry.get_text().c_str()));
        break;
      case FB1_ENTRY_CHANGED:
        cout << "fb1 entry changed" << endl;
        m_box->set_fb1(atof(m_fb1_entry.get_text().c_str()));
        break;
      case VB2_ENTRY_CHANGED:
        cout << "vb2 entry changed" << endl;
        m_box->set_vb2(atof(m_vb2_entry.get_text().c_str()));
        break;
      case FB2_ENTRY_CHANGED:
        cout << "fb2 entry changed" << endl;
        m_box->set_fb2(atof(m_fb2_entry.get_text().c_str()));
        break;
      
    }
    signal_box_selected(m_box);
    signal_box_modified(m_box);
    disable_signals = false;
  }
}
