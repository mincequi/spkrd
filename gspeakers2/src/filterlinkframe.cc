/*
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
  
FilterLinkFrame::FilterLinkFrame(int net_type, string description) :
  Gtk::Frame(description),
  m_speaker_combo(),
  //m_order_optionmenu(),
  //m_type_optionmenu(),
  //m_co_freq_spinbutton(1.0, 5),
  m_inv_pol_checkbutton("Invert polarity", false),
  m_damp_spinbutton(0.5, 2)  
{
  m_net_type = net_type;
  m_description = description;
  
  m_vbox.set_border_width(5);
  m_vbox.pack_start(m_speaker_combo);
    
  if (m_net_type & NET_TYPE_LOWPASS) {
    Gtk::Frame *frame = manage(new Gtk::Frame());
    m_vbox.pack_start(*frame);
    Gtk::VBox *vbox = manage(new Gtk::VBox());
    frame->add(*vbox);
    vbox->set_border_width(5);
    
    /* Setup menus */
    
    Gtk::HBox *hbox = manage(new Gtk::HBox());
    vbox->pack_start(*hbox);
    hbox->pack_start((*manage(new Gtk::Label("Order: "))));
    m_lower_order_optionmenu = manage(new Gtk::OptionMenu());
    hbox->pack_start(*m_lower_order_optionmenu);
    m_lower_type_optionmenu = manage(new Gtk::OptionMenu());
    vbox->pack_start(*m_lower_type_optionmenu);
    hbox = manage(new Gtk::HBox());
    hbox->pack_start((*manage(new Gtk::Label("Cutoff: "))));
    m_lower_co_freq_spinbutton = manage(new Gtk::SpinButton(1.0, 5));
    hbox->pack_start(*m_lower_co_freq_spinbutton);
    vbox->pack_start(*hbox);
  } 
  
  if (m_net_type & NET_TYPE_HIGHPASS) {
    Gtk::Frame *frame = manage(new Gtk::Frame());
    m_vbox.pack_start(*frame);
    Gtk::VBox *vbox = manage(new Gtk::VBox());
    frame->add(*vbox);
    vbox->set_border_width(5);

    Gtk::HBox *hbox = manage(new Gtk::HBox());
    vbox->pack_start(*hbox);
    hbox->pack_start((*manage(new Gtk::Label("Order: "))));
    m_higher_order_optionmenu = manage(new Gtk::OptionMenu());
    hbox->pack_start(*m_higher_order_optionmenu);
    m_higher_type_optionmenu = manage(new Gtk::OptionMenu());
    vbox->pack_start(*m_higher_type_optionmenu);
    hbox = manage(new Gtk::HBox());
    hbox->pack_start((*manage(new Gtk::Label("Cutoff: "))));
    m_higher_co_freq_spinbutton = manage(new Gtk::SpinButton(1.0, 5));
    hbox->pack_start(*m_higher_co_freq_spinbutton);
    vbox->pack_start(*hbox);
  }
    
  m_vbox.pack_start(m_inv_pol_checkbutton);
  Gtk::HBox *hbox = manage(new Gtk::HBox());
  m_vbox.pack_start(*hbox);
  hbox->pack_start((*manage(new Gtk::Label("Damping: "))));
  hbox->pack_start(m_damp_spinbutton);

  add(m_vbox);
  show_all();
}
