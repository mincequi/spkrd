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

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include "filterlinkframe.h"
#include "gspeakersplot.h"
  
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
  enable_edit = true;
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
  
    
  //m_vbox.pack_start(m_inv_pol_checkbutton);
  m_vbox.pack_start(m_imp_corr_checkbutton);
  if (m_net->get_has_imp_corr() == true) {
    m_imp_corr_checkbutton.set_active(true);
  }
  
  Gtk::HBox *hbox = manage(new Gtk::HBox());
  m_vbox.pack_start(*hbox);
  hbox->pack_start((*manage(new Gtk::Label("Damping: "))));
  hbox->pack_start(m_damp_spinbutton);
  
  if (m_net->get_has_damp() == true) {
    /* Set damp value in dB here */
    double r_ser = m_net->get_damp_R1().get_value();
    Speaker speaker = m_speaker_list->get_speaker_by_id_string(m_speaker_combo.get_entry()->get_text());

    m_damp_spinbutton.set_value(round(20 * log10(r_ser / speaker.get_rdc() + 1)));
  }  

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
  signal_net_modified_by_user.connect(slot(*this, &FilterLinkFrame::on_net_updated));
  signal_plot_crossover.connect(slot(*this, &FilterLinkFrame::on_plot_crossover));
  g_settings.defaultValueString("SPICECmdLine", "spice3");
  my_filter_plot_index = -1;
  //on_plot_crossover();
  //signal_plot_crossover();
  
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
  
  if (enable_edit == true) {
    Speaker speaker = m_speaker_list->get_speaker_by_id_string(m_speaker_combo.get_entry()->get_text());
    
    int index = 0;
    vector<double> num_params;
    
    if (m_net->get_type() & NET_TYPE_LOWPASS) {
      m_net->set_lowpass_order(m_lower_order_optionmenu->get_history() + 1);
      double cutoff = m_lower_co_freq_spinbutton->get_value();
      switch (m_net->get_lowpass_order()) {
        case NET_ORDER_1ST:
          num_params = get_filter_params(NET_BUTTERWORTH, NET_ORDER_1ST, NET_TYPE_LOWPASS);
          (*m_net->parts())[index].set_value((speaker.get_rdc() / (num_params[0] * cutoff)) * 1000);
          (*m_net->parts())[index++].set_unit("m");
          break;
        case NET_ORDER_2ND:
          switch (m_lower_type_optionmenu->get_history()) {
            case 0:  // bessel
              num_params = get_filter_params(NET_BESSEL,        NET_ORDER_2ND, NET_TYPE_LOWPASS);
              break;
            case 1:  // butterworth
              num_params = get_filter_params(NET_BUTTERWORTH,   NET_ORDER_2ND, NET_TYPE_LOWPASS);
              break;
            case 2:  // chebychev
              num_params = get_filter_params(NET_CHEBYCHEV,     NET_ORDER_2ND, NET_TYPE_LOWPASS);
              break;
            case 3:  // linkwitz-riley
              num_params = get_filter_params(NET_LINKWITZRILEY, NET_ORDER_2ND, NET_TYPE_LOWPASS);
              break;
          }
          /* inductor */
          (*m_net->parts())[index].set_value(((speaker.get_rdc() * num_params[0]) / cutoff) * 1000);
          (*m_net->parts())[index++].set_unit("m");
          /* capacitor */
          (*m_net->parts())[index].set_value((num_params[1]/ (speaker.get_rdc() * cutoff)) * 1000000);
          (*m_net->parts())[index++].set_unit("u");
          break;
        case NET_ORDER_3RD:
          switch (m_lower_type_optionmenu->get_history()) {
            case 0:   // bessel
              num_params = get_filter_params(NET_BESSEL,        NET_ORDER_3RD, NET_TYPE_LOWPASS);
              break;
            case 1:   // butterworth
              num_params = get_filter_params(NET_BUTTERWORTH,   NET_ORDER_3RD, NET_TYPE_LOWPASS);
              break;
          }
          /* inductor */
          (*m_net->parts())[index].set_value(((speaker.get_rdc() * num_params[0]) / cutoff) * 1000);
          (*m_net->parts())[index++].set_unit("m");
          /* capacitor */
          (*m_net->parts())[index].set_value((num_params[1]/ (speaker.get_rdc() * cutoff)) * 1000000);
          (*m_net->parts())[index++].set_unit("u");
          /* inductor */
          (*m_net->parts())[index].set_value(((speaker.get_rdc() * num_params[2]) / cutoff) * 1000);
          (*m_net->parts())[index++].set_unit("m");
          break;
        case NET_ORDER_4TH:
          switch (m_lower_type_optionmenu->get_history()) {
            case 0:  // bessel
              num_params = get_filter_params(NET_BESSEL,        NET_ORDER_4TH, NET_TYPE_LOWPASS);
              break;
            case 1:  // butterworth
              num_params = get_filter_params(NET_BUTTERWORTH,   NET_ORDER_4TH, NET_TYPE_LOWPASS);
              break;
            case 2:  // gaussian
              num_params = get_filter_params(NET_GAUSSIAN,      NET_ORDER_4TH, NET_TYPE_LOWPASS);
              break;
            case 3:  // legendre
              num_params = get_filter_params(NET_LEGENDRE,      NET_ORDER_4TH, NET_TYPE_LOWPASS);
              break;
            case 4:  // liner-phase
              num_params = get_filter_params(NET_LINEARPHASE,   NET_ORDER_4TH, NET_TYPE_LOWPASS);
              break;
            case 5:  // linkwitz-riley
              num_params = get_filter_params(NET_LINKWITZRILEY, NET_ORDER_4TH, NET_TYPE_LOWPASS);
              break;
          }
          /* inductor */
          (*m_net->parts())[index].set_value(((speaker.get_rdc() * num_params[0]) / cutoff) * 1000);
          (*m_net->parts())[index++].set_unit("m");
          /* capacitor */
          (*m_net->parts())[index].set_value((num_params[1]/ (speaker.get_rdc() * cutoff)) * 1000000);
          (*m_net->parts())[index++].set_unit("u");
          /* inductor */
          (*m_net->parts())[index].set_value(((speaker.get_rdc() * num_params[2]) / cutoff) * 1000);
          (*m_net->parts())[index++].set_unit("m");
          /* capacitor */
          (*m_net->parts())[index].set_value((num_params[3]/ (speaker.get_rdc() * cutoff)) * 1000000);
          (*m_net->parts())[index++].set_unit("u");
          break;
      }
    }
    if (m_net->get_type() & NET_TYPE_HIGHPASS) {
      m_net->set_highpass_order(m_higher_order_optionmenu->get_history() + 1);
  
      double cutoff = m_higher_co_freq_spinbutton->get_value();
      switch (m_net->get_highpass_order()) {
        case NET_ORDER_1ST:
          num_params = get_filter_params(NET_BUTTERWORTH, NET_ORDER_1ST, NET_TYPE_HIGHPASS);
          (*m_net->parts())[index].set_value(num_params[0] / (speaker.get_rdc() * cutoff) * 1000000);
          (*m_net->parts())[index++].set_unit("u");
          break;
        case NET_ORDER_2ND:
          switch (m_higher_type_optionmenu->get_history()) {
            case 0:  // bessel
              num_params = get_filter_params(NET_BESSEL,        NET_ORDER_2ND, NET_TYPE_HIGHPASS);
              break;
            case 1:  // butterworth
              num_params = get_filter_params(NET_BUTTERWORTH,   NET_ORDER_2ND, NET_TYPE_HIGHPASS);
              break;
            case 2:  // chebychev
              num_params = get_filter_params(NET_CHEBYCHEV,     NET_ORDER_2ND, NET_TYPE_HIGHPASS);
              break;
            case 3:  // linkwitz-riley
              num_params = get_filter_params(NET_LINKWITZRILEY, NET_ORDER_2ND, NET_TYPE_HIGHPASS);
              break;
          }
          /* capacitor */
          (*m_net->parts())[index].set_value((num_params[0] / (speaker.get_rdc() * cutoff)) * 1000000);
          (*m_net->parts())[index++].set_unit("u");
          /* inductor */
          (*m_net->parts())[index].set_value((num_params[1] * speaker.get_rdc() / cutoff) * 1000);
          (*m_net->parts())[index++].set_unit("m");
          break;
        case NET_ORDER_3RD:
          switch (m_higher_type_optionmenu->get_history()) {
            case 0:   // bessel
              num_params = get_filter_params(NET_BESSEL, NET_ORDER_3RD, NET_TYPE_HIGHPASS);
              break;
            case 1:   // butterworth
              num_params = get_filter_params(NET_BUTTERWORTH, NET_ORDER_3RD, NET_TYPE_HIGHPASS);
              break;
          }
          /* capacitor */
          (*m_net->parts())[index].set_value((num_params[0] / (speaker.get_rdc() * cutoff)) * 1000000);
          (*m_net->parts())[index++].set_unit("u");
          /* inductor */
          (*m_net->parts())[index].set_value((num_params[1] * speaker.get_rdc() / cutoff) * 1000);
          (*m_net->parts())[index++].set_unit("m");
          /* capacitor */
          (*m_net->parts())[index].set_value((num_params[2] / (speaker.get_rdc() * cutoff)) * 1000000);
          (*m_net->parts())[index++].set_unit("u");
          break;
        case NET_ORDER_4TH:
          switch (m_higher_type_optionmenu->get_history()) {
            case 0:  // bessel
              num_params = get_filter_params(NET_BESSEL, NET_ORDER_4TH, NET_TYPE_HIGHPASS);
              break;
            case 1:  // butterworth
              num_params = get_filter_params(NET_BUTTERWORTH, NET_ORDER_4TH, NET_TYPE_HIGHPASS);
              break;
            case 2:  // gaussian
              num_params = get_filter_params(NET_GAUSSIAN, NET_ORDER_4TH, NET_TYPE_HIGHPASS);
              break;
            case 3:  // legendre
              num_params = get_filter_params(NET_LEGENDRE, NET_ORDER_4TH, NET_TYPE_HIGHPASS);
              break;
            case 4:  // liner-phase
              num_params = get_filter_params(NET_LINEARPHASE, NET_ORDER_4TH, NET_TYPE_HIGHPASS);
              break;
            case 5:  // linkwitz-riley
              num_params = get_filter_params(NET_LINKWITZRILEY, NET_ORDER_4TH, NET_TYPE_HIGHPASS);
              break;
          }
          /* capacitor */
          (*m_net->parts())[index].set_value((num_params[0] / (speaker.get_rdc() * cutoff)) * 1000000);
          (*m_net->parts())[index++].set_unit("u");
          /* inductor */
          (*m_net->parts())[index].set_value((num_params[1] * speaker.get_rdc() / cutoff) * 1000);
          (*m_net->parts())[index++].set_unit("m");
          /* capacitor */
          (*m_net->parts())[index].set_value((num_params[2] / (speaker.get_rdc() * cutoff)) * 1000000);
          (*m_net->parts())[index++].set_unit("u");
          /* inductor */
          (*m_net->parts())[index].set_value((num_params[3] * speaker.get_rdc() / cutoff) * 1000);
          (*m_net->parts())[index++].set_unit("m");
          break;
      }
  
    }
    cout << "imp_corr_checkbutton state: " << m_imp_corr_checkbutton.get_active() << endl;
    if (m_imp_corr_checkbutton.get_active() == true) {
      m_net->set_has_imp_corr(true);
      /* calc imp corr here */
      m_net->get_imp_corr_C().set_value((speaker.get_lvc() / 1000) / pow(speaker.get_rdc(), 2) * 1000000);
      m_net->get_imp_corr_C().set_unit("u");
      m_net->get_imp_corr_R().set_value(speaker.get_rdc());
    } else {
      m_net->set_has_imp_corr(false);
    }
    if (m_damp_spinbutton.get_value_as_int() > 0) {
      m_net->set_has_damp(true);
      
      /* Calculate resistors for damping network */
      double r_ser = speaker.get_rdc() * (pow(10, m_damp_spinbutton.get_value() / 20) - 1);
      double r_par = speaker.get_rdc() + pow(speaker.get_rdc(), 2) / r_ser;
      m_net->get_damp_R2().set_value(r_ser);
      m_net->get_damp_R1().set_value(r_par);
      
      
    } else {
      m_net->set_has_damp(false);
    }
    signal_net_modified_by_wizard();
    
  }
  on_plot_crossover();
  //signal_plot_crossover();
  
}

