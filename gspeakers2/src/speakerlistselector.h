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

#ifndef __GSPEAKERS_SPEAKERLISTSELECTOR
#define __GSPEAKERS_SPEAKERLISTSELECTOR

#include <gtkmm.h>
#include "speakerlist.h"
#include "speakereditor.h"

using namespace std;
using namespace SigC;

/*
 * This is a class that will contain an entry displaying the currently 
 * selected speaker xml file. This window also contains button to open 
 * a new speakerlist xml and a button that launched the speakereditor.
 */
class SpeakerListSelector : public Gtk::Window
{
public:
  SpeakerListSelector();
  ~SpeakerListSelector();
  
protected:
  /* Callbacks */
  void on_open();
  void on_edit_speakers();
  void on_open_ok(Gtk::FileSelection *f);
  void on_speakerlist_loaded(string speaker_list_filename);

  /* Member widgets */
  Gtk::Entry m_SpeakerXmlFilenameEntry;
  Gtk::Button m_OpenButton, m_EditButton;
  Gtk::HBox m_hbox1, m_hbox2;
  Gtk::VBox m_vbox;
  
  Gtk::FileSelection *f_open;
  
private:
  SpeakerList m_speaker_list;
  Speaker_ListStore *speaker_liststore;
};

#endif
