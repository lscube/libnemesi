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

#ifndef _LIB_CC_STREAMING
#define _LIB_CC_STREAMING

//! definition of couples containing name and description for each valid cc license
#define CC_LICENSES { \
			{ "uriLicense", "License URI" }, \
			{ "uriMetadata", "Validation URL" }, \
			{ "title", "Title of the presentation" }, \
			{ "creator", "Author of the presentation" } \
		    }

typedef struct {
	char *uriLicense;	//!< License URI
	char *uriMetadata;	//!< Validation URL
	char *title;		//!< Title of the presentation
	char *creator;		//!< Author of the presentation
} cc_license;

int issdplicense(char *sdp_a);

#endif // _LIB_CC_STREAMING

