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

/** @file rtcp_sdes.c
 * This file contains the functions that perform RTCP Source
 * Description packets parsing and building
 */

#include "rtcp.h"
#include "version.h"

#ifndef WIN32
#       include <pwd.h>
#endif

/**
 * Actually gets the source description inside the RTCP SDES packet
 * and sets it inside the rtp_ssrc structure.
 * @param stm_src The SSRC from which we received the packet
 * @param item The actual SDES item to parse.
 * @return 0
 */
static int rtcp_set_ssrc_sdes(rtp_ssrc * stm_src, rtcp_sdes_item_t * item)
{
    char *str = ((char **) (&(stm_src->ssrc_sdes)))[item->type];

    if (str != NULL) {
        if (memcmp(str, item->data, item->len) != 0) {
            free(str);
            if ((str =
                 (((char **) (&(stm_src->ssrc_sdes)))[item->
                                  type]) =
                 (char *) malloc(item->len + 1)) == NULL)
                return nms_printf(NMSML_FATAL,
                          "Cannot allocate memory!\n");

            memcpy(str, item->data, item->len);
            str[item->len] = 0;
        }

    } else {
        if ((str = ((char **) (&(stm_src->ssrc_sdes)))[item->type] =
             (char *) malloc(item->len + 1)) == NULL)
            return nms_printf(NMSML_FATAL,
                      "Cannot allocate memory!\n");

        memcpy(str, item->data, item->len);
        str[item->len] = 0;
    }
    return 0;
}

/**
 * Parse an incoming RTCP Packet with source description
 * @param stm_src The SSRC from which we received the packet
 * @param pkt The packet itself
 * @return 0
 */
int rtcp_parse_sdes(rtp_ssrc * stm_src, rtcp_pkt * pkt)
{
    int8_t count = pkt->common.count;
    rtcp_sdes_t *sdes = &(pkt->r.sdes);
    rtcp_sdes_item_t *rsp, *rspn;
    rtcp_sdes_item_t *end =
        (rtcp_sdes_item_t *) ((uint32_t *) pkt + pkt->common.len + 1);

    nms_printf(NMSML_DBG3, "Received SDES from SSRC: %u\n",
           pkt->r.sdes.src);
    while (--count >= 0) {
        rsp = &(sdes->item[0]);
        if (rsp >= end)
            break;
        for (; rsp->type; rsp = rspn) {
            rspn =
                (rtcp_sdes_item_t *) ((uint8_t *) rsp + rsp->len +
                          2);
            if (rspn >= end) {
                rsp = rspn;
                break;
            }
            if (rtcp_set_ssrc_sdes(stm_src, rsp))
                return 1;
        }
        sdes =
            (rtcp_sdes_t *) ((uint32_t *) sdes +
                     (((uint8_t *) rsp - (uint8_t *) sdes) >> 2) +
                     1);
    }
    if (count >= 0)
        nms_printf(NMSML_WARN, "Invalid RTCP SDES pkt format!\n");
    else if (stm_src->ssrc_stats.probation)
        stm_src->ssrc_stats.probation = 1;
    return 0;
}


/**
 * Build the Source Description packet
 * @param rtp_sess The RTP Session for which to build the SDES
 * @param pkt The packet where to write the SDES
 * @param left Free space on the packet where the SDES will be written
 * @return Length of the built source description (number of 32bit words)
 */
int rtcp_build_sdes(rtp_session * rtp_sess, rtcp_pkt * pkt, int left)
{
#ifndef WIN32
    struct passwd *pwitem = getpwuid(getuid());
    char * user = pwitem->pw_name;
    char * real_name = pwitem->pw_gecos;
#else
    char * user = "guest";
    char * real_name = "";
#endif

    rtcp_sdes_item_t *item;
    char str[MAX_SDES_LEN] = "";
    int len, pad;
    char addr[128];        /* Unix domain is largest */

    memset(str, 0, MAX_SDES_LEN);

    /* SDES CNAME: username@ipaddress */
    // if ( sock_ntop_host(rtp_sess->transport.dstaddr.addr, rtp_sess->transport.dstaddr.addr_len, addr, sizeof(addr)) ) {
    if (nms_addr_ntop(&rtp_sess->transport.RTP.u.udp.dstaddr, addr, sizeof(addr))) {
        strcpy(str, user);
        strcat(str, "@");
        strcat(str, addr);
    }
    if (((strlen(str) + sizeof(rtcp_sdes_item_t) - 1 +
          sizeof(rtcp_common_t) + 1) >> 2) > (unsigned int) left)
        /* No space left in UDP pkt */
        return 0;

    len =
        (strlen(str) + sizeof(rtcp_sdes_item_t) - 1 +
         sizeof(rtcp_common_t)) >> 2;
    pkt->common.ver = RTP_VERSION;
    pkt->common.pad = 0;
    pkt->common.count = 1;
    pkt->common.pt = RTCP_SDES;
    pkt->r.sdes.src = htonl(rtp_sess->local_ssrc);

    item = pkt->r.sdes.item;

    item->type = RTCP_SDES_CNAME;
    item->len = strlen(str);
    strcpy((char *) item->data, str);

    item = (rtcp_sdes_item_t *) ((char *) item + strlen((char *) item));

    /* SDES NAME: real name, if it exists */
    if (strlen(strcpy(str, real_name))) {
        if (((strlen(str) + sizeof(rtcp_sdes_item_t) - 1 +
              sizeof(rtcp_common_t) + 1) >> 2) > (unsigned int) left) {
            /* No space left in UDP pkt */
            pad = 4 - len % 4;
            len += pad / 4;
            while (pad--)
                *((char *) item++) = 0;
            pkt->common.len = htons(len);
            return len;
        }

        len +=
            (strlen(str) + sizeof(rtcp_sdes_item_t) - 1 +
             sizeof(rtcp_common_t) + 1) >> 2;

        item->type = RTCP_SDES_NAME;
        item->len = strlen(str);
        strcpy((char *) item->data, str);

        item =
            (rtcp_sdes_item_t *) ((char *) item +
                      strlen((char *) item));
    }

    /* SDES TOOL */
    sprintf(str, "%s - %s", PROG_NAME, PROG_DESCR);
    if (((strlen(str) + sizeof(rtcp_sdes_item_t) - 1 +
          sizeof(rtcp_common_t)) >> 2) > (unsigned int) left) {
        /* No space left in UDP pkt */
        pad = 4 - len % 4;
        len += pad / 4;
        while (pad--)
            // *(((char *)item)++)=0;
            *((char *) item++) = 0;
        pkt->common.len = htons(len);
        return len;
    }

    len +=
        (strlen(str) + sizeof(rtcp_sdes_item_t) - 1 +
         sizeof(rtcp_common_t) + 1) >> 2;

    item->type = RTCP_SDES_TOOL;
    item->len = strlen(str);
    strcpy((char *) item->data, str);

    item = (rtcp_sdes_item_t *) ((char *) item + strlen((char *) item));

    pad = 4 - len % 4;
    len += pad / 4;
    while (pad--)
        // *(((char *)item)++)=0;
        *((char *) item++) = 0;
    pkt->common.len = htons(len);

    return len;
}
