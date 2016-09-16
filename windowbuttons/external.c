/***************************************************************************
 *            external.c
 *
 *  Wed Sep  29 01:56:39 2010
 *  Copyright  2010  Andrej Belcijan
 *  <{andrejx} at {gmail.com}>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

/* 
 * This file is responsible for handling of things that are a part of outside
 * programs (Metacity, Compiz...).
 */

#include "external.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <panel-applet.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

gchar *getMetacityLayout(void);
gchar *getMetacityTheme(void);
gboolean issetCompizDecoration(void);
//gboolean issetMetacityDecoration(void); //TODO
void toggleCompizDecoration(gboolean);
//void toggleMetacityDecoration(gboolean); //TODO

gchar *getMetacityLayout() {
	GSettings *settings = g_settings_new(GNOME_WM_PREFERENCES);
	
	gchar *retval = g_settings_get_string(settings, GSETTINGS_WM_BUTTON_LAYOUT);
	
	g_object_unref(settings);
	
	return retval;
}

gchar *getMetacityTheme() {
	GSettings *settings = g_settings_new(GNOME_WM_PREFERENCES);
	
	gchar *retval = g_settings_get_string(settings, GSETTINGS_WM_THEME);
	
	g_object_unref(settings);
	
	return retval;
}

gboolean issetCompizDecoration() {
	gboolean retval = FALSE;
	GSettings *settings = NULL;
	settings = g_settings_new_with_path(COMPIZ_DECOR_SCHEMA, GSETTINGS_COMPIZ_DECOR_PATH);
	gchar *cdm = g_settings_get_string(settings, GSETTINGS_COMPIZ_DECORATION_MATCH);
	
	if (cdm == NULL) {
		retval = FALSE;
	} else if (!g_strcmp0(cdm, COMPIZ_DECORATION_MATCH_VALUE)) {
		retval = TRUE;
	}
	g_free(cdm);
	g_object_unref(settings);
	return retval;
}

void toggleCompizDecoration(gboolean new_value) {
	GSettings *settings = NULL;

	settings = g_settings_new_with_path(COMPIZ_DECOR_SCHEMA, GSETTINGS_COMPIZ_DECOR_PATH);
	if (new_value) {
		g_settings_reset(settings, GSETTINGS_COMPIZ_DECORATION_MATCH);
	} else if (g_settings_get_string(settings, GSETTINGS_COMPIZ_DECORATION_MATCH)) {
					 g_settings_set_string(settings,
						GSETTINGS_COMPIZ_DECORATION_MATCH,
						COMPIZ_DECORATION_MATCH_VALUE);
	} else {
		g_settings_reset(settings, GSETTINGS_COMPIZ_DECORATION_MATCH);
	}

	g_object_unref(settings);
}
