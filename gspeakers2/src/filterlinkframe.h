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

#ifndef __FILTER_LINK_FRAME_H
#define __FILTER_LINK_FRAME_H

#include <gtkmm.h>
#include "crossover.h"
#include "speakerlist.h"

using namespace std;
using namespace SigC;

#define NET_BESSEL        1
#define NET_BUTTERWORTH   2
#define NET_LINKWITZRILEY 3
#define NET_CHEBYCHEV     4
#define NET_GAUSSIAN      5
#define NET_LEGENDRE      6
#define NET_LINEARPHASE   7

class FilterLinkFrame : public Gtk::Frame
{
public:
  FilterLinkFrame(Net *net, const string& description, SpeakerList *speaker_list);
  
private:
  /* callbacks */
  /* which == 0: lower, which == 1: higher, order = filter order */
  void on_order_selected(int which, int order);
  void on_param_changed();
  void on_net_updated(Net *net);

  Gtk::Adjustment    adj;

  Gtk::VBox          m_vbox;
  Gtk::Combo         m_speaker_combo;
  Gtk::CheckButton   m_enable_checkbutton;
  
  /* For lowpass filter */
  Gtk::OptionMenu    *m_lower_order_optionmenu, *m_lower_type_optionmenu;
  Gtk::SpinButton    *m_lower_co_freq_spinbutton;
  Gtk::Menu          *m_lower_order_menu, *m_lower_type_menu;
  
  /* For highpass filter */
  Gtk::OptionMenu    *m_higher_order_optionmenu, *m_higher_type_optionmenu;
  Gtk::SpinButton    *m_higher_co_freq_spinbutton;
  Gtk::Menu          *m_higher_order_menu, *m_higher_type_menu;
  
  /* For both */
  Gtk::CheckButton   m_inv_pol_checkbutton;
  Gtk::SpinButton    m_damp_spinbutton;
  Gtk::CheckButton   m_imp_corr_checkbutton;
  
  /* net_name_type = NET_BESSEL, ..., net_order = NET_ORDER_1ST, ..., net_type = NET_TYPE_LOWPASS, NET_TYPE_HIGHPASS */
  vector<double> get_filter_params(int net_name_type, int net_order, int net_type);
  
  Net *m_net;
  string m_description;
  SpeakerList *m_speaker_list;
  bool enable_edit;
  
  /* Helper function */
  int round( double x );
};

#endif
