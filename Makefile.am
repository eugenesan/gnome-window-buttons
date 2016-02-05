## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = \
	buttons \
	title \
	po

ACLOCAL_AMFLAGS = -I m4

windowappletsdocdir = ${docdir}
windowappletsdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS

INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = \
	$(windowbuttonsdoc_DATA) \
	$(windowtitledoc_DATA) \
	$(INTLTOOL_FILES)

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache
