#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include <nemesi/egui.h>
#include <nemesi/etui.h>

static struct RTSP_args *rtsp_args;

void save_rtsp_args(gpointer data)
{
	rtsp_args = data;
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

	if (gtk_toggle_button_get_active(togglebutton)) {
		send_play(rtsp_args, argstr);
		printf("Play\n");
	} else {
		send_pause(rtsp_args, 'z');
		printf("Pause\n");
	}
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

