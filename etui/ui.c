/* * 
 *  ./etui/ui.c: $Revision: 1.2 $ -- $Date: 2002/11/07 12:12:10 $
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
#include <nemesi/etui.h>

int ui(struct RTSP_args *rtsp_args, int argc, char **argv)
{
	char *urlname = NULL;
	char optstr[256];
	fd_set rdset;
	int n;

	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	if (fcntl(UIINPUT_FILENO, F_SETFL, O_NONBLOCK) < 0){
		fprintf(stderr, "fcntl() error!\n");
		return 1;
	}
	memset(optstr, '\0', 256);

	if (parse_cl(argc, argv, &urlname) > 0)
		return 1;
	fprintf(stderr, "\nWelcome! This is %s - %s -- version %s (%s)\n", PROG_NAME, PROG_DESCR, VERSION, VERSION_NAME);
	if (urlname != NULL) {
		fprintf(stderr, "Connect: Please wait, opening \"%s\"", urlname);
		send_open(rtsp_args, urlname);
	} else
		fprintf(stderr, "Please, enter a command or press 'h' for help\n\n");
	
	while (1) {

		FD_ZERO(&rdset);
		FD_SET(UIINPUT_FILENO, &rdset);
		FD_SET(STDIN_FILENO, &rdset);
		
		if(rtsp_args->rtsp_th->busy)
			throbber(rtsp_args->rtsp_th);
		fprintf(stderr, "[ %s ] => ", statustostr(rtsp_args->rtsp_th->status));

		select(UIINPUT_FILENO+1, &rdset, NULL, NULL, NULL);
		if(FD_ISSET(UIINPUT_FILENO, &rdset)){
			fprintf(stderr, "\r"); /* TODO Da ottimizzare */
			while((n=read(UIINPUT_FILENO, optstr, 1)) > 0)
				write(STDERR_FILENO, optstr, n);
		}
		if(FD_ISSET(STDIN_FILENO, &rdset)){
			scanf("%s", optstr);
			if (parse_prompt(rtsp_args, optstr) > 0)
				return 0;
		}
	}

	return 0;
}
