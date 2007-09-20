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

#include <nemesi/rtcp.h>

struct rtcp_event *rtcp_schedule(struct rtcp_event *head,
                 rtp_session * rtp_sess, struct timeval tv,
                 rtcp_type_t type)
{
    struct rtcp_event *new_event;
    struct rtcp_event *pevent = head;
    struct rtcp_event *event = head;

    if ((new_event =
         (struct rtcp_event *) malloc(sizeof(struct rtcp_event))) ==
        NULL) {
        nms_printf(NMSML_FATAL, "Cannot allocate memory!\n");
        return NULL;
    }
    new_event->rtp_sess = rtp_sess;
    new_event->tv = tv;
    new_event->type = type;
    new_event->next = NULL;

    if (!head)
        return new_event;

    while (event && timeval_subtract(NULL, &(event->tv), &tv)) {
        pevent = event;
        event = event->next;
    }
    if (pevent == head) {
        new_event->next = head;
        return new_event;
    }
    pevent->next = new_event;
    new_event->next = event;

    return head;
}
