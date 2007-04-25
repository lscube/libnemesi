/* *
 *
 *
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *      
 *    Giampaolo "mancho" Mancini - manchoz@inwind.it
 *    Francesco "shawill" Varano - shawill@infinto.it
 *    Dario Gallucci - dario.gallucci@gmail.com
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
#include <nemesi/utils.h>

int set_transport_str_tcp(rtp_session * rtp_sess, char *buff)
{
    uint8 ilvds[2];

    sprintf(buff + strlen(buff), "unicast;");

    if ( rtp_get_interleaved(rtp_sess, ilvds) == RTP_TRANSPORT_SET)
        sprintf(buff+strlen(buff), "interleaved=%u-%u;", ilvds[0], ilvds[1]);
    
    return 0;
}
