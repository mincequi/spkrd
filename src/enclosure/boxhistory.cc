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

#include "boxhistory.h"

#include "gspeakersfilechooser.h"

#include <gtkmm/messagedialog.h>

#include <ctime>
#include <iostream>

/* Use this to signal parent when to gray/ungray save-buttons */
sigc::signal1<void, bool> signal_enclosure_set_save_state;

BoxHistory::BoxHistory() : Gtk::Frame(""), m_vbox(Gtk::ORIENTATION_VERTICAL)
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
    std::cout << "BoxHistory::BoxHistory: m_filename = " << m_filename << std::endl;
#endif

    try
    {
        m_box_list = BoxList(m_filename);
        boxlist_found = true;
    }
    catch (std::runtime_error const& error)
    {
        std::cout << "BoxHistory::BoxHistory: " << error.what() << std::endl;
    }
    std::cout << "boxlist_found = " << boxlist_found << "\n";

    m_label.set_markup("<b>" + Glib::ustring(_("Enclosure list ["))
                       + GSpeakers::short_filename(m_filename) + "]</b>");
    set_label_widget(m_label);

    create_model();

    /* create tree view */
    m_TreeView.set_model(m_refListStore);
    m_TreeView.set_rules_hint();

    signal_add_to_boxlist.connect(sigc::mem_fun(*this, &BoxHistory::on_add_to_boxlist));
    signal_box_modified.connect(sigc::mem_fun(*this, &BoxHistory::on_box_modified));

    add_columns();
    m_ScrolledWindow.add(m_TreeView);

    show_all();
    index = 0;

    Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();

    selection->signal_changed().connect(sigc::mem_fun(*this, &BoxHistory::on_selection_changed));

    if (boxlist_found)
    {
        char* str = nullptr;
        GString* buffer = g_string_new(str);
        g_string_printf(buffer, "%d", 0);
        GtkTreePath* gpath = gtk_tree_path_new_from_string(buffer->str);
        Gtk::TreePath path(gpath);
        Gtk::TreeRow row = *(m_refListStore->get_iter(path));
        selection->select(row);
    }
    selected_plot = -1;
    signal_select_plot.connect(sigc::mem_fun(*this, &BoxHistory::on_plot_selected));

    signal_save_open_files.connect(sigc::mem_fun(*this, &BoxHistory::on_save_open_files));
}

BoxHistory::~BoxHistory() { g_settings.setValue("BoxListXml", m_filename); }

void BoxHistory::on_delete_plot() { signal_remove_box_plot(selected_plot); }

void BoxHistory::on_plot_selected(int i) { selected_plot = i; }

void BoxHistory::on_save_open_files()
{
    if (GSpeakers::enclosurelist_modified())
    {
        on_save();
    }
}

bool BoxHistory::on_delete_event(GdkEventAny* event)
{
    // handle this since we don't want to close the window
    g_settings.setValue("BoxListXml", m_filename);
#ifdef OUTPUT_DEBUG
    std::cout << "BoxHistory: on_delete_event" << std::endl;
#endif
    return true;
}

void BoxHistory::on_open_xml()
{
    GSpeakersFileChooserDialog fc(_("Open box xml"));
    std::string const& filename = fc.get_filename();
    if (!filename.empty())
    {
        open_xml(filename);
    }
}

void BoxHistory::on_append_xml()
{
    GSpeakersFileChooserDialog fc(_("Append box xml"));
    std::string const& filename = fc.get_filename();
    if (!filename.empty())
    {
        append_xml(filename);
    }
}

void BoxHistory::open_xml(const std::string& filename)
{
    BoxList temp_box_list;

    try
    {
        temp_box_list = BoxList(filename);
        m_refListStore->clear();

        m_filename = filename;
        for_each(temp_box_list.box_list().begin(),
                 temp_box_list.box_list().end(),
                 sigc::mem_fun(*this, &BoxHistory::add_item));

        // Delete items in box_list
        m_box_list.box_list().clear();
        m_box_list.box_list().insert(end(m_box_list.box_list()),
                                     begin(temp_box_list.box_list()),
                                     end(temp_box_list.box_list()));

        // Select the first item in the list
        if (!m_box_list.box_list().empty())
        {
            Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

            Gtk::TreePath path(std::to_string(0));

            Gtk::TreeRow row = *(m_refListStore->get_iter(path));

            refSelection->select(row);
        }
        signal_enclosure_set_save_state(false);
        static_cast<Gtk::Label*>(get_label_widget())
            ->set_markup("<b>" + Glib::ustring(_("Enclosure list ["))
                         + GSpeakers::short_filename(m_filename) + "]</b>");
        g_settings.setValue("BoxListXml", m_filename);
    }
    catch (std::runtime_error const& error)
    {
        Gtk::MessageDialog message(error.what(), false, Gtk::MESSAGE_ERROR);
        message.run();
    }
}

