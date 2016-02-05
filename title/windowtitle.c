/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Andrej Belcijan 2009 <andrej.{@}.gmail.dot.com>
 * 
 * main.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * main.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "windowtitle.h"

/* Prototypes */
static void applet_change_background (PanelApplet *, PanelAppletBackgroundType, GdkColor *, GdkPixmap *);
static void active_workspace_changed (WnckScreen *, WnckWorkspace *, gpointer);
static void active_window_changed (WnckScreen *, WnckWindow *, gpointer);
static void active_window_state_changed (WnckWindow *, WnckWindowState, WnckWindowState, gpointer);
static void active_window_nameicon_changed (WnckWindow *, gpointer);
static void current_window_state_changed (WnckWindow *, WnckWindowState, WnckWindowState, gpointer);
static void current_window_nameicon_changed (WnckWindow *, gpointer);
static void viewports_changed (WnckScreen *, gpointer);
static void window_closed (WnckScreen *, WnckWindow *, gpointer);
static void window_opened (WnckScreen *, WnckWindow *, gpointer);
static void about_cb (BonoboUIComponent *, WTApplet *);
static WnckWindow *getUpperMaximized (WTApplet *);
const gchar *getCheckBoxGConfKey (gushort);
void properties_cb (BonoboUIComponent *, WTApplet *, const char *);
void setAlignment(WTApplet *, gfloat);
void toggleHidden(WTApplet *);
void loadPreferencesGConf(WTPreferences *, WTApplet *);
//void loadPreferencesCfg(WTPreferences *, WTApplet *); //TODO
WTPreferences *loadPreferences(WTApplet *);
gchar *getButtonLayoutGConf(WTApplet *, gboolean);

G_DEFINE_TYPE (WTApplet, wt_applet, PANEL_TYPE_APPLET);

static const BonoboUIVerb windowbuttons_menu_verbs [] = {
        BONOBO_UI_UNSAFE_VERB ("WTPreferences", properties_cb),
        BONOBO_UI_UNSAFE_VERB ("WTAbout", about_cb),
        BONOBO_UI_VERB_END
};

WTApplet* wt_applet_new (void) {
        return g_object_new (WT_TYPE_APPLET, NULL);
}

static void wt_applet_init(WTApplet *wtapplet) {
	// Bullshit Function
}

static void wt_applet_class_init (WTAppletClass *klass) {
	// Nothing
}

/* Get our properties (the only properties getter that should be called) */
WTPreferences *loadPreferences(WTApplet *wtapplet) {
	WTPreferences *wtp = g_new0(WTPreferences, 1);

	//TODO
//	if (reading_from_gconf) {
		loadPreferencesGConf(wtp, wtapplet);
//	} else {
//		loadPreferencesCfg(wtp, wtapplet);
//	}

	/*
	//Suggestion from #gtk+
	 GtkSettings *gtks;
	if (gtk_widget_has_screen (GTK_WIDGET(wtapplet))) gtks = gtk_settings_get_for_screen (gtk_widget_get_screen (GTK_WIDGET(wtapplet))); else gtks = gtk_settings_get_default();
	gchar *font_name;
	g_object_get (gtks, "gtk-font-name", &font_name, NULL);
	wtp->title_font = font_name;
	*/
	 
	return wtp;
}

