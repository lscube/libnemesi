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
inline char *rtp_get_spec(rtp_session * rtp_sess)
{
	return rtp_sess->transport.spec;
}

inline enum deliveries rtp_get_delivery(rtp_session * rtp_sess)
{
	return rtp_sess->transport.delivery;
}

inline int rtp_get_srcaddrstr(rtp_session * rtp_sess, char *addrstr,
			      uint32 strlen)
{
	return rtp_transport_get(rtp_sess, RTP_TRANSPORT_SRCADDRSTR, addrstr,
				 strlen);
}

inline nms_addr *rtp_get_srcaddr(rtp_session * rtp_sess)
{
	return &rtp_sess->transport.RTP.u.udp.srcaddr;
}

inline int rtp_get_dstaddrstr(rtp_session * rtp_sess, char *addrstr,
			      uint32 strlen)
{
	return rtp_transport_get(rtp_sess, RTP_TRANSPORT_DSTADDRSTR, addrstr,
				 strlen);
}

inline nms_addr *rtp_get_dstaddr(rtp_session * rtp_sess)
{
	return &rtp_sess->transport.RTP.u.udp.dstaddr;
}

inline enum modes rtp_get_mode(rtp_session * rtp_sess)
{
	return rtp_sess->transport.mode;
}

inline int rtp_get_layers(rtp_session * rtp_sess)
{
	return rtp_sess->transport.layers;
}

inline int rtp_get_append(rtp_session * rtp_sess)
{
	return rtp_sess->transport.append;
}

inline int rtp_get_ttl(rtp_session * rtp_sess)
{
	return rtp_sess->transport.ttl;
}

inline in_port_t rtp_get_mcsrtpport(rtp_session * rtp_sess)
{
	return rtp_sess->transport.RTP.multicast_port;
}

inline in_port_t rtp_get_mcsrtcpport(rtp_session * rtp_sess)
{
	return rtp_sess->transport.RTCP.multicast_port;
}

inline int rtp_get_mcsports(rtp_session * rtp_sess, in_port_t ports[2])
{
	return rtp_transport_get(rtp_sess, RTP_TRANSPORT_MCSPORTS, ports,
				 sizeof(ports));
}

inline in_port_t rtp_get_srvrtpport(rtp_session * rtp_sess)
{
	return rtp_sess->transport.RTP.remote_port;
}

inline in_port_t rtp_get_srvrtcpport(rtp_session * rtp_sess)
{
	return rtp_sess->transport.RTCP.remote_port;
}

inline int rtp_get_srvports(rtp_session * rtp_sess, in_port_t ports[2])
{
	return rtp_transport_get(rtp_sess, RTP_TRANSPORT_SRVPORTS, ports,
				 sizeof(ports));
}

inline in_port_t rtp_get_clirtpport(rtp_session * rtp_sess)
{
	return rtp_sess->transport.RTP.local_port;
}

inline in_port_t rtp_get_clirtcpport(rtp_session * rtp_sess)
{
	return rtp_sess->transport.RTCP.local_port;
}

inline int rtp_get_cliports(rtp_session * rtp_sess, in_port_t ports[2])
{
	return rtp_transport_get(rtp_sess, RTP_TRANSPORT_CLIPORTS, ports,
				 sizeof(ports));
}

inline uint8 rtp_get_ilvdrtp(rtp_session * rtp_sess)
{
	return rtp_sess->transport.RTP.u.tcp.ilvd;
}

inline uint8 rtp_get_ilvdrtcp(rtp_session * rtp_sess)
{
	return rtp_sess->transport.RTCP.u.tcp.ilvd;
}

inline int rtp_get_interleaved(rtp_session * rtp_sess, uint8 ilvds[2])
{
	return rtp_transport_get(rtp_sess, RTP_TRANSPORT_INTERLEAVED, ilvds,
				 sizeof(ilvds));
}

