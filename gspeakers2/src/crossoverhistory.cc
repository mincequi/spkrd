/*
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

#include <time.h>
#include "crossoverhistory.h"
#include "../config.h"

CrossoverHistory::CrossoverHistory() :
  Gtk::Frame("Crossover list"),
  m_Table(10, 4, true), 
  m_NewCopyButton("New copy"), 
  m_NewXmlButton("New Xml"), 
  m_AppendXmlButton("Append xml..."), 
  m_OpenXmlButton("Open..."), 
  m_NewButton("New crossover"), 
  m_SaveButton("Save"),
  m_SaveAsButton("Save as..."),
  m_RemoveButton("Remove")
{
  
//  set_title("Crossover history");
  set_border_width(8);
//  set_default_size(250, 300);
  
  add(m_Table);
  m_Table.set_spacings(4);
  
  m_ScrolledWindow.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  
  m_Table.attach(m_ScrolledWindow, 0, 4, 0, 8, Gtk::FILL);
  
  m_Table.attach(m_NewCopyButton, 0, 1, 8, 9);
  m_Table.attach(m_NewButton, 1, 2, 8, 9);
  m_Table.attach(m_NewXmlButton, 2, 3, 8, 9);
  m_Table.attach(m_RemoveButton, 3, 4, 8, 9);
  m_Table.attach(m_OpenXmlButton, 0, 1, 9, 10);
  m_Table.attach(m_AppendXmlButton, 1, 2, 9, 10);
  m_Table.attach(m_SaveButton, 2, 3, 9, 10);
  m_Table.attach(m_SaveAsButton, 3, 4, 9, 10);
  
  
  /* Read this from settings later */
  g_settings.defaultValueString("CrossoverListXml", string(GSPEAKERS_PREFIX) + "/share/xml/crossover1.xml");
  m_filename = g_settings.getValueString("CrossoverListXml");
  
  m_crossover_list = CrossoverList(m_filename); 
  set_label("Crossover list [" + m_filename + "]");
  
  create_model();

  /* create tree view */
  m_TreeView.set_model(m_refListStore);
  m_TreeView.set_rules_hint();
  
  signal_delete_event().connect(slot(*this, &CrossoverHistory::on_close));
  
  //m_TreeView.set_search_column(m_columns.id.index());
  Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();
  //selection->set_mode(Gtk::SELECTION_MULTIPLE);
  selection->signal_changed().connect(slot(*this, &CrossoverHistory::on_selection_changed));

  m_NewCopyButton.signal_clicked().connect(slot(*this, &CrossoverHistory::on_new_copy));
  m_NewXmlButton.signal_clicked().connect(slot(*this, &CrossoverHistory::on_new_xml));
  m_NewButton.signal_clicked().connect(slot(*this, &CrossoverHistory::on_new));
  m_OpenXmlButton.signal_clicked().connect(slot(*this, &CrossoverHistory::on_open_xml));
  m_RemoveButton.signal_clicked().connect(slot(*this, &CrossoverHistory::on_remove));
  m_SaveButton.signal_clicked().connect(slot(*this, &CrossoverHistory::on_save));
  m_SaveAsButton.signal_clicked().connect(slot(*this, &CrossoverHistory::on_save_as));
  m_AppendXmlButton.signal_clicked().connect(slot(*this, &CrossoverHistory::on_append_xml));


  //signal_part_modified.connect(slot(*this, &CrossoverHistory::on_part_modified));

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
  signal_new_crossover.connect(slot(*this, &CrossoverHistory::on_new_from_menu));
}

bool CrossoverHistory::on_delete_event(GdkEventAny *event)
{
  /* don't wanna close this window */
  return true;
}

void CrossoverHistory::on_part_modified() {
  m_SaveButton.set_sensitive();
}

CrossoverHistory::~CrossoverHistory()
{
  g_settings.setValue("CrossoverListXml", m_filename);
  g_settings.save();
}

void CrossoverHistory::on_open_xml()
{
  if (f_open == NULL) {
    f_open = new Gtk::FileSelection("Open crossover xml");
    f_open->get_ok_button()->signal_clicked().connect(bind<Gtk::FileSelection *>(slot(*this, &CrossoverHistory::on_open_ok), f_open));
    f_open->get_cancel_button()->signal_clicked().connect(slot(*f_open, &Gtk::Widget::hide));
  } else {
    f_open->show();
  }
  f_open->run();
}

void CrossoverHistory::on_append_xml()
{
  if (f_append == NULL) {
    f_append = new Gtk::FileSelection("Append crossover xml");
    f_append->get_ok_button()->signal_clicked().connect(bind<Gtk::FileSelection *>(slot(*this, &CrossoverHistory::on_append_ok), f_append));
    f_append->get_cancel_button()->signal_clicked().connect(slot(*f_append, &Gtk::Widget::hide));
  } else {
    f_append->show();
  }
  f_append->run();
}

