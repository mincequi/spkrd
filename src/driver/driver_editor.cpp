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

#include "driver_editor.hpp"

#include "common.h"
#include "frequency_response_editor.hpp"
#include "file_chooser.hpp"

#include <glibmm.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/separatortoolitem.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/separatormenuitem.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/stock.h>

#include <cmath>
#include <fstream>
#include <iostream>

constexpr auto MENU_INDEX_SAVE = 5;
constexpr auto MENU_INDEX_DELETE = 8;
constexpr auto TOOLBAR_INDEX_SAVE = 3;
constexpr auto TOOLBAR_INDEX_DELETE = 5;

constexpr auto FILE_CHOOSER_CANCEL = 10001;
constexpr auto FILE_CHOOSER_OPEN = 10002;
constexpr auto FILE_CHOOSER_SAVE = 10003;

driver_editor::driver_editor()
    : m_TreeViewTable(10, 4, true),
      m_EditFreqRespButton(_("Edit...")),
      m_BrowseFreqRespButton(_("...")),
      m_BassCheckButton(_("Bass")),
      m_MidrangeCheckButton(_("Midrange")),
      m_TweeterCheckButton(_("Tweeter")),
      m_vbox(Gtk::ORIENTATION_VERTICAL),
      m_treeview_vbox(Gtk::ORIENTATION_VERTICAL),
      m_inner_treeview_vbox(Gtk::ORIENTATION_VERTICAL),
      m_treeview_frame("")
{
#ifdef TARGET_WIN32
    g_settings.defaultValueString("SpeakerListXml", "vifa.xml");
#else
    g_settings.defaultValueString("SpeakerListXml",
                                  std::string(GSPEAKERS_PREFIX) + "/share/xml/vifa.xml");
#endif
    m_filename = g_settings.getValueString("SpeakerListXml");

    g_settings.defaultValueBool("DrawDriverImpPlot", false);
    g_settings.defaultValueBool("DrawDriverFreqRespPlot", false);

    try
    {
        m_driver_list = std::make_unique<driver_list>(m_filename);
        signal_speakerlist_loaded(m_driver_list.get());
    }
    catch (std::runtime_error const& e)
    {
        m_driver_list = std::make_unique<driver_list>();
        std::cout << "driver_editor::driver_editor: " << e.what() << std::endl;
    }

    m_treeview_vbox.set_border_width(5);
    m_treeview_vbox.pack_start(m_treeview_frame);

    m_treeview_frame.add(m_inner_treeview_vbox);
    m_treeview_frame.set_shadow_type(Gtk::SHADOW_NONE);

    m_evbox = Gtk::make_managed<Gtk::EventBox>();

    m_frame_label = Gtk::make_managed<Gtk::Label>();
    m_frame_label->set_markup("<b>" + Glib::ustring(_("Drivers ["))
                              + GSpeakers::short_filename(m_filename, 50) + "]</b>");

    static_cast<Gtk::Container*>(m_evbox)->add(*m_frame_label);

    m_treeview_frame.set_label_widget(*m_evbox);
    m_evbox->set_tooltip_text(m_filename);

    m_inner_treeview_vbox.set_border_width(12);
    m_inner_treeview_vbox.pack_start(m_TreeViewTable);

    auto input_scrolled_window = Gtk::make_managed<Gtk::ScrolledWindow>();
    input_scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    input_scrolled_window->add(m_grid);

    m_grid.set_row_spacing(10);
    // m_grid.set_column_spacing(2);

    m_vbox.set_border_width(10);

    m_vbox.pack_start(*input_scrolled_window);

    // Setup the table
    m_TreeViewTable.set_spacings(2);
    m_TreeViewTable.attach(m_ScrolledWindow, 0, 4, 0, 10);

    // All the entries
    m_grid.attach(*Gtk::make_managed<Gtk::Label>(_("Name:"), Gtk::ALIGN_START), 0, 0);
    m_grid.attach(m_IdStringEntry, 1, 0);
    m_grid.attach(*Gtk::make_managed<Gtk::Label>(_("Qts:"), Gtk::ALIGN_START), 0, 1);
    m_grid.attach(m_QtsEntry, 1, 1);
    m_grid.attach(*Gtk::make_managed<Gtk::Label>(_("Fs: (Hz)"), Gtk::ALIGN_START), 0, 2);
    m_grid.attach(m_FsEntry, 1, 2);
    m_grid.attach(*Gtk::make_managed<Gtk::Label>(_("Vas: (l)"), Gtk::ALIGN_START), 0, 3);
    m_grid.attach(m_VasEntry, 1, 3);
    m_grid.attach(*Gtk::make_managed<Gtk::Label>(_("Rdc: (Ohm)"), Gtk::ALIGN_START), 0, 4);
    m_grid.attach(m_RdcEntry, 1, 4);
    m_grid.attach(*Gtk::make_managed<Gtk::Label>(_("Lvc: (mH)"), Gtk::ALIGN_START), 0, 5);
    m_grid.attach(m_LvcEntry, 1, 5);
    m_grid.attach(*Gtk::make_managed<Gtk::Label>(_("Qms:"), Gtk::ALIGN_START), 0, 6);
    m_grid.attach(m_QmsEntry, 1, 6);
    m_grid.attach(*Gtk::make_managed<Gtk::Label>(_("Qes:"), Gtk::ALIGN_START), 0, 7);
    m_grid.attach(m_QesEntry, 1, 7);
    m_grid.attach(*Gtk::make_managed<Gtk::Label>(_("Impedance: (Ohm)"), Gtk::ALIGN_START), 0, 8);
    m_grid.attach(m_ImpEntry, 1, 8);
    m_grid.attach(*Gtk::make_managed<Gtk::Label>(_("Sensitivity: (dB/W/m)"), Gtk::ALIGN_START), 0, 9);
    m_grid.attach(m_SensEntry, 1, 9);
    m_grid.attach(*Gtk::make_managed<Gtk::Label>(_("Cone mass: (kg)"), Gtk::ALIGN_START), 0, 10);
    m_grid.attach(m_MmdEntry, 1, 10);
    m_grid.attach(*Gtk::make_managed<Gtk::Label>(_("Effective radius: (m)"), Gtk::ALIGN_START), 0, 11);
    m_grid.attach(m_AdEntry, 1, 11);
    m_grid.attach(*Gtk::make_managed<Gtk::Label>(_("Force factor: (N/A)"), Gtk::ALIGN_START), 0, 12);
    m_grid.attach(m_BlEntry, 1, 12);
    m_grid.attach(*Gtk::make_managed<Gtk::Label>(_("Suspension resistance: (Ns/m"), Gtk::ALIGN_START),
                  0,
                  13);
    m_grid.attach(m_RmsEntry, 1, 13);
    m_grid.attach(*Gtk::make_managed<Gtk::Label>(_("Suspension compliance: (m/N)"), Gtk::ALIGN_START),
                  0,
                  14);
    m_grid.attach(m_CmsEntry, 1, 14);

    m_grid.attach(m_BassCheckButton, 0, 15);
    m_grid.attach(m_MidrangeCheckButton, 0, 16);
    m_grid.attach(m_TweeterCheckButton, 0, 17);

    m_IdStringEntry.set_tooltip_text(_("The name or identification string for the driver"));
    m_FsEntry.set_tooltip_text(_("Free air resonance frequency"));
    m_VasEntry.set_tooltip_text(_("Equivalent volume of air that has a compliance equal to that of "
                                  "the air displaced by the driver"));
    m_RdcEntry.set_tooltip_text(_("Voice coil DC resistance"));
    m_LvcEntry.set_tooltip_text(_("Voice coil inductance"));
    m_QtsEntry.set_tooltip_text(_("Parallel combination of Qms and Qes"));
    m_QmsEntry.set_tooltip_text(_("Ratio of the driver electrical equivalent frictional resistance "
                                  "to the reflected motional reactance at Fs"));
    m_QesEntry.set_tooltip_text(_("Ratio of the voice coil DC resistance to the reflected motional "
                                  "reactance at fs."));
    m_ImpEntry.set_tooltip_text(_(""));
    m_SensEntry.set_tooltip_text(_(""));
    m_MmdEntry.set_tooltip_text(_(""));
    m_AdEntry.set_tooltip_text(_(""));
    m_BlEntry.set_tooltip_text(_(""));
    m_RmsEntry.set_tooltip_text(_(""));
    m_CmsEntry.set_tooltip_text(_(""));

    m_BassCheckButton.set_tooltip_text(_("Check if a woofer"));
    m_MidrangeCheckButton.set_tooltip_text(_("Check if a midrange driver"));
    m_TweeterCheckButton.set_tooltip_text(_("Check if a tweeter"));

    auto hbox = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);
    hbox->pack_start(*Gtk::make_managed<Gtk::Label>(_("Frequency response file:"), Gtk::ALIGN_START));
    hbox->pack_start(m_FreqRespFileEntry);
    m_FreqRespFileEntry.set_width_chars(15);
    hbox->pack_start(m_BrowseFreqRespButton);
    hbox->pack_start(m_EditFreqRespButton);
    hbox->set_spacing(12);
    m_grid.attach(*hbox, 0, 18, 2, 1);

    m_ScrolledWindow.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    m_IdStringEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 0));
    m_QtsEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 1));
    m_FsEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 2));
    m_VasEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 3));
    m_RdcEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 4));
    m_LvcEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 5));
    m_QmsEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 6));
    m_QesEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 7));
    m_ImpEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 8));
    m_SensEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 9));
    m_MmdEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 13));
    m_AdEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 14));
    m_BlEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 15));
    m_RmsEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 16));
    m_CmsEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 17));
    m_FreqRespFileEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 18));
    m_FreqRespFileEntry.set_editable(false);

    m_BassCheckButton.signal_toggled().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 10));
    m_MidrangeCheckButton.signal_toggled().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 11));
    m_TweeterCheckButton.signal_toggled().connect(
        sigc::bind(sigc::mem_fun(*this, &driver_editor::on_entry_changed), 12));

    m_EditFreqRespButton.signal_clicked().connect(
        sigc::mem_fun(*this, &driver_editor::on_edit_freq_resp));
    m_BrowseFreqRespButton.signal_clicked().connect(
        sigc::mem_fun(*this, &driver_editor::on_browse_freq_resp));

    m_EditFreqRespButton.set_tooltip_text(_("Edit frequency response for this driver"));
    m_BrowseFreqRespButton.set_tooltip_text(_("Browse for frequency response file"));

    set_entries_sensitive(false);

    create_model();

    // create tree view
    m_TreeView.set_model(m_refListStore);
    m_TreeView.set_rules_hint();
    auto selection = m_TreeView.get_selection();

    selection->signal_changed().connect(sigc::mem_fun(*this, &driver_editor::on_selection_changed));

    g_settings.settings_changed.connect(sigc::mem_fun(*this, &driver_editor::on_settings_changed));

    add_columns();
    m_ScrolledWindow.add(m_TreeView);

    signal_save_open_files.connect(sigc::mem_fun(*this, &driver_editor::on_save_open_files));
}

