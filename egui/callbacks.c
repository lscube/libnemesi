#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "gui_throbber.h"
#include <nemesi/egui.h>
#include <nemesi/etui.h>

#define CREDITS	"NeMeSI -- Network Media Streamer I\n\n"\
		"\tCopyleft 2001\n"\
		"Lead Developers:\n"\
		"\tGiampaolo Mancini - <giampaolo.mancini@polito.it>\n"\
		"\tFrancesco Varano - <francesco.varano@polito.it>\n"\
		"Graduate Developers:\n"\
		"\tMarco Penno - <marco.penno@polito.it>"

static GtkWidget *nemesi;
static GtkWidget *opendialog=NULL;
static GtkWidget *aboutdialog=NULL;
static struct RTSP_args *rtsp_args;

void save_static_data(gpointer new_nemesi, gpointer new_rtsp_args)
{
	nemesi = new_nemesi;
	rtsp_args = new_rtsp_args;
}

void update_toolbar(void)
{
	GtkStatusbar *statusbar = (GtkStatusbar *)lookup_widget(nemesi, "statusbar");
	static guint status_cid=0;
	char statusstr[32];
	GtkWidget *toolbar = lookup_widget(nemesi, "toolbar");
	GtkWidget *open_but;
	GtkWidget *play_tog;
	GtkWidget *stop_but;
	GtkWidget *close_but;

	if (rtsp_args->rtsp_th->busy) {
		gtk_widget_set_sensitive(toolbar, FALSE);
		return;
	}
	open_but = lookup_widget(nemesi, "open_cmd");
	play_tog = lookup_widget(nemesi, "toggle_play_pause");
	stop_but = lookup_widget(nemesi, "stop_cmd");
	close_but = lookup_widget(nemesi, "close_cmd");
	switch (rtsp_args->rtsp_th->status) {
		case INIT:
			gtk_widget_set_sensitive(open_but, TRUE);
			gtk_widget_set_sensitive(play_tog, FALSE);
			gtk_widget_set_sensitive(stop_but, FALSE);
			gtk_widget_set_sensitive(close_but, FALSE);
			break;
		case READY:
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(play_tog), FALSE);
			gtk_widget_set_sensitive(open_but, FALSE);
			gtk_widget_set_sensitive(play_tog, TRUE);
			gtk_widget_set_sensitive(stop_but, FALSE);
			gtk_widget_set_sensitive(close_but, TRUE);
			break;
		case PLAYING:
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(play_tog), TRUE);
			gtk_widget_set_sensitive(open_but, FALSE);
			gtk_widget_set_sensitive(play_tog, TRUE);
			gtk_widget_set_sensitive(stop_but, TRUE);
			gtk_widget_set_sensitive(close_but, TRUE);
			break;
		default:
			break;
	}
	gtk_widget_set_sensitive(toolbar, TRUE);
	if (!status_cid)
		status_cid = gtk_statusbar_get_context_id(statusbar, "Status");
	sprintf(statusstr, "NeMeSI RTSP Status: %s", statustostr(rtsp_args->rtsp_th->status));
	gtk_statusbar_push(statusbar, status_cid, statusstr);
}

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
	gtk_main_quit();
}

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *aboutimage;
	GtkWidget *aboutlabel;

	if (!aboutdialog)
		aboutdialog = create_aboutdialog();
	aboutimage = create_pixmap(NULL, "about-nemesi.png");
	gtk_widget_set_name(aboutimage, "aboutimage");
	g_object_set_data_full (G_OBJECT (aboutdialog), "aboutimage", gtk_widget_ref (aboutimage), (GDestroyNotify) gtk_widget_unref);
	gtk_box_pack_start (GTK_BOX (lookup_widget(aboutdialog, "dialogvbox")), aboutimage, FALSE, FALSE, 0);
	gtk_widget_show(aboutimage);
	// aboutlabel
	aboutlabel = lookup_widget(aboutdialog, "aboutlabel");
	gtk_label_set_markup (GTK_LABEL(aboutlabel), "Release "VERSION"\n");
	gtk_widget_show(aboutdialog);
}


void
on_toggle_play_pause_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	char argstr[] = " ";

	if (gtk_toggle_button_get_active(togglebutton))
		send_play(rtsp_args, argstr);
	else
		send_pause(rtsp_args, 'z');
	gui_throbber(&rtsp_args->rtsp_th->busy);

}

void
on_stop_cmd_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	send_pause(rtsp_args, 's');
	gui_throbber(&rtsp_args->rtsp_th->busy);
}


void
on_close_cmd_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	send_close(rtsp_args);
	gui_throbber(&rtsp_args->rtsp_th->busy);
}


void
on_open_cmd_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	if (!opendialog)
		opendialog = create_opendialog();
	gtk_widget_show(opendialog);
}


void
on_cancelbutton1_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(opendialog);
	opendialog = NULL;
}


void
on_okbutton1_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	char *url;

	url = (char *)gtk_entry_get_text(GTK_ENTRY(lookup_widget(opendialog, "urlname")));
	gtk_widget_destroy(opendialog);
	opendialog = NULL;
	send_open(rtsp_args, url);
	gui_throbber(&rtsp_args->rtsp_th->busy);
}

void
on_closeabout_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(aboutdialog);
	aboutdialog = NULL;
}


void
on_credits_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *aboutimage;
	GtkWidget *credits;
	GtkWidget *creditsview;
	GtkRequisition requisition;

	aboutimage = lookup_widget(aboutdialog, "aboutimage");
	credits = lookup_widget(aboutdialog, "creditscroller");
	creditsview = lookup_widget(aboutdialog, "creditsview");
	gtk_widget_size_request(aboutimage, &requisition);
	gtk_widget_set_size_request(credits, requisition.width, requisition.height);
	gtk_text_buffer_set_text(gtk_text_view_get_buffer (GTK_TEXT_VIEW (creditsview)), CREDITS, -1);
	gtk_widget_hide(aboutimage);
	gtk_widget_show(credits);
	gtk_widget_hide(GTK_WIDGET(button));
	gtk_widget_show(lookup_widget(aboutdialog, "backabout"));
}


void
on_backabout_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_hide(lookup_widget(aboutdialog, "creditscroller"));
	gtk_widget_show(lookup_widget(aboutdialog, "aboutimage"));
	gtk_widget_hide(GTK_WIDGET(button));
	gtk_widget_show(lookup_widget(aboutdialog, "credits"));
}

