/* * 
 *  $Id$
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

#include <nemesi/rtp.h>

int set_stm_src(struct RTP_Session *rtp_sess, struct Stream_Source **stm_src, uint32 ssrc, struct sockaddr_in recfrom, enum proto_types proto_type)
{
	char port[256];
	
	memset(port, 0, sizeof(port));
	
	if(((*stm_src)=(struct Stream_Source *)malloc(sizeof(struct Stream_Source))) == NULL)
		return -nmsprintf(NMSML_FATAL, "Cannot allocate memory\n");

	(*stm_src)->next=rtp_sess->ssrc_queue;
	rtp_sess->ssrc_queue=*stm_src;
	
	(*stm_src)->ssrc=ssrc;
	(*stm_src)->rtcptofd=-1;
	memset(&(*stm_src)->ssrc_stats, 0, sizeof(struct SSRC_Stats));
	memset(&(*stm_src)->ssrc_sdes, 0, sizeof(struct SSRC_Description));
	
	if (proto_type == RTP){
		(*stm_src)->rtp_from=recfrom;
		memset(&((*stm_src)->rtcp_from), 0, sizeof(struct sockaddr_in));
	}
	else if (proto_type == RTCP){
		(*stm_src)->rtcp_from=recfrom;
		memset(&((*stm_src)->rtp_from), 0, sizeof(struct sockaddr_in));
	}

	memset(&((*stm_src)->rtcp_to), 0, sizeof(struct sockaddr_in));

	
	if( memcmp(&(recfrom.sin_addr),&((rtp_sess->transport).srcaddr), sizeof(struct in_addr)) == 0 ){
		/* Nel caso in cui 'indirizzo IP da cui riceviamo i dati sia uguale a
		 * quello annunciato in RTSP, utilizziamo le informazioni specificate
		 * nella sessione RTSP per impostare l'indirizzo di trasporto della
		 * connessione RTCP di destinazione. */
		((*stm_src)->rtcp_to).sin_addr=(rtp_sess->transport).srcaddr;
		((*stm_src)->rtcp_to).sin_port=(rtp_sess->transport).srv_ports[1];

	} else if (proto_type == RTCP){
		/* In mancanza di informazioni assumiamo che l'indirizzo di rete
		 * della destinazione RTCP conicida con quello della sorgente RTCP
		 * e che le porte siano qualle specificate in RTSP */
		((*stm_src)->rtcp_to).sin_addr=recfrom.sin_addr;
		/* ((*stm_src)->rtcp_to).sin_port=recfrom.sin_port; */
		((*stm_src)->rtcp_to).sin_port=(rtp_sess->transport).srv_ports[1];
	}
	if ( ((*stm_src)->rtcp_to).sin_port != 0 ){
		sprintf(port,"%d", ntohs(((*stm_src)->rtcp_to).sin_port));
		if ( server_connect(inet_ntoa(((*stm_src)->rtcp_to).sin_addr), port, &((*stm_src)->rtcptofd), UDP) ){
			nmsprintf(NMSML_WARN, "Cannot connect to remote RTCP destination %s:%s\n", inet_ntoa(((*stm_src)->rtcp_to).sin_addr), port);
			(*stm_src)->rtcptofd=-2;
		}
	}
	return 0;
}
