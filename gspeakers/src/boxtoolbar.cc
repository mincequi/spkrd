/* 
 * speakertoolbar.h
 *
 * Copyright (C) 2001 Daniel Sundberg <dss@home.se>
 *
 * http://sumpan.campus.luth.se/software/jags
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <gtk--/label.h>
#include <gtk--/item.h>
#include <gnome--/dialog.h>
#include "boxtoolbar.h"

using SigC::bind;
BoxToolbar::BoxToolbar( string infile ) : Gtk::HandleBox() {
  new_is_hit = false;

  /* Setup toolbar-UI */
  Gtk::Label *label;

  hbox = manage( new Gtk::HBox() );
  add( *hbox );

  new_box_button = manage( new Gtk::Button("New box") );

  /* relief = NONE for all buttons ??? */
  //new_box_button->set_relief( GTK_RELIEF_NONE );

  hbox->pack_start( *new_box_button, false, false );
  new_box_button->clicked.connect( slot( this, &BoxToolbar::new_box ) );
  new_xml_button = manage( new Gtk::Button("New xml") );
  hbox->pack_start( *new_xml_button, false, false );
  new_xml_button->clicked.connect( slot( this, &BoxToolbar::new_xml ) );

  open_button = manage( new Gtk::Button("Open") );
  open_button->clicked.connect( slot( this, &BoxToolbar::open ) );
  hbox->pack_start( *open_button, false, false );
  save_button = manage( new Gtk::Button("Save") );
  save_button->clicked.connect( slot( this, &BoxToolbar::save ) );
  hbox->pack_start( *save_button, false, false ); 
  save_as_button = manage( new Gtk::Button("Save as") );
  save_as_button->clicked.connect( slot( this, &BoxToolbar::save_as ) );
  hbox->pack_start( *save_as_button, false, false ); 
  remove_button = manage( new Gtk::Button("Del") );
  remove_button->clicked.connect( slot( this, &BoxToolbar::remove ) );
  hbox->pack_start( *remove_button, false, false ); 

  filename_entry = manage( new Gtk::Entry() );
  filename_entry->set_sensitive( false );
  hbox->pack_start( *filename_entry, false, false ); 

  box_combo = manage( new Gtk::Combo() );
  /* combo action on entry changed */
  box_combo->get_entry()->changed.connect( slot( this, &BoxToolbar::combo_changed ) );
  hbox->pack_start( *box_combo, false, false );

  /* Setup the optionmenu */
  types_menu = manage( new Gtk::Menu() );
  types_menu->items().push_back( Gtk::Menu_Helpers::MenuElem( "Ported", bind<int>( slot( this, &BoxToolbar::option_menu_changed ), PORTED ) ) );
  types_menu->items().push_back( Gtk::Menu_Helpers::MenuElem( "Sealed", bind<int>( slot( this, &BoxToolbar::option_menu_changed ), SEALED ) ) );
  box_type_option = manage( new Gtk::OptionMenu() );
  box_type_option->set_menu( *types_menu );
  hbox->pack_start( *box_type_option, false, false );

  label = manage( new Gtk::Label("   Vol1: ") );
  hbox->pack_start( *label, false, false );
  vol1_entry = manage( new Gtk::Entry() );
  vol1_entry->set_usize( 50, 20 );
  hbox->pack_start( *vol1_entry, false, false );
  label = manage( new Gtk::Label("   Fs1: ") );
  hbox->pack_start( *label, false, false );
  fb1_entry = manage( new Gtk::Entry() );
  fb1_entry->set_usize( 50, 20 );
  hbox->pack_start( *fb1_entry, false, false );

//    label = manage( new Gtk::Label("   Vol2: ") );
//    hbox->pack_start( *label, false, false );
    vol2_entry = manage( new Gtk::Entry() );
//    vol2_entry->set_usize( 50, 20 );
//    hbox->pack_start( *vol2_entry, false, false );
//    label = manage( new Gtk::Label("   Fs2: ") );
//    hbox->pack_start( *label, false, false );
    fb2_entry = manage( new Gtk::Entry() );
//    fb2_entry->set_usize( 50, 20 );
//    hbox->pack_start( *fb2_entry, false, false );

//    vol2_entry->set_sensitive( false );
//    fb2_entry->set_sensitive( false );
  
  /* Read default xml-file here */
  current_file = infile;
  filename_entry->set_text( current_file );
  load_xml( current_file );
}

