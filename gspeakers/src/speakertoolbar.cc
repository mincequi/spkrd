/* 
 * speakertoolbar.cc
 *
 * Copyright (C) 2001 Daniel Sundberg <dss@home.se>
 *
 * http://sumpan.campus.luth.se/software/gspeakers
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
#include <gnome--/dialog.h>
#include <gnome--.h>
#include <gtk--/label.h>
#include <gtk--/imageloader.h>
#include <gtk--/tooltips.h>
#include "speakertoolbar.h"


using SigC::bind;

/*
 * This is the constructor that sets up the toolbar
 */
SpeakerToolbar::SpeakerToolbar( string infile,  GSpeakersCFG *icfg ) 
  : Gtk::HandleBox() {
  Gtk::ImageLoader *il;
  Gnome::Pixmap *pixmap;
  cfg = icfg;
  new_is_hit = false;

  /* Setup toolbar-UI */
  Gtk::Label *label;

  hbox = manage( new Gtk::HBox() );
  add( *hbox );

  new_spk_button = manage( new Gtk::Button() );
  il = new Gtk::ImageLoader( "../xpm/new_speaker.xpm" );
  new_spk_button->add_pixmap( il->pix(), il->bit() );
  new_spk_button->set_relief( GTK_RELIEF_NONE );
  cfg->tooltips->set_tip( *new_spk_button, "New Speaker" );
  hbox->pack_start( *new_spk_button, false, false );
  new_spk_button->clicked.connect( slot( this, &SpeakerToolbar::new_spk ) );

  new_xml_button = manage( new Gtk::Button() );
  new_xml_button->set_relief( GTK_RELIEF_NONE );
  il = new Gtk::ImageLoader( "../xpm/new_xml.xpm" );
  new_xml_button->add_pixmap( il->pix(), il->bit() );
  hbox->pack_start( *new_xml_button, false, false );
  new_xml_button->clicked.connect( slot( this, &SpeakerToolbar::new_xml ) );
  cfg->tooltips->set_tip( *new_xml_button, "New XML-file" );

  open_button = manage( new Gtk::Button() );
  pixmap = manage( new Gnome::StockPixmap( GNOME_STOCK_PIXMAP_OPEN ) );
  open_button->add( *pixmap );

  open_button->clicked.connect( slot( this, &SpeakerToolbar::open ) );
  open_button->set_relief( GTK_RELIEF_NONE );
  hbox->pack_start( *open_button, false, false );
  cfg->tooltips->set_tip( *open_button, "Open XML-file" );

  save_button = manage( new Gtk::Button() );
  save_button->set_relief( GTK_RELIEF_NONE );
  save_button->clicked.connect( slot( this, &SpeakerToolbar::save ) );
  pixmap = manage( new Gnome::StockPixmap( GNOME_STOCK_PIXMAP_SAVE ) );
  save_button->add( *pixmap );
  cfg->tooltips->set_tip( *save_button, "Save current speaker" );
  hbox->pack_start( *save_button, false, false ); 

  save_as_button = manage( new Gtk::Button() );
  save_as_button->set_relief( GTK_RELIEF_NONE );
  save_as_button->clicked.connect( slot( this, &SpeakerToolbar::save_as ) );
  pixmap = manage( new Gnome::StockPixmap( GNOME_STOCK_PIXMAP_SAVE_AS ) );
  save_as_button->add( *pixmap );

  hbox->pack_start( *save_as_button, false, false ); 
  cfg->tooltips->set_tip( *save_as_button, "Save XML-file as" );

  remove_button = manage( new Gtk::Button() );
  remove_button->clicked.connect( slot( this, &SpeakerToolbar::remove ) );
  remove_button->set_relief( GTK_RELIEF_NONE );
  pixmap = manage( new Gnome::StockPixmap( GNOME_STOCK_PIXMAP_CLOSE ) );
  remove_button->add( *pixmap );

  hbox->pack_start( *remove_button, false, false ); 
  cfg->tooltips->set_tip( *remove_button, "Delete this speaker" );

  filename_entry = manage( new Gtk::Entry() );
  filename_entry->set_sensitive( false );
  filename_entry->set_usize( 100, 20 );
  hbox->pack_start( *filename_entry, false, false ); 

  speaker_combo = manage( new Gtk::Combo() );
  speaker_combo->set_usize( 100, 20 );
  /* combo action on entry changed */
  speaker_combo->get_entry()->changed.connect( slot( this, &SpeakerToolbar::combo_changed ) );
  hbox->pack_start( *speaker_combo, false, false );
  label = manage( new Gtk::Label("   Vas: ") );
  hbox->pack_start( *label, false, false );
  vas_entry = manage( new Gtk::Entry() );
  vas_entry->set_usize( 50, 20 );
  hbox->pack_start( *vas_entry, false, false );
  label = manage( new Gtk::Label("   Fs: ") );
  hbox->pack_start( *label, false, false );
  fs_entry = manage( new Gtk::Entry() );
  fs_entry->set_usize( 50, 20 );
  hbox->pack_start( *fs_entry, false, false );
  label = manage( new Gtk::Label("   Qts: ") );
  hbox->pack_start( *label, false, false );
  qts_entry = manage( new Gtk::Entry() );
  qts_entry->set_max_length(5);
  qts_entry->set_usize( 50, 20 );
  hbox->pack_start( *qts_entry, false, false );

  /* Read default xml-file here */
  current_file = infile;
  filename_entry->set_text( current_file );
  cfg->tooltips->set_tip( *filename_entry, current_file );
  load_xml( current_file );
}

