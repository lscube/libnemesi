/* * 
 *  ./etui/parse_prompt.c: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:19 $
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

#include <nemesi/etui.h>


int parse_prompt(struct RTSP_args *rtsp_args, char *optstr)
{
	char argstr[256];
	char opt;

	memset(argstr, '\0', 256);
	opt = optstr[0];

	switch (opt) {
	case 'h':
	case '?':
		printf("\nHelp:\n");
		printf("\t* help or ?           print this help\n");
		printf("\t* quit                quit\n");
		printf("\t* version             print infos about NeMeSI.\n");
		printf("\t* open urlname        connect to urlname\n");
		printf("\t* info                print info about the session\n");
		printf("\t* play [range]        start the playback\n");
		printf("\t* zause               pause the playback\n");
		printf("\t* stop                stop the playback\n");
		printf("\t* close               close the connection\n");
		printf("\nEvery command accepts also its first char as abbreviation (e.g. \'h\' for help).\n\n");
		break;
	case 'q':
		send_close(rtsp_args);
		return 1;
	case 'v':
		printf("\nThis is %s - %s -- release %s (%s)\n", PROG_NAME, PROG_DESCR, VERSION, VERSION_NAME);
		printf("Copyleft 2001 - mancho@cclif.polito.it\n");
		printf("              - shawill@cclinf.polito.it\n\n");
		break;
	case 'o':
		scanf("%s%*c", argstr);
		printf("Connect: Please wait, opening \"%s\"\n", argstr);
		send_open(rtsp_args, argstr);
		break;
	case 'i':
		get_infos(rtsp_args);
		break;
	case 'p':
		fgets(argstr, 256, stdin);
		send_play(rtsp_args, argstr);
		break;
	case 'z':
		send_pause(rtsp_args, 'z');
		break;
	case 's':
		send_pause(rtsp_args, 's');
		break;
	case 'c':
		send_close(rtsp_args);
		break;
	default:
		printf("\nUnknown command: %s\n", optstr);
		printf("Press \'h\' for help.\n\n");
		break;
	}
	return 0;
}
