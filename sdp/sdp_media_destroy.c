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

#include <stdlib.h>

#include "sdp.h"

void sdp_media_destroy(sdp_medium_info * media_queue)
{
    sdp_medium_info *sdp_m, *sdp_m_prev;
    sdp_attr *sdp_attr, *sdp_attr_prev;

    for (sdp_m = media_queue; sdp_m;
         sdp_m_prev = sdp_m, sdp_m = sdp_m->next, free(sdp_m_prev))
        for (sdp_attr = sdp_m->attr_list; sdp_attr;
             sdp_attr_prev = sdp_attr, sdp_attr =
             sdp_attr->next, free(sdp_attr_prev));

    /* the last two lines are equivalent to these, but aren't they more beautiful ? ;-)
       sdp_m=session->media_info_queue;
       while (sdp_m) {
       sdp_attr=sdp_m->attr_list;
       while(sdp_attr) {
       sdp_attr_prev=sdp_attr;
       sdp_attr=sdp_attr->next;
       free(sdp_attr_prev);
       }
       sdp_m_prev=sdp_m;
       sdp_m=sdp_m->next;
       free(sdp_m_prev);
       }
     */
}
