/*
  $Id$

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

#include "crossover_history.hpp"

#include "file_chooser.hpp"
#include "signal.hpp"

#include <gtkmm/eventbox.h>
#include <gtkmm/messagedialog.h>

#include <chrono>
#include <iostream>

constexpr auto MENU_INDEX_SAVE = 6;
constexpr auto TOOLBAR_INDEX_SAVE = 4;

sigc::signal1<void, bool> signal_crossover_set_save_state;

namespace
{
auto time_of_day() -> std::string
{
    std::time_t const time = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());
    return {std::ctime(&time)};
}
}

crossover_history::crossover_history() : Gtk::Frame("")
{
    set_border_width(2);
    m_scrolled_window.set_border_width(12);
    add(m_scrolled_window);

    m_scrolled_window.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
    m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

#ifdef TARGET_WIN32
    g_settings.defaultValueString("crossover_listXml", "crossover1.xml");
#else
    g_settings.defaultValueString("crossover_listXml",
                                  std::string(GSPEAKERS_PREFIX)
                                      + "/share/xml/crossover1.xml");
#endif
    m_filename = g_settings.getValueString("crossover_listXml");

    try
    {
        m_crossover_list = crossover_list(m_filename);
    }
    catch (std::runtime_error const& error)
    {
        std::cout << "crossover_history::crossover_history: " << error.what() << '\n';
    }
    set_shadow_type(Gtk::SHADOW_NONE);

    m_evbox = Gtk::make_managed<Gtk::EventBox>();
    m_frame_label = Gtk::make_managed<Gtk::Label>();
    m_frame_label->set_markup("<b>" + Glib::ustring(_("Crossovers ["))
                              + gspk::short_filename(m_filename, 20) + "]</b>");

    static_cast<Gtk::Container*>(m_evbox)->add(*m_frame_label);
    set_label_widget(*m_evbox);

    m_evbox->set_tooltip_text(m_filename);

    create_model();

    // create tree view
    m_tree_view.set_model(m_list_store);

    m_tree_view.get_selection()->signal_changed().connect(
        sigc::mem_fun(*this, &crossover_history::on_selection_changed));

    add_columns();

    m_scrolled_window.add(m_tree_view);

    show_all();

    signal_new_crossover.connect(sigc::mem_fun(*this, &crossover_history::on_new_from_menu));

    signal_net_modified_by_wizard.connect(
        sigc::mem_fun(*this, &crossover_history::on_net_modified_by_user));

    signal_net_modified_by_user.connect(
        sigc::mem_fun(*this, &crossover_history::on_net_modified_by_wizard));

    signal_save_open_files.connect(
        sigc::mem_fun(*this, &crossover_history::on_save_open_files));
}

void crossover_history::on_save_open_files()
{
    if (gspk::crossoverlist_modified())
    {
        on_save();
    }
}

void crossover_history::select_first_row()
{
    if (m_crossover_list.data().empty())
    {
        Gtk::TreePath path(std::to_string(0));
        Gtk::TreeRow row = *(m_list_store->get_iter(path));
        m_tree_view.get_selection()->select(row);
    }

    auto row = *(m_list_store->get_iter(Gtk::TreePath(std::to_string(0))));
    auto selection = m_tree_view.get_selection();
    selection->select(row);
}

void crossover_history::on_net_modified_by_wizard(filter_network* net)
{
    signal_crossover_set_save_state(true);
}

void crossover_history::on_net_modified_by_user()
{
    signal_crossover_set_save_state(true);
}

void crossover_history::on_part_modified() { signal_crossover_set_save_state(true); }

crossover_history::~crossover_history()
{
    g_settings.setValue("crossover_listXml", m_filename);
    try
    {
        g_settings.save();
    }
    catch (std::runtime_error const& error)
    {
        std::cout << "crossover_history::~crossover_history: saving settings error: "
                  << error.what() << "\n";
    }
}

void crossover_history::on_open_xml()
{
    file_chooser_dialog fc(_("Open crossover xml"));
    std::string const& filename = fc.get_filename();
    if (!filename.empty())
    {
        open_xml(filename);
    }
}

void crossover_history::on_append_xml()
{
    file_chooser_dialog fc(_("Append crossover xml"));
    std::string const& filename = fc.get_filename();
    if (filename.empty())
    {
        append_xml(filename);
    }
}

void crossover_history::open_xml(const std::string& filename)
{
    crossover_list temp_crossover_list;
    try
    {
        temp_crossover_list = crossover_list(filename);
        m_list_store->clear();

        m_filename = filename;

        std::for_each(begin(temp_crossover_list.data()),
                      end(temp_crossover_list.data()),
                      sigc::mem_fun(*this, &crossover_history::add_item));

        // Delete items in crossover_list
        m_crossover_list = temp_crossover_list;

        // Select the first item in the list
        if (!m_crossover_list.data().empty())
        {
            Gtk::TreePath path(std::to_string(0));

            Gtk::TreeRow row = *(m_list_store->get_iter(path));
            m_tree_view.get_selection()->select(row);
        }
        signal_crossover_set_save_state(false);
        m_frame_label->set_markup("<b>" + Glib::ustring(_("Crossovers "))
                                  + gspk::short_filename(m_filename, 20) + "]</b>");
        m_evbox->set_tooltip_text(m_filename);
    }
    catch (std::runtime_error const& e)
    {
        Gtk::MessageDialog(e.what(), false, Gtk::MESSAGE_ERROR).run();
    }
}

void crossover_history::append_xml(const std::string& filename)
{
#ifndef NDEBUG
    std::cout << "append xml ok: " << filename << "\n";
#endif
    crossover_list temp_crossover_list;
    try
    {
        temp_crossover_list = crossover_list(filename);

        std::for_each(temp_crossover_list.data().begin(),
                      temp_crossover_list.data().end(),
                      sigc::mem_fun(*this, &crossover_history::add_item));

        m_crossover_list.data().insert(end(m_crossover_list.data()),
                                       begin(temp_crossover_list.data()),
                                       end(temp_crossover_list.data()));
    }
    catch (std::runtime_error const& e)
    {
        Gtk::MessageDialog(e.what(), false, Gtk::MESSAGE_ERROR).run();
    }
    signal_crossover_set_save_state(true);
}

void crossover_history::on_selection_changed()
{
    auto selection = m_tree_view.get_selection();

    if (const Gtk::TreeIter iter = selection->get_selected())
    {
        auto path = m_list_store->get_path(iter);

        if (!path.empty())
        {
            m_index = path[0];
            signal_crossover_selected(&((m_crossover_list.data())[path[0]]));

            // Plot the crossover immediately after we selected it
            if (g_settings.getValueBool("AutoUpdateFilterPlots"))
            {
                signal_plot_crossover();
            }
        }
    }
}

void crossover_history::on_new_copy()
{
    auto selection = m_tree_view.get_selection();

    if (!m_crossover_list.data().empty())
    {
        // Find out which row we selected
        if (auto const iter = selection->get_selected())
        {
            auto path = m_list_store->get_path(iter);

            if (!path.empty())
            {
                /* Here we have the row in indices[0], we want to make a copy of this
                   Crossover and put it last in the list */

                /* Here we want a copy of the original Crossover, not a crossover that has
                   the same id and so on, as we would get if we used the operator = or
                   something similar, Quick and easy solution...use the to_xml function
                   which gets rid of the id */
                xmlNodePtr node = xmlNewDocNode(nullptr,
                                                nullptr,
                                                (xmlChar*)("parent"),
                                                nullptr);

                m_crossover_list.data()[path[0]].to_xml_node(node);

                Crossover c(node->children);

                // Set time of day as this crossovers id_string
                c.set_id_string(_("Crossover: ") + time_of_day());

                // the usual adding of items to the liststore and data-container
                add_item(c);
                m_crossover_list.data().push_back(c);
            }
        }
    }

    // Select the last crossover in the list: the new crossover
    Gtk::TreePath path(Glib::ustring(std::to_string(m_crossover_list.data().size() - 1)));

    Gtk::TreeRow row = *(m_list_store->get_iter(path));
    selection->select(row);
    signal_crossover_set_save_state(true);
}

