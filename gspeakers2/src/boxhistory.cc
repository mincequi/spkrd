/*
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

#include <time.h>
#include "boxhistory.h"
#include "../config.h"

#define MENU_INDEX_SAVE           6
#define MENU_INDEX_DELETE         8
#define MENU_INDEX_DELETE_PLOT    9
#define TOOLBAR_INDEX_SAVE        5
#define TOOLBAR_INDEX_DELETE      7
#define TOOLBAR_INDEX_DELETE_PLOT 7

BoxHistory::BoxHistory() :
  Gtk::Frame("Enclosure list"),
  m_Table(10, 4, true), 
  m_NewCopyButton("New copy"), 
  m_NewXmlButton("New Xml"), 
  m_AppendXmlButton("Append xml..."), 
  m_OpenXmlButton("Open..."), 
  m_NewButton("New box"), 
  m_SaveButton("Save"),
  m_SaveAsButton("Save as..."),
  m_RemoveButton("Remove"),
  m_vbox()
{
  //    ("Box history");
  m_vbox.set_border_width(8);
  //set_default_size(250, 300);
  tbar = NULL;
  add(m_vbox);
  m_vbox.pack_start(m_ScrolledWindow);
  //m_vbox.pack_start(m_Table);
  m_Table.set_spacings(4);
  
  m_ScrolledWindow.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  g_settings.settings_changed.connect(slot(*this, &BoxHistory::on_settings_changed));
  //m_Table.attach(m_ScrolledWindow, 0, 4, 0, 10, Gtk::FILL);
  //m_Table.attach(m_NewCopyButton, 0, 1, 8, 9);
  //m_Table.attach(m_NewButton, 1, 2, 8, 9);
  //m_Table.attach(m_NewXmlButton, 2, 3, 8, 9);
  //m_Table.attach(m_RemoveButton, 3, 4, 8, 9);
  //m_Table.attach(m_OpenXmlButton, 0, 1, 9, 10);
  //m_Table.attach(m_AppendXmlButton, 1, 2, 9, 10);
  //m_Table.attach(m_SaveButton, 2, 3, 9, 10);
  //m_Table.attach(m_SaveAsButton, 3, 4, 9, 10);
  
  
  /* Read this from settings later */
  g_settings.defaultValueString("BoxListXml", string(GSPEAKERS_PREFIX) + "/share/xml/box1.xml");
  
  m_filename = g_settings.getValueString("BoxListXml");
  cout << "BoxHistory: " << m_filename << endl;
  m_box_list = BoxList(m_filename); 
  set_label("Enclosure list [" + m_filename + "]");
  
  create_model();

  /* create tree view */
  m_TreeView.set_model(m_refListStore);
  m_TreeView.set_rules_hint();
  
  //signal_delete_event().connect(slot(*this, &BoxHistory::on_close));
  
  //m_TreeView.set_search_column(m_columns.id.index());
  Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();
  //selection->set_mode(Gtk::SELECTION_MULTIPLE);
  selection->signal_changed().connect(slot(*this, &BoxHistory::on_selection_changed));

  m_NewCopyButton.signal_clicked().connect(slot(*this, &BoxHistory::on_new_copy));
  m_NewXmlButton.signal_clicked().connect(slot(*this, &BoxHistory::on_new_xml));
  m_NewButton.signal_clicked().connect(slot(*this, &BoxHistory::on_new));
  m_OpenXmlButton.signal_clicked().connect(slot(*this, &BoxHistory::on_open_xml));
  m_RemoveButton.signal_clicked().connect(slot(*this, &BoxHistory::on_remove));
  m_SaveButton.signal_clicked().connect(slot(*this, &BoxHistory::on_save));
  m_SaveAsButton.signal_clicked().connect(slot(*this, &BoxHistory::on_save_as));
  m_AppendXmlButton.signal_clicked().connect(slot(*this, &BoxHistory::on_append_xml));

  signal_add_to_boxlist.connect(slot(*this, &BoxHistory::on_add_to_boxlist));
  signal_box_modified.connect(slot(*this, &BoxHistory::on_box_modified));
  //signal_add_plot.connect(slot(*this, &BoxHistory::on_add_plot));

  add_columns();
  m_ScrolledWindow.add(m_TreeView);
  f_append = NULL;
  f_open = NULL;
  f_save_as = NULL;
  show_all();
  index = 0;
  m_SaveButton.set_sensitive(false);
  

  char *str = NULL;
  GString *buffer = g_string_new(str);
  g_string_printf(buffer, "%d", 0);
  GtkTreePath *gpath = gtk_tree_path_new_from_string(buffer->str);
  Gtk::TreePath path(gpath);
  Gtk::TreeRow row = *(m_refListStore->get_iter(path));
  selection->select(row);
  selected_plot = -1;
  signal_select_plot.connect(slot(*this, &BoxHistory::on_plot_selected));
}

