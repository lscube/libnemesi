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
#include <nemesi/comm.h>

/*! \brief License permissions checking function.
 *
 * The function will check the license given as first paramenter is compatible
 * with the permissions accepted by user in <tt>Permission Mask</tt> given as
 * second parameter. Exiting, the function will return in <tt>mask</tt>
 * parameter the mask of permissions that conflict with license.
 *
 * \param license CCLicense struct of license.
 *
 * \param mask pointer to permission mask used as input paramter for
 * accepted permissions by user and as return value for conflicting
 * permissions.
 */
int cc_prms_chk(CCLicense *license, CCPermsMask *mask)
{
	CCPermsMask parsedmsk;

	if (!license)
		return nmserror("no license defined");

	// uriLicense parse
	if (!license->uriLicense)
		return nmserror("no uriLicense present: could not parse license uri");
	if ( (cc_parse_urilicense(license->uriLicense, &parsedmsk)) )
		return nmserror("cannot parse uriLicense (cc_prms_mask)");

	*((CC_BITMASK_T *)mask) = ~(*((CC_BITMASK_T *)mask)) & *((CC_BITMASK_T *)&parsedmsk);

	if (*((CC_BITMASK_T *)mask)) {
		nmserror("You didn't accept some requested conditions of license\n");
		if (mask->by)
			nmsprintf(1, "%s\n", cc_by.name);
		if (mask->nc)
			nmsprintf(1, "%s\n", cc_nc.name);
		if (mask->nd)
			nmsprintf(1, "%s\n", cc_nd.name);
		if (mask->sa)
			nmsprintf(1, "%s\n", cc_sa.name);
		return 1;
	}
	
	return 0;
}