SpeakerToolbar::~SpeakerToolbar() {

}

/*
 * Returns the speaker currently selected in the toolbar
 *
 * For external usage
 */
Speaker *SpeakerToolbar::get_current_speaker() {
  Speaker *s = new Speaker( speaker_combo->get_entry()->get_text(), qts_entry->get_text(), 
			    vas_entry->get_text(), fs_entry->get_text() );
  return s;
}

/*
 * This empties the toolbar entries and prepares for a new speaker
 */
void SpeakerToolbar::new_spk() {
  cout << "new speaker" << endl;
  vas_entry->set_text("");
  qts_entry->set_text("");
  fs_entry->set_text("");
  speaker_combo->get_entry()->set_text("");
  new_is_hit = true;
}

/*
 * This empties the toolbar and prepares for a new xml-file
 */
void SpeakerToolbar::new_xml() {
  cout << "new xml" << endl;
  vector<Speaker> new_spk_list;
  spk_list = new_spk_list;
  vector<string> newpopdown;
  speakers_list = newpopdown;
  speaker_combo->set_popdown_strings( newpopdown );
  vas_entry->set_text("");
  qts_entry->set_text("");
  fs_entry->set_text("");
  filename_entry->set_text("");
  cfg->tooltips->set_tip( *filename_entry, current_file );
  speaker_combo->get_entry()->set_text("");
  new_is_hit = true;
}

/*
 * This opens a fileselection-dialog
 */
void SpeakerToolbar::open() {
  Gtk::FileSelection *s = manage( new Gtk::FileSelection( "Open new speakers xml" ) );
  s->get_ok_button()->clicked.connect( bind<Gtk::FileSelection *>( slot( this, &SpeakerToolbar::open_action ), s ) );
  s->get_cancel_button()->clicked.connect( bind<Gtk::FileSelection *>( slot( this, &SpeakerToolbar::cancel_action ), s ) ); 
 
  s->show(); 
} 

/*
 * This will open/load a new speaker xml-file
 */
void SpeakerToolbar::open_action( Gtk::FileSelection *s ) { 
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
    cfg->tooltips->set_tip( *filename_entry, current_file.substr( i + 1 ) );
  }
} 

/*
 * Called on fileselection cancel
 */
void SpeakerToolbar::cancel_action( Gtk::FileSelection *s ) {
  s->hide(); // maybe destroy instead???
}

/* 
 *  - Insert current speaker into spk_list 
 *  - Insert speaker into xml-tree
 *  - Save xml-tree to disk
 */
void SpeakerToolbar::save() {
  cout << "save speaker" << endl;
  if ( new_is_hit == true ) {
    Speaker s( speaker_combo->get_entry()->get_text(), qts_entry->get_text(), 
	       vas_entry->get_text(), fs_entry->get_text() );
    spk_list.push_back( s );
    speakers_list.insert( speakers_list.begin(), speaker_combo->get_entry()->get_text() );
    speaker_combo->set_popdown_strings( speakers_list );
    new_is_hit = false;
  }
  save_data_to_xml();  
}

/*
 *  - Bring up file-dialog
 */
void SpeakerToolbar::save_as() {
  cout << "save as: " << new_is_hit << endl;
  Gtk::FileSelection *s = manage( new Gtk::FileSelection( "Save this speaker xml" ) );
  s->get_ok_button()->clicked.connect( bind<Gtk::FileSelection *>( slot( this, &SpeakerToolbar::save_as_action ), s ) );
  s->get_cancel_button()->clicked.connect( bind<Gtk::FileSelection *>( slot( this, &SpeakerToolbar::cancel_action ), s ) ); 
 
  s->show(); 

}

/*
 * Get new filename and call save()
 */
void SpeakerToolbar::save_as_action( Gtk::FileSelection *s ) { 
  string::size_type i;
  s->hide(); 

  current_file = s->get_filename();
  i = current_file.find_last_of("/");
  if ( i != string::npos ) {
    filename_entry->set_text( current_file.substr( i + 1 ) );
    cfg->tooltips->set_tip( *filename_entry, current_file.substr( i + 1 ) );
    
  }
  
  save();
} 

/*
 * Load an xml-file into a Speaker-vector
 */
