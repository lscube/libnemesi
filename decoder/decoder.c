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
#include <nemesi/rtpptdefs.h>
#include <nemesi/preferences.h>
#include <nemesi/output.h>
#include <nemesi/audio_format.h>

#ifndef TS_SCHEDULE
#define GRAIN 20
#endif

#define SKIP 4

void *decoder(void *args)
{
	struct Dec_args *dec_args=(struct Dec_args *)args;
	struct RTP_Session *rtp_sess_head=dec_args->rtp_sess_head;
	struct RTP_Session *rtp_sess;
	struct timeval tvsleep, tvdiff; 
	struct timeval tvstart, tvstop;
	struct timeval tv_elapsed;
	// struct timeval tv_sys_buff;
	double ts_elapsed;
#ifdef TS_SCHEDULE
	struct timeval tv_min_next;
	double ts_min_next = 0;
#else // utilizzo lo scheduler basato su FAST CYCLES
	struct timeval tvcheck;
	struct timeval tvsel, tvbody;
	long int select_usec, body_usec, diff_usec, offset_usec=0;
	unsigned short cycles=0;/*AUDIO_SYS_BUFF;*/
#endif // TS_SCHEDULE
	char buffering_audio=1;
	float audio_sysbuff;
	struct Stream_Source *stm_src;
	rtp_pkt *pkt;
	int len=0;
	char output_pref[PREF_MAX_VALUE_LEN];

	/* by sbiro: abilita la cancellazione del thread corrente */
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	
	/* by sbiro: rende possibile la cancellazione del thread corrente in qualunque punto della sua esecuzione */
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	/* pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL); */

	/* by sbiro: fa sí che la funzione "dec_clean" sia chiamata a gestire l'evento "cancellazione del thread corrente" */
	pthread_cleanup_push(dec_clean, (void *)nmsoutc /*audio_buffer */);
	
	tvdiff.tv_sec=tvsleep.tv_sec=dec_args->startime.tv_sec;
	tvdiff.tv_usec=tvsleep.tv_usec=dec_args->startime.tv_usec;

	pthread_mutex_lock(&(dec_args->syn));
	pthread_mutex_unlock(&(dec_args->syn));

	/* FV: startime ora assume il significato di istante di partenza del decoder */
	gettimeofday(&(dec_args->startime), NULL);

	while(1) {

		gettimeofday(&tvstart, NULL);
		
/*	
#ifndef TS_SCHEDULE
		nmsprintf(3, "sum: %7ld - select: %7ld - body: %7ld - diff: %7ld - offset: %7ld - sleep %7ld - cycles: %3hu \n",\
				select_usec + body_usec, select_usec, body_usec, diff_usec, offset_usec, tvdiff.tv_usec, cycles);
#endif // TS_SCHEDULE
*/
#ifndef TS_SCHEDULE
		do { 
#endif // TS_SCHEDULE
			/*by sbiro: ciclo per ogni sessione rtp*/
			for (rtp_sess=rtp_sess_head; rtp_sess; rtp_sess=rtp_sess->next)
			
			/*by sbiro: ciclo per ogni elemento della coda ssrc associata a una sessione rtp*/	
			for (stm_src=rtp_sess->ssrc_queue; stm_src; stm_src=stm_src->next){
				if(stm_src->po.potail >= 0){

					pkt=(rtp_pkt *)(*(stm_src->po.bufferpool)+stm_src->po.potail);
				/*	
					nmsprintf(3, "Version Number:%d\n", pkt->ver);
					nmsprintf(3, "Payload Type:%d\n", pkt->pt);
					nmsprintf(3, "Sequence Number:%d\n", ntohs(pkt->seq));
					nmsprintf(3, "SSRC Number:%lu\n", ntohl(pkt->ssrc));
					nmsprintf(3, "RTP Timestamp:%lu\n", ntohl(pkt->time));
				*/	
					ts_elapsed=((double)(ntohl(pkt->time) - stm_src->ssrc_stats.firstts))/(double)rtp_pt_defs[pkt->pt].rate;
					tv_elapsed.tv_sec=(long)ts_elapsed;
					tv_elapsed.tv_usec=(long)((ts_elapsed-tv_elapsed.tv_sec)*1000000);

					// timeval_add(&tv_elapsed, &(stm_src->ssrc_stats.firsttv), &tv_elapsed);
					timeval_add(&tv_elapsed, &tv_elapsed, &(dec_args->startime));
					// timeval_subtract(&tv_elapsed, &tv_elapsed, &tv_sys_buff);
					
					if(
#ifndef TS_SCHEDULE
							cycles || 
#endif // TS_SCHEDULE
							timeval_subtract(NULL, &tv_elapsed, &tvstart) ){
					
						len= (stm_src->po.pobuff[stm_src->po.potail]).pktlen -\
							((uint8 *)(pkt->data)-(uint8 *)pkt) - pkt->cc - ((*(((uint8 *)pkt)+len-1)) * pkt->pad);
						strcpy(output_pref, get_pref("output"));
						
						if ( (len != 0) && (!strcmp(output_pref, "disk")) )
							diskwriter( nmsoutc->diskwriter, pkt->pt, ((char *)pkt->data + pkt->cc + SKIP), len - SKIP );
						else if ((len != 0) && (decoders[pkt->pt] != NULL)) {
							/* controllo che vada fatta la decodifica*/
							if ( !strcmp(output_pref, "card") ) {
								nmsoutc->elapsed = ts_elapsed * 1000;
								decoders[pkt->pt](((char *)pkt->data + pkt->cc), len, nmsoutc);
								nmsoutc->audio->functions->control(ACTRL_GET_SYSBUF, &audio_sysbuff);

								// AUDIO
								if(buffering_audio) {
									if (audio_sysbuff > /*0.1*/ AUDIO_SYS_BUFF /*0.99*/ ) {
										buffering_audio = 0;
										// start playing audio
										nmsoutc->audio->functions->resume();
									}
								}
								// VIDEO
								if((nmsoutc->video->init) && (!nmsoutc->video->tid)) {
									video_th_start(nmsoutc->video);
									// buffering_audio = 0;
									// nmsoutc->audio->functions->resume();
								}
							}
							/* XXX: not supported any more
							     else if ( !strcmp(output_pref, "diskdecoded") ) {
								decoders[pkt->pt](((char *)pkt->data + pkt->cc), len, \
										(uint8 *(*)(uint32))db_get);
								diskwriter((char *)global_disk_buffer->data, global_disk_buffer->len);
								global_disk_buffer->len = 0;
							} */
						}
/**/
				 		nmsprintf(2, "\rPlayout Buffer Status: %4.1f %% full - System Buffer Status: %4.1f %% full - pkt data len: %d   ",\
								(((float)((rtp_sess->bp).flcount)/(float)BP_SLOT_NUM)*100.0), audio_sysbuff*100.0, len);
/**/				
						bprmv(&(rtp_sess->bp), &(stm_src->po), stm_src->po.potail);

					}
				}
#ifdef TS_SCHEDULE
				/* FV: controlli sul timestamp */
				if(stm_src->po.potail >= 0){
					pkt=(rtp_pkt *)(*(stm_src->po.bufferpool)+stm_src->po.potail); // pacchetto successivo
					if ( !ts_min_next ) {
						ts_min_next = ((double)(ntohl(pkt->time) - stm_src->ssrc_stats.firstts))/(double)rtp_pt_defs[pkt->pt].rate;
						// fprintf(stderr, "\nNuovo min: %3.2f\n", ts_min_next);
					} else	/* minimo tra il ts salvato e quello del prossimo pacchetto */
						ts_min_next = min(ts_min_next, \
								((double)(ntohl(pkt->time) - stm_src->ssrc_stats.firstts))/(double)rtp_pt_defs[pkt->pt].rate);
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
			nmsoutc->audio->functions->control(ACTRL_GET_SYSBUF, &audio_sysbuff);
			if (audio_sysbuff < MIN_AUDIO_SYS_BUFF)
				cycles += 2;
		}
#else // TS_SCHEDULE DEFINED --> utilizzo scheduler basato sui Timestamp
		if ( ts_min_next ) { // esiste un pacchetto successivo?
			tv_min_next.tv_sec=(long)ts_min_next;
			tv_min_next.tv_usec=(long)((ts_min_next-tv_min_next.tv_sec)*1000000);

			timeval_add(&tv_min_next, &tv_min_next, &(dec_args->startime));
			// timeval_subtract(&tv_min_next, &tv_min_next, &tv_sys_buff);

			if ( !timeval_subtract(&tvsleep, &tv_min_next, &tvstop) && (tvsleep.tv_usec > 10000) ) {
				// fprintf(stderr, "\n\tDormiamo per: %lds e %ldus\n", tvsleep.tv_sec, tvsleep.tv_usec);
				select(0, NULL, NULL, NULL, &tvsleep);
			}
			ts_min_next = 0;

		} else { // Buffer di Rete vuoto => dormiamo un po'
			tvsleep.tv_sec = 0;
			tvsleep.tv_usec = 1000;
			select(0, NULL, NULL, NULL, &tvsleep);
/**/
			nmsoutc->audio->functions->control(ACTRL_GET_SYSBUF, &audio_sysbuff);
	 		nmsprintf(2, "\rPlayout Buffer Status: %4.1f %% full - System Buffer Status: %4.1f %% full - no pkt   ",\
					(((float)((rtp_sess_head->bp).flcount)/(float)BP_SLOT_NUM)*100.0), audio_sysbuff*100.0);
			len = 0;
/**/				
		}

#endif // TS_SCHEDULE

		
	}
	
	pthread_cleanup_pop(1);
}
