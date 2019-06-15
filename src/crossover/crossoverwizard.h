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

#include "common.h"
#include "filterlinkframe.h"
#include "speaker_list.hpp"

#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>

class CrossoverWizard : public Gtk::ScrolledWindow {
public:
  CrossoverWizard();

private:
  /* Callbacks */
  void on_speaker_list_loaded(speaker_list*);
  void on_button_plot_clicked();
  void on_button_update_clicked();

  void on_crossover_selected(Crossover* crossover);

private:
  Gtk::VBox m_vbox;
  speaker_list* m_speaker_list = nullptr;
};

#endif
