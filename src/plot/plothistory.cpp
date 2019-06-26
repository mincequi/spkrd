/*
  $Id$

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

#include "plothistory.h"
#include "plot.hpp"

#include <cmath>
#include <ctime>
#include <iostream>

PlotHistory::PlotHistory() : Gtk::Frame(""), m_vbox()
{
    set_border_width(2);
    set_shadow_type(Gtk::SHADOW_NONE);

    m_label.set_markup("<b>" + Glib::ustring(_("Plot list")) + "</b>");
    set_label_widget(m_label);

    m_vbox.set_border_width(12);

    add(m_vbox);

    m_vbox.pack_start(m_ScrolledWindow);

    m_ScrolledWindow.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    create_model();

    // create tree view
    m_TreeView.set_model(m_refListStore);
    m_TreeView.set_rules_hint();

    Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();
    selection->signal_changed().connect(mem_fun(*this, &PlotHistory::on_selection_changed));

    signal_box_modified.connect(mem_fun(*this, &PlotHistory::on_box_modified));
    signal_add_plot.connect(mem_fun(*this, &PlotHistory::on_add_plot));

    add_columns();

    m_ScrolledWindow.add(m_TreeView);

    show_all();
}

PlotHistory::~PlotHistory() = default;

bool PlotHistory::on_delete_event(GdkEventAny* event) { return true; }

void PlotHistory::on_selection_changed()
{
    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

    if (const Gtk::TreeIter iter = refSelection->get_selected())
    {
        Gtk::TreePath path = m_refListStore->get_path(iter);

        if (!path.empty())
        {
            // Check config if user want to mark selected plot
            signal_select_plot(path[0]);
            m_index = path[0];
        }
    }
}

void PlotHistory::on_remove()
{
    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

    if (const Gtk::TreeIter iter = refSelection->get_selected())
    {
        Gtk::TreePath path = m_refListStore->get_path(iter);

        if (!path.empty())
        {
            // Remove item from ListStore
            m_refListStore->erase(iter);
            // Signal to the plot that we got the plot index to remove in indices[0]
            signal_remove_box_plot(path[0]);
        }
    }

    if (m_nof_plots > 0)
    {
        --m_nof_plots;
    }

    if (m_nof_plots > 0)
    {
        // Select first row
        Gtk::TreePath path(Glib::ustring(std::to_string(m_index > 0 ? m_index - 1 : 0)));
        Gtk::TreeRow row = *(m_refListStore->get_iter(path));
        refSelection->select(row);
    }
}

void PlotHistory::on_box_modified(enclosure*) {}

void PlotHistory::on_add_plot(enclosure* b, driver* s, Gdk::Color& color)
{
    if (b != nullptr && s != nullptr)
    {
        m_box_list.box_list().push_back(*b);

        m_speaker_list.data().emplace_back(*s);

        add_item(*b, *s, color);
    }
    ++m_nof_plots;
}

void PlotHistory::on_cell_plot_toggled(const Glib::ustring& path_string)
{
#ifndef NDEBUG
    std::puts("PlotHistory: toggle plot");
#endif

    Gtk::TreePath path(path_string);

    Gtk::TreeRow row = *(m_refListStore->get_iter(path));

    bool view_plot = row[m_columns.view_plot];

    // do something with the value
    view_plot = !view_plot;

    path = m_refListStore->get_path(row);

    if (!path.empty())
    {
        signal_hide_box_plot(path[0]);
    }
    // set new value
    row[m_columns.view_plot] = view_plot;
}

void PlotHistory::create_model() { m_refListStore = Gtk::ListStore::create(m_columns); }

void PlotHistory::add_columns()
{
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Color"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->add_attribute(pRenderer->property_text(), m_columns.color);

        pColumn->add_attribute(pRenderer->property_foreground_gdk(), m_columns.color_);
        pColumn->add_attribute(pRenderer->property_weight(), m_columns.weight_);
    }
    {
        Gtk::CellRendererToggle* pRenderer = Gtk::manage(new Gtk::CellRendererToggle());
        pRenderer->signal_toggled().connect(mem_fun(*this, &PlotHistory::on_cell_plot_toggled));

        int cols_count = m_TreeView.append_column(_("Plot"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->add_attribute(pRenderer->property_active(), m_columns.view_plot);
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Identifier"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->add_attribute(pRenderer->property_text(), m_columns.id_string);
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("driver"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->add_attribute(pRenderer->property_text(), m_columns.speaker_string);
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Type"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->set_cell_data_func(*pRenderer, mem_fun(*this, &PlotHistory::type_cell_data_func));
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Vb1"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->set_cell_data_func(*pRenderer, mem_fun(*this, &PlotHistory::vb1_cell_data_func));
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Fb1"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->set_cell_data_func(*pRenderer, mem_fun(*this, &PlotHistory::fb1_cell_data_func));
    }
}

void PlotHistory::type_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
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
}

void PlotHistory::vb1_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.vb1], 2, 1) + " l";
    renderer.property_xalign() = 1.0;
}

void PlotHistory::fb1_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.fb1], 2, 1) + " Hz";
    renderer.property_xalign() = 1.0;
}

void PlotHistory::add_item(enclosure const& box, driver const& spk, Gdk::Color& color)
{
    gushort r = (int)((color.get_red_p()) * 255);
    gushort g = (int)((color.get_green_p()) * 255);
    gushort b = (int)((color.get_blue_p()) * 255);

    char* str = nullptr;
    GString* buffer = g_string_new(str);
    g_string_printf(buffer, "#%.2X%.2X%.2X", r, g, b);

    Gtk::TreeRow row = *(m_refListStore->append());

    row[m_columns.color] = Glib::ustring(buffer->str);
    row[m_columns.view_plot] = true;
    row[m_columns.id] = box.get_id();
    row[m_columns.id_string] = box.get_id_string();
    row[m_columns.speaker_string] = spk.get_id_string();
    row[m_columns.type] = box.get_type();
    row[m_columns.vb1] = box.get_vb1();
    row[m_columns.fb1] = box.get_fb1();
    row[m_columns.vb2] = box.get_vb2();
    row[m_columns.fb2] = box.get_fb2();
    row[m_columns.color_] = color;
    row[m_columns.weight_] = Pango::WEIGHT_HEAVY;
}