auto driver_editor::get_menu() -> Gtk::MenuItem&
{
    m_menu_item.set_label("Driver");

    // Create the drop down menu options
    auto driver_submenu = Gtk::manage(new Gtk::Menu());

    {
        auto new_driver = Gtk::make_managed<Gtk::MenuItem>("New Driver");
        new_driver->signal_activate().connect(sigc::mem_fun(*this, &driver_editor::on_new));
        driver_submenu->append(*new_driver);
    }
    driver_submenu->append(*Gtk::make_managed<Gtk::SeparatorMenuItem>());
    {
        auto new_driver = Gtk::make_managed<Gtk::MenuItem>("New");
        new_driver->signal_activate().connect(sigc::mem_fun(*this, &driver_editor::on_new_xml));
        driver_submenu->append(*new_driver);
    }
    {
        auto append_xml = Gtk::make_managed<Gtk::MenuItem>("Append XML");
        append_xml->signal_activate().connect(sigc::mem_fun(*this, &driver_editor::on_append_xml));
        driver_submenu->append(*append_xml);
    }
    {
        auto open_xml = Gtk::make_managed<Gtk::MenuItem>("Open XML");
        open_xml->signal_activate().connect(sigc::mem_fun(*this, &driver_editor::on_open_xml));
        driver_submenu->append(*open_xml);
    }
    driver_submenu->append(*Gtk::make_managed<Gtk::SeparatorMenuItem>());
    {
        auto save = Gtk::make_managed<Gtk::ImageMenuItem>(Gtk::Stock::SAVE);
        save->signal_activate().connect(sigc::mem_fun(*this, &driver_editor::on_save));
        driver_submenu->append(*save);
    }
    {
        auto save_as = Gtk::make_managed<Gtk::ImageMenuItem>(Gtk::Stock::SAVE_AS);
        save_as->signal_activate().connect(sigc::mem_fun(*this, &driver_editor::on_save_as));
        driver_submenu->append(*save_as);
    }
    driver_submenu->append(*Gtk::make_managed<Gtk::SeparatorMenuItem>());
    {
        auto delete_item = Gtk::make_managed<Gtk::ImageMenuItem>(Gtk::Stock::DELETE);
        delete_item->signal_activate().connect(sigc::mem_fun(*this, &driver_editor::on_remove));
        driver_submenu->append(*delete_item);
    }

    m_menu_item.set_submenu(*driver_submenu);

    return m_menu_item;

    // menulist[MENU_INDEX_SAVE].set_sensitive(false);
    // menulist[MENU_INDEX_DELETE].set_sensitive(false);
}

