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

#include <nemesi/cc.h>
#include <nemesi/comm.h>

void cc_printmask(cc_perm_mask mask)
{
    unsigned int i;
    CC_BITMASK_T spec_l = (CC_BITMASK_T) mask.spec_license;

    if (spec_l) {
        for (i = 0; cc_spec_licenses[i].int_code; i++) {
            if ((cc_spec_licenses[i].int_code & spec_l))
                nms_printf(NMSML_ALWAYS, "%s: %s\n",
                       cc_spec_licenses[i].name,
                       cc_spec_licenses[i].descr);
        }
    }
    if (mask.by)
        nms_printf(NMSML_ALWAYS, "%s: %s\n", cc_by.name, cc_by.descr);
    if (mask.nc)
        nms_printf(NMSML_ALWAYS, "%s: %s\n", cc_nc.name, cc_nc.descr);
    if (mask.nd)
        nms_printf(NMSML_ALWAYS, "%s: %s\n", cc_nd.name, cc_nd.descr);
    if (mask.sa)
        nms_printf(NMSML_ALWAYS, "%s: %s\n", cc_sa.name, cc_sa.descr);
    nms_printf(NMSML_ALWAYS, "\n");
}
