/*
  $Id$

  crossoverpaned Copyright (C) 2002-2004 Daniel Sundberg

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

#include "crossoverpaned.h"

#define MENU_INDEX_SAVE 6
#define TOOLBAR_INDEX_SAVE 3

CrossoverPaned::CrossoverPaned() {
  m_tbar = NULL;
  g_settings.settings_changed.connect(mem_fun(*this, &CrossoverPaned::on_settings_changed));

  add1(m_crossover_notebook);
  g_settings.defaultValueUnsignedInt("CrossoverPaned1Position", 220);
  set_position(g_settings.getValueUnsignedInt("CrossoverPaned1Position"));
  add2(m_plot_notebook);
  //  m_hpaned2.add1(crossover_treeview);

  //  m_notebook_vbox.pack_start(m_plot_notebook);
  //  m_notebook_vbox.set_border_width(2);
  //  m_vpaned.add1(m_notebook_vbox);

  m_crossover_notebook.append_page(crossover_wizard, _("Wizard"));
  m_crossover_notebook.append_page(crossover_treeview, _("Filter"));
  m_crossover_notebook.append_page(crossover_history, _("History"));

  m_plot_notebook.append_page(filter_plot, _("Crossover freq resp"));
  m_plot_notebook.append_page(total_filter_plot, _("Total crossover freq resp"));
  m_plot_notebook.append_page(summed_freq_resp_plot, _("Total freq resp"));
  m_plot_notebook.append_page(crossover_image_view, _("Crossover network"));

  m_plot_notebook.set_scrollable();
  //  m_vpaned.add2(crossover_history);
  //  m_hpaned2.add2(m_vpaned);
  //  g_settings.defaultValueUnsignedInt("CrossoverPaned2Position", 220);
  //  m_hpaned2.set_position(g_settings.getValueUnsignedInt("CrossoverPaned2Position"));
  //  g_settings.defaultValueUnsignedInt("CrossoverPlotVPanedPosition", 220);
  //  m_vpaned.set_position(g_settings.getValueUnsignedInt("CrossoverPlotVPanedPosition"));

  signal_crossover_set_save_state.connect(mem_fun(*this, &CrossoverPaned::set_save_state));
}

void CrossoverPaned::select_first_crossover() { crossover_history.select_first_row(); }

CrossoverPaned::~CrossoverPaned() {
  g_settings.setValue("CrossoverPaned1Position", get_position());
  //  g_settings.setValue("CrossoverPaned2Position", m_hpaned2.get_position());
  //  g_settings.setValue("CrossoverPlotVPanedPosition", m_vpaned.get_position());
  try {
    g_settings.save();
  } catch (std::runtime_error e) {
#ifdef OUTPUT_DEBUG
    std::cout << "CrossoverPaned::~CrossoverPaned: " << e.what() << std::endl;
#endif
  }
}

Gtk::Menu& CrossoverPaned::get_menu() {
  Gtk::Menu::MenuList& menulist = m_menu.items();
  Gtk::Menu* new_crossover_submenu = manage(new Gtk::Menu());
  Gtk::Menu::MenuList& sub_menulist = new_crossover_submenu->items();

  /* New crossover submenu */
  Gtk::Widget* im = manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
  sub_menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("New _lowpass crossover"), *im,
      sigc::bind<int>(mem_fun(*this, &CrossoverPaned::on_new_crossover_menu_action),
                      CROSSOVER_TYPE_LOWPASS)));
  im = manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
  sub_menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("New _subsonic crossover"), *im,
      sigc::bind<int>(mem_fun(*this, &CrossoverPaned::on_new_crossover_menu_action),
                      CROSSOVER_TYPE_SUBSONIC)));
  im = manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
  sub_menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("New _highpass crossover"), *im,
      sigc::bind<int>(mem_fun(*this, &CrossoverPaned::on_new_crossover_menu_action),
                      CROSSOVER_TYPE_HIGHPASS)));
  im = manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
  sub_menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("New _2-way crossover"), *im,
      sigc::bind<int>(mem_fun(*this, &CrossoverPaned::on_new_crossover_menu_action),
                      CROSSOVER_TYPE_TWOWAY)));
  im = manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
  sub_menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("New 2._5-way crossover"), *im,
      sigc::bind<int>(mem_fun(*this, &CrossoverPaned::on_new_crossover_menu_action),
                      CROSSOVER_TYPE_LOWPASS | CROSSOVER_TYPE_TWOWAY)));
  im = manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
  sub_menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("New _3-way crossover"), *im,
      sigc::bind<int>(mem_fun(*this, &CrossoverPaned::on_new_crossover_menu_action),
                      CROSSOVER_TYPE_THREEWAY)));
  im = manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
  sub_menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("New _4-way crossover"), *im,
      sigc::bind<int>(mem_fun(*this, &CrossoverPaned::on_new_crossover_menu_action),
                      CROSSOVER_TYPE_FOURWAY)));
  im = manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
  menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_New crossover"), *new_crossover_submenu));

  im = manage(new Gtk::Image(Gtk::Stock::COPY, Gtk::ICON_SIZE_MENU));
  menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("_Copy"), *im, mem_fun(crossover_history, &CrossoverHistory::on_new_copy)));

  menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

  im = manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
  menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("New"), *im, mem_fun(crossover_history, &CrossoverHistory::on_new_xml)));

  menulist.push_back(Gtk::Menu_Helpers::MenuElem(
      _("A_ppend Crossover Xml..."), mem_fun(crossover_history, &CrossoverHistory::on_append_xml)));

  im = manage(new Gtk::Image(Gtk::Stock::OPEN, Gtk::ICON_SIZE_MENU));
  menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("_Open"), *im, mem_fun(crossover_history, &CrossoverHistory::on_open_xml)));

  menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

  im = manage(new Gtk::Image(Gtk::Stock::SAVE, Gtk::ICON_SIZE_MENU));
  menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("_Save"), *im, mem_fun(crossover_history, &CrossoverHistory::on_save)));

  im = manage(new Gtk::Image(Gtk::Stock::SAVE_AS, Gtk::ICON_SIZE_MENU));
  menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("Save _As"), *im, mem_fun(crossover_history, &CrossoverHistory::on_save_as)));

  menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

  im = manage(new Gtk::Image(Gtk::Stock::DELETE, Gtk::ICON_SIZE_MENU));
  menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("_Delete"), *im, mem_fun(crossover_history, &CrossoverHistory::on_remove)));

  menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

  menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      _("_Plot"), GSpeakers::image_widget("stock_plot_crossover_16.png"),
      mem_fun(*this, &CrossoverPaned::on_plot_crossover)));

  menulist[MENU_INDEX_SAVE].set_sensitive(false);
  // menulist[MENU_INDEX_DELETE].set_sensitive(false);
  return m_menu;
}

