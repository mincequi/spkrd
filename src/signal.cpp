
#include "signal.hpp"

sigc::signal1<void, Crossover*> signal_crossover_selected;
sigc::signal1<void, speaker_list*> signal_speakerlist_loaded;
sigc::signal1<void, Box*> signal_box_selected;
sigc::signal1<void, Box*> signal_add_to_boxlist;
sigc::signal3<void, Box*, Speaker*, Gdk::Color&> signal_add_plot;
sigc::signal1<void, Box*> signal_box_modified;
sigc::signal2<int, std::vector<gspk::point>&, Gdk::Color&> signal_add_box_plot;
sigc::signal1<void, int> signal_remove_box_plot;
sigc::signal1<void, int> signal_hide_box_plot;
sigc::signal1<void, int> signal_select_plot;
sigc::signal0<void> signal_net_modified_by_wizard;
sigc::signal1<void, Net*> signal_net_modified_by_user;
sigc::signal1<void, int> signal_new_crossover;
sigc::signal0<void> signal_plot_crossover;
sigc::signal4<int, std::vector<gspk::point> const&, Gdk::Color const&, int&, Net*> signal_add_crossover_plot;
sigc::signal0<void> signal_save_open_files;
