/* * 
 *  
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

int get_transport_str_tcp(rtp_session * rtp_sess, char * tkna, char * tknb) {
    char str[256];
    int value;
    uint8 ilvd;
    do {
        if ((tkna = strstrcase(tknb, "interleaved"))) {
            
            for (; (*tkna == ' ') || (*tkna != '='); tkna++);
            for (tknb = tkna++; (*tknb == ' ') || (*tknb != '-');
                 tknb++);

            strncpy(str, tkna, tknb - tkna);
            str[tknb - tkna] = '\0';
            if ((value = atoi(str)) > 255) {
                nms_printf(NMSML_ERR,
                       "Interleaved channel too high!\n");
                return 1;
            }
            ilvd = (uint8) value;
            rtp_transport_set(rtp_sess, RTP_TRANSPORT_ILVDRTP,
                      &ilvd);

            for (tknb++; (*tknb == ' '); tknb++);

            for (tkna = tknb; (*tkna != '\0') && (*tkna != '\r')
                 && (*tkna != '\n'); tkna++);
            strncpy(str, tknb, tkna - tknb);
            str[tkna++ - tknb] = '\0';
            if ((value = atoi(str)) > 255) {
                nms_printf(NMSML_ERR,
                       "Interleaved channel too high!\n");
                return 1;
            }
            ilvd = (uint8) value;
            rtp_transport_set(rtp_sess, RTP_TRANSPORT_ILVDRTCP,
                      &ilvd);

            continue;
        }
        if ((tkna = strstrcase(tknb, "ssrc"))) {
            uint32 ssrc;

            for (; (*tkna == ' ') || (*tkna != '='); tkna++);

            for (tknb = tkna++; (*tknb != '\0') && (*tknb != '\r')
                 && (*tknb != '\n'); tknb++);
            strncpy(str, tkna, tknb - tkna);
            str[tknb++ - tkna] = '\0';

            ssrc = strtoul(str, NULL, 10);
            rtp_transport_set(rtp_sess, RTP_TRANSPORT_SSRC, &ssrc);

            continue;
        }
    } while ((tknb = strtok(NULL, ";")));
    return 0;
}
