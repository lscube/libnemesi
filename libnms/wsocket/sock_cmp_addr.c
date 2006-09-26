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
 *  this piece of code is taken from Richard Stevens source code of
 *    UNIX Network Programming, Volume 1, Second Edition: Networking APIs:
 *      Sockets and XTI, Prentice Hall, 1998, ISBN 0-13-490012-X.
 * */

#include <nemesi/wsocket.h>

#ifdef	HAVE_SOCKADDR_DL_STRUCT
# include	<net/if_dl.h>
#endif
#ifdef	AF_UNIX
#include <sys/un.h>
#endif

int sock_cmp_addr(const struct sockaddr *sa1, const struct sockaddr *sa2 /*, socklen_t salen */ )
{
	if (sa1->sa_family != sa2->sa_family)
		return -1;

	switch (sa1->sa_family) {
	case AF_INET:
		return (memcmp
			(&((struct sockaddr_in *) sa1)->sin_addr, &((struct sockaddr_in *) sa2)->sin_addr,
			 sizeof(struct in_addr)));
		break;

#ifdef	IPV6
	case AF_INET6:
		return (memcmp
			(&((struct sockaddr_in6 *) sa1)->sin6_addr, &((struct sockaddr_in6 *) sa2)->sin6_addr,
			 sizeof(struct in6_addr)));
		break;
#endif

#ifdef	AF_UNIX
	case AF_UNIX:
		return (strcmp(((struct sockaddr_un *) sa1)->sun_path, ((struct sockaddr_un *) sa2)->sun_path));
		break;
#endif

#ifdef	HAVE_SOCKADDR_DL_STRUCT
	case AF_LINK:
		return -1;	/* no idea what to compare here ? */
		break;
#endif
	default:
		return -1;
		break;
	}
	// return -1;
}
