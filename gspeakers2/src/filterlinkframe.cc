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

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include "filterlinkframe.h"
#include "gspeakersplot.h"
#include "common.h"
#include "../config.h"

using namespace sigc;

FilterLinkFrame::FilterLinkFrame(Net *net, const string& description, SpeakerList *speaker_list) :
  Gtk::Frame(""),
  adj(1.0, 1.0, 31.0, 1.0, 5.0, 0.0),
  m_speaker_combo(),
  m_inv_pol_checkbutton(_("Invert polarity"), false),
  m_damp_spinbutton(*(new Gtk::Adjustment(0, 0, 100, 1, 5.0))),
  m_imp_corr_checkbutton(_("Impedance correction")),
  m_adv_imp_model_checkbutton(_("Use adv. driver imp. model"))
{
  set_border_width(2);
  set_shadow_type(Gtk::SHADOW_NONE);
  static_cast<Gtk::Label*>(get_label_widget())->set_markup("<b>" + description + "</b>");
  m_vbox.set_border_width(12);

  init = true;
  enable_edit = false;
  m_net = net;
  m_description = description;
  m_speaker_list = speaker_list;
  string speaker_name = m_net->get_speaker();
  
  /* Setup the speaker combo box */
  m_vbox.set_border_width(5);
  vector<string> popdown_strings;
  if (speaker_name != "") {
    popdown_strings.push_back(speaker_name);
  }

  if (m_speaker_list != NULL) {
    for (
      vector<Speaker>::iterator iter = m_speaker_list->speaker_list()->begin();
      iter != m_speaker_list->speaker_list()->end();
      ++iter)
    {
      /* TODO: only insert speakers appropriate for this particular crossover */
      if (speaker_name != (*iter).get_id_string()) {
        popdown_strings.push_back((*iter).get_id_string());
      }
    }
  }
  m_speaker_combo.set_popdown_strings(popdown_strings);
  m_speaker_combo.get_entry()->set_editable(false);
  m_vbox.pack_start(m_speaker_combo);
  
  m_vbox.pack_start(m_imp_corr_checkbutton);
  if (m_net->get_has_imp_corr() == true) {
    m_imp_corr_checkbutton.set_active(true);
  }

  m_vbox.pack_start(m_adv_imp_model_checkbutton);
  if (m_net->get_adv_imp_model() == 1) {
    m_adv_imp_model_checkbutton.set_active(true);
  }

  Gtk::HBox *hbox = manage(new Gtk::HBox());
  m_vbox.pack_start(*hbox);
  hbox->pack_start((*manage(new Gtk::Label(_("Damping: ")))));
  hbox->pack_start(m_damp_spinbutton);
  
  if (m_net->get_has_damp() == true) {
    /* Set damp value in dB here */
    double r_ser = m_net->get_damp_R1().get_value();
    Speaker speaker;
    if (m_speaker_list != NULL) {
      speaker = m_speaker_list->get_speaker_by_id_string(m_speaker_combo.get_entry()->get_text());
    } 
    m_damp_spinbutton.set_value(GSpeakers::round(20 * log10(r_ser / speaker.get_rdc() + 1)));
  }  
  hbox->pack_start((*manage(new Gtk::Label("dB"))));
  
  Gtk::Label *label;
  if (net->get_type() & NET_TYPE_HIGHPASS) {
    Gtk::Frame *frame = manage(new Gtk::Frame(""));
    
    frame->set_border_width(2);
    frame->set_shadow_type(Gtk::SHADOW_NONE);
    static_cast<Gtk::Label*>(frame->get_label_widget())->set_markup("<b>" + Glib::ustring(_("Highpass")) + "</b>");
    m_vbox.pack_start(*frame);
    Gtk::VBox *vbox = manage(new Gtk::VBox());
    vbox->set_border_width(12);
    frame->add(*vbox);
    
    /* Setup menus */
    m_higher_order_optionmenu = manage(new Gtk::OptionMenu());
    m_higher_order_menu = manage(new Gtk::Menu());
    Gtk::Menu::MenuList& menulist = m_higher_order_menu->items();
    menulist.push_back(Gtk::Menu_Helpers::MenuElem("1", bind<int, int>(mem_fun(*this, &FilterLinkFrame::on_order_selected), 1, 1)));
    menulist.push_back(Gtk::Menu_Helpers::MenuElem("2", bind<int, int>(mem_fun(*this, &FilterLinkFrame::on_order_selected), 1, 2)));
    menulist.push_back(Gtk::Menu_Helpers::MenuElem("3", bind<int, int>(mem_fun(*this, &FilterLinkFrame::on_order_selected), 1, 3)));
    menulist.push_back(Gtk::Menu_Helpers::MenuElem("4", bind<int, int>(mem_fun(*this, &FilterLinkFrame::on_order_selected), 1, 4)));
    m_higher_order_optionmenu->set_menu(*m_higher_order_menu);
    m_higher_type_optionmenu = manage(new Gtk::OptionMenu());
    m_higher_type_menu = manage(new Gtk::Menu());
    m_higher_order_optionmenu->set_history(m_net->get_highpass_order() - 1);
    on_order_selected(1, m_net->get_highpass_order());
    m_higher_type_optionmenu->set_menu(*m_higher_type_menu);
    
    /* menus ready */

    Gtk::HBox *hbox = manage(new Gtk::HBox());
    vbox->pack_start(*hbox);
    hbox->pack_start((*manage(new Gtk::Label(_("Order: "), Gtk::ALIGN_LEFT))));
    hbox->pack_start(*m_higher_order_optionmenu);
    vbox->pack_start(*m_higher_type_optionmenu);
    hbox = manage(new Gtk::HBox());
    hbox->pack_start((*manage(new Gtk::Label(_("Cutoff: ")))));
    m_higher_co_freq_spinbutton = manage(new Gtk::SpinButton(*(new Gtk::Adjustment(2000, 1, 20000, 1, 100))));
    hbox->pack_start(*m_higher_co_freq_spinbutton);
    label = manage(new Gtk::Label(" Hz"));
    label->set_alignment(Gtk::ALIGN_LEFT);
    hbox->pack_start(*label);
    vbox->pack_start(*hbox);
    set_family(m_higher_type_optionmenu, m_net->get_highpass_order(), m_net->get_highpass_family());
  }

  if (m_net->get_type() & NET_TYPE_LOWPASS) {
    Gtk::Frame *frame = manage(new Gtk::Frame(""));
    frame->set_border_width(2);
    frame->set_shadow_type(Gtk::SHADOW_NONE);
    static_cast<Gtk::Label*>(frame->get_label_widget())->set_markup("<b>" + Glib::ustring(_("Lowpass")) + "</b>");

    m_vbox.pack_start(*frame);
    Gtk::VBox *vbox = manage(new Gtk::VBox());
    frame->add(*vbox);
    vbox->set_border_width(12);
    
    /* Setup menus */
    m_lower_order_optionmenu = manage(new Gtk::OptionMenu());
    m_lower_order_menu = manage(new Gtk::Menu());
    Gtk::Menu::MenuList& menulist = m_lower_order_menu->items();
    menulist.push_back(Gtk::Menu_Helpers::MenuElem("1", bind<int, int>(mem_fun(*this, &FilterLinkFrame::on_order_selected), 0, 1)));
    menulist.push_back(Gtk::Menu_Helpers::MenuElem("2", bind<int, int>(mem_fun(*this, &FilterLinkFrame::on_order_selected), 0, 2)));
    menulist.push_back(Gtk::Menu_Helpers::MenuElem("3", bind<int, int>(mem_fun(*this, &FilterLinkFrame::on_order_selected), 0, 3)));
    menulist.push_back(Gtk::Menu_Helpers::MenuElem("4", bind<int, int>(mem_fun(*this, &FilterLinkFrame::on_order_selected), 0, 4)));
    m_lower_order_optionmenu->set_menu(*m_lower_order_menu);
    m_lower_type_optionmenu = manage(new Gtk::OptionMenu());
    m_lower_type_menu = manage(new Gtk::Menu());
    m_lower_order_optionmenu->set_history(m_net->get_lowpass_order() - 1);
    on_order_selected(0, m_net->get_lowpass_order());
    m_lower_type_optionmenu->set_menu(*m_lower_type_menu);
    
    /* menus ready */
    
    Gtk::HBox *hbox = manage(new Gtk::HBox());
    vbox->pack_start(*hbox);
    hbox->pack_start((*manage(new Gtk::Label(_("Order: ")))));
    hbox->pack_start(*m_lower_order_optionmenu);
    vbox->pack_start(*m_lower_type_optionmenu);
    hbox = manage(new Gtk::HBox());
    hbox->pack_start((*manage(new Gtk::Label(_("Cutoff: ")))));
    m_lower_co_freq_spinbutton = manage(new Gtk::SpinButton(*(new Gtk::Adjustment(2000, 1, 20000, 1, 100))));
    hbox->pack_start(*m_lower_co_freq_spinbutton);
    hbox->pack_start((*manage(new Gtk::Label(_("Hz")))));
    vbox->pack_start(*hbox);
    set_family(m_lower_type_optionmenu, m_net->get_lowpass_order(), m_net->get_lowpass_family());
  } 
    
  add(m_vbox);
  show_all();
  
  m_speaker_combo.get_entry()->signal_changed().connect(mem_fun(*this, &FilterLinkFrame::on_param_changed));
  if (net->get_type() & NET_TYPE_LOWPASS) {
    m_lower_type_optionmenu->signal_changed().connect(mem_fun(*this, &FilterLinkFrame::on_param_changed)); 
    m_lower_co_freq_spinbutton->signal_value_changed().connect(mem_fun(*this, &FilterLinkFrame::on_param_changed));
    m_lower_order_optionmenu->signal_changed().connect(mem_fun(*this, &FilterLinkFrame::on_param_changed));
  } 
  if (net->get_type() & NET_TYPE_HIGHPASS) {
    m_higher_order_optionmenu->signal_changed().connect(mem_fun(*this, &FilterLinkFrame::on_param_changed));
    m_higher_co_freq_spinbutton->signal_value_changed().connect(mem_fun(*this, &FilterLinkFrame::on_param_changed));
    m_higher_type_optionmenu->signal_changed().connect(mem_fun(*this, &FilterLinkFrame::on_param_changed));
  }  
  m_imp_corr_checkbutton.signal_toggled().connect(mem_fun(*this, &FilterLinkFrame::on_param_changed));
  m_damp_spinbutton.signal_value_changed().connect(mem_fun(*this, &FilterLinkFrame::on_param_changed));
  m_adv_imp_model_checkbutton.signal_toggled().connect(mem_fun(*this, &FilterLinkFrame::on_param_changed));
  signal_net_modified_by_user.connect(mem_fun(*this, &FilterLinkFrame::on_net_updated));
  signal_plot_crossover.connect(mem_fun(*this, &FilterLinkFrame::on_clear_and_plot));
  g_settings.defaultValueString("SPICECmdLine", "spice3");
  my_filter_plot_index = -1;
  signal_speakerlist_loaded.connect(mem_fun(*this, &FilterLinkFrame::on_speakerlist_loaded));
  g_settings.settings_changed.connect(mem_fun(*this, &FilterLinkFrame::on_settings_changed));
//  on_net_updated(m_net);
  init = false;
  enable_edit = true;
}

