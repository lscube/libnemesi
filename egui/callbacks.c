#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include <nemesi/egui.h>
#include <nemesi/etui.h>

static GtkWidget *nemesi;
static GtkWidget *opendialog=NULL;
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

int load_throbber(void)
{
	GdkImage *throbber_pix;
	GtkImage *image;

	image = lookup_widget(nemesi, "throbber");
	throbber_pix = create_pixmap(NULL, "rest.png");
	gtk_image_set_from_image(image, throbber_pix, NULL);
	// gtk_image_set_from_file(image, "/usr/local/share/nemesi/throbber/rest.png");
	return 0;
}

void
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_main_quit();
}


void
on_cut1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_delete1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

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
	gui_throbber(rtsp_args);

}

void
on_stop_cmd_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	send_pause(rtsp_args, 's');
	gui_throbber(rtsp_args);
}


void
on_close_cmd_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	send_close(rtsp_args);
	gui_throbber(rtsp_args);
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
	gchar *url;

	url = gtk_entry_get_text(GTK_ENTRY(lookup_widget(opendialog, "urlname")));
	gtk_widget_destroy(opendialog);
	opendialog = NULL;
	send_open(rtsp_args, url);
	gui_throbber(rtsp_args);
}

