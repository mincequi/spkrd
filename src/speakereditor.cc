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

#include "speakereditor.h"
#include "common.h"
#include "freqrespeditor.h"
#include "gspeakersfilechooser.h"
#include <cmath>
#include <fstream>

#define MENU_INDEX_SAVE 5
#define MENU_INDEX_DELETE 8
#define TOOLBAR_INDEX_SAVE 3
#define TOOLBAR_INDEX_DELETE 5

#define FILE_CHOOSER_CANCEL 10001
#define FILE_CHOOSER_OPEN 10002
#define FILE_CHOOSER_SAVE 10003

Speaker_ListStore::Speaker_ListStore()
    : m_TreeViewTable(10, 4, true), m_Table(19, 2, true), m_EditFreqRespButton(_("Edit...")),
      m_BrowseFreqRespButton(_("...")), m_BassCheckButton(_("Bass")),
      m_MidrangeCheckButton(_("Midrange")), m_TweeterCheckButton(_("Tweeter")),
      m_treeview_frame("") {

  m_modified = false;
  updating_entries = false;
  tbar = nullptr;
#ifdef TARGET_WIN32
  g_settings.defaultValueString("SpeakerListXml", "vifa.xml");
#else
  g_settings.defaultValueString("SpeakerListXml",
                                std::string(GSPEAKERS_PREFIX) + "/share/xml/vifa.xml");
#endif
  m_filename = g_settings.getValueString("SpeakerListXml");

  g_settings.defaultValueBool("DrawDriverImpPlot", false);
  g_settings.defaultValueBool("DrawDriverFreqRespPlot", false);

  index = -1;
  changed = false;

  try {
    m_speaker_list = new SpeakerList(m_filename);
    signal_speakerlist_loaded(m_speaker_list);
  } catch (GSpeakersException const& e) {
    m_speaker_list = new SpeakerList();
    std::cout << "Speaker_ListStore::Speaker_ListStore: " << e.what() << std::endl;
  }
  m_treeview_vbox.set_border_width(5);
  m_treeview_vbox.pack_start(m_treeview_frame);
  m_treeview_frame.add(m_inner_treeview_vbox);
  m_treeview_frame.set_shadow_type(Gtk::SHADOW_NONE);
  m_evbox = manage(new Gtk::EventBox());
  m_frame_label = manage(new Gtk::Label());
  m_frame_label->set_markup("<b>" + Glib::ustring(_("Drivers [")) +
                            GSpeakers::short_filename(m_filename, 50) + "]</b>");
  static_cast<Gtk::Container*>(m_evbox)->add(*m_frame_label);
  //   static_cast<Gtk::Label*>(m_treeview_frame.get_label_widget())->set_markup("<b>" +
  //   Glib::ustring(_("Drivers [")) +
  //                               GSpeakers::short_filename(m_filename) + "]</b>");
  m_treeview_frame.set_label_widget(*m_evbox);

  GSpeakers::tooltips().set_tip(*m_evbox, m_filename);

  m_inner_treeview_vbox.set_border_width(12);
  m_inner_treeview_vbox.pack_start(m_TreeViewTable);

  // m_vbox.pack_start(m_Table);
  m_vbox.set_border_width(2);
  m_Table.set_spacings(2);
  //  m_vbox.pack_start(m_editor_frame);
  //  m_editor_frame.add(m_inner_vbox);
  m_vbox.pack_start(m_Table);
  m_vbox.set_border_width(12);
  //  m_vbox.pack_start(m_Table);

  //  m_editor_frame.set_shadow_type(Gtk::SHADOW_NONE);
  //  static_cast<Gtk::Label*>(m_editor_frame.get_label_widget())->set_markup("<b>" +
  //  Glib::ustring(_("Driver")) + "</b>");
  // m_editor_frame.set_label(_("Currently selected driver"));

  /* Setup the table */
  m_TreeViewTable.set_spacings(2);
  m_TreeViewTable.attach(m_ScrolledWindow, 0, 4, 0, 10);

  /* All the entries and stuff */
  m_Table.attach(*manage(new Gtk::Label(_("Name:"), Gtk::ALIGN_LEFT)), 0, 1, 0, 1);
  m_Table.attach(m_IdStringEntry, 1, 3, 0, 1);
  GSpeakers::tooltips().set_tip(m_IdStringEntry,
                                _("The name or identificationstd::string for the driver"));
  m_Table.attach(*manage(new Gtk::Label(_("Qts:"), Gtk::ALIGN_LEFT)), 0, 2, 1, 2,
                 Gtk::FILL | Gtk::EXPAND | Gtk::SHRINK);
  m_Table.attach(m_QtsEntry, 2, 3, 1, 2);
  m_Table.attach(*manage(new Gtk::Label(_("Fs: (Hz)"), Gtk::ALIGN_LEFT)), 0, 2, 2, 3);
  m_Table.attach(m_FsEntry, 2, 3, 2, 3);
  m_Table.attach(*manage(new Gtk::Label(_("Vas: (l)"), Gtk::ALIGN_LEFT)), 0, 2, 3, 4);
  m_Table.attach(m_VasEntry, 2, 3, 3, 4);
  m_Table.attach(*manage(new Gtk::Label(_("Rdc: (Ohm)"), Gtk::ALIGN_LEFT)), 0, 2, 4, 5);
  m_Table.attach(m_RdcEntry, 2, 3, 4, 5);
  m_Table.attach(*manage(new Gtk::Label(_("Lvc: (mH)"), Gtk::ALIGN_LEFT)), 0, 2, 5, 6);
  m_Table.attach(m_LvcEntry, 2, 3, 5, 6);
  m_Table.attach(*manage(new Gtk::Label(_("Qms:"), Gtk::ALIGN_LEFT)), 0, 2, 6, 7);
  m_Table.attach(m_QmsEntry, 2, 3, 6, 7);
  m_Table.attach(*manage(new Gtk::Label(_("Qes:"), Gtk::ALIGN_LEFT)), 0, 2, 7, 8);
  m_Table.attach(m_QesEntry, 2, 3, 7, 8);
  m_Table.attach(*manage(new Gtk::Label(_("Impedance: (Ohm)"), Gtk::ALIGN_LEFT)), 0, 2, 8, 9);
  m_Table.attach(m_ImpEntry, 2, 3, 8, 9);
  m_Table.attach(*manage(new Gtk::Label(_("Sensitivity: (dB/W/m)"), Gtk::ALIGN_LEFT)), 0, 2, 9, 10);
  m_Table.attach(m_SensEntry, 2, 3, 9, 10);

  m_Table.attach(*manage(new Gtk::Label(_("Cone mass: (kg)"), Gtk::ALIGN_LEFT)), 0, 2, 10, 11);
  m_Table.attach(m_MmdEntry, 2, 3, 10, 11);
  m_Table.attach(*manage(new Gtk::Label(_("Effective radius: (m)"), Gtk::ALIGN_LEFT)), 0, 2, 11,
                 12);
  m_Table.attach(m_AdEntry, 2, 3, 11, 12);
  m_Table.attach(*manage(new Gtk::Label(_("Force factor: (N/A)"), Gtk::ALIGN_LEFT)), 0, 2, 12, 13);
  m_Table.attach(m_BlEntry, 2, 3, 12, 13);
  m_Table.attach(*manage(new Gtk::Label(_("Susp. resistance: (Ns/m)"), Gtk::ALIGN_LEFT)), 0, 2, 13,
                 14);
  m_Table.attach(m_RmsEntry, 2, 3, 13, 14);
  m_Table.attach(*manage(new Gtk::Label(_("Susp. compleance: (m/N)"), Gtk::ALIGN_LEFT)), 0, 2, 14,
                 15);
  m_Table.attach(m_CmsEntry, 2, 3, 14, 15);

  GSpeakers::tooltips().set_tip(
      m_BassCheckButton, _("Check this box if the driver will work as a woofer (bass speaker)"));
  m_Table.attach(m_BassCheckButton, 0, 3, 15, 16);
  GSpeakers::tooltips().set_tip(m_MidrangeCheckButton,
                                _("Check this box if the driver will work as a midrange speaker"));
  m_Table.attach(m_MidrangeCheckButton, 0, 3, 16, 17);
  GSpeakers::tooltips().set_tip(m_TweeterCheckButton,
                                _("Check this box if the driver will work as a tweeter"));
  m_Table.attach(m_TweeterCheckButton, 0, 3, 17, 18);

  // m_Table.attach(*manage(new Gtk::Label(_("Freq resp file:"), Gtk::ALIGN_LEFT)), 0, 1, 18, 19);
  // m_Table.attach(m_FreqRespFileEntry, 1, 2, 18, 19);
  Gtk::HBox* vbox = manage(new Gtk::HBox());
  vbox->pack_start(*manage(new Gtk::Label(_("Freq resp file:"), Gtk::ALIGN_LEFT)));
  vbox->pack_start(m_FreqRespFileEntry);
  m_FreqRespFileEntry.set_width_chars(15);
  vbox->pack_start(m_BrowseFreqRespButton);
  vbox->pack_start(m_EditFreqRespButton);
  vbox->set_spacing(12);
  m_Table.attach(*vbox, 0, 3, 18, 19);

  m_ScrolledWindow.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  m_IdStringEntry.signal_changed().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 0));
  m_QtsEntry.signal_changed().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 1));
  m_FsEntry.signal_changed().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 2));
  m_VasEntry.signal_changed().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 3));
  m_RdcEntry.signal_changed().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 4));
  m_LvcEntry.signal_changed().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 5));
  m_QmsEntry.signal_changed().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 6));
  m_QesEntry.signal_changed().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 7));
  m_ImpEntry.signal_changed().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 8));
  m_SensEntry.signal_changed().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 9));
  m_MmdEntry.signal_changed().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 13));
  m_AdEntry.signal_changed().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 14));
  m_BlEntry.signal_changed().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 15));
  m_RmsEntry.signal_changed().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 16));
  m_CmsEntry.signal_changed().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 17));
  m_FreqRespFileEntry.signal_changed().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 18));
  m_FreqRespFileEntry.set_editable(false);

  m_BassCheckButton.signal_toggled().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 10));
  m_MidrangeCheckButton.signal_toggled().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 11));
  m_TweeterCheckButton.signal_toggled().connect(
      sigc::bind<int>(mem_fun(*this, &Speaker_ListStore::on_entry_changed), 12));

  m_EditFreqRespButton.signal_clicked().connect(
      mem_fun(*this, &Speaker_ListStore::on_edit_freq_resp));
  m_BrowseFreqRespButton.signal_clicked().connect(
      mem_fun(*this, &Speaker_ListStore::on_browse_freq_resp));

  GSpeakers::tooltips().set_tip(m_EditFreqRespButton, _("Edit frequency response for this driver"));
  GSpeakers::tooltips().set_tip(m_BrowseFreqRespButton, _("Browse for frequnecy response file"));
  set_entries_sensitive(false);

  /* create model */
  create_model();

  /* create tree view */
  m_TreeView.set_model(m_refListStore);
  m_TreeView.set_rules_hint();
  // m_TreeView.set_search_column(m_columns.id.index());
  Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();
  // selection->set_mode(Gtk::SELECTION_MULTIPLE);

  selection->signal_changed().connect(mem_fun(*this, &Speaker_ListStore::on_selection_changed));
  // selection->set_mode(Gtk::SELECTION_SINGLE);

  g_settings.settings_changed.connect(mem_fun(*this, &Speaker_ListStore::on_settings_changed));

  add_columns();
  m_ScrolledWindow.add(m_TreeView);
  f_append = nullptr;
  f_open = nullptr;
  f_save_as = nullptr;
  new_xml_pressed = false;
  signal_save_open_files.connect(mem_fun(*this, &Speaker_ListStore::on_save_open_files));

  /* Select first row */
  //   if (m_refListStore->children().size() > 0) {
  //     Gtk::TreePath path(m_refListStore->children().begin());
  //     Gtk::TreeRow row = *(m_refListStore->get_iter(path));
  //     selection->select(row);
  //   }
}

