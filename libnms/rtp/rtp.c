/* * 
 *  $Id:rtp.c 267 2006-01-12 17:19:45Z shawill $
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

#define PO_BUFF_SIZE_SEC 0
#define PO_BUFF_SIZE_MSEC 700

void *rtp(void *args)
{
	rtp_session *rtp_sess_head=((rtp_thread *)args)->rtp_sess_head;
	pthread_mutex_t *syn = &((rtp_thread *)args)->syn;
	rtp_session *rtp_sess;
	struct timespec ts;
	int maxfd=0;
	
	fd_set readset;
	char buffering=1;
	
	for (rtp_sess=rtp_sess_head; rtp_sess; rtp_sess=rtp_sess->next)
		bpinit(&(rtp_sess->bp));
	
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
/*	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL); */
	pthread_cleanup_push(rtp_clean, (void *)args);

	/* Playout Buffer Size */
	/*
	dec_args->startime.tv_sec=0;
	dec_args->startime.tv_usec=700*(1000);
	*/
	// dec_args->startime.tv_sec=PO_BUFF_SIZE_SEC;
	// dec_args->startime.tv_usec=PO_BUFF_SIZE_MSEC*(1000);
	/* 500 msec */

	while(1){
		FD_ZERO(&readset);

		for (rtp_sess=rtp_sess_head; rtp_sess; rtp_sess=rtp_sess->next){
			maxfd = max(rtp_sess->rtpfd,maxfd);
			FD_SET(rtp_sess->rtpfd, &readset);
		}
		
		select(maxfd+1, &readset, NULL, NULL, NULL);
		
		for (rtp_sess=rtp_sess_head; rtp_sess; rtp_sess=rtp_sess->next)
			if (FD_ISSET(rtp_sess->rtpfd, &readset)){
				if (buffering) {
					if (rtp_sess->bp.flcount > BP_SLOT_NUM/2) {
						pthread_mutex_unlock(syn);
						buffering=0;
					} else { // TODO: buffering based on rtp jitter
						nms_printf(NMSML_DBG1, "\rBuffering (%d)%\t", (100*rtp_sess->bp.flcount)/(BP_SLOT_NUM/2));
					}
				}
				if(rtp_recv(rtp_sess)){
					/* Waiting 20 msec for decoder ready */
					nms_printf(NMSML_DBG1, "Waiting for decoder ready!\n");
					ts.tv_sec=0;
					ts.tv_nsec=20*(1000);
					nanosleep(&ts, NULL);
				}
			}
	}

	pthread_cleanup_pop(1);
}
