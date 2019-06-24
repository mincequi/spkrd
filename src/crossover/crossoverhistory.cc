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

#include "crossoverhistory.h"

#include "gspeakersfilechooser.h"

#include <gtkmm/eventbox.h>
#include <gtkmm/messagedialog.h>

#include <chrono>
#include <iostream>

constexpr auto MENU_INDEX_SAVE = 6;
constexpr auto TOOLBAR_INDEX_SAVE = 4;

sigc::signal1<void, bool> signal_crossover_set_save_state;

CrossoverHistory::CrossoverHistory() : Gtk::Frame("")
{
    set_border_width(2);
    m_ScrolledWindow.set_border_width(12);
    add(m_ScrolledWindow);

    m_ScrolledWindow.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

#ifdef TARGET_WIN32
    g_settings.defaultValueString("CrossoverListXml", "crossover1.xml");
#else
    g_settings.defaultValueString("CrossoverListXml",
                                  std::string(GSPEAKERS_PREFIX) + "/share/xml/crossover1.xml");
#endif
    m_filename = g_settings.getValueString("CrossoverListXml");

    try
    {
        m_crossover_list = CrossoverList(m_filename);
    }
    catch (std::runtime_error const& e)
    {
        std::cout << "CrossoverHistory::CrossoverHistory: " << e.what() << std::endl;
    }
    set_shadow_type(Gtk::SHADOW_NONE);

    m_evbox = Gtk::manage(new Gtk::EventBox());
    m_frame_label = Gtk::manage(new Gtk::Label());
    m_frame_label->set_markup("<b>" + Glib::ustring(_("Crossovers ["))
                              + GSpeakers::short_filename(m_filename, 20) + "]</b>");

    static_cast<Gtk::Container*>(m_evbox)->add(*m_frame_label);
    set_label_widget(*m_evbox);

    m_evbox->set_tooltip_text(m_filename);

    create_model();

    // create tree view
    m_TreeView.set_model(m_refListStore);

    Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();
    selection->signal_changed().connect(sigc::mem_fun(*this, &CrossoverHistory::on_selection_changed));

    add_columns();

    m_ScrolledWindow.add(m_TreeView);

    show_all();

    index = 0;

    signal_new_crossover.connect(sigc::mem_fun(*this, &CrossoverHistory::on_new_from_menu));

    signal_net_modified_by_wizard.connect(
        sigc::mem_fun(*this, &CrossoverHistory::on_net_modified_by_user));

    signal_net_modified_by_user.connect(
        sigc::mem_fun(*this, &CrossoverHistory::on_net_modified_by_wizard));

    signal_save_open_files.connect(sigc::mem_fun(*this, &CrossoverHistory::on_save_open_files));
}

void CrossoverHistory::on_save_open_files()
{
    if (GSpeakers::crossoverlist_modified())
    {
        on_save();
    }
}

void CrossoverHistory::select_first_row()
{
    if (!m_crossover_list.data().empty())
    {
        char* str = nullptr;
        GString* buffer = g_string_new(str);
        g_string_printf(buffer, "%d", 0);
        GtkTreePath* gpath = gtk_tree_path_new_from_string(buffer->str);
        Gtk::TreePath path(gpath);
        Gtk::TreeRow row = *(m_refListStore->get_iter(path));
        Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();
        selection->select(row);
    }
}

void CrossoverHistory::on_net_modified_by_wizard(Net* net)
{
    signal_crossover_set_save_state(true);
}

void CrossoverHistory::on_net_modified_by_user() { signal_crossover_set_save_state(true); }

void CrossoverHistory::on_part_modified() { signal_crossover_set_save_state(true); }

CrossoverHistory::~CrossoverHistory()
{
    g_settings.setValue("CrossoverListXml", m_filename);
    try
    {
        g_settings.save();
    }
    catch (std::runtime_error const& error)
    {
        std::cout << "CrossoverHistory::~CrossoverHistory: saving settings error: " << error.what()
                  << "\n";
    }
}