BoxHistory::~BoxHistory()
{
  g_settings.setValue("BoxListXml", m_filename);
}

Gtk::Menu& BoxHistory::get_menu()
{
  Gtk::Menu::MenuList& menulist = m_menu.items();
  menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("_New Enclosure", GSpeakers::image_widget("stock_new_enclosure_16.png"), 
                      slot(*this, &BoxHistory::on_new) ) );
  menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("New _Copy", GSpeakers::image_widget("stock_new_enclosure_copy_16.png"), 
                      slot(*this, &BoxHistory::on_new_copy) ) );
  menulist.push_back( Gtk::Menu_Helpers::SeparatorElem() );
  menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("N_ew Enclosure Xml", GSpeakers::image_widget("stock_new_enclosure_xml_16.png"), 
                      slot(*this, &BoxHistory::on_new_xml) ) ); 
  menulist.push_back( Gtk::Menu_Helpers::MenuElem("A_ppend Enclosure Xml...", 
                      slot(*this, &BoxHistory::on_append_xml) ) );
  menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("_Open Xml...", GSpeakers::image_widget("open_xml_16.png"),   
                      slot(*this, &BoxHistory::on_open_xml) ) );
  menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("_Save Xml", GSpeakers::image_widget("save_xml_16.png"),   
                      slot(*this, &BoxHistory::on_save) ) );
  menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("Save Xml _As", GSpeakers::image_widget("save_as_xml_16.png"),   
                      slot(*this, &BoxHistory::on_save_as) ) );
  menulist.push_back( Gtk::Menu_Helpers::SeparatorElem() );
  menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("_Delete Enclosure", GSpeakers::image_widget("delete_enclosure_16.png"),  
                      slot(*this, &BoxHistory::on_remove) ) ); 
  //menulist.push_back( Gtk::Menu_Helpers::MenuElem("Delete Selected Plo_t", 
  //                    slot(*this, &BoxHistory::on_delete_plot) ) ); 
  menulist[MENU_INDEX_SAVE].set_sensitive(false);
  //menulist[MENU_INDEX_DELETE].set_sensitive(false);
  return m_menu;
}

