/***************************************************************************
 *            preferences.c
 *
 *  Mon May  4 01:23:08 2009
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

#include "preferences.h"

/* prototypes */
void cb_only_maximized (GtkButton *, gpointer);
void cb_click_effect (GtkButton *, gpointer);
void cb_hover_effect (GtkButton *, gpointer);
void cb_hide_on_unmaximized (GtkButton *, gpointer);
void cb_hide_decoration (GtkButton *, gpointer);
void cb_btn_minimize_hidden (GtkButton *, gpointer);
void cb_btn_maximize_hidden (GtkButton *, gpointer);
void cb_btn_close_hidden (GtkButton *, gpointer);
void properties_close (GtkObject *, gpointer);
void updateImages(WBApplet *);
void savePreferences(WBPreferences *, WBApplet *);
void savePreferencesGConf(WBPreferences *, WBApplet *);
//void savePreferencesCfg(WBPreferences *, WBApplet *); //TODO
void reloadButtons(WBApplet *);
const gchar *getImageGConfKey (gushort, gushort);
const gchar *getCheckBoxGConfKey (gushort);
GdkPixbuf ***getPixbufs(WBApplet *);
gshort *getEBPos(gchar *);
gchar *getMetacityLayout (void);

void savePreferences(WBPreferences *wbp, WBApplet *wbapplet) {
//	if(save_to_gconf) {
		savePreferencesGConf(wbp, wbapplet);
//	} else {
//		savePreferencesCfg(wbp, wbapplet);
//	}
}

void savePreferencesGConf(WBPreferences *wbp, WBApplet *wbapplet) {
	int i, j;

	for (i=0; i<WB_BUTTONS; i++) {
		//panel_applet_gconf_set_bool (PANEL_APPLET(wbapplet), getCheckBoxGConfKey(i), (wbapplet->button[i]->state & WB_BUTTON_STATE_HIDDEN), NULL);
		panel_applet_gconf_set_bool (PANEL_APPLET(wbapplet), getCheckBoxGConfKey(i), (wbapplet->prefs->button_hidden[i]), NULL);
	}

	for (i=0; i<WB_IMAGE_STATES; i++) {
		for (j=0; j<WB_IMAGES; j++) {
			panel_applet_gconf_set_string (PANEL_APPLET(wbapplet), getImageGConfKey(i,j), wbp->images[i][j], NULL);
		}
	}

	panel_applet_gconf_set_bool (PANEL_APPLET(wbapplet), GCK_CHECKBOX_ONLY_MAXIMIZED, wbp->only_maximized, NULL);
	panel_applet_gconf_set_bool (PANEL_APPLET(wbapplet), GCK_CHECKBOX_CLICK_EFFECT, wbp->click_effect, NULL);
	panel_applet_gconf_set_bool (PANEL_APPLET(wbapplet), GCK_CHECKBOX_HOVER_EFFECT, wbp->hover_effect, NULL);
	panel_applet_gconf_set_bool (PANEL_APPLET(wbapplet), GCK_CHECKBOX_HIDE_ON_UNMAXIMIZED, wbp->hide_on_unmaximized, NULL);
	panel_applet_gconf_set_bool (PANEL_APPLET(wbapplet), GCK_CHECKBOX_USE_METACITY_LAYOUT, wbp->use_metacity_layout, NULL);
	panel_applet_gconf_set_string (PANEL_APPLET(wbapplet), GCK_THEME, wbp->theme, NULL);
	
	if (wbp->use_metacity_layout) {
		// we aren't saving anything back to metacity's settings
	} else {
		panel_applet_gconf_set_string (PANEL_APPLET(wbapplet), GCK_BUTTON_LAYOUT, wbp->button_layout, NULL);
	}
}

