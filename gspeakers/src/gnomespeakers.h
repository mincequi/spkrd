/* jagsmm.h
 *
 * Copyright (C) 2001 Daniel Sundberg <dss@home.se>
 *
 * http://sumpan.campus.luth.se/jags
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

#ifndef __GNOMESPEAKERSAPP
#define __GNOMESPEAKERSAPP

#include <gnome--/app.h>
#include <gnome--/appbar.h>
#include <gtk--/box.h>
#include <gtk--/button.h>
#include <gtk--/main.h>
#include <gtk--/toolbar.h>
#include <gtk--/label.h>
#include <gtk--/statusbar.h>
#include <gtk--/paned.h>
#include <vector>
#include <list>


/*
 * This is the main application class. This will display window, menu, 
 * toolbar and statusbar.
 */
class GnomeSpeakersApp : public Gnome::App {
public:

  /*
   * Create a new Jags-application
   */
  GnomeSpeakersApp();
  virtual ~GnomeSpeakersApp();
  void run() {}

protected:
  Gtk::VBox vbox;
  Gtk::Statusbar sbar;
  Gtk::HPaned *paned;

  gint delete_event_impl(GdkEventAny* e);
};

#endif __GNOMESPEAKERSAPP
