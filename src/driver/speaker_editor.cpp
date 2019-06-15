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

#include "speaker_editor.hpp"

#include "common.h"
#include "freqrespeditor.h"
#include "gspeakersfilechooser.h"

#include <glibmm.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/separatortoolitem.h>
#include <gtkmm/imagemenuitem.h>

#include <cmath>
#include <fstream>

constexpr auto MENU_INDEX_SAVE = 5;
constexpr auto MENU_INDEX_DELETE = 8;
constexpr auto TOOLBAR_INDEX_SAVE = 3;
constexpr auto TOOLBAR_INDEX_DELETE = 5;

constexpr auto FILE_CHOOSER_CANCEL = 10001;
constexpr auto FILE_CHOOSER_OPEN = 10002;
constexpr auto FILE_CHOOSER_SAVE = 10003;

speaker_editor::speaker_editor()
    : m_TreeViewTable(10, 4, true),
      m_Table(19, 2, true),
      m_EditFreqRespButton(_("Edit...")),
      m_BrowseFreqRespButton(_("...")),
      m_BassCheckButton(_("Bass")),
      m_MidrangeCheckButton(_("Midrange")),
      m_TweeterCheckButton(_("Tweeter")),
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
        m_speaker_list = new speaker_list(m_filename);
        signal_speakerlist_loaded(m_speaker_list);
    }
    catch (std::runtime_error const& e)
    {
        m_speaker_list = new speaker_list();
        std::cout << "speaker_editor::speaker_editor: " << e.what() << std::endl;
    }
    m_treeview_vbox.set_border_width(5);
    m_treeview_vbox.pack_start(m_treeview_frame);
    m_treeview_frame.add(m_inner_treeview_vbox);
    m_treeview_frame.set_shadow_type(Gtk::SHADOW_NONE);
    m_evbox = Gtk::manage(new Gtk::EventBox());
    m_frame_label = Gtk::manage(new Gtk::Label());
    m_frame_label->set_markup("<b>" + Glib::ustring(_("Drivers ["))
                              + GSpeakers::short_filename(m_filename, 50) + "]</b>");
    static_cast<Gtk::Container*>(m_evbox)->add(*m_frame_label);

    m_treeview_frame.set_label_widget(*m_evbox);

    // GSpeakers::tooltips().set_tip(*m_evbox, m_filename);

    m_inner_treeview_vbox.set_border_width(12);
    m_inner_treeview_vbox.pack_start(m_TreeViewTable);

    m_vbox.set_border_width(2);
    m_Table.set_spacings(2);

    m_vbox.pack_start(m_Table);
    m_vbox.set_border_width(12);

    /* Setup the table */
    m_TreeViewTable.set_spacings(2);
    m_TreeViewTable.attach(m_ScrolledWindow, 0, 4, 0, 10);

    // All the entries
    m_Table.attach(*Gtk::manage(new Gtk::Label(_("Name:"), Gtk::ALIGN_START)), 0, 1, 0, 1);
    m_Table.attach(m_IdStringEntry, 1, 3, 0, 1);

    // GSpeakers::tooltips().set_tip(m_IdStringEntry,
    //                               _("The name or identificationstd::string for the driver"));

    m_Table.attach(*Gtk::manage(new Gtk::Label(_("Qts:"), Gtk::ALIGN_START)),
                   0,
                   2,
                   1,
                   2,
                   Gtk::FILL | Gtk::EXPAND | Gtk::SHRINK);

    m_Table.attach(m_QtsEntry, 2, 3, 1, 2);
    m_Table.attach(*Gtk::manage(new Gtk::Label(_("Fs: (Hz)"), Gtk::ALIGN_START)), 0, 2, 2, 3);
    m_Table.attach(m_FsEntry, 2, 3, 2, 3);
    m_Table.attach(*Gtk::manage(new Gtk::Label(_("Vas: (l)"), Gtk::ALIGN_START)), 0, 2, 3, 4);
    m_Table.attach(m_VasEntry, 2, 3, 3, 4);
    m_Table.attach(*Gtk::manage(new Gtk::Label(_("Rdc: (Ohm)"), Gtk::ALIGN_START)), 0, 2, 4, 5);
    m_Table.attach(m_RdcEntry, 2, 3, 4, 5);
    m_Table.attach(*Gtk::manage(new Gtk::Label(_("Lvc: (mH)"), Gtk::ALIGN_START)), 0, 2, 5, 6);
    m_Table.attach(m_LvcEntry, 2, 3, 5, 6);
    m_Table.attach(*Gtk::manage(new Gtk::Label(_("Qms:"), Gtk::ALIGN_START)), 0, 2, 6, 7);
    m_Table.attach(m_QmsEntry, 2, 3, 6, 7);
    m_Table.attach(*Gtk::manage(new Gtk::Label(_("Qes:"), Gtk::ALIGN_START)), 0, 2, 7, 8);
    m_Table.attach(m_QesEntry, 2, 3, 7, 8);
    m_Table.attach(*Gtk::manage(new Gtk::Label(_("Impedance: (Ohm)"), Gtk::ALIGN_START)), 0, 2, 8, 9);
    m_Table.attach(m_ImpEntry, 2, 3, 8, 9);
    m_Table.attach(*Gtk::manage(new Gtk::Label(_("Sensitivity: (dB/W/m)"), Gtk::ALIGN_START)),
                   0,
                   2,
                   9,
                   10);
    m_Table.attach(m_SensEntry, 2, 3, 9, 10);

    m_Table.attach(*Gtk::manage(new Gtk::Label(_("Cone mass: (kg)"), Gtk::ALIGN_START)), 0, 2, 10, 11);
    m_Table.attach(m_MmdEntry, 2, 3, 10, 11);
    m_Table.attach(*Gtk::manage(new Gtk::Label(_("Effective radius: (m)"), Gtk::ALIGN_START)),
                   0,
                   2,
                   11,
                   12);
    m_Table.attach(m_AdEntry, 2, 3, 11, 12);
    m_Table.attach(*Gtk::manage(new Gtk::Label(_("Force factor: (N/A)"), Gtk::ALIGN_START)),
                   0,
                   2,
                   12,
                   13);
    m_Table.attach(m_BlEntry, 2, 3, 12, 13);
    m_Table.attach(*Gtk::manage(new Gtk::Label(_("Susp. resistance: (Ns/m)"), Gtk::ALIGN_START)),
                   0,
                   2,
                   13,
                   14);
    m_Table.attach(m_RmsEntry, 2, 3, 13, 14);
    m_Table.attach(*Gtk::manage(new Gtk::Label(_("Susp. compleance: (m/N)"), Gtk::ALIGN_START)),
                   0,
                   2,
                   14,
                   15);
    m_Table.attach(m_CmsEntry, 2, 3, 14, 15);

    // GSpeakers::tooltips().set_tip(
    //     m_BassCheckButton, _("Check this box if the driver will work as a woofer (bass speaker)"));
    m_Table.attach(m_BassCheckButton, 0, 3, 15, 16);
    // GSpeakers::tooltips().set_tip(m_MidrangeCheckButton,
    //                               _("Check this box if the driver will work as a midrange
    //                               speaker"));
    m_Table.attach(m_MidrangeCheckButton, 0, 3, 16, 17);
    // GSpeakers::tooltips().set_tip(m_TweeterCheckButton,
    //                               _("Check this box if the driver will work as a tweeter"));
    m_Table.attach(m_TweeterCheckButton, 0, 3, 17, 18);

    // m_Table.attach(*Gtk::manage(new Gtk::Label(_("Freq resp file:"), Gtk::ALIGN_START)), 0, 1,
    // 18, 19); m_Table.attach(m_FreqRespFileEntry, 1, 2, 18, 19);
    Gtk::HBox* vbox = Gtk::manage(new Gtk::HBox());
    vbox->pack_start(*Gtk::manage(new Gtk::Label(_("Freq resp file:"), Gtk::ALIGN_START)));
    vbox->pack_start(m_FreqRespFileEntry);
    m_FreqRespFileEntry.set_width_chars(15);
    vbox->pack_start(m_BrowseFreqRespButton);
    vbox->pack_start(m_EditFreqRespButton);
    vbox->set_spacing(12);
    m_Table.attach(*vbox, 0, 3, 18, 19);

    m_ScrolledWindow.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    m_IdStringEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 0));
    m_QtsEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 1));
    m_FsEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 2));
    m_VasEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 3));
    m_RdcEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 4));
    m_LvcEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 5));
    m_QmsEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 6));
    m_QesEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 7));
    m_ImpEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 8));
    m_SensEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 9));
    m_MmdEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 13));
    m_AdEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 14));
    m_BlEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 15));
    m_RmsEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 16));
    m_CmsEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 17));
    m_FreqRespFileEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 18));
    m_FreqRespFileEntry.set_editable(false);

    m_BassCheckButton.signal_toggled().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 10));
    m_MidrangeCheckButton.signal_toggled().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 11));
    m_TweeterCheckButton.signal_toggled().connect(
        sigc::bind(sigc::mem_fun(*this, &speaker_editor::on_entry_changed), 12));

    m_EditFreqRespButton.signal_clicked().connect(
        sigc::mem_fun(*this, &speaker_editor::on_edit_freq_resp));
    m_BrowseFreqRespButton.signal_clicked().connect(
        sigc::mem_fun(*this, &speaker_editor::on_browse_freq_resp));

    // GSpeakers::tooltips().set_tip(m_EditFreqRespButton, _("Edit frequency response for this
    // driver"));
    // GSpeakers::tooltips().set_tip(m_BrowseFreqRespButton, _("Browse for frequency
    // response file"));
    set_entries_sensitive(false);

    create_model();

    /* create tree view */
    m_TreeView.set_model(m_refListStore);
    m_TreeView.set_rules_hint();
    Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();

    selection->signal_changed().connect(sigc::mem_fun(*this, &speaker_editor::on_selection_changed));

    g_settings.settings_changed.connect(sigc::mem_fun(*this, &speaker_editor::on_settings_changed));

    add_columns();
    m_ScrolledWindow.add(m_TreeView);

    signal_save_open_files.connect(sigc::mem_fun(*this, &speaker_editor::on_save_open_files));
}

