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

#include <stdio.h>
#include <string.h>

#include <nemesi/sdp.h>
#include <nemesi/comm.h>

/**
 *  Parses the SDP
 *  @param descr the text to be parsed
 *  @param descr_len the length of the text
 *  @return NULL on failure or a newly allocated sdp_session_info on success
 */

sdp_session_info *sdp_session_setup(char *descr, int descr_len)
{
    sdp_session_info *new;
    char *tkn = NULL;
    char error = 0;        // error flag

    // we use calloc, so it's all already initialized to NULL
    if (!(new = (sdp_session_info *) calloc(1, sizeof(sdp_session_info))))
        return NULL;

    tkn = strtok(descr, "\r\n");

    do {
        if (tkn == NULL) {
            nms_printf(NMSML_ERR,
                   "Empty SDP description body... discarding\n");
            error = 1;
            break;
            // return NULL;
        }

        switch (*tkn) {
        case 'v':
            new->v = tkn + 2;
            break;
        case 'o':
            new->o = tkn + 2;
            break;
        case 's':
            new->s = tkn + 2;
            break;
        case 'i':
            new->i = tkn + 2;
            break;
        case 'u':
            new->u = tkn + 2;
            break;
        case 'e':
            new->e = tkn + 2;
            break;
        case 'p':
            new->p = tkn + 2;
            break;
        case 'c':
            new->c = tkn + 2;
            break;
        case 'b':
            new->b = tkn + 2;
            break;
        case 't':
            new->t = tkn + 2;
            break;
        case 'r':
            new->r = tkn + 2;
            break;
        case 'z':
            new->z = tkn + 2;
            break;
        case 'k':
            new->k = tkn + 2;
            break;
        case 'a':
            tkn += 2;
            if (sdp_set_attr(&(new->attr_list), tkn)) {
                nms_printf(NMSML_ERR,
                       "Error setting SDP session attribute\n");
                error = 1;
                break;
            }
            tkn += strlen(tkn);
            break;
        case 'm':
            tkn[strlen(tkn)] = '\n';
            if (!(new->media_info_queue = 
                    sdp_media_setup(&tkn, descr_len - (tkn - descr)))) {
                error = 1;
                break;
                // return NULL;
            }
            break;
        }
    } while (tkn = strtok(NULL, "\r\n"));

    if (error) {        // there was an error?
        sdp_session_destroy(new);
        return NULL;
    }

    return new;
}
