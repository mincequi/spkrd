/*
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

#include <time.h>
#include <math.h>
#include "plothistory.h"
#include "gspeakersplot.h"

PlotHistory::PlotHistory() :
  Gtk::Frame(),
  m_Table(9, 4, true), 
  m_RemoveButton("Remove"),
  m_vbox()
{
  //set_title("Plot history");
  m_vbox.set_border_width(8);
  //set_default_size(250, 300);

  m_vbox.pack_start(m_Table);
  add(m_vbox);
  m_Table.set_spacings(4);
  
  m_ScrolledWindow.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  
  m_Table.attach(m_ScrolledWindow, 0, 4, 0, 8, Gtk::FILL);
  m_Table.attach(m_RemoveButton, 3, 4, 8, 9);
  
  create_model();

  /* create tree view */
  m_TreeView.set_model(m_refListStore);
  m_TreeView.set_rules_hint();
  
  //signal_delete_event().connect(slot(*this, &BoxHistory::on_close));
  
  //m_TreeView.set_search_column(m_columns.id.index());
  Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();
  //selection->set_mode(Gtk::SELECTION_MULTIPLE);
  selection->signal_changed().connect(slot(*this, &PlotHistory::on_selection_changed));

  m_RemoveButton.signal_clicked().connect(slot(*this, &PlotHistory::on_remove));

  signal_box_modified.connect(slot(*this, &PlotHistory::on_box_modified));
  signal_add_plot.connect(slot(*this, &PlotHistory::on_add_plot));

  add_columns();
  m_ScrolledWindow.add(m_TreeView);
  show_all();
  index = 0;
}

PlotHistory::~PlotHistory()
{

}

bool PlotHistory::on_delete_event(GdkEventAny *event)
{
  return true;
}

void PlotHistory::on_selection_changed()
{
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  if(const Gtk::TreeIter iter = refSelection->get_selected())
  {
    Gtk::TreePath path = m_refListStore->get_path(iter);

    std::vector<int> indices = path.get_indices();
    if(indices.size() > 0)
    {
      index = indices[0];
      cout << "PlotHistory: selection changed" << endl;
      //signal_box_selected(&((*m_box_list.box_list())[indices[0]]));
    }
  } 
}

void PlotHistory::on_remove()
{
  cout << "PlotHistory: on_remove" << endl;
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  if(const Gtk::TreeIter iter = refSelection->get_selected())
  {
    Gtk::TreePath path = m_refListStore->get_path(iter);

    std::vector<int> indices = path.get_indices();
    if(indices.size() > 0)
    {
      //Remove item from ListStore:
      m_refListStore->erase(iter);

      /* Signal to the plot */
      /* We got the plot index to remove in indices[0] */
      cout << "PlotHistory: plot to remove = " << indices[0] << endl;
      signal_remove_box_plot(indices[0]);
      
      //if(index < (int)m_box_list.box_list()->size())
      //  m_box_list.box_list()->erase(m_box_list.box_list()->begin() + index);
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
}

void PlotHistory::on_box_modified(Box *b)
{

}

void PlotHistory::on_add_plot(Box *b, Speaker *s, Gdk::Color &color) 
{
  //liststore_add_item(*b);
  if ((b != NULL) && (s != NULL)) {

    cout << "PlotHistory: innan boxlist.push_back" << endl;
    m_box_list.box_list()->push_back(*b);
    cout << "PlotHistory: innan speakerlist.push_back" << endl;
    m_speaker_list.speaker_list()->push_back(*s);
    cout << "PlotHistory: innan add item" << endl;
    liststore_add_item(*b, *s, color);
    
    cout << "PlotHistory: plot added" << endl;
  }
}

void PlotHistory::on_cell_plot_toggled(const Glib::ustring& path_string)
{
  cout << "PlotHistory: toggle plot" << endl;
  
  GtkTreePath *gpath = gtk_tree_path_new_from_string (path_string.c_str());
  Gtk::TreePath path(gpath);

  

  /* get toggled iter */
  Gtk::TreeRow row = *(m_refListStore->get_iter(path));

  bool view_plot = row[m_columns.view_plot];

  /* do something with the value */
  view_plot = !view_plot;
  
  path = m_refListStore->get_path(row);

  std::vector<int> indices = path.get_indices();
  if(indices.size() > 0)
  {
    cout << "PlotHistory: hide" << endl;
    signal_hide_box_plot(indices[0]);
  }  
  /* set new value */
  row[m_columns.view_plot] = view_plot;
}

//bool BoxHistory::on_close(GdkEventAny *event)
//{
//  cout << "close" << endl;
//  hide();
//  return false;
//}

void PlotHistory::create_model()
{
  m_refListStore = Gtk::ListStore::create(m_columns);
  
  //add_items(m_box_list);

  //for_each(
  //    m_box_list.box_list()->begin(), m_box_list.box_list()->end(),
  //    slot(*this, &BoxHistory::liststore_add_item));

}

void PlotHistory::add_columns()
{
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count =m_TreeView.append_column("Color", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.color);
    
    pColumn->add_attribute(pRenderer->property_foreground_gdk(), m_columns.color_);
    pColumn->add_attribute(pRenderer->property_weight(), m_columns.weight_);
  }

  {
    Gtk::CellRendererToggle* pRenderer = Gtk::manage( new Gtk::CellRendererToggle() );
    pRenderer->signal_toggled().connect( SigC::slot(*this, &PlotHistory::on_cell_plot_toggled) );

    int cols_count =m_TreeView.append_column("Plot", *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_active(), m_columns.view_plot);
  }


  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count =m_TreeView.append_column("Identifier", *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.id_string);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count =m_TreeView.append_column("Speaker", *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.speaker_string);
  }
  

  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Id", *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.id);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Type", *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.type);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Vb1", *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.vb1);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Fb1", *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.fb1);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Vb2", *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.vb2);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Fb2", *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.fb2);
  }

}

void PlotHistory::liststore_add_item(Box box, Speaker spk, Gdk::Color& color)
{
  gushort  r, g, b;
  r = (int)((color.get_red_p()) * 255);
  g = (int)((color.get_green_p()) * 255);
  b = (int)((color.get_blue_p()) * 255);
  cout << "color: " << r << ", " << g << ", " << b << endl;
  char *str = NULL;
  GString *buffer = g_string_new(str);
  g_string_printf(buffer, "#%.2X%.2X%.2X", r, g, b);
  
  Gtk::TreeRow row = *(m_refListStore->append());
  row[m_columns.color]      = Glib::ustring(buffer->str);
  row[m_columns.view_plot]  = true;
  row[m_columns.id]         = box.get_id();
  row[m_columns.id_string]  = box.get_id_string();
  cout << box.get_id_string() << endl;
  row[m_columns.speaker_string] = spk.get_id_string();
  row[m_columns.type]       = box.get_type();
  row[m_columns.vb1]        = box.get_vb1();
  row[m_columns.fb1]        = box.get_fb1();
  row[m_columns.vb2]        = box.get_vb2();
  row[m_columns.fb2]        = box.get_fb2();
  row[m_columns.color_]     = color;
  row[m_columns.weight_]    = Pango::WEIGHT_HEAVY;
}