auto driver_editor::get_toolbar() -> Gtk::Toolbar&
{
    if (m_toolbar == nullptr)
    {
        m_toolbar = Gtk::make_managed<Gtk::Toolbar>();

        Gtk::Widget* im = Gtk::make_managed<Gtk::Image>(Gtk::Stock::NEW,
                                                        Gtk::ICON_SIZE_LARGE_TOOLBAR);
        Gtk::ToolButton* t = Gtk::make_managed<Gtk::ToolButton>(*im, _("New Driver"));
        t->signal_clicked().connect(sigc::mem_fun(*this, &driver_editor::on_new));
        m_toolbar->append(*t);

        m_toolbar->append(*Gtk::make_managed<Gtk::SeparatorToolItem>());

        t = Gtk::make_managed<Gtk::ToolButton>(Gtk::Stock::OPEN);
        t->signal_clicked().connect(sigc::mem_fun(*this, &driver_editor::on_open_xml));
        m_toolbar->append(*t);
        t = Gtk::make_managed<Gtk::ToolButton>(Gtk::Stock::SAVE);
        t->signal_clicked().connect(sigc::mem_fun(*this, &driver_editor::on_save));
        m_toolbar->append(*t);

        m_toolbar->append(*Gtk::make_managed<Gtk::SeparatorToolItem>());

        t = Gtk::make_managed<Gtk::ToolButton>(Gtk::Stock::DELETE);
        t->signal_clicked().connect(sigc::mem_fun(*this, &driver_editor::on_remove));
        m_toolbar->append(*t);

        m_toolbar->set_toolbar_style(
            static_cast<Gtk::ToolbarStyle>(g_settings.getValueUnsignedInt("ToolbarStyle")));

        m_toolbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(false);
        m_toolbar->get_nth_item(TOOLBAR_INDEX_DELETE)->set_sensitive(false);
    }
    return *m_toolbar;
}

void driver_editor::on_save_open_files()
{
    if (GSpeakers::driverlist_modified())
    {
        on_save();
    }
}

void driver_editor::on_settings_changed(const std::string& setting)
{
    if (setting == "ToolbarStyle")
    {
        m_toolbar->set_toolbar_style(
            static_cast<Gtk::ToolbarStyle>(g_settings.getValueUnsignedInt("ToolbarStyle")));
    }
    if (setting == "DrawDriverImpPlot" || setting == "DrawDriverFreqRespPlot")
    {
        if (index != -1)
        {
            on_selection_changed();
        }
    }
}

void driver_editor::on_close() { signal_speakerlist_loaded(m_driver_list.get()); }

void driver_editor::set_entries_sensitive(bool const is_sensitive)
{
    m_IdStringEntry.set_sensitive(is_sensitive);
    m_QtsEntry.set_sensitive(is_sensitive);
    m_FsEntry.set_sensitive(is_sensitive);
    m_VasEntry.set_sensitive(is_sensitive);
    m_RdcEntry.set_sensitive(is_sensitive);
    m_LvcEntry.set_sensitive(is_sensitive);
    m_QmsEntry.set_sensitive(is_sensitive);
    m_QesEntry.set_sensitive(is_sensitive);
    m_ImpEntry.set_sensitive(is_sensitive);
    m_SensEntry.set_sensitive(is_sensitive);
    m_MmdEntry.set_sensitive(is_sensitive);
    m_AdEntry.set_sensitive(is_sensitive);
    m_BlEntry.set_sensitive(is_sensitive);
    m_RmsEntry.set_sensitive(is_sensitive);
    m_CmsEntry.set_sensitive(is_sensitive);

    m_FreqRespFileEntry.set_sensitive(is_sensitive);
    m_ImpRespFileEntry.set_sensitive(is_sensitive);
    m_BassCheckButton.set_sensitive(is_sensitive);
    m_MidrangeCheckButton.set_sensitive(is_sensitive);
    m_TweeterCheckButton.set_sensitive(is_sensitive);
    m_BrowseFreqRespButton.set_sensitive(is_sensitive);
    m_EditFreqRespButton.set_sensitive(is_sensitive);

    if (!is_sensitive)
    {
        m_BassCheckButton.set_active(false);
        m_MidrangeCheckButton.set_active(false);
        m_TweeterCheckButton.set_active(false);
    }
}

