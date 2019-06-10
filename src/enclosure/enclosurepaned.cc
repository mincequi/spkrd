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

#include "enclosurepaned.h"

constexpr auto MENU_INDEX_SAVE = 6;
constexpr auto MENU_INDEX_DELETE = 8;
constexpr auto MENU_INDEX_DELETE_BOXPLOT = 13;
constexpr auto TOOLBAR_INDEX_SAVE = 4;
constexpr auto TOOLBAR_INDEX_DELETE = 5;
constexpr auto TOOLBAR_INDEX_DELETE_BOXPLOT = 10;

EnclosurePaned::EnclosurePaned()
{
    g_settings.settings_changed.connect(sigc::mem_fun(*this, &EnclosurePaned::on_settings_changed));

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

    signal_enclosure_set_save_state.connect(sigc::mem_fun(*this, &EnclosurePaned::set_save_state));
    signal_remove_box_plot.connect(sigc::mem_fun(*this, &EnclosurePaned::on_remove_boxplot));
    signal_select_plot.connect(sigc::mem_fun(*this, &EnclosurePaned::on_plot_selected));
    signal_add_plot.connect(sigc::mem_fun(*this, &EnclosurePaned::on_add_plot));
}

EnclosurePaned::~EnclosurePaned()
{
#ifdef OUTPUT_DEBUG
    std::puts("EnclosurePaned::~EnclosurePaned");
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
        std::cout << "Main: " << error.what() << std::endl;
    }
}

Gtk::Menu& EnclosurePaned::get_menu()
{
    // FIXME gtk3 port

    // Gtk::Menu::MenuList& menulist = m_menu.items();
    //
    // Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
    // menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("_New Enclosure"), *im, sigc::mem_fun(box_history, &BoxHistory::on_new)));
    //
    // im = Gtk::manage(new Gtk::Image(Gtk::Stock::COPY, Gtk::ICON_SIZE_MENU));
    // menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("_Copy"), *im, sigc::mem_fun(box_history, &BoxHistory::on_new_copy)));
    //
    // menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
    //
    // im = Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
    // menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("N_ew"), *im, sigc::mem_fun(box_history, &BoxHistory::on_new_xml)));
    //
    // menulist.push_back(Gtk::Menu_Helpers::MenuElem(
    //     _("A_ppend Enclosure Xml..."), sigc::mem_fun(box_history, &BoxHistory::on_append_xml)));
    //
    // im = Gtk::manage(new Gtk::Image(Gtk::Stock::OPEN, Gtk::ICON_SIZE_MENU));
    // menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("_Open..."), *im, sigc::mem_fun(box_history, &BoxHistory::on_open_xml)));
    //
    // menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
    //
    // im = Gtk::manage(new Gtk::Image(Gtk::Stock::SAVE, Gtk::ICON_SIZE_MENU));
    // menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("_Save"), *im, sigc::mem_fun(box_history, &BoxHistory::on_save)));
    //
    // im = Gtk::manage(new Gtk::Image(Gtk::Stock::SAVE_AS, Gtk::ICON_SIZE_MENU));
    // menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("Save _As..."), *im, sigc::mem_fun(box_history, &BoxHistory::on_save_as)));
    //
    // menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
    //
    // menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("_Optimize Enclosure"), GSpeakers::image_widget("opt_enclosure_16.png"),
    //     sigc::mem_fun(box_editor, &BoxEditor::on_optimize_button_clicked)));
    // menulist.push_back(Gtk::Menu_Helpers::MenuElem(
    //     _("_Plot"), sigc::mem_fun(box_editor, &BoxEditor::on_append_to_plot_clicked)));
    //
    // im = Gtk::manage(new Gtk::Image(Gtk::Stock::DELETE, Gtk::ICON_SIZE_MENU));
    // menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("_Delete"), *im, sigc::mem_fun(box_history, &BoxHistory::on_remove)));
    //
    // menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
    //
    // im = Gtk::manage(new Gtk::Image(Gtk::Stock::DELETE, Gtk::ICON_SIZE_MENU));
    // menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("Delete Plot"), *im, sigc::mem_fun(plot_history, &PlotHistory::on_remove)));
    //
    // menulist[MENU_INDEX_SAVE].set_sensitive(false);
    // menulist[MENU_INDEX_DELETE_BOXPLOT].set_sensitive(false);
    return m_menu;
}

