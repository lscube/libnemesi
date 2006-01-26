#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "gui_throbber.h"
#include "gui_statusbar.h"
#include "gui_cc.h"
#include "gui_url.h"
#include "gnmsprint.h"
#include <nemesi/egui.h>
#include <nemesi/methods.h>

#define CREDITS	"NeMeSI -- Network Media Streamer I\n\n"\
		"\tCopyleft 2001\n"\
		"Lead Developers:\n"\
		"\tGiampaolo Mancini - <giampaolo.mancini@polito.it>\n"\
		"\tFrancesco Varano - <francesco.varano@polito.it>\n"\
		"Graduate Developers:\n"\
		"\tMarco Penno - <marco.penno@polito.it>"

// static variables declaration
static GtkWidget *nemesi;
static GtkWidget *opendialog=NULL;
static GtkWidget *aboutdialog=NULL;
static GtkWidget *info = NULL;

static GNMSurl *gnmsurl = NULL;

static gint infowidth = 0;

static struct rtsp_ctrl *rtsp_ctrl;

static gboolean internal_call = FALSE;
// end of static variables declarations

// *** generic functions *** //
void save_static_data(gpointer new_nemesi, gpointer new_rtsp_ctrl)
{
	nemesi = new_nemesi;
	rtsp_ctrl = new_rtsp_ctrl;

	// GUI statusbar initialization
	gnms_stbar_init(GTK_BOX(lookup_widget(nemesi, "statushbox")));
	cc_stbarw_add(rtsp_ctrl);
}

void update_toolbar(void)
{
	GtkWidget *toolbar = lookup_widget(nemesi, "toolbar");
	GtkWidget *open_but;
	GtkWidget *play_tog;
	GtkWidget *stop_but;
	GtkWidget *close_but;

	if (rtsp_ctrl->busy) {
		gtk_widget_set_sensitive(toolbar, FALSE);
		return;
	}
	open_but = lookup_widget(nemesi, "open_cmd");
	play_tog = lookup_widget(nemesi, "toggle_play_pause");
	stop_but = lookup_widget(nemesi, "stop_cmd");
	close_but = lookup_widget(nemesi, "close_cmd");
	internal_call = TRUE;
	switch (rtsp_status(rtsp_ctrl)) {
		case INIT:
			gtk_widget_set_sensitive(open_but, TRUE);
			gtk_widget_set_sensitive(play_tog, FALSE);
			gtk_widget_set_sensitive(stop_but, FALSE);
			gtk_widget_set_sensitive(close_but, FALSE);
			// gnms_stbar_clear();
			break;
		case READY:
			gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(play_tog), FALSE);
			gtk_widget_set_sensitive(open_but, FALSE);
			gtk_widget_set_sensitive(play_tog, TRUE);
			gtk_widget_set_sensitive(stop_but, FALSE);
			gtk_widget_set_sensitive(close_but, TRUE);
			break;
		case PLAYING:
			gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(play_tog), TRUE);
			gtk_widget_set_sensitive(open_but, FALSE);
			gtk_widget_set_sensitive(play_tog, TRUE);
			gtk_widget_set_sensitive(stop_but, TRUE);
			gtk_widget_set_sensitive(close_but, TRUE);
			break;
		default:
			break;
	}
	internal_call = FALSE;
	gtk_widget_set_sensitive(toolbar, TRUE);

	gnms_stbar_setstr("NeMeSI RTSP Status: %s", statustostr(rtsp_status(rtsp_ctrl)));
	gnms_stbar_update();
	gnms_showmsgs();

}

