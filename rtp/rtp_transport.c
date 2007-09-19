/* * 
 * This file is part of NetEmbryo
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * NetEmbryo is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * NetEmbryo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with NetEmbryo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *  
 * */

#include <nemesi/utils.h>
#include <nemesi/rtp.h>
#include <nemesi/comm.h>
#include <errno.h>

/**
 * static function that converts strings address in in*_addr struct
 * @return 0 on OK, 1 if string is not valid address.
 */
static int convert_addr_str(const char *address, nms_addr * retaddr)
{
    int res;

    retaddr->family = AF_UNSPEC;

    if ((res = inet_pton(AF_INET, address, &retaddr->addr.in)) > 0) {
        nms_printf(NMSML_DBG2, "IPv4 address converted (%s->%u)\n",
               address, retaddr->addr.in);
        retaddr->family = AF_INET;
    }
#ifdef IPV6
    else if ((res = inet_pton(AF_INET6, address, &retaddr->addr.in6)) > 0) {
        nms_printf(NMSML_DBG2, "IPv6 address converted (%s->%u)\n",
               address, retaddr->addr.in6);
        retaddr->family = AF_INET6;
    }
#endif
    else
        nms_printf(NMSML_ERR, "no address converted\n");

    return res ? 0 : 1;
}

#if 0
/** 
 * static function that checks if string is a valid IPv4 or IPv6 address
 * @return 0 on OK, 1 if string is not valid address.
 */
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

/**
 * Sets options for the transport of the given RTP session.
 * @param rtp_sess The session for which to modify the transport options
 * @param par The ID of the option to modify. Can be any of the RTP_TRANSPORT ids
 *            defined in rtp.h
 * @param value A pointer to the value to set for the given ID.
 */
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
    case RTP_TRANSPORT_SOCKTYPE:
        rtp_sess->transport.type = *(sock_type *) value;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_DELIVERY:
        rtp_sess->transport.delivery = *(enum deliveries *) value;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_SRCADDR:
        memcpy(&rtp_sess->transport.RTP.u.udp.srcaddr, (nms_addr *) value,
               sizeof(nms_addr));
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_SRCADDRSTR:
        if (!convert_addr_str
            ((char *) value, &rtp_sess->transport.RTP.u.udp.srcaddr))
            ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_DSTADDR:
        memcpy(&rtp_sess->transport.RTP.u.udp.dstaddr, (nms_addr *) value,
               sizeof(nms_addr));
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_DSTADDRSTR:
        if (!convert_addr_str
            ((char *) value, &rtp_sess->transport.RTP.u.udp.dstaddr))
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
        rtp_sess->transport.RTP.multicast_port = *(in_port_t *) value;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_MCSRTCP:
        rtp_sess->transport.RTCP.multicast_port = *(in_port_t *) value;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_MCSPORTS:
        rtp_sess->transport.RTP.multicast_port  = ((in_port_t *) value)[0];
        rtp_sess->transport.RTCP.multicast_port = ((in_port_t *) value)[1];
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_CLIRTP:
        rtp_sess->transport.RTP.sock.local_port = *(in_port_t *) value;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_CLIRTCP:
        rtp_sess->transport.RTCP.sock.local_port = *(in_port_t *) value;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_CLIPORTS:
        rtp_sess->transport.RTP.sock.local_port  = ((in_port_t *) value)[0];
        rtp_sess->transport.RTCP.sock.local_port = ((in_port_t *) value)[1];
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_SRVRTP:
        rtp_sess->transport.RTP.sock.remote_port = *(in_port_t *) value;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_SRVRTCP:
        rtp_sess->transport.RTCP.sock.remote_port = *(in_port_t *) value;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_SRVPORTS:
        rtp_sess->transport.RTP.sock.remote_port  = ((in_port_t *) value)[0];
        rtp_sess->transport.RTCP.sock.remote_port = ((in_port_t *) value)[1];
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_ILVDRTP:
        rtp_sess->transport.RTP.u.tcp.ilvd = *(uint8_t *) value;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_ILVDRTCP:
        rtp_sess->transport.RTCP.u.tcp.ilvd = *(uint8_t *) value;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_INTERLEAVED:
        rtp_sess->transport.RTP.u.tcp.ilvd  = ((uint8_t *) value)[0];
        rtp_sess->transport.RTCP.u.tcp.ilvd = ((uint8_t *) value)[1];
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_STREAMRTP:
        rtp_sess->transport.RTP.u.sctp.stream = *(uint16_t *) value;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_STREAMRTCP:
        rtp_sess->transport.RTCP.u.sctp.stream = *(uint16_t *) value;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_STREAMS:
        rtp_sess->transport.RTP.u.sctp.stream  = ((uint16_t *) value)[0];
        rtp_sess->transport.RTCP.u.sctp.stream = ((uint16_t *) value)[1];
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_SSRC:
        rtp_sess->transport.ssrc = *(uint32_t *) value;
        ret = RTP_TRANSPORT_SET;
        break;
    default:
        break;
    }

    return ret;
}

