/*
  speakereditor Copyright (C) 2002 Daniel Sundberg

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

#include <fstream>
#include <math.h>
#include "speakereditor.h"
#include "common.h"
#include "freqrespeditor.h"

Speaker_ListStore::Speaker_ListStore()
: m_TreeViewTable(10, 4, true),
  m_Table(20, 2, true), 
  m_NewButton("New Speaker"), 
  //m_NewButton(), 
  m_NewXmlButton("New Xml"), 
  m_AppendXmlButton("Append xml..."), 
  m_OpenXmlButton("Open..."), 
  m_CloseButton("Close"), 
  m_SaveButton("Save"),
  m_SaveAsButton("Save As..."),
  m_RemoveButton("Delete"),
  m_EditFreqRespButton("Edit..."),
  m_BrowseFreqRespButton("..."),

  m_IdStringEntry(), 
  
  m_QtsEntry(), 
  m_FsEntry(), 
  m_VasEntry(), 
  m_RdcEntry(), 
  m_LvcEntry(), 
  m_QmsEntry(), 
  m_QesEntry(),
  m_ImpEntry(), 
  m_SensEntry(),
  m_FreqRespFileEntry(),
  m_BassCheckButton("Bass"), 
  m_MidrangeCheckButton("Midrange"), 
  m_TweeterCheckButton("Tweeter"),
  m_MmdEntry(),
  m_AdEntry(),
  m_BlEntry(),
  m_RmsEntry(),
  m_CmsEntry(),
  m_vbox(),
  
  plot()
#ifdef ENABLE_BROWSE_FILE
  m_FreqRespFileEntry(), 
  m_ImpRespFileEntry(),
#endif  
  
{
  g_settings.defaultValueString("SpeakerListXml", string(GSPEAKERS_PREFIX) + "/share/xml/vifa.xml");
  m_filename = g_settings.getValueString("SpeakerListXml");
  signal_speakerlist_loaded(m_filename);
  index = 0;
  changed = false;
  m_speaker_list = new SpeakerList(m_filename);
  m_treeview_vbox.set_border_width(5);
  m_treeview_vbox.pack_start(m_treeview_frame);
  m_treeview_frame.add(m_inner_treeview_vbox);
  m_inner_treeview_vbox.set_border_width(5);
  m_inner_treeview_vbox.pack_start(m_TreeViewTable);
  m_treeview_frame.set_label(m_filename);
  
  //m_vbox.pack_start(m_Table);
  m_vbox.set_border_width(5);
  m_Table.set_spacings(2);
  m_vbox.pack_start(m_editor_frame);
  m_editor_frame.add(m_inner_vbox);
  m_inner_vbox.set_border_width(5);
  m_inner_vbox.pack_start(m_Table);
  m_editor_frame.set_label("Currently selected driver");
  
  //Glib::RefPtr<Gdk::Pixbuf> icon = Gdk::Pixbuf::create_from_file("../pixmaps/new_speaker.png");
  //Gtk::Image *i = manage(new Gtk::Image(icon));
  //Glib::RefPtr<Gdk::Pixmap> ref_pixmap;
  //Glib::RefPtr<Gdk::Bitmap> ref_bitmap;
  //i->get_pixmap(ref_pixmap, ref_bitmap);
  //cout << ref_pixmap << ", " << ref_bitmap << endl;
  //m_NewButton.add_pixmap(ref_pixmap, ref_bitmap);
  
  /*
  Gtk::Toolbar *toolbar = manage(new Gtk::Toolbar());
  Gtk::HandleBox *handlebox = manage(new Gtk::HandleBox());
  toolbar->tools().push_back(Gtk::Toolbar_Helpers::ButtonElem("New driver", 
                                                              *manage(new Gtk::Image(Gdk::Pixbuf::create_from_file("../pixmaps/driver_small.png"))), 
                                                              slot(*this, &Speaker_ListStore::on_new), "New driver", ""));
  toolbar->tools().push_back(Gtk::Toolbar_Helpers::Space());
  toolbar->tools().push_back(Gtk::Toolbar_Helpers::ButtonElem("New xml", 
                                                              *manage(new Gtk::Image(Gdk::Pixbuf::create_from_file("../pixmaps/driver_small.png"))), 
                                                              slot(*this, &Speaker_ListStore::on_new), "New driver xml", ""));
  toolbar->set_toolbar_style(Gtk::TOOLBAR_ICONS);
  handlebox->add(*toolbar);
  m_TreeViewTable.attach(*handlebox, 0, 4, 8, 9);
  */
  /* Setup the table */
  m_TreeViewTable.set_spacings(2);
  m_TreeViewTable.attach(m_ScrolledWindow, 0, 4, 0, 8);

  m_TreeViewTable.attach(m_NewButton       , 0, 1, 8, 9);
  m_TreeViewTable.attach(m_NewXmlButton    , 1, 2, 8, 9);
  m_TreeViewTable.attach(m_AppendXmlButton , 2, 3, 8, 9);
  m_TreeViewTable.attach(m_OpenXmlButton   , 3, 4, 8, 9);
  m_TreeViewTable.attach(m_RemoveButton    , 0, 1, 9, 10);
  m_TreeViewTable.attach(m_SaveButton      , 1, 2, 9, 10);
  m_TreeViewTable.attach(m_SaveAsButton    , 2, 3, 9, 10);
  //m_TreeViewTable.attach(m_CloseButton     , 3, 4, 9, 10);
  
  /* All the entries and stuff */
  m_Table.attach(*manage(new Gtk::Label("Speaker name:", Gtk::ALIGN_LEFT)), 0, 1, 0, 1);
  m_Table.attach(m_IdStringEntry, 1, 3, 0, 1);
  m_Table.attach(*manage(new Gtk::Label("Qts:", Gtk::ALIGN_LEFT)), 0, 1, 1, 2, Gtk::FILL|Gtk::EXPAND|Gtk::SHRINK);
  m_Table.attach(m_QtsEntry, 1, 3, 1, 2);
  m_Table.attach(*manage(new Gtk::Label("Fs:", Gtk::ALIGN_LEFT)), 0, 1, 2, 3);
  m_Table.attach(m_FsEntry, 1, 3, 2, 3);
  m_Table.attach(*manage(new Gtk::Label("Vas: (l)", Gtk::ALIGN_LEFT)), 0, 1, 3, 4);
  m_Table.attach(m_VasEntry, 1, 3, 3, 4);
  m_Table.attach(*manage(new Gtk::Label("Rdc:", Gtk::ALIGN_LEFT)), 0, 1, 4, 5);
  m_Table.attach(m_RdcEntry, 1, 3, 4, 5);
  m_Table.attach(*manage(new Gtk::Label("Lvc:", Gtk::ALIGN_LEFT)), 0, 1, 5, 6);
  m_Table.attach(m_LvcEntry, 1, 3, 5, 6);
  m_Table.attach(*manage(new Gtk::Label("Qms:", Gtk::ALIGN_LEFT)), 0, 1, 6, 7);
  m_Table.attach(m_QmsEntry, 1, 3, 6, 7);
  m_Table.attach(*manage(new Gtk::Label("Qes:", Gtk::ALIGN_LEFT)), 0, 1, 7, 8);
  m_Table.attach(m_QesEntry, 1, 3, 7, 8);
  m_Table.attach(*manage(new Gtk::Label("Impedance:", Gtk::ALIGN_LEFT)), 0, 1, 8, 9);
  m_Table.attach(m_ImpEntry, 1, 3, 8, 9);
  m_Table.attach(*manage(new Gtk::Label("Sensitivity:", Gtk::ALIGN_LEFT)), 0, 1, 9, 10);
  m_Table.attach(m_SensEntry, 1, 3, 9, 10);

  m_Table.attach(*manage(new Gtk::Label("Cone mass: (kg)", Gtk::ALIGN_LEFT)), 0, 1, 10, 11);
  m_Table.attach(m_MmdEntry, 1, 3, 10, 11);
  m_Table.attach(*manage(new Gtk::Label("Effective radius: (m)", Gtk::ALIGN_LEFT)), 0, 1, 11, 12);
  m_Table.attach(m_AdEntry, 1, 3, 11, 12);
  m_Table.attach(*manage(new Gtk::Label("Force factor:", Gtk::ALIGN_LEFT)), 0, 1, 12, 13);
  m_Table.attach(m_BlEntry, 1, 3, 12, 13);
  m_Table.attach(*manage(new Gtk::Label("Susp. resistance:", Gtk::ALIGN_LEFT)), 0, 1, 13, 14);
  m_Table.attach(m_RmsEntry, 1, 3, 13, 14);
  m_Table.attach(*manage(new Gtk::Label("Susp. compleance:", Gtk::ALIGN_LEFT)), 0, 1, 14, 15);
  m_Table.attach(m_CmsEntry, 1, 3, 14, 15);

  m_Table.attach(m_BassCheckButton, 0, 3, 15, 16);
  m_Table.attach(m_MidrangeCheckButton, 0, 3, 16, 17);
  m_Table.attach(m_TweeterCheckButton, 0, 3, 17, 18);

  m_Table.attach(*manage(new Gtk::Label("Freq resp file:", Gtk::ALIGN_LEFT)), 0, 1, 18, 19);
  m_Table.attach(m_FreqRespFileEntry, 1, 2, 18, 19);
  Gtk::HBox *vbox = manage(new Gtk::HBox());
  vbox->pack_start(m_BrowseFreqRespButton);
  vbox->pack_start(m_EditFreqRespButton);
  vbox->set_spacing(3);
  m_Table.attach(*vbox, 2, 3, 18, 19);
  
  m_NewXmlButton.signal_clicked().connect(slot(*this, &Speaker_ListStore::on_new_xml));
  m_NewButton.signal_clicked().connect(slot(*this, &Speaker_ListStore::on_new));
  m_AppendXmlButton.signal_clicked().connect(slot(*this, &Speaker_ListStore::on_append_xml));
  m_OpenXmlButton.signal_clicked().connect(slot(*this, &Speaker_ListStore::on_open_xml));
  m_CloseButton.signal_clicked().connect(slot(*this, &Speaker_ListStore::on_close));
  m_SaveButton.signal_clicked().connect(slot(*this, &Speaker_ListStore::on_save));
  m_SaveAsButton.signal_clicked().connect(slot(*this, &Speaker_ListStore::on_save_as));
  m_RemoveButton.signal_clicked().connect(slot(*this, &Speaker_ListStore::on_remove));
  m_BrowseFreqRespButton.signal_clicked().connect(slot(*this, &Speaker_ListStore::on_browse_freq_resp));
  m_EditFreqRespButton.signal_clicked().connect(slot(*this, &Speaker_ListStore::on_edit_freq_resp));
  
  m_ScrolledWindow.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  m_IdStringEntry.signal_changed().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 0));
  m_QtsEntry.signal_changed().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 1));
  m_FsEntry.signal_changed().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 2));
  m_VasEntry.signal_changed().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 3));
  m_RdcEntry.signal_changed().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 4));
  m_LvcEntry.signal_changed().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 5));
  m_QmsEntry.signal_changed().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 6));
  m_QesEntry.signal_changed().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 7));
  m_ImpEntry.signal_changed().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 8));
  m_SensEntry.signal_changed().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 9));
  m_MmdEntry.signal_changed().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 13));
  m_AdEntry.signal_changed().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 14));
  m_BlEntry.signal_changed().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 15));
  m_RmsEntry.signal_changed().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 16));
  m_CmsEntry.signal_changed().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 17));
  m_FreqRespFileEntry.signal_changed().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 18));
  m_FreqRespFileEntry.set_sensitive(false);
  
  m_BassCheckButton.signal_toggled().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 10));
  m_MidrangeCheckButton.signal_toggled().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 11));
  m_TweeterCheckButton.signal_toggled().connect(bind<int>(slot(*this, &Speaker_ListStore::on_entry_changed), 12));

  set_entries_sensitive(false);
  m_AppendXmlButton.set_sensitive(true);
  m_SaveAsButton.set_sensitive(false);
  m_SaveButton.set_sensitive(false);
  m_RemoveButton.set_sensitive(false);
  
  /* create model */
  create_model();

  /* create tree view */
  m_TreeView.set_model(m_refListStore);
  m_TreeView.set_rules_hint();
  //m_TreeView.set_search_column(m_columns.id.index());
  Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView.get_selection();
  //selection->set_mode(Gtk::SELECTION_MULTIPLE);
  selection->signal_changed().connect(slot(*this, &Speaker_ListStore::on_selection_changed));

  add_columns();
  m_ScrolledWindow.add(m_TreeView);
  f_append = NULL;
  f_open = NULL;
  f_save_as = NULL;
  new_xml_pressed = false;
}