void driver_editor::on_new()
{
    driver speaker(_("New driver"));

    speaker.set_id_string(speaker.get_id_string() + " " + std::to_string(speaker.get_id()));

    add_item(speaker);
    m_driver_list->data().push_back(speaker);

    Gtk::TreePath path(std::to_string(m_driver_list->data().size() - 1));
    Gtk::TreeRow row = *(m_refListStore->get_iter(path));
    m_TreeView.get_selection()->select(row);

    m_IdStringEntry.grab_focus();

    set_entries_sensitive(true);

    // FIXME gtk3 port
    // m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
    // m_menu.items()[MENU_INDEX_DELETE].set_sensitive(true);
    m_toolbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);
    m_toolbar->get_nth_item(TOOLBAR_INDEX_DELETE)->set_sensitive(true);
    GSpeakers::driverlist_modified() = true;
    signal_speakerlist_loaded(m_driver_list.get());
    m_modified = true;
}

void driver_editor::on_new_xml()
{
    m_refListStore->clear();
    m_driver_list->clear();

    set_entries_sensitive(false);
    new_xml_pressed = true;

    // Add one new speaker to the new xml file, we usually want to do this
    on_new();
}

void driver_editor::on_remove()
{
    if (const Gtk::TreeIter iter = m_TreeView.get_selection()->get_selected())
    {
        Gtk::TreePath path = m_refListStore->get_path(iter);

        if (!path.empty())
        {
            // Remove item from ListStore:
            m_refListStore->erase(iter);

            if (index < (int)m_driver_list->data().size())
                m_driver_list->data().erase(m_driver_list->data().begin() + index);
        }
    }
    // m_menu.items()[MENU_INDEX_DELETE].set_sensitive(false);
}

void driver_editor::on_save()
{
#ifndef NDEBUG
    std::puts("SpeakerEditor: save");
#endif
    if (new_xml_pressed)
    {
        on_save_as();
        new_xml_pressed = false;
    }
    else
    {
#ifndef NDEBUG
        std::cout << "SpeakerEditor: Filename = " << m_filename << "\n";
#endif
        try
        {
            m_driver_list->to_xml(m_filename);
            // FIXME gtk3 port
            // m_menu.items()[MENU_INDEX_SAVE].set_sensitive(false);
            m_toolbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(false);
            GSpeakers::driverlist_modified() = false;
            m_modified = false;
        }
        catch (std::runtime_error const& e)
        {
            Gtk::MessageDialog(e.what(), false, Gtk::MESSAGE_ERROR).run();
        }
    }
}

void driver_editor::on_save_as()
{
    std::string const& filename = file_chooser_dialog(_("Save speaker xml as"),
                                                      Gtk::FILE_CHOOSER_ACTION_SAVE,
                                                      m_filename)
                                      .get_filename();

    if (!filename.empty())
    {
        save_as(filename);
    }
}

void driver_editor::save_as(const std::string& filename)
{
    try
    {
        m_driver_list->to_xml(filename);
        m_filename = filename;
    }
    catch (std::runtime_error const& error)
    {
        Gtk::MessageDialog(error.what(), false, Gtk::MESSAGE_ERROR).run();
    }
}

void driver_editor::on_selection_changed()
{
    updating_entries = true;

    if (auto const iter = m_TreeView.get_selection()->get_selected())
    {
        Gtk::TreePath path = m_refListStore->get_path(iter);

        if (!path.empty())
        {
            index = path[0];
            driver const& speaker = m_driver_list->data()[index];

            m_IdStringEntry.set_text(speaker.get_id_string());

            char* str = nullptr;
            GString* buffer = g_string_new(str);
            g_string_printf(buffer, "%3.3f", speaker.get_qts());
            m_QtsEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%1.0f", speaker.get_fs());
            m_FsEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%1.3f", speaker.get_vas());
            m_VasEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%1.1f", speaker.get_rdc());
            m_RdcEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.2f", speaker.get_lvc());
            m_LvcEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.3f", speaker.get_qms());
            m_QmsEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.3f", speaker.get_qes());
            m_QesEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.2f", speaker.get_imp());
            m_ImpEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.2f", speaker.get_sens());
            m_SensEntry.set_text(Glib::ustring(buffer->str));

            g_string_printf(buffer, "%0.2f", speaker.get_mmd());
            m_MmdEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.4f", speaker.get_ad());
            m_AdEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.2f", speaker.get_bl());
            m_BlEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.2f", speaker.get_rms());
            m_RmsEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.4f", speaker.get_cms());
            m_CmsEntry.set_text(Glib::ustring(buffer->str));

            // Check buttons
            m_BassCheckButton.set_active(speaker.get_type() == SPEAKER_TYPE_BASS);
            m_MidrangeCheckButton.set_active(speaker.get_type() == SPEAKER_TYPE_MIDRANGE);
            m_TweeterCheckButton.set_active(speaker.get_type() == SPEAKER_TYPE_TWEETER);

            m_FreqRespFileEntry.set_text(speaker.get_freq_resp_filename());

            plot.clear();

            // Plot frequency response if it exists
            if (g_settings.getValueBool("DrawDriverFreqRespPlot")
                && !speaker.get_freq_resp_filename().empty())
            {
                std::ifstream input_file(speaker.get_freq_resp_filename().c_str());

                if (!input_file.is_open())
                {
                    throw std::runtime_error("Could not open " + speaker.get_freq_resp_filename());
                }

                std::vector<gspk::point> points;

                while (input_file)
                {
                    double frequency;
                    double magnitude;
                    char comma;

                    input_file >> frequency >> comma >> magnitude;

                    if (comma != ',')
                    {
                        throw std::runtime_error("Expected comma separated file for driver "
                                                 "frequency response curve "
                                                 + speaker.get_freq_resp_filename());
                    }
                    points.emplace_back(std::round(frequency), magnitude);
                }
                plot.add_plot(points, Gdk::Color("blue"));
            }
            // Plot impedance response increase impedance y coordinate 50 to align to imp scale
            draw_impedance_plot(speaker);
        }
    }
    updating_entries = false;
    m_IdStringEntry.grab_focus();
    set_entries_sensitive(true);

    // FIXME gtk3 port
    // m_menu.items()[MENU_INDEX_DELETE].set_sensitive(true);

    m_toolbar->get_nth_item(TOOLBAR_INDEX_DELETE)->set_sensitive(true);
}

