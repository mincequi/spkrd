/*
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

#include "crossovertreeview.h"
#include "common.h"

CrossoverTreeView::CrossoverTreeView() :
  Gtk::Frame("Currently selected crossover")
{
//  set_title("Current crossover");
  set_border_width(8);
  //set_default_size(300, 300);

  add(m_ScrolledWindow);

  m_ScrolledWindow.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  
  /* create model */
  create_model();

  /* create tree view */
  m_TreeView.set_model(m_refTreeStore);
  m_TreeView.set_rules_hint();

  add_columns();
  m_ScrolledWindow.add(m_TreeView);

  signal_crossover_selected.connect(slot(*this, &CrossoverTreeView::on_crossover_selected));
  signal_net_modified_by_wizard.connect(slot(*this, &CrossoverTreeView::on_net_modified_by_wizard));
  
  show_all();
}

CrossoverTreeView::~CrossoverTreeView()
{
}

bool CrossoverTreeView::on_delete_event(GdkEventAny *event)
{
  return true;
}

void CrossoverTreeView::on_net_modified_by_wizard()
{
  cout << "CrossoverTreeView::on_net_modified_by_wizard" << endl;
  on_crossover_selected(cover);
}

void CrossoverTreeView::on_cell_edited_value(const Glib::ustring& path_string, const Glib::ustring& new_text)
{
  cout << "value cell edited" << endl;
  cout << "Path string: " << path_string << endl;
  
  GtkTreePath *gpath = gtk_tree_path_new_from_string(path_string.c_str());
  Gtk::TreePath path(gpath);

  std::vector<int> indices = path.get_indices();
  if(!indices.empty())
  {
    //int i = indices[0];
    //m_vecItems[i].m_number = atoi(new_text.c_str());

    Gtk::TreeRow row = *(m_refTreeStore->get_iter(path));
    row[m_columns.value] = atof(new_text.c_str());
    
    cout << "indices[0:1:2]: " << indices[0] << ":" << indices[1] << ":" << indices[2] << endl;
    /* Since the stupid signals doesn't seem to work we have to go through the data-containers 
       and update values for the particular part we change... */
       
    /* Index is a counter for the extra circuits (impedance correction network, damping network...) we have
       Number of extra circuits + the crossover is the total number of "base nodes" after filter type nodes */
    Net *n = &(*(cover->networks()))[indices[0]];
  
    int ndx = 0;
    bool mod = false;
    if (n->get_has_imp_corr() == true) {
      /* Check if we have edited imp corr */
      if (indices[1] == ndx) {
        /* If we have edited imp corr, update the appropriate component */
        switch (indices[2]) {
          case 0:
            n->get_imp_corr_C().set_value(row[m_columns.value]);
            mod = true;
            break;
          case 1:
            n->get_imp_corr_R().set_value(row[m_columns.value]);
            mod = true;
            break;
        }
      }
      ndx++;
    }
    if (n->get_has_damp() == true) {
      if (indices[1] == ndx) {
        switch (indices[2]) {
          case 0:
            n->get_damp_R1().set_value(row[m_columns.value]);
            mod = true;
            break;
          case 1:
            n->get_damp_R2().set_value(row[m_columns.value]);
            mod = true;
            break;
        }
      }
      ndx++;
    }
    if (n->get_has_res() == true) {
      if (indices[1] == ndx) {
        switch (indices[2]) {
          case 0:
            n->get_res_L().set_value(row[m_columns.value]);
            mod = true;
            break;
          case 1:
            n->get_res_C().set_value(row[m_columns.value]);
            mod = true;
            break;
          case 2:
            n->get_res_R().set_value(row[m_columns.value]);
            mod = true;
            break;

        }
      }
      ndx++;
    }
    /* If we did not modified anything until here, the actual crossover is the circuit to modify */
    if (mod == false) {
      //Part p = (*n.parts())[indices[2]];
      (*(n->parts()))[indices[2]].set_value(row[m_columns.value]);
    }
    cout << "Id is: " << row[m_columns.id] << endl;
    /* Tell others that we have modified a part */
    signal_part_modified.emit();
    signal_net_modified_by_user(n);
  }
}

