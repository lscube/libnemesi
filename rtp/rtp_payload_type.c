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

#include <nemesi/rtp.h>

rtp_pt * rtp_get_pt_info(rtp_session * rtp_sess, unsigned pt)
{
    return rtp_sess->ptdefs[pt];
}

int16 rtp_get_next_pt(rtp_ssrc * stm_src)
{
    rtp_pkt *pkt;

    if (!(pkt = rtp_get_pkt(stm_src, NULL)))
        return RTP_BUFF_EMPTY;

    return pkt->pt;
}

int rtp_dynpt_reg(rtp_session * rtp_sess, unsigned pt, char *mime)
{
    if (rtp_dynpt_encname(rtp_sess->ptdefs, pt, mime))
        return RTP_ERROR;

    if (rtp_parser_reg(rtp_sess, pt, mime))
        return RTP_ERROR;

    nms_printf(NMSML_DBG2, "registered %s mime for %d payload type\n",
           mime, pt);

    return RTP_OK;
}

int rtp_announce_pt(rtp_session * rtp_sess, unsigned pt,
            rtp_media_type media_type)
{
    rtp_pt *rtppt;
    rtp_fmts_list *fmt, **prev_fmt;

    if (pt > 127) {
        nms_printf(NMSML_ERR, "rtp payload type not valid (%u)\n", pt);
        return RTP_ERROR;
    } else if (pt >= 96) {
        if (!(rtppt = rtp_pt_new(media_type)))
            return RTP_ERROR;
        rtp_dynpt_set(rtp_sess->ptdefs, rtppt, pt);
    }
    if (!(fmt = malloc(sizeof(rtp_fmts_list)))) {
        nms_printf(NMSML_FATAL,
               "Could not alloc memory for rtp_fmts_list\n");
        return RTP_ERRALLOC;
    }
    fmt->pt = pt;
    fmt->rtppt = rtp_sess->ptdefs[pt];
    fmt->next = NULL;
    for (prev_fmt = &rtp_sess->announced_fmts; *prev_fmt;
         prev_fmt = &(*prev_fmt)->next);
    *prev_fmt = fmt;
    prev_fmt = &fmt->next;

    return RTP_OK;
}
