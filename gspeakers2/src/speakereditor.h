/*
  $Id$

  speakereditor Copyright (C) 2002 Daniel Sundberg

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

#ifndef __GSPEAKERS_SPEAKEREDITOR
#define __GSPEAKERS_SPEAKEREDITOR

#include <sigc++/object.h>
#include <gtkmm/widget.h>
#include <gtkmm/fileselection.h>
#include <glibmm/ustring.h>
#include "speakerlist.h"
#include "driverfreqrespplot.h"

using namespace SigC;
using namespace std;

class Speaker_ListStore : public SigC::Object
{
public:
  Speaker_ListStore();
  virtual ~Speaker_ListStore();
  Gtk::Widget& get_treeview_table();
  Gtk::Widget& get_editor_table();
  Gtk::Widget& get_plot();
  Gtk::Menu& get_menu();
  Gtk::Widget& get_toolbar();
  
protected:
  bool updating_entries;

  /* callbacks */
  void on_cell_fixed_toggled(const Glib::ustring& path_string);
  void on_open_xml();
  void on_append_xml();
  void on_open_ok(Gtk::FileSelection *f);
  void on_append_ok(Gtk::FileSelection *f);
  void on_clear();
  void on_selection_changed();
  void on_new();
  void on_new_xml();
  void on_entry_changed(int i);
  void on_save();
  void on_save_as();
  void on_save_as_ok(Gtk::FileSelection *f);
  void on_remove();
  void on_close();
  void on_edit_freq_resp();
  void on_browse_freq_resp();
  void on_settings_changed(const string&);
  void type_cell_data_func(Gtk::CellRenderer *cell, const Gtk::TreeModel::iterator& iter);
  void qts_cell_data_func(Gtk::CellRenderer *cell, const Gtk::TreeModel::iterator& iter);
  void fs_cell_data_func(Gtk::CellRenderer *cell, const Gtk::TreeModel::iterator& iter);
  void vas_cell_data_func(Gtk::CellRenderer *cell, const Gtk::TreeModel::iterator& iter);
  void rdc_cell_data_func(Gtk::CellRenderer *cell, const Gtk::TreeModel::iterator& iter);
  void lvc_cell_data_func(Gtk::CellRenderer *cell, const Gtk::TreeModel::iterator& iter);
  void qms_cell_data_func(Gtk::CellRenderer *cell, const Gtk::TreeModel::iterator& iter);
  void qes_cell_data_func(Gtk::CellRenderer *cell, const Gtk::TreeModel::iterator& iter);
  void imp_cell_data_func(Gtk::CellRenderer *cell, const Gtk::TreeModel::iterator& iter);
  void sens_cell_data_func(Gtk::CellRenderer *cell, const Gtk::TreeModel::iterator& iter);
  void mmd_cell_data_func(Gtk::CellRenderer *cell, const Gtk::TreeModel::iterator& iter);
  void ad_cell_data_func(Gtk::CellRenderer *cell, const Gtk::TreeModel::iterator& iter);	
  void bl_cell_data_func(Gtk::CellRenderer *cell, const Gtk::TreeModel::iterator& iter);
  void rms_cell_data_func(Gtk::CellRenderer *cell, const Gtk::TreeModel::iterator& iter);
  void cms_cell_data_func(Gtk::CellRenderer *cell, const Gtk::TreeModel::iterator& iter);	
		
  /* Helper funtions */
  virtual void create_model();
  virtual void add_columns();
  virtual void liststore_add_item(Speaker foo);
  void draw_imp_plot(Speaker& s, bool update = false);
  
  //Member widgets:
  Gtk::Table m_TreeViewTable;
  Gtk::Table m_Table;
  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::Label m_Label;
  Gtk::TreeView m_TreeView;
  Glib::RefPtr<Gtk::ListStore> m_refListStore;
  Gtk::Button m_EditFreqRespButton, m_BrowseFreqRespButton;
  Gtk::Entry m_IdStringEntry, m_QtsEntry, m_FsEntry, m_VasEntry, m_RdcEntry, m_LvcEntry, m_QmsEntry, m_QesEntry;
  Gtk::Entry m_ImpEntry, m_SensEntry, m_FreqRespFileEntry, m_ImpRespFileEntry;
  Gtk::CheckButton m_BassCheckButton, m_MidrangeCheckButton, m_TweeterCheckButton;
  Gtk::Entry m_MmdEntry, m_AdEntry, m_BlEntry, m_RmsEntry, m_CmsEntry;
  Gtk::VBox m_vbox, m_treeview_vbox, m_inner_vbox, m_inner_treeview_vbox;
  Gtk::Frame m_treeview_frame, m_editor_frame;
  
  Gtk::FileSelection *f_open, *f_save_as, *f_append;
  Gtk::Menu m_menu;
  
  Gtk::HandleBox toolbar;
  Gtk::Toolbar *tbar;
  
  /* Data container */
  SpeakerList *m_speaker_list;
  DriverFreqRespPlot plot;
  
  struct ModelColumns : public Gtk::TreeModelColumnRecord
  {
    Gtk::TreeModelColumn<int>           id;
    Gtk::TreeModelColumn<int>           type;
    Gtk::TreeModelColumn<Glib::ustring> id_string;
    Gtk::TreeModelColumn<double>        qts;
    Gtk::TreeModelColumn<double>        fs;
    Gtk::TreeModelColumn<double>        vas;
    Gtk::TreeModelColumn<double>        rdc;
    Gtk::TreeModelColumn<double>        lvc;
    Gtk::TreeModelColumn<double>        qms;
    Gtk::TreeModelColumn<double>        qes;
    Gtk::TreeModelColumn<double>        imp;
    Gtk::TreeModelColumn<double>        sens;
    Gtk::TreeModelColumn<double>        mmd;
    Gtk::TreeModelColumn<double>        ad;
    Gtk::TreeModelColumn<double>        bl;
    Gtk::TreeModelColumn<double>        rms;
    Gtk::TreeModelColumn<double>        cms;
    

    ModelColumns() { add(id); add(type); add(id_string); add(qts); add(fs); add(vas); 
                     add(rdc); add(lvc); add(qms); add(qes); add(imp); add(sens); 
                     add(mmd); add(ad);  add(bl); add(rms); add(cms); }
  };
  
  /* vaious helper varaibles */
  bool changed, new_xml_pressed;
  int index;
  ModelColumns m_columns;
  //Glib::RefPtr<Gtk::TreeSelection> old_refSelection;
  void set_entries_sensitive(bool value);
  string m_filename;
  bool m_modified;
};

#endif
