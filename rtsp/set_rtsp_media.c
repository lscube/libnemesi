/* * 
 *  $Id:set_rtsp_media.c 267 2006-01-12 17:19:45Z shawill $
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
#include <nemesi/methods.h>
#include <nemesi/utils.h>
#include <nemesi/rtpptdefs.h>

int set_rtsp_media(rtsp_thread * rtsp_th)
{

    rtsp_session *curr_rtsp_s = rtsp_th->rtsp_queue;
    rtsp_medium *curr_rtsp_m = NULL;
    sdp_medium_info *sdp_m;
    sdp_attr *sdp_attr;
    char *tkn, *ch;
    uint8 pt;

    switch (rtsp_th->descr_fmt) {
    case DESCRIPTION_SDP_FORMAT:
        for (sdp_m = curr_rtsp_s->info->media_info_queue; sdp_m;
             sdp_m = sdp_m->next) {
            if (curr_rtsp_m == NULL) {
                /* first medium */
                if ((curr_rtsp_s->media_queue = curr_rtsp_m =
                     rtsp_med_create(rtsp_th->transport.fd)) ==
                    NULL)
                    return 1;
            } else if (rtsp_th->type == CONTAINER) {
                /* media in the same session */
                if ((curr_rtsp_m->next =
                     rtsp_med_create(rtsp_th->transport.fd)) ==
                    NULL)
                    return 1;
                curr_rtsp_m->rtp_sess->next =
                    curr_rtsp_m->next->rtp_sess;
                curr_rtsp_m = curr_rtsp_m->next;
            } else if (rtsp_th->type == M_ON_DEMAND) {
                /* one medium for each session */
                if ((curr_rtsp_s->next =
                     rtsp_sess_dup(curr_rtsp_s)) == NULL)
                    return 1;
                curr_rtsp_s = curr_rtsp_s->next;
                if ((curr_rtsp_s->media_queue =
                     rtsp_med_create(rtsp_th->transport.fd)) ==
                    NULL)
                    return 1;
                curr_rtsp_m->rtp_sess->next =
                    curr_rtsp_s->media_queue->rtp_sess;
                curr_rtsp_m = curr_rtsp_s->media_queue;
            }
            curr_rtsp_m->medium_info = sdp_m;

            // setup rtp format list for current media
            for (tkn = sdp_m->fmts;
                 *tkn && !(!(pt = strtoul(tkn, &ch, 10))
                       && ch == tkn); tkn = ch) {
                switch (sdp_m->media_type) {
                case 'A':
                    if (rtp_announce_pt
                        (curr_rtsp_m->rtp_sess, pt, AU))
                        return 1;
                    break;
                case 'V':
                    if (rtp_announce_pt
                        (curr_rtsp_m->rtp_sess, pt, VI))
                        return 1;
                    break;
                default:
                    // not recognized
                    if (rtp_announce_pt
                        (curr_rtsp_m->rtp_sess, pt, NA))
                        return 1;
                    break;
                }
            }

            for (sdp_attr = sdp_m->attr_list; sdp_attr;
                 sdp_attr = sdp_attr->next) {
                if (!strncmpcase(sdp_attr->a, "control", 7)) {
                    tkn = sdp_attr->a + 7;    // 7 == strlen("control")
                    while ((*tkn == ' ') || (*tkn == ':'))    // skip spaces and colon
                        tkn++;
                    curr_rtsp_m->filename = tkn;
                } else
                    if (!strncmpcase(sdp_attr->a, "rtpmap", 6))
                {
                    /* We assume the string in the format:
                     * rtpmap:PaloadType EncodingName/ClockRate[/Channels] */
                    tkn = sdp_attr->a + 6;    // 6 == strlen("rtpmap")
                    // skip spaces and colon (we should not do this!)
                    while ((*tkn == ' ') || (*tkn == ':'))
                        tkn++;
                    if (((pt =
                          (uint8) strtoul(tkn, &tkn,
                                  10)) >= 96)
                        && (pt <= 127)) {
                        while (*tkn == ' ')
                            tkn++;
                        if (!(ch = strchr(tkn, '/'))) {
                            nms_printf(NMSML_WARN,
                                   "Invalid field rtpmap.\n");
                            break;
                        }
                        *ch = '\0';
                        if (rtp_dynpt_reg
                            (curr_rtsp_m->rtp_sess, pt,
                             tkn))
                            return 1;
                        switch (sdp_m->media_type) {
                        case 'A':
                            sscanf(ch + 1, "%u/%c",
                                   &curr_rtsp_m->
                                   rtp_sess->
                                   ptdefs[pt]->
                                   rate,
                                   &RTP_AUDIO
                                   (curr_rtsp_m->
                                rtp_sess->
                                ptdefs[pt])->
                                   channels);
                            break;
                        case 'V':
                            sscanf(ch + 1, "%u",
                                   &curr_rtsp_m->
                                   rtp_sess->
                                   ptdefs[pt]->
                                   rate);
                            break;
                        default:
                            // not recognized
                            break;
                        }
                        *ch = '/';
                        tkn = ++ch;
                    } else {
                        // shawill: should be an error or a warning?
                        nms_printf(NMSML_WARN,
                               "Warning: rtpmap attribute is trying to set a non-dynamic payload type: not permitted\n");
                    }
                } else
                    if (!strncmpcase(sdp_attr->a, "fmtp", 4)) {
                    /* We assume the string in the format:
                     * fmtp:PaloadType <format specific parameters> */
                    tkn = sdp_attr->a + 4;    // 4 == strlen("fmtp")
                    // skip spaces and colon (we should not do this!)
                    while ((*tkn == ' ') || (*tkn == ':'))
                        tkn++;
                    if ((pt =
                         (uint8) strtoul(tkn, &tkn,
                                 10)) <= 127) {
                        while (*tkn == ' ')
                            tkn++;
                        rtp_pt_attr_add(curr_rtsp_m->
                                rtp_sess->
                                ptdefs, pt,
                                tkn);
                    } else {
                        // shawill: should be an error or a warning?
                        nms_printf(NMSML_WARN,
                               "Warning: fmtp attribute is trying to set an out of bounds payload type: not permitted\n");
                    }
                } else if (!strncmpcase(sdp_attr->a, "med", 3)) {    // dirty keyword from old fenice used for dinamic payload change
                    sdp_medium_info m_info;
                    /* We assume the string in the format:
                     * med:sdp-like m= field */
                    tkn = sdp_attr->a + 3;    // 3 == strlen("med")
                    // skip spaces and colon (we should not do this!)
                    while ((*tkn == ' ') || (*tkn == ':'))
                        tkn++;
                    if (sdp_parse_m_descr(&m_info, tkn)) {
                        nms_printf(NMSML_ERR,
                               "malformed a=med: from fenice\n");
                        return 1;
                    }
                    // check if everything is correct
                    if (!
                        (pt =
                         strtoul(m_info.fmts, &ch, 10))
&& ch == m_info.fmts) {
                        nms_printf(NMSML_ERR,
                               "Could not determine pt value in a=med: string from fenice\n");
                        return 1;
                    }
                    switch (sdp_m->media_type) {
                    case 'A':
                        if (strncmpcase
                            (tkn, "audio ", 6)) {
                            nms_printf(NMSML_ERR,
                                   "a=med; attribute defined a different media type than the original\n");
                            return 1;
                        }
                        if (rtp_announce_pt
                            (curr_rtsp_m->rtp_sess, pt,
                             AU))
                            return 1;
                        break;
                    case 'V':
                        if (strncmpcase
                            (tkn, "video ", 6)) {
                            nms_printf(NMSML_ERR,
                                   "a=med; attribute defined a different media type than the original\n");
                            return 1;
                        }
                        if (rtp_announce_pt
                            (curr_rtsp_m->rtp_sess, pt,
                             VI))
                            return 1;
                        break;
                    default:
                        // not recognized
                        if (rtp_announce_pt
                            (curr_rtsp_m->rtp_sess, pt,
                             NA))
                            return 1;
                        break;
                    }
                }
            }
        }
        break;
    case DESCRIPTION_MH_FORMAT:
        /* not yet implemented */
        // break;
    default:
        nms_printf(NMSML_ERR, "Unknown decription format.\n");
        return 1;
        break;
    }

    return 0;
}
