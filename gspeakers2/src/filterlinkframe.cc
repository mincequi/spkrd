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
  
FilterLinkFrame::FilterLinkFrame(Net *net, const string& description, SpeakerList *speaker_list) :
  Gtk::Frame(description),
  adj(1.0, 1.0, 31.0, 1.0, 5.0, 0.0),
  m_speaker_combo(),
  //m_order_optionmenu(),
  //m_type_optionmenu(),
  //m_co_freq_spinbutton(1.0, 5),
  m_inv_pol_checkbutton("Invert polarity", false),
  m_damp_spinbutton(*(new Gtk::Adjustment(0, 0, 100, 1, 5.0))),
  m_imp_corr_checkbutton("Impedance correction")
{
  m_net = net;
  m_description = description;
  m_speaker_list = speaker_list;
  
  m_vbox.set_border_width(5);
  vector<string> popdown_strings;
  for (
    vector<Speaker>::iterator iter = m_speaker_list->speaker_list()->begin();
    iter != m_speaker_list->speaker_list()->end();
    ++iter)
  {
    /* TODO: only insert speakers appropriate for this particular crossover */
    popdown_strings.push_back((*iter).get_id_string());
  }
  m_speaker_combo.set_popdown_strings(popdown_strings);
  m_speaker_combo.get_entry()->set_editable(false);
  m_vbox.pack_start(m_speaker_combo);
    
  Gtk::Label *label;
    
  if (m_net->get_type() & NET_TYPE_LOWPASS) {
    Gtk::Frame *frame = manage(new Gtk::Frame("Lowpass"));
    m_vbox.pack_start(*frame);
    Gtk::VBox *vbox = manage(new Gtk::VBox());
    frame->add(*vbox);
    vbox->set_border_width(5);
    
    /* Setup menus */
    m_lower_order_optionmenu = manage(new Gtk::OptionMenu());
    m_lower_order_menu = manage(new Gtk::Menu());
    Gtk::Menu::MenuList& menulist = m_lower_order_menu->items();
    menulist.push_back(Gtk::Menu_Helpers::MenuElem("1", bind<int, int>(slot(*this, &FilterLinkFrame::on_order_selected), 0, 1)));
    menulist.push_back(Gtk::Menu_Helpers::MenuElem("2", bind<int, int>(slot(*this, &FilterLinkFrame::on_order_selected), 0, 2)));
    menulist.push_back(Gtk::Menu_Helpers::MenuElem("3", bind<int, int>(slot(*this, &FilterLinkFrame::on_order_selected), 0, 3)));
    menulist.push_back(Gtk::Menu_Helpers::MenuElem("4", bind<int, int>(slot(*this, &FilterLinkFrame::on_order_selected), 0, 4)));
    
    m_lower_order_optionmenu->set_menu(*m_lower_order_menu);

    
    m_lower_type_optionmenu = manage(new Gtk::OptionMenu());
    m_lower_type_menu = manage(new Gtk::Menu());
    m_lower_order_optionmenu->set_history(m_net->get_lowpass_order() - 1);
    on_order_selected(0, m_net->get_lowpass_order());
       
    /* Grey items in this menu */
    
    m_lower_type_optionmenu->set_menu(*m_lower_type_menu);
    /* menus ready */
    
    Gtk::HBox *hbox = manage(new Gtk::HBox());
    vbox->pack_start(*hbox);
    hbox->pack_start((*manage(new Gtk::Label("Order: "))));
    hbox->pack_start(*m_lower_order_optionmenu);
    vbox->pack_start(*m_lower_type_optionmenu);
    hbox = manage(new Gtk::HBox());
    hbox->pack_start((*manage(new Gtk::Label("Cutoff: "))));
    m_lower_co_freq_spinbutton = manage(new Gtk::SpinButton(*(new Gtk::Adjustment(2000, 1, 20000, 1, 100))));
    
    hbox->pack_start(*m_lower_co_freq_spinbutton);
    hbox->pack_start((*manage(new Gtk::Label("Hz"))));
    vbox->pack_start(*hbox);
  } 
  
  if (net->get_type() & NET_TYPE_HIGHPASS) {
    Gtk::Frame *frame = manage(new Gtk::Frame("Highpass"));
    m_vbox.pack_start(*frame);
    Gtk::VBox *vbox = manage(new Gtk::VBox());
    frame->add(*vbox);
    vbox->set_border_width(5);

    /* Setup menus */
    m_higher_order_optionmenu = manage(new Gtk::OptionMenu());
    m_higher_order_menu = manage(new Gtk::Menu());
    Gtk::Menu::MenuList& menulist = m_higher_order_menu->items();
    menulist.push_back(Gtk::Menu_Helpers::MenuElem("1", bind<int, int>(slot(*this, &FilterLinkFrame::on_order_selected), 1, 1)));
    menulist.push_back(Gtk::Menu_Helpers::MenuElem("2", bind<int, int>(slot(*this, &FilterLinkFrame::on_order_selected), 1, 2)));
    menulist.push_back(Gtk::Menu_Helpers::MenuElem("3", bind<int, int>(slot(*this, &FilterLinkFrame::on_order_selected), 1, 3)));
    menulist.push_back(Gtk::Menu_Helpers::MenuElem("4", bind<int, int>(slot(*this, &FilterLinkFrame::on_order_selected), 1, 4)));
    
    m_higher_order_optionmenu->set_menu(*m_higher_order_menu);
    
    
    m_higher_type_optionmenu = manage(new Gtk::OptionMenu());
    m_higher_type_menu = manage(new Gtk::Menu());
    
    m_higher_order_optionmenu->set_history(m_net->get_highpass_order() - 1);
    on_order_selected(1, m_net->get_highpass_order());

    m_higher_type_optionmenu->set_menu(*m_higher_type_menu);
  
    /* menus ready */

    Gtk::HBox *hbox = manage(new Gtk::HBox());
    vbox->pack_start(*hbox);
    hbox->pack_start((*manage(new Gtk::Label("Order: ", Gtk::ALIGN_LEFT))));
    
    hbox->pack_start(*m_higher_order_optionmenu);
    vbox->pack_start(*m_higher_type_optionmenu);
    hbox = manage(new Gtk::HBox());
    hbox->pack_start((*manage(new Gtk::Label("Cutoff: "))));
    m_higher_co_freq_spinbutton = manage(new Gtk::SpinButton(*(new Gtk::Adjustment(2000, 1, 20000, 1, 100))));
    
    hbox->pack_start(*m_higher_co_freq_spinbutton);
    label = manage(new Gtk::Label(" Hz"));
    label->set_alignment(Gtk::ALIGN_LEFT);
    hbox->pack_start(*label);
    vbox->pack_start(*hbox);
  }
    
  //m_vbox.pack_start(m_inv_pol_checkbutton);
  m_vbox.pack_start(m_imp_corr_checkbutton);
  
  Gtk::HBox *hbox = manage(new Gtk::HBox());
  m_vbox.pack_start(*hbox);
  hbox->pack_start((*manage(new Gtk::Label("Damping: "))));
  hbox->pack_start(m_damp_spinbutton);
  
  hbox->pack_start((*manage(new Gtk::Label("dB"))));
  
  add(m_vbox);
  show_all();
  
  m_speaker_combo.get_entry()->signal_changed().connect(slot(*this, &FilterLinkFrame::on_param_changed));
  if (net->get_type() & NET_TYPE_LOWPASS) {
    m_lower_type_optionmenu->signal_changed().connect(slot(*this, &FilterLinkFrame::on_param_changed)); 
    m_lower_co_freq_spinbutton->signal_value_changed().connect(slot(*this, &FilterLinkFrame::on_param_changed));
    m_lower_order_optionmenu->signal_changed().connect(slot(*this, &FilterLinkFrame::on_param_changed));
  } 
  if (net->get_type() & NET_TYPE_HIGHPASS) {
    m_higher_order_optionmenu->signal_changed().connect(slot(*this, &FilterLinkFrame::on_param_changed));
    m_higher_co_freq_spinbutton->signal_value_changed().connect(slot(*this, &FilterLinkFrame::on_param_changed));
    m_higher_type_optionmenu->signal_changed().connect(slot(*this, &FilterLinkFrame::on_param_changed));
  }  
  m_imp_corr_checkbutton.signal_toggled().connect(slot(*this, &FilterLinkFrame::on_param_changed));
  m_damp_spinbutton.signal_value_changed().connect(slot(*this, &FilterLinkFrame::on_param_changed));
}

