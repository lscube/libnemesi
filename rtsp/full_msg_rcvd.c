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

#include <nemesi/rtsp.h>

int full_msg_rcvd( struct RTSP_Thread *rtsp_th )
{
	struct RTSP_buffer *in_buffer=&(rtsp_th->in_buffer);
	char *back_n; /* puntatore ai caratteri newline */
	char *head_end; /* puntatore alla fine dell'header */
	unsigned int body_len;
	
	if ( (head_end=strchr(in_buffer->data, '\n')) == NULL )
		return 0;
	do {
		back_n=head_end;
		if ( (head_end=strchr(head_end+1, '\n')) == NULL )
			return 0; /* non e' arrivato un header intero */
		if ( ((head_end-back_n)==2) && (*(back_n+1)=='\r') )
			break;
	} while( (head_end-back_n)>1 ); /* trovata la fine dell'header */
	while( (*(++head_end)=='\n') || (*head_end=='\r') ); /* cerca il primo 
								carattere valido dopo
							     la line vuota. */
	if ( (body_len=body_exists(in_buffer->data)) == 0 ){
		in_buffer->first_pkt_size=head_end-in_buffer->data;
		return 1; /* ricevuto un intero messaggio (contenente solo l'header) */
	}
	
	if ( strlen(head_end)<body_len )
		return 0; /* non e' arrivato ancora per intero il body */

	in_buffer->first_pkt_size=head_end-in_buffer->data+body_len;
	return 1; /* e' arrivato il messaggio per intero */
}

