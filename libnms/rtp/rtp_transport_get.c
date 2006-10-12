/* * 
 *  $Id:rtp_transport_get.c 267 2006-01-12 17:19:45Z shawill $
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

#include <errno.h>

#include <nemesi/rtp.h>
#include <nemesi/utils.h>

int rtp_transport_get(rtp_session * rtp_sess, int par, void *value, uint32 len)
{
	int ret = RTP_TRANSPORT_NOTSET;
	// switch here for parameters that do NOT need value
	if (!value)
		return RTP_TRANSPORT_ERR;
	// switch here for parameters that need value
	switch (par) {
	case RTP_TRANSPORT_SPEC:
		strncpy(value, rtp_sess->transport.spec, len);
		((char *) value)[len - 1] = '\0';
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_DELIVERY:
		*(enum deliveries *) value = rtp_sess->transport.mode;
		break;
	case RTP_TRANSPORT_SRCADDR:
		memcpy((nms_addr *) value, &rtp_sess->transport.u.udp.srcaddr,
		       min(sizeof(nms_addr), len));
		if (len < sizeof(nms_addr))
			errno = ENOSPC;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_SRCADDRSTR:
		if (addr_ntop
		    (&rtp_sess->transport.u.udp.srcaddr, (char *) value, len))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_DSTADDR:
		memcpy((nms_addr *) value, &rtp_sess->transport.u.udp.dstaddr,
		       min(sizeof(nms_addr), len));
		if (len < sizeof(nms_addr))
			errno = ENOSPC;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_DSTADDRSTR:
		if (addr_ntop
		    (&rtp_sess->transport.u.udp.dstaddr, (char *) value, len))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_LAYERS:
		if ((*(int *) value = rtp_sess->transport.u.udp.layers))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_MODE:
		*(enum modes *) value = rtp_sess->transport.mode;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_APPEND:
		if ((*(int *) value = rtp_sess->transport.append))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_TTL:
		if ((*(int *) value = rtp_sess->transport.u.udp.ttl))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_MCSRTP:
		if ((*(in_port_t *) value = rtp_sess->transport.u.udp.mcs_ports[0]))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_MCSRTCP:
		if ((*(in_port_t *) value = rtp_sess->transport.u.udp.mcs_ports[1]))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_MCSPORTS:
		if ((((in_port_t *) value)[0] =
		     rtp_sess->transport.u.udp.mcs_ports[0])
		    && (((in_port_t *) value)[1] =
			rtp_sess->transport.u.udp.mcs_ports[1]))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_CLIRTP:
		if ((*(in_port_t *) value = rtp_sess->transport.u.udp.cli_ports[0]))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_CLIRTCP:
		if ((*(in_port_t *) value = rtp_sess->transport.u.udp.cli_ports[1]))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_CLIPORTS:
		if ((((in_port_t *) value)[0] =
		     rtp_sess->transport.u.udp.cli_ports[0])
		    && (((in_port_t *) value)[1] =
			rtp_sess->transport.u.udp.cli_ports[1]))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_SRVRTP:
		if ((*(in_port_t *) value = rtp_sess->transport.u.udp.srv_ports[0]))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_SRVRTCP:
		if ((*(in_port_t *) value = rtp_sess->transport.u.udp.srv_ports[1]))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_SRVPORTS:
		if ((((in_port_t *) value)[0] =
		     rtp_sess->transport.u.udp.srv_ports[0])
		    && (((in_port_t *) value)[1] =
			rtp_sess->transport.u.udp.srv_ports[1]))
			ret = RTP_TRANSPORT_SET;
		break;
	
	case RTP_TRANSPORT_ILVDRTP:
		*(uint8 *) value = rtp_sess->transport.u.tcp.RTP;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_ILVDRTCP:
		*(uint8 *) value = rtp_sess->transport.u.tcp.RTCP;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_INTERLEAVED:
		((uint8 *)value)[0] = rtp_sess->transport.u.tcp.RTP;
		((uint8 *)value)[1] = rtp_sess->transport.u.tcp.RTCP;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_STREAMRTP:
		if ((*(uint16 *) value = rtp_sess->transport.u.sctp.RTP))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_STREAMRTCP:
		if ((*(uint16 *) value = rtp_sess->transport.u.sctp.RTCP))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_STREAMS:
		if ( ( ((uint16 *)value)[0] = rtp_sess->transport.u.sctp.RTP) &&
		   ( ((uint16 *)value)[1] = rtp_sess->transport.u.sctp.RTCP) )
			ret = RTP_TRANSPORT_SET;
			break;
	case RTP_TRANSPORT_SSRC:
		if ((*(uint32 *) value = rtp_sess->transport.ssrc))
			ret = RTP_TRANSPORT_SET;
		break;
	default:
		break;
	}

	return ret;
}