gboolean issetCompizDecorationMatch() {
	GConfClient *gcc = gconf_client_get_default();
	gboolean retval = FALSE;
	if(!strcmp(gconf_client_get_string(gcc, GCONF_COMPIZ_DECORATION_MATCH, NULL),
	           COMPIZ_DECORATION_MATCH))
		retval = TRUE;
	return retval;
}

void toggleCompizDecorationMatch(WBApplet *wbapplet, gboolean new_value) {
	GError *err = NULL;
	GConfClient *gconfclient = gconf_client_get_default();
	
	if (new_value) {
		gconf_client_unset(gconfclient, GCONF_COMPIZ_DECORATION_MATCH, &err);
	} else {
		gconf_client_set_string(gconfclient,
		                        GCONF_COMPIZ_DECORATION_MATCH,
		                        COMPIZ_DECORATION_MATCH,
		                        NULL);
	}

	g_free(err);
	g_object_unref(gconfclient);
}

void select_new_image (GtkObject *object, gpointer user_data) {
	GtkWidget *fileopendialog;
	ImageOpenData *iod = (ImageOpenData*)user_data;
	WBApplet *wbapplet = iod->wbapplet;
	fileopendialog = gtk_file_chooser_dialog_new
					(
	    				"Select New Image",
						GTK_WINDOW (wbapplet->window_prefs),
						GTK_FILE_CHOOSER_ACTION_OPEN,
						GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						NULL
	    			);
	if (gtk_dialog_run (GTK_DIALOG (fileopendialog)) == GTK_RESPONSE_ACCEPT) {
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (fileopendialog));

		// save new value to memory
		wbapplet->prefs->images[iod->image_state][iod->image_index] = filename;

		//wbapplet->prefs->theme = "custom"; //TODO: change combo box
		//GtkComboBox *combo_theme = GTK_COMBO_BOX (gtk_builder_get_object(wbapplet->prefbuilder, "combo_theme"));
		//GtkListStore *store = gtk_combo_box_get_model(combo_theme);
		/*
		GtkTreeIter iter;
		gboolean valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL(store), &iter);
		while( valid ){
			// do stuff as 
			gtk_tree_model_get (GTK_TREE_MODEL(store), &iter, ... )
			//			
			valid = gtk_tree_model_iter_next (GTK_TREE_MODEL(store), &iter);   
		}
		 */
		
		// save new value to gconf
		savePreferences(wbapplet->prefs, wbapplet);
		// set new image in preferences
		gtk_button_set_image (GTK_BUTTON(object),
		                      gtk_image_new_from_file(wbapplet->prefs->images[iod->image_state][iod->image_index]));


		// load new images from files
		wbapplet->pixbufs = getPixbufs(wbapplet); 
		
		// reload image(s)
		updateImages(wbapplet);
	}
	gtk_widget_destroy (fileopendialog);
}

void cb_only_maximized(GtkButton *button, gpointer user_data) {
	WBApplet *wbapplet = WB_APPLET (user_data);
	wbapplet->prefs->only_maximized = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(button));
	savePreferences(wbapplet->prefs, wbapplet);
}

void cb_click_effect(GtkButton *button, gpointer user_data) {
	WBApplet *wbapplet = WB_APPLET (user_data);
	wbapplet->prefs->click_effect = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(button));
	savePreferences(wbapplet->prefs, wbapplet);
}

void cb_hover_effect(GtkButton *button, gpointer user_data) {
	WBApplet *wbapplet = WB_APPLET (user_data);
	wbapplet->prefs->hover_effect = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(button));
	savePreferences(wbapplet->prefs, wbapplet);
}

void cb_hide_on_unmaximized(GtkButton *button, gpointer user_data) {
	WBApplet *wbapplet = WB_APPLET (user_data);
	wbapplet->prefs->hide_on_unmaximized = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(button));
	savePreferences(wbapplet->prefs, wbapplet);
}

void cb_hide_decoration(GtkButton *button, gpointer user_data) {
	WBApplet *wbapplet = WB_APPLET(user_data);
	toggleCompizDecorationMatch(wbapplet, !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)));
}