speaker_editor::~speaker_editor()
{
    if (m_speaker_list)
    {
        delete m_speaker_list;
        m_speaker_list = nullptr;
    }
}

Gtk::MenuItem& speaker_editor::get_menu()
{
    // FIXME gtk3 port

    m_menu_item.set_label("Driver");

    // Create the drop down menu options
    auto driver_submenu = Gtk::manage(new Gtk::Menu());

    {
        auto new_driver = Gtk::manage(new Gtk::MenuItem("New Driver"));
        new_driver->signal_activate().connect(sigc::mem_fun(*this, &speaker_editor::on_new));
        driver_submenu->append(*new_driver);
    }
    driver_submenu->append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
    {
        auto new_driver = Gtk::manage(new Gtk::MenuItem("New"));
        new_driver->signal_activate().connect(sigc::mem_fun(*this, &speaker_editor::on_new_xml));
        driver_submenu->append(*new_driver);
    }
    {
        auto append_xml = Gtk::manage(new Gtk::MenuItem("Append XML"));
        append_xml->signal_activate().connect(sigc::mem_fun(*this, &speaker_editor::on_append_xml));
        driver_submenu->append(*append_xml);
    }
    {
        auto open_xml = Gtk::manage(new Gtk::MenuItem("Open XML"));
        open_xml->signal_activate().connect(sigc::mem_fun(*this, &speaker_editor::on_open_xml));
        driver_submenu->append(*open_xml);
    }
    driver_submenu->append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
    {
        auto save = Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::SAVE));
        save->signal_activate().connect(sigc::mem_fun(*this, &speaker_editor::on_save));
        driver_submenu->append(*save);
    }
    {
        auto save_as = Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::SAVE_AS));
        save_as->signal_activate().connect(sigc::mem_fun(*this, &speaker_editor::on_save_as));
        driver_submenu->append(*save_as);
    }
    driver_submenu->append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
    {
        auto delete_item = Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::DELETE));
        delete_item->signal_activate().connect(sigc::mem_fun(*this, &speaker_editor::on_remove));
        driver_submenu->append(*delete_item);
    }

    m_menu_item.set_submenu(*driver_submenu);

    return m_menu_item;

    // menulist[MENU_INDEX_SAVE].set_sensitive(false);
    // menulist[MENU_INDEX_DELETE].set_sensitive(false);
}

