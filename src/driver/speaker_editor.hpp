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

#include "driverfreqrespplot.h"
#include "speaker_list.hpp"

#include <glibmm/ustring.h>

#include <gtkmm/box.h>
#include <gtkmm/cellrenderer.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/handlebox.h>
#include <gtkmm/liststore.h>
#include <gtkmm/menu.h>
#include <gtkmm/table.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include <gtkmm/widget.h>
#include <gtkmm/separatormenuitem.h>

#include <sigc++/sigc++.h>

#include <memory>

class speaker_editor : public sigc::trackable
{
public:
    speaker_editor();

    virtual ~speaker_editor() = default;

    Gtk::Widget& get_treeview_table() noexcept { return m_treeview_vbox; }

    Gtk::Widget& get_editor_table() noexcept { return m_vbox; }

    Gtk::Widget& get_plot() noexcept { return plot; }

    Gtk::MenuItem& get_menu();

    Gtk::Toolbar& get_toolbar();

protected:
    bool open_xml(const std::string& filename);
    void append_xml(const std::string& filename);

    void on_cell_fixed_toggled(const Glib::ustring& path_string);

    void on_open_xml();

    void on_append_xml();

    void on_clear();

    void on_selection_changed();

    void on_new();

    void on_new_xml();

    void on_entry_changed(int i);

    void on_save();

    void on_save_as();

    void save_as(const std::string& filename);

    void on_remove();

    void on_close();

    void on_edit_freq_resp();

    void on_browse_freq_resp();

    void on_save_open_files();

    void on_settings_changed(const std::string&);

    void type_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);
    void qts_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);
    void fs_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);
    void vas_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);
    void rdc_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);
    void lvc_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);
    void qms_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);
    void qes_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);
    void imp_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);
    void sens_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);
    void mmd_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);
    void ad_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);
    void bl_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);
    void rms_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);
    void cms_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);

    virtual void create_model();

    virtual void add_columns();

    virtual void add_item(Speaker const& foo);

    void draw_impedance_plot(Speaker const& s, bool update = false);

    void set_entries_sensitive(bool value);

protected:
    struct ModelColumns : public Gtk::TreeModelColumnRecord
    {
    public:
        ModelColumns()
        {
            add(id);
            add(type);
            add(id_string);
            add(qts);
            add(fs);
            add(vas);
            add(rdc);
            add(lvc);
            add(qms);
            add(qes);
            add(imp);
            add(sens);
            add(mmd);
            add(ad);
            add(bl);
            add(rms);
            add(cms);
            add(freq_resp_file);
        }

    public:
        Gtk::TreeModelColumn<int> id;
        Gtk::TreeModelColumn<int> type;
        Gtk::TreeModelColumn<Glib::ustring> id_string;
        Gtk::TreeModelColumn<double> qts;
        Gtk::TreeModelColumn<double> fs;
        Gtk::TreeModelColumn<double> vas;
        Gtk::TreeModelColumn<double> rdc;
        Gtk::TreeModelColumn<double> lvc;
        Gtk::TreeModelColumn<double> qms;
        Gtk::TreeModelColumn<double> qes;
        Gtk::TreeModelColumn<double> imp;
        Gtk::TreeModelColumn<double> sens;
        Gtk::TreeModelColumn<double> mmd;
        Gtk::TreeModelColumn<double> ad;
        Gtk::TreeModelColumn<double> bl;
        Gtk::TreeModelColumn<double> rms;
        Gtk::TreeModelColumn<double> cms;
        Gtk::TreeModelColumn<Glib::ustring> freq_resp_file;
    };

    Gtk::Table m_TreeViewTable;
    Gtk::Grid m_grid;

    Gtk::ScrolledWindow m_ScrolledWindow;

    Gtk::Label m_Label;
    Gtk::TreeView m_TreeView;
    Glib::RefPtr<Gtk::ListStore> m_refListStore;

    Gtk::Button m_EditFreqRespButton;
    Gtk::Button m_BrowseFreqRespButton;

    Gtk::Entry m_IdStringEntry;
    Gtk::Entry m_QtsEntry;
    Gtk::Entry m_FsEntry;
    Gtk::Entry m_VasEntry;
    Gtk::Entry m_RdcEntry;
    Gtk::Entry m_LvcEntry;
    Gtk::Entry m_QmsEntry;
    Gtk::Entry m_QesEntry;

    Gtk::Entry m_ImpEntry;
    Gtk::Entry m_SensEntry;
    Gtk::Entry m_FreqRespFileEntry;
    Gtk::Entry m_ImpRespFileEntry;

    Gtk::CheckButton m_BassCheckButton;
    Gtk::CheckButton m_MidrangeCheckButton;
    Gtk::CheckButton m_TweeterCheckButton;

    Gtk::Entry m_MmdEntry;
    Gtk::Entry m_AdEntry;
    Gtk::Entry m_BlEntry;
    Gtk::Entry m_RmsEntry;
    Gtk::Entry m_CmsEntry;

    Gtk::Box m_vbox;
    Gtk::Box m_treeview_vbox;
    Gtk::Box m_inner_treeview_vbox;

    Gtk::Frame m_treeview_frame;

    Gtk::Widget* m_evbox;
    Gtk::Label* m_frame_label;

    Gtk::MenuItem m_menu_item;

    Gtk::Toolbar* m_toolbar{nullptr};

    bool updating_entries{false};
    bool m_modified{false};
    bool changed{false};
    bool new_xml_pressed{false};
    int index{-1};

    std::unique_ptr<speaker_list> m_speaker_list{nullptr};
    DriverFreqRespPlot plot;

    ModelColumns m_columns;
    std::string m_filename;
};

#endif