void cb_btn_hidden(GtkButton *button, gpointer user_data) {
	CheckBoxData *cbd = (CheckBoxData*)user_data;
	WBApplet *wbapplet = cbd->wbapplet;
	
	if (wbapplet->button[cbd->button_id]->state & WB_BUTTON_STATE_HIDDEN) {
		gtk_widget_show_all(GTK_WIDGET(wbapplet->button[cbd->button_id]->eventbox));
		wbapplet->button[cbd->button_id]->state &= ~WB_BUTTON_STATE_HIDDEN;
		wbapplet->prefs->button_hidden[cbd->button_id] = 0;
	} else {
		gtk_widget_hide_all(GTK_WIDGET(wbapplet->button[cbd->button_id]->eventbox));
		wbapplet->button[cbd->button_id]->state |= WB_BUTTON_STATE_HIDDEN;
		wbapplet->prefs->button_hidden[cbd->button_id] = 1;
	}

	savePreferences(wbapplet->prefs, wbapplet);
}

// "Use Metacity's button order" checkbox
void cb_metacity_layout(GtkButton *button, gpointer user_data) {
	WBApplet *wbapplet = WB_APPLET(user_data);
	GtkEntry *entry_custom_layout = GTK_ENTRY (gtk_builder_get_object(wbapplet->prefbuilder, "text_custom_order"));
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(button))) {
		wbapplet->prefs->use_metacity_layout = TRUE;
		wbapplet->prefs->button_layout = getMetacityLayout();
		gtk_widget_set_sensitive(GTK_WIDGET(entry_custom_layout), FALSE);
	} else {
		gtk_widget_set_sensitive(GTK_WIDGET(entry_custom_layout), TRUE);
		wbapplet->prefs->use_metacity_layout = FALSE;
		gchar *new_layout = g_strdup(gtk_entry_get_text(entry_custom_layout));
		wbapplet->prefs->button_layout = new_layout;
	}

	savePreferences(wbapplet->prefs, wbapplet);
	
	wbapplet->prefs->eventboxposition = getEBPos(wbapplet->prefs->button_layout);
	reloadButtons (wbapplet);
}

// "Reload" button clicked
void cb_reload_buttons(GtkButton *button, gpointer user_data) { 
	WBApplet *wbapplet = WB_APPLET(user_data);
	GtkEntry *entry_custom_layout = GTK_ENTRY (gtk_builder_get_object(wbapplet->prefbuilder, "text_custom_order"));
	gchar *new_layout = g_strdup(gtk_entry_get_text(entry_custom_layout));
	wbapplet->prefs->button_layout = new_layout;
	savePreferences(wbapplet->prefs, wbapplet);
	wbapplet->prefs->eventboxposition = getEBPos(wbapplet->prefs->button_layout);
	reloadButtons(wbapplet);
}

const gchar* getCheckBoxGConfKey(gushort checkbox_id) {
	switch(checkbox_id) {
		case 0: return "button_minimize_hidden";
		case 1: return "button_maximize_hidden";
		case 2: return "button_close_hidden";
		default: return NULL;
	}
}

const gchar* getButtonImageName(int button_id) {
	switch(button_id) {
		case WB_IMAGE_MINIMIZE: return BTN_NAME_MINIMIZE;
		case WB_IMAGE_UNMAXIMIZE: return BTN_NAME_UNMAXIMIZE;
		case WB_IMAGE_MAXIMIZE: return BTN_NAME_MAXIMIZE;
		case WB_IMAGE_CLOSE: return BTN_NAME_CLOSE;
		default: return NULL;
	}
}

const gchar* getButtonImageState(int state_id) {
	switch(state_id) {
		case WB_IMAGE_FOCUSED: return BTN_STATE_FOCUSED;
		case WB_IMAGE_CLICKED: return BTN_STATE_CLICKED;
		case WB_IMAGE_HOVERED: return BTN_STATE_HOVER;
		case WB_IMAGE_UNFOCUSED: return BTN_STATE_UNFOCUSED;
		default: return NULL;
	}
}

