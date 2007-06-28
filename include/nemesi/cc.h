/* * 
 * This file is part of NetEmbryo
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * NetEmbryo is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * NetEmbryo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with NetEmbryo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *  
 * */

#ifndef _LIB_CC_STREAMING
#define _LIB_CC_STREAMING

#include <string.h>

#include <nemesi/types.h>

/*! If CC_GLOBAL_DATA is defined, we initialize global data containing the
 * licenses
 */
#ifndef CC_GLOBAL_DATA        // set just one time in cc_parse_urilicense.c
#define CC_EXTERN extern
#else                // CC_GLOBAL_DATA
#define CC_EXTERN
#endif                // CC_GLOBAL_DATA

// address without "http://" prefix
#define BASE_URI_LICENSE "creativecommons.org/licenses/"

/*!
 * \brief License condition
 *
 * Here we store one of the conditions defining a CC license.
 * */
typedef struct {
    char *name;
    char *urltkn /*[3] */ ;
    char *descr;
} cc_perm;

CC_EXTERN cc_perm cc_by
#ifdef CC_GLOBAL_DATA
    = { "Attribution", "by",
    "The licensor permits others to copy, distribute, display, and perform the work. In return, licensees must give the original author credit."
}
#endif                // CC_GOBAL_DATA
;

CC_EXTERN cc_perm cc_nc
#ifdef CC_GLOBAL_DATA
    = { "NonCommercial", "nc",
    "The licensor permits others to copy, distribute, display, and perform the work. In return, licensees may not use the work for commercial purposes -- unless they get the licensor's permission."
}
#endif                // CC_GOBAL_DATA
;

CC_EXTERN cc_perm cc_nd
#ifdef CC_GLOBAL_DATA
    = { "NoDerivative", "nd",
    "The licensor permits others to copy, distribute, display and perform only unaltered copies of the work -- not derivative works based on it."
}
#endif                // CC_GOBAL_DATA
;

CC_EXTERN cc_perm cc_sa
#ifdef CC_GLOBAL_DATA
    = { "ShareAlike", "sa",
    "The licensor permits others to distribute derivative works only under a license identical to the one that governs the licensor's work."
}
#endif                // CC_GOBAL_DATA
;

#define CC_BITMASK_T uint8

typedef struct {
    char *name;
    char *urlstr;
    char *descr;
    CC_BITMASK_T int_code;
} cc_spec_license;

/*! definition of internal identification code for special licenses
 *
 * Warning: this code MUST be a power-of-2 number
 * 0001 || 0010 || 0100 || 1000
 *
 * If someday we need to select more special licenses we can "upgrade" the
 * field of bit from uint8 to uint16, or more...
 */
#define CC_PD 1

CC_EXTERN cc_spec_license cc_spec_licenses[]
#ifdef CC_GLOBAL_DATA
    = {
    {"PubblicDomain", "publicdomain", "Public domain dedication", CC_PD},
    /*end */ {0, 0, 0, 0}
}
#endif                // CC_GLOBAL_DATA
;

/*
 * If someday we need to select more special licenses we can "upgrade" the
 * field of bit from uint8 to uint16, or more...
 */
typedef struct _ccpermsmask {
    CC_BITMASK_T by:1;
    CC_BITMASK_T nc:1;
    CC_BITMASK_T nd:1;
    CC_BITMASK_T sa:1;
    // special license ID
    CC_BITMASK_T spec_license:4;
} cc_perm_mask;

#define CC_ACCEPT_ALL(x) memset(&x, 0xFF, sizeof(cc_perm_mask))

//! definition of couples containing name and description for each valid cc license
#define CC_LICENSE { \
            { "uriLicense", "License URI" }, \
            { "uriMetadata", "Validation URL" }, \
            { "title", "Title of the presentation" }, \
            { "creator", "Author of the presentation" } \
            }

#define CC_ATTR_NAME 0
#define CC_ATTR_DESCR 1

/*!
 * Warning: the fields of the struct MUST be ordered with initilzation strings in the define CC_LICENSE
 */
typedef struct {
    char *uriLicense;    //!< License URI
    char *uriMetadata;    //!< Validation URL
    char *title;        //!< Title of the presentation
    char *creator;        //!< Author of the presentation
    // end of CC fields
} cc_license;

typedef struct {
    int8 *header;
    uint32 hdim;
    int8 *footer;
    uint32 fdim;
} cc_tag;

int issdplicense(char *sdp_a);
cc_license *cc_newlicense(void);
int cc_set_sdplicense(cc_license *, char *);
int cc_parse_urilicense(char *, cc_perm_mask *);
int cc_perm_chk(cc_license *, cc_perm_mask *);
void cc_printmask(cc_perm_mask);
// shawill: someday we will rewrite or remove this function
int cc_setag(int, cc_license *);
int cc_getag(int, cc_tag **, char **);
// MPA tagging function (pt 14)
int cc_id3v2(cc_license *, cc_tag *);

#undef CC_EXTERN
#undef CC_GLOBAL_DATA

#endif                // _LIB_CC_STREAMING