Gtk::Widget& CrossoverPaned::get_toolbar() {
  if (m_tbar == NULL) {
    m_tbar = manage(new Gtk::Toolbar());

    Gtk::Widget* im = manage(new Gtk::Image(Gtk::Stock::COPY, Gtk::ICON_SIZE_LARGE_TOOLBAR));
    Gtk::ToolButton* t = manage(new Gtk::ToolButton(*im, _("Copy")));
    t->signal_clicked().connect(mem_fun(crossover_history, &CrossoverHistory::on_new_copy));
    m_tbar->append(*t);

    Gtk::SeparatorToolItem* s = manage(new Gtk::SeparatorToolItem());
    m_tbar->append(*s);

    im = manage(new Gtk::Image(Gtk::Stock::OPEN, Gtk::ICON_SIZE_LARGE_TOOLBAR));
    t = manage(new Gtk::ToolButton(*im, _("Open")));
    t->signal_clicked().connect(mem_fun(crossover_history, &CrossoverHistory::on_open_xml));
    m_tbar->append(*t);

    im = manage(new Gtk::Image(Gtk::Stock::SAVE, Gtk::ICON_SIZE_LARGE_TOOLBAR));
    t = manage(new Gtk::ToolButton(*im, _("Save")));
    t->signal_clicked().connect(mem_fun(crossover_history, &CrossoverHistory::on_save));
    m_tbar->append(*t);

    s = manage(new Gtk::SeparatorToolItem());
    m_tbar->append(*s);

    im = manage(new Gtk::Image(Gtk::Stock::DELETE, Gtk::ICON_SIZE_LARGE_TOOLBAR));
    t = manage(new Gtk::ToolButton(*im, _("Delete")));
    t->signal_clicked().connect(mem_fun(crossover_history, &CrossoverHistory::on_remove));
    m_tbar->append(*t);

    s = manage(new Gtk::SeparatorToolItem());
    m_tbar->append(*s);

    t = manage(
        new Gtk::ToolButton(GSpeakers::image_widget("stock_plot_crossover_24.png"), _("Plot")));
    t->signal_clicked().connect(mem_fun(*this, &CrossoverPaned::on_plot_crossover));
    m_tbar->append(*t);

    m_toolbar.add(*m_tbar);
    m_tbar->set_toolbar_style((Gtk::ToolbarStyle)g_settings.getValueUnsignedInt("ToolbarStyle"));
    m_tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(false);
    // tbar->tools()[TOOLBAR_INDEX_DELETE].get_widget()->set_sensitive(false);
    g_settings.settings_changed.connect(mem_fun(*this, &CrossoverPaned::on_settings_changed));
  }
  return m_toolbar;
}

void CrossoverPaned::on_plot_crossover() {
  filter_plot.clear();
  signal_plot_crossover();
}

void CrossoverPaned::on_settings_changed(const std::string& s) {
  if (s == "ToolbarStyle") {
    m_tbar->set_toolbar_style((Gtk::ToolbarStyle)g_settings.getValueUnsignedInt("ToolbarStyle"));
  }
  if (s == "AutoUpdateFilterPlots") {
    if (g_settings.getValueBool("AutoUpdateFilterPlots") == true) {
      on_plot_crossover();
    }
  }
}

void CrossoverPaned::on_new_crossover_menu_action(int i) { signal_new_crossover(i); }

void CrossoverPaned::set_save_state(bool b) {
  if (m_tbar != NULL) {
    m_tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(b);
  }
  if (m_menu.items().size() > 0) {
    m_menu.items()[MENU_INDEX_SAVE].set_sensitive(b);
    GSpeakers::crossoverlist_modified() = b;
  }
}