BoxToolbar::~BoxToolbar() {

}

/*
 * This empties the toolbar entries and prepares for a new box
 */
void BoxToolbar::new_box() {
  cout << "new box" << endl;
  vol1_entry->set_text("");
  vol2_entry->set_text("");
  fb1_entry->set_text("");
  fb2_entry->set_text("");
  box_combo->get_entry()->set_text("");
  new_is_hit = true;

  /* Deal with the menu */
  current_boxtype = PORTED;
  box_type_option->remove_menu();  
  types_menu = manage( new Gtk::Menu() );
  types_menu->items().push_back( Gtk::Menu_Helpers::MenuElem( "Ported", bind<int>( slot( this, &BoxToolbar::option_menu_changed ), PORTED ) ) );
  types_menu->items().push_back( Gtk::Menu_Helpers::MenuElem( "Sealed", bind<int>( slot( this, &BoxToolbar::option_menu_changed ), SEALED ) ) );
  box_type_option->set_menu( *types_menu );  
}

/*
 * This empties the toolbar and prepares for a new xml-file
 */
void BoxToolbar::new_xml() {
  cout << "new xml" << endl;
  vector<Box> new_box_list;
  box_list = new_box_list;
  vector<string> newpopdown;
  box_string_list = newpopdown;
  box_combo->set_popdown_strings( newpopdown );
  filename_entry->set_text("");
  new_box();
}

/*
 * This opens a fileselection-dialog
 */
void BoxToolbar::open() {
  Gtk::FileSelection *s = manage( new Gtk::FileSelection( "Open new box xml" ) );
  s->get_ok_button()->clicked.connect( bind<Gtk::FileSelection *>( slot( this, &BoxToolbar::open_action ), s ) );
  s->get_cancel_button()->clicked.connect( bind<Gtk::FileSelection *>( slot( this, &BoxToolbar::cancel_action ), s ) ); 
 
  s->show(); 
}

/*
 * This will open/load a new speaker xml-file
 */
void BoxToolbar::open_action( Gtk::FileSelection *s ) {
  string::size_type i;
  new_xml();
  /* Ah, this is enabled by default in new_xml(), disable until user hits "new" */
  new_is_hit = false;
  
  cout << "file open ok clicked: " << s->get_filename() << endl;  //debug
  /* Maybe we should destroy this instead, test test test this */
  s->hide(); 
  load_xml( s->get_filename() ); 
  current_file = s->get_filename();
  i = current_file.find_last_of("/");
  if ( i != string::npos ) {
    filename_entry->set_text( current_file.substr( i + 1 ) );
  }
}

/*
 * Called on fileselection cancel
 */
void BoxToolbar::cancel_action( Gtk::FileSelection *s ) {
  s->hide(); // maybe destroy instead???
}

/* 
 *  - Insert current box into spk_list 
 *  - Insert box into xml-tree
 *  - Save xml-tree to disk
 */
void BoxToolbar::save() {
  cout << "save box" << endl;
  if ( new_is_hit == true ) {
    /* First find out what type of box we have */
    Box b( box_combo->get_entry()->get_text(), vol1_entry->get_text(), current_boxtype,
	   fb1_entry->get_text(), vol2_entry->get_text(), fb2_entry->get_text() );
    box_list.push_back( b );
    box_string_list.insert( box_string_list.begin(), box_combo->get_entry()->get_text() );
    box_combo->set_popdown_strings( box_string_list );
    new_is_hit = false;
  }
  save_data_to_xml();  
}

void BoxToolbar::save_as() {
  cout << "save as: " << new_is_hit << endl;
  Gtk::FileSelection *s = manage( new Gtk::FileSelection( "Save this enclosure xml" ) );
  s->get_ok_button()->clicked.connect( bind<Gtk::FileSelection *>( slot( this, &BoxToolbar::save_as_action ), s ) );
  s->get_cancel_button()->clicked.connect( bind<Gtk::FileSelection *>( slot( this, &BoxToolbar::cancel_action ), s ) ); 
 
  s->show(); 

}

