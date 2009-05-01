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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#include "rtsp.h"
#include "rtp.h"
#include "sdp.h"


int main(int argc, char **argv)
{
    int opt, i = 0;
    char *url, *out = malloc(12), *base = "dump_nms";
    int outfd[128];
    rtsp_ctrl *ctl;
    rtp_thread *rtp_th;
    rtsp_session *sess;
    rtp_ssrc *ssrc;
    rtp_buff conf;
    rtp_frame fr;
    nms_rtsp_hints rtsp_hints = { -1 };
    time_t now, before;
    RTSP_Error reply;


    if (argc < 2) {
        fprintf(stderr, "\tPlease specify at least an url.\n");
        fprintf(stderr, "\tUsage: %s [-f basename ][-p port] url\n",
            argv[0]);
        exit(1);
    }

    while ((opt = getopt(argc, argv, "f:p:")) != -1) {
        switch (opt) {
            /*  Set output file  */
        case 'f':
            base = strdup(optarg);
            out = realloc(out, strlen(base) + 4);
            break;
            /*  Set rtp port  */
        case 'p':
            rtsp_hints.first_rtp_port = atoi(optarg);
            break;
            /* Unknown option  */
        case '?':
            fprintf(stderr, "\n  Unknown option `-%c'.\n", optopt);
            fprintf(stderr,
                "\tUsage: %s [-f outputfile ][-p port] url\n",
                argv[0]);

            return 1;
        }
    }

    memset(outfd, 0, sizeof(outfd));

    url = argv[argc - 1];

    fprintf(stderr, "URL %s.\n", url);

    if ((ctl = rtsp_init(&rtsp_hints)) == NULL) {
        fprintf(stderr, "Cannot init rtsp.\n");
        return 1;
    }

    if (rtsp_open(ctl, url)) {
        fprintf(stderr, "rtsp_open failed.\n");
        // die
        return 1;
    }

    rtsp_wait(ctl);

    //Get the session information
    sess = ctl->rtsp_queue;

    if (!sess) {
        fprintf(stderr, "No session available.\n");
        return 1;
    }

    rtsp_play(ctl, 0.0, 0.0);
    rtsp_wait(ctl);

    fprintf(stderr, "\nDumping...");
    before = time(NULL);

    rtp_th = rtsp_get_rtp_th(ctl);
    while (!rtp_fill_buffers(rtp_th))    // Till there is something to parse
    {            // Foreach ssrc active
        for (ssrc = rtp_active_ssrc_queue(rtsp_get_rtp_queue(ctl));
             ssrc; ssrc = rtp_next_active_ssrc(ssrc)) {
            if (!rtp_fill_buffer(ssrc, &fr, &conf)) {    // Parse the stream
                if (reply.message.reply_code == 200) {
                    printf("RTP Data first byte: %d\n", fr.data[0]);
                    reply.message.reply_code = 0;
                }
            }
        }

        now = time(NULL);
        if (now >= (before+4)) {
            before = now;

            i = rtsp_seek(ctl, 0, 0);
            printf("SEEK Result: %d\n", i);
            reply = rtsp_wait(ctl);
            printf("SEEK Response: %s\n", reply.message.reply_str);

            if (reply.message.reply_code != 200)
                break;
        }
    }

    fprintf(stderr, " Complete\n");

    rtsp_close(ctl);
    rtsp_wait(ctl);

    rtsp_uninit(ctl);

    return 0;
}
