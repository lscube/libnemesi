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

#include "nemesi/cc.h"
#include "utils/utils.h"

const cc_perm cc_by = {
    "Attribution", "by",
    "The licensor permits others to copy, distribute, display, and perform the work. "
    "In return, licensees must give the original author credit."
};

const cc_perm cc_nc = {
    "NonCommercial", "nc",
    "The licensor permits others to copy, distribute, display, and perform the work. "
    "In return, licensees may not use the work for commercial purposes -- "
    "unless they get the licensor's permission."
};

const cc_perm cc_nd = {
    "NoDerivative", "nd",
    "The licensor permits others to copy, distribute, display and perform only "
    "unaltered copies of the work -- not derivative works based on it."
};

const cc_perm cc_sa = {
    "ShareAlike", "sa",
    "The licensor permits others to distribute derivative works only under a "
    "license identical to the one that governs the licensor's work."
};

const cc_spec_license cc_spec_licenses[] = {
    {"PubblicDomain", "publicdomain", "Public domain dedication", CC_PD},
    {0, 0, 0, 0} /*end */
};

/*! \brief Parses Licenes URI and fills cc_perms data structure.
 *
 * To know what are the conditions of the license we parse the uri and look for
 * short names.
 *
 * \param uri license uri to parse.
 * \param conds cc_perms structure to be filled.
 */
int cc_parse_urilicense(char *uri, cc_perm_mask * mask)
{
    char *tkn, *permstr, *step;
    unsigned int i;

    memset(mask, 0, sizeof(*mask));

    // look if there is an "http:// prefix"
    if (strncasecmp(uri, "http://", 7))
        tkn = uri;
    else
        tkn = uri + 7;

    if (strncasecmp(tkn, BASE_URI_LICENSE, strlen(BASE_URI_LICENSE)))    // TODO: must continue or give an error, or ask what to to?
        return nms_printf(NMSML_ERR,
                  "the base URI of license is not \"%s\", so it can't be considered valid\n");

    tkn = tkn + strlen(BASE_URI_LICENSE);
    while (*tkn == '/')
        tkn++;
    if (!(permstr = strdup(tkn)))
        return nms_printf(NMSML_FATAL,
                  "memory error in cc_parse_urilicense\n");
    if ((tkn = strchr(permstr, '/')))
        *tkn = '\0';

    // Check for special licenses :TODO
    // for (i=0; i<sizeof(cc_spec_licenses)/sizeof(*cc_spec_licenses); i++) {
    for (i = 0; cc_spec_licenses[i].int_code; i++) {
        if (!strcasecmp(permstr, cc_spec_licenses[i].urlstr)) {
            mask->spec_license = cc_spec_licenses[i].int_code;
            break;
        }
    }

    if (!mask->spec_license)
        // Search for CC atributes
        for (tkn = strtok_r(permstr, "-", &step);
             tkn;
             tkn = strtok_r(NULL, "-", &step)) {
            // while(tkn) {
            if (!strcasecmp(tkn, cc_by.urltkn))
                mask->by = 1;
            else if (!strcasecmp(tkn, cc_nc.urltkn))
                mask->nc = 1;
            else if (!strcasecmp(tkn, cc_nd.urltkn))
                mask->nd = 1;
            else if (!strcasecmp(tkn, cc_sa.urltkn))
                mask->sa = 1;
        }

    free(permstr);

    return 0;
}
