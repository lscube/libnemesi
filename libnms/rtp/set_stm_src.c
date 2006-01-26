/* * 
 *  $Id:set_stm_src.c 267 2006-01-12 17:19:45Z shawill $
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

int set_stm_src(struct rtp_session *rtp_sess, struct rtp_ssrc **stm_src, uint32 ssrc, nms_sockaddr *recfrom, enum proto_types proto_type)
{
	int addrcmp_err;
	nms_addr nms_addr;
	
	if(((*stm_src)=(struct rtp_ssrc *)calloc(1, sizeof(struct rtp_ssrc))) == NULL)
		return -nms_printf(NMSML_FATAL, "Cannot allocate memory\n");

	(*stm_src)->next=rtp_sess->ssrc_queue;
	rtp_sess->ssrc_queue=*stm_src;
	
	(*stm_src)->ssrc=ssrc;
	(*stm_src)->rtcptofd=-1;
	// we do not need to reset memory area 'cause we use calloc
	// memset(&(*stm_src)->ssrc_stats, 0, sizeof(struct rtp_ssrc_stats));
	// memset(&(*stm_src)->ssrc_sdes, 0, sizeof(struct rtp_ssrc_descr));
	
	if (proto_type == RTP){
		sockaddrdup(&(*stm_src)->rtp_from, recfrom);
		nms_printf(NMSML_DBG2, "RTP/set_stm_src: proto RTP\n");
		// we do not need to reset memory area 'cause we use calloc
		// memset(&((*stm_src)->rtcp_from), 0, sizeof(nms_sockaddr));
	} else if (proto_type == RTCP){
		sockaddrdup(&(*stm_src)->rtcp_from, recfrom);
		nms_printf(NMSML_DBG2, "RTP/set_stm_src: proto RTCP\n");
		// we do not need to reset memory area 'cause we use calloc
		// memset(&((*stm_src)->rtp_from), 0, sizeof(nms_sockaddr));
	}

	// we do not need to reset memory area 'cause we use calloc
	// memset(&(*stm_src)->rtcp_to, 0, sizeof(nms_sockaddr));
	if ( sock_get_addr(recfrom->addr, &nms_addr) )
		return -nms_printf(NMSML_ERR, "Address of received packet not valid\n");
	if (!(addrcmp_err = addrcmp(&nms_addr, &rtp_sess->transport.srcaddr ))) {
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
		nms_printf(NMSML_DBG2, "RTP/set_stm_src: from RTSP\n");

	} else if (proto_type == RTCP){
		/* IT: In mancanza di informazioni assumiamo che l'indirizzo di rete
		 * della destinazione RTCP conicida con quello della sorgente RTCP
		 * e che le porte siano qualle specificate in RTSP */
		/* EN: If we lack of informations we assume that net address of
		 * RTCP destination is the same of RTP address and port is that
		 * specified in RTSP*/
		if ( rtcp_to_connect(*stm_src, &nms_addr, (rtp_sess->transport).srv_ports[1]) < 0 )
			return -1;
		nms_printf(NMSML_DBG2, "RTP/set_stm_src: from RTP\n");
	} else {
		switch (addrcmp_err) {
			case WSOCK_ERRFAMILY:
				nms_printf(NMSML_DBG2, "WSOCK_ERRFAMILY\n");
				break;
			case WSOCK_ERRADDR:
				nms_printf(NMSML_DBG2, "WSOCK_ERRADDR\n");
				break;
			case WSOCK_ERRFAMILYUNKNOWN:
				nms_printf(NMSML_DBG2, "WSOCK_ERRFAMILYUNKNOWN\n");
				break;
		}
		nms_printf(NMSML_DBG2, "RTP/set_stm_src: rtcp_to NOT set!!!\n");
		// return -1;
	}

	return 0;
}
