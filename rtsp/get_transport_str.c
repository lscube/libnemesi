/* * 
 *  ./rtsp/get_transport_str.c: $Revision: 1.3 $ -- $Date: 2002/11/28 12:00:49 $
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

int get_transport_str(struct RTP_Session *rtp_sess, char *buff)
{
	char *tkna, *tknb;
	char str[256];
	
	memset(str, 0, 256);
	
	for(tknb=strtok(buff,";"); (*tknb == ' ') || (*tknb == ':'); tknb++);
	
	do {
		if ((tkna=strstrcase(tknb, "server_port")) || ((tkna=strstrcase(tknb, "port")) && !strncmp(tknb, "port", 4))){
			for(; (*tkna == ' ') || (*tkna != '='); tkna++);
			for(tknb=tkna++; (*tknb == ' ') || (*tknb != '-'); tknb++);

			strncpy(str, tkna, tknb-tkna);
			str[tknb-tkna]='\0';
			rtp_sess->transport.srv_ports[0]=ntohs((unsigned short)atoi(str));
			
			for(tknb++; (*tknb == ' '); tknb++);

			for(tkna=tknb; (*tkna != '\0') && (*tkna != '\r') && (*tkna != '\n'); tkna++);
			strncpy(str, tknb, tkna++-tknb);
			str[tkna-tknb]='\0';
			rtp_sess->transport.srv_ports[1]=ntohs((unsigned short)atoi(str));

			continue;
		}
		if ((tkna=strstrcase(tknb, "source"))){
			for(; (*tkna == ' ') || (*tkna != '='); tkna++);
			
			for(tknb=tkna; (*tknb != '\0') && (*tknb != '\r') && (*tknb != '\n'); tknb++);
			strncpy(str, tkna, tknb++-tkna);
			str[tknb-tkna]='\0';

			if(!inet_aton(str, &(rtp_sess->transport.srcaddr))){
				uiprintf("PANIC: Source IP Address not valid!\n");
				return 1;
			}
			continue;
		}
		if ((tkna=strstrcase(tknb, "destination"))){
			for(; (*tkna == ' ') || (*tkna != '='); tkna++);
			
			for(tknb=tkna; (*tknb != '\0') && (*tknb != '\r') && (*tknb != '\n'); tknb++);
			strncpy(str, tkna, tknb++-tkna);
			str[tknb-tkna]='\0';
			
			if(!inet_aton(str, &(rtp_sess->transport.dstaddr))){
				uiprintf("PANIC: Destination IP Address not valid!\n");
				return 1;
			}
			continue;
		}
		if ((tkna=strstrcase(tknb, "ssrc"))){
			for(; (*tkna == ' ') || (*tkna != '='); tkna++);
			
			for(tknb=tkna; (*tknb != '\0') && (*tknb != '\r') && (*tknb != '\n'); tknb++);
			strncpy(str, tkna, tknb++-tkna);
			str[tknb-tkna]='\0';
			
			rtp_sess->transport.ssrc=(unsigned short)atoi(str);
			
			continue;
		}
		if ((tkna=strstrcase(tknb, "ttl"))){
			for(; (*tkna == ' ') || (*tkna != '='); tkna++);
			
			for(tknb=tkna; (*tknb != '\0') && (*tknb != '\r') && (*tknb != '\n'); tknb++);
			strncpy(str, tkna, tknb++-tkna);
			str[tknb-tkna]='\0';
			
			rtp_sess->transport.ttl=(unsigned short)atoi(str);
			
			continue;
		}
		if ((tkna=strstrcase(tknb, "layers"))){
			for(; (*tkna == ' ') || (*tkna != '='); tkna++);
			
			for(tknb=tkna; (*tknb != '\0') && (*tknb != '\r') && (*tknb != '\n'); tknb++);
			strncpy(str, tkna, tknb++-tkna);
			str[tknb-tkna]='\0';
		
			rtp_sess->transport.layers=(unsigned short)atoi(str);
			
			continue;
		}
	
	} while ((tknb=strtok(NULL, ";")));
		
	return 0;
}