void FilterLinkFrame::on_net_updated(Net *net)
{
  cout << "FilterLinkFrame::on_net_updated" << endl;
  enable_edit = false;
  Speaker speaker = m_speaker_list->get_speaker_by_id_string(m_speaker_combo.get_entry()->get_text());
  int index = 0;
  if (net->get_id() == m_net->get_id()) {
    if (m_net->get_type() & NET_TYPE_LOWPASS) {
      switch (m_net->get_lowpass_order()) {
        case NET_ORDER_1ST:
          m_lower_co_freq_spinbutton->set_value(speaker.get_rdc() / ((*m_net->parts())[index++].get_value()/1000 * 6.28));
          break;
        case NET_ORDER_2ND:
          m_lower_co_freq_spinbutton->set_value((1/sqrt((*m_net->parts())[0].get_value()/1000 * (*m_net->parts())[1].get_value()/1000000)) / (2 * 3.14159));
          break;
        case NET_ORDER_3RD:
          break;
        case NET_ORDER_4TH:
          break;
      }
    }
    if (m_net->get_type() & NET_TYPE_HIGHPASS) {
      switch (m_net->get_highpass_order()) {
        case NET_ORDER_1ST:
          m_higher_co_freq_spinbutton->set_value(0.159 / ((*m_net->parts())[index].get_value()/1000000 * speaker.get_rdc()));
          break;
        case NET_ORDER_2ND:
          m_higher_co_freq_spinbutton->set_value(1 / (sqrt( ((*m_net->parts())[1].get_value()/1000) * (*m_net->parts())[0].get_value()/1000000)) / (2 * 3.14159));
          break;
        case NET_ORDER_3RD:
          break;
        case NET_ORDER_4TH:
          break;
      }
    
    }
  }
  enable_edit = true;
}

