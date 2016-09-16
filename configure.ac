dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(gnome-window-applets, 0.3)

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
GETTEXT_PACKAGE=gnome-window-applets
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AC_ARG_WITH([localedir], AS_HELP_STRING([--with-localedir=<path>],
         [Path to install message files in (default: datadir/locale)]),
         [LOCALEDIR=${withval}], [LOCALEDIR=${datadir}/locale])
AC_SUBST([LOCALEDIR])
AX_DEFINE_DIR(GNOMELOCALEDIR, "${datadir}/locale", [locale directory])
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])

dnl *************
dnl GConf
dnl *************
AM_GCONF_SOURCE_2
AC_PATH_PROG(GCONFTOOL, gconftool-2)

PKG_CHECK_MODULES(WINDOWBUTTONS, libpanelapplet-4.0
[libwnck-3.0 gtk+-3.0 gconf-2.0])

PKG_CHECK_MODULES(WINDOWTITLE, libpanelapplet-4.0
[libwnck-3.0 gtk+-3.0 gconf-2.0])


AS_AC_EXPAND(LIBEXECDIR, $libexecdir)
AC_SUBST(LIBEXECDIR)

AS_AC_EXPAND(DATADIR, ${datadir})
AC_SUBST(DATADIR)

AC_OUTPUT([
Makefile
windowbuttons/Makefile
windowbuttons/pixmaps/Makefile
windowbuttons/themes/Makefile
windowtitle/Makefile
windowtitle/pixmaps/Makefile
po/Makefile.in
])
