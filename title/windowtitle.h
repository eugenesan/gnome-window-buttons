/***************************************************************************
 *            main.h
 *
 *  Mon May  4 01:21:56 2009
 *  Copyright  2009  Andrej Belcijan
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
 
#ifndef __WT_APPLET_H__
#define __WT_APPLET_H__

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <glib.h>
#include <glib-object.h>
#include <gconf/gconf-client.h>
#include <panel-applet.h>
#include <panel-applet-gconf.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gtk/gtklabel.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#ifndef WNCK_I_KNOW_THIS_IS_UNSTABLE
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#endif
#include <libwnck/libwnck.h>

/* static paths and stuff */
#define APPLET_NAME						"Window Title"
#define APPLET_OAFIID					"OAFIID:WindowTitleApplet"
#define APPLET_OAFIID_FACTORY			"OAFIID:WindowTitleApplet_Factory"
#define LOCATION_MAIN 					"/usr/share/windowtitle"
#define LOCATION_BUILDER 				"/usr/share/gnome-applets/builder"
#define GCONF_PREFS 					"/schemas/apps/windowtitle-applet/prefs"
#define GCONF_COMPIZ_DECORATION_MATCH	"/apps/compiz/plugins/decoration/allscreens/options/decoration_match"
#define COMPIZ_DECORATION_MATCH			"!(state=maxvert | maxhorz)"
 
#define GCK_ALIGNMENT					"alignment"
#define GCK_SWAP_ORDER					"swap_order"
#define GCK_HIDE_ICON					"hide_icon"
#define GCK_HIDE_TITLE					"hide_title"
#define GCK_CUSTOM_STYLE				"custom_style"
#define GCK_TITLE_FONT					"title_font"
#define GCK_TITLE_COLOR_FG				"title_color_fg"
#define GCK_ONLY_MAXIMIZED				"only_maximized"
#define GCK_HIDE_ON_UNMAXIMIZED 		"hide_on_unmaximized"

G_BEGIN_DECLS

#define WT_TYPE_APPLET                wt_applet_get_type()
#define WT_APPLET(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), WT_TYPE_APPLET, WTApplet))
#define WT_APPLET_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), WT_TYPE_APPLET, WTAppletClass))
#define WT_IS_APPLET(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WT_TYPE_APPLET))
#define WT_IS_APPLET_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), WT_TYPE_APPLET))
#define WT_APPLET_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), WT_TYPE_APPLET, WTAppletClass))

/* Applet properties (things that get saved) */
typedef struct {
	gboolean 		only_maximized,			// [T/F] Only track maximized windows
					hide_on_unmaximized,	// [T/F] Hide when no maximized windows present
					hide_icon,				// [T/F] Hide the icon
					hide_title,				// [T/F] Hide the title
					swap_order,				// [T/F] Swap title/icon
					custom_style;			// [T/F] Use custom style
	gchar			*title_font;			// Custom title font
	gchar			*title_color;			// Custom title color
	gdouble			alignment;				// Title alignment [0=left, 0.5=center, 1=right]
} WTPreferences;

/* WBApplet definition (inherits from PanelApplet) */
typedef struct {
    PanelApplet		parent;					// Applet parent

	/* Widgets */
	GtkBox      	*box;					// Main container widget
	GtkEventBox		*eb_icon, *eb_title;	// Eventbox widgets
	GtkImage		*icon;					// Icon image widget
	GtkLabel		*title;					// Title label widget
	GtkWidget		*window_prefs;			// Preferences window
	
	/* Variables */
	WTPreferences	*prefs;					// Main properties 
	WnckScreen 		*activescreen;			// Active screen
	WnckWorkspace	*activeworkspace;		// Active workspace
	WnckWindow		*currentwindow,			// Upper-most maximized window
					*activewindow,			// Active window
					*rootwindow;			// Root window (desktop)
	gulong			active_handler_state,	// activewindow's statechange event handler ID
					active_handler_name,	// activewindow's namechange event handler ID
					active_handler_icon,	// activewindow's iconchange event handler ID
					current_handler_state,	// currentwindow's statechange event handler ID
					current_handler_name,	// currentwindow's manechange event handler ID
					current_handler_icon;	// currentwindow's iconchange event handler ID
	GdkPixbuf		*icon_pixbuf;			// Icon pixle buffer
	gboolean		focused;				// [T/F] Window state (focused or unfocused)
	
	PanelAppletOrient	orient;				// Panel orientation
	GdkPixbufRotation	angle;				// Applet angle
	GtkPackType			packtype;			// Packaging direction of buttons
	
	/* GtkBuilder */
	GtkBuilder 		*prefbuilder;			// Glade GtkBuilder for the preferences
} WTApplet;

typedef struct {
        PanelAppletClass applet_class;
} WTAppletClass;

GType wt_applet_get_type (void);
WTApplet* wt_applet_new (void);

G_END_DECLS
#endif
