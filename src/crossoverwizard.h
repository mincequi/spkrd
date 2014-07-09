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

#ifndef __CROSSOVER_WIZARD_H
#define __CROSSOVER_WIZARD_H

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include "filterlinkframe.h"
#include "common.h"
#include "speakerlist.h"

using namespace std;
using namespace sigc;

class CrossoverWizard : public Gtk::ScrolledWindow
{
public:
  CrossoverWizard();
  ~CrossoverWizard();
private:
  /* Callbacks */
  void on_speaker_list_loaded(SpeakerList *);
  void on_button_plot_clicked();
  void on_button_update_clicked();

  //  Gtk::ScrolledWindow m_scrolled_window;
  Gtk::VBox m_vbox;
  void on_crossover_selected(Crossover *crossover);
  SpeakerList *m_speaker_list;
};

#endif