Speaker_ListStore::~Speaker_ListStore()
{
}

Gtk::Widget& Speaker_ListStore::get_treeview_table()
{
  return m_treeview_vbox;
}

Gtk::Widget& Speaker_ListStore::get_editor_table()
{
  return m_vbox;
}

Gtk::Widget& Speaker_ListStore::get_plot()
{
  return plot;
}

void Speaker_ListStore::on_close()
{
  signal_speakerlist_loaded(m_filename);
}

void Speaker_ListStore::set_entries_sensitive(bool value)
{
  m_IdStringEntry.set_sensitive(value);
  m_QtsEntry.set_sensitive(value);
  m_FsEntry.set_sensitive(value);
  m_VasEntry.set_sensitive(value);
  m_RdcEntry.set_sensitive(value);
  m_LvcEntry.set_sensitive(value);
  m_QmsEntry.set_sensitive(value);
  m_QesEntry.set_sensitive(value);
  m_ImpEntry.set_sensitive(value);
  m_SensEntry.set_sensitive(value);
  m_MmdEntry.set_sensitive(value);
  m_AdEntry.set_sensitive(value);
  m_BlEntry.set_sensitive(value);
  m_RmsEntry.set_sensitive(value);
  m_CmsEntry.set_sensitive(value);
    
  //m_FreqRespFileEntry.set_sensitive(value);
  m_ImpRespFileEntry.set_sensitive(value);
  m_BassCheckButton.set_sensitive(value); 
  m_MidrangeCheckButton.set_sensitive(value); 
  m_TweeterCheckButton.set_sensitive(value);
  m_BrowseFreqRespButton.set_sensitive(value);
  m_EditFreqRespButton.set_sensitive(value);
  
  if (value == false) {
    m_BassCheckButton.set_active(false); 
    m_MidrangeCheckButton.set_active(false); 
    m_TweeterCheckButton.set_active(false);
  }
}