/* In case we're reading the properties from GConf */
void loadPreferencesGConf(WTPreferences *wtp, WTApplet *wtapplet) {
	wtp->only_maximized = panel_applet_gconf_get_bool(PANEL_APPLET(wtapplet), GCK_ONLY_MAXIMIZED, NULL);
	wtp->hide_on_unmaximized = panel_applet_gconf_get_bool(PANEL_APPLET(wtapplet), GCK_HIDE_ON_UNMAXIMIZED, NULL);
	wtp->hide_icon = panel_applet_gconf_get_bool(PANEL_APPLET(wtapplet), GCK_HIDE_ICON, NULL);
	wtp->hide_title = panel_applet_gconf_get_bool(PANEL_APPLET(wtapplet), GCK_HIDE_TITLE, NULL);
	wtp->alignment = panel_applet_gconf_get_float(PANEL_APPLET(wtapplet), GCK_ALIGNMENT, NULL);
	wtp->swap_order = panel_applet_gconf_get_bool(PANEL_APPLET(wtapplet), GCK_SWAP_ORDER, NULL);
	wtp->custom_style = panel_applet_gconf_get_bool(PANEL_APPLET(wtapplet), GCK_CUSTOM_STYLE, NULL);
	wtp->title_font = panel_applet_gconf_get_string(PANEL_APPLET(wtapplet), GCK_TITLE_FONT, NULL);
	wtp->title_color = panel_applet_gconf_get_string(PANEL_APPLET(wtapplet), GCK_TITLE_COLOR_FG, NULL);
}

/* The About dialog */
static void about_cb (BonoboUIComponent *uic, WTApplet *applet) {
        static const gchar *authors [] = {
		"Andrej Belcijan <{andrejx} at {gmail.com}>",
		" ",
		"Special thanks to guys from GimpNet channels",
		"#gnome-hackers and #gtk+",
		"for answering my noob questions.",
		NULL
	};

	const gchar *artists[] = {
		"Nasser Alshammari <{designernasser} at {gmail.com}>",
		NULL
	};
	
	const gchar *documenters[] = {
        "Andrej Belcijan <{andrejx} at {gmail.com}>",
		NULL
	};

	GdkPixbuf *logo = gdk_pixbuf_new_from_file (LOCATION_MAIN"/windowtitle.png", NULL);

	gtk_show_about_dialog (NULL,
		"version",	VERSION,
		"comments",	_("Window title for your GNOME Panel."),
		"copyright",	"\xC2\xA9 2009 Andrej Belcijan",
		"authors",	authors,
	    "artists",	artists,	    
		"documenters",	documenters,
		"translator-credits",	_("translator-credits"),
		"logo",		logo,
   	    "website",	"http://www.gnome-look.org/content/show.php?content=103732",
		"website-label", _("Window Applets on Gnome-Look"),
		NULL);
}

/* Returns the highest maximized window */
static WnckWindow *getUpperMaximized (WTApplet *wtapplet) {
	GList *windows = wnck_screen_get_windows_stacked(wtapplet->activescreen);
	WnckWindow *returnwindow = NULL;

	while (windows) {
		if (wnck_window_is_maximized(windows->data)) {
			//if(wnck_window_is_on_workspace(windows->data, wtapplet->activeworkspace))
			if (!wnck_window_is_minimized(windows->data))
				if (wnck_window_is_in_viewport(windows->data, wtapplet->activeworkspace))
					returnwindow = windows->data;
		}
		windows = windows->next;
	}
	 
	// start tracking the new current window
	if (wtapplet->currentwindow) {
		if (g_signal_handler_is_connected(G_OBJECT(wtapplet->currentwindow), wtapplet->current_handler_state))
			g_signal_handler_disconnect(G_OBJECT(wtapplet->currentwindow), wtapplet->current_handler_state);
		if (g_signal_handler_is_connected(G_OBJECT(wtapplet->currentwindow), wtapplet->current_handler_name))
			g_signal_handler_disconnect(G_OBJECT(wtapplet->currentwindow), wtapplet->current_handler_name);
		if (g_signal_handler_is_connected(G_OBJECT(wtapplet->currentwindow), wtapplet->current_handler_icon))
			g_signal_handler_disconnect(G_OBJECT(wtapplet->currentwindow), wtapplet->current_handler_icon); 
	}
	if (returnwindow) {
		// maxed window was found
		wtapplet->current_handler_state = g_signal_connect(G_OBJECT(returnwindow),
		                                             "state-changed",
		                                             G_CALLBACK (current_window_state_changed),
		                                             wtapplet);
		wtapplet->current_handler_name = g_signal_connect(G_OBJECT(returnwindow),
		                                             "name-changed",
		                                             G_CALLBACK (current_window_nameicon_changed),
		                                             wtapplet);		
		wtapplet->current_handler_icon = g_signal_connect(G_OBJECT(returnwindow),
		                                             "icon-changed",
		                                             G_CALLBACK (current_window_nameicon_changed),
		                                             wtapplet);	
		return returnwindow;
	} else {
		// maxed window was not found
		return wtapplet->rootwindow;
		//return wnck_screen_get_windows_stacked(wtapplet->activescreen)->data;
	}
	//WARNING: if this function returns NULL, applet won't detect clicks!
}

