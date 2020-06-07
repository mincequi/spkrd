/*
  $Id$

  crossovertreeview Copyright (C) 2002 Daniel Sundberg

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

#include "crossover_tree_view.hpp"

#include "signal.hpp"
#include "common.h"

#include <gtkmm/messagedialog.h>

namespace spkrd
{
crossover_tree_view::crossover_tree_view() : Gtk::Frame("")
{
    set_border_width(2);
    set_shadow_type(Gtk::SHADOW_NONE);
    static_cast<Gtk::Label*>(get_label_widget())
        ->set_markup("<b>" + Glib::ustring(_("Currently selected crossover")) + "</b>");

    m_scrolled_window.set_border_width(12);
    add(m_scrolled_window);

    m_scrolled_window.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
    m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    this->create_model();

    // create tree view
    m_tree_view.set_model(m_tree_store);
    m_tree_view.set_rules_hint();

    add_columns();
    m_scrolled_window.add(m_tree_view);

    signal_crossover_selected.connect(
        sigc::mem_fun(*this, &crossover_tree_view::on_crossover_selected));
    signal_net_modified_by_wizard.connect(
        sigc::mem_fun(*this, &crossover_tree_view::on_net_modified_by_wizard));

    show_all_children();
}

crossover_tree_view::~crossover_tree_view() = default;

void crossover_tree_view::on_net_modified_by_wizard()
{
    std::puts("crossover_tree_view::on_net_modified_by_wizard");

    this->on_crossover_selected(m_current_crossover);
}

void crossover_tree_view::on_cell_edited_value(Glib::ustring const& path_string,
                                               Glib::ustring const& new_text)
{
    Gtk::TreePath path(path_string);

    if (path.empty())
    {
        return;
    }

    auto row = *(m_tree_store->get_iter(path));

    auto const new_value = std::atof(new_text.c_str());

    if (new_value < 0.0)
    {
        Gtk::MessageDialog(_("The input value must be positive"), false, Gtk::MESSAGE_ERROR)
            .run();
        return;
    }

    row[m_columns.value] = new_value;

    // Since the stupid signals doesn't seem to work we have to go through the
    // data-containers and update values for the particular part we change.

    // Index is a counter for the extra circuits (impedance correction network,
    // damping network...) we have a number of extra circuits + the crossover
    // is the total number of "base nodes" after filter type nodes
    filter_network* network = &(m_current_crossover->networks())[path[0]];

    int ndx = 0;
    bool is_modified = false;

    if (network->get_has_imp_corr())
    {
        // Check if we have edited the impedance correction
        if (path[1] == ndx)
        {
            // If we have edited imp corr, update the appropriate component
            switch (path[2])
            {
                case 0:
                    network->get_imp_corr_C().set_value(row[m_columns.value]);
                    is_modified = true;
                    break;
                case 1:
                    network->get_imp_corr_R().set_value(row[m_columns.value]);
                    is_modified = true;
                    break;
            }
        }
        ndx++;
    }
    if (network->get_has_damp())
    {
        if (path[1] == ndx)
        {
            switch (path[2])
            {
                case 0:
                    network->get_damp_R1().set_value(row[m_columns.value]);
                    is_modified = true;
                    break;
                case 1:
                    network->get_damp_R2().set_value(row[m_columns.value]);
                    is_modified = true;
                    break;
            }
        }
        ndx++;
    }
    if (network->get_has_res())
    {
        if (path[1] == ndx)
        {
            switch (path[2])
            {
                case 0:
                    network->get_res_L().set_value(row[m_columns.value]);
                    is_modified = true;
                    break;
                case 1:
                    network->get_res_C().set_value(row[m_columns.value]);
                    is_modified = true;
                    break;
                case 2:
                    network->get_res_R().set_value(row[m_columns.value]);
                    is_modified = true;
                    break;
            }
        }
        ndx++;
    }

    // If we did not modified anything until here, the actual crossover is
    // the circuit to modify
    if (!is_modified)
    {
        // check path[2], if this is 0 we edit the part with offset 0
        // if path[2] = 1 we have a bandpassfilter and we should add an offset
        // of the lowpass filter order to path[3] which is the index of the
        // highpass part to change
        if (path[2] == 0)
        {
            network->parts()[path[3]].set_value(row[m_columns.value]);
        }
        else if (path[2] == 1)
        {
            network->parts()[path[3] + network->get_lowpass_order()].set_value(
                row[m_columns.value]);
        }
    }
    signal_net_modified_by_user(network);
}

crossover_tree_view::model_columns::model_columns()
{
    add(id_string);
    add(id);
    add(type);
    add(value);
    add(unit);
    add(editable);
    add(visible);
}

void crossover_tree_view::on_crossover_selected(Crossover* new_crossover)
{
    m_tree_store->clear();
    m_items.clear();

    m_current_crossover = new_crossover;

    for (auto network : m_current_crossover->networks())
    {
        std::vector<crossover_cell_item> crossover_elements;
        std::vector<crossover_cell_item> filter;
        std::vector<crossover_cell_item> lowpass_filter;
        std::vector<crossover_cell_item> highpass_filter;
        std::vector<crossover_cell_item> imp_corr;
        std::vector<crossover_cell_item> damp;

        if (network.get_has_imp_corr())
        {
            imp_corr.emplace_back(crossover_cell_item(network.get_imp_corr_C()));
            imp_corr.emplace_back(crossover_cell_item(network.get_imp_corr_R()));
            crossover_elements.emplace_back(
                crossover_cell_item(_("Impedance correction"), imp_corr));
        }
        if (network.get_has_damp())
        {
            damp.emplace_back(crossover_cell_item(network.get_damp_R1()));
            damp.emplace_back(crossover_cell_item(network.get_damp_R2()));
            crossover_elements.emplace_back(crossover_cell_item(_("Damping network"), damp));
        }

        // The rest of the parts
        auto const type = network.get_type();

        auto const& parts = network.parts();

        int counter = 0;

        if (type == NET_TYPE_LOWPASS)
        {
            int nof_lowpass_parts = network.get_lowpass_order();

            for (; counter < nof_lowpass_parts; ++counter)
            {
                lowpass_filter.emplace_back(parts[counter]);
            }
            filter.emplace_back(_("Low pass"), lowpass_filter);
        }
        if (type == NET_TYPE_HIGHPASS)
        {
            for (; (unsigned)counter < parts.size(); counter++)
            {
                highpass_filter.emplace_back(parts[counter]);
            }
            filter.emplace_back(_("High pass"), highpass_filter);
        }
        crossover_elements.emplace_back(_("Filter"), filter);

        switch (type)
        {
            case NET_TYPE_LOWPASS:
                m_items.emplace_back(_("Low pass"), crossover_elements);
                break;
            case NET_TYPE_BANDPASS:
                m_items.emplace_back(_("Band pass"), crossover_elements);
                break;
            case NET_TYPE_HIGHPASS:
                m_items.emplace_back(_("High pass"), crossover_elements);
                break;
            default:
                m_items.emplace_back(_("Other"), crossover_elements);
        }
    }

    std::for_each(begin(m_items),
                  end(m_items),
                  sigc::mem_fun(*this, &crossover_tree_view::treestore_add_item));

    m_tree_view.expand_all();
}

void crossover_tree_view::create_model()
{
    m_tree_store = Gtk::TreeStore::create(m_columns);

    std::for_each(begin(m_items),
                  end(m_items),
                  sigc::mem_fun(*this, &crossover_tree_view::treestore_add_item));
}

void crossover_tree_view::treestore_add_item(crossover_cell_item const& item)
{
    auto row = *(m_tree_store->append());

    row[m_columns.id_string] = item.m_label;
    row[m_columns.type] = 0;
    row[m_columns.value] = 0.0;
    row[m_columns.unit] = "";
    row[m_columns.editable] = false;
    row[m_columns.id] = 0;
    row[m_columns.visible] = false;

    for (crossover_cell_item const& child : item.m_children)
    {
        auto child_row = *(m_tree_store->append(row.children()));
        child_row[m_columns.id_string] = child.m_label;
        child_row[m_columns.id] = 0;
        child_row[m_columns.visible] = false;
        child_row[m_columns.editable] = false;

        for (crossover_cell_item const& child2 : child.m_children)
        {
            auto child_row2 = *(m_tree_store->append(child_row.children()));
            // If this is the filter parts node
            if ((child2.m_label == _("High pass")) || (child2.m_label == _("Low pass")))
            {
                /* One more node with the filter type */
                child_row2[m_columns.id_string] = child2.m_label;
                child_row2[m_columns.id] = 0;
                child_row2[m_columns.visible] = false;
                child_row2[m_columns.editable] = false;

                // Then insert the parts
                for (crossover_cell_item const& child3 : child2.m_children)
                {
                    auto child_row3 = *(m_tree_store->append(child_row2.children()));
                    child_row3[m_columns.id_string] = child3.m_label;
                    child_row3[m_columns.id] = child3.m_id;
                    child_row3[m_columns.type] = child3.m_type;
                    child_row3[m_columns.value] = child3.m_value;
                    child_row3[m_columns.unit] = child3.m_unit;
                    child_row3[m_columns.editable] = true;
                    child_row3[m_columns.visible] = true;
                }
            }
            else
            {
                // Impedance correction or damping network
                child_row2[m_columns.id_string] = child2.m_label;
                child_row2[m_columns.id] = child2.m_id;
                child_row2[m_columns.type] = child2.m_type;
                child_row2[m_columns.value] = child2.m_value;
                child_row2[m_columns.unit] = child2.m_unit;
                child_row2[m_columns.editable] = true;
                child_row2[m_columns.visible] = true;
            }
        }
    }
}

