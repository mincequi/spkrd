/*
  speakerselector Copyright (C) 2002 Daniel Sundberg

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

#include "speakerlistselector.h"
#include "speakereditor.h"
#include "common.h"
#include "../config.h"

SpeakerListSelector::SpeakerListSelector() : 
  Gtk::Frame(_("Speaker list xml")), 
  m_SpeakerXmlFilenameEntry(),
  m_OpenButton(_("Open..")), 
  m_EditButton(_("Edit speakers...")), 
  m_Table(1, 4, false)
{
  Gtk::Label *l = manage(new Gtk::Label(_("Speaker xml: ")));
  m_Table.attach(*l, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
  m_Table.attach(m_SpeakerXmlFilenameEntry, 1, 2, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
  m_Table.attach(m_EditButton, 3, 4, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
  m_Table.attach(m_OpenButton, 2, 3, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
  
  m_Table.set_spacings(5);
  m_vbox.set_border_width(5);
  m_vbox.pack_start(m_Table, Gtk::PACK_SHRINK);
  add(m_vbox);
  
  m_SpeakerXmlFilenameEntry.set_sensitive(false);
  m_OpenButton.signal_clicked().connect(slot(*this, &SpeakerListSelector::on_open));
  m_EditButton.signal_clicked().connect(slot(*this, &SpeakerListSelector::on_edit_speakers));
  
  show_all();
  m_EditButton.set_sensitive(false);
  speaker_liststore = NULL;
  
  f_open = NULL;
  signal_speakerlist_loaded.connect(slot(*this, &SpeakerListSelector::on_speakerlist_loaded));

/* Fix some initstuff since we don't have any settings */
  f_open = new Gtk::FileSelection(_("Open speaker xml"));

  g_settings.defaultValueString("SpeakerListXml", string(GSPEAKERS_PREFIX) + "/share/xml/vifa.xml");
  f_open->set_filename(g_settings.getValueString("SpeakerListXml"));
  f_open->get_ok_button()->signal_clicked().connect(bind<Gtk::FileSelection *>(slot(*this, &SpeakerListSelector::on_open_ok), f_open));
  f_open->get_cancel_button()->signal_clicked().connect(slot(*f_open, &Gtk::Widget::hide));
  on_open_ok(f_open);
  
}

SpeakerListSelector::~SpeakerListSelector()
{
  if (f_open != NULL) delete f_open;
  if (speaker_liststore != NULL) delete speaker_liststore;
}
  
void SpeakerListSelector::on_open()
{
#ifdef OUTPUT_DEBUG
  cout << "on open" << endl;
#endif
  if (f_open == NULL) {
    f_open = new Gtk::FileSelection(_("Open speaker xml"));
    f_open->get_ok_button()->signal_clicked().connect(bind<Gtk::FileSelection *>(slot(*this, &SpeakerListSelector::on_open_ok), f_open));
    f_open->get_cancel_button()->signal_clicked().connect(slot(*f_open, &Gtk::Widget::hide));
  } else {
    f_open->show();
    //f_open->set_title(_("Open speaker xml"));
  }
  f_open->run();
}

void SpeakerListSelector::on_open_ok(Gtk::FileSelection *f)
{
  try {
    m_speaker_list = SpeakerList(f->get_filename());
    signal_speakerlist_loaded(f->get_filename());
    g_settings.setValue("SpeakerListXml", f->get_filename());
    f_open->hide();
  } catch (GSpeakersException e) {
    Gtk::MessageDialog m(e.what(), Gtk::MESSAGE_ERROR);
    m.run();
  }
}

void SpeakerListSelector::on_speakerlist_loaded(string speaker_list_filename)
{
  //set_label("Speaker list xml [" + speaker_list_filename + "]");
  m_speaker_list = SpeakerList(speaker_list_filename);
  m_SpeakerXmlFilenameEntry.set_text(speaker_list_filename);
  g_settings.setValue("SpeakerListXml", speaker_list_filename);
  m_EditButton.set_sensitive(true);
}

void SpeakerListSelector::on_edit_speakers()
{
#ifdef OUTPUT_DEBUG
  cout << "SpeakerListSelector::on_edit_speakers" << endl;
#endif
  speaker_liststore = new Speaker_ListStore(&m_speaker_list, m_SpeakerXmlFilenameEntry.get_text());

}
  
