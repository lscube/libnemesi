/* * 
 *  $Id$
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

#include <nemesi/cc.h>
#include <nemesi/utils.h>

#include <nemesi/comm.h>

int issdplicense(char *sdp_a)
{
	char *cclicenses[][2] = CC_LICENSES;
	unsigned int i;

	// shawill: sizeof(cclicenses)/sizeof(*cclicenses) == number of couples name-description present
	for(i=0; i<sizeof(cclicenses)/sizeof(*cclicenses); i++) {
		if (!strncmpcase(sdp_a, cclicenses[i][CC_NAME], strlen(cclicenses[i][CC_NAME]))) {
			nmsprintf(3, "found valid cc field in SDP description (%s - %s)\n", cclicenses[i][CC_NAME], cclicenses[i][CC_DESCR]);
			return 1;
		}
	}

	return 0;
}