Gtk::Widget& BoxHistory::get_toolbar()
{
  if (tbar == NULL) {
    tbar = manage(new Gtk::Toolbar());
    tbar->tools().push_back( Gtk::Toolbar_Helpers::ButtonElem("New Enclosure", GSpeakers::image_widget("stock_new_enclosure_24.png"), 
                             slot(*this, &BoxHistory::on_new), "Create new enclosure") );
    tbar->tools().push_back( Gtk::Toolbar_Helpers::ButtonElem("New Copy", GSpeakers::image_widget("stock_new_enclosure_copy_24.png"), 
                             slot(*this, &BoxHistory::on_new_copy), "Copy currently selected enclosure") );
    tbar->tools().push_back( Gtk::Toolbar_Helpers::Space() );
    tbar->tools().push_back( Gtk::Toolbar_Helpers::ButtonElem("New Xml", GSpeakers::image_widget("stock_new_enclosure_xml_24.png"), 
                             slot(*this, &BoxHistory::on_new_xml), "Create new enclosure xml (list)") );
    tbar->tools().push_back( Gtk::Toolbar_Helpers::ButtonElem("Open Xml",  GSpeakers::image_widget("open_xml_24.png"),
                             slot(*this, &BoxHistory::on_open_xml), "Open enclosure xml (list)") );
    tbar->tools().push_back( Gtk::Toolbar_Helpers::ButtonElem("Save Xml",  GSpeakers::image_widget("save_xml_24.png"),
                             slot(*this, &BoxHistory::on_save), "Save enclosure xml (list)") );
    tbar->tools().push_back( Gtk::Toolbar_Helpers::Space() );
    tbar->tools().push_back( Gtk::Toolbar_Helpers::ButtonElem("Delete",  GSpeakers::image_widget("delete_enclosure_24.png"),   
                             slot(*this, &BoxHistory::on_remove), "Delete selected enclosure") );
  
    toolbar.add(*tbar);
    tbar->set_toolbar_style((Gtk::ToolbarStyle)g_settings.getValueUnsignedInt("ToolbarStyle"));
    tbar->tools()[TOOLBAR_INDEX_SAVE].get_widget()->set_sensitive(false);
    //tbar->tools()[TOOLBAR_INDEX_DELETE].get_widget()->set_sensitive(false);
  }
  return toolbar;
}

void BoxHistory::on_settings_changed(const string& s)
{
  if (s == "ToolbarStyle") {
    tbar->set_toolbar_style((Gtk::ToolbarStyle)g_settings.getValueUnsignedInt("ToolbarStyle"));
  }
}

void BoxHistory::on_delete_plot()
{
  signal_remove_box_plot(selected_plot);
}

void BoxHistory::on_plot_selected(int i)
{
  selected_plot = i;
  m_menu.items()[MENU_INDEX_DELETE_PLOT].set_sensitive(true);
}

bool BoxHistory::on_delete_event(GdkEventAny *event)
{
 /* handle this since we don't want to close the window */
 g_settings.setValue("BoxListXml", m_filename);
 cout << "BoxHistory: on_delete_event" << endl;
 return true;
}

void BoxHistory::on_open_xml()
{
  if (f_open == NULL) {
    f_open = new Gtk::FileSelection("Open box xml");
    f_open->get_ok_button()->signal_clicked().connect(bind<Gtk::FileSelection *>(slot(*this, &BoxHistory::on_open_ok), f_open));
    f_open->get_cancel_button()->signal_clicked().connect(slot(*f_open, &Gtk::Widget::hide));
  } else {
    f_open->show();
  }
  f_open->run();
}

void BoxHistory::on_append_xml()
{
  if (f_append == NULL) {
    f_append = new Gtk::FileSelection("Append box xml");
    f_append->get_ok_button()->signal_clicked().connect(bind<Gtk::FileSelection *>(slot(*this, &BoxHistory::on_append_ok), f_append));
    f_append->get_cancel_button()->signal_clicked().connect(slot(*f_append, &Gtk::Widget::hide));
  } else {
    f_append->show();
  }
  f_append->run();
}

void BoxHistory::on_open_ok(Gtk::FileSelection *f)
{
  m_refListStore->clear();

  BoxList temp_box_list;
  try {
    temp_box_list = BoxList(f->get_filename());

    m_filename = f->get_filename();
    for_each(
      temp_box_list.box_list()->begin(), temp_box_list.box_list()->end(),
      slot(*this, &BoxHistory::liststore_add_item));
  
    /* Delete items in box_list */
    m_box_list.box_list()->erase(m_box_list.box_list()->begin(), m_box_list.box_list()->end());
  
    for (
      vector<Box>::iterator from = temp_box_list.box_list()->begin();
      from != temp_box_list.box_list()->end();
      ++from)
    {
      m_box_list.box_list()->push_back(*from);
    }
    f->hide();
  
    /* Select the first item in the list */
    if (m_box_list.box_list()->size() > 0) {
      Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();
      GtkTreePath *gpath = gtk_tree_path_new_from_string(GSpeakers::int_to_ustring(0).c_str());
      Gtk::TreePath path(gpath);
  
      Gtk::TreeRow row = *(m_refListStore->get_iter(path));
      refSelection->select(row);
  
    }
    m_AppendXmlButton.set_sensitive(true);
    m_SaveButton.set_sensitive(false);
    m_menu.items()[MENU_INDEX_SAVE].set_sensitive(false);
    tbar->tools()[TOOLBAR_INDEX_SAVE].get_widget()->set_sensitive(false);
    m_SaveAsButton.set_sensitive(true);
    m_RemoveButton.set_sensitive(true);
    set_label("Enclosure list [" + m_filename + "]");
    g_settings.setValue("BoxListXml", m_filename);
  } catch (GSpeakersException e) {
    Gtk::MessageDialog m(e.what(), Gtk::MESSAGE_ERROR);
    m.run();
  }
  
}