void Speaker_ListStore::on_new()
{
  //cout << "new speaker" << endl;
  Speaker s("New speaker");
  liststore_add_item(s);
  m_speaker_list->speaker_list()->push_back(s);
  
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();
 
  char *str = NULL;
  GString *buffer = g_string_new(str);
  g_string_printf(buffer, "%d", m_speaker_list->speaker_list()->size() - 1);
  GtkTreePath *gpath = gtk_tree_path_new_from_string(buffer->str);
  Gtk::TreePath path(gpath);
  Gtk::TreeRow row = *(m_refListStore->get_iter(path));
  refSelection->select(row);
  
  m_IdStringEntry.grab_focus();
  set_entries_sensitive(true);
  m_SaveButton.set_sensitive(true);
  m_SaveAsButton.set_sensitive(true);
  m_AppendXmlButton.set_sensitive(true);
  m_RemoveButton.set_sensitive(true);
}

void Speaker_ListStore::on_new_xml()
{
  //cout << "new xml" << endl;
  m_refListStore->clear();
  m_speaker_list->clear();
  set_entries_sensitive(false);
  m_AppendXmlButton.set_sensitive(true);
  m_RemoveButton.set_sensitive(false);
  new_xml_pressed = true;
}

void Speaker_ListStore::on_remove()
{
   Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  if(const Gtk::TreeIter iter = refSelection->get_selected())
  {
    Gtk::TreePath path = m_refListStore->get_path(iter);

    std::vector<int> indices = path.get_indices();
    if(indices.size() > 0)
    {
      //Remove item from ListStore:
      m_refListStore->erase(iter);

      if(index < (int)m_speaker_list->speaker_list()->size())
        m_speaker_list->speaker_list()->erase(m_speaker_list->speaker_list()->begin() + index);
    }
  } 
}