inline uint16 rtp_get_rtpstream(rtp_session * rtp_sess)
{
	return rtp_sess->transport.RTP.u.sctp.stream;
}

inline uint16 rtp_get_rtcpstream(rtp_session * rtp_sess)
{
	return rtp_sess->transport.RTCP.u.sctp.stream;
}

inline int rtp_get_streams(rtp_session * rtp_sess, uint16 streams[2])
{
	return rtp_transport_get(rtp_sess, RTP_TRANSPORT_STREAMS, streams,
				 sizeof(streams));
}

inline uint32 rtp_get_ssrc(rtp_session * rtp_sess)
{
	return rtp_sess->transport.ssrc;
}

// set wrappers
inline int rtp_set_delivery(rtp_session * rtp_sess, enum deliveries delivery)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_DELIVERY, &delivery);
}

inline int rtp_set_srcaddrstr(rtp_session * rtp_sess, char *address)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_SRCADDRSTR, address);
}

inline int rtp_set_srcaddr(rtp_session * rtp_sess, nms_addr * address)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_SRCADDR, address);
}

inline int rtp_set_dstaddrstr(rtp_session * rtp_sess, char *address)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_DSTADDRSTR, address);
}

inline int rtp_set_dstaddr(rtp_session * rtp_sess, nms_addr * address)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_DSTADDR, address);
}

inline int rtp_set_layers(rtp_session * rtp_sess, int layers)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_LAYERS, &layers);
}

inline int rtp_set_mode(rtp_session * rtp_sess, enum modes mode)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_MODE, &mode);
}

inline int rtp_set_append(rtp_session * rtp_sess, int append)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_APPEND, &append);
}

inline int rtp_set_ttl(rtp_session * rtp_sess, int ttl)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_TTL, &ttl);
}

inline int rtp_set_mcsports(rtp_session * rtp_sess, in_port_t ports[2])
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_MCSPORTS, ports);
}

inline int rtp_set_mcsrtpport(rtp_session * rtp_sess, in_port_t port)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_MCSRTP, &port);
}

inline int rtp_set_mcsrtcpport(rtp_session * rtp_sess, in_port_t port)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_MCSRTCP, &port);
}

inline int rtp_set_srvports(rtp_session * rtp_sess, in_port_t ports[2])
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_SRVPORTS, ports);
}

inline int rtp_set_srvrtpport(rtp_session * rtp_sess, in_port_t port)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_SRVRTP, &port);
}

inline int rtp_set_srvrtcpport(rtp_session * rtp_sess, in_port_t port)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_SRVRTCP, &port);
}

inline int rtp_set_cliports(rtp_session * rtp_sess, in_port_t ports[2])
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_CLIPORTS, ports);
}

inline int rtp_set_clirtpport(rtp_session * rtp_sess, in_port_t port)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_CLIRTP, &port);
}

inline int rtp_set_clirtcpport(rtp_session * rtp_sess, in_port_t port)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_CLIRTCP, &port);
}

inline int rtp_set_ilvdrtp(rtp_session * rtp_sess, uint8 ilvd)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_ILVDRTP, &ilvd);
}

inline int rtp_set_ilvdrtcp(rtp_session * rtp_sess, uint8 ilvd)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_ILVDRTCP, &ilvd);
}

inline int rtp_set_interleaved(rtp_session * rtp_sess, uint8 ilvds[2])
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_INTERLEAVED, ilvds);
}

inline int rtp_set_rtpstream(rtp_session * rtp_sess, uint16 stream)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_STREAMRTP, &stream);
}

inline int rtp_set_rtcpstream(rtp_session * rtp_sess, uint16 stream)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_STREAMRTCP, &stream);
}

inline int rtp_set_streams(rtp_session * rtp_sess, uint16 streams[2])
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_STREAMS, streams);
}

inline int rtp_set_ssrc(rtp_session * rtp_sess, uint32 ssrc)
{
	return rtp_transport_set(rtp_sess, RTP_TRANSPORT_SSRC, &ssrc);
}