/**
 * Gets the value of an option of the transport for the given RTP session.
 * @param rtp_sess The session for which to get the option value of the transport.
 * @param par The ID of the option for which to get the value.
 * @param value A pointer to the buffer where to save the retrieved value
 * @param len The size of the buffer where to save the retrieved value
 */
int rtp_transport_get(rtp_session * rtp_sess, int par, void *value, uint32_t len)
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
    case RTP_TRANSPORT_SOCKTYPE:
        *(sock_type *) value = rtp_sess->transport.type;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_DELIVERY:
        *(enum deliveries *) value = rtp_sess->transport.delivery;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_SRCADDR:
        memcpy((nms_addr *) value, &rtp_sess->transport.RTP.u.udp.srcaddr,
               min(sizeof(nms_addr), len));
        if (len < sizeof(nms_addr))
            errno = ENOSPC;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_SRCADDRSTR:
        if (nms_addr_ntop
            (&rtp_sess->transport.RTP.u.udp.srcaddr, (char *) value, len))
            ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_DSTADDR:
        memcpy((nms_addr *) value, &rtp_sess->transport.RTP.u.udp.dstaddr,
               min(sizeof(nms_addr), len));
        if (len < sizeof(nms_addr))
            errno = ENOSPC;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_DSTADDRSTR:
        if (nms_addr_ntop
            (&rtp_sess->transport.RTP.u.udp.dstaddr, (char *) value, len))
            ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_LAYERS:
        if ((*(int *) value = rtp_sess->transport.layers))
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
        if ((*(int *) value = rtp_sess->transport.ttl))
            ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_MCSRTP:
        if ((*(in_port_t *) value = rtp_sess->transport.RTP.multicast_port))
            ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_MCSRTCP:
        if ((*(in_port_t *) value = rtp_sess->transport.RTCP.multicast_port))
            ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_MCSPORTS:
        if ((((in_port_t *) value)[0] =
             rtp_sess->transport.RTP.multicast_port)
            && (((in_port_t *) value)[1] =
            rtp_sess->transport.RTCP.multicast_port))
            ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_CLIRTP:
        if ((*(in_port_t *) value = rtp_sess->transport.RTP.sock.local_port))
            ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_CLIRTCP:
        if ((*(in_port_t *) value = rtp_sess->transport.RTCP.sock.local_port))
            ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_CLIPORTS:
        if ((((in_port_t *) value)[0] =
             rtp_sess->transport.RTP.sock.local_port)
            && (((in_port_t *) value)[1] =
            rtp_sess->transport.RTCP.sock.local_port))
            ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_SRVRTP:
        if ((*(in_port_t *) value = rtp_sess->transport.RTP.sock.remote_port))
            ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_SRVRTCP:
        if ((*(in_port_t *) value = rtp_sess->transport.RTCP.sock.remote_port))
            ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_SRVPORTS:
        if ((((in_port_t *) value)[0] =
             rtp_sess->transport.RTP.sock.remote_port)
            && (((in_port_t *) value)[1] =
            rtp_sess->transport.RTCP.sock.remote_port))
            ret = RTP_TRANSPORT_SET;
        break;
    
    case RTP_TRANSPORT_ILVDRTP:
        *(uint8_t *) value = rtp_sess->transport.RTP.u.tcp.ilvd;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_ILVDRTCP:
        *(uint8_t *) value = rtp_sess->transport.RTCP.u.tcp.ilvd;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_INTERLEAVED:
        ((uint8_t *)value)[0] = rtp_sess->transport.RTP.u.tcp.ilvd;
        ((uint8_t *)value)[1] = rtp_sess->transport.RTCP.u.tcp.ilvd;
        ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_STREAMRTP:
        if ((*(uint16_t *) value = rtp_sess->transport.RTP.u.sctp.stream))
            ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_STREAMRTCP:
        if ((*(uint16_t *) value = rtp_sess->transport.RTCP.u.sctp.stream))
            ret = RTP_TRANSPORT_SET;
        break;
    case RTP_TRANSPORT_STREAMS:
        if ( ( ((uint16_t *)value)[0] = rtp_sess->transport.RTP.u.sctp.stream) &&
           ( ((uint16_t *)value)[1] = rtp_sess->transport.RTCP.u.sctp.stream) )
            ret = RTP_TRANSPORT_SET;
            break;
    case RTP_TRANSPORT_SSRC:
        if ((*(uint32_t *) value = rtp_sess->transport.ssrc))
            ret = RTP_TRANSPORT_SET;
        break;
    default:
        break;
    }

    return ret;
}

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
                  uint32_t strlen)
{
    return rtp_transport_get(rtp_sess, RTP_TRANSPORT_SRCADDRSTR, addrstr,
                 strlen);
}