Gtk::Widget& speaker_editor::get_toolbar()
{
    if (tbar == nullptr)
    {
        tbar = Gtk::manage(new Gtk::Toolbar());

        // TODO: tooltips
        Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_LARGE_TOOLBAR));
        Gtk::ToolButton* t = Gtk::manage(new Gtk::ToolButton(*im, _("New Driver")));
        t->signal_clicked().connect(sigc::mem_fun(*this, &speaker_editor::on_new));
        tbar->append(*t);

        Gtk::SeparatorToolItem* s = Gtk::manage(new Gtk::SeparatorToolItem());
        tbar->append(*s);

        t = Gtk::manage(new Gtk::ToolButton(Gtk::Stock::OPEN));
        t->signal_clicked().connect(sigc::mem_fun(*this, &speaker_editor::on_open_xml));
        tbar->append(*t);
        t = Gtk::manage(new Gtk::ToolButton(Gtk::Stock::SAVE));
        t->signal_clicked().connect(sigc::mem_fun(*this, &speaker_editor::on_save));
        tbar->append(*t);

        s = Gtk::manage(new Gtk::SeparatorToolItem());
        tbar->append(*s);

        t = Gtk::manage(new Gtk::ToolButton(Gtk::Stock::DELETE));
        t->signal_clicked().connect(sigc::mem_fun(*this, &speaker_editor::on_remove));
        tbar->append(*t);

        toolbar.add(*tbar);
        tbar->set_toolbar_style((Gtk::ToolbarStyle)g_settings.getValueUnsignedInt("ToolbarStyle"));
        tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(false);
        tbar->get_nth_item(TOOLBAR_INDEX_DELETE)->set_sensitive(false);
    }
    return toolbar;
}

void speaker_editor::on_save_open_files()
{
    if (GSpeakers::driverlist_modified())
    {
        on_save();
    }
}

void speaker_editor::on_settings_changed(const std::string& setting)
{
    if (setting == "ToolbarStyle")
    {
        tbar->set_toolbar_style((Gtk::ToolbarStyle)g_settings.getValueUnsignedInt("ToolbarStyle"));
    }
    if (setting == "DrawDriverImpPlot" || setting == "DrawDriverFreqRespPlot")
    {
        if (index != -1)
        {
            on_selection_changed();
        }
    }
}

