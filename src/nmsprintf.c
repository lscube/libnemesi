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

/*!  \brief print function.
 *
 * This function manages the message printing on the <em>standard error</em>
 * stream according with <tt>verbosity</tt> parameter.
 * For each messege to print there is a verbosity level associated and if the
 * global verbosity is greater than this level then the message will be
 * printed.
 * The program global verbosity level is stored in a static variable inside the
 * function itself and can be initialized and modified simply calling the
 * <tt>vctpintf</tt> function with fmt set to <tt>NULL</tt>. In this case the
 * verbosity parameter will be stored as the new global verbosity level.
 *
 * \param verbosity verbosity level associated to message. In case of
 *        <tt>fmt==NULL</tt> it is used to set the global verbosity level.
 * \param fmt string containing message format (<tt>printf</tt> like).
 * \param ... variable list of arguments according with <tt>fmt</tt> description.
 * \return the number of characters written or negative value in case of error.
 */
#define MAX_VERBOSITY 3
#ifndef ENABLE_DEBUG
#define DEFAULT_VERBOSITY 1
#else // ENABLE_DEBUG => debug enabled
#define DEFAULT_VERBOSITY 3
#endif // ENABLE_DEBUG
int nmsprintf(int verbosity, const char *fmt, ...)
{
	static int verbose_nemesi=DEFAULT_VERBOSITY;
	int ret=0;
	va_list args;

	if ( fmt == NULL ) {
		if (verbosity > MAX_VERBOSITY)
			verbose_nemesi = MAX_VERBOSITY;
		else
			verbose_nemesi = verbosity;
		return 0;
	}

	if (verbosity < 0) {
		fprintf(stderr, "warning: verbosity level must be a non negative integer. Setting to 0\n");
		verbosity = 0;
	}

	if ( verbose_nemesi >= verbosity) {
		// fprintf(stderr, "\r");
		va_start(args, fmt);
		ret=vfprintf(stderr, fmt, args);
		va_end(args);
	}

	return ret;
}

