/* * 
 * This file is part of libnemesi
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * libnemesi is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libnemesi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libnemesi; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *  
 * */

#include <nemesi/rtp.h>
#include <nemesi/bufferpool.h>

#define RET_ERR(err_level, ...)    do { \
                    nms_printf(err_level, __VA_ARGS__ ); \
                    free(rtp_sess); \
                    return NULL; \
                } while (0)

/**
 * Initializes a new rtp session with the specified endpoints
 * @param local local endpoint
 * @param peer remote endpoint
 * @return a new rtp session, NULL as failure
 */
rtp_session *rtp_session_init(nms_sockaddr * local, nms_sockaddr * peer)
{
    rtp_session *rtp_sess;
    nms_addr nms_address;

    if ((rtp_sess =
         (rtp_session *) calloc(1, sizeof(rtp_session))) == NULL) {
        nms_printf(NMSML_FATAL, "Cannot allocate memory!\n");
        return NULL;
    }

    rtp_sess->bp = (buffer_pool*)calloc(1, sizeof(buffer_pool));

    rtp_sess->transport.RTP.sock.fd = -1;
    rtp_sess->transport.RTCP.sock.fd = -1;
    rtp_sess->local_ssrc = random32(0);
    if (pthread_mutex_init(&rtp_sess->syn, NULL))
        RET_ERR(NMSML_FATAL, "Cannot init mutex!\n");
    if (!(rtp_sess->transport.spec = strdup(RTP_AVP_UDP)))
        RET_ERR(NMSML_FATAL, "Cannot duplicate string!\n");
    rtp_sess->transport.delivery = unicast;

    // --- remote address
    if (sockaddr_get_nms_addr(peer->addr, &nms_address))
        RET_ERR(NMSML_ERR, "remote address not valid\n");
    if (rtp_transport_set(rtp_sess, RTP_TRANSPORT_SRCADDR, &nms_address))
        RET_ERR(NMSML_ERR,
            "Could not set srcaddr in transport string\n");
    switch (nms_address.family) {
    case AF_INET:
        nms_printf(NMSML_DBG1, "IPv4 address\n");
        break;
    case AF_INET6:
        nms_printf(NMSML_DBG1, "IPv6 address\n");
        break;
    }

    // --- local address
    if (sockaddr_get_nms_addr(local->addr, &nms_address))
        RET_ERR(NMSML_ERR, "local address not valid\n");
    if (rtp_transport_set(rtp_sess, RTP_TRANSPORT_DSTADDR, &nms_address))
        RET_ERR(NMSML_ERR,
            "Could not set dstaddr in transport string\n");
    switch (nms_address.family) {
    case AF_INET:
        nms_printf(NMSML_DBG1, "IPv4 local address\n");
        break;
    case AF_INET6:
        nms_printf(NMSML_DBG1, "IPv6 local address\n");
        break;
    }
    // ---
    rtp_sess->transport.mode = play;
    rtp_sess->transport.ssrc = rtp_sess->local_ssrc;

    rtp_sess->sess_stats.pmembers = 1;
    rtp_sess->sess_stats.members = 1;
    rtp_sess->sess_stats.initial = 1;
    rtp_sess->sess_stats.avg_rtcp_size = 200;    /* RR + SDES ~= 200 Bytes */
    rtp_sess->sess_stats.rtcp_bw = BANDWIDTH;

    // RP Payload types definitions:
    rtpptdefs_new(rtp_sess->ptdefs);
    rtp_parsers_new(rtp_sess->parsers, rtp_sess->parsers_inits);

    return rtp_sess;
}
