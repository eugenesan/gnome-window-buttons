/***************************************************************************
 *            windowbuttons.c
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
 
#include "windowbuttons.h"

/* Prototypes */
static void applet_change_background (PanelApplet *, PanelAppletBackgroundType, GdkColor *, GdkPixmap *);
static void active_workspace_changed (WnckScreen *, WnckWorkspace *, gpointer);
static void active_window_changed (WnckScreen *, WnckWindow *, gpointer);
static void active_window_state_changed (WnckWindow *, WnckWindowState, WnckWindowState, gpointer);
static void current_window_state_changed (WnckWindow *, WnckWindowState, WnckWindowState, gpointer);
static void viewports_changed (WnckScreen *, gpointer);
static void window_closed (WnckScreen *, WnckWindow *, gpointer);
static void window_opened (WnckScreen *, WnckWindow *, gpointer);
static void about_cb (BonoboUIComponent *, WBApplet *);
static gboolean hover_enter (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
static gboolean hover_leave (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
static gboolean button_press (GtkWidget *, GdkEventButton *, gpointer);
static gboolean button_release (GtkWidget *, GdkEventButton *, gpointer);
static WnckWindow *getUpperMaximized (WBApplet *);
void properties_cb (BonoboUIComponent *, WBApplet *, const char *);
void setCustomLayout (WBPreferences *, gchar *);
void placeButtons(WBApplet *);
void reloadButtons(WBApplet *);
void toggleHidden(WBApplet *);
void loadPreferencesGConf(WBPreferences *, WBApplet *);
//void loadPreferencesCfg(WBPreferences *, WBApplet *); //TODO
WBPreferences *loadPreferences(WBApplet *);
gshort *getEBPos(gchar *);
gchar *getButtonLayoutGConf(WBApplet *, gboolean);
gchar *getMetacityLayout(void);
const gchar *getImageGConfKey(gushort, gushort);
const gchar *getCheckBoxGConfKey (gushort);

GdkPixbuf ***getPixbufs(WBApplet *);

//G_DEFINE_TYPE(TN, t_n, T_P) G_DEFINE_TYPE_EXTENDED (TN, t_n, T_P, 0, {})
//This line is very important! It defines how the requested functions will be called
G_DEFINE_TYPE (WBApplet, wb_applet, PANEL_TYPE_APPLET);

static const BonoboUIVerb windowbuttons_menu_verbs [] = {
        BONOBO_UI_UNSAFE_VERB ("WBPreferences", properties_cb),
        BONOBO_UI_UNSAFE_VERB ("WBAbout", about_cb),
        BONOBO_UI_VERB_END
};

WBApplet* wb_applet_new (void) {
        return g_object_new (WB_TYPE_APPLET, NULL);
}

static void wb_applet_init(WBApplet *wbapplet) {
	// Bullshit Function
}

static void wb_applet_class_init (WBAppletClass *klass) {
	// Nothing
}

/* Parses Metacity's GConf entry to get the button order */
gshort *getEBPos(gchar *button_layout) {
	gshort *ebps = g_new(gshort, WB_BUTTONS);
	int i, j;

	// in case we got a faulty button_layout:
	for (i=0; i<WB_BUTTONS; i++) ebps[i] = i;
		if (button_layout == NULL || *button_layout == '\0')
			return ebps;
	
//	for(i=0; i<WB_BUTTONS; i++) ebps[i] = -1; //set to -1 if we don't find some
	gchar **pch = g_strsplit_set(button_layout, ":, ", -1);
	i = 0; j = 0;
	while (pch[j]) {
		if(!g_strcmp0(pch[j], "minimize")) ebps[0] = i++;
		if(!g_strcmp0(pch[j], "maximize")) ebps[1] = i++;
		if(!g_strcmp0(pch[j], "close"))	 ebps[2] = i++;
		j++;
	}
	
	g_strfreev(pch);
	return ebps;
}

/* Get our properties (the only properties getter that should be called) */
WBPreferences *loadPreferences(WBApplet *wbapplet) {
	WBPreferences *wbp = g_new0(WBPreferences, 1);
	int i;
	
	wbp->images = g_new(gchar**, WB_IMAGE_STATES);
	wbp->button_hidden = g_new(gboolean, WB_BUTTONS);
	
	for(i=0; i<WB_IMAGE_STATES; i++) {
		wbp->images[i] = g_new(gchar*, WB_IMAGES);
	}

	//TODO
//	if (reading_from_gconf) {
		loadPreferencesGConf(wbp, wbapplet);
//	} else {
//		loadPreferencesCfg(wbp, wbapplet);
//	}

	wbp->eventboxposition = getEBPos(wbp->button_layout);
	
	return wbp;
}

gchar *getMetacityLayout() {
	GConfClient *gconfclient = gconf_client_get_default();
	gchar *retval = gconf_client_get_string(gconfclient,
	                                        GCONF_METACITY_BUTTON_LAYOUT,
	                                        NULL);
	g_object_unref(gconfclient);
	return retval;
}

/* In case we're reading the properties from GConf */
void loadPreferencesGConf(WBPreferences *wbp, WBApplet *wbapplet) {
	int i, j;

	for (i=0; i<WB_BUTTONS; i++) {
		wbp->button_hidden[i] = panel_applet_gconf_get_bool(PANEL_APPLET(wbapplet), getCheckBoxGConfKey(i), NULL);
	}
	
	for (i=0; i<WB_IMAGE_STATES; i++) {
		for (j=0; j<WB_IMAGES; j++) {
			wbp->images[i][j] = panel_applet_gconf_get_string(PANEL_APPLET(wbapplet), getImageGConfKey(i,j), NULL);
		}
	}

	wbp->only_maximized = panel_applet_gconf_get_bool(PANEL_APPLET(wbapplet), GCK_CHECKBOX_ONLY_MAXIMIZED, NULL);
	wbp->hide_on_unmaximized = panel_applet_gconf_get_bool(PANEL_APPLET(wbapplet), GCK_CHECKBOX_HIDE_ON_UNMAXIMIZED, NULL);
	wbp->click_effect = panel_applet_gconf_get_bool(PANEL_APPLET(wbapplet), GCK_CHECKBOX_CLICK_EFFECT, NULL);
	wbp->hover_effect = panel_applet_gconf_get_bool(PANEL_APPLET(wbapplet), GCK_CHECKBOX_HOVER_EFFECT, NULL);
	wbp->use_metacity_layout = panel_applet_gconf_get_bool(PANEL_APPLET(wbapplet), GCK_CHECKBOX_USE_METACITY_LAYOUT, NULL);
	wbp->theme = panel_applet_gconf_get_string(PANEL_APPLET(wbapplet), GCK_THEME, NULL);
	
	// read positions from GConf
	if (wbp->use_metacity_layout) {
		wbp->button_layout = getMetacityLayout();
	} else {
		wbp->button_layout = panel_applet_gconf_get_string(PANEL_APPLET(wbapplet), GCK_BUTTON_LAYOUT, NULL);
	}
}

/* The About dialog */
static void about_cb (BonoboUIComponent *uic, WBApplet *applet) {
    static const gchar *authors [] = {
		"Andrej Belcijan <{andrejx} at {gmail.com}>",
		" ",
		"Special thanks to guys from GimpNet channels",
		"#gnome-hackers and #gtk+",
		"for answering my noob questions.",
		" ",
		"Also contributed: quiescens",
		NULL
	};

	const gchar *artists[] = {
		"Nasser Alshammari <{designernasser} at {gmail.com}> for logo design",
        "Andrej Belcijan <{andrejx} at {gmail.com}>",
		"Jeff M. Hubbard <{jeffmhubbard} at {gmail.com}> for theme \"Elementary\"",
		"Giacomo Porrà for theme \"Ambiance\"",
		"Gaurang Arora for theme \"Dust-Invert\"",
		NULL
	};
	
	const gchar *documenters[] = {
        "Andrej Belcijan <{andrejx} at {gmail.com}>",
		NULL
	};

	GdkPixbuf *logo = gdk_pixbuf_new_from_file (LOCATION_MAIN"/windowbuttons_logo.png", NULL);

	gtk_show_about_dialog (NULL,
		"version",	VERSION,
		"comments",	_("Window buttons for your GNOME Panel."),
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
static WnckWindow *getUpperMaximized (WBApplet *wbapplet) {
	GList *windows = wnck_screen_get_windows_stacked(wbapplet->activescreen);
	WnckWindow *returnwindow = NULL;
	while (windows && windows->data) {
		if (wnck_window_is_maximized(windows->data)) {
			//if(wnck_window_is_on_workspace(windows->data, wbapplet->activeworkspace))
			if (!wnck_window_is_minimized(windows->data))
				if (wnck_window_is_in_viewport(windows->data, wbapplet->activeworkspace))
					returnwindow = windows->data;
		}
		windows = windows->next;
	}

	// start tracking the new current window
	if (wbapplet->currentwindow) {
		if (g_signal_handler_is_connected(G_OBJECT(wbapplet->currentwindow), wbapplet->current_handler))
			g_signal_handler_disconnect(G_OBJECT(wbapplet->currentwindow), wbapplet->current_handler);
	}
	if (returnwindow) {
		wbapplet->current_handler = g_signal_connect(G_OBJECT(returnwindow),
		                                             "state-changed",
		                                             G_CALLBACK (current_window_state_changed),
		                                             wbapplet);
	}

	return returnwindow;
}

gushort getImageState (WBButtonState button_state) {
	if (button_state & WB_BUTTON_STATE_CLICKED) {
		return WB_IMAGE_CLICKED;
	} else if (button_state & WB_BUTTON_STATE_HOVERED) {
		return WB_IMAGE_HOVERED;		
	} else if (button_state & WB_BUTTON_STATE_FOCUSED) {
		return WB_IMAGE_FOCUSED;
	} else if (!(button_state & WB_BUTTON_STATE_FOCUSED)) {
		return WB_IMAGE_UNFOCUSED;
	} else {
		// (?)
		return WB_IMAGE_FOCUSED;
	}
}

/* Updates the images according to preferences and the current window situation */
void updateImages (WBApplet *wbapplet) {
	WnckWindow *controlledwindow;
	int i;
	
	if (wbapplet->prefs->only_maximized) {
		controlledwindow = wbapplet->currentwindow;
	} else {
		controlledwindow = wbapplet->activewindow;
	}

	if (controlledwindow == NULL) { //TODO: should be done with rootwindow (be careful! see WTA code)
		/* There are no maximized windows (or any windows at all) left */
		for (i=0; i<WB_BUTTONS; i++) wbapplet->button[i]->state &= ~WB_BUTTON_STATE_FOCUSED;

		/* Hide all buttons if hide_on_unmaximized */
		if (wbapplet->prefs->hide_on_unmaximized) {
			for (i=0; i<WB_BUTTONS; i++) // update states
				wbapplet->button[i]->state |= WB_BUTTON_STATE_HIDDEN;
			//gtk_widget_hide (GTK_WIDGET(wbapplet->box));
		}
	} else {
		if (wbapplet->prefs->hide_on_unmaximized) {
			for (i=0; i<WB_BUTTONS; i++) // update states
				if (wbapplet->prefs->button_hidden[i]) {
					wbapplet->button[i]->state |= WB_BUTTON_STATE_HIDDEN;
				} else {
					wbapplet->button[i]->state &= ~WB_BUTTON_STATE_HIDDEN;
				}
			//gtk_widget_show (GTK_WIDGET(wbapplet->box));	
		}
	}

	toggleHidden(wbapplet);
	
	//update minimize button:
	gtk_image_set_from_pixbuf (wbapplet->button[WB_BUTTON_MINIMIZE]->image, wbapplet->pixbufs[getImageState(wbapplet->button[WB_BUTTON_MINIMIZE]->state)][WB_IMAGE_MINIMIZE]);	
	// update maximize/unmaximize button:
	if (controlledwindow && wnck_window_is_maximized(controlledwindow)) {
		gtk_image_set_from_pixbuf (wbapplet->button[WB_BUTTON_UMAXIMIZE]->image, wbapplet->pixbufs[getImageState(wbapplet->button[WB_BUTTON_UMAXIMIZE]->state)][WB_IMAGE_UNMAXIMIZE]);
	} else {
		gtk_image_set_from_pixbuf (wbapplet->button[WB_BUTTON_UMAXIMIZE]->image, wbapplet->pixbufs[getImageState(wbapplet->button[WB_BUTTON_UMAXIMIZE]->state)][WB_IMAGE_MAXIMIZE]);
	}
	//update close button
	gtk_image_set_from_pixbuf (wbapplet->button[WB_BUTTON_CLOSE]->image, wbapplet->pixbufs[getImageState(wbapplet->button[WB_BUTTON_CLOSE]->state)][WB_IMAGE_CLOSE]);
}

/* Called when panel background is changed */
static void applet_change_background (PanelApplet *applet,
                                   PanelAppletBackgroundType type,
                                   GdkColor  *colour,
                                   GdkPixmap *pixmap) {
	GtkRcStyle *rc_style;
	GtkStyle *style;

	/* reset style */
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
	WBApplet *wbapplet;

	wbapplet = WB_APPLET(user_data);
	if (wbapplet->prefs->only_maximized) {
		wbapplet->currentwindow = getUpperMaximized(wbapplet);
	}

	//updateImages(wbapplet); //not required(?)
}

/* Triggers when a window has been closed */
// in case the last maximized window was closed
static void window_closed (WnckScreen *screen,
                           WnckWindow *window,
                           gpointer user_data) {
	WBApplet *wbapplet;

	wbapplet = WB_APPLET(user_data);
	if (wbapplet->prefs->only_maximized) {
		wbapplet->currentwindow = getUpperMaximized(wbapplet);
	}

	updateImages(wbapplet); //required to hide buttons when last window is closed
}

/* Triggers when a new active window is selected */
static void active_window_changed (WnckScreen *screen,
                                   WnckWindow *previous,
                                   gpointer user_data) {
	WBApplet *wbapplet;
	int i;
	
	wbapplet = WB_APPLET(user_data);

	// Start tracking the new active window:
	if (wbapplet->activewindow)
		//if(g_signal_handler_is_connected(G_OBJECT(wbapplet->activewindow), wbapplet->active_handler))
			g_signal_handler_disconnect(G_OBJECT(wbapplet->activewindow), wbapplet->active_handler);
	wbapplet->activewindow = wnck_screen_get_active_window(screen);

	// if we got NULL it would start flickering (but we shouldn't get NULL anymore)
	if (wbapplet->activewindow) {
		wbapplet->active_handler = g_signal_connect(G_OBJECT (wbapplet->activewindow), "state-changed", G_CALLBACK (active_window_state_changed), wbapplet);

		// if the newly selected window is maximized it is also the current window
		if (wbapplet->prefs->only_maximized) {
			//don't use getUpperMaximized(wbapplet) for both! we don't wanna track it twice
				if (wnck_window_is_maximized(wbapplet->activewindow)) {
					wbapplet->currentwindow = wbapplet->activewindow;
				} else {
					wbapplet->currentwindow = getUpperMaximized(wbapplet);
				}
		}

		if (wbapplet->activewindow == wbapplet->currentwindow) {
			// maximized window is on top
			for (i=0; i<WB_BUTTONS; i++) {
				wbapplet->button[i]->state |= WB_BUTTON_STATE_FOCUSED;
			}
		} else {
			if (wbapplet->prefs->only_maximized) {
				for (i=0; i<WB_BUTTONS; i++) {
					wbapplet->button[i]->state &= ~WB_BUTTON_STATE_FOCUSED;
				}
			}
		}

		updateImages(wbapplet);
	}
}

/* Trigger when activewindow's state changes */
static void active_window_state_changed (WnckWindow *window,
                                         WnckWindowState changed_mask,
                                         WnckWindowState new_state,
                                         gpointer user_data) {
	WBApplet *wbapplet;
	int i;
	
	wbapplet = WB_APPLET(user_data);
	if (wbapplet->prefs->only_maximized) {
		wbapplet->currentwindow = getUpperMaximized(wbapplet);
	}

	if ( new_state & (WNCK_WINDOW_STATE_MAXIMIZED_HORIZONTALLY | WNCK_WINDOW_STATE_MAXIMIZED_VERTICALLY) ) {
		for (i=0; i<WB_BUTTONS; i++) {
			wbapplet->button[i]->state |= WB_BUTTON_STATE_FOCUSED;
		}
	} else {
		if (wbapplet->prefs->only_maximized) {
			for (i=0; i<WB_BUTTONS; i++) {
				wbapplet->button[i]->state &= ~WB_BUTTON_STATE_FOCUSED;
			}
		}
	}
	
	updateImages(wbapplet);
}

/* Triggers when currentwindow's state changes */
static void current_window_state_changed (WnckWindow *window,
                                          WnckWindowState changed_mask,
                                          WnckWindowState new_state,
                                          gpointer user_data) {
	WBApplet *wbapplet;
	
	wbapplet = WB_APPLET(user_data);
	if ((wbapplet->prefs)->only_maximized) {	
		wbapplet->currentwindow = getUpperMaximized(wbapplet);
	}
	
	updateImages(wbapplet); //need to hide buttons after click if desktop is below
}

/* Triggers when user changes viewports on Compiz */
// We ONLY need this for Compiz (Metacity doesn't use viewports)
static void viewports_changed (WnckScreen *screen,
                               gpointer user_data) {
	WBApplet *wbapplet;

	wbapplet = WB_APPLET(user_data);

	wbapplet->activeworkspace = wnck_screen_get_active_workspace(screen);
	wbapplet->activewindow = wnck_screen_get_active_window(screen);
	if ((wbapplet->prefs)->only_maximized) {
		wbapplet->currentwindow = getUpperMaximized(wbapplet);
	}
									
	updateImages(wbapplet);
}

/* Triggers when user changes workspace on Metacity (?) */
static void active_workspace_changed (WnckScreen *screen,
                                      WnckWorkspace *previous,
                                      gpointer user_data) {
	WBApplet *wbapplet;

	wbapplet = WB_APPLET(user_data);

	wbapplet->activeworkspace = wnck_screen_get_active_workspace(screen);

	/* //apparently active_window_changed handles this (?)
	wbapplet->activewindow = wnck_screen_get_active_window(screen);
	if((wbapplet->prefs)->only_maximized) {
		wbapplet->currentwindow = getUpperMaximized(wbapplet);
	}
	*/

	//updateImages(wbapplet);
}

/* Called when we release the click on a button */
static gboolean button_release (GtkWidget *event_box,
                               GdkEventButton *event,
                               gpointer data) {
	WBApplet *wbapplet;
	WnckWindow *controlledwindow;
	GtkAllocation imga;
	int i;
	
	if (event->button != 1) return FALSE;

	wbapplet = WB_APPLET(data);

	// Find our button:
	for (i=0; i<WB_BUTTONS; i++) {
		if (GTK_EVENT_BOX(event_box) == wbapplet->button[i]->eventbox) {
			break;
		}
	}

	if (wbapplet->prefs->click_effect) {
		wbapplet->button[i]->state &= ~WB_BUTTON_STATE_CLICKED;
	}
	imga = (GTK_WIDGET(wbapplet->button[i]->image))->allocation; //allocated image size //WARNING! GSEAL! This will be deprecated soon!

	if (!(event->x<0 || event->y<0 || event->x>imga.width || event->y>imga.height)) {
		if ((wbapplet->prefs)->only_maximized) {
			controlledwindow = wbapplet->currentwindow;
		} else {
			controlledwindow = wbapplet->activewindow;
		}
		
		switch (i) {
			case WB_BUTTON_MINIMIZE:
				wnck_window_minimize(controlledwindow);
				break;
			case WB_BUTTON_UMAXIMIZE:
				if (wnck_window_is_maximized(controlledwindow)) {
					wnck_window_unmaximize(controlledwindow);
					wnck_window_activate(controlledwindow, gtk_get_current_event_time()); // make unmaximized window active
				} else {
					wnck_window_maximize(controlledwindow);
				}
				break;
			case WB_BUTTON_CLOSE:
				wnck_window_close(controlledwindow, GDK_CURRENT_TIME);
				break;
		}
	}

	updateImages(wbapplet);
	
	return TRUE;
}

/* Called when we click on a button */
static gboolean button_press (GtkWidget *event_box,
                             GdkEventButton *event,
                             gpointer data) {
	WBApplet *wbapplet;
	int i;
								 
	if (event->button != 1) return FALSE;

	wbapplet = WB_APPLET(data);

	// Find our button:
	if (wbapplet->prefs->click_effect) {
		for (i=0; i<WB_BUTTONS; i++) {
			if (GTK_EVENT_BOX(event_box) == wbapplet->button[i]->eventbox) {
				wbapplet->button[i]->state |= WB_BUTTON_STATE_CLICKED;
				break;
			}
		}
	
		updateImages(wbapplet);
	}

	return TRUE;
}

/* Makes the button 'glow' when the mouse enters it */
static gboolean hover_enter (GtkWidget *widget,
                         GdkEventCrossing *event,
                         gpointer user_data) {
	WBApplet *wbapplet = WB_APPLET(user_data);
	int i;

	// Find our button:
	if (wbapplet->prefs->hover_effect) {
		for (i=0; i<WB_BUTTONS; i++) {	
			if (GTK_EVENT_BOX(widget) == wbapplet->button[i]->eventbox) {
				wbapplet->button[i]->state |= WB_BUTTON_STATE_HOVERED;
				break;
			}
		}
		 
		updateImages(wbapplet);
	}

	return TRUE;
}

/* Makes the button stop 'glowing' when the mouse leaves it */
static gboolean hover_leave (GtkWidget *widget,
                         GdkEventCrossing *event,
                         gpointer user_data) {
	WBApplet *wbapplet = WB_APPLET(user_data);
	int i;

	// Find our button:
	if (wbapplet->prefs->hover_effect) {
		for (i=0; i<WB_BUTTONS; i++) {
			if (GTK_EVENT_BOX(widget) == wbapplet->button[i]->eventbox) {	
				wbapplet->button[i]->state &= ~WB_BUTTON_STATE_HOVERED;
				break;
			}
		}
		
		updateImages(wbapplet);
	}

	return TRUE;
}

GtkImage *getButtonImage(WBApplet *wbapplet, gushort button_id) {
	WnckWindow *firstwindow;
	
	if ((wbapplet->prefs)->only_maximized) {
		firstwindow = wbapplet->currentwindow;
	} else {
		firstwindow = wbapplet->activewindow;
	}

	switch (button_id) {
		case WB_BUTTON_MINIMIZE:
			return GTK_IMAGE (gtk_image_new_from_file((wbapplet->prefs)->images[WB_IMAGE_FOCUSED][WB_IMAGE_MINIMIZE]));
		case WB_BUTTON_UMAXIMIZE:			
			if (wnck_window_is_maximized(firstwindow)) {
				return GTK_IMAGE (gtk_image_new_from_file((wbapplet->prefs)->images[WB_IMAGE_FOCUSED][WB_IMAGE_UNMAXIMIZE]));
			} else {
				return GTK_IMAGE (gtk_image_new_from_file((wbapplet->prefs)->images[WB_IMAGE_FOCUSED][WB_IMAGE_MAXIMIZE]));
			}
		case WB_BUTTON_CLOSE:
			return GTK_IMAGE (gtk_image_new_from_file((wbapplet->prefs)->images[WB_IMAGE_FOCUSED][WB_IMAGE_CLOSE]));
	}

	return NULL;
}

WindowButton **createButtons (WBApplet *wbapplet) {
	WindowButton **button = g_new(WindowButton*, WB_BUTTONS);
	int i;

	for (i=0; i<WB_BUTTONS; i++) {	
		button[i] = g_new0(WindowButton, 1);
		button[i]->eventbox = GTK_EVENT_BOX(gtk_event_box_new());
		button[i]->image = getButtonImage(wbapplet, i);
	
		// Woohooo! This line eliminates PanelApplet borders - no more workarounds!
		GTK_WIDGET_SET_FLAGS (button[i]->eventbox, GTK_CAN_FOCUS);

		button[i]->state = 0;
		button[i]->state |= WB_BUTTON_STATE_FOCUSED;
		if (wbapplet->prefs->button_hidden[i]) {
			button[i]->state |= WB_BUTTON_STATE_HIDDEN;
		} else {
			button[i]->state &= ~WB_BUTTON_STATE_HIDDEN;
		}
	
		gtk_container_add (GTK_CONTAINER (button[i]->eventbox), GTK_WIDGET(button[i]->image));
		gtk_event_box_set_visible_window (button[i]->eventbox, FALSE);
	
		// Add hover events to eventboxes:
		gtk_widget_add_events (GTK_WIDGET (button[i]->eventbox), GDK_ENTER_NOTIFY_MASK); //add the "enter" signal
		gtk_widget_add_events (GTK_WIDGET (button[i]->eventbox), GDK_LEAVE_NOTIFY_MASK); //add the "leave" signal

		// Connect buttons to their respective callback functions
		g_signal_connect (G_OBJECT (button[i]->eventbox), "button-release-event", G_CALLBACK (button_release), wbapplet);
		g_signal_connect (G_OBJECT (button[i]->eventbox), "button-press-event", G_CALLBACK (button_press), wbapplet);
		g_signal_connect (G_OBJECT (button[i]->eventbox), "enter-notify-event", G_CALLBACK (hover_enter), wbapplet);
		g_signal_connect (G_OBJECT (button[i]->eventbox), "leave-notify-event", G_CALLBACK (hover_leave), wbapplet);	
	}

	return button;
}

// Places our buttons in correct order
void placeButtons(WBApplet *wbapplet) {
	int i, j;

	// Set box orientation
	if (wbapplet->angle == GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE || wbapplet->angle == GDK_PIXBUF_ROTATE_CLOCKWISE) {
		gtk_orientable_set_orientation(GTK_ORIENTABLE(wbapplet->box), GTK_ORIENTATION_VERTICAL);
	} else {
		gtk_orientable_set_orientation(GTK_ORIENTABLE(wbapplet->box), GTK_ORIENTATION_HORIZONTAL);
	}
	
	// Add eventboxes to box in preferred order:	
	for (i=0; i<WB_BUTTONS; i++) {
		for (j=0; j<WB_BUTTONS; j++) {
			if (wbapplet->prefs->eventboxposition[j] == i) {
				if (wbapplet->packtype == GTK_PACK_START) {
					gtk_box_pack_start(wbapplet->box, GTK_WIDGET(wbapplet->button[j]->eventbox), TRUE, TRUE, 0);
				} else if (wbapplet->packtype == GTK_PACK_END) {
					gtk_box_pack_end(wbapplet->box, GTK_WIDGET(wbapplet->button[j]->eventbox), TRUE, TRUE, 0);
				}
				break;
			}
		}
	}
}

void reloadButtons(WBApplet *wbapplet) {
	int i;

	// Remove eventbuttons from box:
	for (i=0; i<WB_BUTTONS; i++) {
		g_object_ref(wbapplet->button[i]->eventbox);
		gtk_container_remove(GTK_CONTAINER(wbapplet->box), GTK_WIDGET(wbapplet->button[i]->eventbox));
	}

	placeButtons(wbapplet);

	for (i=0; i<WB_BUTTONS; i++) {
		g_object_unref(wbapplet->button[i]->eventbox);
	}
}

GdkPixbuf ***getPixbufs(WBApplet *wbapplet) {
	GdkPixbuf ***pixbufs = g_new(GdkPixbuf**, WB_IMAGE_STATES);
	int i,j;
	for (i=0; i<WB_IMAGE_STATES; i++) {
		pixbufs[i] = g_new(GdkPixbuf*, WB_IMAGES);
		for (j=0; j<WB_IMAGES; j++) {
			GError *error = NULL;
			pixbufs[i][j] = gdk_pixbuf_new_from_file(wbapplet->prefs->images[i][j], &error);
			if (error) {
				printf("Error loading image \"%s\": %s\n", wbapplet->prefs->images[i][j], error->message);
				continue;
			}
		}
	}
	return pixbufs;
}

/* Returns the apropriate angle for respective panel orientations */
GdkPixbufRotation getOrientAngle(PanelAppletOrient orient) {
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

/* Rotates all the pixbufs by an angle */
void rotatePixbufs(WBApplet *wbapplet, GdkPixbufRotation angle) {
	int i,j;
	for (i=0; i<WB_IMAGE_STATES; i++) {
		for (j=0; j<WB_IMAGES; j++) {
			wbapplet->pixbufs[i][j] = gdk_pixbuf_rotate_simple(wbapplet->pixbufs[i][j], angle);
		}
	}
}

/* Triggered when a different panel orientation is detected */
void applet_change_orient (PanelApplet *panelapplet, PanelAppletOrient orient, gpointer user_data) {
	WBApplet *wbapplet = WB_APPLET(user_data);

	if (orient != wbapplet->orient) {
		wbapplet->orient = orient;
		wbapplet->pixbufs = getPixbufs(wbapplet);
		wbapplet->angle = getOrientAngle(wbapplet->orient);
		wbapplet->packtype = getPackType(wbapplet->angle);
		
		rotatePixbufs(wbapplet, wbapplet->angle);
		reloadButtons(wbapplet);
		updateImages(wbapplet);
	}
}

/* Do the actual applet initialization */
static void init_wbapplet(WBApplet *wbapplet) {
	wbapplet->activescreen = wnck_screen_get_default();
	wnck_screen_force_update(wbapplet->activescreen);
	wbapplet->activeworkspace = wnck_screen_get_active_workspace(wbapplet->activescreen);
	wbapplet->activewindow = wnck_screen_get_active_window(wbapplet->activescreen);
	wbapplet->prefs = loadPreferences(wbapplet);
	wbapplet->prefbuilder = gtk_builder_new();
	wbapplet->box = GTK_BOX(gtk_hbox_new(FALSE, 0));
	wbapplet->button = g_new(WindowButton*, WB_BUTTONS);
	wbapplet->pixbufs = getPixbufs(wbapplet);
	wbapplet->orient = panel_applet_get_orient(PANEL_APPLET(wbapplet));
	wbapplet->angle = getOrientAngle(wbapplet->orient);
	wbapplet->packtype = getPackType(wbapplet->angle);

	//wbapplet->active_handler = g_signal_connect(G_OBJECT (wbapplet->activewindow), "state-changed", G_CALLBACK (active_window_state_changed), wbapplet);
	if (wbapplet->prefs->only_maximized) {
		wbapplet->currentwindow = getUpperMaximized(wbapplet);
	}

	// Create eventboxes, add image to them, set preferences and add all to box
	wbapplet->button = createButtons(wbapplet);

	// Rotate & place buttons
	rotatePixbufs(wbapplet, wbapplet->angle);
	placeButtons(wbapplet);	
	
	// Add box to applet
	gtk_container_add (GTK_CONTAINER(wbapplet), GTK_WIDGET(wbapplet->box));

	// Global window tracking
	g_signal_connect(wbapplet->activescreen, "active-window-changed", G_CALLBACK (active_window_changed), wbapplet);
	g_signal_connect(wbapplet->activescreen, "viewports-changed", G_CALLBACK (viewports_changed), wbapplet);
	g_signal_connect(wbapplet->activescreen, "active-workspace-changed", G_CALLBACK (active_workspace_changed), wbapplet);
	g_signal_connect(wbapplet->activescreen, "window-closed", G_CALLBACK (window_closed), wbapplet);
	g_signal_connect(wbapplet->activescreen, "window-opened", G_CALLBACK (window_opened), wbapplet);

	g_signal_connect(G_OBJECT (wbapplet), "change-background", G_CALLBACK (applet_change_background), NULL);
	g_signal_connect(G_OBJECT (wbapplet), "change-orient", G_CALLBACK (applet_change_orient), wbapplet);

	// ???: Is this still necessary?
	wbapplet->active_handler = g_signal_connect(
							    G_OBJECT (wbapplet->activewindow),
    							"state-changed",
    							G_CALLBACK (active_window_state_changed),
    							wbapplet
    						);
}

/* We need this because things have to be hidden after we 'show' the applet */
void toggleHidden (WBApplet *wbapplet) {
	int j;
	for (j=0; j<WB_BUTTONS; j++) {
		if (wbapplet->button[j]->state & WB_BUTTON_STATE_HIDDEN) {
			gtk_widget_hide_all(GTK_WIDGET(wbapplet->button[j]->eventbox));
		} else {
			gtk_widget_show_all(GTK_WIDGET(wbapplet->button[j]->eventbox));
		}
	}
}

/* Initial function that draws the applet */
static gboolean windowbuttons_applet_fill (PanelApplet *applet, const gchar *iid, gpointer data) {
	if (strcmp (iid, APPLET_OAFIID) != 0) return FALSE;

	g_set_application_name (_(APPLET_NAME));
	panel_applet_set_flags (applet, PANEL_APPLET_EXPAND_MINOR);
	panel_applet_add_preferences (applet, GCONF_PREFS, NULL);

	init_wbapplet(WB_APPLET(applet));

	/* --- Context Menu --- */
	static const char context_menu_xml [] =
	   "<popup name=\"button3\">\n"
	   "   <menuitem name=\"Properties Item\" "
	   "             verb=\"WBPreferences\" "
	   "           _label=\"_Preferences...\"\n"
	   "          pixtype=\"stock\" "
	   "          pixname=\"gtk-properties\"/>\n"
	   "   <menuitem name=\"About Item\" "
	   "             verb=\"WBAbout\" "
	   "           _label=\"_About...\"\n"
	   "          pixtype=\"stock\" "
	   "          pixname=\"gtk-about\"/>\n"
	   "</popup>\n";
	//last parameter here will be the second parameter (WBApplet) in all menu callback functions (properties, about...) !!!
	panel_applet_setup_menu (applet, context_menu_xml, windowbuttons_menu_verbs, applet);

	/* Draw the damn thing */
	updateImages (WB_APPLET(applet));
	gtk_widget_show_all (GTK_WIDGET(applet));

	/* Hide/unhide buttons */
	toggleHidden (WB_APPLET(applet));

	return TRUE;
}

PANEL_APPLET_BONOBO_FACTORY (APPLET_OAFIID_FACTORY,
							 WB_TYPE_APPLET,
							 "windowbuttons",//APPLET_NAME,
							 "0",//VERSION,
							 windowbuttons_applet_fill,
							 NULL);