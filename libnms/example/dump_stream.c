/*
 *  $Id: $
 *
 *  Copyright (C) 2006 by Luca Barbato
 *
 *  libnms is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  libnms is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libnms; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 *  Description: sample program, takes a url and dumps the sdp and 
 *  the information formatted per medium with proper descriptions.
 */


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include <nemesi/rtsp.h>
#include <nemesi/rtp.h>
#include <nemesi/sdp.h>


int main (int argc, char **argv) {

    int opt, i=0;
    char *url, *out=malloc(12), *base="dump_nms";
    int outfd[128];
    struct rtsp_ctrl *ctl;
    struct rtsp_thread *rtsp_th;
    rtp_thread *rtp_th;
    struct rtsp_session *sess;
    struct rtsp_medium *med;
    rtp_ssrc *ssrc;
    rtp_buff conf;
    rtp_frame fr;
    nms_rtsp_hints rtsp_hints = { -1 };

    if (argc < 2) {
        fprintf (stderr, "\tPlease specify at least an url.\n");
        fprintf (stderr, "\tUsage: %s [-f basename ][-p port] url\n",
		argv[0]);
        exit (1);
    }

    while ((opt = getopt (argc, argv, "f:p:")) != -1) {
        switch (opt)
	    {
        /*  Set output file  */
	    case 'f':
		base = strdup (optarg);
                out = realloc(out,strlen(base)+4);
	        break;
        /*  Set rtp port  */
	    case 'p':
		rtsp_hints.first_rtp_port = atoi (optarg);
	        break;
        /* Unknown option  */
	    case '?':
                fprintf (stderr, "\n  Unknown option `-%c'.\n", optopt);
                fprintf (stderr, "\tUsage: %s [-f outputfile ][-p port] url\n",
                                argv[0]);

	        return 1;
        }
    }

    memset(outfd,0,sizeof(outfd));

    url = argv [argc - 1];

    fprintf (stderr, "URL %s.\n", url);

    if ( (ctl = rtsp_init(&rtsp_hints))==NULL ) {
        fprintf (stderr, "Cannot init rtsp.\n");
        return 1;
    } 
    
    if ( rtsp_open( ctl, url) )
    {
        fprintf (stderr, "rtsp_open failed.\n");
	// die
	return 1;
    }
    
    rtsp_wait(ctl);
    
    rtsp_th = (struct rtsp_thread *)ctl;

    //Get the session information
    sess = rtsp_th->rtsp_queue;

    if (!sess) {
	fprintf (stderr, "No session available.\n");
	return 1;
    }

    rtsp_play(ctl,0.0,0.0);
    
    fprintf (stderr, "\nDumping...");
    
    rtp_th = rtsp_th->rtp_th;

    rtp_fill_buffers(rtp_th);
	
    // while(!pthread_mutex_trylock(&(rtp_th->syn))) {
    do {
        for (ssrc = rtp_active_ssrc_queue(rtsp_get_rtp_queue(ctl));
             ssrc;
             ssrc = rtp_next_active_ssrc(ssrc)) {
            if ( !rtp_fill_buffer(ssrc, &fr, &conf)) {
                
                if (outfd[fr.pt] ||
                    sprintf(out,"%s.%d",base,fr.pt) &&
                    (outfd[fr.pt] = creat(out, 00644)) > 0) {
                        if (write(outfd[fr.pt], fr.data, fr.len) < fr.len)
                            return 1;
                    }
                else
                    return 1;
            }

        }

        switch (i++) {
            case 0:
                fprintf(stderr,".");
            break;
            case 1:
                fprintf(stderr,"o");
            break;
            case 2:
                fprintf(stderr,"O");
            break;
            case 3:
                fprintf(stderr,"o");
            default:
                i=0;
        }
    } while (!rtp_fill_buffers(rtp_th));
    for (i=0; i<128; i++) if (outfd[i]) close(outfd[i]);
    fprintf(stderr," Complete\n");

return 0;
}
