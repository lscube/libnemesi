/* * 
 *  $Id:cc_printmask.c 267 2006-01-12 17:19:45Z shawill $
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
#include <nemesi/comm.h>

void cc_printmask(cc_perm_mask mask)
{
	unsigned int i;
	CC_BITMASK_T spec_l = (CC_BITMASK_T) mask.spec_license;

	if (spec_l) {
		for (i = 0; cc_spec_licenses[i].int_code; i++) {
			if ((cc_spec_licenses[i].int_code & spec_l))
				nms_printf(NMSML_ALWAYS, "%s: %s\n",
					   cc_spec_licenses[i].name,
					   cc_spec_licenses[i].descr);
		}
	}
	if (mask.by)
		nms_printf(NMSML_ALWAYS, "%s: %s\n", cc_by.name, cc_by.descr);
	if (mask.nc)
		nms_printf(NMSML_ALWAYS, "%s: %s\n", cc_nc.name, cc_nc.descr);
	if (mask.nd)
		nms_printf(NMSML_ALWAYS, "%s: %s\n", cc_nd.name, cc_nd.descr);
	if (mask.sa)
		nms_printf(NMSML_ALWAYS, "%s: %s\n", cc_sa.name, cc_sa.descr);
	nms_printf(NMSML_ALWAYS, "\n");
}