void FilterLinkFrame::on_order_selected(int which, int order)
{
  cout << "FilterLinkFrame::on_order_selected, which = " << which << "   order = " << order << endl;
  
  Gtk::Menu::MenuList *menulist = &(m_lower_type_menu->items());
    
  if (which == 0) {
    menulist = &m_lower_type_menu->items();
  } else if (which == 1) {
    menulist = &m_higher_type_menu->items(); 
  }
  if (menulist->size() > 0) {
    menulist->erase(menulist->begin(), menulist->end());
  }
  switch (order) {
    case NET_ORDER_1ST:
      menulist->push_back(Gtk::Menu_Helpers::MenuElem("Butterworth"));    
      break;
    case NET_ORDER_2ND:
      menulist->push_back(Gtk::Menu_Helpers::MenuElem("Bessel"));    
      menulist->push_back(Gtk::Menu_Helpers::MenuElem("Butterworth"));    
      menulist->push_back(Gtk::Menu_Helpers::MenuElem("Chebychev"));    
      menulist->push_back(Gtk::Menu_Helpers::MenuElem("Linkwitz-Riley"));    
      break;
    case NET_ORDER_3RD:
      menulist->push_back(Gtk::Menu_Helpers::MenuElem("Bessel"));    
      menulist->push_back(Gtk::Menu_Helpers::MenuElem("Butterworth"));    
      break;
    case NET_ORDER_4TH:
      menulist->push_back(Gtk::Menu_Helpers::MenuElem("Bessel"));    
      menulist->push_back(Gtk::Menu_Helpers::MenuElem("Butterworth"));    
      menulist->push_back(Gtk::Menu_Helpers::MenuElem("Gaussian"));    
      menulist->push_back(Gtk::Menu_Helpers::MenuElem("Legendre"));    
      menulist->push_back(Gtk::Menu_Helpers::MenuElem("Linear-Phase"));    
      menulist->push_back(Gtk::Menu_Helpers::MenuElem("Linkwitz-Riley"));    
      break;
  
  }
  if (which == 0) {
    m_lower_type_optionmenu->set_history(0);
  } else if (which == 1) {
    m_higher_type_optionmenu->set_history(0);
  }
}

