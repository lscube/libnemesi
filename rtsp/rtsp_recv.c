/* * 
 *  $Id:rtsp_recv.c 267 2006-01-12 17:19:45Z shawill $
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *      
 *      Giampaolo "mancho" Mancini - manchoz@inwind.it
 *    Francesco "shawill" Varano - shawill@infinto.it
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

#include <nemesi/rtsp.h>

#ifndef BUFFERSIZE
#define BUFFERSIZE 163840
#endif
int rtsp_recv(rtsp_thread * rtsp_th)
{
    int n = -1, m = 0;
    char buffer[BUFFERSIZE];
    nms_rtsp_interleaved *p;
#ifdef HAVE_SCTP_NEMESI
    struct sctp_sndrcvinfo sinfo;
#endif

    memset(buffer, '\0', BUFFERSIZE);

    switch(rtsp_th->transport.type) {
    case TCP:
        n = nmst_read(&rtsp_th->transport, buffer, BUFFERSIZE, NULL);
        break;
#ifdef HAVE_SCTP_NEMESI
    case SCTP:
        memset(&sinfo, 0, sizeof(sinfo));
        n = nmst_read(&rtsp_th->transport, buffer, BUFFERSIZE, &sinfo);
        m = sinfo.sinfo_stream;
        break;
#endif
    default:
        break;
    }
    if (n == 0) {
        return 0;
    }
    if (n < 0) {
        nms_printf(NMSML_ERR, "Could not read from RTSP socket\n");
        return n;
    }
    if (rtsp_th->transport.type == TCP || (rtsp_th->transport.type == SCTP && m==0)) {
        if (((rtsp_th->in_buffer).size) == 0) {
            if (((rtsp_th->in_buffer).data = 
                 (char *) calloc(1, n + 1)) == NULL)
                return nms_printf(NMSML_FATAL,
                  "Cannot alloc memory space for received RTSP data\n");

            memcpy((rtsp_th->in_buffer).data, buffer, n);
        } else {
            if (((rtsp_th->in_buffer).data =
                 (char *) realloc((rtsp_th->in_buffer).data,
                n + (rtsp_th->in_buffer).size + 1)) ==
                NULL)
                return nms_printf(NMSML_FATAL,
                  "Cannot alloc memory space for received RTSP data\n");

            memcpy((rtsp_th->in_buffer).data + (rtsp_th->in_buffer).size, buffer, n);
    }
    (rtsp_th->in_buffer).size += n;
    (rtsp_th->in_buffer).data[(rtsp_th->in_buffer).size] = '\0';
    } else /* if (rtsp_th->transport.type == SCTP && m!=0) */ {
#ifdef HAVE_SCTP_NEMESI
        for (p = rtsp_th->interleaved; p && !((p->proto.sctp.rtp_st == m)
            || (p->proto.sctp.rtcp_st == m)); p = p->next);
        if (p) {
            if (p->proto.sctp.rtp_st == m) {
                nms_printf(NMSML_DBG2,
                       "Interleaved RTP data (%u bytes: channel %d -> sd %d)\n",
                       n, m, p->rtp_fd);
                send(p->rtp_fd, buffer, n, MSG_EOR);
            } else {
                nms_printf(NMSML_DBG2,
                       "Interleaved RTCP data (%u bytes: channel %d -> sd %d)\n",
                       n, m, p->rtcp_fd);
                send(p->rtcp_fd, buffer, n, MSG_EOR);
            }
        }
#endif
    }
    return n;
}
