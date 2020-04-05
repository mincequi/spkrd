/*
  $Id$

  enclosurepaned Copyright (C) 2002 Daniel Sundberg

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

#include "enclosure_pane.hpp"

#include <gtkmm/separatormenuitem.h>
#include <gtkmm/separatortoolitem.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/menutoolbutton.h>
#include <gtkmm/stock.h>

#include <iostream>

constexpr auto MENU_INDEX_SAVE = 6;
constexpr auto MENU_INDEX_DELETE = 8;
constexpr auto MENU_INDEX_DELETE_BOXPLOT = 13;
constexpr auto TOOLBAR_INDEX_SAVE = 4;
constexpr auto TOOLBAR_INDEX_DELETE = 5;
constexpr auto TOOLBAR_INDEX_DELETE_BOXPLOT = 10;

enclosure_pane::enclosure_pane()
{
    g_settings.settings_changed.connect(sigc::mem_fun(*this, &enclosure_pane::on_settings_changed));

    // Main pane for the enclosure tab
    add1(m_edit_vpaned);
    add2(m_plot_vpaned);
    g_settings.defaultValueUnsignedInt("BoxMainPanedPosition", 250);
    set_position(g_settings.getValueUnsignedInt("BoxMainPanedPosition"));

    // The left part, the editor and the history
    m_edit_vpaned.add1(box_editor);
    m_edit_vpaned.add2(box_history);
    g_settings.defaultValueUnsignedInt("BoxEditPanedPosition", 300);
    m_edit_vpaned.set_position(g_settings.getValueUnsignedInt("BoxEditPanedPosition"));

    // The right part, plot and plothistory
    m_plot_vpaned.add1(box_plot);
    m_plot_vpaned.add2(plot_history);
    g_settings.defaultValueUnsignedInt("BoxPlotPanedPosition", 300);
    m_plot_vpaned.set_position(g_settings.getValueUnsignedInt("BoxPlotPanedPosition"));

    signal_enclosure_set_save_state.connect(sigc::mem_fun(*this, &enclosure_pane::set_save_state));
    signal_remove_box_plot.connect(sigc::mem_fun(*this, &enclosure_pane::on_remove_boxplot));
    signal_select_plot.connect(sigc::mem_fun(*this, &enclosure_pane::on_plot_selected));
    signal_add_plot.connect(sigc::mem_fun(*this, &enclosure_pane::on_add_plot));
}

enclosure_pane::~enclosure_pane()
{
#ifndef NDEBUG
    std::puts("enclosure_pane::~enclosure_pane");
#endif
    g_settings.setValue("BoxMainPanedPosition", get_position());
    g_settings.setValue("BoxEditPanedPosition", m_edit_vpaned.get_position());
    g_settings.setValue("BoxPlotPanedPosition", m_plot_vpaned.get_position());
    try
    {
        g_settings.save();
    }
    catch (std::runtime_error const& error)
    {
        std::cout << "Main: " << error.what() << "\n";
    }
}

auto enclosure_pane::get_menu() -> Gtk::MenuItem&
{
    m_menu_item.set_label("Enclosure");

    // Create the drop down menu options
    auto enclosure_submenu = Gtk::manage(new Gtk::Menu());

    {
        auto new_enclosure = Gtk::manage(new Gtk::MenuItem("New Enclosure"));
        new_enclosure->signal_activate().connect(
            sigc::mem_fun(box_history, &enclosure_history::on_new));
        enclosure_submenu->append(*new_enclosure);
    }
    {
        auto copy = Gtk::manage(new Gtk::MenuItem("Copy"));
        copy->signal_activate().connect(sigc::mem_fun(box_history, &enclosure_history::on_new_copy));
        enclosure_submenu->append(*copy);
    }
    enclosure_submenu->append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
    {
        auto new_enclosure = Gtk::manage(new Gtk::MenuItem("New"));
        new_enclosure->signal_activate().connect(
            sigc::mem_fun(box_history, &enclosure_history::on_new_xml));
        enclosure_submenu->append(*new_enclosure);
    }
    {
        auto append_xml = Gtk::manage(new Gtk::MenuItem("Append XML"));
        append_xml->signal_activate().connect(
            sigc::mem_fun(box_history, &enclosure_history::on_append_xml));
        enclosure_submenu->append(*append_xml);
    }
    {
        auto open_xml = Gtk::manage(new Gtk::MenuItem("Open XML"));
        open_xml->signal_activate().connect(
            sigc::mem_fun(box_history, &enclosure_history::on_open_xml));
        enclosure_submenu->append(*open_xml);
    }

    enclosure_submenu->append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
    {
        auto save = Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::SAVE));
        save->signal_activate().connect(sigc::mem_fun(box_history, &enclosure_history::on_save));
        save->set_sensitive(false);
        enclosure_submenu->append(*save);
    }
    {
        auto save_as = Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::SAVE_AS));
        save_as->signal_activate().connect(sigc::mem_fun(box_history, &enclosure_history::on_save_as));
        save_as->set_sensitive(false);
        enclosure_submenu->append(*save_as);
    }
    enclosure_submenu->append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
    {
        auto optimise = Gtk::manage(new Gtk::MenuItem("Optimise Enclosure"));
        optimise->signal_activate().connect(
            sigc::mem_fun(box_editor, &enclosure_editor::on_optimize_button_clicked));
        enclosure_submenu->append(*optimise);
    }
    {
        auto plot = Gtk::manage(new Gtk::MenuItem("Plot"));
        plot->signal_activate().connect(
            sigc::mem_fun(box_editor, &enclosure_editor::on_append_to_plot_clicked));
        enclosure_submenu->append(*plot);
    }
    {
        auto delete_item = Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::DELETE));
        delete_item->signal_activate().connect(
            sigc::mem_fun(box_history, &enclosure_history::on_remove));
        enclosure_submenu->append(*delete_item);
    }
    enclosure_submenu->append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
    {
        auto delete_plot = Gtk::manage(new Gtk::MenuItem("Delete Plot"));
        delete_plot->signal_activate().connect(sigc::mem_fun(plot_history, &PlotHistory::on_remove));
        delete_plot->set_sensitive(false);
        enclosure_submenu->append(*delete_plot);
    }
    m_menu_item.set_submenu(*enclosure_submenu);

    return m_menu_item;
}

auto enclosure_pane::get_toolbar() -> Gtk::Toolbar&
{
    if (m_tbar == nullptr)
    {
        m_tbar = Gtk::manage(new Gtk::Toolbar());

        {
            auto tool_button = Gtk::manage(
                new Gtk::ToolButton(*Gtk::manage(new Gtk::Image(Gtk::Stock::NEW,
                                                                Gtk::ICON_SIZE_LARGE_TOOLBAR)),
                                    _("New")));
            tool_button->signal_clicked().connect(
                sigc::mem_fun(box_history, &enclosure_history::on_new));
            m_tbar->append(*tool_button);
        }
        {
            auto tool_button = Gtk::manage(
                new Gtk::ToolButton(*Gtk::manage(new Gtk::Image(Gtk::Stock::COPY,
                                                                Gtk::ICON_SIZE_LARGE_TOOLBAR)),
                                    _("Copy")));
            tool_button->signal_clicked().connect(
                sigc::mem_fun(box_history, &enclosure_history::on_new_copy));
            m_tbar->append(*tool_button);
        }
        m_tbar->append(*Gtk::manage(new Gtk::SeparatorToolItem()));
        {
            auto tool_button = Gtk::manage(
                new Gtk::ToolButton(*Gtk::manage(new Gtk::Image(Gtk::Stock::OPEN,
                                                                Gtk::ICON_SIZE_LARGE_TOOLBAR)),
                                    _("Open")));
            tool_button->signal_clicked().connect(
                sigc::mem_fun(box_history, &enclosure_history::on_open_xml));
            m_tbar->append(*tool_button);
        }
        {
            auto tool_button = Gtk::manage(
                new Gtk::ToolButton(*Gtk::manage(new Gtk::Image(Gtk::Stock::SAVE,
                                                                Gtk::ICON_SIZE_LARGE_TOOLBAR)),
                                    _("Save")));
            tool_button->signal_clicked().connect(
                sigc::mem_fun(box_history, &enclosure_history::on_save));
            m_tbar->append(*tool_button);
        }
        m_tbar->append(*Gtk::manage(new Gtk::SeparatorToolItem()));
        {
            auto tool_button = Gtk::manage(
                new Gtk::ToolButton(*Gtk::manage(new Gtk::Image(Gtk::Stock::DELETE,
                                                                Gtk::ICON_SIZE_LARGE_TOOLBAR)),
                                    _("Delete")));
            tool_button->signal_clicked().connect(
                sigc::mem_fun(box_history, &enclosure_history::on_remove));
            m_tbar->append(*tool_button);
        }
        {
            auto tool_button = Gtk::manage(
                new Gtk::ToolButton(GSpeakers::image_widget("opt_enclosure_24.png"), _("Optimize")));
            tool_button->signal_clicked().connect(
                sigc::mem_fun(box_editor, &enclosure_editor::on_optimize_button_clicked));
            m_tbar->append(*tool_button);
        }
        {
            auto tool_button = Gtk::manage(
                new Gtk::ToolButton(GSpeakers::image_widget("opt_enclosure_24.png"), _("Plot")));
            tool_button->signal_clicked().connect(
                sigc::mem_fun(box_editor, &enclosure_editor::on_append_to_plot_clicked));
            m_tbar->append(*tool_button);
        }
        m_tbar->append(*Gtk::manage(new Gtk::SeparatorToolItem()));
        {
            auto tool_button = Gtk::manage(
                new Gtk::ToolButton(*Gtk::manage(new Gtk::Image(Gtk::Stock::DELETE,
                                                                Gtk::ICON_SIZE_LARGE_TOOLBAR)),
                                    _("Delete Plot")));
            tool_button->signal_clicked().connect(
                sigc::mem_fun(plot_history, &PlotHistory::on_remove));
            m_tbar->append(*tool_button);
        }

        m_tbar->set_toolbar_style(
            static_cast<Gtk::ToolbarStyle>(g_settings.getValueUnsignedInt("ToolbarStyle")));

        m_tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(false);
    }
    return *m_tbar;
}

void enclosure_pane::on_settings_changed(const std::string& setting)
{
    if (setting == "ToolbarStyle")
    {
        m_tbar->set_toolbar_style(
            static_cast<Gtk::ToolbarStyle>(g_settings.getValueUnsignedInt("ToolbarStyle")));
    }
}

void enclosure_pane::set_save_state(bool b)
{
    if (m_tbar != nullptr)
    {
        m_tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(b);
    }
    if (!m_tbar->get_children().empty())
    {
        //   m_menu.items()[MENU_INDEX_SAVE].set_sensitive(b);
        GSpeakers::enclosurelist_modified() = b;
    }
}

void enclosure_pane::on_plot_selected(int)
{
#ifndef NDEBUG
    std::puts("EnclsourePaned::on_plot_selected");
#endif
    plot_selected = true;
    m_tbar->get_nth_item(TOOLBAR_INDEX_DELETE_BOXPLOT)->set_sensitive(true);
    // m_menu.items()[MENU_INDEX_DELETE_BOXPLOT].set_sensitive(true);
}

void enclosure_pane::on_remove_boxplot(int)
{
#ifndef NDEBUG
    std::cout << "EnclsourePaned::on_remove_boxplot: nof_plots = " << nof_plots << "\n";
#endif
    if (nof_plots > 0)
    {
        --nof_plots;
    }
    if (nof_plots < 1)
    {
        m_tbar->get_nth_item(TOOLBAR_INDEX_DELETE_BOXPLOT)->set_sensitive(false);
        // m_menu.items()[MENU_INDEX_DELETE_BOXPLOT].set_sensitive(false);
    }
}

void enclosure_pane::on_add_plot(enclosure* b, driver* s, Gdk::Color& c)
{
#ifndef NDEBUG
    std::puts("EnclsourePaned::on_add_plot");
#endif
    ++nof_plots;
}
