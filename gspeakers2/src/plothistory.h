/*
  plothistory Copyright (C) 2002 Daniel Sundberg

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

#ifndef __GSPEAKERS_PLOTHISTORY
#define __GSPEAKERS_PLOTHISTORY

#include <gtkmm.h>
#include "common.h"
#include "boxlist.h"
#include "speakerlist.h"
#include "gspeakerscolor.h"

using namespace SigC;
using namespace std;

/*
 * This class implements a TreeView connected with a ListStore
 * It is pretty much ripped off the liststore example in the 
 * gtk-demo program so you can study this example to learn a 
 * little bit more about how to use the TreeView.
 *
 * The list contains a list of all enclosures...you can select 
 * to plot, not plot or whatever for each enclosure.
 */
class PlotHistory : public Gtk::Window
{
public:
  PlotHistory();
  virtual ~PlotHistory();

protected:
  /* callbacks */
  void on_selection_changed();
  void on_remove();
  void on_box_modified(Box *box);  
  void on_add_plot(Box *box, Speaker *speaker, Gdk::Color&);
  void on_cell_plot_toggled(const Glib::ustring& path_string);
  bool on_delete_event(GdkEventAny *);

  /* Helper member functions */
  virtual void create_model();
  virtual void add_columns();
  virtual void liststore_add_item(Box box, Speaker spk, Gdk::Color &);
  
  //Member widgets:
  Gtk::Table m_Table;
  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::TreeView m_TreeView;
  Glib::RefPtr<Gtk::ListStore> m_refListStore;
  Gtk::Button m_RemoveButton;
  
 
  /* Data containers */
  BoxList m_box_list;
  SpeakerList m_speaker_list;

  GSpeakersColor m_color_list;

  /* This is used in the treemodel */
  struct ModelColumns : public Gtk::TreeModelColumnRecord
  {
    Gtk::TreeModelColumn<int>           id;
    Gtk::TreeModelColumn<int>           type;
    Gtk::TreeModelColumn<Glib::ustring> id_string;
    Gtk::TreeModelColumn<Glib::ustring> speaker_string;
    Gtk::TreeModelColumn<string>        color;
    Gtk::TreeModelColumn<bool>          view_plot;
    Gtk::TreeModelColumn<double>        vb1;
    Gtk::TreeModelColumn<double>        fb1;
    Gtk::TreeModelColumn<double>        vb2;
    Gtk::TreeModelColumn<double>        fb2;
    Gtk::TreeModelColumn<Gdk::Color>    color_;
    Gtk::TreeModelColumn<Pango::Weight>  weight_;
    
    ModelColumns() { add(id); add(type); add(id_string); add(color); add(view_plot); add(vb1); add(vb2); add(fb1); add(fb2); add(color_); add(weight_); add(speaker_string);}
  };
  ModelColumns m_columns;
  
  /* Additional member variables */
  int index;
  int round(double);

};

#endif
