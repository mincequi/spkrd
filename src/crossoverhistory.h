/*
  $Id$

  crossoverhistory Copyright (C) 2002-2004 Daniel Sundberg

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

#ifndef __GSPEAKERS_CROSSOVERHISTORY
#define __GSPEAKERS_CROSSOVERHISTORY

#include "common.h"
#include "crossoverlist.h"

#include <gtkmm.h>

extern sigc::signal1<void, bool> signal_crossover_set_save_state;

/*
 * This class implements a TreeView connected with a ListStore
 * It is pretty much ripped off the liststore example in the
 * gtk-demo program so you can study this example to learn a
 * little bit more about how to use the TreeView.
 *
 * When we select a new crossover the callback function will
 * emit the signal_crossover_selected so that other views that
 * depend on the currently selected crossover can update thier
 * data.
 */
class CrossoverHistory : public Gtk::Frame {
public:
  CrossoverHistory();
  virtual ~CrossoverHistory();
  /* public callbacks, for use in container class */
  void on_open_xml();
  void on_append_xml();
  void on_new_copy();
  void on_new();
  void on_new_from_menu(int type);
  void on_new_xml();
  void on_save();
  void on_save_as();
  void on_remove();
  void select_first_row();

protected:
  /* callbacks */
  void open_xml(const std::string& filename);
  void append_xml(const std::string& filename);
  void save_as_xml(const std::string& filename);
  void on_selection_changed();
  void on_part_modified();
  void on_net_modified_by_wizard(Net* net);
  void on_net_modified_by_user();
  void on_new_crossover_menu_action(int);
  void on_plot_crossover();
  void on_save_open_files();
  void type_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);

  /* Helper member functions */
  virtual void create_model();
  virtual void add_columns();
  virtual void liststore_add_item(Crossover foo);

  // Member widgets:
  Gtk::Widget* m_evbox;
  Gtk::Label* m_frame_label;
  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::TreeView m_TreeView;
  Glib::RefPtr<Gtk::ListStore> m_refListStore;

  Gtk::FileSelection *f_open, *f_save_as, *f_append;

  /* Data containers */
  CrossoverList m_crossover_list;

  /* This is used in the treemodel */
  struct ModelColumns : public Gtk::TreeModelColumnRecord {
    Gtk::TreeModelColumn<int> id;
    Gtk::TreeModelColumn<int> type;
    Gtk::TreeModelColumn<Glib::ustring> id_string;

    ModelColumns() {
      add(id);
      add(type);
      add(id_string);
    }
  };
  ModelColumns m_columns;

  /* Additional member variables */
  std::string m_filename;
  bool new_xml_pressed;
  int index;
};

#endif
