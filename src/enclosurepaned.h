/*
  $Id$

  enclosurepaned Copyright (C) 2002 Daniel Sundberg

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

#ifndef __ENCLOSURE_PANED_H
#define __ENCLOSURE_PANED_H

#include "boxeditor.h"
#include "boxhistory.h"
#include "common.h"
#include "gspeakersboxplot.h"
#include "plothistory.h"
#include <gtkmm.h>

using namespace std;
using namespace sigc;

class EnclosurePaned : public Gtk::HPaned {
public:
  EnclosurePaned();
  virtual ~EnclosurePaned();
  Gtk::Menu& get_menu();
  Gtk::Widget& get_toolbar();

protected:
  /* Callbacks */
  void on_settings_changed(const string&);
  void set_save_state(bool b);

  void on_plot_selected(int);
  void on_remove_boxplot(int);
  void on_add_plot(Box*, Speaker*, Gdk::Color&);

private:
  BoxEditor box_editor;
  BoxHistory box_history;
  PlotHistory plot_history;
  GSpeakersBoxPlot box_plot;

  Gtk::VPaned m_edit_vpaned;
  Gtk::VPaned m_plot_vpaned;

  Gtk::Menu m_menu;
  Gtk::HandleBox m_toolbar;
  Gtk::Toolbar* m_tbar;

  int nof_plots;
  bool plot_selected;
};

#endif