void BoxToolbar::load_xml(string filename) {
  xmlNodePtr cur, child;
  Box cur_box;

  doc = xmlParseFile( filename.c_str() );
  if (doc != NULL) {
    cout << "parsing...enclosure: " << filename << endl;
    cur = xmlDocGetRootElement(doc);
    if ( cur != NULL ) {
       cur = cur->xmlChildrenNode;

       /* Parse the document */
       while ( ( cur != NULL ) && ( strcmp( (char *)cur->name, "enclosure" ) == 0 ) ) {
	 child = cur->childs;
	 while ( child != NULL ) {
	   if ( strcmp( (char *)(child->name), "name" ) == 0 ) {

	     box_string_list.push_back( (char *)xmlNodeGetContent( child ) );
	     cur_box.name = (char *)xmlNodeGetContent( child );

	   }
	   if ( strcmp( (char *)(child->name), "vol1" ) == 0 ) {
	     if ( xmlNodeGetContent( child ) != NULL) {
	       cur_box.vol1 = atof( (char *)xmlNodeGetContent( child ) );
	     } else {
	       cur_box.vol1 = 0;
	     }
	   }
	   if ( strcmp( (char *)(child->name), "vol2" ) == 0 ) {
	     if ( xmlNodeGetContent( child ) != NULL) {
	       cur_box.vol2 = atof( (char *)xmlNodeGetContent( child ) );
	     } else {
	       cur_box.vol2 = 0;
	     }
	   }
	   if ( strcmp( (char *)(child->name), "fb1" ) == 0 ) {
	     if ( xmlNodeGetContent( child ) != NULL) {
	       cur_box.fb1 = atof( (char *)xmlNodeGetContent( child ) );
	     } else {
	       cur_box.fb1 = 0;
	     }
	   }
	   if ( strcmp( (char *)(child->name), "fb2" ) == 0 ) {
	     if ( xmlNodeGetContent( child ) != NULL) {
	       cur_box.fb2 = atof( (char *)xmlNodeGetContent( child ) );
	     } else {
	       cur_box.fb2 = 0;
	     }
	   }
	   if ( strcmp( (char *)(child->name), "type" ) == 0 ) {
	     if ( xmlNodeGetContent( child ) != NULL) {
	       cur_box.type = atoi( (char *)xmlNodeGetContent( child ) );
	     } else {
	       cur_box.type = SEALED;
	     }
	   }
	   child = child->next;
	 }
	 box_list.push_back( cur_box );
	 cur = cur->next;
       }

    } else {
      Gnome::Dialogs::error("Empty document!");
    }
  } else {
    Gnome::Dialogs::error("Could not open xml file!\nThis is probably not a valid xml-file.");
  }
  set_box_data( box_string_list[0] );
  box_combo->set_popdown_strings( box_string_list );

}

void BoxToolbar::info() {
}

void BoxToolbar::option_menu_changed( int new_type ) {
  switch ( new_type ) {
  case PORTED:
    current_boxtype = PORTED;
    cout << "ported" << endl;
    break;
  case SEALED:
    current_boxtype = SEALED;
    cout << "sealed" << endl;
    break;
  default:
    cout << "could not determine type" << endl;
    break;
  }
}


void BoxToolbar::save_as_action( Gtk::FileSelection *s ) {

}


void BoxToolbar::save_data_to_xml() {
  xmlNodePtr box, cur, node;

  /* Create new xml-document */
  doc = xmlNewDoc( (xmlChar *)("1.0") );
  node = xmlNewDocNode( doc, NULL, (xmlChar *)("enclosurefile"), NULL );
  /* The following row was not in the doc, but a look in Galeons source solved my problem */
  xmlDocSetRootElement( doc, node );
  
  /* Put every Speaker-element in xml-tree */
  for (
       vector<Box>::iterator from = box_list.begin(); 
       from != box_list.end(); 
       ++from
       ) {
    box = xmlNewChild( node, NULL, (xmlChar *)("enclosure"), NULL );
    cur = xmlNewChild( box, NULL, (xmlChar *)("name"), NULL );
    xmlNodeSetContent( cur, (xmlChar *)(from->name.c_str()) );
    cur = xmlNewChild( box, NULL, (xmlChar *)("vol1"), NULL );
    xmlNodeSetContent( cur, (xmlChar *)g_strdup_printf("%f", from->vol1 ) );
    cur = xmlNewChild( box, NULL, (xmlChar *)("fb1"), NULL );
    xmlNodeSetContent( cur, (xmlChar *)g_strdup_printf("%f", from->fb1 ) );
    cur = xmlNewChild( box, NULL, (xmlChar *)("vol2"), NULL );
    xmlNodeSetContent( cur, (xmlChar *)g_strdup_printf("%f", from->vol2 ) );
    cur = xmlNewChild( box, NULL, (xmlChar *)("fb2"), NULL );
    xmlNodeSetContent( cur, (xmlChar *)g_strdup_printf("%f", from->fb2 ) );
    cur = xmlNewChild( box, NULL, (xmlChar *)("type"), NULL );
    xmlNodeSetContent( cur, (xmlChar *)g_strdup_printf("%d", from->type ) );

  }
  cout << current_file.c_str() << endl;
  /* Write the xml-tree to disk and output number of bytes written */
  cout << xmlSaveFile( current_file.c_str(), doc ) << endl;

}

