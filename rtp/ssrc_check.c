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

int ssrc_check(struct RTP_Session *rtp_sess, uint32 ssrc, struct Stream_Source **stm_src, struct sockaddr_in recfrom, enum proto_types proto_type)
{
	struct Conflict *stm_conf=rtp_sess->conf_queue;
	struct sockaddr_in *sockaddr=NULL;
	socklen_t socklen=sizeof(struct sockaddr_in);
	char port[256];
	uint8 local_collision;
	
	// memset(port, 0, strlen(port));
	memset(port, 0, sizeof(port));

	local_collision = (rtp_sess->local_ssrc == ssrc);
	pthread_mutex_lock(&rtp_sess->syn);	
	pthread_mutex_unlock(&rtp_sess->syn);	
	for(*stm_src=rtp_sess->ssrc_queue; !local_collision && *stm_src && ((*stm_src)->ssrc != ssrc); *stm_src=(*stm_src)->next);
	if(!*stm_src && !local_collision ){
		/* nuovo SSRC */
		/* inserimento in testa */
		pthread_mutex_lock(&rtp_sess->syn);	
		nmsprintf(NMSML_DBG1, "new SSRC\n");
		if ( set_stm_src(rtp_sess, stm_src, ssrc, recfrom, proto_type) < 0){
			pthread_mutex_unlock(&rtp_sess->syn);	
			return -nmsprintf(NMSML_ERR, "Error while setting new Stream Source\n");
		}

		poinit(&((*stm_src)->po),&(rtp_sess->bp));
		pthread_mutex_unlock(&rtp_sess->syn);	
		return 1;
	} else {
		if (local_collision){
			
			if ( (sockaddr=(struct sockaddr_in *)malloc(sizeof(struct sockaddr_in))) == NULL )
				return -nmsprintf(NMSML_FATAL, "Cannot allocate memory\n");

			if (proto_type == RTP)
				getsockname(rtp_sess->rtpfd, (struct sockaddr *) sockaddr, &socklen);
			else
				getsockname(rtp_sess->rtcpfd, (struct sockaddr *) sockaddr, &socklen);
			
		} else if (proto_type == RTP){
			
			if (((*stm_src)->rtp_from).sin_port == 0) 
				(*stm_src)->rtp_from=recfrom;
			sockaddr=&((*stm_src)->rtp_from);
			
		} else if (proto_type == RTCP){
			
			if (((*stm_src)->rtcp_from).sin_port == 0) 
				(*stm_src)->rtcp_from=recfrom;
			sockaddr=&((*stm_src)->rtcp_from);

			if ( ((*stm_src)->rtcp_to).sin_port == 0 ){
				
				((*stm_src)->rtcp_to).sin_addr=recfrom.sin_addr;
				/*((*stm_src)->rtcp_to).sin_port=recfrom.sin_port;*/
				((*stm_src)->rtcp_to).sin_port=(rtp_sess->transport).srv_ports[1];

				sprintf(port,"%d", ntohs(((*stm_src)->rtcp_to).sin_port));
				if ( server_connect(inet_ntoa(((*stm_src)->rtcp_to).sin_addr), port, &((*stm_src)->rtcptofd), UDP) ){
					nmsprintf(NMSML_WARN, "Cannot connect to remote RTCP port %s:%s\n", inet_ntoa(((*stm_src)->rtcp_to).sin_addr), port);
					(*stm_src)->rtcptofd=-2;
				}
			}
		}

		if(memcmp(&((*sockaddr).sin_family), &(recfrom.sin_family), sizeof(sa_family_t)+sizeof(uint16_t)+sizeof(struct in_addr)) != 0){
			
			/* An identifier collision or a loop is indicated */
			
			if( ssrc != rtp_sess->local_ssrc ){
				/* OPTIONAL error counter step not implemented */
				nmsprintf(NMSML_VERB, "Warning! An identifier collision or a loop is indicated.\n");
				return 2;
			}

			/* A collision or loop of partecipants's own packets */
			
			else {
				while ((stm_conf != NULL) && (memcmp(&((stm_conf->transaddr)->sin_family), &(recfrom.sin_family),\
							sizeof(sa_family_t)+sizeof(uint16_t)+sizeof(struct in_addr)) != 0))
					stm_conf=stm_conf->next;
				
				if (stm_conf){
					
					/* OPTIONAL error counter step not implemented */
					
					stm_conf->time=time(NULL);
					return 2;
				} else {
					
					/* New collision, change SSRC identifier */
					
					nmsprintf(NMSML_VERB, "SSRC collision detected: getting new!\n");
					

					/* Send RTCP BYE pkt */
					/*       TODO        */

					/* choosing new ssrc */
					rtp_sess->local_ssrc=random32(0);
					rtp_sess->transport.ssrc=rtp_sess->local_ssrc;
			
					/* New entry in SSRC queue with conflicting ssrc */
					if( (stm_conf=(struct Conflict *)malloc(sizeof(struct Conflict))) == NULL)
						return -nmsprintf(NMSML_FATAL, "Cannot allocate memory!\n");

					/* inserimento in testa */
					pthread_mutex_lock(&rtp_sess->syn);	
					if ( set_stm_src(rtp_sess, stm_src, ssrc, recfrom, proto_type) < 0) {
						pthread_mutex_unlock(&rtp_sess->syn);	
						return -nmsprintf(NMSML_ERR, "Error while setting new Stream Source\n");
					}
					poinit(&((*stm_src)->po),&(rtp_sess->bp));
					pthread_mutex_unlock(&rtp_sess->syn);	
				
					/* New entry in SSRC Conflict queue */
					stm_conf->transaddr=sockaddr;
					stm_conf->time=time(NULL);
					stm_conf->next=rtp_sess->conf_queue;
					rtp_sess->conf_queue=stm_conf;
				}
				
			}
		}
	}
	
	return local_collision;
}