void SpeakerToolbar::load_xml(string filename) {
  xmlNodePtr cur, child;
  Speaker cur_spk;

  doc = xmlParseFile( filename.c_str() );
  if (doc != NULL) {
    cout << "parsing...speakers" << endl;
    cur = xmlDocGetRootElement(doc);
    if ( cur != NULL ) {
       cur = cur->xmlChildrenNode;

       /* Parse the document */
       while ( ( cur != NULL ) && ( strcmp( (char *)cur->name, "speaker" ) == 0 ) ) {
	 child = cur->childs;
	 while ( child != NULL ) {
	   if ( strcmp( (char *)(child->name), "name" ) == 0 ) {
	     speakers_list.push_back( (char *)xmlNodeGetContent( child ) );
	     cur_spk.name = (char *)xmlNodeGetContent( child );
	   }
	   if ( strcmp( (char *)(child->name), "qts" ) == 0 ) {
	     cur_spk.qts = atof( (char *)xmlNodeGetContent( child ) );
	   }
	   if ( strcmp( (char *)(child->name), "fs" ) == 0 ) {
	     cur_spk.fs = atof( (char *)xmlNodeGetContent( child ) );
	   }
	   if ( strcmp( (char *)(child->name), "vas" ) == 0 ) {
	     cur_spk.vas = atof( (char *)xmlNodeGetContent( child ) );
	   }
	   child = child->next;
	 }
	 spk_list.push_back( cur_spk );
	 cur = cur->next;
       }

    } else {
      Gnome::Dialogs::error("Empty document!");
    }
  } else {
    Gnome::Dialogs::error("Could not open xml file!\nThis is probably not a valid xml-file.");
  }
  //  set_speaker_data( speakers_list[0] );
  speaker_combo->set_popdown_strings( speakers_list );
}

/* 
 * - Build new xml-doc from spk_list
 * - Save the tree to disk
 *
 * This is probably an ignorant implementation but it seems 
 * stable and does what it should
 */
void SpeakerToolbar::save_data_to_xml() {
  xmlNodePtr spk, cur, node;

  /* Create new xml-document */
  doc = xmlNewDoc( (xmlChar *)("1.0") );
  node = xmlNewDocNode( doc, NULL, (xmlChar *)("speakerfile"), NULL );
  /* The following row was not in the doc, but a look in Galeons source solved my problem */
  xmlDocSetRootElement( doc, node );
  
  /* Put every Speaker-element in xml-tree */
  for (
       vector<Speaker>::iterator from = spk_list.begin(); 
       from != spk_list.end(); 
       ++from
       ) {
    spk = xmlNewChild( node, NULL, (xmlChar *)("speaker"), NULL );
    cur = xmlNewChild( spk, NULL, (xmlChar *)("name"), NULL );
    xmlNodeSetContent( cur, (xmlChar *)(from->name.c_str()) );
    cur = xmlNewChild( spk, NULL, (xmlChar *)("vas"), NULL );
    xmlNodeSetContent( cur, (xmlChar *)g_strdup_printf("%f", from->vas ) );
    cur = xmlNewChild( spk, NULL, (xmlChar *)("qts"), NULL );
    xmlNodeSetContent( cur, (xmlChar *)g_strdup_printf("%f", from->qts ) );
    cur = xmlNewChild( spk, NULL, (xmlChar *)("fs"), NULL );
    xmlNodeSetContent( cur, (xmlChar *)g_strdup_printf("%f", from->fs ) );
  }
  cout << current_file.c_str() << endl;
  /* Write the xml-tree to disk and output number of bytes written */
  cout << xmlSaveFile( current_file.c_str(), doc ) << endl;
}

/*
 * Display data for a speaker by it's "name"
 */
int SpeakerToolbar::set_speaker_data( string speaker ) {
  for (
       vector<Speaker>::iterator from = spk_list.begin(); 
       from != spk_list.end(); 
       ++from
       ) {
    if ( from->name == speaker ) {
      vas_entry->set_text( g_strdup_printf("%5.2f", from->vas ) );
      qts_entry->set_text( g_strdup_printf("%5.3f", from->qts ) );
      fs_entry->set_text( g_strdup_printf("%4.1f", from->fs ) );
    }
  }
}

/*
 * Removes a Speaker from Speaker-vector, xml and toolbar
 */
void SpeakerToolbar::remove() {
  cout << "remove" << endl;
  string s = speaker_combo->get_entry()->get_text();
  
  /* Find speaker in speaker-vector and erase it */
  for (
       vector<Speaker>::iterator from = spk_list.begin(); 
       from != spk_list.end(); 
       ++from
       ) {
    if ( from->name == s ) {
      spk_list.erase( from );

      /* Find speaker in list of text-strings and erase it */
      for (
  	   vector<string>::iterator from = speakers_list.begin(); 
  	   from != speakers_list.end(); 
  	   ++from
  	   ) {
  	if ( *from == s ) {
	  speakers_list.erase( from );
	  break;
  	}
      }
      save_data_to_xml();
      speaker_combo->set_popdown_strings( speakers_list );
      set_speaker_data( speakers_list[0] );
      break;
    }
  }
}

/*
 * selfexplaining isn't it?
 */
void SpeakerToolbar::combo_changed() {
  set_speaker_data( speaker_combo->get_entry()->get_text() );
}