void CrossoverTreeView::on_cell_edited_unit(const Glib::ustring& path_string, const Glib::ustring& new_text)
{
  cout << "unit cell edited" << endl;
  cout << "Path string: " << path_string << endl;

  GtkTreePath *gpath = gtk_tree_path_new_from_string(path_string.c_str());
  Gtk::TreePath path(gpath);

  std::vector<int> indices = path.get_indices();
  if(!indices.empty())
  {
    //int i = indices[0];
    //m_vecItems[i].m_number = atoi(new_text.c_str());

    Gtk::TreeRow row = *(m_refTreeStore->get_iter(path));
    row[m_columns.unit] = new_text;
    Glib::ustring text = new_text;
    /* try to remove trailing F, f, H or h:s from the string */
    if ((new_text.c_str()[new_text.length() - 1] == 'F') || (new_text.c_str()[new_text.length() - 1] == 'f') ||
        (new_text.c_str()[new_text.length() - 1] == 'h') || (new_text.c_str()[new_text.length() - 1] == 'h')) {
      cout << "remove the farads" << endl;
      text.resize(text.length() - 1);
    }
    if ((text == "Ohm") || (text == "ohm") || (text == "OHM")) {
      text = Glib::ustring("");
    }
    /* TODO: Check that we have correct SPICE units in text */
    
    cout << "indices[0:1:2]: " << indices[0] << ":" << indices[1] << ":" << indices[2] << endl;
    /* Since the stupid signals doesn't seem to work we have to go through the data-containers 
       and update values for the particular part we change... */
       
    /* Index is a counter for the extra circuits (impedance correction network, damping network...) we have
       Number of extra circuits + the crossover is the total number of "base nodes" after filter type nodes */
    int ndx = 0;
    bool mod = false;
    Net *n = &(*(cover->networks()))[indices[0]];
    if (n->get_has_imp_corr() == true) {
      /* Check if we have edited imp corr */
      if (indices[1] == ndx) {
        /* If we have edited imp corr, update the appropriate component */
        switch (indices[2]) {
          case 0:
            n->get_imp_corr_C().set_unit(text);
            mod = true;
            break;
          case 1:
            n->get_imp_corr_R().set_unit(text);
            mod = true;
            break;
        }
      }
      ndx++;
    }
    if (n->get_has_damp() == true) {
      if (indices[1] == ndx) {
        switch (indices[2]) {
          case 0:
            n->get_damp_R1().set_unit(text);
            mod = true;
            break;
          case 1:
            n->get_damp_R2().set_unit(text);
            mod = true;
            break;
        }
      }
      ndx++;
    }
    if (n->get_has_res() == true) {
      if (indices[1] == ndx) {
        switch (indices[2]) {
          case 0:
            n->get_res_L().set_unit(text);
            mod = true;
            break;
          case 1:
            n->get_res_C().set_unit(text);
            mod = true;
            break;
          case 2:
            n->get_res_R().set_unit(text);
            mod = true;
            break;

        }
      }
      ndx++;
    }
    /* If we did not modified anything until here, the actual crossover is the circuit to modify */
    if (mod == false) {
      //Part p = (*n.parts())[indices[2]];
      (*(n->parts()))[indices[2]].set_unit(text);
    }
    cout << "Id is: " << row[m_columns.id] << endl;
    /* Tell others that we have modified a part */
    signal_part_modified.emit();
  }

}


CrossoverTreeView::ModelColumns::ModelColumns()
{
  add(id_string);
  add(id);
  add(type);
  add(value);
  add(unit);
  add(editable);
  add(visible);
}

void CrossoverTreeView::add_items()
{
  vector<CellItem_Crossover> filter;
  filter.push_back( CellItem_Crossover("L1", 1, 0.1, "m", 1) );
  filter.push_back( CellItem_Crossover("C2", 1, 10, "n", 2) );

  vector<CellItem_Crossover> imp_corr;
  imp_corr.push_back( CellItem_Crossover("C3", 1, 1, "n", 3) );
  imp_corr.push_back( CellItem_Crossover("R4", 1, 1, "", 4) );


  vector<CellItem_Crossover> crossover_parts;
  crossover_parts.push_back( CellItem_Crossover("Impedance correction", imp_corr) );
  crossover_parts.push_back( CellItem_Crossover("Filter", filter) );

  m_vecItems.push_back( CellItem_Crossover("Lowpass", crossover_parts) );
    
}

void CrossoverTreeView::on_crossover_selected(Crossover *new_crossover)
{
  m_refTreeStore->clear();
  m_vecItems.erase(m_vecItems.begin(), m_vecItems.end());
  
  cover = new_crossover; //Crossover((Crossover&)new_crossover);
  vector<Net> networks = *(cover->networks());
  
  for (
    vector<Net>::iterator from = cover->networks()->begin();
    from != cover->networks()->end();
    ++from)
  {
    Net n = *from;
    vector<CellItem_Crossover> crossover_elements;
    vector<CellItem_Crossover> filter;
    vector<CellItem_Crossover> imp_corr;
    vector<CellItem_Crossover> damp;
    
    if (n.get_has_imp_corr() == true) {
      imp_corr.push_back( CellItem_Crossover(n.get_imp_corr_C()) );
      imp_corr.push_back( CellItem_Crossover(n.get_imp_corr_R()) );
      crossover_elements.push_back( CellItem_Crossover("Impedace correction", imp_corr) );
    }
    if (n.get_has_damp() == true) {
      damp.push_back( CellItem_Crossover(n.get_damp_R1()) );
      damp.push_back( CellItem_Crossover(n.get_damp_R2()) );
      crossover_elements.push_back( CellItem_Crossover("Damping network", damp) );
    }

    /* The rest of the parts */
    for (
      vector<Part>::iterator iter = n.parts()->begin();
      iter != n.parts()->end();
      ++iter)
    {
      filter.push_back( CellItem_Crossover(*iter) );
    }
    
    int type = n.get_type();
    crossover_elements.push_back( CellItem_Crossover("Filter", filter) );
    
    switch (type) {
      case NET_TYPE_LOWPASS:
        m_vecItems.push_back( CellItem_Crossover("Lowpass", crossover_elements) );
        break;
      case NET_TYPE_BANDPASS:
        m_vecItems.push_back( CellItem_Crossover("Bandpass", crossover_elements) );
        break;
      case NET_TYPE_HIGHPASS:
        m_vecItems.push_back( CellItem_Crossover("Highpass", crossover_elements) );
        break;
      default:
        m_vecItems.push_back( CellItem_Crossover("Other", crossover_elements) );
    }
    
  }
  for_each(
    m_vecItems.begin(), m_vecItems.end(),
    slot(*this, &CrossoverTreeView::treestore_add_item));
  m_TreeView.expand_all();
  //set_title("Current crossover [" + cover->get_id_string() + "]");
}

