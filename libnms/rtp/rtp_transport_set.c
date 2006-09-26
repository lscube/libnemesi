/* * 
 *  $Id:rtp_transport_set.c 267 2006-01-12 17:19:45Z shawill $
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
#include <nemesi/utils.h>
#include <nemesi/comm.h>

static int convert_addr_str(const char *, nms_addr *);

int rtp_transport_set(rtp_session * rtp_sess, int par, void *value)
{
	int ret = RTP_TRANSPORT_NOTSET;
	// switch here for parameters that do NOT need value
	// for now nothing

	if ((ret != RTP_TRANSPORT_SET) && !value)
		return RTP_TRANSPORT_NOTSET;

	// switch here for parameters that need value
	switch (par) {
	case RTP_TRANSPORT_SPEC:
		// could not set spec for outsid library for now.
		break;
	case RTP_TRANSPORT_DELIVERY:
		rtp_sess->transport.delivery = *(enum deliveries *) value;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_SRCADDR:
		memcpy(&rtp_sess->transport.srcaddr, (nms_addr *) value, sizeof(nms_addr));
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_SRCADDRSTR:
		if (!convert_addr_str((char *) value, &rtp_sess->transport.srcaddr))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_DSTADDR:
		memcpy(&rtp_sess->transport.dstaddr, (nms_addr *) value, sizeof(nms_addr));
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_DSTADDRSTR:
		if (!convert_addr_str((char *) value, &rtp_sess->transport.dstaddr))
			ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_LAYERS:
		rtp_sess->transport.layers = *(int *) value;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_MODE:
		rtp_sess->transport.mode = *(enum modes *) value;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_APPEND:
		rtp_sess->transport.append = *(int *) value;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_TTL:
		rtp_sess->transport.ttl = *(int *) value;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_MCSRTP:
		rtp_sess->transport.mcs_ports[0] = *(in_port_t *) value;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_MCSRTCP:
		rtp_sess->transport.mcs_ports[1] = *(in_port_t *) value;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_MCSPORTS:
		rtp_sess->transport.mcs_ports[0] = ((in_port_t *) value)[0];
		rtp_sess->transport.mcs_ports[1] = ((in_port_t *) value)[0];
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_CLIRTP:
		rtp_sess->transport.cli_ports[0] = *(in_port_t *) value;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_CLIRTCP:
		rtp_sess->transport.cli_ports[1] = *(in_port_t *) value;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_CLIPORTS:
		rtp_sess->transport.cli_ports[0] = ((in_port_t *) value)[0];
		rtp_sess->transport.cli_ports[1] = ((in_port_t *) value)[1];
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_SRVRTP:
		rtp_sess->transport.srv_ports[0] = *(in_port_t *) value;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_SRVRTCP:
		rtp_sess->transport.srv_ports[1] = *(in_port_t *) value;
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_SRVPORTS:
		rtp_sess->transport.srv_ports[0] = ((in_port_t *) value)[0];
		rtp_sess->transport.srv_ports[1] = ((in_port_t *) value)[1];
		ret = RTP_TRANSPORT_SET;
		break;
	case RTP_TRANSPORT_SSRC:
		rtp_sess->transport.ssrc = *(uint32 *) value;
		ret = RTP_TRANSPORT_SET;
		break;
	default:
		break;
	}

	return ret;
}

/* static function that converts strings address in in*_addr struct
 * \return 0 on OK, 1 if string is not valid address.
 * */
static int convert_addr_str(const char *address, nms_addr * retaddr)
{
	int res;

	retaddr->family = AF_UNSPEC;

	if ((res = inet_pton(AF_INET, address, &retaddr->addr.in)) > 0) {
		nms_printf(NMSML_DBG2, "IPv4 address converted (%s->%u)\n", address, retaddr->addr.in);
		retaddr->family = AF_INET;
	}
#ifdef IPV6
	else if ((res = inet_pton(AF_INET6, address, &retaddr->addr.in6)) > 0) {
		nms_printf(NMSML_DBG2, "IPv6 address converted (%s->%u)\n", address, retaddr->addr.in6);
		retaddr->family = AF_INET6;
	}
#endif
	else
		nms_printf(NMSML_ERR, "no address converted\n");

	return res ? 0 : 1;
}

#if 0
/* static function that checks if string is a valid IPv4 or IPv6 address
 * \return 0 on OK, 1 if string is not valid address.
 * */
static int check_addr_str(const char *address)
{
	struct in_addr in_addr;
#ifdef IPV6
	struct in6_addr in6_addr;
#endif
	int res;

	res = inet_pton(AF_INET, (char *) value, &in_addr);
#ifdef IPV6
	if (!res)
		res = inet_pton(AF_INET6, (char *) value, &in6_addr);
#endif

	return res ? 0 : 1;
}
#endif