/* Updates the images according to preferences and the current window situation */
void updateTitle(WTApplet *wtapplet) {
	WnckWindow *controlledwindow;
	gchar *title_text;
	
	if (wtapplet->prefs->only_maximized) {
		controlledwindow = wtapplet->currentwindow;
	} else {
		controlledwindow = wtapplet->activewindow;
	}

	if (controlledwindow == wtapplet->rootwindow) {
		// we're on desktop
		if (wtapplet->prefs->hide_on_unmaximized) {
			// clear everything
			gtk_image_clear(wtapplet->icon);
			title_text = "";
		} else {
			// display "custom" icon/title (TODO: customization via preferences)
			gtk_image_set_from_stock(wtapplet->icon, GTK_STOCK_HOME, GTK_ICON_SIZE_MENU);
			title_text = "Desktop";
		}
	} else {
		// we're updating window info
		wtapplet->icon_pixbuf = gdk_pixbuf_rotate_simple(
									gdk_pixbuf_scale_simple(
										wnck_window_get_icon(controlledwindow),
										16, 16,
										GDK_INTERP_BILINEAR
									),
									wtapplet->angle
								);
		gtk_image_set_from_pixbuf(wtapplet->icon, wtapplet->icon_pixbuf);
		title_text = (gchar*)wnck_window_get_name(controlledwindow);
	}
	
	if (controlledwindow) {
		if (wtapplet->prefs->custom_style) {
			title_text = g_markup_printf_escaped("<span font=\"%s\" color=\"%s\">%s</span>",
					                        	 wtapplet->prefs->title_font,
					                             wtapplet->prefs->title_color,
					                             title_text);
			gtk_label_set_markup (GTK_LABEL(wtapplet->title), title_text);
			g_free(title_text);
		} else {
			gtk_label_set_text(wtapplet->title, title_text); //just use system fonts
		}
	}
}

/* Called when panel background is changed */
static void applet_change_background (PanelApplet *applet,
                                   PanelAppletBackgroundType type,
                                   GdkColor  *colour,
                                   GdkPixmap *pixmap)
{
									   
	GtkRcStyle *rc_style;
	GtkStyle *style;
								   
	// reset style
	gtk_widget_set_style (GTK_WIDGET (applet), NULL);
	rc_style = gtk_rc_style_new ();
	gtk_widget_modify_style (GTK_WIDGET (applet), rc_style);
	gtk_rc_style_unref (rc_style);

	switch (type) {
		case PANEL_NO_BACKGROUND:
			break;
		case PANEL_COLOR_BACKGROUND:
			gtk_widget_modify_bg (GTK_WIDGET (applet), GTK_STATE_NORMAL, colour);
			break;
		case PANEL_PIXMAP_BACKGROUND:
			style = gtk_style_copy (GTK_WIDGET (applet)->style);

			if (style->bg_pixmap[GTK_STATE_NORMAL])
			g_object_unref (style->bg_pixmap[GTK_STATE_NORMAL]);

			style->bg_pixmap[GTK_STATE_NORMAL] = g_object_ref (pixmap);
			gtk_widget_set_style (GTK_WIDGET (applet), style);
			g_object_unref (style);
			break;
	}
										
}

/* Triggers when a new window has been opened */
// in case a new maximized non-active window appears
static void window_opened (WnckScreen *screen,
                           WnckWindow *window,
                           gpointer user_data) {
	WTApplet *wtapplet;

	wtapplet = WT_APPLET(user_data);
	if (wtapplet->prefs->only_maximized) {
		wtapplet->currentwindow = getUpperMaximized(wtapplet);
	}

	//updateTitle(wtapplet); //not required(?)
}