void BoxHistory::append_xml(const std::string& filename)
{
    BoxList temp_box_list;
    try
    {
        temp_box_list = BoxList(filename);
        std::for_each(temp_box_list.box_list().begin(),
                      temp_box_list.box_list().end(),
                      sigc::mem_fun(*this, &BoxHistory::add_item));

        for (auto& from : temp_box_list.box_list())
        {
            m_box_list.box_list().push_back(from);
        }
    }
    catch (std::runtime_error const& e)
    {
        Gtk::MessageDialog m(e.what(), false, Gtk::MESSAGE_ERROR);
        m.run();
    }
    signal_enclosure_set_save_state(true);
}

void BoxHistory::on_selection_changed()
{
    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

    if (const Gtk::TreeIter iter = refSelection->get_selected())
    {
        Gtk::TreePath path = m_refListStore->get_path(iter);

        if (!path.empty())
        {
            index = path[0];
            signal_box_selected(&(m_box_list.box_list()[path[0]]));
        }
    }
}

void BoxHistory::on_new_copy()
{
    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

    if (!m_box_list.box_list().empty())
    {
        // Find out which row we selected
        if (const Gtk::TreeIter iter = refSelection->get_selected())
        {
            Gtk::TreePath path = m_refListStore->get_path(iter);

            if (!path.empty())
            {
                /* Here we have the row in path[0], we want to make a copy of this Crossover
                   and put it last in the list */

                /* Here we want a copy of the original Crossover, not a crossover that has the
                   same id and so on, as we would get if we used the operator = or something similar,
                   Quick and easy solution...use the to_xml function which gets rid of the id */

                xmlNodePtr node = xmlNewDocNode(nullptr, nullptr, (xmlChar*)("parent"), nullptr);
                m_box_list.box_list()[path[0]].to_xml_node(node);
                Box b(node->children);

                // Set time of day as this id_string
                std::time_t const time = std::chrono::system_clock::to_time_t(
                    std::chrono::system_clock::now());
                b.set_id_string(_("Box: ") + std::string(std::ctime(&time)));

                /* the usual adding of items to the liststore and data-container */
                add_item(b);
                m_box_list.box_list().push_back(b);
            }
        }
    }
    Gtk::TreePath path(std::to_string(m_box_list.box_list().size() - 1));
    Gtk::TreeRow row = *(m_refListStore->get_iter(path));

    refSelection->select(row);

    signal_enclosure_set_save_state(true);
}

void BoxHistory::on_new()
{
    Box b;

    // Use time of day as identifier
    std::time_t const time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    b.set_id_string(_("Box: ") + std::string(std::ctime(&time)));

    add_item(b);
    m_box_list.box_list().push_back(b);

    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

    GtkTreePath* gpath = gtk_tree_path_new_from_string(
        GSpeakers::int_to_ustring(m_box_list.box_list().size() - 1).c_str());
    Gtk::TreePath path(gpath);
    Gtk::TreeRow row = *(m_refListStore->get_iter(path));
    refSelection->select(row);
    signal_enclosure_set_save_state(true);
}

void BoxHistory::on_new_xml()
{
    m_refListStore->clear();
    m_box_list.clear();

    new_xml_pressed = true;
    on_new();
    signal_enclosure_set_save_state(true);

    m_label.set_markup("<b>" + Glib::ustring(_("Enclosure list [new file]")) + "</b>");
    set_label_widget(m_label);
}

void BoxHistory::on_save()
{
    if (new_xml_pressed)
    {
        on_save_as();
        new_xml_pressed = false;
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
            Gtk::MessageDialog message(error.what(), false, Gtk::MESSAGE_ERROR);
            message.run();
        }
    }
}

void BoxHistory::on_save_as()
{
    GSpeakersFileChooserDialog fc(_("Save box xml as"), Gtk::FILE_CHOOSER_ACTION_SAVE, m_filename);

    std::string const& filename = fc.get_filename();
    if (!filename.empty())
    {
        save_as_xml(filename);
    }
}