/* returns the GConf schema key for our button */
const gchar *getImageGConfKey(gushort image_state, gushort image_index) {
	return g_strconcat("btn_", getButtonImageState(image_state), "_", getButtonImageName(image_index), NULL);
}

static void cb_theme_changed( GtkComboBox *combo, gpointer data ) {
	WBApplet	*wbapplet = WB_APPLET(data);
	WBPreferences *wbp = wbapplet->prefs;
	GtkTreeIter   iter;
    gchar        *theme = NULL;
    GtkTreeModel *model;

    if (gtk_combo_box_get_active_iter( combo, &iter )) {
        model = gtk_combo_box_get_model( combo );
        gtk_tree_model_get( model, &iter, 0, &theme, -1 );
    }
	
	wbp->theme = theme;

	int i,j;
	gchar *theme_location = g_strconcat(LOCATION_THEMES, "/", wbapplet->prefs->theme, "/", NULL);
	for (i=0; i<WB_IMAGE_STATES; i++) {
		for (j=0; j<WB_IMAGES; j++) {
			wbp->images[i][j] = g_strconcat(theme_location, getButtonImageName(j), "-", getButtonImageState(i), ".png", NULL);
			gtk_button_set_image(
			    GTK_BUTTON(gtk_builder_get_object (wbapplet->prefbuilder, getImageGConfKey(i,j))),
			    gtk_image_new_from_file(wbapplet->prefs->images[i][j])
			);
		}
	}
	g_free(theme_location);

	wbapplet->pixbufs = getPixbufs(wbapplet); // load new images from files
	updateImages(wbapplet);
	
	savePreferences(wbapplet->prefs, wbapplet);
}

/* All the crap required to fill a stupid little combo box (Gtk people think this is useful) */ 
void loadThemes(GtkComboBox *combo, WBApplet *wbapplet) {    
	GtkTreeIter		iter;
	GtkListStore	*store = gtk_list_store_new( 2, G_TYPE_STRING, G_TYPE_INT );

	GError	*error = NULL;
	gchar	*dir_themes_path = LOCATION_THEMES;
	GDir	*dir_themes = g_dir_open(dir_themes_path, 0, &error);
	if (error) {
		g_printerr ("g_dir_open(%s) failed - %s\n", dir_themes_path, error->message);
		g_error_free(error);
		return;
	}

	int active = -1;
	int N_THEMES = 0;
	const gchar *curtheme;
	while((curtheme = g_dir_read_name(dir_themes))) { 
		if( g_strcmp0(
		    	g_ascii_strdown(curtheme,-1),
		    	g_ascii_strdown(wbapplet->prefs->theme,-1)
		    ) == 0 )
		{
			active = N_THEMES;
		}
		
		gtk_list_store_append( store, &iter );
		gtk_list_store_set( store, &iter, 
		    	0, curtheme,
		    	1, 1+N_THEMES++,
				-1 );
	}
	if(active<0) active = N_THEMES;
	
    gtk_list_store_append( store, &iter );
    gtk_list_store_set( store, &iter, 0,"Custom", 1,0, -1 );
	
	gtk_combo_box_set_model( combo, GTK_TREE_MODEL(store) );
    g_object_unref( G_OBJECT( store ) );

	GtkCellRenderer	*cell = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start( GTK_CELL_LAYOUT( combo ), cell, TRUE );
    gtk_cell_layout_set_attributes( GTK_CELL_LAYOUT( combo ), cell, "text",0, NULL );

	gtk_combo_box_set_active(combo, active);
}

