/* $Id$ */
/** @file
 * IPRT - strpbrk().
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
#include <iprt/string.h>


/**
 * Find the first occurrence of a character in pszChars in pszStr.
 *
 * @returns
 */
#ifdef IPRT_NO_CRT
char *strpbrk(const char *pszStr, const char *pszBreakChars)
#elif defined(_MSC_VER)
# if _MSC_VER >= 1400
_CRTIMP __checkReturn _CONST_RETURN char *  __cdecl strpbrk(__in_z const char *pszStr, __in_z const char *pszBreakChars)
# else
_CRTIMP char * __cdecl strpbrk(const char *pszStr, const char *pszBreakChars)
# endif
#elif defined(__WATCOMC__)
_WCRTLINK char *std::strpbrk(const char *pszStr, const char *pszBreakChars)
#else
char *strpbrk(const char *pszStr, const char *pszBreakChars)
# if defined(__THROW) && !defined(RT_OS_WINDOWS) && !defined(RT_OS_OS2)
    __THROW
# endif
#endif
{
    int chCur;
    while ((chCur = *pszStr++) != '\0')
    {
        int chBreak;
        const char *psz = pszBreakChars;
        while ((chBreak = *psz++) != '\0')
            if (chBreak == chCur)
                return (char *)(pszStr - 1);

    }
    return NULL;
}