void FilterLinkFrame::on_plot_crossover()
{
  /* Create spice code for this net */
  Speaker speaker = m_speaker_list->get_speaker_by_id_string(m_speaker_combo.get_entry()->get_text());
  string spice_filename;
  try {
    spice_filename = m_net->to_SPICE(speaker);
  } catch (GSpeakersException e) {
    cout << "FilterLinkFrame::on_plot_crossover: ERROR: " << e.what() << endl;
  }
  
  /* run spice with created file */
  string cmd = g_settings.getValueString("SPICECmdLine") + " -b -o " + spice_filename + ".out " + spice_filename;
  cout << "FilterLinkFrame::on_plot_crossover: running SPICE with \"" + cmd + "\"" << endl;
  system(cmd.c_str());
  cout << "FilterLinkFrame::on_plot_crossover: SPICE done" << endl;
  /* extract spice output into a vector */
  string spice_output_file = spice_filename + ".out";
  ifstream fin(spice_output_file.c_str());
  bool output = false;
  int id;
  float f1, f2, f3;
  vector<GSpeakers::Point> points;
  while (!fin.eof()) {
    char *buffer = new char[100];
    fin.getline(buffer, 100, '\n');
    if (buffer[0] == '0') {
      output = true;
    }
    if (output == true) {
      /* TODO: Read the sscanf string from settings */
      sscanf(buffer, "%d\t%f,\t%f\t%f", &id, &f1, &f2, &f3);
      GSpeakers::Point p(round(f1), f3);
      points.push_back(p);
    }
    if ((buffer[0] == '3') && (buffer[1] == '0')) {
      output = false;
    }
  }
  /* send the spice data to the plot */
  /* TODO: improve color handling here */
  Gdk::Color c;
  if (m_net->get_type() == NET_TYPE_LOWPASS) {
    c = Gdk::Color("blue");
  } else if (m_net->get_type() == NET_TYPE_HIGHPASS) {
    c = Gdk::Color("red");
  } else if (m_net->get_type() == NET_TYPE_BANDPASS) {
    c = Gdk::Color("darkgreen");
  }
  int id2 = m_net->get_id();
  cout << "FilterLinkFrame::on_plot_crossover: id = " << id2 << endl;
  signal_add_crossover_plot(points, c, &my_filter_plot_index);
  /* TODO: update filterwizard, co_freq_spinbutton here */
  
}   

vector<double> FilterLinkFrame::get_filter_params(int net_name_type, int net_order, int net_type)
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
              nums.push_back(0.2756);
              nums.push_back(0.0912);
              break;
            case NET_TYPE_HIGHPASS:
              nums.push_back(0.2756);
              nums.push_back(0.912);
              break;
          }
        
          break;
        case NET_BUTTERWORTH:
          switch (net_type) {
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
          switch (net_type) {
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
          switch (net_type) {
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
      switch (net_name_type) {
        case NET_BESSEL:
          switch (net_type) {
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
          switch (net_type) {
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
      switch (net_name_type) {
        case NET_BESSEL:
          switch (net_type) {
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
          switch (net_type) {
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
          switch (net_type) {
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
          switch (net_type) {
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
          switch (net_type) {
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
          switch (net_type) {
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
  return nums;
}

/*
 * Helper function to convert double->int and round it to nearest int
 *
 * The compiler wouldn't accept round (in math.h) so...
 */
int FilterLinkFrame::round( double x ) {
  if ( ( x - (int)x ) >= 0.5 ) {
    return (int)( (int)x + 1 );
  } else {
    return (int)(x);
  }
  return 0;
}
