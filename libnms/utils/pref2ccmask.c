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

#include <string.h>

#include <nemesi/ccprefs.h>

int pref2ccmask(CCPermsMask *mask)
{
	if (!strcmp(get_pref("acceptAll"),"yes"))
		memset(mask, 0xFF, sizeof(CCPermsMask));
	else {
		memset(mask, 0, sizeof(CCPermsMask));
		if (!strcmp(get_pref("attribution"), "yes"))
			mask->by = 1;
		if (!strcmp(get_pref("nonCommercial"), "yes"))
			mask->nc = 1;
		if (!strcmp(get_pref("noDerivs"), "yes"))
			mask->nd = 1;
		if (!strcmp(get_pref("shareAlike"), "yes"))
			mask->sa = 1;
		// special licenses
		if (!strcmp(get_pref("publicDomain"), "yes"))
			mask->spec_license = CC_PD;
	}
		
	return 0;
}