void view_info(GtkWidget *infow)
{
	gint nmsx, nmsy, nmsw, nmsh;
	// GdkGravity nemesi_gvty;

	if (!info)
		info = create_info();
	else { // TODO: destroy only the internal, not the window
		gtk_widget_destroy(info);
		info = create_info();
	}

	gtk_container_add (GTK_CONTAINER (info), infow);

	/*
	nemesi_gvty = gtk_window_get_gravity(GTK_WINDOW(nemesi));
	gtk_window_set_gravity(GTK_WINDOW(nemesi), GDK_GRAVITY_STATIC);
	gtk_window_set_gravity(GTK_WINDOW(info), GDK_GRAVITY_STATIC);
	*/
	gtk_window_get_position(GTK_WINDOW(nemesi), &nmsx, &nmsy);
	gtk_window_get_size(GTK_WINDOW(nemesi), &nmsw, &nmsh);
	gtk_window_get_size(GTK_WINDOW(info), &infowidth, NULL);

	gtk_window_move(GTK_WINDOW(info), nmsx + nmsw - infowidth, nmsy + nmsh);

	gtk_widget_show(info);
	// gtk_window_set_gravity(GTK_WINDOW(nemesi), nemesi_gvty);
}

void hide_info()
{
	if (info) {
		gtk_widget_destroy(info);
		info = NULL;
	}
}
// *** ENDOF generic functions *** //

// *** event functions *** //
//
// *** main window *** //

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (!opendialog)
		opendialog = create_opendialog();
	gtk_widget_show(opendialog);
}

void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gnms_stbar_clear();
	gtk_main_quit();
}

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (!aboutdialog)
		aboutdialog = create_aboutdialog();
	gtk_widget_show(aboutdialog);
}


void
on_toggle_play_pause_toggled           (GtkToggleToolButton *togglebutton,
                                        gpointer         user_data)
{
	char argstr[] = " ";

	if (internal_call)
		return;
	if (gtk_toggle_tool_button_get_active(togglebutton))
		nms_play(rtsp_ctrl, argstr);
	else
		nms_pause(rtsp_ctrl);

	gui_throbber(&rtsp_ctrl->busy);

}

void
on_stop_cmd_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	nms_stop(rtsp_ctrl);
	gui_throbber(&rtsp_ctrl->busy);
}


void
on_close_cmd_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	nms_close(rtsp_ctrl);
	gui_throbber(&rtsp_ctrl->busy);
}


void
on_open_cmd_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	if (!opendialog)
		opendialog = create_opendialog();
	gtk_widget_show(opendialog);
}


gboolean
on_nemesi_configure_event              (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
	if (info)
		gtk_window_move(GTK_WINDOW(info), event->x + event->width - infowidth, event->y + event->height);

	return FALSE;
}

// *** ENDOF main window *** //
//
// *** open dialog *** //

void
on_urlname_realize                     (GtkWidget       *widget,
                                        gpointer         user_data)
{
	GtkEntryCompletion *completion;

	if (!gnmsurl)
		gnmsurl = gnmsurl_init();
	
	completion = gtk_entry_completion_new();

	// GtkComboBox
	gtk_combo_box_set_model (GTK_COMBO_BOX(widget), gnmsurl->model);
	gtk_combo_box_entry_set_text_column(GTK_COMBO_BOX_ENTRY(widget), gnmsurl->txt_col);

	// GtkEntryCompletion
	gtk_entry_completion_set_model(completion, gnmsurl->model);
	gtk_entry_completion_set_text_column(completion, gnmsurl->txt_col);
	gtk_entry_completion_set_match_func(completion, gnmscompletion_machfunc, gnmsurl, gnmsurl_destroy);
	// gtk_entry_completion_set_inline_completion(completioin, TRUE);
	gtk_entry_set_completion(GTK_ENTRY(GTK_BIN(widget)->child), completion);

	gtk_combo_box_set_active(GTK_COMBO_BOX(widget), 0);

	// press ok button on "enter" button pressed
	gtk_entry_set_activates_default(GTK_ENTRY(GTK_BIN(widget)->child), TRUE);
	gtk_widget_grab_focus(GTK_WIDGET(GTK_BIN(widget)->child));
	gtk_editable_set_position(GTK_EDITABLE(GTK_BIN(widget)->child), -1);
}