void BoxHistory::save_as_xml(const std::string& filename)
{
    try
    {
        m_box_list.to_xml(filename);
        m_filename = filename;

        m_label.set_markup("<b>" + Glib::ustring(_("Enclosure list ["))
                           + GSpeakers::short_filename(m_filename) + "]</b>");
        set_label_widget(m_label);

        g_settings.setValue("BoxListXml", m_filename);
        signal_enclosure_set_save_state(false);
    }
    catch (std::runtime_error const& e)
    {
        Gtk::MessageDialog m(e.what(), false, Gtk::MESSAGE_ERROR);
        m.run();
    }
}

void BoxHistory::on_remove()
{
    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

    if (const Gtk::TreeIter iter = refSelection->get_selected())
    {
        Gtk::TreePath path = m_refListStore->get_path(iter);

        if (!path.empty())
        {
            // Remove item from ListStore:
            m_refListStore->erase(iter);

            if (index < (int)m_box_list.box_list().size())
                m_box_list.box_list().erase(m_box_list.box_list().begin() + index);
        }
    }
    Gtk::TreePath path(std::to_string(index > 0 ? index - 1 : 0));
    Gtk::TreeRow row = *(m_refListStore->get_iter(path));

    refSelection->select(row);

    signal_enclosure_set_save_state(true);
}

void BoxHistory::on_box_modified(Box* b)
{
    // get the row from selection
    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

    if (const Gtk::TreeIter iter = refSelection->get_selected())
    {
        Gtk::TreePath path = m_refListStore->get_path(iter);

        if (!path.empty())
        {
            Gtk::TreeRow row = *(m_refListStore->get_iter(path));

            /* Update the liststore */
            row[m_columns.type] = b->get_type();
            row[m_columns.id_string] = b->get_id_string();
            row[m_columns.speaker] = b->get_speaker();
            row[m_columns.vb1] = b->get_vb1();
            row[m_columns.fb1] = b->get_fb1();
            row[m_columns.vb2] = b->get_vb2();
            row[m_columns.fb2] = b->get_fb2();

            /* Update the boxlist */
            m_box_list.box_list()[path[0]].set_type(b->get_type());
            m_box_list.box_list()[path[0]].set_id_string(b->get_id_string());
            m_box_list.box_list()[path[0]].set_speaker(b->get_speaker());
            m_box_list.box_list()[path[0]].set_vb1(b->get_vb1());
            m_box_list.box_list()[path[0]].set_fb1(b->get_fb1());
            m_box_list.box_list()[path[0]].set_vb2(b->get_vb2());
            m_box_list.box_list()[path[0]].set_fb2(b->get_fb2());

            signal_enclosure_set_save_state(true);
        }
    }
}

void BoxHistory::on_add_to_boxlist(Box* b)
{
    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();
    add_item(*b);
    m_box_list.box_list().push_back(*b);

    /* Select the last crossover in the list: the added crossover */
    Gtk::TreePath path(std::to_string(m_box_list.box_list().size() - 1));
    Gtk::TreeRow row = *(m_refListStore->get_iter(path));
    refSelection->select(row);
}

void BoxHistory::on_add_plot(Box* b, Speaker* s)
{
    add_item(*b);
    m_box_list.box_list().push_back(*b);
}

void BoxHistory::create_model()
{
    m_refListStore = Gtk::ListStore::create(m_columns);

    std::for_each(m_box_list.box_list().begin(),
                  m_box_list.box_list().end(),
                  sigc::mem_fun(*this, &BoxHistory::add_item));
}

void BoxHistory::add_columns()
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
                                    sigc::mem_fun(*this, &BoxHistory::type_cell_data_func));
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        col_cnt = m_TreeView.append_column(_("Vb1"), *pRenderer);

        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(col_cnt - 1);
        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this, &BoxHistory::vb1_cell_data_func));
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        col_cnt = m_TreeView.append_column(_("Fb1"), *pRenderer);

        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(col_cnt - 1);
        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this, &BoxHistory::fb1_cell_data_func));
    }
}

/* This function will execute when a type cell is shown */
void BoxHistory::type_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
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

void BoxHistory::vb1_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    /* Ok i write litres with capital 'L', i know it's not standard but if you use arial or whatever
       serif (?) it doesn't look good */
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.vb1], 3, 1) + " l";
    renderer.property_xalign() = 1.0;
}

void BoxHistory::fb1_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.fb1], 3, 1) + " Hz";
    renderer.property_xalign() = 1.0;
}

void BoxHistory::add_item(Box const& box)
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