Speaker_ListStore::~Speaker_ListStore() {
  if (m_modified) {
#ifdef OUTPUT_DEBUG
    // Insert dialog here that asks if we want to save changes
    std::cout << "Speaker_ListStore::~Speaker_ListStore: save changes?" << std::endl;
#endif
  }
}

Gtk::Widget& Speaker_ListStore::get_treeview_table() { return m_treeview_vbox; }

Gtk::Widget& Speaker_ListStore::get_editor_table() { return m_vbox; }

Gtk::Widget& Speaker_ListStore::get_plot() { return plot; }

Gtk::Menu& Speaker_ListStore::get_menu() {
  Gtk::Menu::MenuList& menulist = m_menu.items();
  Gtk::Widget* im = manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
  menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("_New Driver"), *im,
                                                      mem_fun(*this, &Speaker_ListStore::on_new)));
  menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

  im = manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
  menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("N_ew"), *im, mem_fun(*this, &Speaker_ListStore::on_new_xml)));

  menulist.push_back(Gtk::Menu_Helpers::MenuElem(
      _("A_ppend Driver Xml..."), mem_fun(*this, &Speaker_ListStore::on_append_xml)));

  im = manage(new Gtk::Image(Gtk::Stock::OPEN, Gtk::ICON_SIZE_MENU));
  menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("_Open..."), *im, mem_fun(*this, &Speaker_ListStore::on_open_xml)));

  menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

  im = manage(new Gtk::Image(Gtk::Stock::SAVE, Gtk::ICON_SIZE_MENU));
  menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("_Save"), *im,
                                                      mem_fun(*this, &Speaker_ListStore::on_save)));

  im = manage(new Gtk::Image(Gtk::Stock::SAVE_AS, Gtk::ICON_SIZE_MENU));
  menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("Save _As..."), *im, mem_fun(*this, &Speaker_ListStore::on_save_as)));

  menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

  im = manage(new Gtk::Image(Gtk::Stock::DELETE, Gtk::ICON_SIZE_MENU));
  menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("Delete"), *im, mem_fun(*this, &Speaker_ListStore::on_remove)));

  menulist[MENU_INDEX_SAVE].set_sensitive(false);
  menulist[MENU_INDEX_DELETE].set_sensitive(false);
  return m_menu;
}