void
on_opendialog_response                 (GtkDialog       *dialog,
                                        gint             response_id,
                                        gpointer         user_data)
{
	GtkWidget *combo_box;
	char *url, *true_url;
	GtkTreeModel *list_store;
	GtkTreeIter iter;
	gboolean valid, found = FALSE;
	gint txt_col;

	switch (response_id) {
		case GTK_RESPONSE_NONE:
			nms_printf(NMSML_DBG3, "response: NONE\n");
			break;
		case GTK_RESPONSE_REJECT:
			nms_printf(NMSML_DBG3, "response: REJECT\n");
			break;
		case GTK_RESPONSE_ACCEPT:
			nms_printf(NMSML_DBG3, "response: ACCEPT\n");
			break;
		case GTK_RESPONSE_OK:
			nms_printf(NMSML_DBG3, "response: OK\n");
			combo_box = lookup_widget(opendialog, "urlname");
				
			// url = (char *)gtk_entry_get_text(GTK_ENTRY(lookup_widget(opendialog, "urlname")));
			url = (char *)gtk_entry_get_text(GTK_ENTRY(GTK_BIN(combo_box)->child));
		
			txt_col = gtk_combo_box_entry_get_text_column(GTK_COMBO_BOX_ENTRY(combo_box));
			list_store = gtk_combo_box_get_model(GTK_COMBO_BOX(combo_box));
			valid = gtk_tree_model_get_iter_first(list_store, &iter);
		
			nms_printf(NMSML_DBG1, "List of urls\n");
			while (valid) {
				gchar *str_data;
				gtk_tree_model_get (list_store, &iter, txt_col, &str_data, -1);
				nms_printf(NMSML_DBG1, "%s\n", str_data);
				if (!strcmp(url, str_data)) {
					found = TRUE;
					break;
				}
				g_free(str_data);
				valid = gtk_tree_model_iter_next (list_store, &iter);
			}
			nms_printf(NMSML_DBG1, "ENDOF List of urls\n");
			if (!found)
				gtk_combo_box_append_text (GTK_COMBO_BOX (combo_box), url);
		
			true_url = strdup(url);
			gtk_widget_destroy(opendialog);
			opendialog = NULL;
			/*
			gtk_widget_hide(opendialog);
			*/
			nms_printf(NMSML_DBG1, "%s\n", true_url);
			nms_open(rtsp_ctrl, true_url, gui_throbber, &rtsp_ctrl->busy);
			break;
		case GTK_RESPONSE_CLOSE:
			nms_printf(NMSML_DBG3, "response: CLOSE\n");
			break;
		case GTK_RESPONSE_YES:
			nms_printf(NMSML_DBG3, "response: YES\n");
			break;
		case GTK_RESPONSE_NO:
			nms_printf(NMSML_DBG3, "response: NO\n");
			break;
		case GTK_RESPONSE_APPLY:
			nms_printf(NMSML_DBG3, "response: APPLY\n");
			break;
		case GTK_RESPONSE_HELP:
			nms_printf(NMSML_DBG3, "response: HELP\n");
			break;
		case GTK_RESPONSE_CANCEL:
		case GTK_RESPONSE_DELETE_EVENT:
		default:
			gtk_widget_destroy(opendialog);
			opendialog = NULL;
			nms_printf(NMSML_DBG3, "opendialog closed\n");
			break;
	}

}

// *** ENDOF open dialog *** //
//
// *** about dialog *** //

