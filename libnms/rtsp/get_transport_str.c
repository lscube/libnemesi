/* * 
 *  $Id:get_transport_str.c 267 2006-01-12 17:19:45Z shawill $
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

#include <nemesi/rtsp.h>

int get_transport_str(struct rtp_session *rtp_sess, char *buff)
{
	char *tkna, *tknb;
	char str[256];
	// char addr[128];		/* Unix domain is largest */
	
	memset(str, 0, sizeof(str));
	
	for(tknb=strtok(buff,";"); (*tknb == ' ') || (*tknb == ':'); tknb++);
	
	do {
		if ((tkna=strstrcase(tknb, "server_port")) || ((tkna=strstrcase(tknb, "port")) && !strncmp(tknb, "port", 4))){
			in_port_t port;

			for(; (*tkna == ' ') || (*tkna != '='); tkna++);
			for(tknb=tkna++; (*tknb == ' ') || (*tknb != '-'); tknb++);

			strncpy(str, tkna, tknb-tkna);
			str[tknb-tkna]='\0';
			port=atoi(str);
			rtp_transport_set(rtp_sess, RTP_TRANSPORT_SRVRTP, &port);
			
			for(tknb++; (*tknb == ' '); tknb++);

			for(tkna=tknb; (*tkna != '\0') && (*tkna != '\r') && (*tkna != '\n'); tkna++);
			strncpy(str, tknb, tkna-tknb);
			str[tkna++-tknb]='\0';
			port=atoi(str);
			rtp_transport_set(rtp_sess, RTP_TRANSPORT_SRVRTCP, &port);

			continue;
		}
		if ((tkna=strstrcase(tknb, "source"))){
			for(; (*tkna == ' ') || (*tkna != '='); tkna++);
			
			for(tknb=tkna++; (*tknb != '\0') && (*tknb != '\r') && (*tknb != '\n'); tknb++);
			strncpy(str, tkna, tknb-tkna);
			str[tknb++-tkna]='\0';

			if (rtp_transport_set(rtp_sess, RTP_TRANSPORT_SRCADDRSTR, str)) {
				nms_printf(NMSML_ERR, "Source IP Address not valid!\n");
				return 1;
			}
			continue;
		}
		if ((tkna=strstrcase(tknb, "destination"))){
			for(; (*tkna == ' ') || (*tkna != '='); tkna++);
			
			for(tknb=tkna++; (*tknb != '\0') && (*tknb != '\r') && (*tknb != '\n'); tknb++);
			strncpy(str, tkna, tknb-tkna);
			str[tknb++-tkna]='\0';
			
			if (rtp_transport_set(rtp_sess, RTP_TRANSPORT_DSTADDRSTR, str)) {
				nms_printf(NMSML_ERR, "Destination IP Address not valid!\n");
				return 1;
			}
			continue;
		}
		if ((tkna=strstrcase(tknb, "ssrc"))){
			uint32 ssrc;

			for(; (*tkna == ' ') || (*tkna != '='); tkna++);
			
			for(tknb=tkna++; (*tknb != '\0') && (*tknb != '\r') && (*tknb != '\n'); tknb++);
			strncpy(str, tkna, tknb-tkna);
			str[tknb++-tkna]='\0';
			
			ssrc=strtoul(str, NULL, 10);
			rtp_transport_set(rtp_sess, RTP_TRANSPORT_SSRC, &ssrc);
			
			continue;
		}
		if ((tkna=strstrcase(tknb, "ttl"))){
			int ttl;

			for(; (*tkna == ' ') || (*tkna != '='); tkna++);
			
			for(tknb=tkna++; (*tknb != '\0') && (*tknb != '\r') && (*tknb != '\n'); tknb++);
			strncpy(str, tkna, tknb-tkna);
			str[tknb++-tkna]='\0';
			
			ttl=atoi(str);
			rtp_transport_set(rtp_sess, RTP_TRANSPORT_TTL, &ttl);
			
			continue;
		}
		if ((tkna=strstrcase(tknb, "layers"))){
			int layers;
			
			for(; (*tkna == ' ') || (*tkna != '='); tkna++);
			
			for(tknb=tkna++; (*tknb != '\0') && (*tknb != '\r') && (*tknb != '\n'); tknb++);
			strncpy(str, tkna, tknb-tkna);
			str[tknb++-tkna]='\0';
		
			layers=atoi(str);
			rtp_transport_set(rtp_sess, RTP_TRANSPORT_LAYERS, &layers);
			
			continue;
		}
	
	} while ((tknb=strtok(NULL, ";")));
		
	return 0;
}
