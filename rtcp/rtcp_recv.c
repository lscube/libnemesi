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

#include <errno.h>

#include <nemesi/rtcp.h>

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
        case ENOTSOCK:
            nms_printf(NMSML_ERR, "RTCP recvfrom: not a socket\n");
            break;
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