void CrossoverHistory::on_open_ok(Gtk::FileSelection *f)
{
  m_refListStore->clear();

  CrossoverList temp_crossover_list;
  try {
    temp_crossover_list = CrossoverList(f->get_filename());

    m_filename = f->get_filename();
    for_each(
      temp_crossover_list.crossover_list()->begin(), temp_crossover_list.crossover_list()->end(),
      slot(*this, &CrossoverHistory::liststore_add_item));
  
    /* Delete items in crossover_list */
    m_crossover_list.crossover_list()->erase(m_crossover_list.crossover_list()->begin(), m_crossover_list.crossover_list()->end());
  
    for (
      vector<Crossover>::iterator from = temp_crossover_list.crossover_list()->begin();
      from != temp_crossover_list.crossover_list()->end();
      ++from)
    {
      m_crossover_list.crossover_list()->push_back(*from);
    }
    f->hide();
  
    /* Select the first item in the list */
    if (m_crossover_list.crossover_list()->size() > 0) {
      Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();
      char *str = NULL;
      GString *buffer = g_string_new(str);
      g_string_printf(buffer, "%d", 0);
      GtkTreePath *gpath = gtk_tree_path_new_from_string(buffer->str);
      Gtk::TreePath path(gpath);
  
      Gtk::TreeRow row = *(m_refListStore->get_iter(path));
      refSelection->select(row);
  
    }
    m_AppendXmlButton.set_sensitive(true);
    m_SaveButton.set_sensitive(false);
    m_SaveAsButton.set_sensitive(true);
    m_RemoveButton.set_sensitive(true);
    set_label("Crossover list [" + m_filename + "]");
  } catch (GSpeakersException e) {
    Gtk::MessageDialog m(e.what(), Gtk::MESSAGE_ERROR);
    m.run();
  }
  
}

void CrossoverHistory::on_append_ok(Gtk::FileSelection *f)
{
  cout << "append xml ok" << endl;
  CrossoverList temp_crossover_list;
  try {
    temp_crossover_list = CrossoverList(f->get_filename());

    for_each(
      temp_crossover_list.crossover_list()->begin(), temp_crossover_list.crossover_list()->end(),
      slot(*this, &CrossoverHistory::liststore_add_item));
    for (
      vector<Crossover>::iterator from = temp_crossover_list.crossover_list()->begin();
      from != temp_crossover_list.crossover_list()->end();
      ++from)
    {
      m_crossover_list.crossover_list()->push_back(*from);
    }
    f->hide();
    m_crossover_list.crossover_list()->size();
  } catch (GSpeakersException e) {
    Gtk::MessageDialog m(e.what(), Gtk::MESSAGE_ERROR);
    m.run();
  }
  m_SaveButton.set_sensitive(true);
}

void CrossoverHistory::on_selection_changed()
{
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  if(const Gtk::TreeIter iter = refSelection->get_selected())
  {
    Gtk::TreePath path = m_refListStore->get_path(iter);

    std::vector<int> indices = path.get_indices();
    if(indices.size() > 0)
    {
      index = indices[0];
      signal_crossover_selected(&((*m_crossover_list.crossover_list())[indices[0]]));
    }
  } 
}

void CrossoverHistory::on_new_copy()
{
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  if (m_crossover_list.crossover_list()->size() > 0) {
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
        ((*m_crossover_list.crossover_list())[indices[0]]).to_xml_node(node);
        Crossover c = Crossover(node->children);
        
        /* Set time of day as this crossovers id_string */
        time_t t;
        time(&t);
        /* convert to nice time format */
        string s = string(ctime(&t));
        int length = s.length();
        s[length-1] = '\0';
        c.set_id_string("Crossover: " + s);
        
        /* the usual adding of items to the liststore and data-container */
        liststore_add_item(c);
        m_crossover_list.crossover_list()->push_back(c);
      }
    } 
  } 
  /* Select the last crossover in the list: the new crossover */ 
  char *str = NULL;
  GString *buffer = g_string_new(str);
  g_string_printf(buffer, "%d", m_crossover_list.crossover_list()->size() - 1);
  GtkTreePath *gpath = gtk_tree_path_new_from_string(buffer->str);
  Gtk::TreePath path(gpath);
  Gtk::TreeRow row = *(m_refListStore->get_iter(path));
  refSelection->select(row);
  m_SaveButton.set_sensitive(true);
}

