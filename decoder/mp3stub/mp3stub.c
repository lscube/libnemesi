/* * 
 *  ./decoder/mp3stub/mp3stub.c: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:18 $
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <unistd.h>
#include <stdlib.h>

#include <config.h>

#include <nemesi/comm.h>
#include <nemesi/types.h>


int decode(char *data, int len, int audio_fd, uint8 *(*ab_get)(uint32))
{
	static int init=1;
	static int to_mp3[2];
	static int pid;

	if (init){
		if ( pipe(to_mp3) < 0 ){
			fprintf(stderr,"\nCannot create pipes\n");
			return 1;
		}
		if ( (pid = fork()) < 0){
			fprintf(stderr, "\nFork failed!\n");
			return 1;
		}

		if (pid > 0) /* Parent */
			close(to_mp3[0]);
		else { /* Child */
			close(to_mp3[1]);
			if ( to_mp3[0] != STDIN_FILENO ){
				if ( dup2(to_mp3[0], STDIN_FILENO) != STDIN_FILENO ){
					fprintf(stderr, "\nDup2 failed!\n");
					return 1;
				}
				close(to_mp3[0]);
			}
			if ( audio_fd != STDOUT_FILENO ){
				if ( dup2(audio_fd, STDOUT_FILENO) != STDOUT_FILENO ){
					fprintf(stderr, "\nDup2 failed!\n");
					return 1;
				}
			}
			if ( execlp("mpg123", "mpg123", "-r", "44100", "-s", "-", (char *) 0) < 0){
				fprintf(stderr, "\nCannot execute mpg123\n");
				return 1;
			}
		}
		init=0;
	}
	if ( pid > 0)
		write(to_mp3[1], data+4, len-4);

	return 0;
}

int get_plugin_pt(void)
{
	return 14;
}	
