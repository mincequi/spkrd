/* gspeakersmenu.h
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

#ifndef __GSPEAKERSMENU_H
#define __GSPEAKERSMENU_H

#include <gtk--/menubar.h>
#include "speakertoolbar.h"
#include "boxtoolbar.h"
#include "simtoolbar.h"
#include "gspeakerscfg.h"

#define FILE_QUIT                       1

#define SPEAKER_NEW                    10
#define SPEAKER_NEW_XML                11
#define SPEAKER_OPEN                   12
#define SPEAKER_SAVE                   13
#define SPEAKER_SAVE_AS                14
#define SPEAKER_REMOVE                 15
#define SPEAKER_IMPORT                 16

#define BOX_NEW                        20
#define BOX_NEW_XML                    21
#define BOX_OPEN                       22
#define BOX_SAVE                       23
#define BOX_SAVE_AS                    24
#define BOX_REMOVE                     25

#define SIMULATE_PLOT                  30
#define SIMULATE_PLOT_MBOXES           31
#define SIMULATE_PLOT_MSPEAKERS        32
#define SIMULATE_PLOT_MBOXES_MSPEAKERS 33
#define SIMULATE_PLOT_REMOVE           34
#define SIMULATE_PLOT_REMOVE_ALL       35

#define OPTIMIZE_VOLUME                40

#define SETTINGS_PREFERENCES           50

#define HELP_ABOUT                     90



class GSpeakersMenu : public Gtk::MenuBar {
 public:
  GSpeakersMenu( SpeakerToolbar *isbar, BoxToolbar *ibbar, SimToolbar *isimbar, 
		 GSpeakersCFG *icfg );
  ~GSpeakersMenu();

  void item_activated_cb( int item_id );
 private:
  SpeakerToolbar *sbar;
  BoxToolbar *bbar;
  SimToolbar *simbar;
  GSpeakersCFG *cfg;
};

#endif
