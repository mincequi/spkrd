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

BoxHistory::BoxHistory() : Gtk::Frame("") {
  bool boxlist_found = false;
  set_border_width(2);
  set_shadow_type(Gtk::SHADOW_NONE);
  static_cast<Gtk::Label*>(get_label_widget())
      ->set_markup("<b>" + Glib::ustring(_("Enclosure list")) + "</b>");

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

  try {
    m_box_list = BoxList(m_filename);
    boxlist_found = true;
  } catch (std::runtime_error const& e) {
    std::cout << "BoxHistory::BoxHistory: " << e.what() << std::endl;
  }
  std::cout << "boxlist_found = " << boxlist_found << std::endl;
  static_cast<Gtk::Label*>(get_label_widget())
      ->set_markup("<b>" + Glib::ustring(_("Enclosure list [")) +
                   GSpeakers::short_filename(m_filename) + "]</b>");
  create_model();

  /* create tree view */
  m_TreeView.set_model(m_refListStore);
  m_TreeView.set_rules_hint();

  // m_TreeView.set_search_column(m_columns.id.index());

  signal_add_to_boxlist.connect(sigc::mem_fun(*this, &BoxHistory::on_add_to_boxlist));
  signal_box_modified.connect(sigc::mem_fun(*this, &BoxHistory::on_box_modified));

  add_columns();
  m_ScrolledWindow.add(m_TreeView);
  f_append = nullptr;
  f_open = nullptr;
  f_save_as = nullptr;
  show_all();
  index = 0;

  Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();
  // selection->set_mode(Gtk::SELECTION_MULTIPLE);
  selection->signal_changed().connect(sigc::mem_fun(*this, &BoxHistory::on_selection_changed));

  if (boxlist_found) {
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

void BoxHistory::on_save_open_files() {
  if (GSpeakers::enclosurelist_modified()) {
    on_save();
  }
}

bool BoxHistory::on_delete_event(GdkEventAny* event) {
  /* handle this since we don't want to close the window */
  g_settings.setValue("BoxListXml", m_filename);
#ifdef OUTPUT_DEBUG
  std::cout << "BoxHistory: on_delete_event" << std::endl;
#endif
  return true;
}

void BoxHistory::on_open_xml() {
  GSpeakersFileChooserDialog* fc = new GSpeakersFileChooserDialog(_("Open box xml"));
  std::string filename = fc->get_filename();
  if (filename.length() != 0u) {
    open_xml(filename);
  }
}

void BoxHistory::on_append_xml() {
  GSpeakersFileChooserDialog* fc = new GSpeakersFileChooserDialog(_("Append box xml"));
  std::string const& filename = fc->get_filename();
  if (filename.length() > 0) {
    append_xml(filename);
  }
}

void BoxHistory::open_xml(const std::string& filename) {
  BoxList temp_box_list;
  try {
    temp_box_list = BoxList(filename);
    m_refListStore->clear();

    m_filename = filename;
    for_each(temp_box_list.box_list().begin(), temp_box_list.box_list().end(),
             sigc::mem_fun(*this, &BoxHistory::liststore_add_item));

    /* Delete items in box_list */
    m_box_list.box_list().erase(m_box_list.box_list().begin(), m_box_list.box_list().end());

    for (auto& from : temp_box_list.box_list()) {
      m_box_list.box_list().push_back(from);
    }

    /* Select the first item in the list */
    if (!m_box_list.box_list().empty()) {
      Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

      GtkTreePath* gpath = gtk_tree_path_new_from_string(GSpeakers::int_to_ustring(0).c_str());

      Gtk::TreePath path(gpath);

      Gtk::TreeRow row = *(m_refListStore->get_iter(path));

      refSelection->select(row);
    }
    signal_enclosure_set_save_state(false);
    static_cast<Gtk::Label*>(get_label_widget())
        ->set_markup("<b>" + Glib::ustring(_("Enclosure list [")) +
                     GSpeakers::short_filename(m_filename) + "]</b>");
    g_settings.setValue("BoxListXml", m_filename);
  } catch (std::runtime_error const& e) {
    Gtk::MessageDialog m(e.what(), false, Gtk::MESSAGE_ERROR);
    m.run();
  }
}

void BoxHistory::append_xml(const std::string& filename) {
  BoxList temp_box_list;
  try {
    temp_box_list = BoxList(filename);
    for_each(temp_box_list.box_list().begin(), temp_box_list.box_list().end(),
             sigc::mem_fun(*this, &BoxHistory::liststore_add_item));
    for (auto& from : temp_box_list.box_list()) {
      m_box_list.box_list().push_back(from);
    }
    m_box_list.box_list().size();
  } catch (std::runtime_error const& e) {
    Gtk::MessageDialog m(e.what(), false, Gtk::MESSAGE_ERROR);
    m.run();
  }
  signal_enclosure_set_save_state(true);
}

void BoxHistory::on_selection_changed() {
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  if (const Gtk::TreeIter iter = refSelection->get_selected()) {
    Gtk::TreePath path = m_refListStore->get_path(iter);

    std::vector<int> const& indices = path.get_indices();
    if (!indices.empty()) {
      index = indices[0];
      signal_box_selected(&(m_box_list.box_list()[indices[0]]));
    }
  }
}

void BoxHistory::on_new_copy() {
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  if (!m_box_list.box_list().empty()) {
    /* Find out which row we selected */

    if (const Gtk::TreeIter iter = refSelection->get_selected()) {
      Gtk::TreePath path = m_refListStore->get_path(iter);

      std::vector<int> const& indices = path.get_indices();
      if (!indices.empty()) {
        /* Here we have the row in indices[0], we want to make a copy of this Crossover
           and put it last in the list */

        /* Here we want a copy of the original Crossover, not a crossover that has the
           same id and so on, as we would get if we used the operator = or something similar,
           Quick and easy solution...use the to_xml function which gets rid of the id */

        xmlNodePtr node = xmlNewDocNode(nullptr, nullptr, (xmlChar*)("parent"), nullptr);
        m_box_list.box_list()[indices[0]].to_xml_node(node);
        Box b = Box(node->children);

        /* Set time of day as this crossovers id_string */
        time_t t;
        time(&t);
        /* convert to nice time format */
        std::string s = std::string(ctime(&t));
        int length = s.length();
        s[length - 1] = '\0';
        b.set_id_string(_("Box: ") + s);

        /* the usual adding of items to the liststore and data-container */
        liststore_add_item(b);
        m_box_list.box_list().push_back(b);
      }
    }
  }
  /* Select the last crossover in the list: the new crossover */
  GtkTreePath* gpath = gtk_tree_path_new_from_string(
      GSpeakers::int_to_ustring(m_box_list.box_list().size() - 1).c_str());
  Gtk::TreePath path(gpath);
  Gtk::TreeRow row = *(m_refListStore->get_iter(path));
  refSelection->select(row);
  signal_enclosure_set_save_state(true);
}

void BoxHistory::on_new() {

  Box b;

  /* Set time of day as this crossovers id_string */
  time_t t;
  time(&t);
  /* convert to nice time format */
  std::string s = std::string(ctime(&t));
  int length = s.length();
  s[length - 1] = '\0';
  b.set_id_string(_("Box: ") + s);

  liststore_add_item(b);
  m_box_list.box_list().push_back(b);

  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  GtkTreePath* gpath = gtk_tree_path_new_from_string(
      GSpeakers::int_to_ustring(m_box_list.box_list().size() - 1).c_str());
  Gtk::TreePath path(gpath);
  Gtk::TreeRow row = *(m_refListStore->get_iter(path));
  refSelection->select(row);
  signal_enclosure_set_save_state(true);
}

void BoxHistory::on_new_xml() {
  m_refListStore->clear();
  m_box_list.clear();
  new_xml_pressed = true;
  on_new();
  signal_enclosure_set_save_state(true);
  static_cast<Gtk::Label*>(get_label_widget())
      ->set_markup("<b>" + Glib::ustring(_("Enclosure list [new file]")) + "</b>");
}

void BoxHistory::on_save() {
  if (new_xml_pressed) {
    on_save_as();
    new_xml_pressed = false;
  } else {
    try {
      m_box_list.to_xml(m_filename);
      signal_enclosure_set_save_state(false);
    } catch (std::runtime_error const& e) {
      Gtk::MessageDialog m(e.what(), false, Gtk::MESSAGE_ERROR);
      m.run();
    }
  }
}

void BoxHistory::on_save_as() {

  GSpeakersFileChooserDialog* fc = new GSpeakersFileChooserDialog(
      _("Save box xml as"), Gtk::FILE_CHOOSER_ACTION_SAVE, m_filename);
  std::string filename = fc->get_filename();
  if (filename.length() > 0) {
    save_as_xml(filename);
  }
}

void BoxHistory::save_as_xml(const std::string& filename) {
  try {
    m_box_list.to_xml(filename);
    m_filename = filename;
    static_cast<Gtk::Label*>(get_label_widget())
        ->set_markup("<b>" + Glib::ustring(_("Enclosure list [")) +
                     GSpeakers::short_filename(m_filename) + "]</b>");
    g_settings.setValue("BoxListXml", m_filename);
    signal_enclosure_set_save_state(false);
  } catch (std::runtime_error const& e) {
    Gtk::MessageDialog m(e.what(), false, Gtk::MESSAGE_ERROR);
    m.run();
  }
}

void BoxHistory::on_remove() {
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  if (const Gtk::TreeIter iter = refSelection->get_selected()) {
    Gtk::TreePath path = m_refListStore->get_path(iter);

    std::vector<int> const& indices = path.get_indices();
    if (!indices.empty()) {
      // Remove item from ListStore:
      m_refListStore->erase(iter);

      if (index < (int)m_box_list.box_list().size())
        m_box_list.box_list().erase(m_box_list.box_list().begin() + index);
    }
  }

  char* str = nullptr;
  GString* buffer = g_string_new(str);
  if (index > 0) {
    g_string_printf(buffer, "%d", index - 1);
  } else {
    g_string_printf(buffer, "%d", 0);
  }
  GtkTreePath* gpath = gtk_tree_path_new_from_string(buffer->str);
  Gtk::TreePath path(gpath);
  Gtk::TreeRow row = *(m_refListStore->get_iter(path));
  refSelection->select(row);
  signal_enclosure_set_save_state(true);
}

void BoxHistory::on_box_modified(Box* b) {
  // std::cout << "BoxHistory: box modified: " << b << std::endl;
  // get the row from selection
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  if (const Gtk::TreeIter iter = refSelection->get_selected()) {
    Gtk::TreePath path = m_refListStore->get_path(iter);

    std::vector<int> indices = path.get_indices();
    if (!indices.empty()) {
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
      m_box_list.box_list()[indices[0]].set_type(b->get_type());
      m_box_list.box_list()[indices[0]].set_id_string(b->get_id_string());
      m_box_list.box_list()[indices[0]].set_speaker(b->get_speaker());
      m_box_list.box_list()[indices[0]].set_vb1(b->get_vb1());
      m_box_list.box_list()[indices[0]].set_fb1(b->get_fb1());
      m_box_list.box_list()[indices[0]].set_vb2(b->get_vb2());
      m_box_list.box_list()[indices[0]].set_fb2(b->get_fb2());
      signal_enclosure_set_save_state(true);
    }
  }
}

void BoxHistory::on_add_to_boxlist(Box* b) {
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();
  liststore_add_item(*b);
  m_box_list.box_list().push_back(*b);
  /* Select the last crossover in the list: the added crossover */
  GtkTreePath* gpath = gtk_tree_path_new_from_string(
      GSpeakers::int_to_ustring(m_box_list.box_list().size() - 1).c_str());
  Gtk::TreePath path(gpath);
  Gtk::TreeRow row = *(m_refListStore->get_iter(path));
  refSelection->select(row);
}

void BoxHistory::on_add_plot(Box* b, Speaker* s) {
  liststore_add_item(*b);
  m_box_list.box_list().push_back(*b);
}

void BoxHistory::create_model() {
  m_refListStore = Gtk::ListStore::create(m_columns);

  std::for_each(m_box_list.box_list().begin(), m_box_list.box_list().end(),
                sigc::mem_fun(*this, &BoxHistory::liststore_add_item));
}

void BoxHistory::add_columns() {
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
    pColumn->set_cell_data_func(*pRenderer, sigc::mem_fun(*this, &BoxHistory::type_cell_data_func));
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

    col_cnt = m_TreeView.append_column(_("Vb1"), *pRenderer);

    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(col_cnt - 1);
    // pColumn->add_attribute(pRenderer->property_text(), m_columns.vb1_str);
    pColumn->set_cell_data_func(*pRenderer, sigc::mem_fun(*this, &BoxHistory::vb1_cell_data_func));
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());

    col_cnt = m_TreeView.append_column(_("Fb1"), *pRenderer);

    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(col_cnt - 1);
    // pColumn->add_attribute(pRenderer->property_text(), m_columns.fb1_str);
    pColumn->set_cell_data_func(*pRenderer, sigc::mem_fun(*this, &BoxHistory::fb1_cell_data_func));
  }
  /* Disable vb2 and fb2 until we use it */
  /*
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column(_("Vb2"), *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.vb2);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column(_("Fb2"), *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.fb2);
  }
  */
}