Gtk::Widget& EnclosurePaned::get_toolbar()
{
    if (m_tbar == nullptr)
    {
        m_tbar = Gtk::manage(new Gtk::Toolbar());

        Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_LARGE_TOOLBAR));
        Gtk::ToolButton* t = Gtk::manage(new Gtk::ToolButton(*im, _("New")));
        t->signal_clicked().connect(sigc::mem_fun(box_history, &BoxHistory::on_new));
        m_tbar->append(*t);

        im = Gtk::manage(new Gtk::Image(Gtk::Stock::COPY, Gtk::ICON_SIZE_LARGE_TOOLBAR));
        t = Gtk::manage(new Gtk::ToolButton(*im, _("Copy")));
        t->signal_clicked().connect(sigc::mem_fun(box_history, &BoxHistory::on_new_copy));
        m_tbar->append(*t);

        // Gtk::SeparatorToolItem* s = Gtk::manage(new Gtk::SeparatorToolItem());
        // m_tbar->append(*s);

        im = Gtk::manage(new Gtk::Image(Gtk::Stock::OPEN, Gtk::ICON_SIZE_LARGE_TOOLBAR));
        t = Gtk::manage(new Gtk::ToolButton(*im, _("Open")));
        t->signal_clicked().connect(sigc::mem_fun(box_history, &BoxHistory::on_open_xml));
        m_tbar->append(*t);

        im = Gtk::manage(new Gtk::Image(Gtk::Stock::SAVE, Gtk::ICON_SIZE_LARGE_TOOLBAR));
        t = Gtk::manage(new Gtk::ToolButton(*im, _("Save")));
        t->signal_clicked().connect(sigc::mem_fun(box_history, &BoxHistory::on_save));
        m_tbar->append(*t);

        // s = Gtk::manage(new Gtk::SeparatorToolItem());
        // m_tbar->append(*s);

        im = Gtk::manage(new Gtk::Image(Gtk::Stock::DELETE, Gtk::ICON_SIZE_LARGE_TOOLBAR));
        t = Gtk::manage(new Gtk::ToolButton(*im, _("Delete")));
        t->signal_clicked().connect(sigc::mem_fun(box_history, &BoxHistory::on_remove));
        m_tbar->append(*t);

        t = Gtk::manage(
            new Gtk::ToolButton(GSpeakers::image_widget("opt_enclosure_24.png"), _("Optimize")));
        t->signal_clicked().connect(sigc::mem_fun(box_editor, &BoxEditor::on_optimize_button_clicked));
        m_tbar->append(*t);

        t = Gtk::manage(
            new Gtk::ToolButton(GSpeakers::image_widget("opt_enclosure_24.png"), _("Plot")));
        t->signal_clicked().connect(sigc::mem_fun(box_editor, &BoxEditor::on_append_to_plot_clicked));
        m_tbar->append(*t);

        // s = Gtk::manage(new Gtk::SeparatorToolItem());
        // m_tbar->append(*s);

        im = Gtk::manage(new Gtk::Image(Gtk::Stock::DELETE, Gtk::ICON_SIZE_LARGE_TOOLBAR));
        t = Gtk::manage(new Gtk::ToolButton(*im, _("Delete Plot")));

        t->signal_clicked().connect(sigc::mem_fun(plot_history, &PlotHistory::on_remove));
        m_tbar->append(*t);

        m_toolbar.add(*m_tbar);
        m_tbar->set_toolbar_style(
            static_cast<Gtk::ToolbarStyle>(g_settings.getValueUnsignedInt("ToolbarStyle")));

        m_tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(false);
        // FIXME gtk3 port
        // m_tbar->get_nth_item(TOOLBAR_INDEX_DELETE_BOXPLOT)->set_sensitive(false);
    }
    return m_toolbar;
}

void EnclosurePaned::on_settings_changed(const std::string& setting)
{
    if (setting == "ToolbarStyle")
    {
        m_tbar->set_toolbar_style(
            static_cast<Gtk::ToolbarStyle>(g_settings.getValueUnsignedInt("ToolbarStyle")));
    }
}

void EnclosurePaned::set_save_state(bool b)
{
    if (m_tbar != nullptr)
    {
        m_tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(b);
    }
    // if (m_menu.items().size() > 0) {
    //   m_menu.items()[MENU_INDEX_SAVE].set_sensitive(b);
    //   GSpeakers::enclosurelist_modified() = b;
    // }
}

void EnclosurePaned::on_plot_selected(int)
{
#ifdef OUTPUT_DEBUG
    std::puts("EnclsourePaned::on_plot_selected");
#endif
    plot_selected = true;
    m_tbar->get_nth_item(TOOLBAR_INDEX_DELETE_BOXPLOT)->set_sensitive(true);
    // m_menu.items()[MENU_INDEX_DELETE_BOXPLOT].set_sensitive(true);
}

void EnclosurePaned::on_remove_boxplot(int)
{
#ifdef OUTPUT_DEBUG
    std::cout << "EnclsourePaned::on_remove_boxplot: nof_plots = " << nof_plots << std::endl;
#endif
    if (nof_plots > 0)
    {
        nof_plots--;
    }
    if (nof_plots < 1)
    {
        m_tbar->get_nth_item(TOOLBAR_INDEX_DELETE_BOXPLOT)->set_sensitive(false);
        // m_menu.items()[MENU_INDEX_DELETE_BOXPLOT].set_sensitive(false);
    }
}

void EnclosurePaned::on_add_plot(Box* b, Speaker* s, Gdk::Color& c)
{
#ifdef OUTPUT_DEBUG
    std::cout << "EnclsourePaned::on_add_plot" << std::endl;
#endif
    nof_plots++;
}