void speaker_editor::on_close() { signal_speakerlist_loaded(m_speaker_list); }

void speaker_editor::set_entries_sensitive(bool const is_sensitive)
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

void speaker_editor::on_new()
{
    Speaker s(_("New Speaker"));

    s.set_id_string(s.get_id_string() + " " + std::to_string(s.get_id()));

    add_item(s);
    m_speaker_list->data().push_back(s);

    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

    Gtk::TreePath path(std::to_string(m_speaker_list->data().size() - 1));
    Gtk::TreeRow row = *(m_refListStore->get_iter(path));
    refSelection->select(row);

    m_IdStringEntry.grab_focus();

    set_entries_sensitive(true);

    // FIXME gtk3 port
    // m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
    // m_menu.items()[MENU_INDEX_DELETE].set_sensitive(true);
    tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);
    tbar->get_nth_item(TOOLBAR_INDEX_DELETE)->set_sensitive(true);
    GSpeakers::driverlist_modified() = true;
    signal_speakerlist_loaded(m_speaker_list);
    m_modified = true;
}

void speaker_editor::on_new_xml()
{
    m_refListStore->clear();
    m_speaker_list->clear();

    set_entries_sensitive(false);
    new_xml_pressed = true;
    // Add one new speaker to the new xml file, we usually want to do this
    on_new();
}

void speaker_editor::on_remove()
{
    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

    if (const Gtk::TreeIter iter = refSelection->get_selected())
    {
        Gtk::TreePath path = m_refListStore->get_path(iter);

        if (!path.empty())
        {
            // Remove item from ListStore:
            m_refListStore->erase(iter);

            if (index < (int)m_speaker_list->data().size())
                m_speaker_list->data().erase(m_speaker_list->data().begin() + index);
        }
    }
    // m_menu.items()[MENU_INDEX_DELETE].set_sensitive(false);
}

void speaker_editor::on_save()
{
#ifdef OUTPUT_DEBUG
    std::puts("SpeakerEditor: save");
#endif
    if (new_xml_pressed)
    {
        on_save_as();
        new_xml_pressed = false;
    }
    else
    {
#ifdef OUTPUT_DEBUG
        std::cout << "SpeakerEditor: Filename = " << m_filename << "\n";
#endif
        try
        {
            m_speaker_list->to_xml(m_filename);
            // FIXME gtk3 port
            // m_menu.items()[MENU_INDEX_SAVE].set_sensitive(false);
            tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(false);
            GSpeakers::driverlist_modified() = false;
            m_modified = false;
        }
        catch (std::runtime_error const& e)
        {
            Gtk::MessageDialog m(e.what(), false, Gtk::MESSAGE_ERROR);
            m.run();
        }
    }
}

void speaker_editor::on_save_as()
{
    std::string const& filename = GSpeakersFileChooserDialog(_("Save speaker xml as"),
                                                             Gtk::FILE_CHOOSER_ACTION_SAVE,
                                                             m_filename)
                                      .get_filename();

    if (!filename.empty())
    {
        save_as(filename);
    }
}

void speaker_editor::save_as(const std::string& filename)
{
    try
    {
        m_speaker_list->to_xml(filename);
        m_filename = filename;
    }
    catch (std::runtime_error const& error)
    {
        Gtk::MessageDialog message(error.what(), false, Gtk::MESSAGE_ERROR);
        message.run();
    }
}

