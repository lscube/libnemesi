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

#include <nemesi/rtsp.h>
#include <nemesi/rtp.h>
#include <nemesi/sdp.h>

int main(int argc, char **argv)
{

    int opt, reply;
    char *url, *out = "nemesi.dump";
    FILE *outfile = NULL;
    rtsp_ctrl *ctl;
    rtsp_session *sess;
    rtsp_medium *med;
    nms_rtsp_hints rtsp_hints = { -1 };
    sdp_attr *attr;

    if (argc < 2) {
        fprintf(stderr, "\tPlease specify at least an url.\n");
        fprintf(stderr,
            "\tUsage: %s [-f outputfile ] [-p rtp_port] [-t url\n",
            argv[0]);
        exit(1);
    }

    while ((opt = getopt(argc, argv, "df:p:v:")) != -1) {
        switch (opt) {

            /*  Set output file  */
        case 'f':
            out = strdup(optarg);
            break;
            /*  Set rtp port  */
        case 'p':
            rtsp_hints.first_rtp_port = atoi(optarg);
            break;
            /*  Set verbosity  */
        case 'v':
            nms_verbosity_set(atoi(optarg));
            break;
            /* Unknown option  */
        case '?':
            fprintf(stderr, "\n  Unknown option `-%c'.\n", optopt);
            fprintf(stderr,
                "\tUsage: %s [-f outputfile ] [-d] url\n\n",
                argv[0]);

            return 1;
        }
    }

    outfile = fopen(out, "rb");
    if (outfile == NULL)
        outfile = stderr;

    url = argv[argc - 1];

    fprintf(stderr, "URL %s.\n", url);

    /* initialize the rtsp state machine, starts the rtsp and the rtp threads
     * the hints available are just one:
     *  - the first port to use (instead of picking one at random)
     */
    if ((ctl = rtsp_init(&rtsp_hints)) == NULL) {
        fprintf(stderr, "Cannot init rtsp.\n");
        return 1;
    }

    if (rtsp_open(ctl, url)) {
        fprintf(stderr, "rtsp_open failed.\n");
        // die
        return 1;
    }
       
    // you must call rtsp_wait after issuing any command
    reply = rtsp_wait(ctl);
    printf("OPEN: Received reply from server: %d\n", reply);

    // Get the session information
    sess = ctl->rtsp_queue;

    if (!sess) {
        fprintf(stderr, "No session available.\n");
        return 1;
    }

    while (sess) {        // foreach session...
        fprintf(outfile, "\tSession %s\n", sess->pathname);
        fprintf(outfile, "\tSession Duration %s\n", sess->info->t);

        med = sess->media_queue;
        while (med) {    //... foreach medium
            switch (med->medium_info->media_type) {
                // Just care about audio and video
            case 'A':
            case 'V':
                fprintf(outfile, "\tTransport %s\n",
                        med->medium_info->transport);
                fprintf(outfile, "\tMedia format %s\n",
                        med->medium_info->fmts);
                break;
            default:
                //do nothing
                break;
            }

            // attributes are already parsed, get them from the list
            for (attr = med->medium_info->attr_list;
                 attr;
                 attr = attr->next) {
                fprintf(outfile, "\t* %s\n", attr->a);
            }

            med = med->next;
        }
        sess = sess->next;
    }

    /*
     * Close the rtsp connection, we are polite
     */

    rtsp_close(ctl);
    reply = rtsp_wait(ctl);
    printf("CLOSE: Received reply from server: %d\n", reply);

    /*
     * Kill the threads, dealloc everything.
     */

    rtsp_uninit(ctl);

    return 0;
}