void crossover_tree_view::add_columns()
{
    {
        auto const column_sizes = m_tree_view.append_column(_("Identifier"),
                                                            m_columns.id_string);
        m_tree_view.get_column(column_sizes - 1)->set_resizable();
    }
    {
        m_treeviewcolumn_validated.set_title("Value");
        m_treeviewcolumn_validated.pack_start(m_cellrenderer_validated);

        m_tree_view.append_column(m_treeviewcolumn_validated);

        m_cellrenderer_validated.property_editable() = true;
        m_cellrenderer_validated.property_xalign().set_value(0.0);
        m_cellrenderer_validated.property_yalign().set_value(0.5);

        m_cellrenderer_validated.signal_edited().connect(
            sigc::mem_fun(*this, &crossover_tree_view::on_cell_edited_value));

        m_treeviewcolumn_validated
            .set_cell_data_func(m_cellrenderer_validated,
                                sigc::mem_fun(*this,
                                              &crossover_tree_view::value_cell_data_func));

        m_treeviewcolumn_validated.add_attribute(m_cellrenderer_validated.property_editable(),
                                                 m_columns.editable);
        m_treeviewcolumn_validated.add_attribute(m_cellrenderer_validated.property_visible(),
                                                 m_columns.visible);

        m_treeviewcolumn_validated.set_min_width(55);
        m_treeviewcolumn_validated.set_resizable();
    }
    {
        auto* renderer = Gtk::make_managed<Gtk::CellRendererText>();
        renderer->property_xalign().set_value(0.0);
        renderer->property_yalign().set_value(0.5);

        auto* column = m_tree_view.get_column(
            m_tree_view.append_column(_("Unit"), *renderer) - 1);
        if (column != nullptr)
        {
            column->add_attribute(renderer->property_text(), m_columns.unit);
            column->add_attribute(renderer->property_visible(), m_columns.visible);
            column->set_resizable();
        }
    }
}

void crossover_tree_view::value_cell_data_func(Gtk::CellRenderer* cell,
                                               Gtk::TreeModel::iterator const& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = to_ustring((*iter)[m_columns.value], 3, 1);
    renderer.property_xalign() = 1.0;
    renderer.property_yalign() = 1.0;
}

void crossover_tree_view::on_realize()
{
    m_tree_view.expand_all();
    Frame::on_realize();
}
}
