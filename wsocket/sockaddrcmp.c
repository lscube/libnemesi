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

/*
 * The function that compares two sockaddr structures.
 * \param addr1 first sockaddr struct
 * \param addr1_len length of first sockaddr struct
 * \param addr2 second sockaddr struct
 * \param addr1_len length of second sockaddr struct
 * \return 0 if the two structires are egual, otherwise an error reflecting the
 * first difference encountered. 
 */
int sockaddrcmp(struct sockaddr *addr1, socklen_t addr1_len, struct sockaddr *addr2, socklen_t addr2_len)
{
	if (addr1_len != addr2_len)
		return WSOCK_CMP_ERRSIZE;
	if (addr1->sa_family != addr1->sa_family)
		return WSOCK_CMP_ERRFAMILY;
	if (sock_cmp_addr(addr1, addr2 /*, addr1_len */))
		return WSOCK_CMP_ERRADDR;
	if (sock_cmp_port(addr1, addr2 /*, addr1_len */))
		return WSOCK_CMP_ERRPORT;

	return 0;
}

