/*
  $Id$

  boxhistory Copyright (C) 2002 Daniel Sundberg

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

#ifndef __GSPEAKERS_BOXHISTORY
#define __GSPEAKERS_BOXHISTORY

#include <gtkmm.h>
#include "common.h"
#include "boxlist.h"

using namespace SigC;
using namespace std;

extern Signal1<void, bool> signal_enclosure_set_save_state;

/*
 * This class implements a TreeView connected with a ListStore
 * It is pretty much ripped off the liststore example in the 
 * gtk-demo program so you can study this example to learn a 
 * little bit more about how to use the TreeView.
 *
 * The list contains a list of all enclosures...you can select 
 * to plot, not plot or whatever for each enclosure.
 */
class BoxHistory : public Gtk::Frame
{
public:
  BoxHistory();
  virtual ~BoxHistory();

  /* public callbacks, used in container classes from toolbar/menu events */
  void on_new();
  void on_new_copy();
  void on_open_xml();
  void on_append_xml();
  void on_new_xml();
  void on_save();
  void on_save_as();
  void on_remove();
  void on_delete_plot();
  
protected:
  /* callbacks */
  void on_open_ok(Gtk::FileSelection *f);
  void on_append_ok(Gtk::FileSelection *f);
  void on_selection_changed();
  void on_save_as_ok(Gtk::FileSelection *f);
  void on_box_modified(Box *box);  
  void on_add_plot(Box *box, Speaker *speaker);
  void on_append_box(Box *box);
  void on_add_to_boxlist(Box *b);
  bool on_delete_event(GdkEventAny* event);
  void on_plot_selected(int i);

  /* Helper member functions */
  virtual void create_model();
  virtual void add_columns();
  virtual void liststore_add_item(Box box);
  
  //Member widgets:
  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::TreeView m_TreeView;
  Glib::RefPtr<Gtk::ListStore> m_refListStore;
  Gtk::VBox m_vbox;
  
  Gtk::FileSelection *f_open, *f_save_as, *f_append;

  /* Data containers */
  BoxList m_box_list;

  /* This is used in the treemodel */
  struct ModelColumns : public Gtk::TreeModelColumnRecord
  {
    Gtk::TreeModelColumn<int>           type;
    Gtk::TreeModelColumn<Glib::ustring> id_string;
    Gtk::TreeModelColumn<double>        vb1;
    Gtk::TreeModelColumn<double>        fb1;
    Gtk::TreeModelColumn<double>        vb2;
    Gtk::TreeModelColumn<double>        fb2;
    
    ModelColumns() { add(type); add(id_string); add(vb1); add(vb2); add(fb1); add(fb2); }
  };
  ModelColumns m_columns;
  
  /* Additional member variables */
  string m_filename;
  bool new_xml_pressed;
  int index;
  int selected_plot;
};

#endif
