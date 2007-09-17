/* * 
 * This file is part of NetEmbryo
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * NetEmbryo is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * NetEmbryo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with NetEmbryo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *  
 * */

#include <nemesi/rtp.h>
#include <nemesi/comm.h>

#define PO_BUFF_SIZE_SEC 0
#define PO_BUFF_SIZE_MSEC 700

/**
 * Given an rtp_thread deallocates the binded payload parsers, the transport informations
 * and every session of the thread.
 *
 * @param thrd The thread to clean
 */
static void rtp_clean(void * thrd)
{
    rtp_thread *rtp_th = (rtp_thread *) thrd;
    rtp_session *rtp_sess = rtp_th->rtp_sess_head;
    rtp_session *prev_rtp_sess;
    rtp_ssrc *csrc, *psrc;
    struct rtp_conflict *conf, *pconf;
    rtp_fmts_list *fmtlist, *pfmtlist;
    int i;

    nms_printf(NMSML_DBG1, "RTP Thread is dying suicide!\n");
//      pthread_mutex_lock(&rtp_th->syn);
//      pthread_mutex_trylock(&rtp_th->syn);

    while (rtp_sess != NULL) {
        close(rtp_sess->transport.RTP.sock.fd);
        close(rtp_sess->transport.RTCP.sock.fd);

        csrc = rtp_sess->ssrc_queue;

        while (csrc != NULL) {
            psrc = csrc;
            csrc = csrc->next;
            for (i = 0; i < 9; i++)
                free(((char **) (&(psrc->ssrc_sdes)))[i]);
            free(psrc->rtp_from.addr);
            free(psrc->rtcp_from.addr);
            free(psrc->rtcp_to.addr);
            for (i = 0; i < 128; i++)
                if (rtp_sess->parsers_uninits[i])
                    rtp_sess->parsers_uninits[i] (psrc, i);
            free(psrc);
        }
        bpkill(&(rtp_sess->bp));

        // transport allocs
        free((rtp_sess->transport).spec);

        conf = rtp_sess->conf_queue;
        while (conf) {
            pconf = conf;
            conf = conf->next;
            free(pconf->transaddr.addr);
            free(pconf);
        }
        // announced rtp payload list
        for (fmtlist = rtp_sess->announced_fmts; fmtlist;
             pfmtlist = fmtlist, fmtlist =
             fmtlist->next, free(pfmtlist));
        // rtp payload types definitions attributes
        for (i = 0; i < 128; i++)
            if (rtp_sess->ptdefs[i])
                free(rtp_sess->ptdefs[i]->attrs.data);
        // rtp payload types dynamic definitions
        for (i = 96; i < 128; free(rtp_sess->ptdefs[i++]));

        prev_rtp_sess = rtp_sess;
        rtp_sess = rtp_sess->next;
        free(prev_rtp_sess);
    }
    rtp_th->rtp_sess_head = NULL;

//      pthread_mutex_unlock(&rtp_th->syn);

    nms_printf(NMSML_DBG1, "RTP Thread R.I.P.\n");
}

/**
 * The RTP thread main loop, continuously calls rtp_recv every time there is data available.
 *
 * @param args The rtp_thread for which to loop.
 */
