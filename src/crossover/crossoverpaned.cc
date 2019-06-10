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

constexpr auto MENU_INDEX_SAVE = 6;
constexpr auto TOOLBAR_INDEX_SAVE = 3;

CrossoverPaned::CrossoverPaned() {
  m_tbar = nullptr;
  g_settings.settings_changed.connect(sigc::mem_fun(*this, &CrossoverPaned::on_settings_changed));

  add1(m_crossover_notebook);
  g_settings.defaultValueUnsignedInt("CrossoverPaned1Position", 220);
  set_position(g_settings.getValueUnsignedInt("CrossoverPaned1Position"));
  add2(m_plot_notebook);

  m_crossover_notebook.append_page(crossover_wizard, _("Wizard"));
  m_crossover_notebook.append_page(crossover_treeview, _("Filter"));
  m_crossover_notebook.append_page(crossover_history, _("History"));

  m_plot_notebook.append_page(filter_plot, _("Crossover freq resp"));
  m_plot_notebook.append_page(total_filter_plot, _("Total crossover freq resp"));
  m_plot_notebook.append_page(summed_freq_resp_plot, _("Total freq resp"));
  m_plot_notebook.append_page(crossover_image_view, _("Crossover network"));

  m_plot_notebook.set_scrollable();

  signal_crossover_set_save_state.connect(sigc::mem_fun(*this, &CrossoverPaned::set_save_state));
}

void CrossoverPaned::select_first_crossover() { crossover_history.select_first_row(); }

CrossoverPaned::~CrossoverPaned() {

  g_settings.setValue("CrossoverPaned1Position", get_position());

  try {
    g_settings.save();
  } catch (std::runtime_error const& error) {
    std::cout << "CrossoverPaned::~CrossoverPaned: " << error.what() << "\n";
  }
}

Gtk::Menu& CrossoverPaned::get_menu() {

  // Gtk::Menu* new_crossover_submenu = Gtk::manage(new Gtk::Menu());

  // Create new crossover submenu
  {
    Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
    // new_crossover_submenu->attach(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("New _lowpass crossover"), *im,
    //     sigc::bind(sigc::mem_fun(*this, &CrossoverPaned::on_new_crossover_menu_action),
    //                CROSSOVER_TYPE_LOWPASS)));
  }
  {
    Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
    // new_crossover_submenu->attach(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("New _subsonic crossover"), *im,
    //     sigc::bind(sigc::mem_fun(*this, &CrossoverPaned::on_new_crossover_menu_action),
    //                CROSSOVER_TYPE_SUBSONIC)));
  }
  {
    Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
    // new_crossover_submenu->attach(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("New _highpass crossover"), *im,
    //     sigc::bind(sigc::mem_fun(*this, &CrossoverPaned::on_new_crossover_menu_action),
    //                CROSSOVER_TYPE_HIGHPASS)));
  }
  {
    Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
    // new_crossover_submenu->attach(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("New _2-way crossover"), *im,
    //     sigc::bind(sigc::mem_fun(*this, &CrossoverPaned::on_new_crossover_menu_action),
    //                CROSSOVER_TYPE_TWOWAY)));
  }
  {
    Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
    // new_crossover_submenu->attach(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("New 2._5-way crossover"), *im,
    //     sigc::bind(sigc::mem_fun(*this, &CrossoverPaned::on_new_crossover_menu_action),
    //                CROSSOVER_TYPE_LOWPASS | CROSSOVER_TYPE_TWOWAY)));
  }
  {
    Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
    // new_crossover_submenu->attach(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("New _3-way crossover"), *im,
    //     sigc::bind(sigc::mem_fun(*this, &CrossoverPaned::on_new_crossover_menu_action),
    //                CROSSOVER_TYPE_THREEWAY)));
  }
  {
    Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
    // new_crossover_submenu->attach(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("New _4-way crossover"), *im,
    //     sigc::bind(sigc::mem_fun(*this, &CrossoverPaned::on_new_crossover_menu_action),
    //                CROSSOVER_TYPE_FOURWAY)));
  }

  // Gtk::Menu::MenuList& menulist = m_menu.items();

  // m_menu.attach(Gtk::Menu_Helpers::MenuElem(_("_New crossover"), *new_crossover_submenu));

  {
      // Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::COPY, Gtk::ICON_SIZE_MENU));
      // menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
      //     _("_Copy"), *im, sigc::mem_fun(crossover_history, &CrossoverHistory::on_new_copy)));
  }

  // menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

  {
    Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
    // menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("New"), *im, sigc::mem_fun(crossover_history, &CrossoverHistory::on_new_xml)));
  }

  // menulist.push_back(Gtk::Menu_Helpers::MenuElem(
  // _("A_ppend Crossover Xml..."),
  // sigc::mem_fun(crossover_history, &CrossoverHistory::on_append_xml)));

  {
    Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::OPEN, Gtk::ICON_SIZE_MENU));
    // menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("_Open"), *im, sigc::mem_fun(crossover_history, &CrossoverHistory::on_open_xml)));
  }

  // menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

  {
    Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::SAVE, Gtk::ICON_SIZE_MENU));
    // menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("_Save"), *im, sigc::mem_fun(crossover_history, &CrossoverHistory::on_save)));
  }
  {
    Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::SAVE_AS, Gtk::ICON_SIZE_MENU));
    // menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
    //     _("Save _As"), *im, sigc::mem_fun(crossover_history, &CrossoverHistory::on_save_as)));
  }

  // menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

  Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::DELETE, Gtk::ICON_SIZE_MENU));
  // menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
  // _("_Delete"), *im, sigc::mem_fun(crossover_history, &CrossoverHistory::on_remove)));

  // menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

  // menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(
  // _("_Plot"), GSpeakers::image_widget("stock_plot_crossover_16.png"),
  // sigc::mem_fun(*this, &CrossoverPaned::on_plot_crossover)));

  // menulist[MENU_INDEX_SAVE].set_sensitive(false);

  return m_menu;
}

