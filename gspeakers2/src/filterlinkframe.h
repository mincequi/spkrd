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

using namespace std;
using namespace SigC;

class FilterLinkFrame : public Gtk::Frame
{
public:
  FilterLinkFrame(int net_type, string description);
  
private:
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
  
  int m_net_type;
  string m_description;
};

#endif