void CrossoverHistory::on_open_xml()
{
    GSpeakersFileChooserDialog fc(_("Open crossover xml"));
    std::string const& filename = fc.get_filename();
    if (!filename.empty())
    {
        open_xml(filename);
    }
}

void CrossoverHistory::on_append_xml()
{
    GSpeakersFileChooserDialog fc(_("Append crossover xml"));
    std::string const& filename = fc.get_filename();
    if (filename.empty())
    {
        append_xml(filename);
    }
}

void CrossoverHistory::open_xml(const std::string& filename)
{
    CrossoverList temp_crossover_list;
    try
    {
        temp_crossover_list = CrossoverList(filename);
        m_refListStore->clear();

        m_filename = filename;
        std::for_each(temp_crossover_list.data().begin(),
                      temp_crossover_list.data().end(),
                      sigc::mem_fun(*this, &CrossoverHistory::add_item));

        /* Delete items in crossover_list */
        m_crossover_list.data().clear();
        m_crossover_list.data().insert(begin(m_crossover_list.data()),
                                                 temp_crossover_list.data().begin(),
                                                 temp_crossover_list.data().end());

        /* Select the first item in the list */
        if (!m_crossover_list.data().empty())
        {
            Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

            char* str = nullptr;
            GString* buffer = g_string_new(str);
            g_string_printf(buffer, "%d", 0);

            GtkTreePath* gpath = gtk_tree_path_new_from_string(buffer->str);

            Gtk::TreePath path(gpath);

            Gtk::TreeRow row = *(m_refListStore->get_iter(path));
            refSelection->select(row);
        }
        signal_crossover_set_save_state(false);
        m_frame_label->set_markup("<b>" + Glib::ustring(_("Crossovers "))
                                  + GSpeakers::short_filename(m_filename, 20) + "]</b>");
        m_evbox->set_tooltip_text(m_filename);
    }
    catch (std::runtime_error const& e)
    {
        Gtk::MessageDialog m(e.what(), false, Gtk::MESSAGE_ERROR);
        m.run();
    }
}

void CrossoverHistory::append_xml(const std::string& filename)
{
#ifndef NDEBUG
    std::cout << "append xml ok: " << filename << std::endl;
#endif
    CrossoverList temp_crossover_list;
    try
    {
        temp_crossover_list = CrossoverList(filename);

        std::for_each(temp_crossover_list.data().begin(),
                      temp_crossover_list.data().end(),
                      sigc::mem_fun(*this, &CrossoverHistory::add_item));
        for (auto& from : temp_crossover_list.data())
        {
            m_crossover_list.data().push_back(from);
        }
        m_crossover_list.data().size();
    }
    catch (std::runtime_error const& e)
    {
        Gtk::MessageDialog m(e.what(), false, Gtk::MESSAGE_ERROR);
        m.run();
    }
    signal_crossover_set_save_state(true);
}

void CrossoverHistory::on_selection_changed()
{
    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

    if (const Gtk::TreeIter iter = refSelection->get_selected())
    {
        Gtk::TreePath path = m_refListStore->get_path(iter);

        if (!path.empty())
        {
            index = path[0];
            signal_crossover_selected(&((m_crossover_list.data())[path[0]]));
            /* Plot the crossover immediately after we selected it */
            if (g_settings.getValueBool("AutoUpdateFilterPlots"))
            {
                signal_plot_crossover();
            }
        }
    }
}