void driver_editor::draw_impedance_plot(driver const& s, bool update)
{
    std::array<gchar, 8> buffer;

    if (g_settings.getValueBool("DrawDriverImpPlot"))
    {
        std::vector<gspk::point> points;
        // Produce SPICE input-file
#ifdef TARGET_WIN32
        std::string tmp_file = Glib::get_tmp_dir() + "\\speaker" + std::to_string(s.get_id())
                               + ".tmp";
#else
        std::string tmp_file = Glib::get_tmp_dir() + "/speaker" + std::to_string(s.get_id()) + ".tmp";
#endif

        std::ofstream of(tmp_file.c_str());

        if (of.good())
        {
            of << "SPICE code generated by GSpeakers Redux-" << VERSION << "\n";
            of << "vamp 1 0 dc 0 ac 1\n";

            auto const cmes = s.get_mmd() / (s.get_bl() * s.get_bl()) * 1000000;
            auto const lces = s.get_bl() * s.get_bl() * s.get_cms() * 1000;
            auto const res = s.get_bl() * s.get_bl() / s.get_rms();

            // air density kg/m^3
            constexpr auto po = 1.18;

            auto const cmef = 8 * po * s.get_ad() * s.get_ad() * s.get_ad()
                              / (3 * s.get_bl() * s.get_bl()) * 1000000;

            of << "R" << s.get_id() << " 1 2 " << g_ascii_dtostr(buffer.data(), 8, s.get_rdc())
               << "\n";
            of << "L" << s.get_id() << " 2 3 " << g_ascii_dtostr(buffer.data(), 8, s.get_lvc())
               << "mH"
               << "\n";
            of << "lces 3 0 " << g_ascii_dtostr(buffer.data(), 8, lces) << "mH\n";
            of << "cmes 3 0 " << g_ascii_dtostr(buffer.data(), 8, cmes) << "uF\n";
            of << "res 3 0 " << g_ascii_dtostr(buffer.data(), 8, res) << "\n";
            of << "cmef 3 0 " << g_ascii_dtostr(buffer.data(), 8, cmef) << "uF\n";

            if (g_settings.getValueBool("SPICEUseGNUCAP"))
            {
                of << ".print ac ir(vamp) ii(vamp)\n";
                of << ".ac DEC 50 20 20k\n";
            }
            else
            {
                of << ".ac DEC 50 20 20k\n";
                of << ".print ac 1/mag(i(vamp))\n";
            }

            of << ".end\n";

            of.close();

            std::string cmd;

            if (g_settings.getValueBool("SPICEUseNGSPICE")
                || g_settings.getValueBool("SPICEUseGNUCAP"))
            {
                cmd = g_settings.getValueString("SPICECmdLine") + " -b " + tmp_file + " > "
                      + tmp_file + ".out";
            }
            else
            {
                cmd = g_settings.getValueString("SPICECmdLine") + " -b -o " + tmp_file + ".out "
                      + tmp_file;
            }
            // g_settings.getValueString("SPICECmdLine") + " -b -o " + tmp_file + ".out " + tmp_file;
#ifndef NDEBUG
            std::cout << "driver_editor::draw_impedance_plot: running SPICE with \"" + cmd + "\"\n";
#endif
            system(cmd.c_str());
#ifndef NDEBUG
            std::puts("driver_editor::draw_impedance_plot: SPICE done");
#endif
            std::string spice_output_file = tmp_file + ".out";
            std::ifstream input_file(spice_output_file.c_str());

            if (input_file.good())
            {
                bool output = false;

                while (!input_file.eof())
                {
                    std::array<char, 100> buffer;
                    input_file.getline(buffer.data(), 100, '\n');
                    if (g_settings.getValueBool("SPICEUseGNUCAP"))
                    {
                        if (buffer[0] == ' ')
                        {
                            output = true;
                        }
                    }
                    else
                    {
                        if (buffer[0] == '0')
                        {
                            output = true;
                        }
                    }

                    if (output)
                    {
                        if (g_settings.getValueBool("SPICEUseGNUCAP"))
                        {
                            float const f_id = std::atof(buffer.data());

                            if (f_id != 0)
                            {
                                /* Check if we got a freq more than 10kHz */
                                char* substr_ptr = strstr(buffer.data(), "K");
                                float const f1 = substr_ptr != nullptr ? f_id * 1000 : f_id;

                                substr_ptr = strtok(buffer.data(), " ");
                                substr_ptr = strtok(nullptr, " ");
                                float f2 = g_ascii_strtod(substr_ptr, nullptr);
                                if (strstr(substr_ptr, "m") != nullptr)
                                {
                                    f2 /= 1000.0;
                                }
                                else if (strstr(substr_ptr, "u") != nullptr)
                                {
                                    f2 /= 1000000.0;
                                }
                                else if (strstr(substr_ptr, "n") != nullptr)
                                {
                                    f2 /= 1000000000.0;
                                }
                                else if (strstr(substr_ptr, "p") != nullptr)
                                {
                                    f2 /= 1000000000000.0;
                                }
                                substr_ptr = strtok(nullptr, " ");
                                float f3 = g_ascii_strtod(substr_ptr, nullptr);

                                if (strstr(substr_ptr, "m") != nullptr)
                                {
                                    f3 /= 1000.0;
                                }
                                else if (strstr(substr_ptr, "u") != nullptr)
                                {
                                    f3 /= 1000000.0;
                                }
                                else if (strstr(substr_ptr, "n") != nullptr)
                                {
                                    f3 /= 1000000000.0;
                                }
                                else if (strstr(substr_ptr, "p") != nullptr)
                                {
                                    f3 /= 1000000000000.0;
                                }
                                points.emplace_back(std::round(f1), 50 + (1 / std::hypot(f2, f3)));
                            }
                        }
                        else
                        {
                            if (buffer[0] >= '0' && buffer[0] <= '9')
                            {
                                strtok(buffer.data(), "\t");
                                char* substr_ptr = strtok(nullptr, "\t");

                                auto const f1 = g_ascii_strtod(substr_ptr, nullptr);
                                substr_ptr = strtok(nullptr, "\t");
                                auto const f2 = g_ascii_strtod(substr_ptr, nullptr);

                                points.emplace_back(std::round(f1), 50 + f2);
                            }
                        }
                    }
                }

                if (update)
                {
                    plot.replace_plot(static_cast<int>(!(s.get_freq_resp_filename().empty()
                                                         || !g_settings.getValueBool("DrawDriverFre"
                                                                                     "qRespPlot"))),
                                      points,
                                      Gdk::Color("red"));
                }
                else
                {
                    plot.add_plot(points, Gdk::Color("red"));
                }
            }
        }
    }
}

