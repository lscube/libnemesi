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

#include <nemesi/wsocket.h>

char *addr_ntop(const NMSaddr *addr, char *str, size_t len)
{
	switch (addr->family) {
		case AF_INET:
			if (inet_ntop(AF_INET, &addr->addr.in, str, len) == NULL)
				return(NULL);
			return(str);
			break;
#ifdef	IPV6
		case AF_INET6:
			if (inet_ntop(AF_INET6, &addr->addr.in6, str, len) == NULL)
				return(NULL);
			return(str);
			break;
#endif
			
#if 0 // not yet supported by NMSaddr
#ifdef	AF_UNIX
		case AF_UNIX:
			/* OK to have no pathname bound to the socket: happens on
			 * every connect() unless client calls bind() first. */
			if (addr->addr.un_path[0] == 0)
				strcpy(str, "(no pathname bound)");
			else
				snprintf(str, len, "%s", addr->addr.un_path);
			return(str);
#endif
			
#ifdef	HAVE_SOCKADDR_DL_STRUCT
		case AF_LINK:
			if (addr->addr.dl_nlen > 0)
				snprintf(str, len, "%*s",
						addr->addr.dl_nlen, &addr->addr.dl_data[0]);
			else
				snprintf(str, len, "AF_LINK, index=%d", addr->addr.dl_index);
			return(str);
#endif
#endif
		default:
			snprintf(str, len, "addr_ntop: unknown AF_xxx: %d", addr->family);
			return(str);
	}
	return (NULL);
}

