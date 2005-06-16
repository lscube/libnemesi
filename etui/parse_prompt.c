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

#include <nemesi/etui.h>
#include <nemesi/preferences.h>


int parse_prompt(struct RTSP_Ctrl *rtsp_ctrl, char *optstr)
{
	char argstr[256];
	char opt;

	memset(argstr, '\0', sizeof(argstr));
	opt = optstr[0];

	switch (opt) {
	case 'h':
	case '?':
		nmsprintf(NMSML_NORM, "\nHelp:\n");
		nmsprintf(NMSML_NORM, "\t* help or ?           print this help\n");
		nmsprintf(NMSML_NORM, "\t* quit                quit\n");
		nmsprintf(NMSML_NORM, "\t* version             print infos about NeMeSI.\n");
		nmsprintf(NMSML_NORM, "\t* open urlname        connect to urlname\n");
		nmsprintf(NMSML_NORM, "\t* info                print info about the session\n");
		nmsprintf(NMSML_NORM, "\t* play [range]        start the playback\n");
		nmsprintf(NMSML_NORM, "\t* zause               pause the playback\n");
		nmsprintf(NMSML_NORM, "\t* stop                stop the playback\n");
		nmsprintf(NMSML_NORM, "\t* close               close the connection\n");
		nmsprintf(NMSML_NORM, "\t* edit option value   change the value of specified option\n");
		nmsprintf(NMSML_NORM, "\t* list                show list of editable options, with assigned values and short description\n");
		nmsprintf(NMSML_NORM, "\nEvery command accepts also its first char as abbreviation (e.g. \'h\' for help).\n\n");
		break;
	case 'q':
		send_close(rtsp_ctrl);
		return 1;
	case 'v':
		nmsprintf(NMSML_NORM, "\nThis is %s - %s -- release %s (%s)\n", PROG_NAME, PROG_DESCR, VERSION, VERSION_NAME);
		nmsprintf(NMSML_NORM, "Copyleft 2001 - mancho@cclif.polito.it\n");
		nmsprintf(NMSML_NORM, "              - shawill@cclinf.polito.it\n\n");
		break;
	case 'o':
		if ( sscanf(optstr, "%*s %s", argstr) != 1)
			nmsprintf(NMSML_ERR, "Can't connect: no address given\n");
		else {
			nmsprintf(NMSML_NORM, "Connect: Please wait, opening \"%s\"\n", argstr);
			send_open(rtsp_ctrl, argstr);
		}
		break;
	case 'i':
		get_infos(rtsp_ctrl);
		break;
	case 'p':
		// fgets(argstr, 256, stdin);
		sscanf(optstr, "%*s %s", argstr);
		send_play(rtsp_ctrl, argstr);
		break;
	case 'z':
		send_pause(rtsp_ctrl, 'z');
		break;
	case 's':
		send_pause(rtsp_ctrl, 's');
		break;
	case 'c':
		send_close(rtsp_ctrl);
		break;
	case 'e':
		/*
		fgets(argstr, 256, stdin);
		*(argstr+strlen(argstr)-1)='\0';
		edit_pref(argstr+1); // c'è lo spazio all'inizio, lo togliamo
		 */
		if ( sscanf(optstr, "%*s %[^\n]", argstr) > 0)
			edit_pref(argstr);
		else
			nmsprintf(NMSML_ERR, "No preference give to edit\n");
		break;
	case 'l':
		list_prefs();
		break;
	default:
		nmsprintf(NMSML_ERR, "Unknown command: %s\n", optstr);
		nmsprintf(NMSML_NORM, "Press \'h\' for help.\n\n");
		break;
	}
	return 0;
}