void FilterLinkFrame::on_param_changed()
{
  cout << "FilterLinkFrame::on_param_changed" << endl;
  Speaker speaker = m_speaker_list->get_speaker_by_id_string(m_speaker_combo.get_entry()->get_text());
  
  int index = 0;
  
  if (m_net->get_type() & NET_TYPE_LOWPASS) {
    m_net->set_lowpass_order(m_lower_order_optionmenu->get_history() + 1);
    double cutoff = m_lower_co_freq_spinbutton->get_value();
    switch (m_net->get_lowpass_order()) {
      case NET_ORDER_1ST:
        (*m_net->parts())[index].set_value((0.159 / (speaker.get_rdc() * cutoff)) * 1000000);
        (*m_net->parts())[index++].set_unit("u");
        break;
      case NET_ORDER_2ND:
        switch (m_lower_type_optionmenu->get_history()) {
          case 0:  // bessel
            break;
          case 1:  // butterworth
            break;
          case 2:  // chebychev
            break;
          case 3:  // linkwitz-riley
            break;
        }
        break;
      case NET_ORDER_3RD:
        switch (m_lower_type_optionmenu->get_history()) {
          case 0:   // bessel
            break;
          case 1:   // butterworth
            break;
        }
        break;
      case NET_ORDER_4TH:
        switch (m_lower_type_optionmenu->get_history()) {
          case 0:  // bessel
            break;
          case 1:  // butterworth
            break;
          case 2:  // gaussian
            break;
          case 3:  // legendre
            break;
          case 4:  // liner-phase
            break;
          case 5:  // linkwitz-riley
            break;
        }
        break;
    }
  }
  if (m_net->get_type() & NET_TYPE_HIGHPASS) {
    m_net->set_highpass_order(m_higher_order_optionmenu->get_history() + 1);

    double cutoff = m_higher_co_freq_spinbutton->get_value();
    switch (m_net->get_highpass_order()) {
      case NET_ORDER_1ST:
        (*m_net->parts())[index].set_value((0.159 / (speaker.get_rdc() * cutoff)) * 1000000);
        (*m_net->parts())[index++].set_unit("u");
        break;
      case NET_ORDER_2ND:
        switch (m_higher_type_optionmenu->get_history()) {
          case 0:  // bessel
            break;
          case 1:  // butterworth
            break;
          case 2:  // chebychev
            break;
          case 3:  // linkwitz-riley
            break;
        }
        break;
      case NET_ORDER_3RD:
        switch (m_higher_type_optionmenu->get_history()) {
          case 0:   // bessel
            break;
          case 1:   // butterworth
            break;
        }
        break;
      case NET_ORDER_4TH:
        switch (m_higher_type_optionmenu->get_history()) {
          case 0:  // bessel
            break;
          case 1:  // butterworth
            break;
          case 2:  // gaussian
            break;
          case 3:  // legendre
            break;
          case 4:  // liner-phase
            break;
          case 5:  // linkwitz-riley
            break;
        }
        break;
    }

  }
  if (m_imp_corr_checkbutton.get_state() == true) {
    m_net->set_has_imp_corr(true);
    /* calc imp corr here */
  } else {
    m_net->set_has_imp_corr(false);
  }
  if (m_damp_spinbutton.get_value_as_int() > 0) {
    m_net->set_has_damp(true);
    /* calculate damping network */
  } else {
    m_net->set_has_damp(false);
  }
  signal_net_modified_by_wizard();
}