void speaker_editor::on_selection_changed()
{
    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();
    updating_entries = true;

    if (const Gtk::TreeIter iter = refSelection->get_selected())
    {
        Gtk::TreePath path = m_refListStore->get_path(iter);

        if (!path.empty())
        {
            index = path[0];
            Speaker s = m_speaker_list->data()[index];

            m_IdStringEntry.set_text(Glib::ustring(s.get_id_string()));
            char* str = nullptr;
            GString* buffer = g_string_new(str);
            g_string_printf(buffer, "%3.3f", s.get_qts());
            m_QtsEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%1.0f", s.get_fs());
            m_FsEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%1.3f", s.get_vas());
            m_VasEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%1.1f", s.get_rdc());
            m_RdcEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.2f", s.get_lvc());
            m_LvcEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.3f", s.get_qms());
            m_QmsEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.3f", s.get_qes());
            m_QesEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.2f", s.get_imp());
            m_ImpEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.2f", s.get_sens());
            m_SensEntry.set_text(Glib::ustring(buffer->str));

            g_string_printf(buffer, "%0.2f", s.get_mmd());
            m_MmdEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.4f", s.get_ad());
            m_AdEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.2f", s.get_bl());
            m_BlEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.2f", s.get_rms());
            m_RmsEntry.set_text(Glib::ustring(buffer->str));
            g_string_printf(buffer, "%0.4f", s.get_cms());
            m_CmsEntry.set_text(Glib::ustring(buffer->str));

            /* Check buttons */
            if (s.get_type() == SPEAKER_TYPE_BASS)
            {
                m_BassCheckButton.set_active(true);
            }
            else
            {
                m_BassCheckButton.set_active(false);
            }

            if (s.get_type() == SPEAKER_TYPE_MIDRANGE)
            {
                m_MidrangeCheckButton.set_active(true);
            }
            else
            {
                m_MidrangeCheckButton.set_active(false);
            }

            if (s.get_type() == SPEAKER_TYPE_TWEETER)
            {
                m_TweeterCheckButton.set_active(true);
            }
            else
            {
                m_TweeterCheckButton.set_active(false);
            }

            m_FreqRespFileEntry.set_text(s.get_freq_resp_filename());

            plot.clear();
            if (g_settings.getValueBool("DrawDriverFreqRespPlot"))
            {
                /* Plot freq resp if it exists */
                if (!s.get_freq_resp_filename().empty())
                {
                    std::ifstream fin(s.get_freq_resp_filename().c_str());
                    if (fin.good())
                    {
                        std::vector<GSpeakers::Point> points;
                        while (!fin.eof())
                        {
                            std::array<char, 100> buffer;
                            fin.getline(buffer.data(), 100, '\n');

                            char* substr_ptr = strtok(buffer.data(), ",");
                            auto const f1 = g_ascii_strtod(substr_ptr, nullptr);
                            substr_ptr = strtok(nullptr, ",");
                            auto const f2 = g_ascii_strtod(substr_ptr, nullptr);

                            points.emplace_back(std::round(f1), f2);
                        }
                        Gdk::Color c("blue");
                        plot.add_plot(points, c);
                    }
                    else
                    {
                        std::cout << "Could not open " << s.get_freq_resp_filename() << "\n";
                    }
                }
            }
            // Plot impedance response increase impedance y coordinate 50 to align to imp scale
            draw_imp_plot(s);
        }
    }
    updating_entries = false;
    m_IdStringEntry.grab_focus();
    set_entries_sensitive(true);

    // FIXME gtk3 port
    // m_menu.items()[MENU_INDEX_DELETE].set_sensitive(true);

    tbar->get_nth_item(TOOLBAR_INDEX_DELETE)->set_sensitive(true);
}

void speaker_editor::draw_imp_plot(Speaker& s, bool update)
{
    std::array<gchar, 8> buffer;

    if (g_settings.getValueBool("DrawDriverImpPlot"))
    {
        std::vector<GSpeakers::Point> points;
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
            of << "SPICE code generated by GSpeakers Redux-" << VERSION << std::endl;
            of << "vamp 1 0 dc 0 ac 1\n";

            double const cmes = s.get_mmd() / (s.get_bl() * s.get_bl()) * 1000000;
            double const lces = s.get_bl() * s.get_bl() * s.get_cms() * 1000;
            double const res = (s.get_bl() * s.get_bl()) / s.get_rms();
            double const po = 1.18; // air density kg/m^3
            double const cmef = 8 * po * s.get_ad() * s.get_ad() * s.get_ad()
                                / (3 * s.get_bl() * s.get_bl()) * 1000000;

            of << "R" << s.get_id() << " 1 2 " << g_ascii_dtostr(buffer.data(), 8, s.get_rdc())
               << std::endl;
            of << "L" << s.get_id() << " 2 3 " << g_ascii_dtostr(buffer.data(), 8, s.get_lvc())
               << "mH" << std::endl;
            of << "lces 3 0 " << g_ascii_dtostr(buffer.data(), 8, lces) << "mH\n";
            of << "cmes 3 0 " << g_ascii_dtostr(buffer.data(), 8, cmes) << "uF\n";
            of << "res 3 0 " << g_ascii_dtostr(buffer.data(), 8, res) << "\n";
            of << "cmef 3 0 " << g_ascii_dtostr(buffer.data(), 8, cmef) << "uF\n";

            if (g_settings.getValueBool("SPICEUseGNUCAP"))
            {
                of << ".print ac ir(vamp) ii(vamp)" << std::endl;
                of << ".ac DEC 50 20 20k" << std::endl;
            }
            else
            {
                of << ".ac DEC 50 20 20k" << std::endl;
                of << ".print ac 1/mag(i(vamp))" << std::endl;
            }

            of << ".end" << std::endl;
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
#ifdef OUTPUT_DEBUG
            std::cout << "speaker_editor::draw_imp_plot: running SPICE with \"" + cmd + "\"\n";
#endif
            system(cmd.c_str());
#ifdef OUTPUT_DEBUG
            std::puts("speaker_editor::draw_imp_plot: SPICE done");
#endif
            std::string spice_output_file = tmp_file + ".out";
            std::ifstream fin(spice_output_file.c_str());

            if (fin.good())
            {
                bool output = false;

                while (!fin.eof())
                {
                    std::array<char, 100> buffer;
                    fin.getline(buffer.data(), 100, '\n');
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
                            if ((buffer[0] >= '0') && (buffer[0] <= '9'))
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
                Gdk::Color c2("red");
                if (update)
                {
                    plot.replace_plot(!(s.get_freq_resp_filename().empty()
                                        || !g_settings.getValueBool("DrawDriverFreqRespPlot")),
                                      points,
                                      c2);
                }
                else
                {
                    plot.add_plot(points, c2);
                }
            }
        }
    }
}