void CrossoverTreeView::create_model()
{
  m_refTreeStore = Gtk::TreeStore::create(m_columns);

  //add_items();

  std::for_each(
      m_vecItems.begin(), m_vecItems.end(),
      SigC::slot(*this, &CrossoverTreeView::treestore_add_item));
}

void CrossoverTreeView::treestore_add_item(const CellItem_Crossover& foo)
{
  Gtk::TreeRow row = *(m_refTreeStore->append());

  row[m_columns.id_string]    = foo.m_label;
  row[m_columns.type]         = 0;
  row[m_columns.value]        = 0.0;
  row[m_columns.unit]         = "";
  row[m_columns.editable]     = false;
  row[m_columns.id]           = 0;  
  row[m_columns.visible]      = false;
  //Add Children:
  for(vector<CellItem_Crossover>::const_iterator iter = foo.m_children.begin();
      iter != foo.m_children.end();
      ++iter)
  {
    const CellItem_Crossover& child = *iter;

    Gtk::TreeRow child_row = *(m_refTreeStore->append(row.children()));
    child_row[m_columns.id_string]    = child.m_label;
    child_row[m_columns.id]           = 0;
    child_row[m_columns.visible]      = false;
    child_row[m_columns.editable]     = false;
    for(
      vector<CellItem_Crossover>::const_iterator from = child.m_children.begin();
      from != child.m_children.end();
      ++from)
    {
      const CellItem_Crossover& child2 = *from;

      Gtk::TreeRow child_row2 = *(m_refTreeStore->append(child_row.children()));
      child_row2[m_columns.id_string]    = child2.m_label;
      child_row2[m_columns.id]           = child2.m_id;
      child_row2[m_columns.type]         = child2.m_type;
      child_row2[m_columns.value]        = child2.m_value;
      child_row2[m_columns.unit]         = child2.m_unit;
      child_row2[m_columns.editable]     = false;
      child_row2[m_columns.visible]      = true;
    }

  }
}

void CrossoverTreeView::add_columns()
{


  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );
    pRenderer->property_xalign().set_value(0.0);

    int cols_count = m_TreeView.append_column("Id_string", *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count-1);
    if(pColumn)
    {
      pColumn->add_attribute(pRenderer->property_text(), m_columns.id_string);
    }
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );
    pRenderer->property_xalign().set_value(0.0);

    int cols_count = m_TreeView.append_column("Id", *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count-1);
    if(pColumn)
    {
      pColumn->add_attribute(pRenderer->property_text(), m_columns.id);
      pColumn->add_attribute(pRenderer->property_visible(), m_columns.visible);
    }
  }

  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );
    pRenderer->property_xalign().set_value(0.0);

    int cols_count = m_TreeView.append_column("Type", *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count-1);
    if(pColumn)
    {
      pColumn->add_attribute(pRenderer->property_text(), m_columns.type);
      pColumn->add_attribute(pRenderer->property_visible(), m_columns.visible);
    }
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );
    pRenderer->property_xalign().set_value(0.0);
    pRenderer->signal_edited().connect(slot(*this, &CrossoverTreeView::on_cell_edited_value));
    
    int cols_count = m_TreeView.append_column("Value", *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count-1);
    if(pColumn)
    {
      pColumn->add_attribute(pRenderer->property_text(), m_columns.value);
      pColumn->add_attribute(pRenderer->property_editable(), m_columns.editable);
      pColumn->add_attribute(pRenderer->property_visible(), m_columns.visible);
    }
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );
    pRenderer->property_xalign().set_value(0.0);
    pRenderer->signal_edited().connect(slot(*this, &CrossoverTreeView::on_cell_edited_unit));
    
    int cols_count = m_TreeView.append_column("Unit", *pRenderer);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count-1);
    if(pColumn)
    {
      pColumn->add_attribute(pRenderer->property_text(), m_columns.unit);
      pColumn->add_attribute(pRenderer->property_editable(), m_columns.editable);
      pColumn->add_attribute(pRenderer->property_visible(), m_columns.visible);
    }
  }

  
}

void CrossoverTreeView::on_realize()
{
  m_TreeView.expand_all();

  //call base class:
  Frame::on_realize();
}