void Speaker_ListStore::on_save()
{
#ifdef OUTPUT_DEBUG
  cout << "SpeakerEditor: save" << endl;
#endif
  if (new_xml_pressed == true) {
    on_save_as();
    new_xml_pressed = false;
  } else {
#ifdef OUTPUT_DEBUG
    cout << "SpeakerEditor: Filename = " << m_filename << endl;
#endif
    try {
      m_speaker_list->to_xml(m_filename);
      m_SaveButton.set_sensitive(false);
    } catch (GSpeakersException e) {
      Gtk::MessageDialog m(e.what(), Gtk::MESSAGE_ERROR);
      m.run();
    }
  }
}

void Speaker_ListStore::on_save_as()
{
  //cout << "save as" << endl;
  if (f_save_as == NULL) {
    f_save_as = new Gtk::FileSelection("Save speaker xml as");
    f_save_as->get_ok_button()->signal_clicked().connect(bind<Gtk::FileSelection *>(slot(*this, &Speaker_ListStore::on_save_as_ok), f_save_as));
    f_save_as->get_cancel_button()->signal_clicked().connect(slot(*f_save_as, &Gtk::Widget::hide));
  } else {
    f_save_as->set_title("Save speaker xml as");
    f_save_as->show();
  }
  f_save_as->run();
}

void Speaker_ListStore::on_save_as_ok(Gtk::FileSelection *f)
{
  //cout << "save as ok" << endl;
  try {
    m_speaker_list->to_xml(f->get_filename());
    f->hide();
    m_filename = f->get_filename();
    signal_speakerlist_loaded(m_filename);
  } catch (GSpeakersException e) {
      Gtk::MessageDialog m(e.what(), Gtk::MESSAGE_ERROR);
      m.run();
  }
}

