/* * 
 *  ./output/audio/get_sys_buff.c: $Revision: 1.3 $ -- $Date: 2003/01/15 11:18:00 $
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

#include <nemesi/audio.h>

#ifndef HAVE_SDL
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#endif

/* int get_sy_buff(struct audio_buff *audio_buffer) */
int get_sys_buff(void)
{
#ifndef HAVE_SDL
	audio_buf_info info;
#endif
	float sys_buff;
	struct audio_buff *audio_buffer = global_audio_buffer;

#ifdef HAVE_SDL
	sys_buff=(float)audio_buffer->len/(float)AUDIO_BUFF_SIZE;
/*	fprintf(stderr, "System buffer: %4.1f %% \r", (100.0*sys_buff) ); */

	sys_buff=sys_buff_size(&sys_buff);
	if(sys_buff<0.2) {
		/* fprintf(stderr, "\n***\tSystem buffer empty: %f -> required %d Fast Cicles\n", sys_buff, (int)((0.2-sys_buff)*50)); */
		/* return (int)((0.5-sys_buff)*50); / 0.5-sys_buff/0.02 -> 0.5-sys_buff/20ms */
		return 2;
	}
#else
	ioctl(audio_buffer->audio_fd,SNDCTL_DSP_GETOSPACE, &info);
/*	fprintf(stderr, "System buffer: %4.1f %% \r", (100.0-(100.0*(double)info.bytes/(double)(info.fragsize*info.fragstotal))) ); */
	sys_buff=(1.0-((float)info.bytes/(float)(info.fragsize*info.fragstotal)));
	sys_buff=sys_buff_size(&sys_buff);

	if((info.fragsize*info.fragstotal-info.bytes < 2*info.fragsize)){
	/*if((info.bytes > 2*info.fragsize)){*/
		fprintf(stderr, "\nfast cycle !!!%d\n", (info.bytes/info.fragsize)/2);
		return 2;
	}
#endif

	return 0;
}