Gtk::Widget& Speaker_ListStore::get_toolbar() {
  if (tbar == nullptr) {
    tbar = manage(new Gtk::Toolbar());
    // TODO: tooltips
    Gtk::Widget* im = manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_LARGE_TOOLBAR));
    Gtk::ToolButton* t = manage(new Gtk::ToolButton(*im, _("New Driver")));
    t->signal_clicked().connect(mem_fun(*this, &Speaker_ListStore::on_new));
    tbar->append(*t);

    Gtk::SeparatorToolItem* s = manage(new Gtk::SeparatorToolItem());
    tbar->append(*s);

    t = manage(new Gtk::ToolButton(Gtk::Stock::OPEN));
    t->signal_clicked().connect(mem_fun(*this, &Speaker_ListStore::on_open_xml));
    tbar->append(*t);
    t = manage(new Gtk::ToolButton(Gtk::Stock::SAVE));
    t->signal_clicked().connect(mem_fun(*this, &Speaker_ListStore::on_save));
    tbar->append(*t);

    s = manage(new Gtk::SeparatorToolItem());
    tbar->append(*s);

    t = manage(new Gtk::ToolButton(Gtk::Stock::DELETE));
    t->signal_clicked().connect(mem_fun(*this, &Speaker_ListStore::on_remove));
    tbar->append(*t);

    toolbar.add(*tbar);
    tbar->set_toolbar_style((Gtk::ToolbarStyle)g_settings.getValueUnsignedInt("ToolbarStyle"));
    tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(false);
    tbar->get_nth_item(TOOLBAR_INDEX_DELETE)->set_sensitive(false);
  }
  return toolbar;
}

void Speaker_ListStore::on_save_open_files() {
  if (GSpeakers::driverlist_modified()) {
    on_save();
  }
}

void Speaker_ListStore::on_settings_changed(const std::string& s) {
  if (s == "ToolbarStyle") {
    tbar->set_toolbar_style((Gtk::ToolbarStyle)g_settings.getValueUnsignedInt("ToolbarStyle"));
  }
  if ((s == "DrawDriverImpPlot") || (s == "DrawDriverFreqRespPlot")) {
    if (index != -1) {
      on_selection_changed();
    }
  }
}

void Speaker_ListStore::on_close() { signal_speakerlist_loaded(m_speaker_list); }

void Speaker_ListStore::set_entries_sensitive(bool value) {
  m_IdStringEntry.set_sensitive(value);
  m_QtsEntry.set_sensitive(value);
  m_FsEntry.set_sensitive(value);
  m_VasEntry.set_sensitive(value);
  m_RdcEntry.set_sensitive(value);
  m_LvcEntry.set_sensitive(value);
  m_QmsEntry.set_sensitive(value);
  m_QesEntry.set_sensitive(value);
  m_ImpEntry.set_sensitive(value);
  m_SensEntry.set_sensitive(value);
  m_MmdEntry.set_sensitive(value);
  m_AdEntry.set_sensitive(value);
  m_BlEntry.set_sensitive(value);
  m_RmsEntry.set_sensitive(value);
  m_CmsEntry.set_sensitive(value);

  m_FreqRespFileEntry.set_sensitive(value);
  m_ImpRespFileEntry.set_sensitive(value);
  m_BassCheckButton.set_sensitive(value);
  m_MidrangeCheckButton.set_sensitive(value);
  m_TweeterCheckButton.set_sensitive(value);
  m_BrowseFreqRespButton.set_sensitive(value);
  m_EditFreqRespButton.set_sensitive(value);

  if (!value) {
    m_BassCheckButton.set_active(false);
    m_MidrangeCheckButton.set_active(false);
    m_TweeterCheckButton.set_active(false);
  }
}

void Speaker_ListStore::on_new() {
  Speaker s(_("New Speaker"));
  s.set_id_string(s.get_id_string() + " " + GSpeakers::int_to_ustring(s.get_id()));
  liststore_add_item(s);
  m_speaker_list->speaker_list()->push_back(s);

  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  char* str = nullptr;
  GString* buffer = g_string_new(str);
  g_string_printf(buffer, "%lu", m_speaker_list->speaker_list()->size() - 1);
  GtkTreePath* gpath = gtk_tree_path_new_from_string(buffer->str);
  Gtk::TreePath path(gpath);
  Gtk::TreeRow row = *(m_refListStore->get_iter(path));
  refSelection->select(row);

  m_IdStringEntry.grab_focus();
  set_entries_sensitive(true);
  m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
  m_menu.items()[MENU_INDEX_DELETE].set_sensitive(true);
  tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);
  tbar->get_nth_item(TOOLBAR_INDEX_DELETE)->set_sensitive(true);
  GSpeakers::driverlist_modified() = true;
  signal_speakerlist_loaded(m_speaker_list);
  m_modified = true;
}

void Speaker_ListStore::on_new_xml() {
  m_refListStore->clear();
  m_speaker_list->clear();
  set_entries_sensitive(false);
  new_xml_pressed = true;
  /* Add one new speaker to the new xml file, we usually want to do this  */
  on_new();
}

void Speaker_ListStore::on_remove() {
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  if (const Gtk::TreeIter iter = refSelection->get_selected()) {
    Gtk::TreePath path = m_refListStore->get_path(iter);

    std::vector<int> indices = path.get_indices();
    if (!indices.empty()) {
      // Remove item from ListStore:
      m_refListStore->erase(iter);

      if (index < (int)m_speaker_list->speaker_list()->size())
        m_speaker_list->speaker_list()->erase(m_speaker_list->speaker_list()->begin() + index);
    }
  }
  m_menu.items()[MENU_INDEX_DELETE].set_sensitive(false);
}

