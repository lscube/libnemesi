/* * 
 *  $Id:get_curr_sess.c 267 2006-01-12 17:19:45Z shawill $
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

#include <stdarg.h>

#include <nemesi/rtsp.h>

void *get_curr_sess(int cmd, ...)
{
	va_list ap;
	rtsp_thread *rtsp_th;
	static rtsp_session *static_sess = NULL;
	static rtsp_medium *static_med = NULL;

	switch (cmd) {
		case GCS_INIT:
			va_start(ap, cmd);
			rtsp_th = va_arg(ap, rtsp_thread *);
			static_sess = rtsp_th->rtsp_queue;
			static_med = static_sess->media_queue;
			va_end(ap);
			break;
		case GCS_NXT_SESS:
			if ( static_sess )
				static_sess = static_sess->next;
			if ( static_sess )
				static_med = static_sess->media_queue;
			else
				static_med = NULL;
		case GCS_CUR_SESS:
				return static_sess;
			break;
		case GCS_NXT_MED:
			/* sessione corrente, prossimo media */
			if ( static_med )
				static_med = static_med->next;
			/* prossima sessione, primo media */
			if ( (!static_med) && static_sess ) {
				static_sess = static_sess->next;
				if (static_sess)
					static_med = static_sess->media_queue;
			}
		case GCS_CUR_MED:
			return static_med;
			break;
		case GCS_UNINIT:
			static_sess = NULL;
			static_med = NULL;
			break;
		default:
			break;
	}
	
	return NULL;
}
// XXX: OLD get_curr_session FUNCTION
#if 0 
#include <nemesi/rtsp.h>

/**
* Imposta e/o restituisce i puntatori alla sessione e al medium RTSP correnti.
* Deve essere chiamata almeno una volta, con il primo argomento non nullo,
* per inizializzare le variabili statiche interne alla prima sessione e al primo
* medium RTSP. Se il secondo e/o il terzo argomento sono diversi da NULL, dopo la
* chiamata essi contengono i puntatori ai puntatori alla sessione e/o al medium
* RTSP correnti.
* Se il primo argomento e' diverso da NULL, le chiamate successive alla prima
* aggiornano le variabili statiche interne ai puntatori alla prossima sessione
* e al prossimo medium RTSP.
* Altrimenti, se il primo argomento e' NULL, le variabili statiche interne non
* vengono aggiornate.
* Infine, se tutti gli argomenti sono NULL le variabili statiche interne vengono
* reinizializzate a NULL.
* @param rstp_th Il puntatore al Thread RTSP corrente.
* @param rtsp_sess Il puntatore al puntatore alla sessione RTSP.
* @param rtsp_med Il puntatore al puntatore al medium RTSP.
* @return 1 in caso di errore, 0 altrimenti. Se il valore di ritorno e' 1 
* quando il primo argomento e' NULL, allora la funzione non e' stata mai chiamata
* e le variabili statiche non sono mai state inizializzate. Se il valore di ritorno
* e' 1 quando il primo argomento e' diverso da NULL, allora non ci sono piu'
* sessioni ne' media RTSP da scandire.
* */

int get_curr_sess(rtsp_thread *rtsp_th, rtsp_session **rtsp_sess, rtsp_medium **rtsp_med)
{
	static rtsp_session *static_sess = NULL;
	static rtsp_medium *static_med = NULL;

	if ((rtsp_th == NULL) && (rtsp_sess == NULL) && (rtsp_med == NULL)) {
		static_sess = NULL;
		static_med = NULL;
		return 0;
	}

	if (rtsp_th != NULL) {

		/* Prima chiamata: prima sessione, primo media */
		if ((static_sess == NULL) && (static_med == NULL)) {
			static_sess = rtsp_th->rtsp_queue;
			static_med = static_sess->media_queue;
			return 0;
		} else {	/* Chiamate successive */
			/* sessione corrente, prossimo media */
			if (static_med->next != NULL)
				static_med = static_med->next;
			/* prossima sessione, primo media */
			else if (static_sess->next != NULL) {
				static_sess = static_sess->next;
				static_med = static_sess->media_queue;
			} else
				return 1;
		}
	}
	if (rtsp_sess != NULL)
		if ((*rtsp_sess = static_sess) == NULL)
			return 1;
	if (rtsp_med != NULL)
		if ((*rtsp_med = static_med) == NULL)
			return 1;

	return 0;
}
#endif // if 0
