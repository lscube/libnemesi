/* *
 * This file is part of libnemesi
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 *
 * libnemesi is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libnemesi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libnemesi; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * */

#ifndef NEMESI_CC_H
#define NEMESI_CC_H

#include <string.h>
#include <stdint.h>

// address without "http://" prefix
#define BASE_URI_LICENSE "creativecommons.org/licenses/"

/*!
 * \brief License condition
 *
 * Here we store one of the conditions defining a CC license.
 * */
typedef struct {
    const char *name;
    const char *urltkn /*[3] */ ;
    const char *descr;
} cc_perm;

extern const cc_perm cc_by;
extern const cc_perm cc_nc;
extern const cc_perm cc_nd;
extern const cc_perm cc_sa;

#define CC_BITMASK_T uint8_t

typedef struct {
    const char *name;
    const char *urlstr;
    const char *descr;
    CC_BITMASK_T int_code;
} cc_spec_license;

/*! definition of internal identification code for special licenses
 *
 * Warning: this code MUST be a power-of-2 number
 * 0001 || 0010 || 0100 || 1000
 *
 * If someday we need to select more special licenses we can "upgrade" the
 * field of bit from uint8_t to uint16, or more...
 */
#define CC_PD 1

extern const cc_spec_license cc_spec_licenses[];

/*
 * If someday we need to select more special licenses we can "upgrade" the
 * field of bit from uint8_t to uint16, or more...
 */
typedef struct {
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
    int8_t *header;
    uint32_t hdim;
    int8_t *footer;
    uint32_t fdim;
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

#endif  /* NEMESI_CC_H */
