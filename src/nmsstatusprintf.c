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
#include <stdarg.h>

#include <nemesi/comm.h>

/*! \brief buffer status print function
 *
 * This function manages the printing on standard error of the stutus of both
 * buffer fill level and audio and video elapsed time.  There are two
 * interested threads that give messages in concurrency and so this is the
 * function that manages the correct printing on the screeen.
 *
 * \param cmd kind of status that printed: currently: BUFFERS_STATUS or
 * ELAPSED_STATUS
 * \param fmt string containing message format (<tt>printf</tt> like).
 * \param ... variable list of arguments according with <tt>fmt</tt> description.
 * \return the number of characters written or negative value in case of error.
 */

int nmsstatusprintf(int cmd, const char *fmt, ...)
{
	static char buffers[256]="\0";
	static char elapsed[256]="\0";
	static char no_status = 0;
	int ret=0;
	va_list args;
	int v; // verbosity

	if ( cmd == PRINT_STATUS )
		no_status = 0;
	if ( no_status )
		return 0;
	v = nmsprintf(-1, NULL);
	switch( cmd ) {
		case NO_STATUS:
			no_status = 1;
			return 0;
			break;
		case BUFFERS_STATUS:
			if (v>=BUFFERS_STATUS_VERBOSITY) {
				va_start(args, fmt);
				ret=vsprintf(buffers, fmt, args);
				va_end(args);
			}
			break;
		case ELAPSED_STATUS:
			if (v>=ELAPSED_STATUS_VERBOSITY) {
				va_start(args, fmt);
				ret=vsprintf(elapsed, fmt, args);
				va_end(args);
			}
			break;
		default:
			return 0;
			break;
	}

	if ( (v>=BUFFERS_STATUS_VERBOSITY) && (v>=ELAPSED_STATUS_VERBOSITY) )
		fprintf(stderr, "\r%s - %s   ", elapsed, buffers);
	else if ( v>=BUFFERS_STATUS_VERBOSITY )
		fprintf(stderr, "\r%s   ", buffers);
	else if ( v>=ELAPSED_STATUS_VERBOSITY )
		fprintf(stderr, "\r%s   ", elapsed);

	return ret;
}

