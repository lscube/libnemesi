/* * 
 *  ./rtsp/set_transport_str.c: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:20 $
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

int set_transport_str(struct RTP_Session *rtp_sess, char **str)
{
	char buff[256];

	memset(buff, 0, 256);
	
	strcpy(buff, rtp_sess->transport.spec);
	*(buff+strlen(buff))=';';
	if (rtp_sess->transport.mode == multicast)
		sprintf(buff+strlen(buff), "multicast;");
	else
		sprintf(buff+strlen(buff), "unicast;");
	if (rtp_sess->transport.dstaddr.s_addr != inet_addr("0.0.0.0"))
		sprintf(buff+strlen(buff), "destination=%s;", inet_ntoa(rtp_sess->transport.dstaddr));
	if (rtp_sess->transport.srcaddr.s_addr != inet_addr("0.0.0.0"))
		sprintf(buff+strlen(buff), "source=%s;", inet_ntoa(rtp_sess->transport.srcaddr));
	if (rtp_sess->transport.layers)
		sprintf(buff+strlen(buff), "layers=%d;", rtp_sess->transport.layers);
	if (rtp_sess->transport.mode == record)
		sprintf(buff+strlen(buff), "mode=record;");
	else
		sprintf(buff+strlen(buff), "mode=play;");
	if (rtp_sess->transport.append)
		sprintf(buff+strlen(buff), "append;");
	if (rtp_sess->transport.ttl)
		sprintf(buff+strlen(buff), "ttl=%d;", rtp_sess->transport.ttl);
	if (rtp_sess->transport.mcs_ports[0])
		sprintf(buff+strlen(buff), "port=%d-%d;", \
				(int)ntohs(rtp_sess->transport.mcs_ports[0]), \
				(int)ntohs(rtp_sess->transport.mcs_ports[1]));
	if (rtp_sess->transport.cli_ports[0])
		sprintf(buff+strlen(buff), "client_port=%d-%d;", \
				(int)ntohs(rtp_sess->transport.cli_ports[0]), \
				(int)ntohs(rtp_sess->transport.cli_ports[1]));
	if(rtp_sess->transport.ssrc)
		sprintf(buff+strlen(buff), "ssrc=%u;", rtp_sess->transport.ssrc);
	
	/* eliminiamo l'ultimo ; */
	*(buff+strlen(buff)-1)='\0';

	if ((*str=(char *)malloc(sizeof(char)*(strlen(buff)+1))) == NULL){
		uiprintf("Cannot allocate memory!\n");
		return 1;
	}
	strcpy(*str, buff);

	return 0;
}
