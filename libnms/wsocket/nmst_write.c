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
 *	Francesco "shawill" Varano - francesco.varano@polito.it
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

#include <nemesi/wsocket.h>

int nmst_write(nms_transport * transport, void *buffer, size_t nbytes, void *protodata)
{
#ifdef HAVE_SCTP_NEMESI
	struct sctp_sndrcvinfo sinfo;
#endif
	switch (transport->type) {
	case TCP:
		return write(transport->fd, buffer, nbytes);
		break;
#ifdef HAVE_SCTP_NEMESI
	case SCTP:
		if (!protodata) {
			protodata = &sinfo;
			memset(protodata, 0, sizeof(struct sctp_sndrcvinfo));
		}
		return sctp_send(transport->fd, buffer, nbytes, 
			(struct sctp_sndrcvinfo *) protodata, MSG_EOR);
		break;
#endif
	default:
		break;
	}

	return 1;
}
