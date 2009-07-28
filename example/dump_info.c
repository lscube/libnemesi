/*
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "nemesi/rtsp.h"
#include "nemesi/rtp.h"
#include "nemesi/sdp.h"

static void usage(char *name)
{
    fprintf(stderr,
            "\tUsage: %s [-f outputfile ] [-p rtp_port] [-t|-s] url\n",
            name);
}

int main(int argc, char **argv)
{

    int opt;
    char *url, *out = "nemesi.dump";
    FILE *outfile = NULL;
    rtsp_ctrl *ctl;
    rtsp_session *sess;
    rtsp_medium *med;
    nms_rtsp_hints rtsp_hints = { -1 };
    sdp_attr *attr;
    RTSP_Error reply;

    if (argc < 2) {
        fprintf(stderr, "\tPlease specify at least an url.\n");
        usage(argv[0]);
        exit(1);
    }

#ifndef WIN32
    while ((opt = getopt(argc, argv, "f:p:v:ts")) != -1) {
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
            /* Force TCP interleaved */
        case 't':
            rtsp_hints.pref_rtsp_proto = TCP;
            rtsp_hints.pref_rtp_proto = TCP;
            break;
            /* Force SCTP */
        case 's':
            rtsp_hints.pref_rtsp_proto = SCTP;
            rtsp_hints.pref_rtp_proto = SCTP;
            break;
            /* Unknown option  */
        case '?':
            fprintf(stderr, "\n  Unknown option `-%c'.\n", optopt);
            usage(argv[0]);

            return 1;
        }
    }
#endif

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
    printf("OPEN: Received reply from server: %s\n", reply.message.reply_str);

    // Get the session information
    sess = ctl->rtsp_queue;

    if (!sess) {
        fprintf(stderr, "No session available.\n");
        return 1;
    }

    while (sess) {        // foreach session...
        fprintf(outfile, "\tSession %s\n", sess->pathname);
        fprintf(outfile, "\tSession Duration %s\n", sess->info->t);
        for (attr = sess->info->attr_list;
             attr;
             attr = attr->next) {
            fprintf(outfile, "\t* %s %s\n", attr->name, attr->value);
        }

        med = sess->media_queue;
        while (med) {    //... foreach medium
            switch (med->medium_info->media_type) {
                // Just care about audio and video
            case 'A':
            case 'V':
                fprintf(outfile, "\tTransport %s\n",
                        med->medium_info->transport);
                fprintf(outfile, "\tMedia Type %s\n",
                        (med->medium_info->media_type == 'A'?
                            "Audio" : "Video"));
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
                fprintf(outfile, "\t* %s %s\n", attr->name, attr->value);
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
    printf("CLOSE: Received reply from server: %s\n", reply.message.reply_str);

    /*
     * Kill the threads, dealloc everything.
     */

    rtsp_uninit(ctl);

    return 0;
}
