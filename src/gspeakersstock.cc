/*
 * $Id$
 *
 * Copyright (C) 2004 Daniel Sundberg <dss@home.se>
 * Copyright (C) 2003 Imendio HB
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "gspeakersstock.h"

#include <gtk/gtk.h>

// TODO Use C++ bindings for these
const GtkStockItem stock_items[] = {{const_cast<char*>(GSPEAKERS_STOCK_DRIVER), nullptr},
                                    {const_cast<char*>(GSPEAKERS_STOCK_SPEAKER), nullptr},
                                    {const_cast<char*>(GSPEAKERS_STOCK_FILTER), nullptr}};

void gspeakers_stock_init() {

  gtk_stock_add(stock_items, G_N_ELEMENTS(stock_items));

  GtkIconFactory* icon_factory = gtk_icon_factory_new();
  gtk_icon_factory_add_default(icon_factory);

  for (const auto& stock_item : stock_items) {
    gchar* filename =
        g_strdup_printf(GSPEAKERS_PREFIX "/share/pixmaps/%s.png", stock_item.stock_id);
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(filename, nullptr);
    g_free(filename);

    GtkIconSet* icon_set = gtk_icon_set_new_from_pixbuf(pixbuf);

    gtk_icon_factory_add(icon_factory, stock_item.stock_id, icon_set);

    g_object_unref(pixbuf);
  }
}