void Speaker_ListStore::on_save() {
#ifdef OUTPUT_DEBUG
  std::cout << "SpeakerEditor: save" << std::endl;
#endif
  if (new_xml_pressed) {
    on_save_as();
    new_xml_pressed = false;
  } else {
#ifdef OUTPUT_DEBUG
    std::cout << "SpeakerEditor: Filename = " << m_filename << std::endl;
#endif
    try {
      m_speaker_list->to_xml(m_filename);
      m_menu.items()[MENU_INDEX_SAVE].set_sensitive(false);
      tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(false);
      GSpeakers::driverlist_modified() = false;
      m_modified = false;
    } catch (GSpeakersException const& e) {
      Gtk::MessageDialog m(e.what(), false, Gtk::MESSAGE_ERROR);
      m.run();
    }
  }
}

void Speaker_ListStore::on_save_as() {
  GSpeakersFileChooserDialog* fc = new GSpeakersFileChooserDialog(
      _("Save speaker xml as"), Gtk::FILE_CHOOSER_ACTION_SAVE, m_filename);
  std::string filename = fc->get_filename();
  if (filename.length() > 0) {
    save_as(filename);
  }
}

void Speaker_ListStore::save_as(const std::string& filename) {
  try {
    m_speaker_list->to_xml(filename);
    m_filename = filename;
  } catch (GSpeakersException const& e) {
    Gtk::MessageDialog m(e.what(), false, Gtk::MESSAGE_ERROR);
    m.run();
  }
}

void Speaker_ListStore::on_selection_changed() {
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();
  updating_entries = true;
  if (const Gtk::TreeIter iter = refSelection->get_selected()) {
    Gtk::TreePath path = m_refListStore->get_path(iter);

    std::vector<int> indices = path.get_indices();
    if (!indices.empty()) {
      index = indices[0];
      Speaker s = (*(m_speaker_list->speaker_list()))[index];

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
      if ((s.get_type() & SPEAKER_TYPE_BASS) != 0) {
        m_BassCheckButton.set_active(true);
      } else {
        m_BassCheckButton.set_active(false);
      }
      if ((s.get_type() & SPEAKER_TYPE_MIDRANGE) != 0) {
        m_MidrangeCheckButton.set_active(true);
      } else {
        m_MidrangeCheckButton.set_active(false);
      }
      if ((s.get_type() & SPEAKER_TYPE_TWEETER) != 0) {
        m_TweeterCheckButton.set_active(true);
      } else {
        m_TweeterCheckButton.set_active(false);
      }

      /* Freq resp file entry */
      m_FreqRespFileEntry.set_text(s.get_freq_resp_filename());
      GSpeakers::tooltips().set_tip(m_FreqRespFileEntry, s.get_freq_resp_filename());
      plot.clear();
      if (g_settings.getValueBool("DrawDriverFreqRespPlot")) {

        /* Plot freq resp if it exists */
        if (!s.get_freq_resp_filename().empty()) {
          std::ifstream fin(s.get_freq_resp_filename().c_str());
          if (fin.good()) {
            std::vector<GSpeakers::Point> points;
            while (!fin.eof()) {
              char* buffer = new char[100];
              fin.getline(buffer, 100, '\n');

              float f1, f2;
              // sscanf(buffer, "%f,%f", &f1, &f2);
              char* substr_ptr = strtok(buffer, ",");
              f1 = g_ascii_strtod(substr_ptr, nullptr);
              substr_ptr = strtok(nullptr, ",");
              f2 = g_ascii_strtod(substr_ptr, nullptr);

              // std::cout << f1 << ", " << f2 << std::endl;
              GSpeakers::Point p(GSpeakers::round(f1), f2);
              points.push_back(p);
              delete buffer;
            }
            Gdk::Color c("blue");
            plot.add_plot(points, c);
          } else {
            std::cout << "Could not open " << s.get_freq_resp_filename() << std::endl;
          }
        }
      }
      /* Plot impedance response */
      /* increase impedance y coordinate 50 to align to imp scale */
      draw_imp_plot(s);
    }
  }
  updating_entries = false;
  m_IdStringEntry.grab_focus();
  set_entries_sensitive(true);
  m_menu.items()[MENU_INDEX_DELETE].set_sensitive(true);
  tbar->get_nth_item(TOOLBAR_INDEX_DELETE)->set_sensitive(true);
}

