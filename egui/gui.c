/*
 * Initial main.c file generated by Glade. Edit as required.
 * Glade will not overwrite this file.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "interface.h"
#include "support.h"
#include "callbacks.c"

#include <nemesi/egui.h>
#include <nemesi/etui.h>

int gui(struct RTSP_args *rtsp_args, NMSUiHints *ui_hints, int argc, char *argv[])
{
  GtkWidget *nemesi;

  gtk_set_locale ();
  gtk_init (&argc, &argv);

  // add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
  add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/throbber");

  /*
   * The following code was added by Glade to create one of each component
   * (except popup menus), just so that you see something after building
   * the project. Delete any components that you don't want shown initially.
   */
  nemesi = create_nemesi ();

  save_static_data(nemesi, rtsp_args); // must be done fist of all
  load_throbber();

	update_toolbar();
	if (ui_hints->url) {
		nmsprintf(1, "Connect: Please wait, opening \"%s\"", ui_hints->url);
		send_open(rtsp_args, ui_hints->url);
		gui_throbber(rtsp_args);
		printf("\nthrobber\n");
	} else
		nmsprintf(1, "Please, enter a command or press 'h' for help\n\n");
  gtk_widget_show (nemesi);

  gtk_main ();
  return 0;
}