void Speaker_ListStore::on_selection_changed()
{
  //cout << "selection changed" << endl;

  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();

  if(const Gtk::TreeIter iter = refSelection->get_selected())
  {
    Gtk::TreePath path = m_refListStore->get_path(iter);

    std::vector<int> indices = path.get_indices();
    if (indices.size() > 0) {
      index = indices[0];
      Speaker s = (*(m_speaker_list->speaker_list()))[index];
  
      m_IdStringEntry.set_text(Glib::ustring(s.get_id_string()));
      char *str = NULL;
      GString *buffer = g_string_new(str);
      g_string_printf(buffer, "%3.3f", s.get_qts());
      m_QtsEntry.set_text(Glib::ustring(buffer->str));
      g_string_printf(buffer, "%3.0f", s.get_fs());
      m_FsEntry.set_text(Glib::ustring(buffer->str));
      g_string_printf(buffer, "%3.0f", s.get_vas());
      m_VasEntry.set_text(Glib::ustring(buffer->str));
      g_string_printf(buffer, "%2.0f", s.get_rdc());
      m_RdcEntry.set_text(Glib::ustring(buffer->str));
      g_string_printf(buffer, "%0.2f", s.get_lvc());
      m_LvcEntry.set_text(Glib::ustring(buffer->str));
      g_string_printf(buffer, "%0.3f", s.get_qms());
      m_QmsEntry.set_text(Glib::ustring(buffer->str));
      g_string_printf(buffer, "%0.3f", s.get_qes());
      m_QesEntry.set_text(Glib::ustring(buffer->str));
      g_string_printf(buffer, "%0.2f", s.get_imp());
      m_ImpEntry.set_text(Glib::ustring(buffer->str));
      g_string_printf(buffer, "%0.2f", s.get_sens());
      m_SensEntry.set_text(Glib::ustring(buffer->str));

      g_string_printf(buffer, "%0.2f", s.get_mmd());
      m_MmdEntry.set_text(Glib::ustring(buffer->str));
      g_string_printf(buffer, "%0.4f", s.get_ad());
      m_AdEntry.set_text(Glib::ustring(buffer->str));
      g_string_printf(buffer, "%0.2f", s.get_bl());
      m_BlEntry.set_text(Glib::ustring(buffer->str));
      g_string_printf(buffer, "%0.2f", s.get_rms());
      m_RmsEntry.set_text(Glib::ustring(buffer->str));
      g_string_printf(buffer, "%0.4f", s.get_cms());
      m_CmsEntry.set_text(Glib::ustring(buffer->str));


      /* Check buttons */
      if (s.get_type() & SPEAKER_TYPE_BASS) {
        m_BassCheckButton.set_active(true);
      } else {
        m_BassCheckButton.set_active(false);
      }
      if (s.get_type() & SPEAKER_TYPE_MIDRANGE) {
        m_MidrangeCheckButton.set_active(true);
      } else {
        m_MidrangeCheckButton.set_active(false);
      }
      if (s.get_type() & SPEAKER_TYPE_TWEETER) {
        m_TweeterCheckButton.set_active(true);
      } else {
        m_TweeterCheckButton.set_active(false);
      }      
      
      /* Freq resp file entry */
      plot.clear();
      m_FreqRespFileEntry.set_text(s.get_freq_resp_filename());
      /* Plot freq resp if it exists */
      if (s.get_freq_resp_filename() != "") {
        //cout << "opening file" << endl;
        ifstream fin(s.get_freq_resp_filename().c_str());
        if (fin.good()) {
          //cout << "reading file" << endl;
          vector<GSpeakers::Point> points;
          for (int i = 0; i < 30; i++) {
            char *buffer = new char[100];
            fin.getline(buffer, 100, '\n');
          
            float f1, f2;
            sscanf(buffer, "%f,%f", &f1, &f2);
            //cout << f1 << ", " << f2 << endl;
            GSpeakers::Point p(GSpeakers::round(f1), f2 );
            points.push_back(p);
          }
          Gdk::Color c("blue");
          plot.add_plot(points, c);
        } else {
          cout << "Could not open " << s.get_freq_resp_filename() << endl;
        }
      }
      /* Plot impedance response */
      /* increase impedance y coordinate 50 to align to imp scale */
      {
        vector<GSpeakers::Point> points;
        /* Produce SPICE input-file */
        string tmp_file = Glib::get_tmp_dir() + "/speaker" + GSpeakers::int_to_ustring(s.get_id()) + ".tmp";

        ofstream of(tmp_file.c_str());
        if (of.good()) {
          of << "SPICE code generated by GSpeakers-" << VERSION << endl;
          of << "vamp 1 0 ac" << endl;
          double cmes = s.get_mmd() / (s.get_bl() * s.get_bl()) * 1000000;
          double lces = s.get_bl() * s.get_bl() * s.get_cms() * 1000;
          double res = s.get_bl() * s.get_bl() / s.get_rms();
          double po = 1.18; // air density kg/m^3
          double cmef = 8 * po * s.get_ad() * s.get_ad() * s.get_ad() / (3 * s.get_bl() * s.get_bl()) * 1000000;
          of << "R" << s.get_id() << " 1 2 " << s.get_rdc() << endl;
          of << "L" << s.get_id() << " 2 3 " << s.get_lvc() << "mH" << endl;
          of << "lces 3 0 " << lces << "mH" << endl;
          of << "cmes 3 0 " << cmes << "uF" << endl;
          of << "res 3 0 " << res << endl;
          of << "cmef 3 0 " << cmef << "uF" << endl;
          of << ".ac DEC 10 20 20k" << endl;
          of << ".print ac i(vamp)" << endl;
          of << ".end" << endl;
          of.close();
          string cmd = g_settings.getValueString("SPICECmdLine") + " -b -o " + tmp_file + ".out " + tmp_file;
#ifdef OUTPUT_DEBUG
          cout << "FilterLinkFrame::on_plot_crossover: running SPICE with \"" + cmd + "\"" << endl;
#endif
          system(cmd.c_str());
#ifdef OUTPUT_DEBUG
          cout << "FilterLinkFrame::on_plot_crossover: SPICE done" << endl;
#endif
          string spice_output_file = tmp_file + ".out";
          ifstream fin(spice_output_file.c_str());
          bool output = false;
          int id;
          float f1, f2, f3;
          while (!fin.eof()) {
            char *buffer = new char[100];
            fin.getline(buffer, 100, '\n');
            if (buffer[0] == '0') {
              output = true;
            }
            if (output == true) {
              /* TODO: Read the sscanf string from settings */
              sscanf(buffer, "%d\t%f,\t%f\t%f", &id, &f1, &f2, &f3);
              GSpeakers::Point p(GSpeakers::round(f1), 50 + (1 / hypot(f2, f3)));
              points.push_back(p);
            }
            if ((buffer[0] == '3') && (buffer[1] == '0')) {
              output = false;
            }
          }
          Gdk::Color c2("red");
          plot.add_plot(points, c2);
          
        }
      }
    }  
  }
  m_IdStringEntry.grab_focus();
  set_entries_sensitive(true);
  m_RemoveButton.set_sensitive(true);
}

