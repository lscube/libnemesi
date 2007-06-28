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

void rtp_clean(void *args)
{
    rtp_thread *rtp_th = (rtp_thread *) args;
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