void Speaker_ListStore::draw_imp_plot(Speaker& s, bool update) {
  auto* buffer = new gchar[8];

  if (g_settings.getValueBool("DrawDriverImpPlot")) {
    std::vector<GSpeakers::Point> points;
    /* Produce SPICE input-file */
#ifdef TARGET_WIN32
    std::string tmp_file =
        Glib::get_tmp_dir() + "\\speaker" + GSpeakers::int_to_ustring(s.get_id()) + ".tmp";
#else
    std::string tmp_file =
        Glib::get_tmp_dir() + "/speaker" + GSpeakers::int_to_ustring(s.get_id()) + ".tmp";
#endif

    std::ofstream of(tmp_file.c_str());
    if (of.good()) {
      of << "SPICE code generated by GSpeakers Redux-" << VERSION << std::endl;
      of << "vamp 1 0 dc 0 ac 1" << std::endl;
      double cmes = (s.get_mmd() / (s.get_bl() * s.get_bl())) * 1000000;
      double lces = s.get_bl() * s.get_bl() * s.get_cms() * 1000;
      double res = (s.get_bl() * s.get_bl()) / s.get_rms();
      double po = 1.18; // air density kg/m^3
      double cmef =
          8 * po * s.get_ad() * s.get_ad() * s.get_ad() / (3 * s.get_bl() * s.get_bl()) * 1000000;
      of << "R" << s.get_id() << " 1 2 " << g_ascii_dtostr(buffer, 8, s.get_rdc()) << std::endl;
      of << "L" << s.get_id() << " 2 3 " << g_ascii_dtostr(buffer, 8, s.get_lvc()) << "mH"
         << std::endl;
      of << "lces 3 0 " << g_ascii_dtostr(buffer, 8, lces) << "mH" << std::endl;
      of << "cmes 3 0 " << g_ascii_dtostr(buffer, 8, cmes) << "uF" << std::endl;
      of << "res 3 0 " << g_ascii_dtostr(buffer, 8, res) << std::endl;
      of << "cmef 3 0 " << g_ascii_dtostr(buffer, 8, cmef) << "uF" << std::endl;
      if (g_settings.getValueBool("SPICEUseGNUCAP")) {
        of << ".print ac ir(vamp) ii(vamp)" << std::endl;
        of << ".ac DEC 50 20 20k" << std::endl;
      } else {
        of << ".ac DEC 50 20 20k" << std::endl;
        of << ".print ac 1/mag(i(vamp))" << std::endl;
      }

      of << ".end" << std::endl;
      of.close();
      std::string cmd;
      if (g_settings.getValueBool("SPICEUseNGSPICE") || g_settings.getValueBool("SPICEUseGNUCAP")) {
        cmd = g_settings.getValueString("SPICECmdLine") + " -b " + tmp_file + " > " + tmp_file +
              ".out";
      } else {
        cmd = g_settings.getValueString("SPICECmdLine") + " -b -o " + tmp_file + ".out " + tmp_file;
      }
      // g_settings.getValueString("SPICECmdLine") + " -b -o " + tmp_file + ".out " + tmp_file;
#ifdef OUTPUT_DEBUG
      std::cout << "Speaker_ListStore::draw_imp_plot: running SPICE with \"" + cmd + "\""
                << std::endl;
#endif
      system(cmd.c_str());
#ifdef OUTPUT_DEBUG
      std::cout << "Speaker_ListStore::draw_imp_plot: SPICE done" << std::endl;
#endif
      std::string spice_output_file = tmp_file + ".out";
      std::ifstream fin(spice_output_file.c_str());
      if (fin.good()) {
        bool output = false;

        float f_id;
        float f1, f2, f3;
        while (!fin.eof()) {
          char* buffer = new char[100];
          fin.getline(buffer, 100, '\n');
          if (g_settings.getValueBool("SPICEUseGNUCAP")) {
            if (buffer[0] == ' ') {
              output = true;
            }
          } else {
            if (buffer[0] == '0') {
              output = true;
            }
          }

          if (output) {

            if (g_settings.getValueBool("SPICEUseGNUCAP")) {
              f_id = atof(buffer);
              //	      std::cout << f_id << std::endl;
              if (f_id != 0) {
                /* Check if we got a freq more than 10kHz */
                char* substr_ptr = strstr(buffer, "K");
                if (substr_ptr != nullptr) {
                  f1 = f_id * 1000;
                } else {
                  f1 = f_id;
                }
                substr_ptr = strtok(buffer, " ");
                substr_ptr = strtok(nullptr, " ");
                f2 = g_ascii_strtod(substr_ptr, nullptr);
                if (strstr(substr_ptr, "m") != nullptr) {
                  f2 = f2 / 1000.;
                } else if (strstr(substr_ptr, "u") != nullptr) {
                  f2 = f2 / 1000000.;
                } else if (strstr(substr_ptr, "n") != nullptr) {
                  f2 = f2 / 1000000000.;
                } else if (strstr(substr_ptr, "p") != nullptr) {
                  f2 = f2 / 1000000000000.;
                }
                substr_ptr = strtok(nullptr, " ");
                f3 = g_ascii_strtod(substr_ptr, nullptr);
                if (strstr(substr_ptr, "m") != nullptr) {
                  f3 = f3 / 1000.;
                } else if (strstr(substr_ptr, "u") != nullptr) {
                  f3 = f3 / 1000000.;
                } else if (strstr(substr_ptr, "n") != nullptr) {
                  f3 = f3 / 1000000000.;
                } else if (strstr(substr_ptr, "p") != nullptr) {
                  f3 = f3 / 1000000000000.;
                }

                // std::cout << f1 << "\t" << f2 << "\t" << f3 << std::endl;
                GSpeakers::Point p(GSpeakers::round(f1), 50 + (1 / std::hypot(f2, f3)));
                points.push_back(p);
              }
            } else {
              if ((buffer[0] >= '0') && (buffer[0] <= '9')) {

                strtok(buffer, "\t");
                char* substr_ptr = strtok(nullptr, "\t");

                f1 = g_ascii_strtod(substr_ptr, nullptr);
                substr_ptr = strtok(nullptr, "\t");
                f2 = g_ascii_strtod(substr_ptr, nullptr);

                GSpeakers::Point p(GSpeakers::round(f1), 50 + f2);
                points.push_back(p);
              }
            }
          }
          delete buffer;
        }
        Gdk::Color c2("red");
        if (update) {
          int i;
          if ((s.get_freq_resp_filename().empty()) ||
              !(g_settings.getValueBool("DrawDriverFreqRespPlot"))) {
            i = 0;
          } else {
            i = 1;
          }
          plot.replace_plot(i, points, c2);
        } else {
          plot.add_plot(points, c2);
        }
      }
    }
  }
}

void Speaker_ListStore::on_clear() {
  m_refListStore->clear();
  m_speaker_list->speaker_list()->erase(m_speaker_list->speaker_list()->begin(),
                                        m_speaker_list->speaker_list()->end());
  m_modified = true;
  // m_TreeView.signal_select_all();
}