void Speaker_ListStore::on_clear()
{
  m_refListStore->clear();
  m_speaker_list->speaker_list()->erase(m_speaker_list->speaker_list()->begin(), m_speaker_list->speaker_list()->end());
  //m_TreeView.signal_select_all();
}

void Speaker_ListStore::on_entry_changed(int i)
{
  
  /* This treeview stuff is kind of weird... */
  Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();
  if (Gtk::TreeIter iter = refSelection->get_selected()) {
    Gtk::TreePath path = m_refListStore->get_path(iter);
    Gtk::TreeRow row = *(m_refListStore->get_iter(path));
      
    /* Update the data container */
    switch (i) {
      case 0:
        row[m_columns.id_string] = m_IdStringEntry.get_text();                             // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_id_string(m_IdStringEntry.get_text()); // speaker_list
        break;
      case 1:
        row[m_columns.qts] = atof(m_QtsEntry.get_text().c_str());                             // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_qts(atof(m_QtsEntry.get_text().c_str())); // speaker_list
        break;
      case 2:
        row[m_columns.fs] = atof(m_FsEntry.get_text().c_str());                             // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_fs(atof(m_FsEntry.get_text().c_str())); // speaker_list
        break;
      case 3:
        row[m_columns.vas] = atof(m_VasEntry.get_text().c_str());                             // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_vas(atof(m_VasEntry.get_text().c_str())); // speaker_list
        break;
      case 4:
        row[m_columns.rdc] = atof(m_RdcEntry.get_text().c_str());                             // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_rdc(atof(m_RdcEntry.get_text().c_str())); // speaker_list
        break;
      case 5:
        row[m_columns.lvc] = atof(m_LvcEntry.get_text().c_str());                             // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_lvc(atof(m_LvcEntry.get_text().c_str())); // speaker_list
        break;
      case 6:
        row[m_columns.qms] = atof(m_QmsEntry.get_text().c_str());                             // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_qms(atof(m_QmsEntry.get_text().c_str())); // speaker_list
        break;
      case 7:
        row[m_columns.qes] = atof(m_QesEntry.get_text().c_str());                             // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_qes(atof(m_QesEntry.get_text().c_str())); // speaker_list
        break;
      case 8:
        row[m_columns.imp] = atof(m_ImpEntry.get_text().c_str());                             // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_imp(atof(m_ImpEntry.get_text().c_str())); // speaker_list
        break;
      case 9:
        row[m_columns.sens] = atof(m_SensEntry.get_text().c_str());                             // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_sens(atof(m_SensEntry.get_text().c_str())); // speaker_list
        break;
      case 10:
        //cout << "bass" << endl;
        if (m_BassCheckButton.get_active() == true) {
          (*(m_speaker_list->speaker_list()))[index].set_type((*(m_speaker_list->speaker_list()))[index].get_type() | SPEAKER_TYPE_BASS);
        } else {
          (*(m_speaker_list->speaker_list()))[index].set_type((*(m_speaker_list->speaker_list()))[index].get_type() & ~SPEAKER_TYPE_BASS);
        }
        row[m_columns.type] = (*(m_speaker_list->speaker_list()))[index].get_type();
        break;
      case 11:
        //cout << "midrange" << endl;
        if (m_MidrangeCheckButton.get_active() == true) {
          (*(m_speaker_list->speaker_list()))[index].set_type((*(m_speaker_list->speaker_list()))[index].get_type() | SPEAKER_TYPE_MIDRANGE);
        } else {
          (*(m_speaker_list->speaker_list()))[index].set_type((*(m_speaker_list->speaker_list()))[index].get_type() & ~SPEAKER_TYPE_MIDRANGE);
        }
        row[m_columns.type] = (*(m_speaker_list->speaker_list()))[index].get_type();
        break;
      case 12:
        //cout << "tweeter:" << m_TweeterCheckButton.get_state() << endl;
        if (m_TweeterCheckButton.get_active() == true) {
          (*(m_speaker_list->speaker_list()))[index].set_type((*(m_speaker_list->speaker_list()))[index].get_type() | SPEAKER_TYPE_TWEETER);
        } else {
          (*(m_speaker_list->speaker_list()))[index].set_type((*(m_speaker_list->speaker_list()))[index].get_type() & ~SPEAKER_TYPE_TWEETER);
        }
        row[m_columns.type] = (*(m_speaker_list->speaker_list()))[index].get_type();
        break;
      case 13:
        row[m_columns.mmd] = atof(m_MmdEntry.get_text().c_str());                             // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_mmd(atof(m_MmdEntry.get_text().c_str())); // speaker_list
        break;
      case 14:
        row[m_columns.ad] = atof(m_AdEntry.get_text().c_str());                             // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_ad(atof(m_AdEntry.get_text().c_str())); // speaker_list
        break;
      case 15:
        row[m_columns.bl] = atof(m_BlEntry.get_text().c_str());                             // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_bl(atof(m_BlEntry.get_text().c_str())); // speaker_list
        break;
      case 16:
        row[m_columns.rms] = atof(m_RmsEntry.get_text().c_str());                             // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_rms(atof(m_RmsEntry.get_text().c_str())); // speaker_list
        break;
      case 17:
        row[m_columns.cms] = atof(m_CmsEntry.get_text().c_str());                             // the treestore
        (*(m_speaker_list->speaker_list()))[index].set_cms(atof(m_CmsEntry.get_text().c_str())); // speaker_list
        break;
        
      
    }
  }
  m_SaveButton.set_sensitive(true);
  m_SaveAsButton.set_sensitive(true);

  //cout << "centry hanged" << endl;
  
}