void
on_aboutdialog_realize                 (GtkWidget       *widget,
                                        gpointer         user_data)
{
	GtkWidget *aboutimage;
	GtkWidget *aboutlabel;

	nms_printf(NMSML_DBG3, "about dialog realized\n");
	aboutimage = create_pixmap(NULL, "about-nemesi.png");
	gtk_widget_set_name(aboutimage, "aboutimage");
	g_object_set_data_full (G_OBJECT (aboutdialog), "aboutimage", gtk_widget_ref (aboutimage), (GDestroyNotify) gtk_widget_unref);
	gtk_box_pack_start (GTK_BOX (lookup_widget(aboutdialog, "dialogvbox")), aboutimage, FALSE, FALSE, 0);
	gtk_widget_show(aboutimage);
	// aboutlabel
	aboutlabel = lookup_widget(aboutdialog, "aboutlabel");
	gtk_label_set_markup (GTK_LABEL(aboutlabel), "Release "VERSION"\n");
}


void
on_aboutdialog_response                (GtkDialog       *dialog,
                                        gint             response_id,
                                        gpointer         user_data)
{
	GtkWidget *aboutimage;
	GtkWidget *credits;
	GtkWidget *creditsview;
	GtkRequisition requisition;

	switch (response_id) {
		case 1: // show credits
			nms_printf(NMSML_DBG3, "response: CREDITS\n");
			aboutimage = lookup_widget(aboutdialog, "aboutimage");
			credits = lookup_widget(aboutdialog, "creditscroller");
			creditsview = lookup_widget(aboutdialog, "creditsview");
			gtk_widget_size_request(aboutimage, &requisition);
			gtk_widget_set_size_request(credits, requisition.width, requisition.height);
			gtk_text_buffer_set_text(gtk_text_view_get_buffer (GTK_TEXT_VIEW (creditsview)), CREDITS, -1);
			gtk_widget_hide(aboutimage);
			gtk_widget_show(credits);
			// gtk_widget_hide(GTK_WIDGET(button));
			gtk_widget_hide(lookup_widget(aboutdialog, "credits"));
			gtk_widget_show(lookup_widget(aboutdialog, "backabout"));
			break;
		case 2: // return back
			nms_printf(NMSML_DBG3, "response: BACK\n");
			gtk_widget_hide(lookup_widget(aboutdialog, "creditscroller"));
			gtk_widget_show(lookup_widget(aboutdialog, "aboutimage"));
			gtk_widget_hide(lookup_widget(aboutdialog, "backabout"));
			gtk_widget_show(lookup_widget(aboutdialog, "credits"));
			break;
		case GTK_RESPONSE_NONE:
			nms_printf(NMSML_DBG3, "response: NONE\n");
			break;
		case GTK_RESPONSE_REJECT:
			nms_printf(NMSML_DBG3, "response: REJECT\n");
			break;
		case GTK_RESPONSE_ACCEPT:
			nms_printf(NMSML_DBG3, "response: ACCEPT\n");
			break;
		case GTK_RESPONSE_OK:
			nms_printf(NMSML_DBG3, "response: OK\n");
			break;
		case GTK_RESPONSE_CANCEL:
		case GTK_RESPONSE_CLOSE:
		case GTK_RESPONSE_DELETE_EVENT:
			nms_printf(NMSML_DBG3, "response: CLOSE\n");
			gtk_widget_destroy(aboutdialog);
			aboutdialog = NULL;
			break;
		case GTK_RESPONSE_YES:
			nms_printf(NMSML_DBG3, "response: YES\n");
			break;
		case GTK_RESPONSE_NO:
			nms_printf(NMSML_DBG3, "response: NO\n");
			break;
		case GTK_RESPONSE_APPLY:
			nms_printf(NMSML_DBG3, "response: APPLY\n");
			break;
		case GTK_RESPONSE_HELP:
			nms_printf(NMSML_DBG3, "response: HELP\n");
			break;
		default:
			nms_printf(NMSML_DBG3, "NO EVENT\n");
			break;
	}
}

// *** ENDOF about dialog *** //
// 
// *** info dialog *** //
// 
// *** ENDOF info dialog *** //
//
// *** ENDOF event functions *** //