void speaker_editor::on_clear()
{
    m_refListStore->clear();
    m_speaker_list->data().clear();
    m_modified = true;
}

void speaker_editor::on_entry_changed(int i)
{
    if (updating_entries)
    {
        return;
    }

    // This treeview stuff is kind of weird...
    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

    if (Gtk::TreeIter iter = refSelection->get_selected())
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
                m_speaker_list->data()[index].set_id_string(m_IdStringEntry.get_text());
                break;
            case 1:
                // the treestore
                row[m_columns.qts] = std::atof(m_QtsEntry.get_text().c_str());
                m_speaker_list->data()[index].set_qts(std::atof(m_QtsEntry.get_text().c_str()));
                break;
            case 2:
                // the treestore
                row[m_columns.fs] = std::atof(m_FsEntry.get_text().c_str());
                m_speaker_list->data()[index].set_fs(std::atof(m_FsEntry.get_text().c_str()));
                break;
            case 3:
                // the treestore
                row[m_columns.vas] = std::atof(m_VasEntry.get_text().c_str());
                m_speaker_list->data()[index].set_vas(std::atof(m_VasEntry.get_text().c_str()));
                break;
            case 4:
                d = std::atof(m_RdcEntry.get_text().c_str());
                row[m_columns.rdc] = d; // the treestore
                if (d == 0.0) d = 1.0;
                m_speaker_list->data()[index].set_rdc(d);
                update_imp_plot = true;
                break;
            case 5:
                d = std::atof(m_LvcEntry.get_text().c_str());
                row[m_columns.lvc] = d;
                if (d == 0.0) d = 1.0;
                m_speaker_list->data()[index].set_lvc(d);
                break;
                update_imp_plot = true;
            case 6:
                // the treestore
                row[m_columns.qms] = std::atof(m_QmsEntry.get_text().c_str());
                m_speaker_list->data()[index].set_qms(std::atof(m_QmsEntry.get_text().c_str()));
                break;
            case 7:
                // the treestore
                row[m_columns.qes] = std::atof(m_QesEntry.get_text().c_str());
                m_speaker_list->data()[index].set_qes(std::atof(m_QesEntry.get_text().c_str()));
                break;
            case 8:
                // the treestore
                row[m_columns.imp] = std::atof(m_ImpEntry.get_text().c_str());
                m_speaker_list->data()[index].set_imp(std::atof(m_ImpEntry.get_text().c_str()));
                break;
            case 9:
                // the treestore
                row[m_columns.sens] = std::atof(m_SensEntry.get_text().c_str());
                m_speaker_list->data()[index].set_sens(std::atof(m_SensEntry.get_text().c_str()));
                break;
            case 10:
                // std::cout << "bass" << std::endl;
                if (m_BassCheckButton.get_active())
                {
                    m_speaker_list->data()[index].set_type(m_speaker_list->data()[index].get_type()
                                                           | SPEAKER_TYPE_BASS);
                }
                else
                {
                    m_speaker_list->data()[index].set_type(m_speaker_list->data()[index].get_type()
                                                           & ~SPEAKER_TYPE_BASS);
                }
                row[m_columns.type] = m_speaker_list->data()[index].get_type();
                signal_speakerlist_loaded(m_speaker_list);
                break;
            case 11:
                // std::cout << "midrange" << std::endl;
                if (m_MidrangeCheckButton.get_active())
                {
                    m_speaker_list->data()[index].set_type(m_speaker_list->data()[index].get_type()
                                                           | SPEAKER_TYPE_MIDRANGE);
                }
                else
                {
                    m_speaker_list->data()[index].set_type(m_speaker_list->data()[index].get_type()
                                                           & ~SPEAKER_TYPE_MIDRANGE);
                }
                row[m_columns.type] = m_speaker_list->data()[index].get_type();
                break;
            case 12:
                // std::cout << "tweeter:" << m_TweeterCheckButton.get_state() << std::endl;
                if (m_TweeterCheckButton.get_active())
                {
                    m_speaker_list->data()[index].set_type(m_speaker_list->data()[index].get_type()
                                                           | SPEAKER_TYPE_TWEETER);
                }
                else
                {
                    m_speaker_list->data()[index].set_type(m_speaker_list->data()[index].get_type()
                                                           & ~SPEAKER_TYPE_TWEETER);
                }
                row[m_columns.type] = m_speaker_list->data()[index].get_type();
                break;
            case 13:
                d = std::atof(m_MmdEntry.get_text().c_str());
                row[m_columns.mmd] = d; // the treestore
                if (d == 0.0) d = 1.0;
                m_speaker_list->data()[index].set_mmd(d);
                update_imp_plot = true;
                break;
            case 14:
                d = std::atof(m_AdEntry.get_text().c_str());
                row[m_columns.ad] = d; // the treestore
                if (d == 0.0) d = 1.0;
                m_speaker_list->data()[index].set_ad(d);
                update_imp_plot = true;
                break;
            case 15:
                d = std::atof(m_BlEntry.get_text().c_str());
                row[m_columns.bl] = d; // the treestore
                if (d == 0.0) d = 1.0;
                m_speaker_list->data()[index].set_bl(d);
                update_imp_plot = true;
                break;
            case 16:
                d = std::atof(m_RmsEntry.get_text().c_str());
                row[m_columns.rms] = d; // the treestore
                if (d == 0.0) d = 1.0;
                m_speaker_list->data()[index].set_rms(d);
                update_imp_plot = true;
                break;
            case 17:
                d = std::atof(m_CmsEntry.get_text().c_str());
                row[m_columns.cms] = d; // the treestore
                if (d == 0.0) d = 1.0;
                m_speaker_list->data()[index].set_cms(d);
                update_imp_plot = true;
                break;
        }
        if (update_imp_plot)
        {
            draw_imp_plot(m_speaker_list->data()[index], true);
        }
    }
    // FIXME gtk3 port
    // m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
    tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);
    GSpeakers::driverlist_modified() = true;
    m_modified = true;
}

