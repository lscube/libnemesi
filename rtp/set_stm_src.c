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

int set_stm_src(struct RTP_Session *rtp_sess, struct Stream_Source **stm_src, uint32 ssrc, NMSsockaddr *recfrom, enum proto_types proto_type)
{
	int addrcmp_err;
	
	
	if(((*stm_src)=(struct Stream_Source *)calloc(1, sizeof(struct Stream_Source))) == NULL)
		return -nmsprintf(NMSML_FATAL, "Cannot allocate memory\n");

	(*stm_src)->next=rtp_sess->ssrc_queue;
	rtp_sess->ssrc_queue=*stm_src;
	
	(*stm_src)->ssrc=ssrc;
	(*stm_src)->rtcptofd=-1;
	// we do not need to reset memory area 'cause we use calloc
	// memset(&(*stm_src)->ssrc_stats, 0, sizeof(struct SSRC_Stats));
	// memset(&(*stm_src)->ssrc_sdes, 0, sizeof(struct SSRC_Description));
	
	if (proto_type == RTP){
		sockaddrdup(&(*stm_src)->rtp_from, recfrom);
		nmsprintf(NMSML_DBG2, "RTP/set_stm_src: proto RTP\n");
		// we do not need to reset memory area 'cause we use calloc
		// memset(&((*stm_src)->rtcp_from), 0, sizeof(NMSsockaddr));
	} else if (proto_type == RTCP){
		sockaddrdup(&(*stm_src)->rtcp_from, recfrom);
		nmsprintf(NMSML_DBG2, "RTP/set_stm_src: proto RTCP\n");
		// we do not need to reset memory area 'cause we use calloc
		// memset(&((*stm_src)->rtp_from), 0, sizeof(NMSsockaddr));
	}

	// we do not need to reset memory area 'cause we use calloc
	// memset(&(*stm_src)->rtcp_to, 0, sizeof(NMSsockaddr));

	if (!(addrcmp_err = sockaddrcmp(recfrom->addr, recfrom->addr_len, (rtp_sess->transport).srcaddr.addr, (rtp_sess->transport).srcaddr.addr_len ))) {
		/* IT: Nel caso in cui l'indirizzo IP da cui riceviamo i dati
		 * sia uguale a quello annunciato in RTSP, utilizziamo le
		 * informazioni specificate nella sessione RTSP per impostare
		 * l'indirizzo di trasporto della connessione RTCP di
		 * destinazione. */
		/* EN: If the address from which we are receiving data is the
		 * same to that announced in RTSP session, then we use RTSP
		 * informations to set transport address for RTCP connection */
		if ( rtcp_to_connect(*stm_src, &rtp_sess->transport.srcaddr, (rtp_sess->transport).srv_ports[1]) < 0 )
			return -1;
		nmsprintf(NMSML_DBG2, "RTP/set_stm_src: from RTSP\n");

	} else if (proto_type == RTCP){
		/* IT: In mancanza di informazioni assumiamo che l'indirizzo di rete
		 * della destinazione RTCP conicida con quello della sorgente RTCP
		 * e che le porte siano qualle specificate in RTSP */
		/* EN: If we lack of informations we assume that net address of
		 * RTCP destination is the same of RTP address and port is that
		 * specified in RTSP*/
		if ( rtcp_to_connect(*stm_src, recfrom, (rtp_sess->transport).srv_ports[1]) < 0 )
			return -1;
		nmsprintf(NMSML_DBG2, "RTP/set_stm_src: from RTP\n");
	} else {
		switch (addrcmp_err) {
			case WSOCK_CMP_ERRSIZE:
				nmsprintf(NMSML_DBG2, "WSOCK_CMP_ERRSIZE (%d!=%d)\n", recfrom->addr_len, (rtp_sess->transport).srcaddr.addr_len);
				break;
			case WSOCK_CMP_ERRFAMILY:
				nmsprintf(NMSML_DBG2, "WSOCK_CMP_ERRFAMILY\n");
				break;
			case WSOCK_CMP_ERRADDR:
				nmsprintf(NMSML_DBG2, "WSOCK_CMP_ERRADDR\n");
				break;
			case WSOCK_CMP_ERRPORT:
				nmsprintf(NMSML_DBG2, "WSOCK_CMP_ERRPORT\n");
				break;
		}
		nmsprintf(NMSML_DBG2, "RTP/set_stm_src: rtcp_to NOT set!!!\n");
	}

	return 0;
}