/* Triggers when a window has been closed */
// in case the last maximized window was closed
static void window_closed (WnckScreen *screen,
                           WnckWindow *window,
                           gpointer user_data) {
	WTApplet *wtapplet;

	wtapplet = WT_APPLET(user_data);
	if (wtapplet->prefs->only_maximized) {
		wtapplet->currentwindow = getUpperMaximized(wtapplet);
	}

	updateTitle(wtapplet); // required when closing window in the background
}

/* Triggers when a new active window is selected */
static void active_window_changed (WnckScreen *screen,
                                   WnckWindow *previous,
                                   gpointer user_data) {
	WTApplet *wtapplet;
	
	wtapplet = WT_APPLET(user_data);
 
	// Start tracking the new active window:
	//if (g_signal_handler_is_connected(G_OBJECT(wtapplet->activewindow), wtapplet->active_handler_state))
		g_signal_handler_disconnect(G_OBJECT(wtapplet->activewindow), wtapplet->active_handler_state);
	//if (g_signal_handler_is_connected(G_OBJECT(wtapplet->activewindow), wtapplet->active_handler_name))
		g_signal_handler_disconnect(G_OBJECT(wtapplet->activewindow), wtapplet->active_handler_name);
	//if (g_signal_handler_is_connected(G_OBJECT(wtapplet->activewindow), wtapplet->active_handler_icon))
		g_signal_handler_disconnect(G_OBJECT(wtapplet->activewindow), wtapplet->active_handler_icon);

	wtapplet->activewindow = wnck_screen_get_active_window(screen);
	wtapplet->rootwindow = wnck_screen_get_windows_stacked(wtapplet->activescreen)->data;									   

	if (wtapplet->activewindow) {
		wtapplet->active_handler_state = g_signal_connect(G_OBJECT (wtapplet->activewindow), "state-changed", G_CALLBACK (active_window_state_changed), wtapplet);
		wtapplet->active_handler_name = g_signal_connect(G_OBJECT (wtapplet->activewindow), "name-changed", G_CALLBACK (active_window_nameicon_changed), wtapplet);
		wtapplet->active_handler_icon = g_signal_connect(G_OBJECT (wtapplet->activewindow), "icon-changed", G_CALLBACK (active_window_nameicon_changed), wtapplet);
										   
		// if the newly selected window is maximized it is also the current window
		if (wtapplet->prefs->only_maximized) {
			//don't use getUpperMaximized(wtapplet) for both! we don't wanna track it twice
			if (wnck_window_is_maximized(wtapplet->activewindow)) {
				wtapplet->currentwindow = wtapplet->activewindow;
			} else {
				wtapplet->currentwindow = getUpperMaximized(wtapplet);
			}
		}

		wtapplet->focused = TRUE;

		updateTitle(wtapplet);
	}
}

/* Trigger when activewindow's state changes */
static void active_window_state_changed (WnckWindow *window,
                                         WnckWindowState changed_mask,
                                         WnckWindowState new_state,
                                         gpointer user_data) {
	WTApplet *wtapplet;
	
	wtapplet = WT_APPLET(user_data);
	if (wtapplet->prefs->only_maximized) {
		wtapplet->currentwindow = getUpperMaximized(wtapplet);
	}
  	wtapplet->rootwindow = wnck_screen_get_windows_stacked(wtapplet->activescreen)->data;

	if (new_state & (WNCK_WINDOW_STATE_MAXIMIZED_HORIZONTALLY | WNCK_WINDOW_STATE_MAXIMIZED_VERTICALLY)) {
		wtapplet->focused = TRUE;
	} else {
		if (wtapplet->prefs->only_maximized) {
				wtapplet->focused = FALSE;
		}
	}
	
	updateTitle(wtapplet);
}

/* Triggers when activewindow's name changes */
static void active_window_nameicon_changed (WnckWindow *window, gpointer user_data) {
	WTApplet *wtapplet = WT_APPLET(user_data);
	updateTitle(wtapplet);
}

