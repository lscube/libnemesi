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

/*!  \brief error function.
 *
 * This function just prints the error message described in <tt>fmt</tt> and
 * following parameters and return 1.
 *
 * \param fmt string containing message format (<tt>printf</tt> like). 
 * \param ... variable list of arguments according with <tt>fmt</tt> description.
 * \return 1. Error.  \see vctprintf
 */
int nmserror(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	nmscolour(stderr, NMSCLR_RED_BOLD);
	fprintf(stderr, "error: ");
	nmscolour(stderr, NMSCLR_DEFAULT);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);

	return 1;
}

