/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *  	
 *  	Giampaolo "mancho" Mancini - manchoz@inwind.it
 *	Francesco "shawill" Varano - shawill@infinto.it
 *
 *  NeMeSI is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NeMeSI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NeMeSI; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 * */

#include <nemesi/comm.h>

#include <nemesi/main.h>
#include <nemesi/etui.h>
#include <nemesi/egui.h>
#include <nemesi/decoder.h>

#define GLOBAL_PREFERENCES
#include <nemesi/preferences.h>

int main(int argc, char *argv[])
{
	struct rtsp_ctrl *rtsp_ctl;
	nms_out_hints output_hints = {NULL, NULL, NULL, 0};
	nms_ui_hints ui_hints = { 0, 0, NULL };
	nms_rtsp_hints rtsp_hints = { -1 };
	nms_cl_opts cl_opt = { &output_hints, &ui_hints, &rtsp_hints };
	int n;
	char *slash;
	
	// extern int (*decoders[])(char *, int, uint8 *(*)());

	// nms_printf(0, "\nWelcome! This is %s - %s -- version %s (%s)\n\n", PROG_NAME, PROG_DESCR, VERSION, VERSION_NAME);
	nms_header();

	// command line parsing
	if ( (n=parse_cl(argc, argv, &cl_opt)) )
		exit((n<0) ? 1 : 0);

	memset(decoders, 0, 128*sizeof(int (*)()));

	srand(time(NULL));
	
#ifdef USE_UIPRINTF
	if(pipe(uipipe) < 0)
		exit( nms_printf(NMSML_FATAL, "Cannot create UI pipe!\n") );
#endif // USE_UIPRINTF

	if ((n = load_plugins()) > 0)
		exit( nms_printf(NMSML_FATAL, "Cannot load plugins: %s\n", strerror(n)) );

	// output initialization
	if (output_init(&output_hints))
		exit( nms_printf(NMSML_FATAL, "Error initializing output module\n") );

	if ( !(rtsp_ctl = rtsp_init(&rtsp_hints)) )
		exit( nms_printf(NMSML_FATAL, "Cannot initialize RTSP: %s\n", strerror(errno)) );
	// UI interface function
	if (argv[0]) // if we are called with the initial 'g' => start gui
		if ((*argv[0]=='g') || ((slash=strrchr(argv[0],'/')) && (*(slash + 1) == 'g'))) {
			ui_hints.gui = 1;
			nms_statusprintf(NO_STATUS, NULL);
		}
	if (ui_hints.gui)
#if HAVE_GUI
		gui(rtsp_ctl, &ui_hints, argc, argv);
	else
#else	// HAVE_GUI
		nms_printf(NMSML_ERR, "no GUI present: falling back to e-TUI\n");
#endif	// HAVE_GUI
		if ((n=ui(rtsp_ctl, &ui_hints, argc, argv)) > 0)
			exit(1);

	rtsp_uninit(rtsp_ctl);

	output_uninit();

	// if (output_hints.audio)
		free(output_hints.audio);
	// if (output_hints.video)
		free(output_hints.video);
	// if (output_hints.diskwriter)
		free(output_hints.diskwriter);
		free(ui_hints.url);

	unload_plugins();

	nms_printf(NMSML_NORM, "\nBye bye!\n\n");

	exit(0);
}
