#include <gtk/gtk.h>


void save_static_data(gpointer new_nemesi, gpointer new_rtsp_args);
void update_toolbar(void);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_toggle_play_pause_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_stop_cmd_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_close_cmd_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_open_cmd_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancelbutton1_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_okbutton1_clicked                   (GtkButton       *button,
                                        gpointer         user_data);


void
on_aboutdialog_realize                 (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_closeabout_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_credits_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_backabout_clicked                   (GtkButton       *button,
                                        gpointer         user_data);