void FilterLinkFrame::on_net_updated(Net *net)
{

}

vector<double> get_filter_params(int net_name_type, int net_order, int net_type)
{
  vector<double> nums;
  switch (net_order) {
    case NET_ORDER_1ST:
      switch (net_type) {
        case NET_TYPE_LOWPASS:
          nums.push_back(6.28);
          break;
        case NET_TYPE_HIGHPASS:
          nums.push_back(0.159);
          break;
      }
      break;
    case NET_ORDER_2ND:
      switch (net_name_type) {
        case NET_BESSEL:
          switch (net_type) {
            case NET_TYPE_LOWPASS:
              break;
            case NET_TYPE_HIGHPASS:
              break;
          }
        
          break;
        case NET_BUTTERWORTH:
          switch (net_type) {
            case NET_TYPE_LOWPASS:
              break;
            case NET_TYPE_HIGHPASS:
              break;
          }
          break;
        case NET_CHEBYCHEV:
          switch (net_type) {
            case NET_TYPE_LOWPASS:
              break;
            case NET_TYPE_HIGHPASS:
              break;
          }
        
          break;
        case NET_LINKWITZRILEY:
          switch (net_type) {
            case NET_TYPE_LOWPASS:
              break;
            case NET_TYPE_HIGHPASS:
              break;
          }
        
          break;
      }
      break;
    case NET_ORDER_3RD:
      switch (net_name_type) {
        case NET_BESSEL:
          switch (net_type) {
            case NET_TYPE_LOWPASS:
              break;
            case NET_TYPE_HIGHPASS:
              break;
          }
        
          break;
        case NET_BUTTERWORTH:
          switch (net_type) {
            case NET_TYPE_LOWPASS:
              break;
            case NET_TYPE_HIGHPASS:
              break;
          }
          break;
      }
    
      break;
    case NET_ORDER_4TH:
      switch (net_name_type) {
        case NET_BESSEL:
          switch (net_type) {
            case NET_TYPE_LOWPASS:
              break;
            case NET_TYPE_HIGHPASS:
              break;
          }
        
          break;
        case NET_BUTTERWORTH:
          switch (net_type) {
            case NET_TYPE_LOWPASS:
              break;
            case NET_TYPE_HIGHPASS:
              break;
          }
          break;
        case NET_GAUSSIAN:
          switch (net_type) {
            case NET_TYPE_LOWPASS:
              break;
            case NET_TYPE_HIGHPASS:
              break;
          }
        
          break;
        case NET_LEGENDRE:
          switch (net_type) {
            case NET_TYPE_LOWPASS:
              break;
            case NET_TYPE_HIGHPASS:
              break;
          }
        case NET_LINEARPHASE:
          switch (net_type) {
            case NET_TYPE_LOWPASS:
              break;
            case NET_TYPE_HIGHPASS:
              break;
          }
        
          break;
        case NET_LINKWITZRILEY:
          switch (net_type) {
            case NET_TYPE_LOWPASS:
              break;
            case NET_TYPE_HIGHPASS:
              break;
          }
        
          break;
      }
    
      break;
  }
  return nums;
}
