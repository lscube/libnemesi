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


/*! \file sdp.h
 * \brief Header containig definitions for \b sdp library.
 * */

/*! \defgroup SDP SDP Library
 *
 * \brief Implementation of Source Description Protocol (SDP) -
 * rfc 2327.
 *
 * SDP is intended for describing multimedia sessions for the purposes of
 * session announcement, session invitation, and other forms of multimedia
 * session initiation. (from RFC abstract)
 *
 * @{ */

#ifndef __SDP_H
#define __SDP_H

#include <nemesi/cc.h>

/*!
 * \brief SDP attributes list basic element.
 *
 * In the body of a SDP description of a session or medium, among other well
 * know paramters, there could be a sequence of \em attribues, relevant to the
 * description of both session and medium, that can carry additional
 * information, not always standard. The number of these additional fields is
 * not predictable and so a dymanic list is needed.
 *
 * \see Medium_info
 * \see RTSP_Session_info
 * */
typedef struct _SDP_attr {
	char *a;		/*!< Pointer to the string containig the
				  attribute. */
	struct _SDP_attr *next;	/*!< Pointer to the next element of attributes
				  list. \c NULL if the current element is the
				  last. */
} SDP_attr;

#define SDP_MEDIA_FIELDS { \
				"Multimedia Type & Transport Address", \
				"Medium Title", \
				"Connection Info", \
				"Bandwidth Info", \
				"Crypto Key" \
			 };
/*!
 * \brief Medium imformations.
 * 
 * Each field has a non-NULL value if it's present the equivalent field in the
 * description body.
 *
 * For further informations about efficency of char pointers here adopted we
 * suggest to read documentation about <tt>\ref SDP_Session_Info</tt>.
 *
 * \see RTSP_Session_info
 * */
typedef struct _SDP_Medium_info {
	char *m;		/*!< medium name and transport address */
	char *i;		/*!< medium title */
	char *c;		/*!< connection information - optional if included at session-level */
	char *b;		/*!< bandwidth information */
	char *k;		/*!< encryption key */
	SDP_attr *attr_list;	/*!< zero or more medium attribute lines */

	CCLicense *cc;		/*!< Creative Commons License struct*/
	/*! informations from media description string (m=) */
	char media_type;	/*!< possibile values: A=Audio, V=Video, P=aPplication, D=Data, C=Control */
	int port;		/*!< port of stream*/	
	int n_ports;		/*!< number of consecutive ports */
	char transport[8];	/*!< transport string */
	char *fmts;		/*!< media formats: AVP payload types for audio/video*/

	struct _SDP_Medium_info *next;	/*!< Next medium informatioin struct */
} SDP_Medium_info;

#define SDP_SESSION_FIELDS { \
				"Protocol Version", \
				"Session Identifier & Creator", \
				"Session Name", \
				"Session Info", \
				"URI Description", \
				"e-mail Address", \
				"Phone Number", \
				"Connection Info", \
				"Bandwidth Info", \
				"Active Time", \
				"I' so' llu re", \
				"Time Zone", \
				"Crypto Key" \
			   }
/*!
 * \brief Session informations
 *
 * Each field will point to a description string inside the body of the
 * description arrived from server.
 *
 * On efficency, the same is true for the informations about medium, we can
 * analyse the strategy used for savig the fields.  In detail, the \b body of
 * the SDP description is entirely saved in memory and, then, \em tokenized
 * line by line. This way, the memory location is divided into strings, each
 * containing an information field.
 * Now it's possible to scan the entire memory location, whose size is known,
 * and to assign each pointer field to the right description string. If an
 * information is not present the corresponding pointer is left to the
 * initialization value: \c NULL. Using this strategy we have a slight waste of
 * memory space, but this is certainly lower than using pre-allocated strings.
 * Furthermore we avoid many \c malloc in the code that would make heavier the
 * execution of the program.
 *
 * \see SDP_Medium_info
 * */
typedef struct {
	char *v;		/*!< protocol version */
	char *o;		/*!< owner/creator and session identifier */
	char *s;		/*!< session name */
	char *i;		/*!< session information */
	char *u;		/*!< URI of description */
	char *e;		/*!< email address */
	char *p;		/*!< phone number */
	char *c;		/*!< onnection information - not required if included in all media */
	char *b;		/*!< bandwidth information */

	char *t;		/*!< time the session is active */
	char *r;		/*!< zero or more repeat times */

	char *z;		/*!< time zone adjustments */
	char *k;		/*!< encryption key */

	SDP_attr *attr_list;	/*!< zero or more session attribute lines */

	CCLicense *cc;		/*!< Creative Commons License struct */

	SDP_Medium_info *media_info_queue; /*!< Media information queue */
} SDP_Session_info;

SDP_Session_info *sdp_session_new(void);
SDP_Session_info *sdp_session_setup(char *descr, int descr_len);
SDP_Medium_info *sdp_media_setup(char **descr, int descr_len);
int sdp_set_attr(SDP_attr **attr_list, char *a);
void sdp_session_destroy(SDP_Session_info *);
void sdp_media_destroy(SDP_Medium_info *);
int sdp_parse_m_descr(SDP_Medium_info *, char *);

#endif // __SDP_H
/* @} */

