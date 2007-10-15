/* * 
 * This file is part of libnemesi
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * libnemesi is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libnemesi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libnemesi; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *  
 * */

/** @file rtcp.c
 * This file contains the RTCP main loop function and functions to
 * create and run an RTCP loop for an RTP Thread.
 */

#include <nemesi/rtcp.h>

/**
 * RTCP Layer clean up is demanded to RTP layer (rtp_clean)
 * This function actually does nothing
 * 
 * @param args The rtp_thread for which to clean up the RTCP layer
 */
static void rtcp_clean(void *args)
{
    /*
    rtp_session *rtp_sess_head = (*(rtp_session **) args);
    rtp_session *rtp_sess;
    rtp_ssrc *stm_src;

    for (rtp_sess = rtp_sess_head; rtp_sess; rtp_sess = rtp_sess->next)
        for (stm_src = rtp_sess->ssrc_queue; stm_src;
             stm_src = stm_src->next)
            if (stm_src->rtcptofd > 0)
                close(stm_src->rtcptofd);
    */
    nms_printf(NMSML_DBG1, "RTCP Thread R.I.P.\n");
}

/**
 * The RTCP thread main loop, continuously calls rctp_recv every time there is data available
 * or handles pending events if no data is available
 *
 * @param args The rtp_thread for which to loop.
 */
static void *rtcp(void *args)
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
            maxfd = max(rtp_sess->transport.RTCP.sock.fd, maxfd);
            FD_SET(rtp_sess->transport.RTCP.sock.fd, &readset);
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
            if (FD_ISSET(rtp_sess->transport.RTCP.sock.fd, &readset)) {
                if ((ret = rtcp_recv(rtp_sess)) < 0)
                    pthread_exit(NULL);
            }
    }

    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
}

/**
 * Given an rtp_thread binds an RTCP main loop to it
 *
 * @param rtp_th The newly allocated and initialized rtp thread
 *
 * @return 0 if everything was ok, 1 otherwise
 */
int rtcp_thread_create(rtp_thread * rtp_th)
{
    int n;
    pthread_attr_t rtcp_attr;

    pthread_attr_init(&rtcp_attr);
    if (pthread_attr_setdetachstate(&rtcp_attr, PTHREAD_CREATE_JOINABLE) !=
        0)
        return nms_printf(NMSML_FATAL,
                  "Cannot set RTCP Thread attributes!\n");

    if ((n =
         pthread_create(&rtp_th->rtcp_tid, &rtcp_attr, &rtcp,
                (void *) rtp_th)) > 0)
        return nms_printf(NMSML_FATAL, "%s\n", strerror(n));

    return 0;
}

