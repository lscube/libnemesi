/* * 
 *  $Id:rtcp.c 267 2006-01-12 17:19:45Z shawill $
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

#include <nemesi/rtcp.h>

void *rtcp(void *args)
{
	rtp_session *rtp_sess_head = ((rtp_thread *) args)->rtp_sess_head;
	rtp_session *rtp_sess;
	struct rtcp_event *head = NULL;
	int maxfd = 0, ret;
	double t;
	struct timeval tv, now;

	fd_set readset;

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	// pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	pthread_cleanup_push(rtcp_clean, (void *) &rtp_sess_head);
	pthread_cleanup_push(rtcp_clean_events, (void *) &head);

	for (rtp_sess = rtp_sess_head; rtp_sess; rtp_sess = rtp_sess->next) {
		t = rtcp_interval(rtp_sess->sess_stats.members,
				  rtp_sess->sess_stats.senders,
				  rtp_sess->sess_stats.rtcp_bw,
				  rtp_sess->sess_stats.we_sent,
				  rtp_sess->sess_stats.avg_rtcp_size,
				  rtp_sess->sess_stats.initial);

		tv.tv_sec = (long int) t;
		tv.tv_usec = (long int) ((t - tv.tv_sec) * 1000000);
		gettimeofday(&now, NULL);
		timeval_add(&(rtp_sess->sess_stats.tn), &now, &tv);

		if ((head =
		     rtcp_schedule(head, rtp_sess, rtp_sess->sess_stats.tn,
				   RTCP_RR)) == NULL)
			pthread_exit(NULL);
		nms_printf(NMSML_DBG1, "RTCP: %d.%d -> %d.%d\n", now.tv_sec,
			   now.tv_usec, head->tv.tv_sec, head->tv.tv_usec);
	}

	while (1) {

		pthread_testcancel();

		FD_ZERO(&readset);

		for (rtp_sess = rtp_sess_head; rtp_sess;
		     rtp_sess = rtp_sess->next) {
			maxfd = max(rtp_sess->rtcpfd, maxfd);
			FD_SET(rtp_sess->rtcpfd, &readset);
		}

		gettimeofday(&now, NULL);
		if (timeval_subtract(&tv, &(head->tv), &now)) {
			tv.tv_sec = 0;
			tv.tv_usec = 0;
		}
		nms_printf(NMSML_DBG3,
			   "RTCP: now: %d.%d -> head:%d.%d - sleep: %d.%d\n",
			   now.tv_sec, now.tv_usec, head->tv.tv_sec,
			   head->tv.tv_usec, tv.tv_sec, tv.tv_usec);

		if (select(maxfd + 1, &readset, NULL, NULL, &tv) == 0) {
			/* timer scaduto */
			if ((head = rtcp_handle_event(head)) == NULL)
				pthread_exit(NULL);
		}

		for (rtp_sess = rtp_sess_head; rtp_sess;
		     rtp_sess = rtp_sess->next)
			if (FD_ISSET(rtp_sess->rtcpfd, &readset)) {
				if ((ret = rtcp_recv(rtp_sess)) < 0)
					pthread_exit(NULL);
			}
	}

	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
}
