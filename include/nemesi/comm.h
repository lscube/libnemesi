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

#ifndef COMM_GLOBALS
#define COMM_EXTERN extern
#else // COMM_GLOBALS
#define COMM_EXTERN
#endif // COMM_GLOBALS

COMM_EXTERN int uipipe[2];

#define UIINPUT_FILENO uipipe[0]
#define UIERROR_FILENO uipipe[1]

/***** BLANK LINE * BLANK LINE * BLANK LINE * BLANK LINE * BLANK LINE * BLANK LINE * BLANK LINE * *****/
#define BLANK_LINE "                                                                                \n"
/***** BLANK LINE * BLANK LINE * BLANK LINE * BLANK LINE * BLANK LINE * BLANK LINE * BLANK LINE * *****/

/* Separators */
#define SEPARATOR "\n<------------------------------------------------------------->\n\n"
#define BEGIN_SEP "\n<--------------\n"
#define END_SEP "\n-------------->\n\n"

int uiprintf(const char *fmt, ...);
int uierror(const char *fmt, ...);

#undef COMM_GLOBALS
#undef COMM_EXTERN

#endif