void BoxHistory::on_append_ok(Gtk::FileSelection *f)
{
  //cout << "append xml ok" << endl;
  BoxList temp_box_list;
  try {
    temp_box_list = BoxList(f->get_filename());

    for_each(
      temp_box_list.box_list()->begin(), temp_box_list.box_list()->end(),
      slot(*this, &BoxHistory::liststore_add_item));
    for (
      vector<Box>::iterator from = temp_box_list.box_list()->begin();
      from != temp_box_list.box_list()->end();
      ++from)
    {
      m_box_list.box_list()->push_back(*from);
    }
    f->hide();
    m_box_list.box_list()->size();
  } catch (GSpeakersException e) {
    Gtk::MessageDialog m(e.what(), Gtk::MESSAGE_ERROR);
    m.run();
  }
  m_SaveButton.set_sensitive(true);
  m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
  tbar->tools()[TOOLBAR_INDEX_SAVE].get_widget()->set_sensitive(true);

}

void BoxHistory::on_selection_changed()
{
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  if(const Gtk::TreeIter iter = refSelection->get_selected())
  {
    Gtk::TreePath path = m_refListStore->get_path(iter);

    std::vector<int> indices = path.get_indices();
    if(indices.size() > 0)
    {
      index = indices[0];
      //cout << "selection changed" << endl;
      signal_box_selected(&((*m_box_list.box_list())[indices[0]]));
    }
  } 
}

void BoxHistory::on_new_copy()
{
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  if (m_box_list.box_list()->size() > 0) {
    /* Find out which row we selected */
  
    if(const Gtk::TreeIter iter = refSelection->get_selected())
    {
      Gtk::TreePath path = m_refListStore->get_path(iter);
  
      std::vector<int> indices = path.get_indices();
      if(indices.size() > 0)
      {
        /* Here we have the row in indices[0], we want to make a copy of this Crossover 
           and put it last in the list */
           
        /* Here we want a copy of the original Crossover, not a crossover that has the 
           same id and so on, as we would get if we used the operator = or something similar, 
           Quick and easy solution...use the to_xml function which gets rid of the id */
        
        xmlNodePtr node = xmlNewDocNode( NULL, NULL, (xmlChar *)("parent"), NULL );
        ((*m_box_list.box_list())[indices[0]]).to_xml_node(node);
        Box b = Box(node->children);
        
        /* Set time of day as this crossovers id_string */
        time_t t;
        time(&t);
        /* convert to nice time format */
        string s = string(ctime(&t));
        int length = s.length();
        s[length-1] = '\0';
        b.set_id_string("Box: " + s);
        
        /* the usual adding of items to the liststore and data-container */
        liststore_add_item(b);
        m_box_list.box_list()->push_back(b);
      }
    } 
  } 
  /* Select the last crossover in the list: the new crossover */ 
  //char *str = NULL;
  //GString *buffer = g_string_new(str);
  //g_string_printf(buffer, "%d", m_box_list.box_list()->size() - 1);
  GtkTreePath *gpath = gtk_tree_path_new_from_string(GSpeakers::int_to_ustring(m_box_list.box_list()->size() - 1).c_str());
  Gtk::TreePath path(gpath);
  Gtk::TreeRow row = *(m_refListStore->get_iter(path));
  refSelection->select(row);
  m_SaveButton.set_sensitive(true);
  m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
  tbar->tools()[TOOLBAR_INDEX_SAVE].get_widget()->set_sensitive(true);
}