FilterLinkFrame::~FilterLinkFrame() 
{
  cout << "FilterLinkFrame: dtor" << endl;
}

void FilterLinkFrame::on_order_selected(int which, int order)
{
#ifdef OUTPUT_DEBUG
  cout << "FilterLinkFrame::on_order_selected, which = " << which << "   order = " << order << endl;
#endif
  Gtk::Menu::MenuList *menulist;// = &(m_lower_type_menu->items());
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

void FilterLinkFrame::on_settings_changed(const string& s)
{
  if (s == "DisableFilterAmp") {
    on_param_changed();
  }
}

void FilterLinkFrame::on_param_changed()
{
  if (enable_edit == true) {
#ifdef OUTPUT_DEBUG
    cout << "FilterLinkFrame::on_param_changed" << endl;
#endif
    enable_edit = false;
    Speaker speaker;
    if (m_speaker_list != NULL) {
      speaker = m_speaker_list->get_speaker_by_id_string(m_speaker_combo.get_entry()->get_text());
    }
    m_net->set_speaker(speaker.get_id_string());

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
          m_net->set_lowpass_family(NET_BUTTERWORTH);
          break;
        case NET_ORDER_2ND:
          switch (m_lower_type_optionmenu->get_history()) {
            case 0:  // bessel
              num_params = get_filter_params(NET_BESSEL,        NET_ORDER_2ND, NET_TYPE_LOWPASS);
              m_net->set_lowpass_family(NET_BESSEL);
              break;
            case 1:  // butterworth
              num_params = get_filter_params(NET_BUTTERWORTH,   NET_ORDER_2ND, NET_TYPE_LOWPASS);
              m_net->set_lowpass_family(NET_BUTTERWORTH);
              break;
            case 2:  // chebychev
              num_params = get_filter_params(NET_CHEBYCHEV,     NET_ORDER_2ND, NET_TYPE_LOWPASS);
              m_net->set_lowpass_family(NET_CHEBYCHEV);
              break;
            case 3:  // linkwitz-riley
              num_params = get_filter_params(NET_LINKWITZRILEY, NET_ORDER_2ND, NET_TYPE_LOWPASS);
              m_net->set_lowpass_family(NET_LINKWITZRILEY);
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
              m_net->set_lowpass_family(NET_BESSEL);
              break;
            case 1:   // butterworth
              num_params = get_filter_params(NET_BUTTERWORTH,   NET_ORDER_3RD, NET_TYPE_LOWPASS);
              m_net->set_lowpass_family(NET_BUTTERWORTH);
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
              m_net->set_lowpass_family(NET_BESSEL);
              break;
            case 1:  // butterworth
              num_params = get_filter_params(NET_BUTTERWORTH,   NET_ORDER_4TH, NET_TYPE_LOWPASS);
              m_net->set_lowpass_family(NET_BUTTERWORTH);
              break;
            case 2:  // gaussian
              num_params = get_filter_params(NET_GAUSSIAN,      NET_ORDER_4TH, NET_TYPE_LOWPASS);
              m_net->set_lowpass_family(NET_GAUSSIAN);
              break;
            case 3:  // legendre
              num_params = get_filter_params(NET_LEGENDRE,      NET_ORDER_4TH, NET_TYPE_LOWPASS);
              m_net->set_lowpass_family(NET_LEGENDRE);
              break;
            case 4:  // liner-phase
              num_params = get_filter_params(NET_LINEARPHASE,   NET_ORDER_4TH, NET_TYPE_LOWPASS);
              m_net->set_lowpass_family(NET_LINEARPHASE);
              break;
            case 5:  // linkwitz-riley
              num_params = get_filter_params(NET_LINKWITZRILEY, NET_ORDER_4TH, NET_TYPE_LOWPASS);
              m_net->set_lowpass_family(NET_LINKWITZRILEY);
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
          m_net->set_highpass_family(NET_BUTTERWORTH);
          break;
        case NET_ORDER_2ND:
          switch (m_higher_type_optionmenu->get_history()) {
            case 0:  // bessel
              num_params = get_filter_params(NET_BESSEL,        NET_ORDER_2ND, NET_TYPE_HIGHPASS);
              m_net->set_highpass_family(NET_BESSEL);
              break;
            case 1:  // butterworth
              num_params = get_filter_params(NET_BUTTERWORTH,   NET_ORDER_2ND, NET_TYPE_HIGHPASS);
              m_net->set_highpass_family(NET_BUTTERWORTH);
              break;
            case 2:  // chebychev
              num_params = get_filter_params(NET_CHEBYCHEV,     NET_ORDER_2ND, NET_TYPE_HIGHPASS);
              m_net->set_highpass_family(NET_CHEBYCHEV);
              break;
            case 3:  // linkwitz-riley
              num_params = get_filter_params(NET_LINKWITZRILEY, NET_ORDER_2ND, NET_TYPE_HIGHPASS);
              m_net->set_highpass_family(NET_LINKWITZRILEY);
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
              m_net->set_highpass_family(NET_BESSEL);
              break;
            case 1:   // butterworth
              num_params = get_filter_params(NET_BUTTERWORTH, NET_ORDER_3RD, NET_TYPE_HIGHPASS);
              m_net->set_highpass_family(NET_BUTTERWORTH);
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
              m_net->set_highpass_family(NET_BESSEL);
              break;
            case 1:  // butterworth
              num_params = get_filter_params(NET_BUTTERWORTH, NET_ORDER_4TH, NET_TYPE_HIGHPASS);
              m_net->set_highpass_family(NET_BUTTERWORTH);
              break;
            case 2:  // gaussian
              num_params = get_filter_params(NET_GAUSSIAN, NET_ORDER_4TH, NET_TYPE_HIGHPASS);
              m_net->set_highpass_family(NET_GAUSSIAN);
              break;
            case 3:  // legendre
              num_params = get_filter_params(NET_LEGENDRE, NET_ORDER_4TH, NET_TYPE_HIGHPASS);
              m_net->set_highpass_family(NET_LEGENDRE);
              break;
            case 4:  // liner-phase
              num_params = get_filter_params(NET_LINEARPHASE, NET_ORDER_4TH, NET_TYPE_HIGHPASS);
              m_net->set_highpass_family(NET_LINEARPHASE);
              break;
            case 5:  // linkwitz-riley
              num_params = get_filter_params(NET_LINKWITZRILEY, NET_ORDER_4TH, NET_TYPE_HIGHPASS);
              m_net->set_highpass_family(NET_LINKWITZRILEY);
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
    if (m_imp_corr_checkbutton.get_active() == true) {
      m_net->set_has_imp_corr(true);
      /* calc imp corr here */
      m_net->get_imp_corr_C().set_value((speaker.get_lvc() / 1000) / pow(speaker.get_rdc(), 2) * 1000000);
      m_net->get_imp_corr_C().set_unit("u");
      m_net->get_imp_corr_R().set_value(speaker.get_rdc());
    } else {
      m_net->set_has_imp_corr(false);
    }
    if (m_adv_imp_model_checkbutton.get_active() == true) {
      m_net->set_adv_imp_model(1);
    } else {
      m_net->set_adv_imp_model(0);
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
    
    if (g_settings.getValueBool("AutoUpdateFilterPlots") == true) {
      on_plot_crossover();
    }
    enable_edit = true;  
  }
}

void FilterLinkFrame::on_net_updated(Net *net)
{
  if (m_net->get_id() == net->get_id()) {
#ifdef OUPUTDEBUG
    cout << "FilterLinkFrame::on_net_updated" << endl;
#endif
    //enable_edit = false;
    
    if (g_settings.getValueBool("AutoUpdateFilterPlots") == true) {
      on_plot_crossover();
    }
/*    
    if (m_net->get_type() & NET_TYPE_LOWPASS) {
      int i = 0, index1 = 0;
      for (vector<GSpeakers::Point>::iterator iter = points.begin();
           iter != points.end();
           ++iter)
      {
        if ((*iter).get_y() > (-3 - m_damp_spinbutton.get_value())) {
          index1 = i;
        }
        i++;
      }
      points[index1 + 1].set_y(points[index1 + 1].get_y() + m_damp_spinbutton.get_value());
      points[index1].set_y(points[index1].get_y() + m_damp_spinbutton.get_value());

      double ydiff = points[index1 + 1].get_y() - points[index1].get_y();
      int xdiff = points[index1 + 1].get_x() - points[index1].get_x();
      double ytodbdiff = points[index1].get_y() + 3;
      m_lower_co_freq_spinbutton->set_value((ytodbdiff / ydiff) * xdiff + points[index1 + 1].get_x());
    } 
    if (m_net->get_type() & NET_TYPE_HIGHPASS) {
      int i = 0, index2 = 0;
      for (vector<GSpeakers::Point>::iterator iter = points.begin();
           iter != points.end();
           ++iter)
      {
        if ((*iter).get_y() < (-3 - m_damp_spinbutton.get_value())) {
          index2 = i;
        }
        i++;
      }
      index2++;
      points[index2 - 1].set_y(points[index2 - 1].get_y() + m_damp_spinbutton.get_value());
      points[index2].set_y(points[index2].get_y() + m_damp_spinbutton.get_value());
      
      double ydiff = points[index2 - 1].get_y() - points[index2].get_y();
      int xdiff = points[index2].get_x() - points[index2 - 1].get_x();
      double ytodbdiff = points[index2].get_y() + 3;
      m_higher_co_freq_spinbutton->set_value((ytodbdiff / ydiff) * xdiff + points[index2].get_x());
    }
*/
    //enable_edit = true;
  }
}

void FilterLinkFrame::on_clear_and_plot()
{
  my_filter_plot_index = -1;
  on_plot_crossover();
}

void FilterLinkFrame::on_speakerlist_loaded(SpeakerList *speaker_list)
{
#ifdef OUTPUT_DEBUG
  cout << "FilterLinkFrame::on_speakerlist_loaded" << endl;
#endif
  //if (speaker_list != NULL) {
    m_speaker_list = speaker_list;
  //}
  
  string speaker_name = m_net->get_speaker();
  /* Setup the speaker combo box */
  vector<string> popdown_strings;
  bool speaker_is_in_speakerlist = false;
  if (m_speaker_list != NULL) {
    for (
      vector<Speaker>::iterator iter = m_speaker_list->speaker_list()->begin();
      iter != m_speaker_list->speaker_list()->end();
      ++iter)
    {
      /* TODO: only insert speakers appropriate for this particular crossover */
      if (speaker_name != (*iter).get_id_string()) {
        popdown_strings.push_back((*iter).get_id_string());
      } else {
        speaker_is_in_speakerlist = true;
      }
    }
  }
  if (speaker_is_in_speakerlist == true) {
    popdown_strings.insert(popdown_strings.begin(), speaker_name);
  }
  m_speaker_combo.set_popdown_strings(popdown_strings);
}

void FilterLinkFrame::on_plot_crossover()
{
  /* Create spice code for this net */
  Speaker speaker;
  if (m_speaker_list != NULL) {
    speaker = m_speaker_list->get_speaker_by_id_string(m_speaker_combo.get_entry()->get_text());
  }
  string spice_filename;
  try {
    spice_filename = m_net->to_SPICE(speaker);
  } catch (GSpeakersException e) {
#ifdef OUTPUT_DEBUG
    cout << "FilterLinkFrame::on_plot_crossover: ERROR: " << e.what() << endl;
#endif
    Gtk::MessageDialog d(_("FilterLinkFrame::on_plot_crossover: ERROR: ") + Glib::ustring(e.what()), 
                         false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
    d.run();
  }
  
  /* run spice with created file */
  string cmd;
  if (g_settings.getValueBool("SPICEUseNGSPICE") == true) {
    cmd = g_settings.getValueString("SPICECmdLine") + " -b " + spice_filename + "> " + spice_filename + ".out";
  } else {
    cmd = g_settings.getValueString("SPICECmdLine") + " -b -o " + spice_filename + ".out " + spice_filename;
  }
#ifdef OUTPUT_DEBUG
  cout << "FilterLinkFrame::on_plot_crossover: running SPICE with \"" + cmd + "\"" << endl;
#endif
  system(cmd.c_str());
#ifdef OUTPUT_DEBUG
  cout << "FilterLinkFrame::on_plot_crossover: SPICE done" << endl;
#endif

  /* extract spice output into a vector */
  string spice_output_file = spice_filename + ".out";
  ifstream fin(spice_output_file.c_str());
  if (fin.good()) {
    bool output = false;
    int id;
    float f1, f2, f3;
    points.erase(points.begin(), points.end());
    while (!fin.eof()) {
      char *buffer = new char[100];
      fin.getline(buffer, 100, '\n');
      if (buffer[0] == '0') {
        output = true;
      }
      if (output == true) {
        // sscanf(buffer, "%d\t%f,\t%f\t%f", &id, &f1, &f2, &f3);
      
        id = atoi(buffer);

        strtok(buffer, "\t");
        char *substr_ptr = strtok(NULL, "\t");
          
        f1 = g_ascii_strtod(substr_ptr, NULL);
        substr_ptr = strtok(NULL, "\t");
        f2 = g_ascii_strtod(substr_ptr, NULL);
        substr_ptr = strtok(NULL, "\t");
        f3 = g_ascii_strtod(substr_ptr, NULL);

        GSpeakers::Point p(GSpeakers::round(f1), f3);
        points.push_back(p);
      }
      if ((buffer[0] == '3') && (buffer[1] == '0')) {
        output = false;
      }
      delete buffer;
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
    signal_add_crossover_plot(points, c, &my_filter_plot_index, m_net);
  
    if (enable_edit == true) {
      enable_edit = false;
      if (m_net->get_type() & NET_TYPE_LOWPASS) {
        int i = 0, index1 = 0;
        for (vector<GSpeakers::Point>::iterator iter = points.begin();
             iter != points.end();
             ++iter)
        {
          if ((*iter).get_y() > (-3 - m_damp_spinbutton.get_value())) {
            index1 = i;
          }
          i++;
        }
        points[index1 + 1].set_y(points[index1 + 1].get_y() + m_damp_spinbutton.get_value());
        points[index1].set_y(points[index1].get_y() + m_damp_spinbutton.get_value());
  
        double ydiff = points[index1 + 1].get_y() - points[index1].get_y();
        int xdiff = points[index1 + 1].get_x() - points[index1].get_x();
        double ytodbdiff = points[index1].get_y() + 3;
        m_lower_co_freq_spinbutton->set_value((ytodbdiff / ydiff) * xdiff + points[index1 + 1].get_x());
      } 
      if (m_net->get_type() & NET_TYPE_HIGHPASS) {
        bool done = false;
        int i = 0, index2 = 0;
        for (vector<GSpeakers::Point>::iterator iter = points.begin();
             iter != points.end();
             ++iter)
        {
          if (((*iter).get_y() < (-3 - m_damp_spinbutton.get_value())) && (done == false)) {
            index2 = i;
          } else {
            done = true;
          }
          i++;
        }
        index2++;
        points[index2 - 1].set_y(points[index2 - 1].get_y() + m_damp_spinbutton.get_value());
        points[index2].set_y(points[index2].get_y() + m_damp_spinbutton.get_value());
      
        double ydiff = points[index2 - 1].get_y() - points[index2].get_y();
        int xdiff = points[index2].get_x() - points[index2 - 1].get_x();
        double ytodbdiff = points[index2].get_y() + 3;
        m_higher_co_freq_spinbutton->set_value((ytodbdiff / ydiff) * xdiff + points[index2].get_x());
      }
      enable_edit = true;
    }
  }
}   

/* 
 * Numerical coefficients for the filter principles 
 */
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
              nums.push_back(0.0912);
              nums.push_back(0.2756);
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

void FilterLinkFrame::set_family(Gtk::OptionMenu *option_menu, int order, int family)
{
#ifdef OUTPUT_DEBUG
  cout << "FilterLinkFrame::set_family: order = " << order << ", family = " << family << endl;
#endif
  switch (order) {
    case NET_ORDER_2ND:
      switch (family) {
        case NET_BESSEL:
          option_menu->set_history(0);
          break;
        case NET_BUTTERWORTH:
          option_menu->set_history(1);
          break;
        case NET_CHEBYCHEV:
          option_menu->set_history(2);
          break;
        case NET_LINKWITZRILEY:
          option_menu->set_history(3);
          break;
      }
      break;
    case NET_ORDER_3RD:
      switch (family) {
        case NET_BUTTERWORTH:
          option_menu->set_history(1);
          break;
        case NET_BESSEL:
          option_menu->set_history(0);
          break;
      }
      break;
    case NET_ORDER_4TH:
      switch (family) {
        case NET_BESSEL:
          option_menu->set_history(0);
          break;
        case NET_BUTTERWORTH:
          option_menu->set_history(1);
          break;
        case NET_GAUSSIAN:
          option_menu->set_history(2);
          break;
        case NET_LEGENDRE:
          option_menu->set_history(3);
          break;
        case NET_LINEARPHASE:
          option_menu->set_history(4);
          break;
        case NET_LINKWITZRILEY:
          option_menu->set_history(5);
          break;
      }
      break;
  }
}
