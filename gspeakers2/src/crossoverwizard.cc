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
#ifdef OUTPUT_DEBUG
  cout << "CrossoverWizard::on_crossover_selected" << endl;
#endif
  //  delete m_vbox.children()[0].get_widget();
  
  /* We want to make sure the content of m_vbox get deleted */
  //m_vbox.children().erase(m_vbox.children().begin(), m_vbox.children().end());
  if (m_vbox.children().size() > 0) {
    for (int i = m_vbox.children().size() - 1; i >= 0; i--) {
      delete m_vbox.children()[i].get_widget();
    }
  }

  for (vector<Net>::iterator iter = crossover->networks()->begin();
       iter != crossover->networks()->end();
       ++iter)
  {
    cout << "net_id: " << iter->get_id() << endl;
    //FilterLinkFrame *link1 = manage(new FilterLinkFrame(&(*iter), "filter", m_speaker_list));
    FilterLinkFrame *link1 = new FilterLinkFrame(&(*iter), "filter", m_speaker_list);
    m_vbox.pack_start(*link1);
  }
}

void CrossoverWizard::on_speaker_list_loaded(SpeakerList *speaker_list)
{
#ifdef OUTPUT_DEBUG
  cout << "CrossoverWizard::on_speaker_list_loaded" << endl;
#endif
  //m_speaker_list = SpeakerList(speaker_list_filename);
  m_speaker_list = speaker_list;
}

void CrossoverWizard::on_button_plot_clicked()
{
#ifdef OUTPUT_DEBUG
  cout << "CrossoverWiard::on_button_plot_cliecked" << endl;
#endif
}

void CrossoverWizard::on_button_update_clicked()
{
#ifdef OUTPUT_DEBUG
  cout << "CrossoverWiard::on_button_update_cliecked" << endl;
#endif
}
