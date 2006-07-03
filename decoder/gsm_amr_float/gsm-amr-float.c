/*
 * ===================================================================
 *  TS 26.104
 *  R99   V3.3.0 2001-09
 *  REL-4 V4.2.0 2001-09
 *  3GPP AMR Floating-point Speech Codec
 * ===================================================================
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <memory.h>
#include "interf_dec.h"
#include "sp_dec.h"
#include "interf_rom.h"

#include "typedef.h"

#include <config.h>
#include <nemesi/types.h>
#include <nemesi/output.h>

#include <nemesi/plugin.h>

plugin_init("GSM-AMR", 108)
/*
void Copyright(void)
{
	fprintf(stderr, "===================================================================\n"
		" TS 26.104                                                         \n"
		" R99   V3.3.0 2001-09                                              \n"
		" REL-4 V4.2.0 2001-09                                              \n"
		" 3GPP AMR Floating-point Speech Decoder                            \n"
		"===================================================================\n");
}
*/
/*
 * main
 *
 *
 * Function:
 *    Speech decoder main program
 *
 *    Usage: decoder bitstream_file synthesis_file
 *
 *    Format for ETSI bitstream file:
 *       1 word (2-byte) for the TX frame type
 *       244 words (2-byte) containing 244 bits.
 *          Bit 0 = 0x0000 and Bit 1 = 0x0001
 *       1 word (2-byte) for the mode indication
 *       4 words for future use, currently written as zero
 *
 *    Format for 3GPP bitstream file:
 *       Holds mode information and bits packed to octets.
 *       Size is from 1 byte to 31 bytes.
 *
 *    Format for synthesis_file:
 *       Speech is written to a 16 bit 8kHz file.
 *
 *    ETSI bitstream file format is defined using ETSI as preprocessor
 *    definition
 * Returns:
 *    0
 */
/*int main (int argc, char * argv[]){*/

#define CHANNELS 2
#define FACTOR_OVERSAMPLE 5
#define ELEVEN 11

#define L_FRAME 160

static int decode(char *data, int len, nms_output *outc)
{
	nms_au_fnc *funcs = outc->audio->functions;

	short synth[L_FRAME];
	int frames = 0;
	static int *destate = NULL;
	int read_size, i, j, c;
	unsigned char adjust = 0, adjust40 = 0;
#if 0
	char outbuff[L_FRAME * sizeof(short) * ELEVEN + L_FRAME * sizeof(short) * ELEVEN / 40];
	short *p = (short *) outbuff;
#endif
	uint8 *outbuff;
	uint16 *p;
	uint32 req_len;

	unsigned char *analysis = (unsigned char *) data;
	enum Mode dec_mode;

	req_len = L_FRAME * sizeof(uint16) * ELEVEN + L_FRAME * sizeof(uint16) / 40;
	// outbuff=(uint8 *)p=funcs->get_buff(req_len);
	outbuff=funcs->get_buff(req_len);
	p=(uint16 *)outbuff;

	/* init decoder */
	if (!destate)
		destate = Decoder_Interface_init();

#if 0	
#ifndef WORDS_BIGENDIAN
	for (i = 0; i < len; i += 2) {
		sample = data[i + 1];
		data[i + 1] = data[i];
		data[i] = sample;
	}
#endif
#endif

	/* find mode */
	while (len > 0) {
		dec_mode = analysis[0] & 0x000F;
		switch (dec_mode) {
		case 0:
			read_size = 12;
			break;
		case 1:
			read_size = 13;
			break;
		case 2:
			read_size = 15;
			break;
		case 3:
			read_size = 17;
			break;
		case 4:
			read_size = 18;
			break;
		case 5:
			read_size = 20;
			break;
		case 6:
			read_size = 25;
			break;
		case 7:
			read_size = 30;
			break;
		case 8:
			read_size = 5;
			break;
		case 15:
			read_size = 0;
		default:
			read_size = 0;
			break;
		};

		frames++;

		/* call decoder */
		Decoder_Interface_Decode(destate, analysis, synth, 0);
		analysis += read_size + 1;
		len -= read_size + 1;
		

		/* Oversample to 44100Hz -- very very quick, raw and dirt */
		for (i = 0; i < L_FRAME ; i++) {
			for (j = 0; j < FACTOR_OVERSAMPLE + adjust; j++) {
				for (c = 0; c < CHANNELS; c++)
					*(p++) = synth[i];
			}
			if (!(i % 40))
				adjust40 = 1;
			adjust = !adjust;
			if (adjust40 && !adjust) {
				adjust = 1;
				adjust40 = 0;
			}
		}
		funcs->play_buff(outbuff, req_len, outc->elapsed);
/*		if (write(audio_fd, outbuff, L_FRAME * sizeof(short) * ELEVEN + L_FRAME * sizeof(short) / 40) != (L_FRAME * sizeof(short) * ELEVEN) + L_FRAME * sizeof(short) / 40)
			fprintf(stderr, "\nerror writing output file: %s\n", strerror(errno));
*/
	}

	return 0;
}

int get_plugin_pt(void)
{
	return 108;
}