static void *rtp(void *args)
{
    rtp_session *rtp_sess_head = ((rtp_thread *) args)->rtp_sess_head;
    pthread_mutex_t *syn = &((rtp_thread *) args)->syn;
    rtp_session *rtp_sess;
    struct timespec ts;
    int maxfd = 0;

    fd_set readset;
    char buffering = 1;

    for (rtp_sess = rtp_sess_head; rtp_sess; rtp_sess = rtp_sess->next)
        bpinit(&(rtp_sess->bp));

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
/*    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL); */
    pthread_cleanup_push(rtp_clean, (void *) args);

    /* Playout Buffer Size */
    /*
       dec_args->startime.tv_sec=0;
       dec_args->startime.tv_usec=700*(1000);
     */
    // dec_args->startime.tv_sec=PO_BUFF_SIZE_SEC;
    // dec_args->startime.tv_usec=PO_BUFF_SIZE_MSEC*(1000);
    /* 500 msec */

    while (1) {
        FD_ZERO(&readset);

        for (rtp_sess = rtp_sess_head; rtp_sess;
             rtp_sess = rtp_sess->next) {
            maxfd = max(rtp_sess->transport.RTP.sock.fd, maxfd);
            FD_SET(rtp_sess->transport.RTP.sock.fd, &readset);
        }

        select(maxfd + 1, &readset, NULL, NULL, NULL);

        for (rtp_sess = rtp_sess_head; rtp_sess;
             rtp_sess = rtp_sess->next)
            if (FD_ISSET(rtp_sess->transport.RTP.sock.fd, &readset)) {
                if (buffering) {
                    if (rtp_sess->bp.flcount >
                        BP_SLOT_NUM / 2) {
                        pthread_mutex_unlock(syn);
                        buffering = 0;
                    } else {    // TODO: buffering based on rtp jitter
                        nms_printf(NMSML_DBG1,
                               "\rBuffering (%d)%\t",
                               (100 *
                                rtp_sess->bp.
                                flcount) /
                               (BP_SLOT_NUM / 2));
                    }
                }
                if (rtp_recv(rtp_sess)) {
                    /* Waiting 20 msec for decoder ready */
                    nms_printf(NMSML_DBG1,
                           "Waiting for decoder ready!\n");
                    ts.tv_sec = 0;
                    ts.tv_nsec = 20 * (1000);
                    nanosleep(&ts, NULL);
                }
            }
    }

    pthread_cleanup_pop(1);
}

/**
 * Allocates a new rtp_thread and initializes the parsers for rtp packets
 *
 * @return A valid rtp_thread if it was possible to allocate it or NULL
 */
rtp_thread *rtp_init(void)
{
    rtp_thread *rtp_th;

    if (!(rtp_th = (rtp_thread *) calloc(1, sizeof(rtp_thread)))) {
        nms_printf(NMSML_FATAL, "Could not alloc memory!\n");
        return NULL;
    }

    rtp_parsers_init();

    if (pthread_mutex_init(&(rtp_th->syn), NULL)) {
        free(rtp_th);
        return NULL;
    }
    /* Decoder blocked 'till buffering is complete */
    pthread_mutex_lock(&(rtp_th->syn));

    return rtp_th;
}

/**
 * Given an rtp_thread registers the main loop for the thread, binds the specific parsers
 * for the payloads announced for the thread.
 *
 * @param rtp_th The newly allocated and initialized rtp thread
 *
 * @return 0 if everything was ok, 1 otherwise
 */
int rtp_thread_create(rtp_thread * rtp_th)
{
    int err;
    pthread_attr_t rtp_attr;
    rtp_session *rtp_sess;
    rtp_fmts_list *fmt;

    pthread_attr_init(&rtp_attr);
    if (pthread_attr_setdetachstate(&rtp_attr, PTHREAD_CREATE_JOINABLE) != 0)
        return nms_printf(NMSML_FATAL,
                  "Cannot set RTP Thread attributes (detach state)\n");

    if ((err = pthread_create(&rtp_th->rtp_tid, 
                              &rtp_attr, &rtp, (void *) rtp_th)) > 0)
        return nms_printf(NMSML_FATAL, "%s\n", strerror(err));

    for (rtp_sess = rtp_th->rtp_sess_head; rtp_sess;
         rtp_sess = rtp_sess->next) {
        for (fmt = rtp_sess->announced_fmts; fmt; fmt = fmt->next) {
            if (rtp_sess->parsers_inits[fmt->pt]) {
                err = rtp_sess->parsers_inits[fmt->pt] (rtp_sess, fmt->pt);
                if (err)
                    return nms_printf(NMSML_FATAL,
                            "Cannot init the parser for pt %d\n", fmt->pt);
            }
        }
    }

    rtp_th->run = 1;
    return 0;
}
