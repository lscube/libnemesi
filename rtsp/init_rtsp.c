/* * 
 *  ./rtsp/init_rtsp.c: $Revision: 1.3 $ -- $Date: 2002/11/28 12:00:49 $
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

/**
* funzione chiamata all'inizio edl thread RTSP (funzione rtsp) per inizializzarlo.
* @param rtsp_args puntatore alla struttura in comnune tra i thread RTSP e UI.
* @return 1 in caso di errore, 0 altrimenti.
* @see rtsp
* @see create_thread
* */
int init_rtsp(void)
{
	cmd[0] = open_cmd;
	cmd[1] = play_cmd;
	cmd[2] = pause_cmd;
	cmd[3] = stop_cmd;
	cmd[4] = close_cmd;

	state_machine[0] = init_state;
	state_machine[1] = ready_state;
	state_machine[2] = playing_state;
	state_machine[3] = recording_state;

	return 0;
}