void speaker_editor::on_append_xml()
{
    GSpeakersFileChooserDialog fc(_("Append speaker xml"));
    std::string const& filename = fc.get_filename();
    if (!filename.empty())
    {
        append_xml(filename);
    }
}

void speaker_editor::on_open_xml()
{
    GSpeakersFileChooserDialog fc(_("Open speaker xml"));
    std::string filename = fc.get_filename();
    if (!filename.empty())
    {
        open_xml(filename);
    }
}

void speaker_editor::append_xml(const std::string& filename)
{
    try
    {
        auto const& temp_speaker_list = speaker_list(filename);

        std::for_each(begin(temp_speaker_list.data()),
                      end(temp_speaker_list.data()),
                      sigc::mem_fun(*this, &speaker_editor::add_item));
        for (auto& from : temp_speaker_list.data())
        {
            m_speaker_list->data().push_back(from);
        }

        set_entries_sensitive(true);
        m_modified = true;
        // m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
        tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);
        GSpeakers::driverlist_modified() = true;
    }
    catch (std::runtime_error const& e)
    {
        Gtk::MessageDialog m(e.what(), false, Gtk::MESSAGE_ERROR);
        m.run();
    }
}

bool speaker_editor::open_xml(const std::string& filename)
{
    if (!filename.empty())
    {
        try
        {
            speaker_list const& temp_speaker_list = speaker_list(filename);
            m_refListStore->clear();

            m_filename = filename;
            g_settings.setValue("SpeakerListXml", m_filename);
            m_frame_label->set_markup("<b>" + Glib::ustring(_("Drivers ["))
                                      + GSpeakers::short_filename(m_filename, 40) + "]</b>");

            // GSpeakers::tooltips().set_tip(*m_evbox, m_filename);
            std::for_each(temp_speaker_list.data().begin(),
                          temp_speaker_list.data().end(),
                          sigc::mem_fun(*this, &speaker_editor::add_item));

            m_speaker_list->data().clear();
            m_speaker_list->data().insert(begin(m_speaker_list->data()),
                                          begin(temp_speaker_list.data()),
                                          end(temp_speaker_list.data()));

            // Select the first item in the list
            if (!m_speaker_list->data().empty())
            {
                Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

                Gtk::TreeRow row = *(m_refListStore->get_iter(Gtk::TreePath(std::to_string(0))));
                refSelection->select(row);
            }
            m_IdStringEntry.grab_focus();
            set_entries_sensitive(true);

            // FIXME gtk3 port
            // m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
            // m_menu.items()[MENU_INDEX_DELETE].set_sensitive(true);

            GSpeakers::driverlist_modified() = true;
            tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);
            tbar->get_nth_item(TOOLBAR_INDEX_DELETE)->set_sensitive(true);

            signal_speakerlist_loaded(m_speaker_list);

            m_modified = true;
        }
        catch (std::runtime_error const& error)
        {
            Gtk::MessageDialog message(error.what(), false, Gtk::MESSAGE_ERROR);
            message.run();
            return false;
        }
        return true;
    }
    return false;
}

