AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DGNOMELOCALEDIR=\"$(LOCALEDIR)\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(WINDOWBUTTONS_CFLAGS)

AM_CFLAGS = \
	-Wall \
	-DGNOMELOCALEDIR=\"$(LOCALEDIR)\" \
	-g

libexec_PROGRAMS = windowbuttons

windowbuttons_SOURCES = \
	windowbuttons.c\
	windowbuttons.h \
	preferences.c \
	preferences.h \
	external.c \
	external.h \
	theme.c \
	theme.h

windowbuttons_LDFLAGS = \
	-Wl,--export-dynamic

windowbuttons_LDADD = $(WINDOWBUTTONS_LIBS)
#windowbuttons_libdir=$(libexecdir)/gnome-applets/5.0

builderdir = $(datadir)/windowbuttons
builder_DATA = windowbuttons.ui

gsettings_file = org.gnome.window-applets.windowbuttons.gschema.xml
gsettings_SCHEMAS = $(gsettings_file)

@INTLTOOL_XML_NOMERGE_RULE@

# The new applet files (D-Bus???)
appletdir       = $(datadir)/gnome-panel/5.0/applets
applet_in_files = org.gnome.panel.WindowButtonsApplet.panel-applet.in
applet_DATA     = $(applet_in_files:.panel-applet.in=.panel-applet)

$(applet_in_files): $(applet_in_files).in Makefile
	$(AM_V_GEN)sed \
		-e "s|\@LIBEXECDIR\@|$(libexecdir)|" \
		-e "s|\@DATADIR\@|$(datadir)|" \
		-e "s|\@VERSION\@|$(PACKAGE_VERSION)|" \
		$< > $@

$(gsettings_file).in: $(gsettings_file).in.in
	sed \
	  -e "s|\@GETTEXT_PACKAGE\@|$(GETTEXT_PACKAGE)|" \
	  $< > $@

@GSETTINGS_RULES@

# The new Service file (D-Bus)
servicedir       = $(datadir)/dbus-1/services
service_in_files = org.gnome.panel.applet.WindowButtonsAppletFactory.service.in
service_DATA     = $(service_in_files:.service.in=.service)

org.gnome.panel.applet.WindowButtonsAppletFactory.service: $(service_in_files)
	$(AM_V_GEN)sed \
		-e "s|\@LIBEXECDIR\@|$(libexecdir)|" \
		$< > $@

%.panel-applet: %.panel-applet.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

EXTRA_DIST = \
	org.gnome.panel.WindowButtonsApplet.panel-applet.in.in \
	$(service_in_files) \
	$(gsettings_file).in.in \
	$(builder_DATA)

CLEANFILES = \
	$(applet_DATA) \
	$(applet_DATA).in \
	$(gsettings_file).in \
	$(gsettings_file) \
	$(service_DATA) \
	*.gschema.valid

SUBDIRS = \
	pixmaps\
	themes
