/* $Id$ */
/** @file
 * IPRT - System firmware information for Solaris.
 */

/*
 * Copyright (C) 2022 Oracle and/or its affiliates.
 *
 * This file is part of VirtualBox base platform packages, as
 * available from https://www.virtualbox.org.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, in version 3 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses>.
 *
 * The contents of this file may alternatively be used under the terms
 * of the Common Development and Distribution License Version 1.0
 * (CDDL), a copy of it is provided in the "COPYING.CDDL" file included
 * in the VirtualBox distribution, in which case the provisions of the
 * CDDL are applicable instead of those of the GPL.
 *
 * You may elect to license modified versions of this file under the
 * terms and conditions of either the GPL or the CDDL or both.
 *
 * SPDX-License-Identifier: GPL-3.0-only OR CDDL-1.0
 */


/*********************************************************************************************************************************
*   Header Files                                                                                                                 *
*********************************************************************************************************************************/
#include "internal/iprt.h"
#include <iprt/system.h>
#include <iprt/errcore.h>

#include <libdevinfo.h>


/**
 * Determine the type of firmware of a host, i.e. whether it booted using UEFI
 * or BIOS.
 *
 * @param  penmFirmwareType    Where to store the firmware type.
 *
 * @return VINF_SUCCESS, if successfully able to determine the firmware type,
 *         otherwise VERR_NOT_SUPPORTED.
 */
RTDECL(int) RTSystemQueryFirmwareType(PRTSYSFWTYPE penmFirmwareType)
{
    di_node_t di_node;
    di_node = di_init("/", DINFOSUBTREE|DINFOPROP);
    if (di_node == DI_NODE_NIL)
    {
        *penmFirmwareType = RTSYSFWTYPE_INVALID;
        return VERR_NOT_SUPPORTED;
    }

    int64_t *efiprop;
    int rc = di_prop_lookup_int64(DDI_DEV_T_ANY, di_node, "efi-systab", &efiprop);
    if (rc == -1)
        *penmFirmwareType = RTSYSFWTYPE_BIOS;
    else
        *penmFirmwareType = RTSYSFWTYPE_UEFI;

    di_fini(di_node);

    return VINF_SUCCESS;
}
RT_EXPORT_SYMBOL(RTSystemQueryFirmwareType);


RTDECL(int) RTSystemQueryFirmwareBoolean(RTSYSFWBOOL enmBoolean, bool *pfValue)
{
    RT_NOREF(enmBoolean, pfValue);
    return VERR_NOT_SUPPORTED;
}
RT_EXPORT_SYMBOL(RTSystemQueryFirmwareBoolean);