/* Triggers when currentwindow's state changes */
static void current_window_state_changed (WnckWindow *window,
                                          WnckWindowState changed_mask,
                                          WnckWindowState new_state,
                                          gpointer user_data)
{
	WTApplet *wtapplet;
	
	wtapplet = WT_APPLET(user_data);
	if (wtapplet->prefs->only_maximized) {	
		wtapplet->currentwindow = getUpperMaximized(wtapplet);
	}
	wtapplet->rootwindow = wnck_screen_get_windows_stacked(wtapplet->activescreen)->data;
	
	updateTitle(wtapplet);
}

/* Triggers when currentwindow's name OR ICON changes */
static void current_window_nameicon_changed(WnckWindow *window, gpointer user_data) {
	WTApplet *wtapplet = WT_APPLET(user_data);
	updateTitle(wtapplet);
}

/* Triggers when user changes viewports (Compiz?) */
static void viewports_changed (WnckScreen *screen,
                               gpointer user_data)
{
	WTApplet *wtapplet;

	wtapplet = WT_APPLET(user_data);

	wtapplet->activeworkspace = wnck_screen_get_active_workspace(screen);
	wtapplet->activewindow = wnck_screen_get_active_window(screen);

	if (wtapplet->prefs->only_maximized) {
		wtapplet->currentwindow = getUpperMaximized(wtapplet);
	}

	updateTitle(wtapplet);
}

/* Triggers when.... ? not sure... (Metacity?) */
static void active_workspace_changed (WnckScreen *screen,
                                      WnckWorkspace *previous,
                                      gpointer user_data)
{
	WTApplet *wtapplet;

	wtapplet = WT_APPLET(user_data);

	wtapplet->activeworkspace = wnck_screen_get_active_workspace(screen);
	/*
	wtapplet->activewindow = wnck_screen_get_active_window(screen);
	((wtapplet->prefs)->only_maximized) {
		wtapplet->currentwindow = getUpperMaximized(wtapplet);
	}

	updateTitle(wtapplet);
	*/
}

static gboolean icon_clicked (GtkWidget *icon,
                              GdkEventButton *event,
                              gpointer user_data)
{
	if (event->button != 1) return FALSE;
	
	WTApplet *wtapplet;
	WnckWindow *controlledwindow;
	
	wtapplet = WT_APPLET(user_data);

	if (wtapplet->prefs->only_maximized) {
		controlledwindow = wtapplet->currentwindow;
	} else {
		controlledwindow = wtapplet->activewindow;
	}

	// single click:
	if (controlledwindow) {
		wnck_window_activate(controlledwindow, gtk_get_current_event_time());
	}

	// double click:
	if (event->type==GDK_2BUTTON_PRESS) {
		wnck_window_close(controlledwindow, GDK_CURRENT_TIME);
	}		

	return TRUE;
}

static gboolean title_clicked (GtkWidget *title,
                               GdkEventButton *event,
                               gpointer user_data)
{
	if (event->button != 1) return FALSE;
	
	WTApplet *wtapplet;
	WnckWindow *controlledwindow;
	
	wtapplet = WT_APPLET(user_data);

	if (wtapplet->prefs->only_maximized) {
		controlledwindow = wtapplet->currentwindow;
	} else {
		controlledwindow = wtapplet->activewindow;
	}

	// single click:
	if (controlledwindow) {
		wnck_window_activate(controlledwindow, gtk_get_current_event_time());
	}

	// double click:
	//if (event->type==GDK_2BUTTON_PRESS || event->type==GDK_3BUTTON_PRESS) {
		if(event->type==GDK_2BUTTON_PRESS) {
			if(wnck_window_is_maximized(controlledwindow)) {
				wnck_window_unmaximize(controlledwindow);
			} else {
				wnck_window_maximize(controlledwindow);
			}
		}	
	//}

	return TRUE;
}

