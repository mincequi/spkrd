/*
  crossoverhistory Copyright (C) 2002 Daniel Sundberg

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

#include <gtkmm.h>
#include "common.h"
#include "crossoverlist.h"

using namespace SigC;
using namespace std;

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
class CrossoverHistory : public Gtk::Frame
{
public:
  CrossoverHistory();
  virtual ~CrossoverHistory();

protected:
  /* callbacks */
  void on_open_xml();
  void on_append_xml();
  void on_open_ok(Gtk::FileSelection *f);
  void on_append_ok(Gtk::FileSelection *f);
  void on_selection_changed();
  void on_new_copy();
  void on_new();
  void on_new_from_menu(int type);
  void on_new_xml();
  void on_save();
  void on_save_as();
  void on_save_as_ok(Gtk::FileSelection *f);
  void on_remove();
  bool on_close(GdkEventAny *event);
  void on_part_modified();  
  bool on_delete_event(GdkEventAny *event);
  
  /* Helper member functions */
  virtual void create_model();
  virtual void add_columns();
  virtual void add_items(CrossoverList clist);
  virtual void liststore_add_item(Crossover foo);

  //Member widgets:
  Gtk::Table m_Table;
  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::TreeView m_TreeView;
  Glib::RefPtr<Gtk::ListStore> m_refListStore;
  Gtk::Button m_NewCopyButton, m_NewXmlButton, m_AppendXmlButton, m_OpenXmlButton, m_NewButton;
  Gtk::Button m_SaveButton, m_SaveAsButton, m_RemoveButton;
  
  Gtk::FileSelection *f_open, *f_save_as, *f_append;
  
  /* Data containers */
  CrossoverList m_crossover_list;

  /* This is used in the treemodel */
  struct ModelColumns : public Gtk::TreeModelColumnRecord
  {
    Gtk::TreeModelColumn<int>           id;
    Gtk::TreeModelColumn<int>           type;
    Gtk::TreeModelColumn<Glib::ustring> id_string;

    ModelColumns() { add(id); add(type); add(id_string); }
  };
  ModelColumns m_columns;
  
  /* Additional member variables */
  string m_filename;
  bool new_xml_pressed;
  int index;
};

#endif