/* The Preferences Dialog */
void properties_cb (BonoboUIComponent *uic,
                    WBApplet *wbapplet,
                    const char *verb)
{
	GtkWidget		***btn;
	ImageOpenData 	***iod;
	gint i,j;

	// Create the Properties dialog from the GtkBuilder file
	gtk_builder_add_from_file (wbapplet->prefbuilder, LOCATION_MAIN"/windowbuttons.ui", NULL);
	if (!wbapplet->window_prefs) {
		wbapplet->window_prefs = GTK_WIDGET (gtk_builder_get_object (wbapplet->prefbuilder, "properties"));
	} else {
		gtk_window_present(GTK_WINDOW(wbapplet->window_prefs));
	}
	//gtk_builder_connect_signals (wbapplet->prefbuilder, NULL); // no need for now

	/* Get the widgets from GtkBuilder & Init data structures we'll pass to our buttons */
	btn = g_new(GtkWidget**, WB_IMAGE_STATES);
	iod =  g_new(ImageOpenData**, WB_IMAGE_STATES);
	for (i=0; i<WB_IMAGE_STATES; i++) {
		btn[i] = g_new(GtkWidget*, WB_IMAGES);
		iod[i] = g_new(ImageOpenData*, WB_IMAGES);
		for (j=0; j<WB_IMAGES; j++) {
			iod[i][j] = g_new0(ImageOpenData, 1);
			iod[i][j]->wbapplet = wbapplet;
			iod[i][j]->gconf_key = getImageGConfKey(i,j);
			iod[i][j]->image_state = i;
			iod[i][j]->image_index = j;

			btn[i][j] = GTK_WIDGET (gtk_builder_get_object (wbapplet->prefbuilder, iod[i][j]->gconf_key));

			// set widgets according to our memorized properties
			gtk_button_set_image(GTK_BUTTON (btn[i][j]), gtk_image_new_from_file(wbapplet->prefs->images[i][j]));
			gtk_widget_set_tooltip_text(btn[i][j], wbapplet->prefs->images[i][j]);

			// Connect buttons to select_new_image callback
			g_signal_connect(G_OBJECT (btn[i][j]), "clicked", G_CALLBACK (select_new_image), iod[i][j]);
		}
	}

	GtkToggleButton 
		*chkb_only_maximized = GTK_TOGGLE_BUTTON (gtk_builder_get_object(wbapplet->prefbuilder, "chkb_only_maximized")),
		*chkb_click_effect = GTK_TOGGLE_BUTTON (gtk_builder_get_object(wbapplet->prefbuilder, "chkb_click_effect")),
		*chkb_hover_effect = GTK_TOGGLE_BUTTON (gtk_builder_get_object(wbapplet->prefbuilder, "chkb_hover_effect")),
		*chkb_hide_on_unmaximized = GTK_TOGGLE_BUTTON (gtk_builder_get_object(wbapplet->prefbuilder, "chkb_hide_on_unmaximized")),
		*chkb_hide_decoration = GTK_TOGGLE_BUTTON (gtk_builder_get_object(wbapplet->prefbuilder, "chkb_hide_decoration")),
		*chkb_metacity_order = GTK_TOGGLE_BUTTON (gtk_builder_get_object(wbapplet->prefbuilder, "chkb_metacity_order"));
	GtkToggleButton **chkb_btn_hidden = g_new(GtkToggleButton*, WB_BUTTONS);
	chkb_btn_hidden[0] = GTK_TOGGLE_BUTTON (gtk_builder_get_object(wbapplet->prefbuilder, "cb_btn0_visible")),
	chkb_btn_hidden[1] = GTK_TOGGLE_BUTTON (gtk_builder_get_object(wbapplet->prefbuilder, "cb_btn1_visible")),
	chkb_btn_hidden[2] = GTK_TOGGLE_BUTTON (gtk_builder_get_object(wbapplet->prefbuilder, "cb_btn2_visible"));		
	GtkEntry *entry_custom_order = GTK_ENTRY (gtk_builder_get_object(wbapplet->prefbuilder, "text_custom_order"));
	GtkButton *btn_reload_order = GTK_BUTTON (gtk_builder_get_object(wbapplet->prefbuilder, "btn_reload_order"));
	GtkComboBox *combo_theme = GTK_COMBO_BOX (gtk_builder_get_object(wbapplet->prefbuilder, "combo_theme"));
	GtkButton *btn_close = GTK_BUTTON (gtk_builder_get_object(wbapplet->prefbuilder, "btn_close"));

	loadThemes(combo_theme, wbapplet);

	// set the checkboxes according to preferences
	gtk_widget_set_sensitive(GTK_WIDGET(entry_custom_order), !wbapplet->prefs->use_metacity_layout);
	gtk_toggle_button_set_active (chkb_only_maximized, wbapplet->prefs->only_maximized);
	gtk_toggle_button_set_active (chkb_click_effect, wbapplet->prefs->click_effect);
	gtk_toggle_button_set_active (chkb_hover_effect, wbapplet->prefs->hover_effect);
	gtk_toggle_button_set_active (chkb_hide_on_unmaximized, wbapplet->prefs->hide_on_unmaximized);
	gtk_toggle_button_set_active (chkb_hide_decoration, issetCompizDecorationMatch());
	gtk_toggle_button_set_active (chkb_metacity_order, wbapplet->prefs->use_metacity_layout);
	gtk_entry_set_text (entry_custom_order, (const gchar*)wbapplet->prefs->button_layout);
						
	CheckBoxData **cbd = g_new(CheckBoxData*, WB_BUTTONS);
	for (i=0; i<WB_BUTTONS; i++) {
		cbd[i] = g_new(CheckBoxData,1);
		cbd[i]->button_id = i;
		cbd[i]->wbapplet = wbapplet;

		gtk_toggle_button_set_active (chkb_btn_hidden[i], (wbapplet->button[i]->state & WB_BUTTON_STATE_HIDDEN));
		g_signal_connect(G_OBJECT(chkb_btn_hidden[i]), "clicked", G_CALLBACK (cb_btn_hidden), cbd[i]);
	}

	g_signal_connect(G_OBJECT(chkb_only_maximized), "clicked", G_CALLBACK (cb_only_maximized), wbapplet);
	g_signal_connect(G_OBJECT(chkb_click_effect), "clicked", G_CALLBACK (cb_click_effect), wbapplet);
	g_signal_connect(G_OBJECT(chkb_hover_effect), "clicked", G_CALLBACK (cb_hover_effect), wbapplet);						
	g_signal_connect(G_OBJECT(chkb_hide_on_unmaximized), "clicked", G_CALLBACK (cb_hide_on_unmaximized), wbapplet);
	g_signal_connect(G_OBJECT(chkb_hide_decoration), "clicked", G_CALLBACK (cb_hide_decoration), wbapplet);
	g_signal_connect(G_OBJECT(chkb_metacity_order), "clicked", G_CALLBACK (cb_metacity_layout), wbapplet);
	g_signal_connect(G_OBJECT(btn_reload_order), "clicked", G_CALLBACK (cb_reload_buttons), wbapplet);
	g_signal_connect(G_OBJECT(combo_theme), "changed", G_CALLBACK(cb_theme_changed), wbapplet);
	g_signal_connect(G_OBJECT(btn_close), "clicked", G_CALLBACK (properties_close), wbapplet);
	g_signal_connect(G_OBJECT(wbapplet->window_prefs), "destroy", G_CALLBACK(properties_close), wbapplet);
	
	//g_object_unref (G_OBJECT (wbapplet->prefs->prefbuilder));
	gtk_widget_show (wbapplet->window_prefs);
}

/* Close the Properties dialog - we're not saving anything (it's already saved) */
void properties_close (GtkObject *object, gpointer user_data) {
	WBApplet *wbapplet = WB_APPLET(user_data);
	gtk_widget_destroy(wbapplet->window_prefs);
	wbapplet->window_prefs = NULL;
}