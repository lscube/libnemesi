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

#include <fcntl.h>
#include <nemesi/comm.h>
#include <nemesi/etui.h>

int ui(struct RTSP_Ctrl *rtsp_ctrl, NMSUiHints *ui_hints, int argc, char **argv)
{
	char *urlname = ui_hints->url; // NULL;
	char optstr[256];

#ifdef USE_UIPRINTF
	fd_set rdset;
	int n;

	/*
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	*/
	/*
	if (fcntl(UIINPUT_FILENO, F_SETFL, O_NONBLOCK) < 0){
		fprintf(stderr, "fcntl() error!\n");
		return 1;
	}
	*/
	if (fcntl(UIINPUT_FILENO, F_SETFL, O_NONBLOCK) < 0)
		return nmserror(stderr, "fcntl() error!\n");

#endif // USE_UIPRINTF
	memset(optstr, '\0', 256);

	/*
	if (parse_ui_cl(argc, argv, &urlname) > 0)
		return 1;

	*/
	if (urlname != NULL) {
		nmsprintf(1, "Connect: Please wait, opening \"%s\"", urlname);
		send_open(rtsp_ctrl, urlname);
	} else
		nmsprintf(1, "Please, enter a command or press 'h' for help\n\n");
	
	while (1) {
		if(rtsp_ctrl->busy)
			throbber(rtsp_ctrl);
		fprintf(stderr, "[ %s ] => ", statustostr(rtsp_ctrl->status));

#ifdef USE_UIPRINTF
		FD_ZERO(&rdset);
		FD_SET(UIINPUT_FILENO, &rdset);
		FD_SET(STDIN_FILENO, &rdset);
		
		select(UIINPUT_FILENO+1, &rdset, NULL, NULL, NULL);
		if(FD_ISSET(UIINPUT_FILENO, &rdset)){
			fprintf(stderr, "\r"); // TODO Da ottimizzare
			// while((n=read(UIINPUT_FILENO, optstr, 1)) > 0)
			if((n=read(UIINPUT_FILENO, optstr, 256)) > 0)
				write(STDERR_FILENO, optstr, n);
		}
		if(FD_ISSET(STDIN_FILENO, &rdset)){
			scanf("%s", optstr);
			if (parse_prompt(rtsp_ctrl, optstr) > 0)
				return 0;
		}
#else // USE_UIPRINTF
		scanf("%s", optstr);
		if (parse_prompt(rtsp_ctrl, optstr) > 0)
			return 0;
#endif // USE_UIPRINTF
	}

	return 0;
}
