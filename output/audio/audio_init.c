/* * 
 *  ./decoder/audio/audio_init.c: $Revision: 1.3 $ -- $Date: 2002/11/28 12:00:48 $
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

int audio_init(void)
{
	struct audio_buff *audio_buffer;
#ifdef HAVE_SDL
	SDL_AudioSpec *SDL_audio_fmt;
#endif
	/* audio_buffer initialization */
	if ( (audio_buffer=ab_init()) == NULL ) {
		uiprintf("Failed while initializing Audio Buffer\n");
		return 1;
	}

#ifdef HAVE_SDL /* SDL Initialization */
	if( (SDL_audio_fmt = init_SDL(audio_buffer)) == NULL ) {
		uiprintf("Cannot initialize SDL Library\n");
		return 1;
	}
#else
	/* OSS Initialization */
	if((audio_buffer->audio_fd=set_audiodev()) == -1)
		return 1;
#endif
	global_audio_buffer=audio_buffer;

	return 0;
}
