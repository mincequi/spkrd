/* gspeakersmenu.cc
 *
 * Copyright (C) 2002 Daniel Sundberg <dss@home.se>
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

#include <gtk--/menu.h>
#include "gspeakersmenu.h"

using SigC::slot;
using SigC::bind;
using namespace Gtk;

GSpeakersMenu::GSpeakersMenu( SpeakerToolbar *isbar, BoxToolbar *ibbar, 
			      SimToolbar *isimbar, GSpeakersCFG *icfg ) 
  : Gtk::MenuBar() {
  sbar = isbar;
  bbar = ibbar;
  simbar = isimbar;
  cfg = icfg;

  {
    using namespace Menu_Helpers;

    /* File menu */
    Menu    *menu_file      = manage( new Menu());
    MenuList& list_file     = menu_file->items();
    
    list_file.push_back( MenuElem( "_Quit", "<control>q", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), FILE_QUIT ) ) );
    
    /* Speaker menu */
    Menu *menu_speaker         = manage( new Menu());
    MenuList& list_speaker     = menu_speaker->items();
    list_speaker.push_back( MenuElem( "_New", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), SPEAKER_NEW ) ) );
    list_speaker.push_back( MenuElem( "New _xml", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), SPEAKER_NEW_XML ) ) );
    list_speaker.push_back( MenuElem( "_Open", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), SPEAKER_OPEN ) ) );
    list_speaker.push_back( MenuElem( "_Save", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), SPEAKER_SAVE ) ) );
    list_speaker.push_back( MenuElem( "Save _as...", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), SPEAKER_SAVE_AS ) ) );
    list_speaker.push_back( MenuElem( "_Delete", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), SPEAKER_REMOVE ) ) );
    list_speaker.push_back( SeparatorElem() );
    list_speaker.push_back( MenuElem( "_Import...", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), SPEAKER_IMPORT ) ) );

    /* Box/Enclosure menu */
    Menu *menu_box         = manage( new Menu());
    MenuList& list_box     = menu_box->items();
    list_box.push_back( MenuElem( "_New", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), BOX_NEW ) ) );
    list_box.push_back( MenuElem( "New _xml", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), BOX_NEW_XML ) ) );
    list_box.push_back( MenuElem( "_Open", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), BOX_OPEN ) ) );
    list_box.push_back( MenuElem( "_Save", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), BOX_SAVE ) ) );
    list_box.push_back( MenuElem( "Save _as...", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), BOX_SAVE_AS ) ) );
    list_box.push_back( MenuElem( "_Delete", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), BOX_REMOVE ) ) );

    /* Plot menu */
    Menu *menu_simulate         = manage( new Menu());
    MenuList& list_simulate     = menu_simulate->items();
    list_simulate.push_back( MenuElem( "_Plot current speaker in current enclsure", "<control>p", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), SIMULATE_PLOT ) ) );
    list_simulate.push_back( MenuElem( "Plot speaker in all _boxes", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), SIMULATE_PLOT_MBOXES ) ) );
    list_simulate.push_back( MenuElem( "Plot all _speakers in box", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), SIMULATE_PLOT_MSPEAKERS ) ) );
    list_simulate.push_back( MenuElem( "Plot _each speaker in each box", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), SIMULATE_PLOT_MBOXES_MSPEAKERS ) ) );
    list_simulate.push_back( SeparatorElem() );
    list_simulate.push_back( MenuElem( "_Remove selected plot", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), SIMULATE_PLOT_REMOVE ) ) );
    list_simulate.push_back( MenuElem( "Remove _all plots", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), SIMULATE_PLOT_REMOVE_ALL ) ) );

    Menu *menu_optimize         = manage( new Menu());
    MenuList& list_optimize     = menu_optimize->items();
    list_optimize.push_back( MenuElem( "Calculate _optimized enclosure...", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), OPTIMIZE_VOLUME ) ) );

    Menu *menu_settings         = manage( new Menu());
    MenuList& list_settings     = menu_settings->items();
    list_settings.push_back( MenuElem( "_Preferences...", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), SETTINGS_PREFERENCES ) ) );
    
    Menu *menu_help         = manage( new Menu());
    MenuList& list_help     = menu_help->items();
    list_help.push_back( MenuElem( "_About...", bind<int>( slot( this, &GSpeakersMenu::item_activated_cb ), HELP_ABOUT ) ) );

    //    MenuList& list_bar = items();
    items().push_front(MenuElem("_Help", "<alt>h", *menu_help ) );
    items().push_front(MenuElem("Se_ttings", "<alt>t", *menu_settings ) );
    items().push_front(MenuElem("_Optimize", "<alt>o", *menu_optimize ) );
    items().push_front(MenuElem("_Plot", "<alt>p", *menu_simulate ) );
    items().push_front(MenuElem("_Enclosure", "<alt>e", *menu_box ) );
    items().push_front(MenuElem("_Speaker", "<alt>s", *menu_speaker ) );
    items().push_front(MenuElem("_File", "<alt>f", *menu_file ) );
  }
} 

GSpeakersMenu::~GSpeakersMenu() {

}

/* 
 * One callback for every menucommand since this menu just request the functions 
 * in the toolbar classes.
 */
void GSpeakersMenu::item_activated_cb( int item_id ) {
  switch ( item_id ) {
    /* File menu */
  case FILE_QUIT:
    Gtk::Main::quit();
    break;
    /* Speaker menu */
  case SPEAKER_NEW:
    sbar->new_spk();
    break;
  case SPEAKER_NEW_XML:
    sbar->new_xml();
    break;
  case SPEAKER_OPEN:
    sbar->open();
    break;
  case SPEAKER_SAVE:
    sbar->save();
    break;
  case SPEAKER_SAVE_AS:
    sbar->save_as();
    break;
  case SPEAKER_REMOVE:
    sbar->remove();
    break;
    /* Enclosure menu */
  case BOX_NEW:
    bbar->new_box();
    break;
  case BOX_NEW_XML:
    bbar->new_xml();
    break;
  case BOX_OPEN:
    bbar->open();
    break;
  case BOX_SAVE:
    bbar->save();
    break;
  case BOX_SAVE_AS:
    bbar->save_as();
    break;
  case BOX_REMOVE:
    bbar->remove();
    break;
    /* Simulate menu */
  case SIMULATE_PLOT:
    simbar->sim_clicked();
    break;
  case SIMULATE_PLOT_MBOXES:
    simbar->sim_all_boxes_clicked();
    break;
  case SIMULATE_PLOT_MSPEAKERS:
    simbar->sim_all_speakers_clicked();
    break;
  case SIMULATE_PLOT_MBOXES_MSPEAKERS:
    simbar->all_boxes_all_speakers_clicked();
    break;
  case SIMULATE_PLOT_REMOVE:
    simbar->rem_plot_clicked();
    break;
  case SIMULATE_PLOT_REMOVE_ALL:
    simbar->rem_all_clicked();
    break;
    /* Optimize menu */
  case OPTIMIZE_VOLUME:
    simbar->opt_box_clicked();
    break;
    /* Settings menu */
  case SETTINGS_PREFERENCES:
    simbar->cfg_clicked();
    break;
    /* Help menu */
  case HELP_ABOUT:
    simbar->about_clicked();
    break;
  default:
    break;
  }
  //  cout << item_id << endl;
}
