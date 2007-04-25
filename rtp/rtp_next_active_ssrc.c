/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *      
 *  Giampaolo "mancho" Mancini - giampaolo.mancini@polito.it
 *    Francesco "shawill" Varano - francesco.varano@polito.it
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

#include <nemesi/rtp.h>

rtp_ssrc *rtp_next_active_ssrc(rtp_ssrc * ssrc)
{
    rtp_session *rtp_sess;

    if (!ssrc)
        return NULL;

    if (ssrc->next_active)
        return ssrc->next_active;

    for (rtp_sess = ssrc->rtp_sess->next; rtp_sess;
         rtp_sess = rtp_sess->next)
        if (rtp_sess->active_ssrc_queue)
            return rtp_sess->active_ssrc_queue;

    return NULL;
}
