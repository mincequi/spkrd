
#include "common.h"
#include "main_window.hpp"

#include <glibmm.h>
#include <gtkmm/main.h>
#include <gtkmm/messagedialog.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <clocale>
#include <cstdlib>
#include <iostream>

auto main(int argc, char* argv[]) -> int
{
    // Initialize gettext
#if ENABLE_NLS
    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, GNOMELOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(PACKAGE);
#endif

    Gtk::Main kit(argc, argv);

    try
    {
#ifdef TARGET_WIN32
        g_settings.load("gspeakers2.conf");
#else
        g_settings.load(Glib::get_home_dir() + "/.gspeakers/gspeakers2.conf");
#endif
    }
    catch (std::runtime_error const& error)
    {
        std::cout << "Main: " << error.what() << "\n";

#ifdef TARGET_WIN32
        Gtk::MessageDialog message_dialog(_("No configuration file found!") + Glib::ustring("\n")
                                              + _("gspeakers2.conf created in current directory"),
                                          true,
                                          Gtk::MESSAGE_INFO,
                                          Gtk::BUTTONS_OK,
                                          true);
#else
        Gtk::MessageDialog message_dialog(_("No configuration file found!") + Glib::ustring("\n\n")
                                              + Glib::get_home_dir()
                                              + "/.gspeakers/gspeakers2.conf created",
                                          true,
                                          Gtk::MESSAGE_INFO,
                                          Gtk::BUTTONS_OK,
                                          true);
#endif
        message_dialog.run();
#ifdef TARGET_WIN32
        std::string s = "echo > gspeakers2.conf";
        system(s.c_str());
        g_settings.load("gspeakers2.conf");
#else
        std::string s = "mkdir " + Glib::get_home_dir() + "/.gspeakers";
        system(s.c_str());
        s = "touch " + Glib::get_home_dir() + "/.gspeakers/gspeakers2.conf";
        system(s.c_str());
        g_settings.load(Glib::get_home_dir() + "/.gspeakers/gspeakers2.conf");
#endif
    }

    g_settings.defaultValueUnsignedInt("ToolbarStyle", Gtk::TOOLBAR_BOTH);

    main_window mw;

    Gtk::Main::run(mw);

    try
    {
        g_settings.save();
    }
    catch (std::runtime_error const& e)
    {
        std::cout << "Main: " << e.what() << std::endl;
    }
    return 0;
}