void BoxHistory::on_new()
{
  Box b;
  
  /* Set time of day as this crossovers id_string */
  time_t t;
  time(&t);
  /* convert to nice time format */
  string s = string(ctime(&t));
  int length = s.length();
  s[length-1] = '\0';
  b.set_id_string("Box: " + s);
  
  
  liststore_add_item(b);
  m_box_list.box_list()->push_back(b);
  
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();
 
  //char *str = NULL;
  //GString *buffer = g_string_new(str);
  //g_string_printf(buffer, "%d", m_box_list.box_list()->size() - 1);
  GtkTreePath *gpath = gtk_tree_path_new_from_string(GSpeakers::int_to_ustring(m_box_list.box_list()->size() - 1).c_str());
  //GtkTreePath *gpath = gtk_tree_path_new_from_string(buffer->str);
  Gtk::TreePath path(gpath);
  Gtk::TreeRow row = *(m_refListStore->get_iter(path));
  refSelection->select(row);
  m_SaveButton.set_sensitive(true);
  m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
  tbar->tools()[TOOLBAR_INDEX_SAVE].get_widget()->set_sensitive(true);
}

void BoxHistory::on_new_xml()
{
  m_refListStore->clear();
  m_box_list.clear();
  new_xml_pressed = true;
  on_new();
  m_SaveButton.set_sensitive(true);
  m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
  tbar->tools()[TOOLBAR_INDEX_SAVE].get_widget()->set_sensitive(true);
  set_label("Enclosure list [new file]");
}

void BoxHistory::on_save()
{
  //cout << "save" << endl;
  if (new_xml_pressed == true) {
    on_save_as();
    new_xml_pressed = false;
  } else {
    try {
      m_box_list.to_xml(m_filename);
      m_SaveButton.set_sensitive(false);
      m_menu.items()[MENU_INDEX_SAVE].set_sensitive(false);
      tbar->tools()[TOOLBAR_INDEX_SAVE].get_widget()->set_sensitive(false);
    } catch (GSpeakersException e) {
      Gtk::MessageDialog m(e.what(), Gtk::MESSAGE_ERROR);
      m.run();
    }
  }
}

void BoxHistory::on_save_as()
{
  //cout << "save as" << endl;
  if (f_save_as == NULL) {
    f_save_as = new Gtk::FileSelection("Save box xml as");
    f_save_as->get_ok_button()->signal_clicked().connect(bind<Gtk::FileSelection *>(slot(*this, &BoxHistory::on_save_as_ok), f_save_as));
    f_save_as->get_cancel_button()->signal_clicked().connect(slot(*f_save_as, &Gtk::Widget::hide));
  } else {
    f_save_as->show();
  }
  f_save_as->run();
  
}

void BoxHistory::on_save_as_ok(Gtk::FileSelection *f)
{
  //cout << "save as ok" << endl;
  try {
    m_box_list.to_xml(f->get_filename());
    f->hide();
    m_filename = f->get_filename();
    set_label("Enclosure list [" + m_filename + "]");
    g_settings.setValue("BoxListXml", m_filename);
    m_SaveButton.set_sensitive(false);
    m_menu.items()[MENU_INDEX_SAVE].set_sensitive(false);
    tbar->tools()[TOOLBAR_INDEX_SAVE].get_widget()->set_sensitive(false);
  } catch (GSpeakersException e) {
      Gtk::MessageDialog m(e.what(), Gtk::MESSAGE_ERROR);
      m.run();
  }
}

