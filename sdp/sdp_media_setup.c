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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <nemesi/sdp.h>
#include <nemesi/comm.h>
/**
 *  Parse SDP media informations
 *  @param descr the body to parse
 *  @param descr_len its length
 *  @return NULL on failure or a newly allocated sdp_medium_info on success
 */
sdp_medium_info *sdp_media_setup(char **descr, int descr_len)
{
    sdp_medium_info *queue = NULL, *curr_sdp_m = NULL;
    char *tkn = NULL;
    char error = 0;        // error flag
    // for cc tagging
    int pt;
    char *endtkn = NULL;

    tkn = strtok(*descr, "\r\n");

    do {
        if (tkn == NULL) {
            nms_printf(NMSML_ERR,
                   "Invalid SDP Media description section.\n");
            return NULL;
        }
        switch (*tkn) {
        case 'm':    /* create struct for new medium */
            if (!curr_sdp_m) {    // first medium description
                // we use calloc, so it's all already initialized to NULL
                if (!(queue = curr_sdp_m = calloc(1, sizeof (sdp_medium_info))))
                    return NULL;
            } else {    // not first medium in sdp session
                // we use calloc, so it's all already initialized to NULL
                if (!(curr_sdp_m->next = calloc(1, sizeof(sdp_medium_info))))
                {
                    error = 1;
                    break;
                    // return NULL;
                }
                curr_sdp_m = curr_sdp_m->next;
            }
            curr_sdp_m->m = tkn + 2;
            if (sdp_parse_m_descr(curr_sdp_m, curr_sdp_m->m))
                error = 1;
            break;
        case 'i':
            curr_sdp_m->i = tkn + 2;
            break;
        case 'c':
            curr_sdp_m->c = tkn + 2;
            break;
        case 'b':
            curr_sdp_m->b = tkn + 2;
            break;
        case 'k':
            curr_sdp_m->k = tkn + 2;
            break;
        case 'a':
            tkn += 2;
            if (sdp_set_attr(&(curr_sdp_m->attr_list), tkn)) {
                nms_printf(NMSML_ERR,
                       "Error setting SDP media attribute\n");
                error = 1;
                break;
                // return NULL;
            }
            if (issdplicense(tkn)) {
                if (!curr_sdp_m->cc)
                    if (!(curr_sdp_m->cc = cc_newlicense())) {
                        nms_printf(NMSML_ERR,
                               "Could not get new CC license struct\n");
                        error = 1;
                        break;
                        // return NULL;
                    }
                if (cc_set_sdplicense(curr_sdp_m->cc, tkn)) {
                    error = 1;
                    break;
                }
            }
            tkn += strlen(tkn);
            break;
        }
    } while (tkn = strtok(NULL, "\r\n"));

    *descr += descr_len;

    if (error) {        // there was an error?
        sdp_media_destroy(queue);
        return NULL;
    } else {        // setup CC tags for disk writing
        for (curr_sdp_m = queue;
             curr_sdp_m;
             curr_sdp_m = curr_sdp_m->next) {
            for (tkn = curr_sdp_m->fmts; *tkn; tkn = endtkn) {
                for (; *tkn == ' '; tkn++);    // skip spaces
                pt = strtol(tkn, &endtkn, 10);
                if (tkn != endtkn)
                    cc_setag(pt, curr_sdp_m->cc);
                else
                    break;
            }
        }
    }

    return queue;
}
