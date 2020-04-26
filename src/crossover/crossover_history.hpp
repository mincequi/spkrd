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

#pragma once

#include "common.h"
#include "crossover_list.hpp"

#include <gtkmm/cellrenderer.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>

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
class crossover_history : public Gtk::Frame
{
public:
    crossover_history();

    ~crossover_history() override;

    // public callbacks, for use in container class

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
    void open_xml(const std::string& filename);

    void append_xml(const std::string& filename);

    void save_as_xml(const std::string& filename);

    void on_selection_changed();

    void on_part_modified();

    void on_net_modified_by_wizard(filter_network* net);

    void on_net_modified_by_user();

    void on_new_crossover_menu_action(int);

    void on_plot_crossover();

    void on_save_open_files();

    void type_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter);

    virtual void create_model();

    virtual void add_columns();

    virtual void add_item(Crossover const& foo);

protected:
    // This is used in the treemodel
    struct model_columns : public Gtk::TreeModelColumnRecord
    {
        model_columns()
        {
            add(id);
            add(type);
            add(id_string);
        }
        Gtk::TreeModelColumn<int> id;
        Gtk::TreeModelColumn<int> type;
        Gtk::TreeModelColumn<Glib::ustring> id_string;
    };

protected:
    Gtk::Widget* m_evbox;
    Gtk::Label* m_frame_label;
    Gtk::ScrolledWindow m_scrolled_window;
    Gtk::TreeView m_tree_view;
    Glib::RefPtr<Gtk::ListStore> m_list_store;

    crossover_list m_crossover_list;

    std::string m_filename;
    bool new_xml_pressed;
    int index = 0;

    model_columns m_columns;
};
