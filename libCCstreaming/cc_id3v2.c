/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *  	
 *  	Giampaolo "mancho" Mancini - giampaolo.mancini@polito.it
 *	Francesco "shawill" Varano - francesco.varano@polito.it
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
#include <string.h>

#include <nemesi/cc.h>
#include <nemesi/types.h>
#include <nemesi/utils.h>

// inspired by id3lib
#define MASK(bits) ((1 << (bits)) - 1)

#define ID3v2_MAJOR 0x04
#define ID3v2_REV 0x00

#define ID3v2_HDRLEN 10 // basic header lenght for id3 tag
#define ID3v2_FRAMEHDRLEN 11 // 10 for header + 1 for text encoding (redundant)

#define CC_URILIC "This work is licenced under the "
#define CC_URIMETA " verify at "

#define FRAME_ID { \
			"TENC", \
			"WXXX", \
			"TCOP", \
			"TOPE", \
			"TCOM", \
			"COMM", \
			"TYER", \
			"TPE1", \
			"TALB", \
			"TIT2", \
			"TRAK", \
			"TCOM"  \
		 }

struct id3hdr {
	uint8 id[3];
	uint8 major;
	uint8 rev;
	uint8 flags;
	uint32 size;
	// not using extended header
	uint8 frames[];
};

static uint32 enc_synchsafe_int(uint32 num)
{
	uint32 encoded=0;
	uint8 *enc_chr=(uint8 *)(&encoded);
	const unsigned char bitsused = 7;
	const uint32 maxval = MASK(bitsused * sizeof(uint32));
	int i;

	num = min(num, maxval);

	for (i=sizeof(uint32) - 1; i>=0; i--) {
		enc_chr[i] = (uint8)(num & MASK(bitsused));
		num >>= bitsused;
	}

	return encoded;
}

int cc_id3v2(CCLicense *license, CCTag *tag)
{
	uint32 id3len=0;
	struct id3hdr *id3;
	uint8 *pos;

	// id3 length computation
	// first we calculate len of each tag
	
	if (license->title) {
		// there's the title info => put TPTT frame
		id3len += ID3v2_FRAMEHDRLEN + strlen(license->title);
	}
	if (license->creator) {
		// there's the creator info => put TIT2 frame
		id3len += ID3v2_FRAMEHDRLEN + strlen(license->creator);
	}
	if ((license->uriLicense) || (license->uriMetadata)) {
		// there's CC info => put TCOP frame
		id3len += ID3v2_FRAMEHDRLEN;
		if (license->uriLicense)
			id3len += strlen(CC_URILIC) + strlen(license->uriLicense);
		if (license->uriMetadata)
			id3len += strlen(CC_URIMETA) + strlen(license->uriMetadata);
	}

	if (!id3len) // no frame to put in => error
		return 1;

	// add id3 header len to total dimension of buffer
	id3len += ID3v2_HDRLEN;

	// alloc buffer for tag
	if ( !( id3 = malloc(id3len * sizeof(uint8)) ) )
		return 1;

	// ID3v2 header creation
	// string "ID3"
	strncpy(id3->id, "ID3", 3);
	// ID3v2 version
	id3->major = ID3v2_MAJOR;
	id3->rev = ID3v2_REV;
	// flags
	id3->flags = 0x00;
	// synchsafe integer for size
	id3->size = enc_synchsafe_int(id3len);

	pos = id3->frames;

	// put frames

	tag->header = (char *)id3;
	tag->hdim = id3len;

	return 0;
}

