/* * 
 *  $Id:rtp_clean.c 267 2006-01-12 17:19:45Z shawill $
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
	struct rtp_thread *rtp_th = (struct rtp_thread *)args;
	rtp_session *rtp_sess=rtp_th->rtp_sess_head;
	rtp_session *prev_rtp_sess;
	rtp_ssrc *csrc, *psrc;
	struct rtp_conflict *conf, *pconf;
	rtp_fmts_list *fmtlist, *pfmtlist;
	int i;
	
	nms_printf(NMSML_DBG1, "RTP Thread is dying suicide!\n");
//	pthread_mutex_lock(&rtp_th->syn);
//	pthread_mutex_trylock(&rtp_th->syn);
	
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
			for (i=0; i<128; i++)
				if (rtp_sess->parsers_uninits[i])
					rtp_sess->parsers_uninits[i](psrc, i);
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
		// announced rtp payload list
		for(fmtlist=rtp_sess->announced_fmts; fmtlist; pfmtlist=fmtlist, fmtlist=fmtlist->next, free(pfmtlist));
		// rtp payload types definitions attributes
		for (i=0; i<128; i++)
			if (rtp_sess->rtpptdefs[i])
				free(rtp_sess->rtpptdefs[i]->attrs.data);
		// rtp payload types dynamic definitions
		for (i=96; i<128; free(rtp_sess->rtpptdefs[i++]));

		prev_rtp_sess=rtp_sess;
		rtp_sess=rtp_sess->next;
		free(prev_rtp_sess);
	}
	rtp_th->rtp_sess_head = NULL;
	
//	pthread_mutex_unlock(&rtp_th->syn);

	nms_printf(NMSML_DBG1, "RTP Thread R.I.P.\n");
}
