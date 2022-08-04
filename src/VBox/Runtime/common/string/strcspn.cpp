/* $Id$ */
/** @file
 * IPRT - CRT Strings, strcspn().
 */

/*
 * Copyright (C) 2006-2022 Oracle Corporation
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
#include "internal/iprt.h"
#include <iprt/string.h>


/**
 * strpbrk with a offset return instead of a pointer.
 */
#undef strcspn
size_t RT_NOCRT(strcspn)(const char *pszString, const char *pszBreakChars)
{
    const char * const pszStringStart = pszString;
    int                chCur;
    while ((chCur = *pszString++) != '\0')
    {
        int            chBreak;
        const char    *psz = pszBreakChars;
        while ((chBreak = *psz++) != '\0')
            if (chBreak == chCur)
                return (size_t)(pszString - pszStringStart + 1);

    }
    return (size_t)(pszString - pszStringStart);
}
RT_ALIAS_AND_EXPORT_NOCRT_SYMBOL(strcspn);

