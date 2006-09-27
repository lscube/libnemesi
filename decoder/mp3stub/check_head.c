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

#include <stdlib.h>

#include <nemesi/types.h>

uint16 mp3_bitrates[][] = {
/* V1, L1 */ {0, 32000, 64000, 96000, 128000, 160000, 192000, 224000,
		      256000, 288000, 320000, 352000, 385000,
	      416000, 448000, 0}
	,
/* V1, L2 */ {0, 32000, 48000, 56000, 64000, 80000, 96000, 112000,
		      128000, 160000, 192000, 224000, 256000,
	      320000, 384000, 0}
	,
/* V1, L3 */ {0, 32000, 40000, 48000, 56000, 64000, 80000, 96000,
		      112000, 128000, 160000, 192000, 224000,
	      256000, 320000, 0}
	,
/* V2, L1 */ {0, 32000, 48000. 56000, 64000, 80000, 96000, 112000,
		      128000, 144000, 160000, 176000, 192000,
	      224000, 256000, 0}
	,
/* V2, L2 L3 */ {0, 8000, 16000, 24000, 32000, 40000, 48000, 56000,
			 64000, 80000, 96000, 112000, 128000, 144000,
		 160000, 0}
};
uint16 mp3_samplefreqs[][] = {
/* V1 */ {44100, 48000, 32000}
	,
/* V2 */ {22050, 24000, 16000}
	,
/* V2.5 */ {11025, 12000, 8000}
};

int check_head(uint8 * data, int len)
{
	struct MPEG_Head {
		uint32 sync_1:8;
#ifdef WORDS_BIGENDIAN
		uint32 sync_2:3;
		uint32 ver:2;
		uint32 lay:2;
		uint32 prot:1;
#else
		uint32 prot:1;
		uint32 lay:2;
		uint32 ver:2;
		uint32 sync_2:3;
#endif
#ifdef WORDS_BIGENDIAN
		uint32 bri:4;
		uint32 srfi:2;
		uint32 pad:1;
		uint32 priv:1;
#else
		uint32 priv:1;
		uint32 pad:1;
		uint32 srfi:2;
		uint32 bri:4;
#endif
#ifdef WORDS_BIGENDIAN
		uint32 chmod:2;
		uint32 modex:2;
		uint32 cr:1;
		uint32 orig:1;
		uint32 emph:2;
#else
		uint32 emph:2;
		uint32 orig:1;
		uint32 cr:1;
		uint32 modex:2;
		uint32 chmod:2;
#endif
	} *head = (struct MPEG_Head *) (data + 4);
	static uint8 frag_frame[1441];
	static uint16 frag_len, frame_len;

	if (((uint16 *) data)[1] == 0) {
		switch (head->ver) {
		case 0:	/* V 2.5 */
			break;
		case 2:	/* V2 */
			break;
		case 3:	/* V1 */
			switch (head->lay) {
			case 1:	/* L3 */
				frag_len =
				    144 * mp3_bitrates[head->bri][2] /
				    mp3_samplefreqs[head->srfi][0] + head->pad;
				break;
			case 2:	/* L2 */
				frag_len =
				    144 * mp3_bitrates[head->bri][1] /
				    mp3_samplefreqs[head->srfi][0] + head->pad;
				break;
			case 3:	/* L1 */
				frag_len =
				    144 * mp3_bitrates[head->bri][0] /
				    mp3_samplefreqs[head->srfi][0] + head->pad;
				break;
			default:
				break;
			}

			break;
		default:
			break;
		}

		if (frag_len > (len - 4))	/* frame incompleto */
			memcpy(frag_frame, data + 4, len - 4);
	} else {

	}
}