void Speaker_ListStore::on_entry_changed(int i) {
  bool update_imp_plot = false;

  if (!updating_entries) {
    /* This treeview stuff is kind of weird... */
    Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

    if (Gtk::TreeIter iter = refSelection->get_selected()) {
      Gtk::TreePath path = m_refListStore->get_path(iter);
      Gtk::TreeRow row = *(m_refListStore->get_iter(path));

      /* Update the data container */
      double d;
      switch (i) {
      case 0:
        row[m_columns.id_string] = m_IdStringEntry.get_text(); // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_id_string(
            m_IdStringEntry.get_text()); // speaker_list
        break;
      case 1:
        row[m_columns.qts] = atof(m_QtsEntry.get_text().c_str()); // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_qts(
            atof(m_QtsEntry.get_text().c_str())); // speaker_list
        break;
      case 2:
        row[m_columns.fs] = atof(m_FsEntry.get_text().c_str()); // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_fs(
            atof(m_FsEntry.get_text().c_str())); // speaker_list
        break;
      case 3:
        row[m_columns.vas] = atof(m_VasEntry.get_text().c_str()); // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_vas(
            atof(m_VasEntry.get_text().c_str())); // speaker_list
        break;
      case 4:
        d = atof(m_RdcEntry.get_text().c_str());
        row[m_columns.rdc] = d; // the treestore
        std::cout << "d = " << d << std::endl;
        if (d == 0.0)
          d = 1.0;
        (*(m_speaker_list->speaker_list()))[index].set_rdc(d); // speaker_list
        update_imp_plot = true;
        break;
      case 5:
        d = atof(m_LvcEntry.get_text().c_str());
        row[m_columns.lvc] = d;
        if (d == 0.0)
          d = 1.0;
        (*(m_speaker_list->speaker_list()))[index].set_lvc(d); // speaker_list
        break;
        update_imp_plot = true;
      case 6:
        row[m_columns.qms] = atof(m_QmsEntry.get_text().c_str()); // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_qms(
            atof(m_QmsEntry.get_text().c_str())); // speaker_list
        break;
      case 7:
        row[m_columns.qes] = atof(m_QesEntry.get_text().c_str()); // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_qes(
            atof(m_QesEntry.get_text().c_str())); // speaker_list
        break;
      case 8:
        row[m_columns.imp] = atof(m_ImpEntry.get_text().c_str()); // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_imp(
            atof(m_ImpEntry.get_text().c_str())); // speaker_list
        break;
      case 9:
        row[m_columns.sens] = atof(m_SensEntry.get_text().c_str()); // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_sens(
            atof(m_SensEntry.get_text().c_str())); // speaker_list
        break;
      case 10:
        // std::cout << "bass" << std::endl;
        if (m_BassCheckButton.get_active()) {
          (*(m_speaker_list->speaker_list()))[index].set_type(
              (*(m_speaker_list->speaker_list()))[index].get_type() | SPEAKER_TYPE_BASS);
        } else {
          (*(m_speaker_list->speaker_list()))[index].set_type(
              (*(m_speaker_list->speaker_list()))[index].get_type() & ~SPEAKER_TYPE_BASS);
        }
        row[m_columns.type] = (*(m_speaker_list->speaker_list()))[index].get_type();
        signal_speakerlist_loaded(m_speaker_list);
        break;
      case 11:
        // std::cout << "midrange" << std::endl;
        if (m_MidrangeCheckButton.get_active()) {
          (*(m_speaker_list->speaker_list()))[index].set_type(
              (*(m_speaker_list->speaker_list()))[index].get_type() | SPEAKER_TYPE_MIDRANGE);
        } else {
          (*(m_speaker_list->speaker_list()))[index].set_type(
              (*(m_speaker_list->speaker_list()))[index].get_type() & ~SPEAKER_TYPE_MIDRANGE);
        }
        row[m_columns.type] = (*(m_speaker_list->speaker_list()))[index].get_type();
        break;
      case 12:
        // std::cout << "tweeter:" << m_TweeterCheckButton.get_state() << std::endl;
        if (m_TweeterCheckButton.get_active()) {
          (*(m_speaker_list->speaker_list()))[index].set_type(
              (*(m_speaker_list->speaker_list()))[index].get_type() | SPEAKER_TYPE_TWEETER);
        } else {
          (*(m_speaker_list->speaker_list()))[index].set_type(
              (*(m_speaker_list->speaker_list()))[index].get_type() & ~SPEAKER_TYPE_TWEETER);
        }
        row[m_columns.type] = (*(m_speaker_list->speaker_list()))[index].get_type();
        break;
      case 13:
        d = atof(m_MmdEntry.get_text().c_str());
        row[m_columns.mmd] = d; // the treestore
        if (d == 0.0)
          d = 1.0;
        (*(m_speaker_list->speaker_list()))[index].set_mmd(d); // speaker_list
        update_imp_plot = true;
        break;
      case 14:
        d = atof(m_AdEntry.get_text().c_str());
        row[m_columns.ad] = d; // the treestore
        if (d == 0.0)
          d = 1.0;
        (*(m_speaker_list->speaker_list()))[index].set_ad(d); // speaker_list
        update_imp_plot = true;
        break;
      case 15:
        d = atof(m_BlEntry.get_text().c_str());
        row[m_columns.bl] = d; // the treestore
        if (d == 0.0)
          d = 1.0;
        (*(m_speaker_list->speaker_list()))[index].set_bl(d); // speaker_list
        update_imp_plot = true;
        break;
      case 16:
        d = atof(m_RmsEntry.get_text().c_str());
        row[m_columns.rms] = d; // the treestore
        if (d == 0.0)
          d = 1.0;
        (*(m_speaker_list->speaker_list()))[index].set_rms(d); // speaker_list
        update_imp_plot = true;
        break;
      case 17:
        d = atof(m_CmsEntry.get_text().c_str());
        row[m_columns.cms] = d; // the treestore
        if (d == 0.0)
          d = 1.0;
        (*(m_speaker_list->speaker_list()))[index].set_cms(d); // speaker_list
        update_imp_plot = true;
        break;
      }
      if (update_imp_plot) {
        /* update impedance plot */
        draw_imp_plot((*(m_speaker_list->speaker_list()))[index], true);
      }
    }
    m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
    tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);
    GSpeakers::driverlist_modified() = true;
    m_modified = true;
  }
}

void Speaker_ListStore::on_append_xml() {
  GSpeakersFileChooserDialog* fc = new GSpeakersFileChooserDialog(_("Append speaker xml"));
  std::string filename = fc->get_filename();
  if (filename.length() > 0) {
    append_xml(filename);
  }
}

void Speaker_ListStore::on_open_xml() {
  GSpeakersFileChooserDialog* fc = new GSpeakersFileChooserDialog(_("Open speaker xml"));
  std::string filename = fc->get_filename();
  if (filename.length() > 0) {
    open_xml(filename);
  }
}

void Speaker_ListStore::append_xml(const std::string& filename) {
  SpeakerList temp_speaker_list;
  try {
    temp_speaker_list = SpeakerList(filename);

    for_each(temp_speaker_list.speaker_list()->begin(), temp_speaker_list.speaker_list()->end(),
             mem_fun(*this, &Speaker_ListStore::liststore_add_item));
    for (auto& from : *temp_speaker_list.speaker_list()) {
      m_speaker_list->speaker_list()->push_back(from);
    }
    m_speaker_list->speaker_list()->size();
    set_entries_sensitive(true);
    m_modified = true;
    m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
    tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);
    GSpeakers::driverlist_modified() = true;
  } catch (GSpeakersException const& e) {
    Gtk::MessageDialog m(e.what(), false, Gtk::MESSAGE_ERROR);
    m.run();
  }
}