void crossover_history::on_new_from_menu(int type)
{
    std::cout << "crossover_history::on_new_from_menu: " << type << "\n";

    Crossover c(type, _("Crossover ") + time_of_day());

    // Add to liststore
    add_item(c);
    m_crossover_list.data().push_back(c);

    Glib::RefPtr<Gtk::TreeSelection> selection = m_tree_view.get_selection();

    // make our new crossover the selected crossover
    Gtk::TreePath path(std::to_string(m_crossover_list.data().size() - 1));
    Gtk::TreeRow row = *(m_list_store->get_iter(path));
    selection->select(row);
    signal_crossover_set_save_state(true);
}

void crossover_history::on_new()
{
    Crossover c;

    // Set time of day as this crossovers id_string
    c.set_id_string(_("Crossover: ") + time_of_day());

    add_item(c);

    m_crossover_list.data().push_back(c);

    Glib::RefPtr<Gtk::TreeSelection> selection = m_tree_view.get_selection();

    Gtk::TreePath path(std::to_string(m_crossover_list.data().size() - 1));

    Gtk::TreeRow row = *(m_list_store->get_iter(path));

    selection->select(row);
    signal_crossover_set_save_state(true);
}

void crossover_history::on_new_xml()
{
    m_list_store->clear();
    m_crossover_list.clear();
    m_new_xml_pressed = true;
    on_new();
    signal_crossover_set_save_state(true);
    static_cast<Gtk::Label*>(get_label_widget())
        ->set_markup("<b>" + Glib::ustring(_("Crossover list [new file]")) + "</b>");
}

