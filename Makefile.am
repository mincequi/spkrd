SUBDIRS = src xml pixmaps #po

# Desktop entry
desktopdir		= $(datadir)/applications
desktop_in_files	= gspeakers.desktop.in
desktop_DATA 		= $(desktop_in_files:.desktop.in=.desktop)
# @INTLTOOL_DESKTOP_RULE@

# Icon
icondir			= $(datadir)/pixmaps
icon_DATA		= gspeakers.png

EXTRA_DIST		= $(desktop_in_files) $(desktop_DATA) $(icon_DATA)