/* This function will execute when a type cell is shown */
void BoxHistory::type_cell_data_func(Gtk::CellRenderer* cell,
                                     const Gtk::TreeModel::iterator& iter) {
  // std::cout << "BoxHistory::type_cell_data_func" << std::endl;
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  switch ((*iter)[m_columns.type]) {
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

void BoxHistory::vb1_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  /* Ok i write litres with capital 'L', i know it's not standard but if you use arial or whatever
     serif (?) it doesn't look good */
  renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.vb1], 3, 1) + " l";
  renderer.property_xalign() = 1.0;
}

void BoxHistory::fb1_cell_data_func(Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter) {
  auto& renderer = dynamic_cast<Gtk::CellRendererText&>(*cell);
  renderer.property_text() = GSpeakers::double_to_ustring((*iter)[m_columns.fb1], 3, 1) + " Hz";
  renderer.property_xalign() = 1.0;
}

void BoxHistory::liststore_add_item(Box const& box) {
  Gtk::TreeRow row = *(m_refListStore->append());
  row[m_columns.id_string] = box.get_id_string();
  row[m_columns.speaker] = box.get_speaker();
  row[m_columns.type] = box.get_type();
  row[m_columns.vb1] = box.get_vb1();
  row[m_columns.fb1] = box.get_fb1();
  row[m_columns.vb2] = box.get_vb2();
  row[m_columns.fb2] = box.get_fb2();
}
