dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(gnome-window-applets, 0.2.9)

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC




dnl ***************************************************************************
dnl Internatinalization
dnl ***************************************************************************
GETTEXT_PACKAGE=windowutils
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AC_DEFINE_DIR(GNOMELOCALEDIR, "${datadir}/locale", [locale directory])
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])

dnl *************
dnl GConf
dnl *************
AM_GCONF_SOURCE_2
AC_PATH_PROG(GCONFTOOL, gconftool-2)

PKG_CHECK_MODULES(GNOME_APPLETS, libpanelapplet-2.0) 

PKG_CHECK_MODULES(WINDOWBUTTONS, [libgnome-2.0 >= 2.14 libpanelapplet-2.0 >= 2.14 libwnck-1.0 gtk+-2.0 gconf-2.0])

PKG_CHECK_MODULES(WINDOWTITLE, [libgnome-2.0 >= 2.14 libpanelapplet-2.0 >= 2.14 libwnck-1.0 gtk+-2.0 gconf-2.0])


AS_AC_EXPAND(LIBEXECDIR, $libexecdir)
AC_SUBST(LIBEXECDIR)

AC_OUTPUT([
Makefile
buttons/Makefile
buttons/WindowButtons.server.in
buttons/pixmaps/Makefile
buttons/themes/Makefile
title/Makefile
title/WindowTitle.server.in
title/pixmaps/Makefile
po/Makefile.in
])
