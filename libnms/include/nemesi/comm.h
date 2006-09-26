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

#ifndef __COMM_H
#define __COMM_H

#include <stdio.h>

#define NMS_COLOURED

// #define USE_UIPRINTF
#undef USE_UIPRINTF

#ifdef USE_UIPRINTF

extern int uipipe[2];

#define UIINPUT_FILENO uipipe[0]
#define UIERROR_FILENO uipipe[1]

#endif				// USE_UIPRINTF

/***** BLANK LINE * BLANK LINE * BLANK LINE * BLANK LINE * BLANK LINE * BLANK LINE * BLANK LINE * *****/
#define BLANK_LINE "                                                                                \n"
/***** BLANK LINE * BLANK LINE * BLANK LINE * BLANK LINE * BLANK LINE * BLANK LINE * BLANK LINE * *****/

#ifdef NMS_COLOURED

// COLOURS definitions
#define NMSCLR_RED		"\033[0;31m"	// "\033[0;31;40m"
#define NMSCLR_GREEN		"\033[0;32m"	// "\033[0;32;40m"
#define NMSCLR_YELLOW		"\033[0;33m"	// "\033[0;33;40m"
#define NMSCLR_BLUE		"\033[0;34m"	// "\033[0;34;40m"
#define NMSCLR_MAGENTA		"\033[0;35m"	// "\033[0;35;40m"
#define NMSCLR_CYAN		"\033[0;36m"	// "\033[0;36;40m"
#define NMSCLR_WHITE		"\033[0;37m"	// "\033[0;37;40m"
#define NMSCLR_RED_BOLD		"\033[1;31m"	// "\033[1;31;40m"
#define NMSCLR_YELLOW_BOLD	"\033[1;33m"	// "\033[1;33;40m"
#define NMSCLR_BLUE_BOLD	"\033[1;34m"	// "\033[1;34;40m"
#define NMSCLR_DEFAULT		"\033[0m"

#define nmscolour(stm, clr)	fprintf(stm, clr)

#else				// NMS_COLOURED

#define NMSCLR_RED
#define NMSCLR_GREEN
#define NMSCLR_YELLOW
#define NMSCLR_BLUE
#define NMSCLR_MAGENTA
#define NMSCLR_CYAN
#define NMSCLR_WHITE
#define NMSCLR_RED_BOLD
#define NMSCLR_YELLOW_BOLD
#define NMSCLR_BLUE_BOLD
#define NMSCLR_DEFAULT

#define nmscolour(stm, clr)

#endif				// NMS_COLOURED

/* Separators */
#define SEPARATOR NMSCLR_CYAN"\n<------------------------------------------------------------->\n\n"NMSCLR_DEFAULT
#define BEGIN_SEP NMSCLR_CYAN"\n<--------------\n"NMSCLR_DEFAULT
#define END_SEP NMSCLR_CYAN"\n-------------->\n\n"NMSCLR_DEFAULT

// Definition of message levels
#define NMSML_GET_VERB	-1	// MUST BE < 0
#define NMSML_FATAL	0
#define NMSML_ERR	1
#define NMSML_WARN	2
#define NMSML_ALWAYS	3	// -v0
#define NMSML_NORM	4	// -v1
#define NMSML_VERB	5	// -v2
#define NMSML_DBG1	6	// -v3
#define NMSML_DBG2	7	// -v4
#define NMSML_DBG3	8	// -v5

#define NMSML_MAX	8

#ifdef NMS_COLOURED
#define NMSML_COLOURS	NMSCLR_RED_BOLD, \
			NMSCLR_RED_BOLD, \
			NMSCLR_YELLOW_BOLD, \
			NMSCLR_DEFAULT, \
			NMSCLR_DEFAULT, \
			NMSCLR_DEFAULT, \
			NMSCLR_CYAN, \
			NMSCLR_MAGENTA, \
			NMSCLR_GREEN
#endif

int nms_printf_default(int level, const char *fmt, ...);

#define NO_STATUS 0
#define PRINT_STATUS 1
#define ELAPSED_STATUS 2
#define BUFFERS_STATUS 3
#define ELAPSED_STATUS_VERBOSITY NMSML_NORM
#define BUFFERS_STATUS_VERBOSITY NMSML_VERB
int nms_status_printf_default(int cmd, const char *fmt, ...);

int nms_verbosity_set(int);
int nms_verbosity_get(void);
extern int (*nms_printf) (int level, const char *fmt, ...);
extern int (*nms_statusprintf) (int cmd, const char *fmt, ...);

#ifdef USE_UIPRINTF
int uiprintf(const char *fmt, ...);
int uierror(const char *fmt, ...);

#endif				// USE_UIPRINTF

inline void nms_header(void);

#endif
