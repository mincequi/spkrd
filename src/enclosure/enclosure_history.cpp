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

#include "enclosure_history.hpp"

#include "file_chooser.hpp"
#include "signal.hpp"

#include <gtkmm/messagedialog.h>

#include <ctime>
#include <iostream>

/* Use this to signal parent when to gray/ungray save-buttons */
sigc::signal1<void, bool> signal_enclosure_set_save_state;

enclosure_history::enclosure_history() : Gtk::Frame(""), m_vbox(Gtk::ORIENTATION_VERTICAL)
{
    bool boxlist_found = false;
    set_border_width(2);
    set_shadow_type(Gtk::SHADOW_NONE);

    m_vbox.set_border_width(12);
    add(m_vbox);
    m_vbox.pack_start(m_ScrolledWindow);

    m_ScrolledWindow.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

#ifdef TARGET_WIN32
    g_settings.defaultValueString("BoxListXml", "box1.xml");
#else
    g_settings.defaultValueString("BoxListXml",
                                  std::string(GSPEAKERS_PREFIX) + "/share/xml/box1.xml");
#endif
    m_filename = g_settings.getValueString("BoxListXml");
#ifdef __OUTPUT_DEBUG
    std::cout << "enclosure_history::enclosure_history: m_filename = " << m_filename
              << std::endl;
#endif

    try
    {
        m_box_list = enclosure_list(m_filename);
        boxlist_found = true;
    }
    catch (std::runtime_error const& error)
    {
        std::cout << "enclosure_history::enclosure_history: " << error.what() << std::endl;
    }
    std::cout << "boxlist_found = " << boxlist_found << "\n";

    m_label.set_markup("<b>" + Glib::ustring(_("Enclosure list ["))
                       + gspk::short_filename(m_filename) + "]</b>");
    set_label_widget(m_label);

    create_model();

    // create tree view
    m_TreeView.set_model(m_refListStore);
    m_TreeView.set_rules_hint();

    signal_add_to_boxlist.connect(
        sigc::mem_fun(*this, &enclosure_history::on_add_to_boxlist));
    signal_box_modified.connect(sigc::mem_fun(*this, &enclosure_history::on_box_modified));

    add_columns();
    m_ScrolledWindow.add(m_TreeView);

    show_all();
    index = 0;

    auto selection = m_TreeView.get_selection();

    selection->signal_changed().connect(
        sigc::mem_fun(*this, &enclosure_history::on_selection_changed));

    if (boxlist_found)
    {
        Gtk::TreePath path(std::to_string(0));

        Gtk::TreeRow row = *(m_refListStore->get_iter(path));

        selection->select(row);
    }
    selected_plot = -1;
    signal_select_plot.connect(sigc::mem_fun(*this, &enclosure_history::on_plot_selected));

    signal_save_open_files.connect(
        sigc::mem_fun(*this, &enclosure_history::on_save_open_files));
}

enclosure_history::~enclosure_history() { g_settings.setValue("BoxListXml", m_filename); }

void enclosure_history::on_delete_plot() { signal_remove_box_plot(selected_plot); }

void enclosure_history::on_plot_selected(int i) { selected_plot = i; }

void enclosure_history::on_save_open_files()
{
    if (gspk::enclosurelist_modified())
    {
        on_save();
    }
}

auto enclosure_history::on_delete_event(GdkEventAny* event) -> bool
{
    // handle this since we don't want to close the window
    g_settings.setValue("BoxListXml", m_filename);
#ifndef NDEBUG
    std::puts("enclosure_history: on_delete_event");
#endif
    return true;
}

void enclosure_history::on_open_xml()
{
    file_chooser_dialog fc(_("Open box xml"));
    std::string const& filename = fc.get_filename();
    if (!filename.empty())
    {
        open_xml(filename);
    }
}

void enclosure_history::on_append_xml()
{
    file_chooser_dialog fc(_("Append box xml"));
    std::string const& filename = fc.get_filename();
    if (!filename.empty())
    {
        append_xml(filename);
    }
}

void enclosure_history::open_xml(const std::string& filename)
{
    enclosure_list temp_box_list;

    try
    {
        temp_box_list = enclosure_list(filename);
        m_refListStore->clear();

        m_filename = filename;
        for_each(temp_box_list.data().begin(),
                 temp_box_list.data().end(),
                 sigc::mem_fun(*this, &enclosure_history::add_item));

        // Delete items in data
        m_box_list.data().clear();
        m_box_list.data().insert(end(m_box_list.data()),
                                 begin(temp_box_list.data()),
                                 end(temp_box_list.data()));

        // Select the first item in the list
        if (!m_box_list.data().empty())
        {
            Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();

            Gtk::TreePath path(std::to_string(0));

            Gtk::TreeRow row = *(m_refListStore->get_iter(path));

            selection->select(row);
        }
        signal_enclosure_set_save_state(false);
        static_cast<Gtk::Label*>(get_label_widget())
            ->set_markup("<b>" + Glib::ustring(_("Enclosure list ["))
                         + gspk::short_filename(m_filename) + "]</b>");
        g_settings.setValue("BoxListXml", m_filename);
    }
    catch (std::runtime_error const& error)
    {
        Gtk::MessageDialog message(error.what(), false, Gtk::MESSAGE_ERROR);
        message.run();
    }
}

