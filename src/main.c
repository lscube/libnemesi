/* * 
 *  ./src/main.c: $Revision: 1.6 $ -- $Date: 2003/01/15 17:39:13 $
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
#include <nemesi/rtpptdefs.h>
#include <nemesi/preferences.h>

#include <nemesi/audio.h>

struct Preference preferences[] = {
	/* output */ { "output", "card", "card/diskraw/diskdecoded", "Select where the stream will be written."},
	PREFS_TABLE_END
};

int (*decoders[128])(char *, int, uint8 *(*)());

struct rtp_pt_def rtp_pt_defs[128] = {
/*   0 -   3 */	{ "PCMU", AU, 8000, 1},		{ "res", AU, 0, 0},		{ "G726-32", AU, 8000, 1},	{ "GSM", AU, 8000, 1},
/*   4 -   7 */	{ "res", AU, 0, 0},		{ "DVI4", AU, 8000, 1},		{ "DVI4", AU, 16000, 1},	{ "LPC", AU, 8000, 1},
/*   8 -  11 */	{ "PCMA", AU, 8000, 1},		{ "G722", AU, 8000, 1},		{ "L16", AU, 44100, 2},		{ "L16", AU, 44100,1},
/*  12 -  15 */	{ "QCELP", AU, 8000, 1},	{ "res", AU, 0, 0},		{ "MPA", AU, 90000, 0},		{ "G728", AU, 8000, 1},
/*  16 -  19 */	{ "DVI4", AU, 11025, 1},	{ "DVI4", AU, 22050, 1},	{ "G729", AU, 8000, 1},		{ "res", AU, 0, 0},
/*  20 -  23 */	{ "unas", AU, 0, 0},		{ "unas", AU, 0, 0},		{ "unas", AU, 0, 0},		{ "unas", AU, 0, 0},
/*  24 -  27 */	{ "unas", VI, 0, 0},		{ "CelB", VI, 90000, 0},	{ "JPEG", VI, 90000, 0},	{ "unas", VI, 0, 0},
/*  28 -  31 */	{ "nv", VI, 90000, 0},		{ "unas", VI, 0, 0},		{ "unas", VI, 0, 0},		{ "H261", VI, 90000, 0},
/*  32 -  35 */	{ "MPV", VI, 90000, 0},		{ "res", VI, 0, 0},		{ "res", VI, 0, 0},		{ "unas", NA, 0, 0},
/*  36 -  39 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  40 -  43 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  44 -  47 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  48 -  51 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  52 -  55 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  56 -  59 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  60 -  63 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  64 -  67 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  68 -  71 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  72 -  75 */	{ "res", NA, 0, 0},		{ "res", NA, 0, 0},		{ "res", NA, 0, 0},		{ "res", NA, 0, 0},
/*  76 -  79 */	{ "res", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  80 -  83 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  84 -  87 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  88 -  91 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  92 -  95 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  96 -  99 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 100 - 103 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 104 - 107 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 108 - 111 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 112 - 115 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 116 - 119 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 120 - 123 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 124 - 127 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0}
};

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
		uiprintf("Cannot set RTSP Thread attributes!\n");
		exit(1);
	}

	if ( audio_init() )
		exit(1);

	if ( diskwriter_init() )
		exit(1);

	if ((n = pthread_create(&rtsp_tid, NULL, &rtsp, (void *) rtsp_args)) > 0) {
		fprintf(stderr, "Cannot create RTSP Thread: %s\n", strerror(n));
		exit(1);
	}
	if ((n=ui(rtsp_args, argc, argv)) > 0)
		exit(1);
	/* THREAD CANCEL */	
	uiprintf("Sending cancel signal to all threads\n");
	if(rtsp_tid > 0){
			uiprintf("Sending cancel signal to RTSP Thread (ID: %ld)\n", rtsp_tid);
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