void CrossoverHistory::on_new_copy()
{
    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

    if (!m_crossover_list.data().empty())
    {
        /* Find out which row we selected */

        if (const Gtk::TreeIter iter = refSelection->get_selected())
        {
            Gtk::TreePath path = m_refListStore->get_path(iter);

            if (!path.empty())
            {
                /* Here we have the row in indices[0], we want to make a copy of this Crossover
                   and put it last in the list */

                /* Here we want a copy of the original Crossover, not a crossover that has the
                   same id and so on, as we would get if we used the operator = or something similar,
                   Quick and easy solution...use the to_xml function which gets rid of the id */
                xmlNodePtr node = xmlNewDocNode(nullptr, nullptr, (xmlChar*)("parent"), nullptr);

                ((m_crossover_list.data())[path[0]]).to_xml_node(node);

                Crossover c(node->children);

                /* convert to nice time format */

                /* Set time of day as this crossovers id_string */
                std::time_t time = std::chrono::system_clock::to_time_t(
                    std::chrono::system_clock::now());
                c.set_id_string(_("Crossover: ") + std::string(std::ctime(&time) + '\0'));

                /* the usual adding of items to the liststore and data-container */
                add_item(c);
                m_crossover_list.data().push_back(c);
            }
        }
    }

    /* Select the last crossover in the list: the new crossover */
    Gtk::TreePath path(Glib::ustring(std::to_string(m_crossover_list.data().size() - 1)));

    Gtk::TreeRow row = *(m_refListStore->get_iter(path));
    refSelection->select(row);
    signal_crossover_set_save_state(true);
}

void CrossoverHistory::on_new_from_menu(int type)
{
#ifndef NDEBUG
    std::cout << "CrossoverHistory::on_new_from_menu: " << type << std::endl;
#endif
    /* add new crossover of appropriate type here */
    time_t t;
    time(&t);
    /* convert to nice time format */
    std::string s = std::string(ctime(&t));
    int length = s.length();
    s[length - 1] = '\0';

    Crossover c(type, _("Crossover ") + s);

    /* Add to liststore */
    add_item(c);
    m_crossover_list.data().push_back(c);

    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

    // make our new crossover the selected crossover
    Gtk::TreePath path(std::to_string(m_crossover_list.data().size() - 1));
    Gtk::TreeRow row = *(m_refListStore->get_iter(path));
    refSelection->select(row);
    signal_crossover_set_save_state(true);
}

void CrossoverHistory::on_new()
{
    Crossover c;

    // Set time of day as this crossovers id_string
    time_t t;
    time(&t);
    // convert to nice time format
    std::string s = std::string(ctime(&t));
    int length = s.length();
    s[length - 1] = '\0';
    c.set_id_string(_("Crossover: ") + s);

    add_item(c);

    m_crossover_list.data().push_back(c);

    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

    Gtk::TreePath path(std::to_string(m_crossover_list.data().size() - 1));

    Gtk::TreeRow row = *(m_refListStore->get_iter(path));

    refSelection->select(row);
    signal_crossover_set_save_state(true);
}

void CrossoverHistory::on_new_xml()
{
    m_refListStore->clear();
    m_crossover_list.clear();
    new_xml_pressed = true;
    on_new();
    signal_crossover_set_save_state(true);
    static_cast<Gtk::Label*>(get_label_widget())
        ->set_markup("<b>" + Glib::ustring(_("Crossover list [new file]")) + "</b>");
}