void CrossoverHistory::on_new_from_menu(int type)
{
  cout << "CrossoverHistory::on_new_from_menu: " << type << endl;
  /* add new crossover of appropriate type here */
  time_t t;
  time(&t);
  /* convert to nice time format */
  string s = string(ctime(&t));
  int length = s.length();
  s[length-1] = '\0';
  
  Crossover c(type, "Crossover " + s);

  /* Add to liststore */
  liststore_add_item(c);
  m_crossover_list.crossover_list()->push_back(c);
  
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();
 
  /* make our new crossover the selected crossover */
  char *str = NULL;
  GString *buffer = g_string_new(str);
  g_string_printf(buffer, "%d", m_crossover_list.crossover_list()->size() - 1);
  GtkTreePath *gpath = gtk_tree_path_new_from_string(buffer->str);
  Gtk::TreePath path(gpath);
  Gtk::TreeRow row = *(m_refListStore->get_iter(path));
  refSelection->select(row);
  m_SaveButton.set_sensitive(true);

}

void CrossoverHistory::on_new()
{
  Crossover c;
  
  /* Set time of day as this crossovers id_string */
  time_t t;
  time(&t);
  /* convert to nice time format */
  string s = string(ctime(&t));
  int length = s.length();
  s[length-1] = '\0';
  c.set_id_string("Crossover: " + s);
  
  
  liststore_add_item(c);
  m_crossover_list.crossover_list()->push_back(c);
  
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();
 
  char *str = NULL;
  GString *buffer = g_string_new(str);
  g_string_printf(buffer, "%d", m_crossover_list.crossover_list()->size() - 1);
  GtkTreePath *gpath = gtk_tree_path_new_from_string(buffer->str);
  Gtk::TreePath path(gpath);
  Gtk::TreeRow row = *(m_refListStore->get_iter(path));
  refSelection->select(row);
  m_SaveButton.set_sensitive(true);
}

void CrossoverHistory::on_new_xml()
{
  m_refListStore->clear();
  m_crossover_list.clear();
  new_xml_pressed = true;
  on_new();
  m_SaveButton.set_sensitive(true);
  set_label("Crossover list [new file]");
}

void CrossoverHistory::on_save()
{
  cout << "save" << endl;
  if (new_xml_pressed == true) {
    on_save_as();
    new_xml_pressed = false;
  } else {
    try {
      m_crossover_list.to_xml(m_filename);
      m_SaveButton.set_sensitive(false);
    } catch (GSpeakersException e) {
      Gtk::MessageDialog m(e.what(), Gtk::MESSAGE_ERROR);
      m.run();
    }
  }
}

void CrossoverHistory::on_save_as()
{
  cout << "save as" << endl;
  if (f_save_as == NULL) {
    f_save_as = new Gtk::FileSelection("Save crossover xml as");
    f_save_as->get_ok_button()->signal_clicked().connect(bind<Gtk::FileSelection *>(slot(*this, &CrossoverHistory::on_save_as_ok), f_save_as));
    f_save_as->get_cancel_button()->signal_clicked().connect(slot(*f_save_as, &Gtk::Widget::hide));
  } else {
    f_save_as->show();
  }
  f_save_as->run();
  
}

void CrossoverHistory::on_save_as_ok(Gtk::FileSelection *f)
{
  cout << "save as ok" << endl;
  try {
    m_crossover_list.to_xml(f->get_filename());
    f->hide();
    m_filename = f->get_filename();
    set_label("Crossover list [" + m_filename + "]");
    m_SaveButton.set_sensitive(false);
  } catch (GSpeakersException e) {
      Gtk::MessageDialog m(e.what(), Gtk::MESSAGE_ERROR);
      m.run();
  }
}

void CrossoverHistory::on_remove()
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

      if(index < (int)m_crossover_list.crossover_list()->size())
        m_crossover_list.crossover_list()->erase(m_crossover_list.crossover_list()->begin() + index);
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
}

bool CrossoverHistory::on_close(GdkEventAny *event)
{
  cout << "close" << endl;
  hide();
  return false;
}

void CrossoverHistory::create_model()
{
  m_refListStore = Gtk::ListStore::create(m_columns);
  
  //add_items(m_crossover_list);

  for_each(
      m_crossover_list.crossover_list()->begin(), m_crossover_list.crossover_list()->end(),
      slot(*this, &CrossoverHistory::liststore_add_item));

}

void CrossoverHistory::add_columns()
{
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count =m_TreeView.append_column("Id", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.id);
  }

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

}

void CrossoverHistory::add_items(CrossoverList clist)
{

}

void CrossoverHistory::liststore_add_item(Crossover foo)
{
  Gtk::TreeRow row = *(m_refListStore->append());
  row[m_columns.id]         = foo.get_id();
  row[m_columns.id_string]  = foo.get_id_string();
  row[m_columns.type]       = foo.get_type();

}
