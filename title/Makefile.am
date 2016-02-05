## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(WINDOWTITLE_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

libexec_PROGRAMS = windowtitle

windowtitle_SOURCES = \
	windowtitle.c\
	windowtitle.h \
	preferences.c \
	preferences.h 

windowtitle_LDFLAGS = \
	-Wl,--export-dynamic

windowtitle_LDADD = $(WINDOWTITLE_LIBS)

serverdir       = $(libdir)/bonobo/servers
server_in_files = WindowTitle.server.in
server_DATA     = $(server_in_files:.server.in=.server)

schemasdir = @GCONF_SCHEMA_FILE_DIR@
schemas_DATA = windowtitle.schemas

CLEANFILES = $(server_in_files) $(server_DATA)

EXTRA_DIST = \
	WindowTitle.server.in.in \
	$(builder_DATA) \
	$(schemas_DATA)

builder_DATA = \
	windowtitle.ui

builderdir = \
	$(prefix)/share/windowtitle

SUBDIRS = \
	pixmaps

%.server: %.server.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*.po)
	LC_ALL=C $(INTLTOOL_MERGE) -s -u -c $(top_buildir)/po/.intltool-merge-cache $(top_srcdir)/po $< $@


# Display a message at the end of install
install-data-hook:
	@echo "***"
	@echo "*** If you have installed your applet in a non standard directory. You have to"
	@echo "*** add it in the bonobo server list, so it can find your applet."
	@echo "***"
	@echo "*** Run 'bonobo-activation-sysconf --add-directory=$(serverdir)'"
	@echo "*** Log out from your current GNOME session"
	@echo "*** Run 'bonobo-slay'"
	@echo "*** Check that the bonobo server is not running"
	@echo "*** Log in again, your applet should be available"
	@echo "***"
	
install-data-local:
if GCONF_SCHEMAS_INSTALL
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(schemas_DATA)
endif