void enclosure_history::append_xml(const std::string& filename)
{
    enclosure_list temp_box_list;
    try
    {
        temp_box_list = enclosure_list(filename);
        std::for_each(temp_box_list.data().begin(),
                      temp_box_list.data().end(),
                      sigc::mem_fun(*this, &enclosure_history::add_item));

        for (auto& from : temp_box_list.data())
        {
            m_box_list.data().push_back(from);
        }
    }
    catch (std::runtime_error const& e)
    {
        Gtk::MessageDialog m(e.what(), false, Gtk::MESSAGE_ERROR);
        m.run();
    }
    signal_enclosure_set_save_state(true);
}

void enclosure_history::on_selection_changed()
{
    Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();

    if (const Gtk::TreeIter iter = selection->get_selected())
    {
        Gtk::TreePath path = m_refListStore->get_path(iter);

        if (!path.empty())
        {
            index = path[0];
            signal_box_selected(&(m_box_list.data()[path[0]]));
        }
    }
}

void enclosure_history::on_new_copy()
{
    Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();

    if (!m_box_list.data().empty())
    {
        // Find out which row we selected
        if (const Gtk::TreeIter iter = selection->get_selected())
        {
            Gtk::TreePath path = m_refListStore->get_path(iter);

            if (!path.empty())
            {
                xmlNodePtr node = xmlNewDocNode(nullptr,
                                                nullptr,
                                                (xmlChar*)("parent"),
                                                nullptr);
                m_box_list.data()[path[0]].to_xml_node(node);
                enclosure b(node->children);

                // Set time of day as this id_string
                std::time_t const time = std::chrono::system_clock::to_time_t(
                    std::chrono::system_clock::now());
                b.set_id_string(_("enclosure: ") + std::string(std::ctime(&time)));

                // the usual adding of items to the liststore and data-container
                add_item(b);
                m_box_list.data().push_back(b);
            }
        }
    }
    Gtk::TreePath path(std::to_string(m_box_list.data().size() - 1));
    Gtk::TreeRow row = *(m_refListStore->get_iter(path));

    selection->select(row);

    signal_enclosure_set_save_state(true);
}

void enclosure_history::on_new()
{
    enclosure b;

    // Use time of day as identifier
    std::time_t const time = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());
    b.set_id_string(_("enclosure: ") + std::string(std::ctime(&time)));

    add_item(b);
    m_box_list.data().push_back(b);

    Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();

    GtkTreePath* gpath = gtk_tree_path_new_from_string(
        gspk::int_to_ustring(m_box_list.data().size() - 1).c_str());
    Gtk::TreePath path(gpath);
    Gtk::TreeRow row = *(m_refListStore->get_iter(path));
    selection->select(row);
    signal_enclosure_set_save_state(true);
}

void enclosure_history::on_new_xml()
{
    m_refListStore->clear();
    m_box_list.clear();

    m_new_xml_pressed = true;
    on_new();
    signal_enclosure_set_save_state(true);

    m_label.set_markup("<b>" + Glib::ustring(_("Enclosure list [new file]")) + "</b>");
    set_label_widget(m_label);
}

void enclosure_history::on_save()
{
    if (m_new_xml_pressed)
    {
        on_save_as();
        m_new_xml_pressed = false;
    }
    else
    {
        try
        {
            m_box_list.to_xml(m_filename);
            signal_enclosure_set_save_state(false);
        }
        catch (std::runtime_error const& error)
        {
            Gtk::MessageDialog(error.what(), false, Gtk::MESSAGE_ERROR).run();
        }
    }
}

void enclosure_history::on_save_as()
{
    file_chooser_dialog fc(_("Save box xml as"), Gtk::FILE_CHOOSER_ACTION_SAVE, m_filename);

    std::string const& filename = fc.get_filename();
    if (!filename.empty())
    {
        save_as_xml(filename);
    }
}

void enclosure_history::save_as_xml(const std::string& filename)
{
    try
    {
        m_box_list.to_xml(filename);
        m_filename = filename;

        m_label.set_markup("<b>" + Glib::ustring(_("Enclosure list ["))
                           + gspk::short_filename(m_filename) + "]</b>");
        set_label_widget(m_label);

        g_settings.setValue("BoxListXml", m_filename);
        signal_enclosure_set_save_state(false);
    }
    catch (std::runtime_error const& error)
    {
        Gtk::MessageDialog(error.what(), false, Gtk::MESSAGE_ERROR).run();
    }
}

void enclosure_history::on_remove()
{
    Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();

    if (const Gtk::TreeIter iter = selection->get_selected())
    {
        Gtk::TreePath path = m_refListStore->get_path(iter);

        if (!path.empty())
        {
            // Remove item from ListStore:
            m_refListStore->erase(iter);

            if (index < (int)m_box_list.data().size())
                m_box_list.data().erase(begin(m_box_list.data()) + index);
        }
    }
    Gtk::TreePath path(std::to_string(index > 0 ? index - 1 : 0));
    Gtk::TreeRow row = *(m_refListStore->get_iter(path));

    selection->select(row);

    signal_enclosure_set_save_state(true);
}

