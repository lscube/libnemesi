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

/** @file rtcp_recv.c
 * This file contains the functions that perform RTCP packets
 * reception and handling.
 */

#include <errno.h>
#include "rtcp.h"

/**
 * Checks if an RTCP packet has a valid header
 * @param pkt The packet to check
 * @param len The Length of the packet
 * @return 0 if the header if fine, 1 otherwise
 */
static int rtcp_hdr_val_chk(rtcp_pkt * pkt, int len)
{
    rtcp_pkt *end;

    if (len < (int) sizeof(rtcp_common_t)) {
        nms_printf(NMSML_ERR, "RTCP packet too small!!! (%d)\n", len);
        return 1;
    }

    if (len / 4 > (ntohs((pkt->common).len) + 1)) {
        /* This is a fu**ing compound pkt */
        nms_printf(NMSML_DBG2,
               "RTCP Compound packet arrived (total len=%d)\n",
               len);
        if ((*(uint16_t *) pkt & RTCP_VALID_MASK) != RTCP_VALID_VALUE) {
            nms_printf(NMSML_WARN,
                   "RTCP Header not valid: first pkt of Compound is not a SR (or RR)!\n"
                   BLANK_LINE);
            return 1;
        }
        end = (rtcp_pkt *) ((uint32_t *) pkt + len / 4);
        do
            pkt =
                (rtcp_pkt *) ((uint32_t *) pkt +
                      ntohs((pkt->common).len) + 1);
        while ((pkt < end) && ((pkt->common).ver == 2));

        if (pkt != end) {
            nms_printf(NMSML_WARN,
                   "RTCP Header not valid: mismatching lenght (%d)!\n"
                   BLANK_LINE, len);
            return 1;
        }
    } else {
        nms_printf(NMSML_DBG2, "RTCP packet arrived (total len=%d)\n",
               len);
        if ((pkt->common).ver != RTP_VERSION) {
            nms_printf(NMSML_WARN,
                   "RTCP Header not valid: mismatching RTP version number!\n"
                   BLANK_LINE);
            return 1;
        }
        if (!(((pkt->common).pt >= 200) && ((pkt->common).pt <= 204))) {
            nms_printf(NMSML_WARN,
                   "RTCP Header not valid: mismatching payload type!\n"
                   BLANK_LINE);
            return 1;
        }
        if (((pkt->common).pad)
            && (*(((uint8_t *) pkt) + len - 1) > (pkt->common).len * 4)) {
            nms_printf(NMSML_WARN,
                   "RTCP Header not valid: mismatching lenght!\n"
                   BLANK_LINE);
            return 1;
        }
    }

    return 0;
}

/**
 * Actually receives an RTCP packet for the given RTP Session
 * @param rtp_sess The Session for which to receive the packet
 * @return 0 if everything was ok, 1 if the packet was malformed
 */
int rtcp_recv(rtp_session * rtp_sess)
{
    uint8_t buffer[1024];
    rtp_ssrc *stm_src;

    struct sockaddr_storage serveraddr;
    nms_sockaddr server =
        { (struct sockaddr *) &serveraddr, sizeof(serveraddr) };

    rtcp_pkt *pkt;
    int ret, n;

    memset(buffer, 0, 1024);

    if ((n =
         recvfrom(rtp_sess->transport.RTCP.sock.fd, buffer, 1024, 0, server.addr,
              &server.addr_len)) == -1) {
        switch (errno) {
        case EBADF:
            nms_printf(NMSML_ERR,
                   "RTCP recvfrom: invalid descriptor\n");
            break;
#ifndef WIN32
        case ENOTSOCK:
            nms_printf(NMSML_ERR, "RTCP recvfrom: not a socket\n");
            break;
#endif
        case EINTR:
            nms_printf(NMSML_ERR,
                   "RTCP recvfrom: The receive was interrupted by delivery of a signal\n");
            break;
        case EFAULT:
            nms_printf(NMSML_ERR,
                   "RTCP recvfrom: The buffer points outside userspace\n");
            break;
        case EINVAL:
            nms_printf(NMSML_ERR,
                   "RTCP recvfrom: Invalid argument passed.\n");
            break;
        default:
            nms_printf(NMSML_ERR, "in RTCP recvfrom\n");
            break;
        }
        return 1;
    }

    pkt = (rtcp_pkt *) buffer;

    if (rtcp_hdr_val_chk(pkt, n)) {
        nms_printf(NMSML_WARN,
               "RTCP Header Validity Check failed!" BLANK_LINE);
        return 1;
    }

    switch (rtp_ssrc_check
        (rtp_sess, ntohl((pkt->r).sr.ssrc), &stm_src, &server, RTCP)) {
    case SSRC_NEW:
        if (pkt->common.pt == RTCP_SR)
            rtp_sess->sess_stats.senders++;
        rtp_sess->sess_stats.members++;
    case SSRC_RTCPNEW:
        break;
    case -1:
        return 1;
        break;
    default:
        break;
    }

    if ((ret = rtcp_parse_pkt(stm_src, pkt, n)) != 0)
        return ret;
    else
        rtp_sess->sess_stats.avg_rtcp_size =
            n / 16. + rtp_sess->sess_stats.avg_rtcp_size * 15. / 16.;

    return 0;
}

/**
 * Parses a given packet and calls the correct handling function
 * @param stm_src The SSRC for which the packet has arrived
 * @param pkt The Received packet
 * @param len The length of the packet
 * @return 0 or 1 if the packet if of an unknown type
 */
int rtcp_parse_pkt(rtp_ssrc * stm_src, rtcp_pkt * pkt, int len)
{
    rtcp_pkt *end;
    end = (rtcp_pkt *) ((uint32_t *) pkt + len / 4);

    while (pkt < end) {
        switch ((pkt->common).pt) {
        case RTCP_SR:
            rtcp_parse_sr(stm_src, pkt);
            break;
        case RTCP_SDES:
            if (rtcp_parse_sdes(stm_src, pkt))
                return -1;
            break;
        case RTCP_RR:
            rtcp_parse_rr(pkt);
            break;
        case RTCP_BYE:
            rtcp_parse_bye(stm_src, pkt);
            break;
        case RTCP_APP:
            rtcp_parse_app(pkt);
            break;
        default:
            nms_printf(NMSML_WARN, "Received unknown RTCP pkt\n");
            return 1;
        }
        pkt =
            (rtcp_pkt *) ((uint32_t *) pkt + ntohs((pkt->common).len) +
                  1);
    }
    return 0;
}