void speaker_editor::on_edit_freq_resp()
{
#ifdef OUTPUT_DEBUG
    std::cout << "SpeakerEditor::on_edit_freq_resp" << std::endl;
    std::cout << "SpeakerEditor::on_edit_freq_resp: index = " << index << std::endl;
#endif
    auto f = std::make_unique<FreqRespEditor>(m_FreqRespFileEntry.get_text());
    f->run();

    m_FreqRespFileEntry.set_text(f->get_filename());

    // GSpeakers::tooltips().set_tip(m_FreqRespFileEntry, m_FreqRespFileEntry.get_text());

    m_speaker_list->data()[index].set_freq_resp_filename(f->get_filename());

    on_selection_changed();

    // m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);

    tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);
    GSpeakers::driverlist_modified() = true;
    m_modified = true;
}

void speaker_editor::on_browse_freq_resp()
{
#ifdef OUTPUT_DEBUG
    std::puts("SpeakerEditor::on_browse_freq_resp");
#endif
    GSpeakersFileChooserDialog fc(_("Open frequency response file"));
    std::string const& filename = fc.get_filename();

    if (!filename.empty())
    {
        /* TODO: Check that selected file exists */
        m_FreqRespFileEntry.set_text(m_filename);

        // GSpeakers::tooltips().set_tip(m_FreqRespFileEntry, m_FreqRespFileEntry.get_text());
        m_speaker_list->data()[index].set_freq_resp_filename(filename);

        on_selection_changed();
        // FIXME gtk3 port
        // m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
        tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);

        m_modified = true;

        GSpeakers::driverlist_modified() = true;
    }
}

void speaker_editor::create_model()
{
    m_refListStore = Gtk::ListStore::create(m_columns);

    std::for_each(begin(m_speaker_list->data()),
                  end(m_speaker_list->data()),
                  sigc::mem_fun(*this, &speaker_editor::add_item));
}

void speaker_editor::add_item(Speaker const& spk)
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

void speaker_editor::add_columns()
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
                                    sigc::mem_fun(*this, &speaker_editor::type_cell_data_func));
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Qts"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this, &speaker_editor::qts_cell_data_func));
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Fs"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this, &speaker_editor::fs_cell_data_func));
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Vas"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this, &speaker_editor::vas_cell_data_func));
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Impedance"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this, &speaker_editor::imp_cell_data_func));
    }
    {
        Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

        int cols_count = m_TreeView.append_column(_("Sensitivity"), *pRenderer);
        Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

        pColumn->set_cell_data_func(*pRenderer,
                                    sigc::mem_fun(*this, &speaker_editor::sens_cell_data_func));
    }
}

void speaker_editor::type_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    std::string s;
    if ((*iter)[m_columns.type] == SPEAKER_TYPE_BASS)
    {
        s = _("Woofer");
    }
    if ((*iter)[m_columns.type] == SPEAKER_TYPE_MIDRANGE)
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
    if ((*iter)[m_columns.type] == SPEAKER_TYPE_TWEETER)
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

void speaker_editor::qts_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.qts], 3, 3);
    renderer.property_xalign() = 1.0;
}

void speaker_editor::fs_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.fs], 3, 0) + " Hz";
    renderer.property_xalign() = 1.0;
}

void speaker_editor::vas_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.vas], 3, 3) + " l";
    renderer.property_xalign() = 1.0;
}

void speaker_editor::rdc_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.rdc], 3, 1) + " Ohm";
    renderer.property_xalign() = 1.0;
}

void speaker_editor::lvc_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.lvc], 3, 2) + " mH";
    renderer.property_xalign() = 1.0;
}

void speaker_editor::qms_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.qms], 3, 3);
    renderer.property_xalign() = 1.0;
}

void speaker_editor::qes_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.qes], 3, 2);
    renderer.property_xalign() = 1.0;
}

void speaker_editor::imp_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.imp], 3, 1) + " Ohm";
    renderer.property_xalign() = 1.0;
}

void speaker_editor::sens_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.sens], 3, 1) + " dB";
    renderer.property_xalign() = 1.0;
}

void speaker_editor::mmd_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.mmd] * 1000, 3, 2)
                               + " g";
    renderer.property_xalign() = 1.0;
}

void speaker_editor::ad_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.ad], 3, 3) + " m";
    renderer.property_xalign() = 1.0;
}

void speaker_editor::bl_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.bl], 3, 1) + " N/A";
    renderer.property_xalign() = 1.0;
}

void speaker_editor::rms_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.rms], 3, 2) + " Ns/m";
    renderer.property_xalign() = 1.0;
}

void speaker_editor::cms_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter)
{
    auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
    renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.cms], 3, 4) + " m/N";
    renderer.property_xalign() = 1.0;
}

void speaker_editor::on_cell_fixed_toggled(const Glib::ustring& path_string)
{
    // TODO: Why doesn't the path(strng) constructor work?
    // Using it causes the new text to be put always in the first row.
    Gtk::TreePath path(path_string);

    // get toggled iter
    Gtk::TreeRow row = *(m_refListStore->get_iter(path));

    bool fixed = row[m_columns.id] != 0;

    // do something with the value
    fixed = !fixed;

    // set new value
    row[m_columns.id] = static_cast<const int>(fixed);
}
