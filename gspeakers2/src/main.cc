// // #include "gfilter.h"
#include <gtkmm/main.h>
#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdlib.h>
#include <locale.h>
#include "common.h"
#include "mainwindow.h"

int main (int argc, char *argv[])
{
  /* Initialize gettext */
#if ENABLE_NLS
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, GNOMELOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain(PACKAGE);
#endif

  Gtk::Main kit(argc, argv);
  //SpeakerList slist;
  //slist.speaker_list()->push_back(Speaker("Vifa P21WO-20-08"));
  //slist.speaker_list()->push_back(Speaker("Vifa P22WO-20-08"));
  //slist.speaker_list()->push_back(Speaker("Vifa P23WO-20-08"));
  //slist.speaker_list()->push_back(Speaker("Vifa P24WO-20-08"));
  //slist.speaker_list()->push_back(Speaker("Vifa P25WO-20-08"));
  //slist.to_xml("test.xml");
//  Speaker_ListStore etl;
//  CrossoverList clist;
//  Crossover c1;
//  (*c1.networks())[0].set_has_imp_corr(true);
//  clist.crossover_list()->push_back(c1);
//  (*c1.networks())[1].set_has_damp(true);
//  clist.crossover_list()->push_back(c1);
//  clist.to_xml("crossover1.xml");
  //clist.to_xml("crossover1.xml");
  
  /* Init CrossoverTreeView before CrossoverHistory so that CrossoverHistory can select a Crossover to the TreeView */
  //CrossoverTreeView ctv;
  
  try {
#ifdef TARGET_WIN32
    g_settings.load("gspeakers2.conf");
#else
    g_settings.load(Glib::get_home_dir() + "/.gspeakers/gspeakers2.conf");
#endif
  } catch (std::runtime_error e) {
#ifdef OUTPUT_DEBUG
    cout << "Main: " << e.what() << endl;
#endif
#ifdef TARGET_WIN32
    Gtk::MessageDialog md(_("No configuration file found!") + 
                          Glib::ustring("\n") + 
			  _("gspeakers2.conf created in current directory"), 
                          true, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
#else
    Gtk::MessageDialog md(_("No configuration file found!") + 
			  Glib::ustring("\n\n") + 
                          Glib::get_home_dir() + "/.gspeakers/gspeakers2.conf created", 
                          true, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
#endif
    md.run();
#ifdef TARGET_WIN32
    // Is touch a valid windows command???
    string s = "touch gspeakers2.conf";
    system(s.c_str());
    g_settings.load("gspeakers2.conf");
#else
    string s = "mkdir " + Glib::get_home_dir() + "/.gspeakers";
    system(s.c_str());
    s = "touch " + Glib::get_home_dir() + "/.gspeakers/gspeakers2.conf";
    system(s.c_str());
    g_settings.load(Glib::get_home_dir() + "/.gspeakers/gspeakers2.conf");
#endif
  }
  g_settings.defaultValueUnsignedInt("ToolbarStyle", Gtk::TOOLBAR_BOTH);
  /* Init BoxEditor before BoxHistory, this will make BoxHistory signal BoxEditor with selected box, which is nice (tm) */
  //BoxEditor be;
  //BoxHistory bh;
  
  /* Plot history will always be empty at startup for now */
  //PlotHistory ph;

  
  //GSpeakersBoxPlot gsbp;
  cout << _("Testing gettext") << endl;
  //SpeakerListSelector sls;
  MainWindow mw;
  //mw.show_all_children();
  //signal_plot_crossover();
  kit.run(mw);
//  cout << c1;
  /* save settings */
  try {
    g_settings.save();
  } catch (std::runtime_error e) {
#ifdef OUTPUT_DEBUG
    cout << "Main: " << e.what() << endl;
#endif
  }

  //Net n;
  //n.set_has_imp_corr(true);
  //n.set_has_damp(true);
  //Speaker s;
  //cout << n.to_SPICE(s) << endl;
  //printf("%e\n", 3.0);
  return 0;
}
/*
// test net
Net n1;
//  cout << n1 << "--------" << endl;
  n1.set_highpass_order(NET_ORDER_1ST);
  n1.set_lowpass_order(NET_ORDER_3RD);
  (*n1.parts())[0].set_value(4.7);
  (*n1.parts())[1].set_value(6.8);
  (*n1.parts())[2].set_value(3.3);
  (*n1.parts())[n1.get_lowpass_order() + 0].set_value(4.7);    
  cout << n1 << "--------" << endl;
  n1.set_lowpass_order(NET_ORDER_1ST);
  cout << n1 << "--------" << endl;
  n1.set_highpass_order(NET_NOT_PRESENT);
  n1.set_lowpass_order(NET_NOT_PRESENT);
  n1.set_lowpass_order(NET_ORDER_2ND);
  (*n1.parts())[1].set_value(6.8);
  cout << n1;
  
*/

/*
  vector<string> v;
  v.push_back("hejsan");
  v.push_back("halloe");
  v.push_back("halihalo");
  
  vector<string>::iterator iter = v.begin();
    
  for(
      vector<string>::iterator from = v.begin();
      from != v.end();
      ++from
     )
  {
    cout << *from << endl;
  }
  v.erase(iter + 1);
  for(
      vector<string>::iterator from = v.begin();
      from != v.end();
      ++from
     )
  {
    cout << *from << endl;
  }

  vector<string> v;
  v.push_back("hejsan");
  v.push_back("halloe");
  v.push_back("halihalo");
  
  vector<string>::iterator iter = v.begin();
    
  for(
      vector<string>::iterator from = v.begin();
      from != v.end();
      ++from
     )
  {
    cout << *from << endl;
  }
  cout << "---" << endl;
  iter = v.insert(iter + 1, "hej1");
  iter = v.insert(iter, "hej2");
  v.insert(iter, "hej3");
  for(
      vector<string>::iterator from = v.begin();
      from != v.end();
      ++from
     )
  {
    cout << *from << endl;
  }


*/
