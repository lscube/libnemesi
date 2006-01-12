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

/*! \brief Set the correct field in License definition.
 *
 * The sdp_l string is in the form, coming from an sdp description,
 * <param>=<value>. According to <param>, we set the right field.  Warning the
 * function does't copy the string, it sets just the right pointer to sdp_l, so
 * the sdp_l parameter cannot be a temporary string.
 */
int cc_set_sdplicense(CCLicense *cc, char *sdp_l)
{
	char *cclicenses[][2] = CC_LICENSE;
	unsigned int i;

	// shawill: sizeof(cclicenses)/sizeof(*cclicenses) == number of couples name-description present
	for(i=0; i<sizeof(cclicenses)/sizeof(*cclicenses); i++) {
		if (!strncmpcase(sdp_l, cclicenses[i][CC_ATTR_NAME], strlen(cclicenses[i][CC_ATTR_NAME]))) {
			// XXX: we do not duplicate the string!!! Do we have to do that?
			/* set the correct field using CCLicense struct like an array of strings
			 * skipping the sdp param and setting the pointer after the colon */
			((char **)cc)[i] = sdp_l + strlen(cclicenses[i][CC_ATTR_NAME]) + 1;
			return 0;
		}
	}

	return 1;
}