void enclosure_history::on_box_modified(enclosure* b)
{
    // get the row from selection
    Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();

    if (const Gtk::TreeIter iter = selection->get_selected())
    {
        Gtk::TreePath path = m_refListStore->get_path(iter);

        if (!path.empty())
        {
            Gtk::TreeRow row = *(m_refListStore->get_iter(path));

            // Update the liststore
            row[m_columns.type] = b->get_type();
            row[m_columns.id_string] = b->get_id_string();
            row[m_columns.speaker] = b->get_speaker();
            row[m_columns.vb1] = b->get_vb1();
            row[m_columns.fb1] = b->get_fb1();
            row[m_columns.vb2] = b->get_vb2();
            row[m_columns.fb2] = b->get_fb2();

            // Update the boxlist
            m_box_list.data()[path[0]].set_type(b->get_type());
            m_box_list.data()[path[0]].set_id_string(b->get_id_string());
            m_box_list.data()[path[0]].set_speaker(b->get_speaker());
            m_box_list.data()[path[0]].set_vb1(b->get_vb1());
            m_box_list.data()[path[0]].set_fb1(b->get_fb1());
            m_box_list.data()[path[0]].set_vb2(b->get_vb2());
            m_box_list.data()[path[0]].set_fb2(b->get_fb2());

            signal_enclosure_set_save_state(true);
        }
    }
}

void enclosure_history::on_add_to_boxlist(enclosure* b)
{
    Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();
    add_item(*b);
    m_box_list.data().push_back(*b);

    /* Select the last crossover in the list: the added crossover */
    Gtk::TreePath path(std::to_string(m_box_list.data().size() - 1));
    Gtk::TreeRow row = *(m_refListStore->get_iter(path));
    selection->select(row);
}

void enclosure_history::on_add_plot(enclosure* b, driver* s)
{
    add_item(*b);
    m_box_list.data().push_back(*b);
}

void enclosure_history::create_model()
{
    m_refListStore = Gtk::ListStore::create(m_columns);

    std::for_each(m_box_list.data().begin(),
                  m_box_list.data().end(),
                  sigc::mem_fun(*this, &enclosure_history::add_item));
}

void enclosure_history::add_columns()
{
    int col_cnt;
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        col_cnt = m_TreeView.append_column(_("Identifier"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(col_cnt - 1);

        pColumn->add_attribute(pRenderer->property_text(), m_columns.id_string);
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        col_cnt = m_TreeView.append_column(_("Driver"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(col_cnt - 1);

        pColumn->add_attribute(pRenderer->property_text(), m_columns.speaker);
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        col_cnt = m_TreeView.append_column(_("Type"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(col_cnt - 1);

        // pColumn->add_attribute(pRenderer->property_text(), m_columns.type_str);
        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this,
                                                  &enclosure_history::type_cell_data_func));
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        col_cnt = m_TreeView.append_column(_("Vb1"), *pRenderer);

        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(col_cnt - 1);
        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this,
                                                  &enclosure_history::vb1_cell_data_func));
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        col_cnt = m_TreeView.append_column(_("Fb1"), *pRenderer);

        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(col_cnt - 1);
        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this,
                                                  &enclosure_history::fb1_cell_data_func));
    }
}

/* This function will execute when a type cell is shown */
void enclosure_history::type_cell_data_func(Gtk::CellRenderer* cell,
                                            const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    switch ((*iter)[m_columns.type])
    {
        case BOX_TYPE_SEALED:
            renderer.property_text() = _("Sealed");
            break;
        case BOX_TYPE_PORTED:
            renderer.property_text() = _("Ported");
            break;
        default:
            renderer.property_text() = _("Unknown");
            break;
    }
    renderer.property_xalign() = 1.0;
}

void enclosure_history::vb1_cell_data_func(Gtk::CellRenderer* cell,
                                           const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);

    renderer.property_text() = gspk::to_ustring((*iter)[m_columns.vb1], 3, 1) + " l";
    renderer.property_xalign() = 1.0;
}

void enclosure_history::fb1_cell_data_func(Gtk::CellRenderer* cell,
                                           const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = gspk::to_ustring((*iter)[m_columns.fb1], 3, 1) + " Hz";
    renderer.property_xalign() = 1.0;
}

void enclosure_history::add_item(enclosure const& box)
{
    Gtk::TreeRow row = *(m_refListStore->append());
    row[m_columns.id_string] = box.get_id_string();
    row[m_columns.speaker] = box.get_speaker();
    row[m_columns.type] = box.get_type();
    row[m_columns.vb1] = box.get_vb1();
    row[m_columns.fb1] = box.get_fb1();
    row[m_columns.vb2] = box.get_vb2();
    row[m_columns.fb2] = box.get_fb2();
}
