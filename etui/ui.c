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

int ui(rtsp_ctrl * rtsp_ctl, nms_ui_hints * ui_hints, int argc, char **argv)
{
	char *urlname = ui_hints->url;	// NULL;
	char optstr[256];
	// select vars
	// struct timeval seleep;
	fd_set rdset;
	int maxfd;
	char event;

#ifdef USE_UIPRINTF
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
		return nms_printf(NMSML_FATAL, "fcntl() error!\n");

#endif				// USE_UIPRINTF
	memset(optstr, '\0', sizeof(optstr));

	/*
	   if (parse_ui_cl(argc, argv, &urlname) > 0)
	   return 1;

	 */
	if (urlname != NULL) {
		nms_printf(NMSML_NORM, "Connect: Please wait, opening \"%s\"", urlname);
		nms_open(rtsp_ctl, urlname, throbber, rtsp_ctl);
		if (!ui_hints->interactive)
			nms_play(rtsp_ctl, -1, -1);
	} else
		nms_printf(NMSML_NORM, "Please, enter a command or press 'h' for help\n\n");

	while (1) {
		if (rtsp_is_busy(rtsp_ctl))
			// rtsp_wait(rtsp_ctl);
			throbber(rtsp_ctl);
		fprintf(stderr, "\r[ %s ] => ", statustostr(rtsp_status(rtsp_ctl)));

		FD_ZERO(&rdset);
		FD_SET(STDIN_FILENO, &rdset);
		maxfd = STDIN_FILENO;
#ifdef USE_UIPRINTF
		FD_SET(UIINPUT_FILENO, &rdset);
		maxfd = max(maxfd, UIINPUT_FILENO);
#endif				// USE_UIPRINTF

		/*
		   seleep.tv_sec = 1;
		   seleep.tv_usec = 0;

		   select(maxfd+1, &rdset, NULL, NULL, &seleep);
		 */
		select(maxfd + 1, &rdset, NULL, NULL, NULL);
#ifdef USE_UIPRINTF
		if (FD_ISSET(UIINPUT_FILENO, &rdset)) {
			fprintf(stderr, "\r");	// TODO Da ottimizzare
			// while((n=read(UIINPUT_FILENO, optstr, 1)) > 0)
			if ((n = read(UIINPUT_FILENO, optstr, 256)) > 0)
				write(STDERR_FILENO, optstr, n);
		}
//#else // USE_UIPRINTF
#endif				// USE_UIPRINTF
		if (FD_ISSET(STDIN_FILENO, &rdset)) {
			/*
			   scanf("%s", optstr);
			   if (parse_prompt(rtsp_ctl, optstr) > 0)
			   return 0;
			   }
			 */
			// scanf("%s", optstr);
			if (ui_hints->interactive) {
				fgets(optstr, sizeof(optstr) - 1, stdin);
				optstr[strlen(optstr) - 1] = '\0';
				if (strlen(optstr) && (parse_prompt(rtsp_ctl, optstr) > 0))
					return 0;
			} else {
				printf("\ntui event\n");
//                              event=getchar();
				read(STDIN_FILENO, &event, 1);
				printf("\ntui event\n");
				if (tui_event(rtsp_ctl, event) > 0)
					return 0;
			}
		}
	}

	return 0;
}
