/*
  crossoverwizard Copyright (C) 2002 Daniel Sundberg

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

#include "crossoverwizard.h"

CrossoverWizard::CrossoverWizard() :
  Gtk::Frame("Crossover wizard"),
  m_scrolled_window(),
  m_vbox(),
  m_outer_vbox()

{
  signal_crossover_selected.connect(slot(*this, &CrossoverWizard::on_crossover_selected));
  m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  //m_scrolled_window.set_shadow_type(Gtk::SHADOW_NONE);
  add(m_scrolled_window);
  m_scrolled_window.add(m_outer_vbox);
  m_outer_vbox.pack_start(m_vbox);
  m_outer_vbox.set_border_width(5);
  show_all();
  signal_speakerlist_loaded.connect(slot(*this, &CrossoverWizard::on_speaker_list_loaded));
}

CrossoverWizard::~CrossoverWizard() 
{

}

void CrossoverWizard::on_crossover_selected(Crossover *crossover)
{
  cout << "CrossoverWizard::on_crossover_selected" << endl;
  m_vbox.children().erase(m_vbox.children().begin(), m_vbox.children().end());
  for (vector<Net>::iterator iter = crossover->networks()->begin();
       iter != crossover->networks()->end();
       ++iter)
  {
    cout << iter->get_type() << endl;
    FilterLinkFrame *link1 = manage(new FilterLinkFrame((Net *)iter, "filter", &m_speaker_list));
    m_vbox.pack_start(*link1);
  }
  //signal_plot_crossover();
}

void CrossoverWizard::on_speaker_list_loaded(string speaker_list_filename)
{
  cout << "CrossoverWizard::on_speaker_list_loaded" << endl;
  m_speaker_list = SpeakerList(speaker_list_filename);
}

void CrossoverWizard::on_button_plot_clicked()
{
  cout << "CrossoverWiard::on_button_plot_cliecked" << endl;
}

void CrossoverWizard::on_button_update_clicked()
{
  cout << "CrossoverWiard::on_button_update_cliecked" << endl;
}