void driver_editor::on_clear()
{
    m_refListStore->clear();
    m_driver_list->data().clear();
    m_modified = true;
}

void driver_editor::on_entry_changed(int i)
{
    if (updating_entries)
    {
        return;
    }

    // This treeview stuff is kind of weird...
    if (Gtk::TreeIter iter = m_TreeView.get_selection()->get_selected())
    {
        Gtk::TreePath path = m_refListStore->get_path(iter);
        Gtk::TreeRow row = *(m_refListStore->get_iter(path));

        bool update_imp_plot = false;

        // Update the data container
        double d;
        switch (i)
        {
            case 0:
                // the treestore
                row[m_columns.id_string] = m_IdStringEntry.get_text();
                m_driver_list->data()[index].set_id_string(m_IdStringEntry.get_text());
                break;
            case 1:
                // the treestore
                row[m_columns.qts] = std::atof(m_QtsEntry.get_text().c_str());
                m_driver_list->data()[index].set_qts(std::atof(m_QtsEntry.get_text().c_str()));
                break;
            case 2:
                // the treestore
                row[m_columns.fs] = std::atof(m_FsEntry.get_text().c_str());
                m_driver_list->data()[index].set_fs(std::atof(m_FsEntry.get_text().c_str()));
                break;
            case 3:
                // the treestore
                row[m_columns.vas] = std::atof(m_VasEntry.get_text().c_str());
                m_driver_list->data()[index].set_vas(std::atof(m_VasEntry.get_text().c_str()));
                break;
            case 4:
                d = std::atof(m_RdcEntry.get_text().c_str());
                row[m_columns.rdc] = d; // the treestore
                if (d == 0.0) d = 1.0;
                m_driver_list->data()[index].set_rdc(d);
                update_imp_plot = true;
                break;
            case 5:
                d = std::atof(m_LvcEntry.get_text().c_str());
                row[m_columns.lvc] = d;
                if (d == 0.0) d = 1.0;
                m_driver_list->data()[index].set_lvc(d);
                break;
                update_imp_plot = true;
            case 6:
                // the treestore
                row[m_columns.qms] = std::atof(m_QmsEntry.get_text().c_str());
                m_driver_list->data()[index].set_qms(std::atof(m_QmsEntry.get_text().c_str()));
                break;
            case 7:
                // the treestore
                row[m_columns.qes] = std::atof(m_QesEntry.get_text().c_str());
                m_driver_list->data()[index].set_qes(std::atof(m_QesEntry.get_text().c_str()));
                break;
            case 8:
                // the treestore
                row[m_columns.imp] = std::atof(m_ImpEntry.get_text().c_str());
                m_driver_list->data()[index].set_imp(std::atof(m_ImpEntry.get_text().c_str()));
                break;
            case 9:
                // the treestore
                row[m_columns.sens] = std::atof(m_SensEntry.get_text().c_str());
                m_driver_list->data()[index].set_sens(std::atof(m_SensEntry.get_text().c_str()));
                break;
            case 10:
                if (m_BassCheckButton.get_active())
                {
                    m_driver_list->data()[index].set_type(m_driver_list->data()[index].get_type()
                                                          | SPEAKER_TYPE_BASS);
                }
                else
                {
                    m_driver_list->data()[index].set_type(m_driver_list->data()[index].get_type()
                                                          & ~SPEAKER_TYPE_BASS);
                }
                row[m_columns.type] = m_driver_list->data()[index].get_type();
                signal_speakerlist_loaded(m_driver_list.get());
                break;
            case 11:
                if (m_MidrangeCheckButton.get_active())
                {
                    m_driver_list->data()[index].set_type(m_driver_list->data()[index].get_type()
                                                          | SPEAKER_TYPE_MIDRANGE);
                }
                else
                {
                    m_driver_list->data()[index].set_type(m_driver_list->data()[index].get_type()
                                                          & ~SPEAKER_TYPE_MIDRANGE);
                }
                row[m_columns.type] = m_driver_list->data()[index].get_type();
                break;
            case 12:
                if (m_TweeterCheckButton.get_active())
                {
                    m_driver_list->data()[index].set_type(m_driver_list->data()[index].get_type()
                                                          | SPEAKER_TYPE_TWEETER);
                }
                else
                {
                    m_driver_list->data()[index].set_type(m_driver_list->data()[index].get_type()
                                                          & ~SPEAKER_TYPE_TWEETER);
                }
                row[m_columns.type] = m_driver_list->data()[index].get_type();
                break;
            case 13:
                d = std::atof(m_MmdEntry.get_text().c_str());
                row[m_columns.mmd] = d; // the treestore
                if (d == 0.0) d = 1.0;
                m_driver_list->data()[index].set_mmd(d);
                update_imp_plot = true;
                break;
            case 14:
                d = std::atof(m_AdEntry.get_text().c_str());
                row[m_columns.ad] = d; // the treestore
                if (d == 0.0) d = 1.0;
                m_driver_list->data()[index].set_ad(d);
                update_imp_plot = true;
                break;
            case 15:
                d = std::atof(m_BlEntry.get_text().c_str());
                row[m_columns.bl] = d; // the treestore
                if (d == 0.0) d = 1.0;
                m_driver_list->data()[index].set_bl(d);
                update_imp_plot = true;
                break;
            case 16:
                d = std::atof(m_RmsEntry.get_text().c_str());
                row[m_columns.rms] = d; // the treestore
                if (d == 0.0) d = 1.0;
                m_driver_list->data()[index].set_rms(d);
                update_imp_plot = true;
                break;
            case 17:
                d = std::atof(m_CmsEntry.get_text().c_str());
                row[m_columns.cms] = d; // the treestore
                if (d == 0.0) d = 1.0;
                m_driver_list->data()[index].set_cms(d);
                update_imp_plot = true;
                break;
        }
        if (update_imp_plot)
        {
            draw_impedance_plot(m_driver_list->data()[index], true);
        }
    }
    // FIXME gtk3 port
    // m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
    m_toolbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);
    GSpeakers::driverlist_modified() = true;
    m_modified = true;
}

