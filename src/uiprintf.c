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

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include <nemesi/comm.h>

#ifdef USE_UIPRINTF

int uipipe[2];

int uiprintf(const char *fmt, ...)
{
	va_list ap;
	int fd;
	FILE *uistderr;
	
	if( (fd=dup(UIERROR_FILENO)) < 0 ){
		fprintf(stderr, "\nfailed duplicating UIERROR_FILENO\n");
		return 0;
	}
	if( !(uistderr=fdopen(fd, "a")) ){
		fprintf(stderr, "\nfailed opening uistderr stream.\n");
		return 1;
	}

	va_start(ap, fmt);

	vfprintf(uistderr, fmt, ap);

	va_end(ap);
	fclose(uistderr);
	close(fd);

	return 0;
}
#endif // USE_UIPRINTF
