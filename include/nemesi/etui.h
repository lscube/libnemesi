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

#ifndef __E_TUI_H
#define __E_TUI_H

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include <nemesi/version.h>
#include <nemesi/main.h>
#include <nemesi/utils.h>
#include <nemesi/comm.h>

#define header()	nmsprintf(1, "\n%s - %s -- release %s (%s)\n\n", PROG_NAME, PROG_DESCR, VERSION, VERSION_NAME);

#define usage()	{ \
			nmsprintf(1, "Usage: %s [OPTION]... [URLNAME]\n", PROG_NAME); \
			nmsprintf(1, "\n"); \
			nmsprintf(1, "\t-h|--help\t\tdisplay this help and exit\n"); \
			nmsprintf(1, "\t-V|--version\t\tdisplay version information and exit\n"); \
			nmsprintf(1, "\t-v|--verbose [n]\tset verbose level to 'n'. Without 'n' the level will be incremented.\n"); \
			nmsprintf(1, "\t--nostatus\t\tDo not show buffers status and elapsed time.\n"); \
			nmsprintf(1, "\n   Output options:\n"); \
			nmsprintf(1, "\t--sysbuff <msec>\tsystem cache of decoded milliseconds\n"); \
			nmsprintf(1, "\t--ao <drv[:dev]>\tselect audio output driver (--ao help for a list)\n"); \
			nmsprintf(1, "\t--noaudio\tdo not inizialize audio output\n"); \
			nmsprintf(1, "\t--vo <drv[:opt]>\tselect video output driver (--vo help for a list)\n"); \
			nmsprintf(1, "\t--novideo\tdo not inizialize video output\n"); \
			nmsprintf(1, "\t--nodisk\tdo not inizialize diskwriter output\n"); \
			nmsprintf(1, "\n"); \
		}

#define version() { \
			nmsprintf(0, "Copyleft 2001 - giampaolo.mancini@polito.it\n"); \
			nmsprintf(0, "              - francesco.varano@polito.it\n"); \
			nmsprintf(0, "              - marco.penno@polito.it\n\n"); \
		  }

#define CL_MAIN_OPTIONS	"hv::V"		/* help, verbosity, version */
#define CL_MAIN_LONG_OPTIONS	{"version", 0, NULL, 'V'}, \
				{"help", 0, NULL, 'h'}, \
				{"verbose", 2, NULL, 'v'}, \
				{"ao", 1, NULL, 1}, \
				{"vo", 1, NULL, 2}, \
				{"sysbuff", 1, NULL, 3}, \
				{"gui", 0, NULL, 4}, \
				{"nostatus", 0, NULL, 5}, \
				{"tui", 0, NULL, 6}, \
				{"noaudio", 0, NULL, 7}, \
				{"novideo", 0, NULL, 8}, \
				{"nodisk", 0, NULL, 9}
#define CL_UI_OPTIONS	""

#define CL_OPTIONS	CL_MAIN_OPTIONS CL_UI_OPTIONS
#define CL_LONG_OPTIONS CL_MAIN_LONG_OPTIONS

int ui(struct RTSP_args *, NMSUiHints *, int, char **);
int parse_cl(int, char **, NMSCLOptions *);
int parse_prompt(struct RTSP_args *, char *);
int seturlname(struct RTSP_args *, char *);
int throbber(struct RTSP_Thread *);
int send_open(struct RTSP_args *, char *);
int send_play(struct RTSP_args *, char *);
int send_pause(struct RTSP_args *, char);
int send_close(struct RTSP_args *);
int get_infos(struct RTSP_args *);

#endif