void CrossoverHistory::on_save()
{
#ifndef NDEBUG
    std::puts("CrossoverHistory::on_save");
#endif
    if (new_xml_pressed)
    {
        on_save_as();
        new_xml_pressed = false;
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

void CrossoverHistory::on_save_as()
{
#ifndef NDEBUG
    std::cout << "save as" << std::endl;
#endif
    GSpeakersFileChooserDialog fc(_("Save crossover xml as"),
                                  Gtk::FILE_CHOOSER_ACTION_SAVE,
                                  m_filename);
    std::string const& filename = fc.get_filename();
    if (filename.empty())
    {
        save_as_xml(filename);
    }
}

void CrossoverHistory::save_as_xml(const std::string& filename)
{
#ifndef NDEBUG
    std::cout << _("save as ok") << "\n";
#endif
    try
    {
        m_crossover_list.to_xml(filename);
        m_filename = filename;

        {
            auto crossover_label = Gtk::manage(new Gtk::Label());
            crossover_label->set_markup("<b>" + Glib::ustring(_("Crossover list ["))
                                        + GSpeakers::short_filename(m_filename, 20) + "]</b>");
            set_label_widget(*crossover_label);
        }
        signal_crossover_set_save_state(false);
    }
    catch (std::runtime_error const& e)
    {
        Gtk::MessageDialog(e.what(), false, Gtk::MESSAGE_ERROR).run();
    }
}

void CrossoverHistory::on_remove()
{
    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

    if (const Gtk::TreeIter iter = refSelection->get_selected())
    {
        Gtk::TreePath path = m_refListStore->get_path(iter);
        if (!path.empty())
        {
            // Remove item from ListStore:
            m_refListStore->erase(iter);

            if (index < (int)m_crossover_list.data().size())
                m_crossover_list.data().erase(m_crossover_list.data().begin()
                                                        + index);
        }
    }
    Gtk::TreePath path(std::to_string(index > 0 ? index - 1 : 0));
    Gtk::TreeRow row = *(m_refListStore->get_iter(path));
    refSelection->select(row);
    signal_crossover_set_save_state(true);
}

void CrossoverHistory::create_model()
{
    m_refListStore = Gtk::ListStore::create(m_columns);

    std::for_each(m_crossover_list.data().begin(),
                  m_crossover_list.data().end(),
                  sigc::mem_fun(*this, &CrossoverHistory::add_item));
}

void CrossoverHistory::add_columns()
{
    /*
      {
        Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

        int cols_count =m_TreeView.append_column(_("Id"), *pRenderer);
        Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

        pColumn->add_attribute(pRenderer->property_text(), m_columns.id);
      }
    */
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Identifier"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->add_attribute(pRenderer->property_text(), m_columns.id_string);
    }

    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Type"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this, &CrossoverHistory::type_cell_data_func));
        //    pColumn->add_attribute(pRenderer->property_text(), m_columns.type);
    }
}

void CrossoverHistory::type_cell_data_func(Gtk::CellRenderer* cell,
                                           const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    std::string s;
    if (((*iter)[m_columns.type] & CROSSOVER_TYPE_LOWPASS) != 0)
    {
        s = s + _("lowpass");
    }
    if (((*iter)[m_columns.type] & CROSSOVER_TYPE_SUBSONIC) != 0)
    {
        if (s.length() > 0)
        {
            s = s + _(", subsonic");
        }
        else
        {
            s = s + _("subsonic");
        }
    }
    if (((*iter)[m_columns.type] & CROSSOVER_TYPE_HIGHPASS) != 0)
    {
        if (s.length() > 0)
        {
            s = s + _(", highpass");
        }
        else
        {
            s = s + _("highpass");
        }
    }
    if (((*iter)[m_columns.type] & CROSSOVER_TYPE_TWOWAY) != 0)
    {
        if (s.length() > 0)
        {
            s = s + _(", 2-way");
        }
        else
        {
            s = s + _("2-way");
        }
    }
    if (((*iter)[m_columns.type] & CROSSOVER_TYPE_THREEWAY) != 0)
    {
        if (s.length() > 0)
        {
            s = s + _(", 3-way");
        }
        else
        {
            s = s + _("3-way");
        }
    }
    if (((*iter)[m_columns.type] & CROSSOVER_TYPE_FOURWAY) != 0)
    {
        if (s.length() > 0)
        {
            s = s + _(", 4-way");
        }
        else
        {
            s = s + _("4-way");
        }
    }
    renderer.property_text() = s;
}

void CrossoverHistory::add_item(Crossover const& foo)
{
    Gtk::TreeRow row = *(m_refListStore->append());
    row[m_columns.id] = foo.get_id();
    row[m_columns.id_string] = foo.get_id_string();
    row[m_columns.type] = foo.get_type();
}
