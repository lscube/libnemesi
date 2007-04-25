/* * 
 *  $Id:playing_state.c 267 2006-01-12 17:19:45Z shawill $
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *      
 *      Giampaolo "mancho" Mancini - manchoz@inwind.it
 *    Francesco "shawill" Varano - shawill@infinto.it
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

#include <nemesi/rtsp.h>
#include <nemesi/methods.h>

int playing_state(rtsp_thread * rtsp_th, short event)
{
    switch (event) {
    case RTSP_PAUSE_RESPONSE:
        if (handle_pause_response(rtsp_th))
            return 1;
        // if (get_curr_sess(rtsp_th, NULL, NULL)) {
        if (!get_curr_sess(GCS_NXT_SESS)) {
            /* Nessun altra PLAY da inviare */
            rtsp_th->status = READY;
            rtsp_unbusy(rtsp_th);
            // rtsp_th->busy = 0;
            nms_printf(NMSML_NORM, "----- Play paused -----\n");
            /* Inizializza a NULL le variabili statiche interne */
            // get_curr_sess(NULL, NULL, NULL);
            get_curr_sess(GCS_UNINIT);
            break;
        }
        if (send_pause_request(rtsp_th, ""))
            return 1;
        break;
    case RTSP_CLOSE_RESPONSE:
        if (handle_teardown_response(rtsp_th))
            return 1;
        // if (get_curr_sess(rtsp_th, NULL, NULL)) {
        if (!get_curr_sess(GCS_NXT_MED)) {
            /* Nessun altra TEARDOWN da inviare */
            rtsp_th->status = INIT;
            rtsp_reinit(rtsp_th);
            rtsp_unbusy(rtsp_th);
            // rtsp_th->busy = 0;
            nms_printf(NMSML_NORM,
                   "----- All Connections closed -----\n");
            /* Inizializza a NULL le variabili statiche interne */
            // get_curr_sess(NULL, NULL, NULL);
            get_curr_sess(GCS_UNINIT);
            break;
        }
        if (send_teardown_request(rtsp_th))
            return 1;
        break;
    default:
        nms_printf(NMSML_ERR,
               "Could not handle method in PLAYING state\n");
        return 1;
        break;

    }
    return 0;
}
