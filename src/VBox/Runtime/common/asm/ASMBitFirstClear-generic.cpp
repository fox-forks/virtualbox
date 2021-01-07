/* $Id$ */
/** @file
 * IPRT - ASMBitFirstClear - generic C implementation.
 */

/*
 * Copyright (C) 2006-2021 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * The contents of this file may alternatively be used under the terms
 * of the Common Development and Distribution License Version 1.0
 * (CDDL) only, as it comes in the "COPYING.CDDL" file of the
 * VirtualBox OSE distribution, in which case the provisions of the
 * CDDL are applicable instead of those of the GPL.
 *
 * You may elect to license modified versions of this file under the
 * terms and conditions of either the GPL or the CDDL or both.
 */


/*********************************************************************************************************************************
*   Header Files                                                                                                                 *
*********************************************************************************************************************************/
#include <iprt/asm.h>
#include "internal/iprt.h"

#include <iprt/assert.h>


DECLASM(int32_t) ASMBitFirstClear(const volatile void RT_FAR *pvBitmap, uint32_t cBits) RT_NOTHROW_DEF
{
    const volatile size_t RT_FAR *pu = (const volatile size_t RT_FAR *)pvBitmap;
    Assert(!(cBits & 31));

    while (cBits >= sizeof(size_t) * 8)
    {
        size_t u = *pu;
        if (u == ~(size_t)0)
        { }
        else
        {
            size_t const iBaseBit = ((uintptr_t)pu - (uintptr_t)pvBitmap) * 8;
#if ARCH_BITS == 32
            return iBaseBit + ASMBitFirstSetU32(~RT_LE2H_U32(u)) - 1;
#elif ARCH_BITS == 64
            return iBaseBit + ASMBitFirstSetU64(~RT_LE2H_U64(u)) - 1;
#else
# error "ARCH_BITS or RT_BIG_ENDIAN/RT_LITTLE_ENDIAN is not supported" /** @todo figure out bitmaps on bigendian systems! */
#endif
        }

        pu++;
        cBits -= sizeof(size_t) * 8;
    }

#if ARCH_BITS > 32
    if (cBits >= 32)
    {
        uint32_t u32 = *(const volatile uint32_t RT_FAR *)pu;
        if (u32 != UINT32_MAX)
        {
            size_t const iBaseBit = ((uintptr_t)pu - (uintptr_t)pvBitmap) * 8;
            return iBaseBit + ASMBitFirstSetU32(~RT_LE2H_U32(u32)) - 1;
        }
    }
#endif

    return -1;
}

