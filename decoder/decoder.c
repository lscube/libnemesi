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

/* FV: utilizzo scheduler basato sui timestamp */
/* commentare la definizione per utilizzare lo */
/* scheduler basato su FAST CYCLES.            */
#define TS_SCHEDULE

#include <nemesi/decoder.h>
#ifndef ENABLE_DECODER_2

#include <nemesi/rtpptdefs.h>
#include <nemesi/preferences.h>
#include <nemesi/audio_format.h>

#ifndef TS_SCHEDULE
#define GRAIN 20
#endif

#define SKIP 4

pthread_t decoder_tid; // decoder thread ID
int (*decoders[128])(char *, int, nms_output *);

void *decoder(void *args)
{
	rtp_thread *rtp_th=(rtp_thread *)args;
	rtp_session *rtp_sess_head; // =rtp_th->rtp_sess_head;
	rtp_session *rtp_sess;
	struct timeval startime;
	struct timeval tvsleep;
	struct timeval tvstart, tvstop;
	struct timeval tv_elapsed;
	// struct timeval tv_sys_buff;
	double ts_elapsed;
#ifdef TS_SCHEDULE
	struct timeval tv_min_next;
	double ts_min_next = 0, ts_next;
#else // utilizzo lo scheduler basato su FAST CYCLES
	struct timeval tvcheck;
	struct timeval tvdiff; 
	struct timeval tvsel, tvbody;
	long int select_usec, body_usec, diff_usec, offset_usec=0;
	unsigned short cycles=0;/*AUDIO_SYS_BUFF;*/
#endif // TS_SCHEDULE
	char buffering_audio=1;
	float audio_sysbuff=0;
	float video_sysbuff=0;
	rtp_ssrc *stm_src;
	rtp_pkt *pkt;
	int len=0;
	char output_pref[PREF_MAX_VALUE_LEN];

	/* by sbiro: abilita la cancellazione del thread corrente */
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	
	/* by sbiro: rende possibile la cancellazione del thread corrente in qualunque punto della sua esecuzione */
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	/* pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL); */

	/* by sbiro: fa s� che la funzione "dec_clean" sia chiamata a gestire l'evento "cancellazione del thread corrente" */
	pthread_cleanup_push(dec_clean, (void *)nms_outc /*audio_buffer */);
	
	// tvdiff.tv_sec=tvsleep.tv_sec=rtp_th->startime.tv_sec;
	// tvdiff.tv_usec=tvsleep.tv_usec=rtp_th->startime.tv_usec;
//while(1) {
	pthread_mutex_lock(&(rtp_th->syn));
	pthread_mutex_unlock(&(rtp_th->syn));
	rtp_sess_head=rtp_th->rtp_sess_head;

	/* FV: startime ora assume il significato di istante di partenza del decoder */
	// gettimeofday(&(rtp_th->startime), NULL);
	gettimeofday(&startime, NULL);

	while(!pthread_mutex_trylock(&(rtp_th->syn))) {
//		pthread_mutex_lock(&(rtp_th->syn));
		gettimeofday(&tvstart, NULL);
		
/*	
#ifndef TS_SCHEDULE
		nms_printf(NMSML_DBG3, "sum: %7ld - select: %7ld - body: %7ld - diff: %7ld - offset: %7ld - sleep %7ld - cycles: %3hu \n",\
				select_usec + body_usec, select_usec, body_usec, diff_usec, offset_usec, tvdiff.tv_usec, cycles);
#endif // TS_SCHEDULE
*/
#ifndef TS_SCHEDULE
		do {
#endif // TS_SCHEDULE
#if 0
			/*by sbiro: ciclo per ogni sessione rtp*/
			for (rtp_sess=rtp_sess_head; rtp_sess; rtp_sess=rtp_sess->next)
			
			/*by sbiro: ciclo per ogni elemento della coda ssrc associata a una sessione rtp*/	
			for (stm_src=rtp_sess->ssrc_queue; stm_src; stm_src=stm_src->next) {
#else
			for (stm_src=rtp_active_ssrc_queue(rtp_sess_head); stm_src; stm_src=rtp_next_active_ssrc(stm_src)) {
				rtp_sess = stm_src->rtp_sess;
#endif

				if ( (pkt=rtp_get_pkt(stm_src, &len)) ) {
				/**/	
					nms_printf(NMSML_DBG3, "Version Number:%d\n", pkt->ver);
					nms_printf(NMSML_DBG3, "Payload Type:%d\n", pkt->pt);
					nms_printf(NMSML_DBG3, "Sequence Number:%d\n", ntohs(pkt->seq));
					nms_printf(NMSML_DBG3, "SSRC Number:%lu\n", ntohl(pkt->ssrc));
					nms_printf(NMSML_DBG3, "RTP Timestamp:%lu\n", ntohl(pkt->time));
				/**/	
					ts_elapsed=((double)(ntohl(pkt->time) - stm_src->ssrc_stats.firstts))/(double)rtp_sess->ptdefs[pkt->pt]->rate;
					tv_elapsed.tv_sec=(long)ts_elapsed;
					tv_elapsed.tv_usec=(long)((ts_elapsed-tv_elapsed.tv_sec)*1000000);

					   // timeval_add(&tv_elapsed, &(stm_src->ssrc_stats.firsttv), &tv_elapsed);
					// timeval_add(&tv_elapsed, &tv_elapsed, &(rtp_th->startime));
					timeval_add(&tv_elapsed, &tv_elapsed, &startime);
					   // timeval_subtract(&tv_elapsed, &tv_elapsed, &tv_sys_buff);
					
					if(
#ifndef TS_SCHEDULE
							cycles || 
#endif // TS_SCHEDULE
							timeval_subtract(NULL, &tv_elapsed, &tvstart) ) {
					
						/* istruzione con bug
						len= (stm_src->po.pobuff[stm_src->po.potail]).pktlen -\
							((uint8 *)(pkt->data)-(uint8 *)pkt) - pkt->cc - ((*(((uint8 *)pkt)+len-1)) * pkt->pad);
						*/
//						len= (stm_src->po.pobuff[stm_src->po.potail]).pktlen;
						if (len) {
							len -= ((uint8 *)(pkt->data)-(uint8 *)pkt) - pkt->cc - ((*(((uint8 *)pkt)+len-1)) * pkt->pad);
						}
						strcpy(output_pref, get_pref("output"));
						
						if ( (len != 0) && (!strcmp(output_pref, "disk")) ) {
							if (nms_outc->diskwriter)
								diskwriter( nms_outc->diskwriter, pkt->pt, ((char *)pkt->data + pkt->cc + SKIP), len - SKIP );
						} else if ((len != 0) && decoders[pkt->pt]) {
							/* controllo che vada fatta la decodifica*/
							if ( !strcmp(output_pref, "card") ) {
								nms_outc->elapsed = ts_elapsed * 1000;
								decoders[pkt->pt](((char *)pkt->data + pkt->cc), len, nms_outc);
								if (nms_outc->audio)
									nms_outc->audio->functions->control(ACTRL_GET_SYSBUF, &audio_sysbuff);
								if (nms_outc->video)
									nms_outc->video->functions->control(VCTRL_GET_SYSBUF, &video_sysbuff);

								// AUDIO
								if(buffering_audio) {
									if (audio_sysbuff > /*0.1*/ AUDIO_SYS_BUFF /*0.99*/ ) {
										buffering_audio = 0;
										// start playing audio
										nms_outc->audio->functions->resume();
									}
								}
								// VIDEO
								if((nms_outc->video) && (nms_outc->video->init) && (!nms_outc->video->tid))
									video_th_start(nms_outc);
							}
							/* XXX: not supported any more
							     else if ( !strcmp(output_pref, "diskdecoded") ) {
								decoders[pkt->pt](((char *)pkt->data + pkt->cc), len, \
										(uint8 *(*)(uint32))db_get);
								diskwriter((char *)global_disk_buffer->data, global_disk_buffer->len);
								global_disk_buffer->len = 0;
							} */
						}
/*
				 		nms_printf(NMSML_DBG2, "\rPlayout Buffer Status: %4.1f %% full - System Buffer Status: %4.1f %% full - pkt data len: %d   ",\
								(((float)((rtp_sess->bp).flcount)/(float)BP_SLOT_NUM)*100.0), audio_sysbuff*100.0, len);
*/				
/**/				
				 		nms_statusprintf(BUFFERS_STATUS, "Buffers: Net: %4.1f %% - A: %4.1f %% - V: %4.1f ",\
								(((float)((rtp_sess->bp).flcount)/(float)BP_SLOT_NUM)*100.0), audio_sysbuff*100.0, video_sysbuff*100.0);
						nms_printf(NMSML_DBG2, " - pkt len: %d\n", len);
/**/	
						rtp_rm_pkt(stm_src);

					} else
						nms_printf(NMSML_DBG3, "*** not taken:%d\n", ntohs(pkt->seq));
				}
#ifdef TS_SCHEDULE
				/* FV: controls on timestamp */
				if ( (ts_next=rtp_get_next_ts(stm_src)) >= 0 ) {
					if ( ts_min_next ) /* min between stored ts and next pkt's */
						ts_min_next = min(ts_min_next, ts_next);
					else
						ts_min_next = ts_next;
				}
#endif // TS_SCHEDULE
			}
#ifndef TS_SCHEDULE
		} while( cycles-- > 0);
		cycles=0;
#endif // TS_SCHEDULE
		
		gettimeofday(&tvstop, NULL);

#ifndef TS_SCHEDULE // FV: scheduler FAST CYCLES
		
		timeval_subtract(&tvbody, &tvstop, &tvstart);
		
		if((body_usec=tvbody.tv_sec*1000000+tvbody.tv_usec) > (GRAIN*1000-offset_usec)){
			cycles=(body_usec+offset_usec)/(GRAIN*1000);
			offset_usec=(body_usec+offset_usec)%(GRAIN*1000);
		} else {
			
			tvdiff.tv_sec=tvsleep.tv_sec=tvbody.tv_sec;
			tvdiff.tv_usec=tvsleep.tv_usec=GRAIN*1000-tvbody.tv_usec-offset_usec;
			if (tvdiff.tv_usec < 0){
				tvdiff.tv_usec+=1000000;
				tvdiff.tv_sec--;
			}
			diff_usec=tvdiff.tv_sec*1000000+tvdiff.tv_usec;
			if ( tvsleep.tv_usec > 10000 )
				select(0, NULL, NULL, NULL, &tvsleep);
			gettimeofday(&tvcheck, NULL);

			timeval_subtract(&tvsel, &tvcheck, &tvstop);
			if((select_usec=tvsel.tv_sec*1000000+tvsel.tv_usec) > diff_usec){
				cycles=(select_usec-diff_usec)/(GRAIN*1000);
			}
				offset_usec=(select_usec-diff_usec)%(GRAIN*1000);
		}
		if ( !strcmp(get_pref("output"), "card") ) {
			// cycles+=get_sys_buff();
			// VF: new audio sysbuff len request
			nms_outc->audio->functions->control(ACTRL_GET_SYSBUF, &audio_sysbuff);
			if (audio_sysbuff < MIN_AUDIO_SYS_BUFF)
				cycles += 2;
		}
#else // TS_SCHEDULE DEFINED --> utilizzo scheduler basato sui Timestamp
		if ( ts_min_next ) { // esiste un pacchetto successivo?
			tv_min_next.tv_sec=(long)ts_min_next;
			tv_min_next.tv_usec=(long)((ts_min_next-tv_min_next.tv_sec)*1000000);

			// timeval_add(&tv_min_next, &tv_min_next, &(rtp_th->startime));
			timeval_add(&tv_min_next, &tv_min_next, &startime);
			   // timeval_subtract(&tv_min_next, &tv_min_next, &tv_sys_buff);

			// nms_printf(NMSML_DBG3, "tv_min_next: %ld.%ld tv_stop: %ld.%ld\n", tv_min_next.tv_sec, tv_min_next.tv_usec, tvstop.tv_sec, tvstop.tv_usec);
			if ( !timeval_subtract(&tvsleep, &tv_min_next, &tvstop) && (tvsleep.tv_usec > 10000) ) {
				nms_printf(NMSML_DBG3, "\n\tWe sleep for: %lds e %ldus\n", tvsleep.tv_sec, tvsleep.tv_usec);
				select(0, NULL, NULL, NULL, &tvsleep);
			}
			ts_min_next = 0;

		} else { // Buffer di Rete vuoto => dormiamo un po'
			dec_idle();
/**/
			/*
			nms_outc->audio->functions->control(ACTRL_GET_SYSBUF, &audio_sysbuff);
			nms_outc->video->functions->control(VCTRL_GET_SYSBUF, &video_sysbuff);
			nms_statusprintf(BUFFERS_STATUS, "Buffers: Net: %4.1f %% - A: %4.1f %% - V: %4.1f ",\
					(((float)((rtp_sess_head->bp).flcount)/(float)BP_SLOT_NUM)*100.0), audio_sysbuff*100.0, video_sysbuff*100.0);
			*/
			/*
	 		nms_printf(NMSML_DBG2, "\rPlayout Buffer Status: %4.1f %% full - System Buffer Status: %4.1f %% full - no pkt   ",\
					(((float)((rtp_sess_head->bp).flcount)/(float)BP_SLOT_NUM)*100.0), audio_sysbuff*100.0);
			*/
			len = 0;
/**/				
		}

#endif // TS_SCHEDULE

		pthread_mutex_unlock(&(rtp_th->syn));
	}
	pthread_cleanup_pop(1);
	
	return NULL;
}
#endif // ENABLE_DECODER_2
