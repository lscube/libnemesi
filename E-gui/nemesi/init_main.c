/* * 
 *  ./E-gui/nemesi/init_main.c: $Revision: 1.2 $ -- $Date: 2002/11/07 12:12:05 $
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

#include <fcntl.h>

#include <ourhdr.h>
#include <nemesi/main.h>

pthread_t init_main(struct RTSP_args **rtsp_args)
{
	pthread_t rtsp_tid;
	pthread_attr_t rtsp_attr;	
	int n;

	extern int (*decoders[])(char *, int, uint8 *(*)());

	memset(decoders, 0, 128*sizeof(int (*)()));

	srand(time(NULL));
	
	if(pipe(uipipe) < 0){
		fprintf(stderr, "Cannot create UI pipe!\n");
		return -1;
	}

	if (fcntl(UIINPUT_FILENO, F_SETFL, O_NONBLOCK) < 0){
		printf("fcntl() error!\n");
		return -1;
	}

	if ((n = init_rtsp_args(rtsp_args)) > 0) {
		fprintf(stderr, "Cannot initialize RTSP: %s\n", strerror(n));
		return -1;
	}

	if ((n = load_plugins()) > 0) {
		fprintf(stderr, "Cannot load plugins: %s\n", strerror(n));
		return -1;
	}

	pthread_attr_init(&rtsp_attr);
	if (pthread_attr_setdetachstate(&rtsp_attr, PTHREAD_CREATE_JOINABLE) != 0){
		uiprintf("Cannot set RTP Thread attributes!\n");
		return -1;
	}

	if ((n = pthread_create(&rtsp_tid, NULL, &rtsp, (void *) *rtsp_args)) > 0) {
		fprintf(stderr, "Cannot create RTSP Thread: %s\n", strerror(n));
		return -1;
	}
	get_rtsp_args(*rtsp_args);

	return rtsp_tid;
}