void BoxHistory::on_remove()
{
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  if(const Gtk::TreeIter iter = refSelection->get_selected())
  {
    Gtk::TreePath path = m_refListStore->get_path(iter);

    std::vector<int> indices = path.get_indices();
    if(indices.size() > 0)
    {
      //Remove item from ListStore:
      m_refListStore->erase(iter);

      if(index < (int)m_box_list.box_list()->size())
        m_box_list.box_list()->erase(m_box_list.box_list()->begin() + index);
    }
  }

  char *str = NULL;
  GString *buffer = g_string_new(str);
  if (index > 0) {
    g_string_printf(buffer, "%d", index - 1);
  } else {
    g_string_printf(buffer, "%d", 0);
  }
  GtkTreePath *gpath = gtk_tree_path_new_from_string(buffer->str);
  Gtk::TreePath path(gpath);
  Gtk::TreeRow row = *(m_refListStore->get_iter(path));
  refSelection->select(row);
  m_SaveButton.set_sensitive(true);
  m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
  tbar->tools()[TOOLBAR_INDEX_SAVE].get_widget()->set_sensitive(true);
}

void BoxHistory::on_box_modified(Box *b)
{
  cout << "BoxHistory: box modified" << endl;
  /* Modify fields in treeview here */
  // get the row from selection
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  if(const Gtk::TreeIter iter = refSelection->get_selected())
  {
    Gtk::TreePath path = m_refListStore->get_path(iter);

    std::vector<int> indices = path.get_indices();
    if(indices.size() > 0)
    {
      Gtk::TreeRow row = *(m_refListStore->get_iter(path));
      /* Update the liststore */
      row[m_columns.type]      = b->get_type();
      row[m_columns.id_string] = b->get_id_string();
      row[m_columns.vb1]       = b->get_vb1();
      row[m_columns.fb1]       = b->get_fb1();
      row[m_columns.vb2]       = b->get_vb2();
      row[m_columns.fb2]       = b->get_fb2();
      /* Update the boxlist */
      (*m_box_list.box_list())[indices[0]].set_type(b->get_type());
      (*m_box_list.box_list())[indices[0]].set_id_string(b->get_id_string());
      (*m_box_list.box_list())[indices[0]].set_vb1(b->get_vb1());
      (*m_box_list.box_list())[indices[0]].set_fb1(b->get_fb1());
      (*m_box_list.box_list())[indices[0]].set_vb2(b->get_vb2());
      (*m_box_list.box_list())[indices[0]].set_fb2(b->get_fb2());
      m_SaveButton.set_sensitive(true);
      m_menu.items()[MENU_INDEX_SAVE].set_sensitive(true);
      tbar->tools()[TOOLBAR_INDEX_SAVE].get_widget()->set_sensitive(true);
      
    }
  }
  
  
  /* Modify box in box_list */
  // Find out index of selection == index in box-list to change
}

void BoxHistory::on_add_to_boxlist(Box *b)
{
  liststore_add_item(*b);
  m_box_list.box_list()->push_back(*b);
}

void BoxHistory::on_add_plot(Box *b, Speaker *s) 
{
  liststore_add_item(*b);
  m_box_list.box_list()->push_back(*b);
}

void BoxHistory::create_model()
{
  m_refListStore = Gtk::ListStore::create(m_columns);
  
  //add_items(m_box_list);

  for_each(
      m_box_list.box_list()->begin(), m_box_list.box_list()->end(),
      slot(*this, &BoxHistory::liststore_add_item));

}

void BoxHistory::add_columns()
{
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count =m_TreeView.append_column("Identifier", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.id_string);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Type", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.type);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Vb1", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.vb1);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Fb1", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.fb1);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Vb2", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.vb2);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Fb2", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.fb2);
  }

}

void BoxHistory::liststore_add_item(Box box)
{
  
  Gtk::TreeRow row = *(m_refListStore->append());
  row[m_columns.id_string]  = box.get_id_string();
  row[m_columns.type]       = box.get_type();
  row[m_columns.vb1]        = box.get_vb1();
  row[m_columns.fb1]        = box.get_fb1();
  row[m_columns.vb2]        = box.get_vb2();
  row[m_columns.fb2]        = box.get_fb2();
}
