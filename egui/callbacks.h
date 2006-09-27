#include <gtk/gtk.h>


// *** generic functions *** //
void save_static_data(gpointer new_nemesi, gpointer new_rtsp_ctl);
void update_toolbar(void);
void view_info(GtkWidget *);
void hide_info(void);
// *** ENDOF generic functions *** //
//
// *** main window *** //

void on_open1_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_quit1_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_about1_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_toggle_play_pause_toggled(GtkToggleToolButton * togglebutton,
				  gpointer user_data);

void on_stop_cmd_clicked(GtkButton * button, gpointer user_data);

void on_close_cmd_clicked(GtkButton * button, gpointer user_data);

void on_open_cmd_clicked(GtkButton * button, gpointer user_data);


void on_aboutdialog_realize(GtkWidget * widget, gpointer user_data);

gboolean on_nemesi_configure_event(GtkWidget * widget,
				   GdkEventConfigure * event,
				   gpointer user_data);
// *** ENDOF main window *** //
//
// *** open dialog *** //

void on_urlname_realize(GtkWidget * widget, gpointer user_data);

void on_opendialog_response(GtkDialog * dialog, gint response_id,
			    gpointer user_data);
// *** ENDOF open dialog *** //
//
// *** about dialog *** //

void on_aboutdialog_realize(GtkWidget * widget, gpointer user_data);

void on_aboutdialog_response(GtkDialog * dialog, gint response_id,
			     gpointer user_data);
// *** ENDOF about dialog *** //
//
// *** info dialog *** //
//
// *** ENDOF info dialog *** //
//
// *** ENDOF event functions *** //
