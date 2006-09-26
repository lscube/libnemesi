/* * 
 *  $Id:statustostr.c 267 2006-01-12 17:19:45Z shawill $
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

#include <nemesi/rtsp.h>
// #include <nemesi/utils.h>

char *statustostr(int status)
{

	switch (status) {
	case INIT:
		// return strcpy((char *)calloc(4+1, 1),"INIT");
		return "INIT";
	case READY:
		// return strcpy((char *)calloc(5+1, 1),"READY");
		return "READY";
	case PLAYING:
		// return strcpy((char *)calloc(7+1, 1),"PLAYING");
		return "PLAYING";
	case RECORDING:
		// return strcpy((char *)calloc(9+1, 1),"RECORDING");
		return "RECORDING";
	default:
		break;
	}
	// return strcpy((char *)calloc(4+1, 1),"BOUH");
	return "BOUH";

}
