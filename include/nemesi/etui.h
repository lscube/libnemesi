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

#define header()	nmsheader();

#if HAVE_GUI
#define ui_usage()	nmsprintf(NMSML_NORM, "   --gui              Use Graphical User Interface\n"); \
			nmsprintf(NMSML_NORM, "   --tui              Use Textual User Interface (e-tui)\n");
#else	// HAVE_GUI
#define ui_usage()
#endif	// HAVE_GUI
#define usage()	{ \
			nmsprintf(NMSML_NORM, "Usage: %s [OPTION]... [URLNAME]\n", PROG_NAME); \
			nmsprintf(NMSML_NORM, "\n"); \
			nmsprintf(NMSML_NORM, "   -h|--help          Display this help and exit\n"); \
			nmsprintf(NMSML_NORM, "   -V|--version       Display version information and exit\n"); \
			nmsprintf(NMSML_NORM, "   -v|--verbose [n]   Set verbose level to 'n'. Without 'n' the level will be incremented.\n"); \
			ui_usage() \
			nmsprintf(NMSML_NORM, "   --nostatus         Do not show buffers status and elapsed time.\n"); \
			nmsprintf(NMSML_NORM, "\n Output options:\n"); \
			nmsprintf(NMSML_NORM, "   --sysbuff <msec>   System cache of decoded milliseconds\n"); \
			nmsprintf(NMSML_NORM, "   --ao <drv[:dev]>   Select audio output driver (--ao help for a list)\n"); \
			nmsprintf(NMSML_NORM, "   --vo <drv[:opt]>   Select video output driver (--vo help for a list)\n"); \
			nmsprintf(NMSML_NORM, "   --noaudio          Do not inizialize audio output\n"); \
			nmsprintf(NMSML_NORM, "   --novideo          Do not inizialize video output\n"); \
			nmsprintf(NMSML_NORM, "   --nodisk           Do not inizialize diskwriter output\n"); \
			nmsprintf(NMSML_NORM, "\n RTP options:\n"); \
			nmsprintf(NMSML_NORM, "   -p|--first-port [n]   Force RTP/RTCP ports choise.\n"); \
			nmsprintf(NMSML_NORM, "\n"); \
		}

#define version() { \
			nmsprintf(NMSML_ALWAYS, "Copyleft 2001 - giampaolo.mancini@polito.it\n"); \
			nmsprintf(NMSML_ALWAYS, "              - francesco.varano@polito.it\n"); \
			nmsprintf(NMSML_ALWAYS, "              - marco.penno@polito.it\n\n"); \
		  }

#define CL_MAIN_OPTIONS	"hv::Vp:"		/* help, verbosity, version, first-port */
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
				{"nodisk", 0, NULL, 9}, \
				{"first-port", 1, NULL, 'p'}
#define CL_UI_OPTIONS	""

#define CL_OPTIONS	CL_MAIN_OPTIONS CL_UI_OPTIONS
#define CL_LONG_OPTIONS CL_MAIN_LONG_OPTIONS

int ui(struct RTSP_Ctrl *, NMSUiHints *, int, char **);
int parse_cl(int, char **, NMSCLOptions *);
int parse_prompt(struct RTSP_Ctrl *, char *);
int throbber(struct RTSP_Ctrl *);
int send_open(struct RTSP_Ctrl *, char *);
int send_play(struct RTSP_Ctrl *, char *);
int send_pause(struct RTSP_Ctrl *, char);
int send_close(struct RTSP_Ctrl *);
int get_infos(struct RTSP_Ctrl *);

#endif