inline nms_addr *rtp_get_srcaddr(rtp_session * rtp_sess)
{
    return &rtp_sess->transport.RTP.u.udp.srcaddr;
}

inline int rtp_get_dstaddrstr(rtp_session * rtp_sess, char *addrstr,
                  uint32_t strlen)
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
    return rtp_sess->transport.RTP.sock.remote_port;
}

inline in_port_t rtp_get_srvrtcpport(rtp_session * rtp_sess)
{
    return rtp_sess->transport.RTCP.sock.remote_port;
}

inline int rtp_get_srvports(rtp_session * rtp_sess, in_port_t ports[2])
{
    return rtp_transport_get(rtp_sess, RTP_TRANSPORT_SRVPORTS, ports,
                 sizeof(ports));
}

inline in_port_t rtp_get_clirtpport(rtp_session * rtp_sess)
{
    return rtp_sess->transport.RTP.sock.local_port;
}

inline in_port_t rtp_get_clirtcpport(rtp_session * rtp_sess)
{
    return rtp_sess->transport.RTCP.sock.local_port;
}

inline int rtp_get_cliports(rtp_session * rtp_sess, in_port_t ports[2])
{
    return rtp_transport_get(rtp_sess, RTP_TRANSPORT_CLIPORTS, ports,
                 sizeof(ports));
}

inline uint8_t rtp_get_ilvdrtp(rtp_session * rtp_sess)
{
    return rtp_sess->transport.RTP.u.tcp.ilvd;
}

inline uint8_t rtp_get_ilvdrtcp(rtp_session * rtp_sess)
{
    return rtp_sess->transport.RTCP.u.tcp.ilvd;
}

inline int rtp_get_interleaved(rtp_session * rtp_sess, uint8_t ilvds[2])
{
    return rtp_transport_get(rtp_sess, RTP_TRANSPORT_INTERLEAVED, ilvds,
                 sizeof(ilvds));
}

inline uint16_t rtp_get_rtpstream(rtp_session * rtp_sess)
{
    return rtp_sess->transport.RTP.u.sctp.stream;
}

inline uint16_t rtp_get_rtcpstream(rtp_session * rtp_sess)
{
    return rtp_sess->transport.RTCP.u.sctp.stream;
}

inline int rtp_get_streams(rtp_session * rtp_sess, uint16_t streams[2])
{
    return rtp_transport_get(rtp_sess, RTP_TRANSPORT_STREAMS, streams,
                 sizeof(streams));
}

inline uint32_t rtp_get_ssrc(rtp_session * rtp_sess)
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

inline int rtp_set_ilvdrtp(rtp_session * rtp_sess, uint8_t ilvd)
{
    return rtp_transport_set(rtp_sess, RTP_TRANSPORT_ILVDRTP, &ilvd);
}

inline int rtp_set_ilvdrtcp(rtp_session * rtp_sess, uint8_t ilvd)
{
    return rtp_transport_set(rtp_sess, RTP_TRANSPORT_ILVDRTCP, &ilvd);
}

inline int rtp_set_interleaved(rtp_session * rtp_sess, uint8_t ilvds[2])
{
    return rtp_transport_set(rtp_sess, RTP_TRANSPORT_INTERLEAVED, ilvds);
}

inline int rtp_set_rtpstream(rtp_session * rtp_sess, uint16_t stream)
{
    return rtp_transport_set(rtp_sess, RTP_TRANSPORT_STREAMRTP, &stream);
}

inline int rtp_set_rtcpstream(rtp_session * rtp_sess, uint16_t stream)
{
    return rtp_transport_set(rtp_sess, RTP_TRANSPORT_STREAMRTCP, &stream);
}

inline int rtp_set_streams(rtp_session * rtp_sess, uint16_t streams[2])
{
    return rtp_transport_set(rtp_sess, RTP_TRANSPORT_STREAMS, streams);
}

inline int rtp_set_ssrc(rtp_session * rtp_sess, uint32_t ssrc)
{
    return rtp_transport_set(rtp_sess, RTP_TRANSPORT_SSRC, &ssrc);
}