/* Places widgets in box accordingly with angle and order */
void placeWidgets (WTApplet *wtapplet) {
	// set box orientation
	if (wtapplet->angle == GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE || wtapplet->angle == GDK_PIXBUF_ROTATE_CLOCKWISE) {
		gtk_orientable_set_orientation(GTK_ORIENTABLE(wtapplet->box), GTK_ORIENTATION_VERTICAL);
	} else {
		gtk_orientable_set_orientation(GTK_ORIENTABLE(wtapplet->box), GTK_ORIENTATION_HORIZONTAL);
	}

	// set packing order
	if (!(wtapplet->prefs->swap_order == wtapplet->packtype)) {
		gtk_box_pack_start(wtapplet->box, GTK_WIDGET(wtapplet->eb_title), TRUE, TRUE, 0);
		gtk_box_pack_start(wtapplet->box, GTK_WIDGET(wtapplet->eb_icon), FALSE, TRUE, 0);
	} else {
		gtk_box_pack_start(wtapplet->box, GTK_WIDGET(wtapplet->eb_icon), FALSE, TRUE, 0);
		gtk_box_pack_start(wtapplet->box, GTK_WIDGET(wtapplet->eb_title), TRUE, TRUE, 0);
	}
}

/* Reloads all widgets */
void reloadWidgets (WTApplet *wtapplet) {
	g_object_ref(wtapplet->eb_icon);
	g_object_ref(wtapplet->eb_title);
	gtk_container_remove(GTK_CONTAINER(wtapplet->box), GTK_WIDGET(wtapplet->eb_icon));
	gtk_container_remove(GTK_CONTAINER(wtapplet->box), GTK_WIDGET(wtapplet->eb_title));
	placeWidgets(wtapplet);
	g_object_unref(wtapplet->eb_icon);
	g_object_unref(wtapplet->eb_title);
}

/* Sets alignment to title according to panel orientation */
void setAlignment (WTApplet *wtapplet, gfloat alignment) {
	if (wtapplet->angle == GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE || wtapplet->angle == GDK_PIXBUF_ROTATE_CLOCKWISE) {
		if (wtapplet->angle == GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE) {
			gtk_misc_set_alignment(GTK_MISC(wtapplet->title), 0.5, 1-alignment);
		} else {
			gtk_misc_set_alignment(GTK_MISC(wtapplet->title), 0.5, alignment);
		}
	} else {
		gtk_misc_set_alignment(GTK_MISC(wtapplet->title), alignment, 0.5);
	}
}

/* Returns the apropriate angle for respective panel orientations */
GdkPixbufRotation getOrientAngle (PanelAppletOrient orient) {
	GdkPixbufRotation new_angle;
	if (orient == PANEL_APPLET_ORIENT_RIGHT) {
		new_angle = GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE;
	} else if (orient == PANEL_APPLET_ORIENT_LEFT) {
		new_angle = GDK_PIXBUF_ROTATE_CLOCKWISE;
	} else {
		new_angle = GDK_PIXBUF_ROTATE_NONE;
	}
	return new_angle;
}

/* Returns packtype according to panel angle */
GtkPackType getPackType(GdkPixbufRotation angle) {
	if (angle == GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE) {
		return GTK_PACK_END;
	} else {
		return GTK_PACK_START;
	}
}

/* Sets angle to widgets */
void rotateWidgets (WTApplet *wtapplet, GdkPixbufRotation angle) {
	gtk_label_set_angle( wtapplet->title, angle );
	setAlignment(wtapplet, (gfloat)wtapplet->prefs->alignment);
}

/* Triggered when a different panel orientation is detected */
void applet_change_orient (PanelApplet *panelapplet, PanelAppletOrient orient, gpointer user_data) {
	WTApplet *wtapplet = WT_APPLET(user_data);

	if (orient != wtapplet->orient) {
		wtapplet->orient = orient;
		wtapplet->angle = getOrientAngle(wtapplet->orient);
		wtapplet->packtype = getPackType(wtapplet->angle);
		
		rotateWidgets(wtapplet, wtapplet->angle);
		reloadWidgets(wtapplet);
		updateTitle(wtapplet);
	}
}

