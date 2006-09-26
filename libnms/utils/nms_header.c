/* * 
 *  $Id:nmsheader.c 267 2006-01-12 17:19:45Z shawill $
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *  	
 *  	Giampaolo "mancho" Mancini - giampaolo.mancini@polito.it
 *	Francesco "shawill" Varano - francesco.varano@polito.it
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

#include <string.h>

#include <nemesi/version.h>
#include <nemesi/comm.h>

inline void nms_header(void)
{
	char svnrev[32] = "";
#ifdef SVNREV
	char *tkn;

	// strcat(svnrev, SVNREV+1);
	snprintf(svnrev, sizeof(svnrev), "- SVN %s", SVNREV + 1);
	if ((tkn = strchr(svnrev, '$')))
		*tkn = '\0';
#endif
	nms_printf(NMSML_ALWAYS, "\n" NMSCLR_BLUE_BOLD "%s - %s -- release %s %s(%s)\n\n" NMSCLR_DEFAULT, PROG_NAME,
		   PROG_DESCR, VERSION, svnrev, VERSION_NAME);
}