void crossover_history::on_save()
{
#ifndef NDEBUG
    std::puts("crossover_history::on_save");
#endif
    if (m_new_xml_pressed)
    {
        on_save_as();
        m_new_xml_pressed = false;
    }
    else
    {
        try
        {
            m_crossover_list.to_xml(m_filename);
            signal_crossover_set_save_state(false);
        }
        catch (std::runtime_error const& e)
        {
            Gtk::MessageDialog(e.what(), false, Gtk::MESSAGE_ERROR).run();
        }
    }
}

void crossover_history::on_save_as()
{
#ifndef NDEBUG
    std::cout << "save as" << std::endl;
#endif
    file_chooser_dialog fc(_("Save crossover xml as"),
                           Gtk::FILE_CHOOSER_ACTION_SAVE,
                           m_filename);
    std::string const& filename = fc.get_filename();
    if (filename.empty())
    {
        save_as_xml(filename);
    }
}

void crossover_history::save_as_xml(const std::string& filename)
{
#ifndef NDEBUG
    std::cout << _("save as ok") << "\n";
#endif
    try
    {
        m_crossover_list.to_xml(filename);
        m_filename = filename;

        {
            auto crossover_label = Gtk::make_managed<Gtk::Label>();
            crossover_label->set_markup("<b>" + Glib::ustring(_("Crossover list ["))
                                        + gspk::short_filename(m_filename, 20) + "]</b>");
            set_label_widget(*crossover_label);
        }
        signal_crossover_set_save_state(false);
    }
    catch (std::runtime_error const& e)
    {
        Gtk::MessageDialog(e.what(), false, Gtk::MESSAGE_ERROR).run();
    }
}

void crossover_history::on_remove()
{
    Glib::RefPtr<Gtk::TreeSelection> selection = m_tree_view.get_selection();

    if (Gtk::TreeIter const iter = selection->get_selected())
    {
        Gtk::TreePath path = m_list_store->get_path(iter);
        if (!path.empty())
        {
            // Remove item from ListStore:
            m_list_store->erase(iter);

            if (m_index < m_crossover_list.data().size())
                m_crossover_list.data().erase(m_crossover_list.data().begin() + m_index);
        }
    }

    Gtk::TreePath path(std::to_string(m_index > 0 ? m_index - 1 : 0));
    Gtk::TreeRow row = *(m_list_store->get_iter(path));
    selection->select(row);

    signal_crossover_set_save_state(true);
}

void crossover_history::create_model()
{
    m_list_store = Gtk::ListStore::create(m_columns);

    std::for_each(cbegin(m_crossover_list.data()),
                  cend(m_crossover_list.data()),
                  sigc::mem_fun(*this, &crossover_history::add_item));
}

void crossover_history::add_columns()
{
    {
        auto renderer = Gtk::make_managed<Gtk::CellRendererText>();

        Gtk::TreeViewColumn* column = m_tree_view.get_column(
            m_tree_view.append_column(_("Identifier"), *renderer) - 1);

        column->add_attribute(renderer->property_text(), m_columns.id_string);
    }

    {
        auto renderer = Gtk::make_managed<Gtk::CellRendererText>();

        m_tree_view.get_column(m_tree_view.append_column(_("Type"), *renderer) - 1)
            ->set_cell_data_func(*renderer,
                                 sigc::mem_fun(*this,
                                               &crossover_history::type_cell_data_func));
    }
}

void crossover_history::type_cell_data_func(Gtk::CellRenderer* cell,
                                            Gtk::TreeModel::iterator const& iter)
{
    std::string s;

    if (((*iter)[m_columns.type] & CROSSOVER_TYPE_LOWPASS) != 0)
    {
        s += _("lowpass");
    }
    if (((*iter)[m_columns.type] & CROSSOVER_TYPE_SUBSONIC) != 0)
    {
        if (!s.empty())
        {
            s += (", ");
        }
        s += _("subsonic");
    }
    if (((*iter)[m_columns.type] & CROSSOVER_TYPE_HIGHPASS) != 0)
    {
        if (!s.empty())
        {
            s += _(", ");
        }
        s += _("highpass");
    }
    if (((*iter)[m_columns.type] & CROSSOVER_TYPE_TWOWAY) != 0)
    {
        if (!s.empty())
        {
            s += _(", ");
        }
        s += _("2-way");
    }
    if (((*iter)[m_columns.type] & CROSSOVER_TYPE_THREEWAY) != 0)
    {
        if (!s.empty())
        {
            s += _(", ");
        }
        s += _("3-way");
    }
    if (((*iter)[m_columns.type] & CROSSOVER_TYPE_FOURWAY) != 0)
    {
        if (!s.empty())
        {
            s += _(", ");
        }
        s += _("4-way");
    }

    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);

    renderer.property_text() = s;
}

void crossover_history::add_item(Crossover const& foo)
{
    Gtk::TreeRow row = *(m_list_store->append());

    row[m_columns.id] = foo.get_id();
    row[m_columns.id_string] = foo.get_id_string();
    row[m_columns.type] = foo.get_type();
}