/* Do the actual applet initialization */
static void init_wtapplet (WTApplet *wtapplet) {
	wtapplet->activescreen = wnck_screen_get_default();
	wnck_screen_force_update(wtapplet->activescreen);
	wtapplet->activeworkspace = wnck_screen_get_active_workspace(wtapplet->activescreen);
	wtapplet->activewindow = wnck_screen_get_active_window(wtapplet->activescreen);
	//wtapplet->rootwindow = wnck_screen_get_windows_stacked(wtapplet->activescreen)->data; //!!! WARNING! THIS CRASHES DURING BOOTUP - MUST BE CALLED ELSEWHERE... but where?
	wtapplet->rootwindow = NULL;
	wtapplet->prefs = loadPreferences(wtapplet);
	wtapplet->prefbuilder = gtk_builder_new();
	wtapplet->box = GTK_BOX(gtk_hbox_new(FALSE, 0));
	wtapplet->icon = GTK_IMAGE(gtk_image_new());
	wtapplet->title = GTK_LABEL(gtk_label_new(NULL));
	wtapplet->eb_icon = GTK_EVENT_BOX(gtk_event_box_new());
	wtapplet->eb_title = GTK_EVENT_BOX(gtk_event_box_new());
	wtapplet->orient = panel_applet_get_orient(PANEL_APPLET(wtapplet));
	wtapplet->angle = getOrientAngle(wtapplet->orient);
	wtapplet->packtype = getPackType(wtapplet->angle);
	
	if (wtapplet->prefs->only_maximized) {
		wtapplet->currentwindow = getUpperMaximized(wtapplet);
	}
	
	// Widgets to eventboxes, eventboxes to box
	GTK_WIDGET_SET_FLAGS (wtapplet->icon, GTK_CAN_FOCUS);
	GTK_WIDGET_SET_FLAGS (wtapplet->title, GTK_CAN_FOCUS);
	gtk_container_add (GTK_CONTAINER (wtapplet->eb_icon), GTK_WIDGET(wtapplet->icon));
	gtk_container_add (GTK_CONTAINER (wtapplet->eb_title), GTK_WIDGET(wtapplet->title));
	gtk_event_box_set_visible_window (wtapplet->eb_icon, FALSE);
	gtk_event_box_set_visible_window (wtapplet->eb_title, FALSE);
	// Set spacing between icon and label to 5:
	//gtk_box_set_spacing(wtapplet->box, 5);
	gtk_misc_set_padding(GTK_MISC(wtapplet->icon), 5, 0);
	setAlignment(wtapplet, (gfloat)wtapplet->prefs->alignment);
	// Set event handling (icon & title clicks)
	g_signal_connect(G_OBJECT (wtapplet->eb_icon), "button-press-event", G_CALLBACK (icon_clicked), wtapplet);
	g_signal_connect(G_OBJECT (wtapplet->eb_title), "button-press-event", G_CALLBACK (title_clicked), wtapplet);
	
	// Rotate & place elements
	rotateWidgets(wtapplet, wtapplet->angle);
	placeWidgets(wtapplet);

	// Add box to applet
	gtk_container_add (GTK_CONTAINER(wtapplet), GTK_WIDGET(wtapplet->box));
	
	// Global window tracking
	g_signal_connect(wtapplet->activescreen, "active-window-changed", G_CALLBACK (active_window_changed), wtapplet);
	g_signal_connect(wtapplet->activescreen, "viewports-changed", G_CALLBACK (viewports_changed), wtapplet);
	g_signal_connect(wtapplet->activescreen, "active-workspace-changed", G_CALLBACK (active_workspace_changed), wtapplet);
	g_signal_connect(wtapplet->activescreen, "window-closed", G_CALLBACK (window_closed), wtapplet);
	g_signal_connect(wtapplet->activescreen, "window-opened", G_CALLBACK (window_opened), wtapplet);
	
	g_signal_connect(G_OBJECT (wtapplet), "change-background", G_CALLBACK (applet_change_background), NULL);
	g_signal_connect(G_OBJECT (wtapplet), "change-orient", G_CALLBACK (applet_change_orient), wtapplet);

	// ???: Is this still necessary?
	wtapplet->active_handler_state = 
		g_signal_connect(G_OBJECT (wtapplet->activewindow), "state-changed", G_CALLBACK (active_window_state_changed), wtapplet);
	wtapplet->active_handler_name = 
		g_signal_connect(G_OBJECT (wtapplet->activewindow), "name-changed", G_CALLBACK (active_window_nameicon_changed), wtapplet);
	wtapplet->active_handler_icon = 
		g_signal_connect(G_OBJECT (wtapplet->activewindow), "icon-changed", G_CALLBACK (active_window_nameicon_changed), wtapplet);
}