void Speaker_ListStore::on_append_xml()
{
  //cout << "append xml" << endl;
  if (f_append == NULL) {
    f_append = new Gtk::FileSelection("Append speaker xml");
    f_append->get_ok_button()->signal_clicked().connect(bind<Gtk::FileSelection *>(slot(*this, &Speaker_ListStore::on_append_ok), f_append));
    f_append->get_cancel_button()->signal_clicked().connect(slot(*f_append, &Gtk::Widget::hide));
  } else {
    f_append->set_title("Append speaker xml");
    f_append->show();
  }

  f_append->run();
  
}

void Speaker_ListStore::on_open_xml()
{
  //cout << "open xml ok" << endl;
  if (f_open == NULL) {
    f_open = new Gtk::FileSelection("Open speaker xml");
    f_open->get_ok_button()->signal_clicked().connect(bind<Gtk::FileSelection *>(slot(*this, &Speaker_ListStore::on_open_ok), f_open));
    f_open->get_cancel_button()->signal_clicked().connect(slot(*f_open, &Gtk::Widget::hide));
  } else {
    f_open->show();
    f_open->set_title("Open speaker xml");
  }
  f_open->run();

}


void Speaker_ListStore::on_append_ok(Gtk::FileSelection *f) 
{
  //cout << "append xml ok" << endl;
  SpeakerList temp_speaker_list;
  try {
    temp_speaker_list = SpeakerList(f->get_filename());

    for_each(
      temp_speaker_list.speaker_list()->begin(), temp_speaker_list.speaker_list()->end(),
      slot(*this, &Speaker_ListStore::liststore_add_item));
    for (
      vector<Speaker>::iterator from = temp_speaker_list.speaker_list()->begin();
      from != temp_speaker_list.speaker_list()->end();
      ++from)
    {
      m_speaker_list->speaker_list()->push_back(*from);
    }
    f->hide();
    m_speaker_list->speaker_list()->size();
    set_entries_sensitive(true);
  } catch (GSpeakersException e) {
    Gtk::MessageDialog m(e.what(), Gtk::MESSAGE_ERROR);
    //cout << "Error: " << e.what() << endl;
    m.run();
  }
  
}

void Speaker_ListStore::on_open_ok(Gtk::FileSelection *f) 
{
  //cout << "open ok" << endl;
  m_refListStore->clear();

  SpeakerList temp_speaker_list;
  try {
    temp_speaker_list = SpeakerList(f->get_filename());

    m_filename = f->get_filename();
    for_each(
      temp_speaker_list.speaker_list()->begin(), temp_speaker_list.speaker_list()->end(),
      slot(*this, &Speaker_ListStore::liststore_add_item));
  
    /* Delete items in speaker_list */
    m_speaker_list->speaker_list()->erase(m_speaker_list->speaker_list()->begin(), m_speaker_list->speaker_list()->end());
  
    for (
      vector<Speaker>::iterator from = temp_speaker_list.speaker_list()->begin();
      from != temp_speaker_list.speaker_list()->end();
      ++from)
    {
      m_speaker_list->speaker_list()->push_back(*from);
    }
    f->hide();
  
    /* Select the first item in the list */
    //cout << m_speaker_list.speaker_list()->size() << endl;
    if (m_speaker_list->speaker_list()->size() > 0) {
      Glib::RefPtr<Gtk::TreeSelection> refSelection = m_TreeView.get_selection();
      char *str = NULL;
      GString *buffer = g_string_new(str);
      g_string_printf(buffer, "%d", 0);
      GtkTreePath *gpath = gtk_tree_path_new_from_string(buffer->str);
      Gtk::TreePath path(gpath);
  
      Gtk::TreeRow row = *(m_refListStore->get_iter(path));
      refSelection->select(row);
  
    }
    m_IdStringEntry.grab_focus();
    set_entries_sensitive(true);
    m_AppendXmlButton.set_sensitive(true);
    m_SaveButton.set_sensitive(true);
    m_SaveAsButton.set_sensitive(true);
    m_RemoveButton.set_sensitive(true);
    signal_speakerlist_loaded(m_filename);
  } catch (GSpeakersException e) {
    Gtk::MessageDialog m(e.what(), Gtk::MESSAGE_ERROR);
    //cout << "Error: " << e.what() << endl;
    m.run();
  }

}

