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

CrossoverWizard::CrossoverWizard() : Gtk::ScrolledWindow(), m_vbox() {
  using namespace sigc;

  m_speaker_list = NULL;
  // set_border_width(2);
  //  set_shadow_type(Gtk::SHADOW_NONE);
  //  static_cast<Gtk::Label*>(get_label_widget())->set_markup("<b>" + Glib::ustring(_("Crossover
  //  wizard")) + "</b>");
  signal_crossover_selected.connect(mem_fun(*this, &CrossoverWizard::on_crossover_selected));
  set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  set_border_width(2);
  add(m_vbox);
  show_all();
  signal_speakerlist_loaded.connect(mem_fun(*this, &CrossoverWizard::on_speaker_list_loaded));
}

CrossoverWizard::~CrossoverWizard() {}

void CrossoverWizard::on_crossover_selected(Crossover* crossover) {
#ifdef OUTPUT_DEBUG
  std::cout << "CrossoverWizard::on_crossover_selected" << std::endl;
#endif
  //  delete m_vbox.children()[0].get_widget();

  /* We want to make sure the content of m_vbox get deleted */
  // m_vbox.children().erase(m_vbox.children().begin(), m_vbox.children().end());
  if (m_vbox.children().size() > 0) {
    for (int i = m_vbox.children().size() - 1; i >= 0; i--) {
      delete m_vbox.children()[i].get_widget();
    }
  }

  int index = 0;
  int type = crossover->get_type();
  for (vector<Net>::iterator iter = crossover->networks()->begin();
       iter != crossover->networks()->end(); ++iter, index++) {
    /* The following code is here because we want pretty labels for what was each subfilter is there
     * for */
    if (type == CROSSOVER_TYPE_TWOWAY) {
      if (index == 0) {
        FilterLinkFrame* link1 =
            new FilterLinkFrame(&(*iter), _("Woofer/midrange filter"), m_speaker_list);
        m_vbox.pack_start(*link1);
      } else {
        FilterLinkFrame* link1 = new FilterLinkFrame(&(*iter), _("Tweeter filter"), m_speaker_list);
        m_vbox.pack_start(*link1);
      }
    } else if (type == CROSSOVER_TYPE_THREEWAY) {
      if (index == 0) {
        FilterLinkFrame* link1 = new FilterLinkFrame(&(*iter), _("Woofer filter"), m_speaker_list);
        m_vbox.pack_start(*link1);
      } else if (index == 1) {
        FilterLinkFrame* link2 =
            new FilterLinkFrame(&(*iter), _("Midrange filter"), m_speaker_list);
        m_vbox.pack_start(*link2);
      } else {
        FilterLinkFrame* link3 = new FilterLinkFrame(&(*iter), _("Tweeter filter"), m_speaker_list);
        m_vbox.pack_start(*link3);
      }
    } else if (type == (CROSSOVER_TYPE_TWOWAY | CROSSOVER_TYPE_LOWPASS)) {
      if (index == 0) {
        FilterLinkFrame* link1 = new FilterLinkFrame(&(*iter), _("Woofer filter"), m_speaker_list);
        m_vbox.pack_start(*link1);
      } else if (index == 1) {
        FilterLinkFrame* link2 =
            new FilterLinkFrame(&(*iter), _("Woofer/midrange filter"), m_speaker_list);
        m_vbox.pack_start(*link2);
      } else {
        FilterLinkFrame* link3 = new FilterLinkFrame(&(*iter), _("Tweeter filter"), m_speaker_list);
        m_vbox.pack_start(*link3);
      }
    } else {
      // FilterLinkFrame *link1 = manage(new FilterLinkFrame(&(*iter), _("filter"),
      // m_speaker_list));
      FilterLinkFrame* link1 = new FilterLinkFrame(&(*iter), _("filter"), m_speaker_list);
      m_vbox.pack_start(*link1);
    }
  }
}

void CrossoverWizard::on_speaker_list_loaded(SpeakerList* speaker_list) {
#ifdef OUTPUT_DEBUG
  std::cout << "CrossoverWizard::on_speaker_list_loaded" << std::endl;
#endif
  // m_speaker_list = SpeakerList(speaker_list_filename);
  m_speaker_list = speaker_list;
}

void CrossoverWizard::on_button_plot_clicked() {
#ifdef OUTPUT_DEBUG
  std::cout << "CrossoverWiard::on_button_plot_cliecked" << std::endl;
#endif
}

void CrossoverWizard::on_button_update_clicked() {
#ifdef OUTPUT_DEBUG
  std::cout << "CrossoverWiard::on_button_update_cliecked" << std::endl;
#endif
}
