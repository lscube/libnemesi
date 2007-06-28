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

#include <nemesi/rtcp.h>

void rtcp_clean(void *args)
{
    /*
    rtp_session *rtp_sess_head = (*(rtp_session **) args);
    rtp_session *rtp_sess;
    rtp_ssrc *stm_src;

    for (rtp_sess = rtp_sess_head; rtp_sess; rtp_sess = rtp_sess->next)
        for (stm_src = rtp_sess->ssrc_queue; stm_src;
             stm_src = stm_src->next)
            if (stm_src->rtcptofd > 0)
                close(stm_src->rtcptofd);
    */
    nms_printf(NMSML_DBG1, "RTCP Thread R.I.P.\n");
}