int BoxToolbar::set_box_data( string enclosure ) {
  
  for (
       vector<Box>::iterator from = box_list.begin(); 
       from != box_list.end(); 
       ++from
       ) {
    if ( from->name == enclosure ) {
      vol1_entry->set_text( g_strdup_printf("%5.2f", from->vol1 ) );
      fb1_entry->set_text( g_strdup_printf("%5.2f", from->fb1 ) );
      vol2_entry->set_text( g_strdup_printf("%5.2f", from->vol2 ) );
      fb2_entry->set_text( g_strdup_printf("%5.2f", from->fb2 ) );

      /* Deal with the menu */
      current_boxtype = from->type;
      //      cout << from->type << endl;
      box_type_option->remove_menu();  
      types_menu = manage( new Gtk::Menu() );
      if ( current_boxtype == PORTED ) {
	types_menu->items().push_back( Gtk::Menu_Helpers::MenuElem( "Ported", bind<int>( slot( this, &BoxToolbar::option_menu_changed ), PORTED ) ) );
	types_menu->items().push_back( Gtk::Menu_Helpers::MenuElem( "Sealed", bind<int>( slot( this, &BoxToolbar::option_menu_changed ), SEALED ) ) );
      } else {
	types_menu->items().push_back( Gtk::Menu_Helpers::MenuElem( "Sealed", bind<int>( slot( this, &BoxToolbar::option_menu_changed ), SEALED ) ) );
	types_menu->items().push_back( Gtk::Menu_Helpers::MenuElem( "Ported", bind<int>( slot( this, &BoxToolbar::option_menu_changed ), PORTED ) ) );
      }
      box_type_option->set_menu( *types_menu );  
    }
  }

}

/*
 * Removes a Speaker from Speaker-vector, xml and toolbar
 */
void BoxToolbar::remove() {
  cout << "remove" << endl;
  string s = box_combo->get_entry()->get_text();
  
  /* Find speaker in speaker-vector and erase it */
  for (
       vector<Box>::iterator from = box_list.begin(); 
       from != box_list.end(); 
       ++from
       ) {
    if ( from->name == s ) {
      box_list.erase( from );

      /* Find speaker in list of text-strings and erase it */
      for (
  	   vector<string>::iterator from = box_string_list.begin(); 
  	   from != box_string_list.end(); 
  	   ++from
  	   ) {
  	if ( *from == s ) {
	  box_string_list.erase( from );
	  break;
  	}
      }
      save_data_to_xml();
      box_combo->set_popdown_strings( box_string_list );
      set_box_data( box_string_list[0] );
      break;
    }
  }
}


void BoxToolbar::combo_changed() {
  set_box_data( box_combo->get_entry()->get_text() );
}

void BoxToolbar::set_box_data(double vol1, double fb1 = 0, double vol2 = 0, double fb2 = 0) {
  vol1_entry->set_text( g_strdup_printf("%5.2f", vol1 ) );
  if ( fb1 != 0 ) {
    fb1_entry->set_text( g_strdup_printf("%5.2f", fb1 ) );
  }
  if ( vol2 != 0 ) {
  vol2_entry->set_text( g_strdup_printf("%5.2f", vol2 ) );
  }
  if ( fb2 != 0 ) {
  fb2_entry->set_text( g_strdup_printf("%5.2f", fb2 ) );
  }

}

Box *BoxToolbar::get_box(void) {
  Box *b = new Box( box_combo->get_entry()->get_text(), vol1_entry->get_text(), 
		    current_boxtype, fb1_entry->get_text(), 
		    vol2_entry->get_text(), fb2_entry->get_text() );
  return b;
}
