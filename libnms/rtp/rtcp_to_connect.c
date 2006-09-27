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

#include <nemesi/rtp.h>

/*
 * Connect local client to oremote rtcp input port in order to send our rtcp statistics.
 *
 * \param stm_src the stream source whose statistics are.
 * \param remoteaddr sockaddr of remote address.
 * \param port remote rtcp port.
 * \return 0 on OK, 1 if connection went wrong, -1 on internal fatal error.
 * */
// int rtcp_to_connect(rtp_ssrc *stm_src, nms_sockaddr *remoteaddr, uint16 port)
int rtcp_to_connect(rtp_ssrc * stm_src, nms_addr * remoteaddr, in_port_t port)
{
	char addr[128];		/* Unix domain is largest */
	char port_str[256];
	struct sockaddr_storage rtcp_to_addr_s;
	nms_sockaddr rtcp_to_addr =
	    { (struct sockaddr *) &rtcp_to_addr_s, sizeof(rtcp_to_addr_s) };

	if (port > 0)
		// sprintf(port_str,"%d", ntohs(port));
		sprintf(port_str, "%d", port);
	else
		return nms_printf(NMSML_ERR,
				  "RTCP: Cannot connect to port (%d)\n", port);

	if (!addr_ntop(remoteaddr, addr, sizeof(addr))) {
		nms_printf(NMSML_WARN,
			   "RTP: Cannot get address from source\n");
		stm_src->rtcptofd = -2;
		return 1;
	} else
		nms_printf(NMSML_DBG2, "RTCP to host=%s\n", addr);

	if (server_connect(addr, port_str, &(stm_src->rtcptofd), UDP)) {
		nms_printf(NMSML_WARN,
			   "Cannot connect to remote RTCP destination %s:%s\n",
			   addr, port_str);
		stm_src->rtcptofd = -2;
	}
	getsockname(stm_src->rtcptofd, rtcp_to_addr.addr,
		    &rtcp_to_addr.addr_len);
	sockaddrdup(&stm_src->rtcp_to, &rtcp_to_addr);

	return 0;
}
