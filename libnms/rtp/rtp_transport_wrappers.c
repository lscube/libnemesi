/* * 
 *  $Id:rtp_transport_wrappers.c 267 2006-01-12 17:19:45Z shawill $
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

// get wrappers
inline char *rtp_transport_get_spec(struct rtp_session *rtp_sess)
{
	return rtp_sess->transport.spec;
}

inline enum deliveries rtp_transport_get_delivery(struct rtp_session *rtp_sess)
{
	return rtp_sess->transport.delivery;
}

inline int rtp_trasnport_get_srcaddrstr(struct rtp_session *rtp_sess, char *addrstr, uint32 strlen)
{
	return rtp_transport_get(rtp_sess, RTP_TRANSPORT_SRCADDRSTR, addrstr, strlen);
}

inline nms_addr *rtp_transport_get_srcaddr(struct rtp_session *rtp_sess)
{
	return &rtp_sess->transport.srcaddr;
}

inline int rtp_trasnport_get_dstaddrstr(struct rtp_session *rtp_sess, char *addrstr, uint32 strlen)
{
	return rtp_transport_get(rtp_sess, RTP_TRANSPORT_DSTADDRSTR, addrstr, strlen);
}

inline nms_addr *rtp_transport_get_dstaddr(struct rtp_session *rtp_sess)
{
	return &rtp_sess->transport.dstaddr;
}

inline enum modes rtp_transport_get_mode(struct rtp_session *rtp_sess)
{
	return rtp_sess->transport.mode;
}

inline int rtp_transport_get_layers(struct rtp_session *rtp_sess)
{
	return rtp_sess->transport.layers;
}

inline int rtp_transport_get_append(struct rtp_session *rtp_sess)
{
	return rtp_sess->transport.append;
}

inline int rtp_transport_get_ttl(struct rtp_session *rtp_sess)
{
	return rtp_sess->transport.ttl;
}

inline in_port_t rtp_transport_get_mcsrtpport(struct rtp_session *rtp_sess)
{
	return rtp_sess->transport.mcs_ports[0];
}

inline in_port_t rtp_transport_get_mcsrtcpport(struct rtp_session *rtp_sess)
{
	return rtp_sess->transport.mcs_ports[1];
}

inline int rtp_transport_get_mcsports(struct rtp_session *rtp_sess, in_port_t ports[2])
{
	return rtp_transport_get(rtp_sess, RTP_TRANSPORT_MCSPORTS, ports, sizeof(ports));
}

inline in_port_t rtp_transport_get_srvrtpport(struct rtp_session *rtp_sess)
{
	return rtp_sess->transport.srv_ports[0];
}

inline in_port_t rtp_transport_get_srvrtcpport(struct rtp_session *rtp_sess)
{
	return rtp_sess->transport.srv_ports[1];
}

inline int rtp_transport_get_srvports(struct rtp_session *rtp_sess, in_port_t ports[2])
{
	return rtp_transport_get(rtp_sess, RTP_TRANSPORT_SRVPORTS, ports, sizeof(ports));
}

inline in_port_t rtp_transport_get_clirtpport(struct rtp_session *rtp_sess)
{
	return rtp_sess->transport.cli_ports[0];
}

inline in_port_t rtp_transport_get_clirtcpport(struct rtp_session *rtp_sess)
{
	return rtp_sess->transport.cli_ports[1];
}

inline int rtp_transport_get_cliports(struct rtp_session *rtp_sess, in_port_t ports[2])
{
	return rtp_transport_get(rtp_sess, RTP_TRANSPORT_CLIPORTS, ports, sizeof(ports));
}

inline uint32 rtp_transport_get_ssrc(struct rtp_session *rtp_sess)
{
	return rtp_sess->transport.ssrc;
}

// set wrappers
inline int rtp_transport_set_delivery(struct rtp_session *rtp_sess, enum deliveries delivery)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_DELIVERY, &delivery);
}

inline int rtp_trasnport_set_srcaddrstr(struct rtp_session *rtp_sess, char *address)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_SRCADDRSTR, address);
}

inline int rtp_transport_set_srcaddr(struct rtp_session *rtp_sess, nms_addr *address)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_SRCADDR, address);
}

inline int rtp_trasnport_set_dstaddrstr(struct rtp_session *rtp_sess, char *address)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_DSTADDRSTR, address);
}

inline int rtp_transport_set_dstaddr(struct rtp_session *rtp_sess, nms_addr *address)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_DSTADDR, address);
}

inline int rtp_transport_set_layers(struct rtp_session *rtp_sess, int layers)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_LAYERS, &layers);
}

inline int rtp_transport_set_mode(struct rtp_session *rtp_sess, enum modes mode)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_MODE, &mode);
}

inline int rtp_transport_set_append(struct rtp_session *rtp_sess, int append)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_APPEND, &append);
}

inline int rtp_transport_set_ttl(struct rtp_session *rtp_sess, int ttl)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_TTL, &ttl);
}

inline int rtp_transport_set_mcsports(struct rtp_session *rtp_sess, in_port_t ports[2])
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_MCSPORTS, ports);
}

inline int rtp_transport_set_mcsrtpport(struct rtp_session *rtp_sess, in_port_t port)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_MCSRTP, &port);
}

inline int rtp_transport_set_mcsrtcpport(struct rtp_session *rtp_sess, in_port_t port)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_MCSRTCP, &port);
}

inline int rtp_transport_set_srvports(struct rtp_session *rtp_sess, in_port_t ports[2])
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_SRVPORTS, ports);
}

inline int rtp_transport_set_srvrtpport(struct rtp_session *rtp_sess, in_port_t port)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_SRVRTP, &port);
}

inline int rtp_transport_set_srvrtcpport(struct rtp_session *rtp_sess, in_port_t port)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_SRVRTCP, &port);
}

inline int rtp_transport_set_cliports(struct rtp_session *rtp_sess, in_port_t ports[2])
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_CLIPORTS, ports);
}

inline int rtp_transport_set_clirtpport(struct rtp_session *rtp_sess, in_port_t port)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_CLIRTP, &port);
}

inline int rtp_transport_set_clirtcpport(struct rtp_session *rtp_sess, in_port_t port)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_CLIRTCP, &port);
}

inline int rtp_transport_set_ssrc(struct rtp_session *rtp_sess, uint32 ssrc)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_SSRC, &ssrc);
}

