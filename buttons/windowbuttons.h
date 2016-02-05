/***************************************************************************
 *            windowbuttons.h
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
 
#ifndef __WB_APPLET_H__
#define __WB_APPLET_H__

#include <config.h>

#include <glib.h>
#include <glib-object.h>
#include <gconf/gconf-client.h>
#include <panel-applet.h>
#include <panel-applet-gconf.h>
#include <gtk/gtk.h>
#include <gtk/gtklabel.h>

#ifndef WNCK_I_KNOW_THIS_IS_UNSTABLE
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#endif
#include <libwnck/libwnck.h>

/* static paths and stuff */
#define APPLET_NAME						"Window Buttons"
#define APPLET_OAFIID					"OAFIID:WindowButtonsApplet"
#define APPLET_OAFIID_FACTORY			"OAFIID:WindowButtonsApplet_Factory"
#define LOCATION_MAIN 					"/usr/share/windowbuttons"
#define LOCATION_THEMES 				LOCATION_MAIN"/themes"
#define LOCATION_BUILDER 				"/usr/share/gnome-applets/builder"
#define GCONF_PREFS 					"/schemas/apps/windowbuttons-applet/prefs"
#define GCONF_METACITY_BUTTON_LAYOUT 	"/apps/metacity/general/button_layout"
#define GCONF_COMPIZ_DECORATION_MATCH	"/apps/compiz/plugins/decoration/allscreens/options/decoration_match"
#define METACITY_XML 					"metacity-theme-1.xml"
#define COMPIZ_DECORATION_MATCH			"!(state=maxvert | maxhorz)"

/* strings that identify button states and names */
#define BTN_STATE_FOCUSED				"focused"
#define BTN_STATE_CLICKED				"clicked"
#define BTN_STATE_UNFOCUSED				"unfocused"
#define BTN_STATE_HOVER					"hover"

#define BTN_NAME_CLOSE					"close"
#define BTN_NAME_MINIMIZE				"minimize"
#define BTN_NAME_MAXIMIZE				"maximize"
#define BTN_NAME_UNMAXIMIZE				"unmaximize"

/* GConf key strings (also used in GtkBuilder .ui file) */
#define GCK_CHECKBOX_ONLY_MAXIMIZED			"only_maximized"
#define GCK_CHECKBOX_HIDE_ON_UNMAXIMIZED 	"hide_on_unmaximized"
#define GCK_CHECKBOX_CLICK_EFFECT			"click_effect"
#define GCK_CHECKBOX_HOVER_EFFECT			"hover_effect"
#define GCK_CHECKBOX_USE_METACITY_LAYOUT	"use_metacity_layout"
#define GCK_BUTTON_LAYOUT					"button_layout"
#define GCK_THEME							"theme"

G_BEGIN_DECLS

#define WB_TYPE_APPLET                wb_applet_get_type()
#define WB_APPLET(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), WB_TYPE_APPLET, WBApplet))
#define WB_APPLET_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), WB_TYPE_APPLET, WBAppletClass))
#define WB_IS_APPLET(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WB_TYPE_APPLET))
#define WB_IS_APPLET_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), WB_TYPE_APPLET))
#define WB_APPLET_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), WB_TYPE_APPLET, WBAppletClass))

/* we will index images for convenience */
typedef enum {
	WB_IMAGE_MINIMIZE = 0,
	WB_IMAGE_UNMAXIMIZE,
	WB_IMAGE_MAXIMIZE,
	WB_IMAGE_CLOSE,

	WB_IMAGES
} WBImageIndices;

/* we will also index image states for convenience */
typedef enum {
	WB_IMAGE_FOCUSED = 0,
	WB_IMAGE_CLICKED,
	WB_IMAGE_HOVERED,
	WB_IMAGE_UNFOCUSED,

	WB_IMAGE_STATES
} WBImageStates;

/* indexing of buttons */
typedef enum {
	WB_BUTTON_MINIMIZE = 0,	// minimize button
	WB_BUTTON_UMAXIMIZE,	// maximize/unmaximize button
	WB_BUTTON_CLOSE,		// close button
	
	WB_BUTTONS				// number of buttons
} WindowButtonIndices;

/* Button state masks */
typedef enum {
	WB_BUTTON_STATE_FOCUSED	= 1 << 0, // [0001]
	WB_BUTTON_STATE_CLICKED	= 1 << 1, // [0010]
	WB_BUTTON_STATE_HOVERED	= 1 << 2, // [0100]
	WB_BUTTON_STATE_HIDDEN	= 1 << 3  // [1000]
} WBButtonState;

/* Applet properties (things that get saved) */
typedef struct {
	gchar		*theme;					// Selected theme name [NULL = "Custom"]
	gchar		***images;				// Absolute paths to images
	gshort 		*eventboxposition;		// Position of eventboxes (left=0 to right=WB_BUTTONS-1)
										// the index represents the button [0=minimize,unmaximize,close]
										// index of -1 means the button will not be put into box
	gchar		*button_layout;			// Button layout string
	gboolean	*button_hidden;			// Indicates whether a button is hidden
	gboolean 	only_maximized,
				hide_on_unmaximized,
				use_metacity_layout,
				click_effect,
				hover_effect;
} WBPreferences;

/* Definition for our button */
typedef struct {
	GtkEventBox 	*eventbox;
	GtkImage 		*image;
	WBButtonState 	state;
} WindowButton;

/* WBApplet definition (inherits from PanelApplet) */
typedef struct {
    PanelApplet		parent;
	
	/* Widgets */
	GtkBox      	*box;				// Main container
	GtkWidget		*window_prefs;		// Preferences window
	
	/* Variables */
	WBPreferences	*prefs;				// Main properties 
	WindowButton	**button;			// Array of buttons
	WnckScreen 		*activescreen;		// Active screen
	WnckWorkspace	*activeworkspace;	// Active workspace
	WnckWindow		*currentwindow,		// Upper most maximized window
					*activewindow,		// Active window
					*rootwindow;		// Root window (desktop)
	gulong			active_handler,		// activewindow's event handler ID
					current_handler;	// currentwindow's event handler ID
	
	PanelAppletOrient orient;			// Panel orientation
	GdkPixbufRotation angle;			// Applet angle
	GtkPackType		packtype;			// Packaging direction of buttons
	
	GdkPixbuf		***pixbufs;			// Images in memory
	
	/* GtkBuilder */
	GtkBuilder 		*prefbuilder;
} WBApplet;

typedef struct {
        PanelAppletClass applet_class;
} WBAppletClass;

GType wb_applet_get_type (void);
WBApplet* wb_applet_new (void);

G_END_DECLS
#endif

/*
Applet structure:

              Panel
                |
                | 
              Applet
                |
                |
     _________ Box _________
    |           |           |
    |           |           | 
EventBox[0] EventBox[1] EventBox[2]
    |           |           |
    |           |           | 
  Image[0]   Image[1]    Image[2]

* note that EventBox/Image pairs (buttons) may be positioned in a different order

*/