void driver_editor::on_append_xml()
{
    file_chooser_dialog fc(_("Append speaker xml"));
    std::string const& filename = fc.get_filename();
    if (!filename.empty())
    {
        append_xml(filename);
    }
}

void driver_editor::on_open_xml()
{
    file_chooser_dialog fc(_("Open speaker xml"));
    std::string const& filename = fc.get_filename();
    if (!filename.empty())
    {
        open_xml(filename);
    }
}

void driver_editor::append_xml(const std::string& filename)
{
    try
    {
        auto const& temp_diver_list = driver_list(filename);

        std::for_each(begin(temp_diver_list.data()),
                      end(temp_diver_list.data()),
                      sigc::mem_fun(*this, &driver_editor::add_item));

        for (auto& from : temp_diver_list.data())
        {
            m_driver_list->data().push_back(from);
        }

        set_entries_sensitive(true);
        m_modified = true;

        // m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);

        m_toolbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);
        GSpeakers::driverlist_modified() = true;
    }
    catch (std::runtime_error const& e)
    {
        Gtk::MessageDialog(e.what(), false, Gtk::MESSAGE_ERROR).run();
    }
}

auto driver_editor::open_xml(const std::string& filename) -> bool
{
    if (filename.empty())
    {
        return false;
    }

    try
    {
        driver_list const& temp_diver_list = driver_list(filename);

        m_refListStore->clear();

        m_filename = filename;

        g_settings.setValue("SpeakerListXml", m_filename);
        m_frame_label->set_markup("<b>" + Glib::ustring(_("Drivers ["))
                                  + GSpeakers::short_filename(m_filename, 40) + "]</b>");
        m_evbox->set_tooltip_text(m_filename);

        std::for_each(temp_diver_list.data().begin(),
                      temp_diver_list.data().end(),
                      sigc::mem_fun(*this, &driver_editor::add_item));

        m_driver_list->data().clear();
        m_driver_list->data().insert(begin(m_driver_list->data()),
                                     begin(temp_diver_list.data()),
                                     end(temp_diver_list.data()));

        // Select the first item in the list
        if (!m_driver_list->data().empty())
        {
            Gtk::TreeRow row = *(m_refListStore->get_iter(Gtk::TreePath(std::to_string(0))));
            m_TreeView.get_selection()->select(row);
        }

        m_IdStringEntry.grab_focus();
        set_entries_sensitive(true);

        // FIXME gtk3 port
        // m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
        // m_menu.items()[MENU_INDEX_DELETE].set_sensitive(true);

        GSpeakers::driverlist_modified() = true;
        m_toolbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);
        m_toolbar->get_nth_item(TOOLBAR_INDEX_DELETE)->set_sensitive(true);

        signal_speakerlist_loaded(m_driver_list.get());

        m_modified = true;
    }
    catch (std::runtime_error const& error)
    {
        Gtk::MessageDialog(error.what(), false, Gtk::MESSAGE_ERROR).run();
        return false;
    }
    return true;
}

void driver_editor::on_edit_freq_resp()
{
#ifndef NDEBUG
    std::cout << "SpeakerEditor::on_edit_freq_resp" << std::endl;
    std::cout << "SpeakerEditor::on_edit_freq_resp: index = " << index << std::endl;
#endif

    frequency_response_editor frequency_editor(m_FreqRespFileEntry.get_text());
    frequency_editor.run();

    m_FreqRespFileEntry.set_text(frequency_editor.get_filename());
    m_FreqRespFileEntry.set_tooltip_text(m_FreqRespFileEntry.get_text());

    m_driver_list->data()[index].set_freq_resp_filename(frequency_editor.get_filename());

    on_selection_changed();

    // gtk3 port
    // m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);

    m_toolbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);
    GSpeakers::driverlist_modified() = true;
    m_modified = true;
}

