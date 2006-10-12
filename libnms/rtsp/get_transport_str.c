/* * 
 *  $Id:get_transport_str.c 267 2006-01-12 17:19:45Z shawill $
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *  	
 *  	Giampaolo "mancho" Mancini - manchoz@inwind.it
 *	Francesco "shawill" Varano - shawill@infinto.it
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

int get_transport_str(rtp_session * rtp_sess, char *buff)
{
	char *tkna, *tknb;
	char str[256];
	int n;
	// char addr[128];              /* Unix domain is largest */

	memset(str, 0, sizeof(str));

	if (strstr(buff, RTP_AVP_TCP))
		rtp_sess->transport.type = TCP;
#ifdef HAVE_SCTP_NEMESI
	else if (strstr(buff, RTP_AVP_SCTP))
		rtp_sess->transport.type = SCTP;
#endif
	else if (strstr(buff, RTP_AVP_UDP))
		rtp_sess->transport.type = UDP;
	else
		return 1;

	for (tknb = strtok(buff, ";"); (*tknb == ' ') || (*tknb == ':');
	     tknb++);

	switch (rtp_sess->transport.type) {
	case UDP:
		n = get_transport_str_udp(rtp_sess, tkna, tknb);
		break;
	case TCP:
		n = get_transport_str_tcp(rtp_sess, tkna, tknb);
		break;
	case SCTP:
		n = get_transport_str_sctp(rtp_sess, tkna, tknb);
		break;
	default:
		break;
	}

	return n;
}