bool Speaker_ListStore::open_xml(const std::string& filename) {
  SpeakerList temp_speaker_list;

  if (filename.length() > 0) {
    try {
      temp_speaker_list = SpeakerList(filename);
      m_refListStore->clear();

      m_filename = filename;
      g_settings.setValue("SpeakerListXml", m_filename);
      m_frame_label->set_markup("<b>" + Glib::ustring(_("Drivers [")) +
                                GSpeakers::short_filename(m_filename, 40) + "]</b>");
      GSpeakers::tooltips().set_tip(*m_evbox, m_filename);
      for_each(temp_speaker_list.speaker_list()->begin(), temp_speaker_list.speaker_list()->end(),
               mem_fun(*this, &Speaker_ListStore::liststore_add_item));

      /* Delete items in speaker_list */
      m_speaker_list->speaker_list()->erase(m_speaker_list->speaker_list()->begin(),
                                            m_speaker_list->speaker_list()->end());

      for (auto& from : *temp_speaker_list.speaker_list()) {
        m_speaker_list->speaker_list()->push_back(from);
      }

      /* Select the first item in the list */
      // std::cout << m_speaker_list.speaker_list()->size() << std::endl;
      if (!m_speaker_list->speaker_list()->empty()) {
        Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();
        char* str = nullptr;
        GString* buffer = g_string_new(str);
        g_string_printf(buffer, "%d", 0);
        GtkTreePath* gpath = gtk_tree_path_new_from_string(buffer->str);
        Gtk::TreePath path(gpath);

        Gtk::TreeRow row = *(m_refListStore->get_iter(path));
        refSelection->select(row);
      }
      m_IdStringEntry.grab_focus();
      set_entries_sensitive(true);
      m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
      m_menu.items()[MENU_INDEX_DELETE].set_sensitive(true);
      GSpeakers::driverlist_modified() = true;
      tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);
      tbar->get_nth_item(TOOLBAR_INDEX_DELETE)->set_sensitive(true);

      signal_speakerlist_loaded(m_speaker_list);
      m_modified = true;
    } catch (GSpeakersException const& e) {
      Gtk::MessageDialog m(e.what(), false, Gtk::MESSAGE_ERROR);
      m.run();
      return false;
    }
    return true;
  } else {
    return false;
  }
}

void Speaker_ListStore::on_edit_freq_resp() {
#ifdef OUTPUT_DEBUG
  std::cout << "SpeakerEditor::on_edit_freq_resp" << std::endl;
  std::cout << "SpeakerEditor::on_edit_freq_resp: index = " << index << std::endl;
#endif
  FreqRespEditor* f = new FreqRespEditor(m_FreqRespFileEntry.get_text());
  f->run();
  m_FreqRespFileEntry.set_text(f->get_filename());
  GSpeakers::tooltips().set_tip(m_FreqRespFileEntry, m_FreqRespFileEntry.get_text());
  (*m_speaker_list->speaker_list())[index].set_freq_resp_filename(f->get_filename());
  delete f;
  on_selection_changed();
  m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
  tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);
  GSpeakers::driverlist_modified() = true;
  m_modified = true;
}

void Speaker_ListStore::on_browse_freq_resp() {
#ifdef OUTPUT_DEBUG
  std::cout << "SpeakerEditor::on_browse_freq_resp" << std::endl;
#endif
  GSpeakersFileChooserDialog* fc =
      new GSpeakersFileChooserDialog(_("Open frequency response file"));
  std::string filename = fc->get_filename();

  if (filename.length() > 0) {
    /* TODO: Check that selected file exists */
    m_FreqRespFileEntry.set_text(m_filename);
    GSpeakers::tooltips().set_tip(m_FreqRespFileEntry, m_FreqRespFileEntry.get_text());
    (*m_speaker_list->speaker_list())[index].set_freq_resp_filename(filename);

    on_selection_changed();
    m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
    tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(true);
    m_modified = true;
    GSpeakers::driverlist_modified() = true;
  }
}

void Speaker_ListStore::create_model() {
  m_refListStore = Gtk::ListStore::create(m_columns);

  for_each(m_speaker_list->speaker_list()->begin(), m_speaker_list->speaker_list()->end(),
           mem_fun(*this, &Speaker_ListStore::liststore_add_item));
}