void driver_editor::on_browse_freq_resp()
{
#ifndef NDEBUG
    std::puts("SpeakerEditor::on_browse_freq_resp");
#endif
    file_chooser_dialog fc(_("Open frequency response file"));
    std::string const& filename = fc.get_filename();

    if (filename.empty())
    {
        return;
    }

    // TODO: Check that selected file exists
    m_FreqRespFileEntry.set_text(m_filename);
    m_FreqRespFileEntry.set_tooltip_text(m_FreqRespFileEntry.get_text());

    m_driver_list->data()[index].set_freq_resp_filename(filename);

    on_selection_changed();
    // FIXME gtk3 port
    // m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
    m_toolbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);

    m_modified = true;

    GSpeakers::driverlist_modified() = true;
}

void driver_editor::create_model()
{
    m_refListStore = Gtk::ListStore::create(m_columns);

    std::for_each(begin(m_driver_list->data()),
                  end(m_driver_list->data()),
                  sigc::mem_fun(*this, &driver_editor::add_item));
}

void driver_editor::add_item(driver const& spk)
{
    Gtk::TreeRow row = *(m_refListStore->append());

    row[m_columns.id] = spk.get_id();
    row[m_columns.id_string] = spk.get_id_string();
    row[m_columns.type] = spk.get_type();
    row[m_columns.qts] = spk.get_qts();
    row[m_columns.fs] = spk.get_fs();
    row[m_columns.vas] = spk.get_vas();
    row[m_columns.rdc] = spk.get_rdc();
    row[m_columns.lvc] = spk.get_lvc();
    row[m_columns.qms] = spk.get_qms();
    row[m_columns.qes] = spk.get_qes();
    row[m_columns.imp] = spk.get_imp();
    row[m_columns.sens] = spk.get_sens();
    row[m_columns.mmd] = spk.get_mmd();
    row[m_columns.ad] = spk.get_ad();
    row[m_columns.bl] = spk.get_bl();
    row[m_columns.rms] = spk.get_rms();
    row[m_columns.cms] = spk.get_cms();
    row[m_columns.freq_resp_file] = spk.get_freq_resp_filename();
}

void driver_editor::add_columns()
{
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        auto const column_count = m_TreeView.append_column(_("Identifier"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(column_count - 1);

        pColumn->add_attribute(pRenderer->property_text(), m_columns.id_string);
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Type"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this, &driver_editor::type_cell_data_func));
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Qts"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this, &driver_editor::qts_cell_data_func));
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Fs"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this, &driver_editor::fs_cell_data_func));
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Vas"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this, &driver_editor::vas_cell_data_func));
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Impedance"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this, &driver_editor::imp_cell_data_func));
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Sensitivity"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this, &driver_editor::sens_cell_data_func));
    }
}

void driver_editor::type_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    std::string s;
    if (is_bass_driver((*iter)[m_columns.type]))
    {
        s = _("Woofer");
    }
    if (is_midrange_driver((*iter)[m_columns.type]))
    {
        if (!s.empty())
        {
            s += _(", Midrange");
        }
        else
        {
            s = _("Midrange");
        }
    }
    if (is_tweeter_driver((*iter)[m_columns.type]))
    {
        if (!s.empty())
        {
            s = s + _(", Tweeter");
        }
        else
        {
            s = _("Tweeter");
        }
    }
    if (s.empty())
    {
        s = _("Unknown");
    }
    renderer.property_text() = s;
}

void driver_editor::qts_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = gpsk::to_ustring((*iter)[m_columns.qts], 3, 3);
    renderer.property_xalign() = 1.0;
}

void driver_editor::fs_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = gpsk::to_ustring((*iter)[m_columns.fs], 3, 0) + " Hz";
    renderer.property_xalign() = 1.0;
}

void driver_editor::vas_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = gpsk::to_ustring((*iter)[m_columns.vas], 3, 3) + " l";
    renderer.property_xalign() = 1.0;
}

void driver_editor::rdc_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = gpsk::to_ustring((*iter)[m_columns.rdc], 3, 1) + " Ohm";
    renderer.property_xalign() = 1.0;
}

void driver_editor::lvc_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = gpsk::to_ustring((*iter)[m_columns.lvc], 3, 2) + " mH";
    renderer.property_xalign() = 1.0;
}

void driver_editor::qms_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = gpsk::to_ustring((*iter)[m_columns.qms], 3, 3);
    renderer.property_xalign() = 1.0;
}

void driver_editor::qes_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = gpsk::to_ustring((*iter)[m_columns.qes], 3, 2);
    renderer.property_xalign() = 1.0;
}

void driver_editor::imp_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = gpsk::to_ustring((*iter)[m_columns.imp], 3, 1) + " Ohm";
    renderer.property_xalign() = 1.0;
}

void driver_editor::sens_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = gpsk::to_ustring((*iter)[m_columns.sens], 3, 1) + " dB";
    renderer.property_xalign() = 1.0;
}

void driver_editor::mmd_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = gpsk::to_ustring((*iter)[m_columns.mmd] * 1000, 3, 2)
                               + " g";
    renderer.property_xalign() = 1.0;
}

void driver_editor::ad_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = gpsk::to_ustring((*iter)[m_columns.ad], 3, 3) + " m";
    renderer.property_xalign() = 1.0;
}

void driver_editor::bl_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = gpsk::to_ustring((*iter)[m_columns.bl], 3, 1) + " N/A";
    renderer.property_xalign() = 1.0;
}

void driver_editor::rms_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = gpsk::to_ustring((*iter)[m_columns.rms], 3, 2) + " Ns/m";
    renderer.property_xalign() = 1.0;
}

void driver_editor::cms_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = gpsk::to_ustring((*iter)[m_columns.cms], 3, 4) + " m/N";
    renderer.property_xalign() = 1.0;
}

void driver_editor::on_cell_fixed_toggled(const Glib::ustring& path_string)
{
    Gtk::TreePath path(path_string);

    // get toggled iter
    Gtk::TreeRow row = *(m_refListStore->get_iter(path));

    bool fixed = row[m_columns.id] != 0;

    // do something with the value
    fixed = !fixed;

    // set new value
    row[m_columns.id] = static_cast<const int>(fixed);
}
