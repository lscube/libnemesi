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

void rtp_clean(void *args)
{
	struct RTP_Session *rtp_sess=((struct Dec_args *)args)->rtp_sess_head;
	struct RTP_Session *prev_rtp_sess;
	struct Stream_Source *csrc, *psrc;
	struct Conflict *conf, *pconf;
	void *ret;
	int i;
	
	nmsprintf(NMSML_DBG1, "RTP Thread is dying suicide!\n");

	if(rtp_sess->dec_tid > 0) {
		nmsprintf(NMSML_DBG1, "Sending cancel signal to Decoder Thread (ID: %lu)\n", rtp_sess->dec_tid);
		if (pthread_cancel(rtp_sess->dec_tid) != 0)
			nmsprintf(NMSML_DBG2, "Error while sending cancelation to Decoder Thread.\n");
		else
			pthread_join(rtp_sess->dec_tid, (void **)&ret);
		if ( ret != PTHREAD_CANCELED )
			nmsprintf(NMSML_DBG2, "Warning! Decoder Thread joined, but  not canceled!\n");
	}
	rtp_sess->dec_tid = -1;
	while(rtp_sess != NULL) {
		close(rtp_sess->rtpfd);
		close(rtp_sess->rtcpfd);

		csrc=rtp_sess->ssrc_queue;

		while(csrc != NULL){
			psrc=csrc;
			csrc=csrc->next;
			for(i=0; i<9; i++)
				free( ((char **)(&(psrc->ssrc_sdes)))[i] );
			free(psrc->rtp_from.addr);
			free(psrc->rtcp_from.addr);
			free(psrc->rtcp_to.addr);
			free(psrc);
		}
		bpkill(&(rtp_sess->bp));

		// transport allocs
		free((rtp_sess->transport).spec);

		conf=rtp_sess->conf_queue;
		while(conf) {
			pconf=conf;
			conf=conf->next;
			free(pconf->transaddr.addr);
			free(pconf);
		}

		prev_rtp_sess=rtp_sess;
		rtp_sess=rtp_sess->next;
		free(prev_rtp_sess);
	}
	free((struct Dec_args *)args);
	nmsprintf(NMSML_DBG1, "RTP Thread R.I.P.\n");
}