void Speaker_ListStore::liststore_add_item(Speaker spk) {
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

void Speaker_ListStore::add_columns() {
  /* Do not show id column
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
                                mem_fun(*this, &Speaker_ListStore::type_cell_data_func));
    // pColumn->add_attribute(pRenderer->property_text(), m_columns.type);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

    int cols_count = m_TreeView.append_column(_("Qts"), *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

    pColumn->set_cell_data_func(*pRenderer, mem_fun(*this, &Speaker_ListStore::qts_cell_data_func));
    // pColumn->add_attribute(pRenderer->property_text(), m_columns.qts);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

    int cols_count = m_TreeView.append_column(_("Fs"), *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

    pColumn->set_cell_data_func(*pRenderer, mem_fun(*this, &Speaker_ListStore::fs_cell_data_func));
    // pColumn->add_attribute(pRenderer->property_text(), m_columns.fs);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

    int cols_count = m_TreeView.append_column(_("Vas"), *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

    pColumn->set_cell_data_func(*pRenderer, mem_fun(*this, &Speaker_ListStore::vas_cell_data_func));
    // pColumn->add_attribute(pRenderer->property_text(), m_columns.vas);
  }

  //   {
  //     Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

  //     int cols_count = m_TreeView.append_column(_("Rdc"), *pRenderer);
  //     Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

  // 		pColumn->set_cell_data_func(*pRenderer, mem_fun(*this,
  // &Speaker_ListStore::rdc_cell_data_func));
  //     //pColumn->add_attribute(pRenderer->property_text(), m_columns.rdc);
  //   }
  //   {
  //     Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

  //     int cols_count = m_TreeView.append_column(_("Lvc"), *pRenderer);
  //     Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

  // 		pColumn->set_cell_data_func(*pRenderer, mem_fun(*this,
  // &Speaker_ListStore::lvc_cell_data_func));
  //     //pColumn->add_attribute(pRenderer->property_text(), m_columns.lvc);
  //   }
  //   {
  //     Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

  //     int cols_count = m_TreeView.append_column(_("Qms"), *pRenderer);
  //     Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

  // 		pColumn->set_cell_data_func(*pRenderer, mem_fun(*this,
  // &Speaker_ListStore::qms_cell_data_func));
  //     //pColumn->add_attribute(pRenderer->property_text(), m_columns.qms);
  //   }
  //   {
  //     Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

  //     int cols_count = m_TreeView.append_column(_("Qes"), *pRenderer);
  //     Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

  // 		pColumn->set_cell_data_func(*pRenderer, mem_fun(*this,
  // &Speaker_ListStore::qes_cell_data_func));
  //     //pColumn->add_attribute(pRenderer->property_text(), m_columns.qes);
  //   }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

    int cols_count = m_TreeView.append_column(_("Impedance"), *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

    pColumn->set_cell_data_func(*pRenderer, mem_fun(*this, &Speaker_ListStore::imp_cell_data_func));
    // pColumn->add_attribute(pRenderer->property_text(), m_columns.imp);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

    int cols_count = m_TreeView.append_column(_("Sensitivity"), *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);

    pColumn->set_cell_data_func(*pRenderer,
                                mem_fun(*this, &Speaker_ListStore::sens_cell_data_func));
    // pColumn->add_attribute(pRenderer->property_text(), m_columns.sens);
  }
  //   {
  //     Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

  //     int cols_count = m_TreeView.append_column(_("Cone mass"), *pRenderer);
  //     Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

  // 		pColumn->set_cell_data_func(*pRenderer, mem_fun(*this,
  // &Speaker_ListStore::mmd_cell_data_func));
  //     //pColumn->add_attribute(pRenderer->property_text(), m_columns.mmd);
  //   }
  //   {
  //     Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

  //     int cols_count = m_TreeView.append_column(_("Active radius"), *pRenderer);
  //     Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

  // 		pColumn->set_cell_data_func(*pRenderer, mem_fun(*this,
  // &Speaker_ListStore::ad_cell_data_func));
  //     //pColumn->add_attribute(pRenderer->property_text(), m_columns.ad);
  //   }
  //   {
  //     Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

  //     int cols_count = m_TreeView.append_column(_("Force factor"), *pRenderer);
  //     Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

  // 		pColumn->set_cell_data_func(*pRenderer, mem_fun(*this,
  // &Speaker_ListStore::bl_cell_data_func));
  //     //pColumn->add_attribute(pRenderer->property_text(), m_columns.bl);
  //   }
  //   {
  //     Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

  //     int cols_count = m_TreeView.append_column(_("Susp. resistance"), *pRenderer);
  //     Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

  // 		pColumn->set_cell_data_func(*pRenderer, mem_fun(*this,
  // &Speaker_ListStore::rms_cell_data_func));
  //     //pColumn->add_attribute(pRenderer->property_text(), m_columns.rms);
  //   }
  //   {
  //     Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

  //     int cols_count = m_TreeView.append_column(_("Susp. compliance"), *pRenderer);
  //     Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

  // 		pColumn->set_cell_data_func(*pRenderer, mem_fun(*this,
  // &Speaker_ListStore::cms_cell_data_func));
  //     //pColumn->add_attribute(pRenderer->property_text(), m_columns.cms);
  //   }
  //   {
  //     Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

  //     int cols_count = m_TreeView.append_column(_("Freq resp file"), *pRenderer);
  //     Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

  //     pColumn->add_attribute(pRenderer->property_text(), m_columns.freq_resp_file);
  //   }
}

void Speaker_ListStore::type_cell_data_func(Gtk::CellRenderer* cell,
                                            const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  std::string s;
  if (((*iter)[m_columns.type] & SPEAKER_TYPE_BASS) != 0) {
    s = _("Woofer");
  }
  if (((*iter)[m_columns.type] & SPEAKER_TYPE_MIDRANGE) != 0) {
    if (!s.empty()) {
      s = s + _(", Midrange");
    } else {
      s = _("Midrange");
    }
  }
  if (((*iter)[m_columns.type] & SPEAKER_TYPE_TWEETER) != 0) {
    if (!s.empty()) {
      s = s + _(", Tweeter");
    } else {
      s = _("Tweeter");
    }
  }
  if (s.empty()) {
    s = _("Unknown");
  }
  renderer.property_text() = s;
}

void Speaker_ListStore::qts_cell_data_func(Gtk::CellRenderer* cell,
                                           const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.qts], 3, 3);
  renderer.property_xalign() = 1.0;
}

void Speaker_ListStore::fs_cell_data_func(Gtk::CellRenderer* cell,
                                          const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.fs], 3, 0) + " Hz";
  renderer.property_xalign() = 1.0;
}

void Speaker_ListStore::vas_cell_data_func(Gtk::CellRenderer* cell,
                                           const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.vas], 3, 3) + " l";
  renderer.property_xalign() = 1.0;
}

void Speaker_ListStore::rdc_cell_data_func(Gtk::CellRenderer* cell,
                                           const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.rdc], 3, 1) + " Ohm";
  renderer.property_xalign() = 1.0;
}

void Speaker_ListStore::lvc_cell_data_func(Gtk::CellRenderer* cell,
                                           const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.lvc], 3, 2) + " mH";
  renderer.property_xalign() = 1.0;
}

void Speaker_ListStore::qms_cell_data_func(Gtk::CellRenderer* cell,
                                           const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.qms], 3, 3);
  renderer.property_xalign() = 1.0;
}

void Speaker_ListStore::qes_cell_data_func(Gtk::CellRenderer* cell,
                                           const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.qes], 3, 2);
  renderer.property_xalign() = 1.0;
}

void Speaker_ListStore::imp_cell_data_func(Gtk::CellRenderer* cell,
                                           const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.imp], 3, 1) + " Ohm";
  renderer.property_xalign() = 1.0;
}

void Speaker_ListStore::sens_cell_data_func(Gtk::CellRenderer* cell,
                                            const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.sens], 3, 1) + " dB";
  renderer.property_xalign() = 1.0;
}

void Speaker_ListStore::mmd_cell_data_func(Gtk::CellRenderer* cell,
                                           const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  renderer.property_text() =
      GSpeakers::double_to_ustring((*iter)[m_columns.mmd] * 1000, 3, 2) + " g";
  renderer.property_xalign() = 1.0;
}

void Speaker_ListStore::ad_cell_data_func(Gtk::CellRenderer* cell,
                                          const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.ad], 3, 3) + " m";
  renderer.property_xalign() = 1.0;
}

void Speaker_ListStore::bl_cell_data_func(Gtk::CellRenderer* cell,
                                          const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.bl], 3, 1) + " N/A";
  renderer.property_xalign() = 1.0;
}

void Speaker_ListStore::rms_cell_data_func(Gtk::CellRenderer* cell,
                                           const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.rms], 3, 2) + " Ns/m";
  renderer.property_xalign() = 1.0;
}

void Speaker_ListStore::cms_cell_data_func(Gtk::CellRenderer* cell,
                                           const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.cms], 3, 4) + " m/N";
  renderer.property_xalign() = 1.0;
}

void Speaker_ListStore::on_cell_fixed_toggled(const Glib::ustring& path_string) {
  // TODO: Why doesn't the path(strng) constructor work?
  // Using it causes the new text to be put always in the first row.
  // Gtk::TreePath path(path_string);
  GtkTreePath* gpath = gtk_tree_path_new_from_string(path_string.c_str());
  Gtk::TreePath path(gpath);

  /* get toggled iter */
  Gtk::TreeRow row = *(m_refListStore->get_iter(path));

  bool fixed = row[m_columns.id] != 0;

  /* do something with the value */
  fixed = !fixed;

  /* set new value */
  row[m_columns.id] = static_cast<const int>(fixed);
}