void Speaker_ListStore::on_edit_freq_resp()
{
#ifdef OUTPUT_DEBUG
  cout << "SpeakerEditor::on_edit_freq_resp" << endl;
#endif
  //m_FreqRespFileEntry.get_text();
  cout << "index == " << index << endl;
  FreqRespEditor *f = new FreqRespEditor(m_FreqRespFileEntry.get_text());
  f->run();
  m_FreqRespFileEntry.set_text(f->get_filename());
  (*m_speaker_list->speaker_list())[index].set_freq_resp_filename(f->get_filename());
  delete f;
}

void Speaker_ListStore::on_browse_freq_resp()
{
#ifdef OUTPUT_DEBUG
  cout << "SpeakerEditor::on_browse_freq_resp" << endl;
#endif
  Gtk::FileSelection *f = new Gtk::FileSelection("Enter filename...");
  f->set_modal();
  /* -5 == ok button clicked */
  if (f->run() == -5) {
    m_FreqRespFileEntry.set_text(f->get_filename());
    (*m_speaker_list->speaker_list())[index].set_freq_resp_filename(f->get_filename());
  } 
  f->hide();
  delete f;
}


void Speaker_ListStore::add_items(SpeakerList slist)
{
  //m_vecItems.push_back( Speaker("Vifa P21") );
  //m_vecItems.push_back( Speaker("Vifa P22") );
  //m_vecItems.push_back( Speaker("Vifa P23") );
}

void Speaker_ListStore::create_model()
{
  m_refListStore = Gtk::ListStore::create(m_columns);
  
  //add_items(*m_speaker_list);

  for_each(
      m_speaker_list->speaker_list()->begin(), m_speaker_list->speaker_list()->end(),
      slot(*this, &Speaker_ListStore::liststore_add_item));
}

//void Speaker_ListStore::liststore_add_item(const Speaker& spk)
void Speaker_ListStore::liststore_add_item(Speaker spk)
{
  Gtk::TreeRow row = *(m_refListStore->append());

  row[m_columns.id]         = spk.get_id();
  row[m_columns.id_string]  = spk.get_id_string();
  row[m_columns.type]       = spk.get_type();
  row[m_columns.qts]        = spk.get_qts();
  row[m_columns.fs]         = spk.get_fs();
  row[m_columns.vas]        = spk.get_vas();
  row[m_columns.rdc]        = spk.get_rdc();
  row[m_columns.lvc]        = spk.get_lvc();
  row[m_columns.qms]        = spk.get_qms();
  row[m_columns.qes]        = spk.get_qes();
  row[m_columns.imp]        = spk.get_imp();
  row[m_columns.sens]       = spk.get_sens();
  row[m_columns.mmd]        = spk.get_mmd();
  row[m_columns.ad]         = spk.get_ad();
  row[m_columns.bl]         = spk.get_bl();
  row[m_columns.rms]        = spk.get_rms();
  row[m_columns.cms]        = spk.get_cms();
  
}

void Speaker_ListStore::add_columns()
{
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count =m_TreeView.append_column("Id", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.id);
  }

  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count =m_TreeView.append_column("Identifier", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.id_string);
  }

  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Type", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.type);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Qts", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.qts);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Fs", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.fs);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Vas", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.vas);
  }

  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Rdc", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.rdc);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Lvc", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.lvc);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Qms", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.qms);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Qes", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.qes);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Impedance", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.imp);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Sensitivity", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.sens);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Cone mass", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.mmd);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Active radius", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.ad);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Force factor", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.bl);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Susp. resistance", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.rms);
  }
  {
    Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );

    int cols_count = m_TreeView.append_column("Susp. compliance", *pRenderer);
    Gtk::TreeViewColumn* pColumn =m_TreeView.get_column(cols_count-1);

    pColumn->add_attribute(pRenderer->property_text(), m_columns.cms);
  }

}

void Speaker_ListStore::on_cell_fixed_toggled(const Glib::ustring& path_string)
{
  //TODO: Why doesn't the path(strng) constructor work?
  //Using it causes the new text to be put always in the first row.
  //Gtk::TreePath path(path_string);
  GtkTreePath *gpath = gtk_tree_path_new_from_string (path_string.c_str());
  Gtk::TreePath path(gpath);

  /* get toggled iter */
  Gtk::TreeRow row = *(m_refListStore->get_iter(path));

  bool fixed = row[m_columns.id];

  /* do something with the value */
  fixed = !fixed;

  /* set new value */
  row[m_columns.id] = fixed;
}