/* Hide/unhide stuff according to preferences */
void toggleHidden (WTApplet *wtapplet) {
	if (wtapplet->prefs->hide_icon) {
		gtk_widget_hide (GTK_WIDGET(wtapplet->icon));
	} else {
		gtk_widget_show (GTK_WIDGET(wtapplet->icon));
	}
	
	if (wtapplet->prefs->hide_title) {
		gtk_widget_hide (GTK_WIDGET(wtapplet->title));

		//TODO: this could be a separate option:
		panel_applet_set_flags ((PanelApplet*)wtapplet, PANEL_APPLET_EXPAND_MINOR);
		// Unset minimal box size:
		gtk_widget_set_size_request(GTK_WIDGET(wtapplet->box), -1, -1);
	} else {
		gtk_widget_show (GTK_WIDGET(wtapplet->title));

		//TODO: this could be a separate option:
		panel_applet_set_flags ((PanelApplet*)wtapplet, PANEL_APPLET_EXPAND_MINOR | PANEL_APPLET_EXPAND_MAJOR);
		// Set minimal box size to 5x1, the next time it is resized it will not exceed max panelapplet size:
		gtk_widget_set_size_request(GTK_WIDGET(wtapplet->box), 5, 1);
	}
}

/* Initial function that draws the applet */
static gboolean windowtitle_applet_fill (PanelApplet *applet, const gchar *iid, gpointer data) {
	if (strcmp (iid, APPLET_OAFIID) != 0) return FALSE;
	
	g_set_application_name (_(APPLET_NAME)); //GLib-WARNING **: g_set_application_name() called multiple times
	panel_applet_set_flags (applet, PANEL_APPLET_EXPAND_MINOR | PANEL_APPLET_EXPAND_MAJOR);
	panel_applet_add_preferences (applet, GCONF_PREFS, NULL);

	init_wtapplet(WT_APPLET(applet));

	/* --- Context Menu --- */
	static const char context_menu_xml [] =
	   "<popup name=\"button3\">\n"
	   "   <menuitem name=\"Properties Item\" "
	   "             verb=\"WTPreferences\" "
	   "           _label=\"_Preferences...\"\n"
	   "          pixtype=\"stock\" "
	   "          pixname=\"gtk-properties\"/>\n"
	   "   <menuitem name=\"About Item\" "
	   "             verb=\"WTAbout\" "
	   "           _label=\"_About...\"\n"
	   "          pixtype=\"stock\" "
	   "          pixname=\"gtk-about\"/>\n"
	   "</popup>\n";
	//last parameter here will be the second parameter (WTApplet) in all menu callback functions (properties, about...) !!!
	panel_applet_setup_menu (applet, context_menu_xml, windowbuttons_menu_verbs, applet);

	/* Draw the damn thing */
	updateTitle (WT_APPLET(applet));
	gtk_widget_show_all (GTK_WIDGET (applet));
	
	/* We need this because things have to be hidden after we 'show' the applet */
	toggleHidden (WT_APPLET (applet));

	return TRUE;
}

PANEL_APPLET_BONOBO_FACTORY (APPLET_OAFIID_FACTORY,
							 WT_TYPE_APPLET,
							 "windowtitle",//APPLET_NAME,
							 "0",//VERSION,
							 windowtitle_applet_fill,
							 NULL);