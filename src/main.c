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

#define COMM_GLOBALS // define global variables
#include <nemesi/comm.h>

#include <nemesi/main.h>
#include <nemesi/etui.h>

#define GLOBAL_RTP_DEFS
#include <nemesi/rtpptdefs.h>

#define GLOBAL_PREFERENCES
#include <nemesi/preferences.h>

int main(int argc, char *argv[])
{
	struct RTSP_args *rtsp_args;
	pthread_t rtsp_tid;
	pthread_attr_t rtsp_attr;
	NMSOutputHints hints = {NULL, NULL, NULL, DEF_SYSBUFF_MS};
	int n;
	void *ret;
	
	// extern int (*decoders[])(char *, int, uint8 *(*)());

	// nmsprintf(0, "\nWelcome! This is %s - %s -- version %s (%s)\n\n", PROG_NAME, PROG_DESCR, VERSION, VERSION_NAME);
	header();

	// command line parsing
	if ( (n=parse_main_cl(argc, argv, &hints)) )
		exit((n<0) ? 1 : 0);

	memset(decoders, 0, 128*sizeof(int (*)()));

	srand(time(NULL));
	
#ifdef USE_UIPRINTF
	if(pipe(uipipe) < 0)
		exit( nmserror("Cannot create UI pipe!") );
#endif // USE_UIPRINTF

	if ((n = init_rtsp_args(&rtsp_args)) > 0)
		exit( nmserror("Cannot initialize RTSP: %s", strerror(n)) );

	if ((n = load_plugins()) > 0)
		exit( nmserror("Cannot load plugins: %s", strerror(n)) );

	// output initialization
	if (output_init(&hints))
		exit( nmserror("Error initialazing output module") );

	// Creation of RTSP Thread
	pthread_attr_init(&rtsp_attr);
	if (pthread_attr_setdetachstate(&rtsp_attr, PTHREAD_CREATE_JOINABLE) != 0)
		exit( nmserror("Cannot set RTSP Thread attributes!") );

	if ((n = pthread_create(&rtsp_tid, NULL, &rtsp, (void *) rtsp_args)) > 0)
		exit( nmserror("Cannot create RTSP Thread: %s", strerror(n)) );

	// UI interface function
	if ((n=ui(rtsp_args, argc, argv)) > 0)
		exit(1);

	/* THREAD CANCEL */	
	nmsprintf(2, "Sending cancel signal to all threads\n");
	if(rtsp_tid > 0){
		nmsprintf(2, "Sending cancel signal to RTSP Thread (ID: %ld)\n", rtsp_tid);
		if (pthread_cancel(rtsp_tid) != 0)
			nmsprintf(3, "Error while sending cancelation to RTSP Thread.\n");
		else
			pthread_join(rtsp_tid, (void **)&ret);
		if ( ret != PTHREAD_CANCELED )
			nmsprintf(3, "Warning! RTSP Thread joined, but  not canceled!\n");
	} else
		nmsprintf(2, "Cannot send cancel signal to RTSP Thread\n");

	output_uninit();

	if (hints.audio)
		free(hints.audio);
	if (hints.video)
		free(hints.video);
	if (hints.diskwriter)
		free(hints.diskwriter);

	nmsprintf(1, "\nBye bye!\n\n");

	exit(0);
}
