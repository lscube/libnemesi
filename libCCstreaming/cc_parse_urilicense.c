/* * 
 *  $Id:cc_parse_urilicense.c 267 2006-01-12 17:19:45Z shawill $
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *      
 *      Giampaolo "mancho" Mancini - giampaolo.mancini@polito.it
 *    Francesco "shawill" Varano - francesco.varano@polito.it
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

//! This define will make global initialization in cc.h header
#define CC_GLOBAL_DATA
#include <nemesi/cc.h>
#include <nemesi/utils.h>

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
    char *tkn, *permstr;
    unsigned int i;

    memset(mask, 0, sizeof(*mask));

    // look if there is an "http:// prefix"
    if (strncmpcase(uri, "http://", 7))
        tkn = uri;
    else
        tkn = uri + 7;

    if (strncmpcase(tkn, BASE_URI_LICENSE, strlen(BASE_URI_LICENSE)))    // TODO: must continue or give an error, or ask what to to?
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
        if (!strcmpcase(permstr, cc_spec_licenses[i].urlstr)) {
            mask->spec_license = cc_spec_licenses[i].int_code;
            break;
        }
    }

    if (!mask->spec_license)
        // Search for CC atributes
        for (tkn = strtok(permstr, "-"); tkn; tkn = strtok(NULL, "-")) {
            // while(tkn) {
            if (!strcmpcase(tkn, cc_by.urltkn))
                mask->by = 1;
            else if (!strcmpcase(tkn, cc_nc.urltkn))
                mask->nc = 1;
            else if (!strcmpcase(tkn, cc_nd.urltkn))
                mask->nd = 1;
            else if (!strcmpcase(tkn, cc_sa.urltkn))
                mask->sa = 1;
        }

    free(permstr);

    return 0;
}
