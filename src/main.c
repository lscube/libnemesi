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

#include <nemesi/main.h>
#include <nemesi/etui.h>

#define GLOBAL_RTP_DEFS
#include <nemesi/rtpptdefs.h>

#define GLOBAL_PREFERENCES
#include <nemesi/preferences.h>

#include <nemesi/audio.h>
#include <nemesi/diskwriter.h>

int main(int argc, char *argv[])
{
	struct RTSP_args *rtsp_args;
	pthread_t rtsp_tid;
	pthread_attr_t rtsp_attr;
	int n;
	void *ret;
	
	extern int (*decoders[])(char *, int, uint8 *(*)());

	memset(decoders, 0, 128*sizeof(int (*)()));

	srand(time(NULL));
	
	if(pipe(uipipe) < 0){
		fprintf(stderr, "Cannot create UI pipe!\n");
		exit(1);
	}
	if ((n = init_rtsp_args(&rtsp_args)) > 0) {
		fprintf(stderr, "Cannot initialize RTSP: %s\n", strerror(n));
		exit(1);
	}
	if ((n = load_plugins()) > 0) {
		fprintf(stderr, "Cannot load plugins: %s\n", strerror(n));
		exit(1);
	}

	pthread_attr_init(&rtsp_attr);
	if (pthread_attr_setdetachstate(&rtsp_attr, PTHREAD_CREATE_JOINABLE) != 0){
		fprintf(stderr, "Cannot set RTSP Thread attributes!\n");
		exit(1);
	}

	if ( audio_init() ) {
		fprintf(stderr, "Audio module not available: setting \"output\" to \"diskdecoded\"\n");
		rem_avail_pref("output card");
		edit_pref("output diskdecoded");
	}

	if ( diskwriter_init() ) {
		fprintf(stderr, "Disk Writer module not available\n");
		rem_avail_pref("output diskraw");
		rem_avail_pref("output diskdecoded");
		if ( strcmp("card", get_pref("output")) ) {
			fprintf(stderr, "\nNo output device available\n Cannot continue.\n");
			exit(1);
		}
	}

	if ((n = pthread_create(&rtsp_tid, NULL, &rtsp, (void *) rtsp_args)) > 0) {
		fprintf(stderr, "Cannot create RTSP Thread: %s\n", strerror(n));
		exit(1);
	}
	if ((n=ui(rtsp_args, argc, argv)) > 0)
		exit(1);
	/* THREAD CANCEL */	
	uiprintf("Sending cancel signal to all threads\n");
	if(rtsp_tid > 0){
			fprintf(stderr, "Sending cancel signal to RTSP Thread (ID: %ld)\n", rtsp_tid);
		if (pthread_cancel(rtsp_tid) != 0)
			fprintf(stderr, "Error while sending cancelation to RTSP Thread.\n");
		else
			pthread_join(rtsp_tid, (void **)&ret);
		if ( ret != PTHREAD_CANCELED )
			fprintf(stderr, "Warning! RTSP Thread joined, but  not canceled!\n");
	} else
		fprintf(stderr, "Cannot send cancel signal to RTSP Thread\n");

	audio_close();
	diskwriter_close();
	fprintf(stderr, "\nBye bye!\n\n");

	exit(0);
}