Gtk::Widget& CrossoverPaned::get_toolbar() {
  if (m_tbar == nullptr) {

    m_tbar = Gtk::manage(new Gtk::Toolbar());

    Gtk::Widget* im = Gtk::manage(new Gtk::Image(Gtk::Stock::COPY, Gtk::ICON_SIZE_LARGE_TOOLBAR));
    Gtk::ToolButton* t = Gtk::manage(new Gtk::ToolButton(*im, _("Copy")));
    t->signal_clicked().connect(sigc::mem_fun(crossover_history, &CrossoverHistory::on_new_copy));

    m_tbar->append(*t);

    Gtk::SeparatorToolItem* s = Gtk::manage(new Gtk::SeparatorToolItem());
    m_tbar->append(*s);

    im = Gtk::manage(new Gtk::Image(Gtk::Stock::OPEN, Gtk::ICON_SIZE_LARGE_TOOLBAR));
    t = Gtk::manage(new Gtk::ToolButton(*im, _("Open")));
    t->signal_clicked().connect(sigc::mem_fun(crossover_history, &CrossoverHistory::on_open_xml));
    m_tbar->append(*t);

    im = Gtk::manage(new Gtk::Image(Gtk::Stock::SAVE, Gtk::ICON_SIZE_LARGE_TOOLBAR));
    t = Gtk::manage(new Gtk::ToolButton(*im, _("Save")));
    t->signal_clicked().connect(sigc::mem_fun(crossover_history, &CrossoverHistory::on_save));
    m_tbar->append(*t);

    s = Gtk::manage(new Gtk::SeparatorToolItem());
    m_tbar->append(*s);

    im = Gtk::manage(new Gtk::Image(Gtk::Stock::DELETE, Gtk::ICON_SIZE_LARGE_TOOLBAR));
    t = Gtk::manage(new Gtk::ToolButton(*im, _("Delete")));
    t->signal_clicked().connect(sigc::mem_fun(crossover_history, &CrossoverHistory::on_remove));
    m_tbar->append(*t);

    s = Gtk::manage(new Gtk::SeparatorToolItem());
    m_tbar->append(*s);

    t = Gtk::manage(
        new Gtk::ToolButton(GSpeakers::image_widget("stock_plot_crossover_24.png"), _("Plot")));
    t->signal_clicked().connect(sigc::mem_fun(*this, &CrossoverPaned::on_plot_crossover));
    m_tbar->append(*t);

    m_toolbar.add(*m_tbar);
    m_tbar->set_toolbar_style((Gtk::ToolbarStyle)g_settings.getValueUnsignedInt("ToolbarStyle"));
    m_tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(false);

    g_settings.settings_changed.connect(sigc::mem_fun(*this, &CrossoverPaned::on_settings_changed));
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
  if (s == "AutoUpdateFilterPlots" && g_settings.getValueBool("AutoUpdateFilterPlots")) {
    on_plot_crossover();
  }
}

void CrossoverPaned::on_new_crossover_menu_action(int i) { signal_new_crossover(i); }

void CrossoverPaned::set_save_state(bool b) {
  if (m_tbar != nullptr) {
    m_tbar->get_nth_item(TOOLBAR_INDEX_SAVE)->set_sensitive(b);
  }
  // if (m_menu.items().size() > 0) {
  //   m_menu.items()[MENU_INDEX_SAVE].set_sensitive(b);
  //   GSpeakers::crossoverlist_modified() = b;
  // }
}
