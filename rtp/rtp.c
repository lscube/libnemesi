/* * 
 *  ./rtp/rtp.c: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:20 $
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
#include <nemesi/decoder.h>

void *rtp(void *args)
{
	struct RTP_Session *rtp_sess_head=((struct RTP_Session *)args);
	struct RTP_Session *rtp_sess;
	struct timeval tv;
	struct Dec_args *dec_args;
	int maxfd=0;
	
	fd_set readset;
	char first=1;
	
	for (rtp_sess=rtp_sess_head; rtp_sess; rtp_sess=rtp_sess->next)
		bpinit(&(rtp_sess->bp));
	
	if((dec_args=(struct Dec_args *)malloc(sizeof(struct Dec_args))) == NULL){
		uiprintf("Cannot allocate memory!\n");
		pthread_exit(NULL);
	}
	dec_args->rtp_sess_head=rtp_sess_head;

	/* Playout Buffer Size */
	dec_args->startime.tv_sec=0;
	dec_args->startime.tv_usec=500*(1000);
	/* 500 msec */

	pthread_mutex_init(&(dec_args->syn), NULL);
	/* Decoder bloccato fino alla ricezione del primo pkt */
	pthread_mutex_lock(&(dec_args->syn));
	
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
/*	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL); */
	pthread_cleanup_push(rtp_clean, (void *)dec_args);

	if(dec_create(dec_args))
		pthread_exit(NULL);

	while(1){
		/*
		pthread_testcancel();
		*/
		FD_ZERO(&readset);

		for (rtp_sess=rtp_sess_head; rtp_sess; rtp_sess=rtp_sess->next){
			maxfd = max(rtp_sess->rtpfd,maxfd);
			FD_SET(rtp_sess->rtpfd, &readset);
		}
		
		select(maxfd+1, &readset, NULL, NULL, NULL);
		
		for (rtp_sess=rtp_sess_head; rtp_sess; rtp_sess=rtp_sess->next)
			if (FD_ISSET(rtp_sess->rtpfd, &readset)){
				if(first){
					first=!first;
					pthread_mutex_unlock(&(dec_args->syn));
				}
				if(rtp_recv(rtp_sess)){
					/* Waiting 20 msec for decoder ready */
					uiprintf("Waiting for decoder ready!\n");
					tv.tv_sec=0;
					tv.tv_usec=20*(1000);
					select(0, NULL, NULL, NULL, &tv);
				}
			}
	}

	pthread_cleanup_pop(1);
}
