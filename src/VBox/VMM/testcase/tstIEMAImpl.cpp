/* $Id$ */
/** @file
 * IEM Assembly Instruction Helper Testcase.
 */

/*
 * Copyright (C) 2022 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */


/*********************************************************************************************************************************
*   Header Files                                                                                                                 *
*********************************************************************************************************************************/
#include "../include/IEMInternal.h"

#include <iprt/errcore.h>
#include <VBox/log.h>
#include <iprt/assert.h>
#include <iprt/ctype.h>
#include <iprt/getopt.h>
#include <iprt/initterm.h>
#include <iprt/message.h>
#include <iprt/mp.h>
#include <iprt/rand.h>
#include <iprt/stream.h>
#include <iprt/string.h>
#include <iprt/test.h>


/*********************************************************************************************************************************
*   Structures and Typedefs                                                                                                      *
*********************************************************************************************************************************/
/** @name 8-bit binary (PFNIEMAIMPLBINU8)
 * @{ */
typedef struct BINU8_TEST_T
{
    uint32_t                fEflIn;
    uint32_t                fEflOut;
    uint8_t                 uDstIn;
    uint8_t                 uDstOut;
    uint8_t                 uSrcIn;
    uint8_t                 uMisc;
} BINU8_TEST_T;

typedef struct BINU8_T
{
    const char             *pszName;
    PFNIEMAIMPLBINU8        pfn;
    PFNIEMAIMPLBINU8        pfnNative;
    BINU8_TEST_T const     *paTests;
    uint32_t                cTests;
    uint32_t                uExtra;
    uint8_t                 idxCpuEflFlavour;
} BINU8_T;
/** @} */


/** @name 16-bit binary (PFNIEMAIMPLBINU16)
 * @{ */
typedef struct BINU16_TEST_T
{
    uint32_t                fEflIn;
    uint32_t                fEflOut;
    uint16_t                uDstIn;
    uint16_t                uDstOut;
    uint16_t                uSrcIn;
    uint16_t                uMisc;
} BINU16_TEST_T;

typedef struct BINU16_T
{
    const char             *pszName;
    PFNIEMAIMPLBINU16       pfn;
    PFNIEMAIMPLBINU16       pfnNative;
    BINU16_TEST_T const    *paTests;
    uint32_t                cTests;
    uint32_t                uExtra;
    uint8_t                 idxCpuEflFlavour;
} BINU16_T;
/** @} */


/** @name 32-bit binary (PFNIEMAIMPLBINU32)
 * @{ */
typedef struct BINU32_TEST_T
{
    uint32_t                fEflIn;
    uint32_t                fEflOut;
    uint32_t                uDstIn;
    uint32_t                uDstOut;
    uint32_t                uSrcIn;
    uint32_t                uMisc;
} BINU32_TEST_T;

typedef struct BINU32_T
{
    const char             *pszName;
    PFNIEMAIMPLBINU32       pfn;
    PFNIEMAIMPLBINU32       pfnNative;
    BINU32_TEST_T const    *paTests;
    uint32_t                cTests;
    uint32_t                uExtra;
    uint8_t                 idxCpuEflFlavour;
} BINU32_T;
/** @} */


/** @name 64-bit binary (PFNIEMAIMPLBINU64)
 * @{ */
typedef struct BINU64_TEST_T
{
    uint32_t                fEflIn;
    uint32_t                fEflOut;
    uint64_t                uDstIn;
    uint64_t                uDstOut;
    uint64_t                uSrcIn;
    uint64_t                uMisc;
} BINU64_TEST_T;

typedef struct BINU64_T
{
    const char             *pszName;
    PFNIEMAIMPLBINU64       pfn;
    PFNIEMAIMPLBINU64       pfnNative;
    BINU64_TEST_T const    *paTests;
    uint32_t                cTests;
    uint32_t                uExtra;
    uint8_t                 idxCpuEflFlavour;
} BINU64_T;
/** @} */


/** @name mult/div (PFNIEMAIMPLBINU8, PFNIEMAIMPLBINU16, PFNIEMAIMPLBINU32, PFNIEMAIMPLBINU64)
 * @{ */
typedef struct MULDIVU8_TEST_T
{
    uint32_t                fEflIn;
    uint32_t                fEflOut;
    uint16_t                uDstIn;
    uint16_t                uDstOut;
    uint8_t                 uSrcIn;
    int32_t                 rc;
} MULDIVU8_TEST_T;

typedef struct MULDIVU16_TEST_T
{
    uint32_t                fEflIn;
    uint32_t                fEflOut;
    uint16_t                uDst1In;
    uint16_t                uDst1Out;
    uint16_t                uDst2In;
    uint16_t                uDst2Out;
    uint16_t                uSrcIn;
    int32_t                 rc;
} MULDIVU16_TEST_T;

typedef struct MULDIVU32_TEST_T
{
    uint32_t                fEflIn;
    uint32_t                fEflOut;
    uint32_t                uDst1In;
    uint32_t                uDst1Out;
    uint32_t                uDst2In;
    uint32_t                uDst2Out;
    uint32_t                uSrcIn;
    int32_t                 rc;
} MULDIVU32_TEST_T;

typedef struct MULDIVU64_TEST_T
{
    uint32_t                fEflIn;
    uint32_t                fEflOut;
    uint64_t                uDst1In;
    uint64_t                uDst1Out;
    uint64_t                uDst2In;
    uint64_t                uDst2Out;
    uint64_t                uSrcIn;
    int32_t                 rc;
} MULDIVU64_TEST_T;
/** @} */


/*********************************************************************************************************************************
*   Defined Constants And Macros                                                                                                 *
*********************************************************************************************************************************/
#define ENTRY(a_Name)       ENTRY_EX(a_Name, 0)
#define ENTRY_EX(a_Name, a_uExtra) \
    { RT_XSTR(a_Name), iemAImpl_ ## a_Name, NULL, \
      g_aTests_ ## a_Name, RT_ELEMENTS(g_aTests_ ## a_Name), \
      a_uExtra, IEMTARGETCPU_EFL_BEHAVIOR_NATIVE /* means same for all here */ }

#define ENTRY_INTEL(a_Name, a_fEflUndef) ENTRY_INTEL_EX(a_Name, a_fEflUndef, 0)
#define ENTRY_INTEL_EX(a_Name, a_fEflUndef, a_uExtra) \
    { RT_XSTR(a_Name) "_intel", iemAImpl_ ## a_Name ## _intel, iemAImpl_ ## a_Name, \
      g_aTests_ ## a_Name ## _intel, RT_ELEMENTS(g_aTests_ ## a_Name ## _intel), \
      a_uExtra, IEMTARGETCPU_EFL_BEHAVIOR_INTEL }

#define ENTRY_AMD(a_Name, a_fEflUndef)   ENTRY_AMD_EX(a_Name, a_fEflUndef, 0)
#define ENTRY_AMD_EX(a_Name, a_fEflUndef, a_uExtra) \
    { RT_XSTR(a_Name) "_amd", iemAImpl_ ## a_Name ## _amd,   iemAImpl_ ## a_Name, \
      g_aTests_ ## a_Name ## _amd, RT_ELEMENTS(g_aTests_ ## a_Name ## _amd), \
      a_uExtra, IEMTARGETCPU_EFL_BEHAVIOR_AMD }


/*********************************************************************************************************************************
*   Global Variables                                                                                                             *
*********************************************************************************************************************************/
static RTTEST       g_hTest;
static uint8_t      g_idxCpuEflFlavour = IEMTARGETCPU_EFL_BEHAVIOR_INTEL;
#ifdef TSTIEMAIMPL_WITH_GENERATOR
static uint32_t     g_cZeroDstTests = 2;
static uint32_t     g_cZeroSrcTests = 4;
#endif
static uint8_t     *g_pu8,   *g_pu8Two;
static uint16_t    *g_pu16,  *g_pu16Two;
static uint32_t    *g_pu32,  *g_pu32Two,  *g_pfEfl;
static uint64_t    *g_pu64,  *g_pu64Two;
static RTUINT128U  *g_pu128, *g_pu128Two;

static char         g_aszBuf[16][256];
static unsigned     g_idxBuf = 0;


#include "tstIEMAImplData.h"
#include "tstIEMAImplData-Intel.h"
#include "tstIEMAImplData-Amd.h"


/*********************************************************************************************************************************
*   Internal Functions                                                                                                           *
*********************************************************************************************************************************/
static const char *FormatR80(PCRTFLOAT80U pr80);
static const char *FormatR64(PCRTFLOAT64U pr64);
static const char *FormatR32(PCRTFLOAT32U pr32);


/*
 * Random helpers.
 */

static uint32_t RandEFlags(void)
{
    uint32_t fEfl = RTRandU32();
    return (fEfl & X86_EFL_LIVE_MASK) | X86_EFL_RA1_MASK;
}


static uint8_t  RandU8(void)
{
    return RTRandU32Ex(0, 0xff);
}


static uint16_t  RandU16(void)
{
    return RTRandU32Ex(0, 0xffff);
}


static uint32_t  RandU32(void)
{
    return RTRandU32();
}


static uint64_t  RandU64(void)
{
    return RTRandU64();
}


static RTUINT128U RandU128(void)
{
    RTUINT128U Ret;
    Ret.s.Hi = RTRandU64();
    Ret.s.Lo = RTRandU64();
    return Ret;
}

#ifdef TSTIEMAIMPL_WITH_GENERATOR

static uint8_t  RandU8Dst(uint32_t iTest)
{
    if (iTest < g_cZeroDstTests)
        return 0;
    return RandU8();
}


static uint8_t  RandU8Src(uint32_t iTest)
{
    if (iTest < g_cZeroSrcTests)
        return 0;
    return RandU8();
}


static uint16_t  RandU16Dst(uint32_t iTest)
{
    if (iTest < g_cZeroDstTests)
        return 0;
    return RandU16();
}


static uint16_t  RandU16Src(uint32_t iTest)
{
    if (iTest < g_cZeroSrcTests)
        return 0;
    return RandU16();
}


static uint32_t  RandU32Dst(uint32_t iTest)
{
    if (iTest < g_cZeroDstTests)
        return 0;
    return RandU32();
}


static uint32_t  RandU32Src(uint32_t iTest)
{
    if (iTest < g_cZeroSrcTests)
        return 0;
    return RandU32();
}


static uint64_t  RandU64Dst(uint32_t iTest)
{
    if (iTest < g_cZeroDstTests)
        return 0;
    return RandU64();
}


static uint64_t  RandU64Src(uint32_t iTest)
{
    if (iTest < g_cZeroSrcTests)
        return 0;
    return RandU64();
}


static uint16_t  RandFcw(void)
{
    return RandU16() & ~X86_FCW_ZERO_MASK;
}


static uint16_t  RandFsw(void)
{
    AssertCompile((X86_FSW_C_MASK | X86_FSW_XCPT_ES_MASK | X86_FSW_TOP_MASK | X86_FSW_B) == 0xffff);
    return RandU16();
}


static void SafeR80FractionShift(PRTFLOAT80U pr80, uint8_t cShift)
{
    if (pr80->sj64.uFraction >= RT_BIT_64(cShift))
        pr80->sj64.uFraction >>= cShift;
    else
        pr80->sj64.uFraction = (cShift % 19) + 1;
}


static RTFLOAT80U RandR80Ex(unsigned cTarget = 80)
{
    Assert(cTarget == 80 || cTarget == 64 || cTarget == 32);

    RTFLOAT80U r80;
    r80.au64[0] = RandU64();
    r80.au16[4] = RandU16();

    /*
     * Make it more likely that we get a good selection of special values.
     */
    uint8_t bType = RandU8() & 0x1f;
    if (bType == 0 || bType == 1 || bType == 2 || bType == 3)
    {
        /* Zero (0), Pseudo-Infinity (1), Infinity (2), Indefinite (3). We only keep fSign here. */
        r80.sj64.uExponent     = bType == 0 ? 0 : 0x7fff;
        r80.sj64.uFraction     = bType <= 2 ? 0 : RT_BIT_64(62);
        r80.sj64.fInteger      = bType >= 2 ? 1 : 0;
        AssertMsg(bType != 0 || RTFLOAT80U_IS_ZERO(&r80),       ("%s\n", FormatR80(&r80)));
        AssertMsg(bType != 1 || RTFLOAT80U_IS_PSEUDO_INF(&r80), ("%s\n", FormatR80(&r80)));
        AssertMsg(bType != 2 || RTFLOAT80U_IS_INF(&r80),        ("%s\n", FormatR80(&r80)));
        AssertMsg(bType != 3 || RTFLOAT80U_IS_INDEFINITE(&r80), ("%s\n", FormatR80(&r80)));
    }
    else if (bType == 4 || bType == 5 || bType == 6 || bType == 7)
    {
        /* Denormals (4,5) and Pseudo denormals (6,7) */
        if (bType & 1)
            SafeR80FractionShift(&r80, r80.sj64.uExponent % 62);
        else if (r80.sj64.uFraction == 0 && bType < 6)
            r80.sj64.uFraction = RTRandU64Ex(1, RT_BIT_64(RTFLOAT80U_FRACTION_BITS) - 1);
        r80.sj64.uExponent = 0;
        r80.sj64.fInteger  = bType >= 6;
        AssertMsg(bType >= 6 || RTFLOAT80U_IS_DENORMAL(&r80),        ("%s bType=%#x\n", FormatR80(&r80), bType));
        AssertMsg(bType < 6  || RTFLOAT80U_IS_PSEUDO_DENORMAL(&r80), ("%s bType=%#x\n", FormatR80(&r80), bType));
    }
    else if (bType == 8 || bType == 9)
    {
        /* Pseudo NaN. */
        if (bType & 1)
            SafeR80FractionShift(&r80, r80.sj64.uExponent % 62);
        else if (r80.sj64.uFraction == 0 && !r80.sj64.fInteger)
            r80.sj64.uFraction = RTRandU64Ex(1, RT_BIT_64(RTFLOAT80U_FRACTION_BITS) - 1);
        r80.sj64.uExponent = 0x7fff;
        if (r80.sj64.fInteger)
            r80.sj64.uFraction |= RT_BIT_64(62);
        else
            r80.sj64.uFraction &= ~RT_BIT_64(62);
        r80.sj64.fInteger  = 0;
        AssertMsg(RTFLOAT80U_IS_PSEUDO_NAN(&r80), ("%s bType=%#x\n", FormatR80(&r80), bType));
        AssertMsg(RTFLOAT80U_IS_NAN(&r80),        ("%s bType=%#x\n", FormatR80(&r80), bType));
    }
    else if (bType == 10 || bType == 11)
    {
        /* Quiet and signalling NaNs (using fInteger to pick which). */
        if (bType & 1)
            SafeR80FractionShift(&r80, r80.sj64.uExponent % 62);
        else if (r80.sj64.uFraction == 0)
            r80.sj64.uFraction = RTRandU64Ex(1, RT_BIT_64(RTFLOAT80U_FRACTION_BITS) - 1);
        r80.sj64.uExponent = 0x7fff;
        if (r80.sj64.fInteger)
            r80.sj64.uFraction |= RT_BIT_64(62);
        else
            r80.sj64.uFraction &= ~RT_BIT_64(62);
        r80.sj64.fInteger  = 1;
        AssertMsg(RTFLOAT80U_IS_SIGNALLING_NAN(&r80) || RTFLOAT80U_IS_QUIET_NAN(&r80), ("%s\n", FormatR80(&r80)));
        AssertMsg(RTFLOAT80U_IS_NAN(&r80), ("%s\n", FormatR80(&r80)));
    }
    else if (bType == 12 || bType == 13)
    {
        /* Unnormals */
        if (bType & 1)
            SafeR80FractionShift(&r80, RandU8() % 62);
        r80.sj64.fInteger  = 0;
        AssertMsg(RTFLOAT80U_IS_UNNORMAL(&r80), ("%s\n", FormatR80(&r80)));
    }
    else if (bType < 24)
    {
        /* Make sure we have lots of normalized values. */
        const unsigned uMinExp = cTarget == 64 ? RTFLOAT80U_EXP_BIAS - RTFLOAT64U_EXP_BIAS
                               : cTarget == 32 ? RTFLOAT80U_EXP_BIAS - RTFLOAT32U_EXP_BIAS : 0;
        const unsigned uMaxExp = cTarget == 64 ? uMinExp + RTFLOAT64U_EXP_MAX
                               : cTarget == 32 ? uMinExp + RTFLOAT32U_EXP_MAX : RTFLOAT80U_EXP_MAX;
        r80.sj64.fInteger = 1;
        if (r80.sj64.uExponent <= uMinExp)
            r80.sj64.uExponent = uMinExp + 1;
        else if (r80.sj64.uExponent >= uMaxExp)
            r80.sj64.uExponent = uMaxExp - 1;

        if (bType == 14)
        {   /* All 1s is useful to testing rounding. Also try trigger special
               behaviour by sometimes rounding out of range, while we're at it. */
            r80.sj64.uFraction = RT_BIT_64(63) - 1;
            uint8_t bExp = RandU8();
            if ((bExp & 3) == 0)
                r80.sj64.uExponent = uMaxExp - 1;
            else if ((bExp & 3) == 1)
                r80.sj64.uExponent = uMinExp + 1;
            else if ((bExp & 3) == 2)
                r80.sj64.uExponent = uMinExp - (bExp & 15); /* (small numbers are mapped to subnormal values) */
        }

        AssertMsg(RTFLOAT80U_IS_NORMAL(&r80), ("%s\n", FormatR80(&r80)));
    }
    return r80;
}


static RTFLOAT80U RandR80Src(uint32_t iTest)
{
    RT_NOREF(iTest);
    return RandR80Ex();
}


static void SafeR64FractionShift(PRTFLOAT64U pr64, uint8_t cShift)
{
    if (pr64->s64.uFraction >= RT_BIT_64(cShift))
        pr64->s64.uFraction >>= cShift;
    else
        pr64->s64.uFraction = (cShift % 19) + 1;
}


static RTFLOAT64U RandR64Src(uint32_t iTest)
{
    RT_NOREF(iTest);

    RTFLOAT64U r64;
    r64.u = RandU64();

    /*
     * Make it more likely that we get a good selection of special values.
     * On average 6 out of 16 calls should return a special value.
     */
    uint8_t bType = RandU8() & 0xf;
    if (bType == 0 || bType == 1)
    {
        /* 0 or Infinity. We only keep fSign here. */
        r64.s.uExponent     = bType == 0 ? 0 : 0x7ff;
        r64.s.uFractionHigh = 0;
        r64.s.uFractionLow  = 0;
        AssertMsg(bType != 0 || RTFLOAT64U_IS_ZERO(&r64), ("%s bType=%#x\n", FormatR64(&r64), bType));
        AssertMsg(bType != 1 || RTFLOAT64U_IS_INF(&r64),  ("%s bType=%#x\n", FormatR64(&r64), bType));
    }
    else if (bType == 2 || bType == 3)
    {
        /* Subnormals */
        if (bType == 3)
            SafeR64FractionShift(&r64, r64.s64.uExponent % 51);
        else if (r64.s64.uFraction == 0)
            r64.s64.uFraction = RTRandU64Ex(1, RT_BIT_64(RTFLOAT64U_FRACTION_BITS) - 1);
        r64.s64.uExponent = 0;
        AssertMsg(RTFLOAT64U_IS_SUBNORMAL(&r64), ("%s bType=%#x\n", FormatR64(&r64), bType));
    }
    else if (bType == 4 || bType == 5)
    {
        /* NaNs */
        if (bType == 5)
            SafeR64FractionShift(&r64, r64.s64.uExponent % 51);
        else if (r64.s64.uFraction == 0)
            r64.s64.uFraction = RTRandU64Ex(1, RT_BIT_64(RTFLOAT64U_FRACTION_BITS) - 1);
        r64.s64.uExponent = 0x7ff;
        AssertMsg(RTFLOAT64U_IS_NAN(&r64), ("%s bType=%#x\n", FormatR64(&r64), bType));
    }
    else if (bType < 12)
    {
        /* Make sure we have lots of normalized values. */
        if (r64.s.uExponent == 0)
            r64.s.uExponent = 1;
        else if (r64.s.uExponent == 0x7ff)
            r64.s.uExponent = 0x7fe;
        AssertMsg(RTFLOAT64U_IS_NORMAL(&r64), ("%s bType=%#x\n", FormatR64(&r64), bType));
    }
    return r64;
}


static void SafeR32FractionShift(PRTFLOAT32U pr32, uint8_t cShift)
{
    if (pr32->s.uFraction >= RT_BIT_32(cShift))
        pr32->s.uFraction >>= cShift;
    else
        pr32->s.uFraction = (cShift % 19) + 1;
}


static RTFLOAT32U RandR32Src(uint32_t iTest)
{
    RT_NOREF(iTest);

    RTFLOAT32U r32;
    r32.u = RandU32();

    /*
     * Make it more likely that we get a good selection of special values.
     * On average 6 out of 16 calls should return a special value.
     */
    uint8_t bType = RandU8() & 0xf;
    if (bType == 0 || bType == 1)
    {
        /* 0 or Infinity. We only keep fSign here. */
        r32.s.uExponent = bType == 0 ? 0 : 0xff;
        r32.s.uFraction = 0;
        AssertMsg(bType != 0 || RTFLOAT32U_IS_ZERO(&r32), ("%s\n", FormatR32(&r32)));
        AssertMsg(bType != 1 || RTFLOAT32U_IS_INF(&r32), ("%s\n", FormatR32(&r32)));
    }
    else if (bType == 2 || bType == 3)
    {
        /* Subnormals */
        if (bType == 3)
            SafeR32FractionShift(&r32, r32.s.uExponent % 22);
        else if (r32.s.uFraction == 0)
            r32.s.uFraction = RTRandU32Ex(1, RT_BIT_32(RTFLOAT32U_FRACTION_BITS) - 1);
        r32.s.uExponent = 0;
        AssertMsg(RTFLOAT32U_IS_SUBNORMAL(&r32), ("%s bType=%#x\n", FormatR32(&r32), bType));
    }
    else if (bType == 4 || bType == 5)
    {
        /* NaNs */
        if (bType == 5)
            SafeR32FractionShift(&r32, r32.s.uExponent % 22);
        else if (r32.s.uFraction == 0)
            r32.s.uFraction = RTRandU32Ex(1, RT_BIT_32(RTFLOAT32U_FRACTION_BITS) - 1);
        r32.s.uExponent = 0xff;
        AssertMsg(RTFLOAT32U_IS_NAN(&r32), ("%s bType=%#x\n", FormatR32(&r32), bType));
    }
    else if (bType < 12)
    {
        /* Make sure we have lots of normalized values. */
        if (r32.s.uExponent == 0)
            r32.s.uExponent = 1;
        else if (r32.s.uExponent == 0xff)
            r32.s.uExponent = 0xfe;
        AssertMsg(RTFLOAT32U_IS_NORMAL(&r32), ("%s bType=%#x\n", FormatR32(&r32), bType));
    }
    return r32;
}


static RTPBCD80U RandD80Src(uint32_t iTest)
{
    if (iTest < 3)
    {
        RTPBCD80U d80Zero = RTPBCD80U_INIT_ZERO(!(iTest & 1));
        return d80Zero;
    }
    if (iTest < 5)
    {
        RTPBCD80U d80Ind = RTPBCD80U_INIT_INDEFINITE();
        return d80Ind;
    }

    RTPBCD80U d80;
    uint8_t b = RandU8();
    d80.s.fSign = b & 1;

    if ((iTest & 7) >= 6)
    {
        /* Illegal */
        d80.s.uPad = (iTest & 7) == 7 ? b >> 1 : 0;
        for (size_t iPair = 0; iPair < RT_ELEMENTS(d80.s.abPairs); iPair++)
            d80.s.abPairs[iPair] = RandU8();
    }
    else
    {
        /* Normal */
        d80.s.uPad = 0;
        for (size_t iPair = 0; iPair < RT_ELEMENTS(d80.s.abPairs); iPair++)
        {
            uint8_t const uLo = (uint8_t)RTRandU32Ex(0, 9);
            uint8_t const uHi = (uint8_t)RTRandU32Ex(0, 9);
            d80.s.abPairs[iPair] = RTPBCD80U_MAKE_PAIR(uHi, uLo);
        }
    }
    return d80;
}


const char *GenFormatR80(PCRTFLOAT80U plrd)
{
    char *pszBuf = g_aszBuf[g_idxBuf++ % RT_ELEMENTS(g_aszBuf)];
    RTStrPrintf(pszBuf, sizeof(g_aszBuf[0]), "RTFLOAT80U_INIT_C(%d,%#RX64,%u)",
                plrd->s.fSign, plrd->s.uMantissa, plrd->s.uExponent);
    return pszBuf;
}

const char *GenFormatR64(PCRTFLOAT64U prd)
{
    char *pszBuf = g_aszBuf[g_idxBuf++ % RT_ELEMENTS(g_aszBuf)];
    RTStrPrintf(pszBuf, sizeof(g_aszBuf[0]), "RTFLOAT64U_INIT_C(%d,%#RX64,%u)",
                prd->s.fSign, RT_MAKE_U64(prd->s.uFractionLow, prd->s.uFractionHigh), prd->s.uExponent);
    return pszBuf;
}


const char *GenFormatR32(PCRTFLOAT32U pr)
{
    char *pszBuf = g_aszBuf[g_idxBuf++ % RT_ELEMENTS(g_aszBuf)];
    RTStrPrintf(pszBuf, sizeof(g_aszBuf[0]), "RTFLOAT32U_INIT_C(%d,%#RX32,%u)", pr->s.fSign, pr->s.uFraction, pr->s.uExponent);
    return pszBuf;
}


const char *GenFormatD80(PCRTPBCD80U pd80)
{
    char *pszBuf = g_aszBuf[g_idxBuf++ % RT_ELEMENTS(g_aszBuf)];
    size_t off;
    if (pd80->s.uPad == 0)
        off = RTStrPrintf(pszBuf, sizeof(g_aszBuf[0]), "RTPBCD80U_INIT_C(%d", pd80->s.fSign);
    else
        off = RTStrPrintf(pszBuf, sizeof(g_aszBuf[0]), "RTPBCD80U_INIT_EX_C(%#x,%d", pd80->s.uPad, pd80->s.fSign);
    size_t iPair = RT_ELEMENTS(pd80->s.abPairs);
    while (iPair-- > 0)
        off += RTStrPrintf(&pszBuf[off], sizeof(g_aszBuf[0]) - off, ",%d,%d",
                           RTPBCD80U_HI_DIGIT(pd80->s.abPairs[iPair]),
                           RTPBCD80U_LO_DIGIT(pd80->s.abPairs[iPair]));
    pszBuf[off++] = ')';
    pszBuf[off++] = '\0';
    return pszBuf;
}


const char *GenFormatI64(int64_t i64)
{
    if (i64 == INT64_MIN) /* This one is problematic */
        return "INT64_MIN";
    if (i64 == INT64_MAX)
        return "INT64_MAX";
    char *pszBuf = g_aszBuf[g_idxBuf++ % RT_ELEMENTS(g_aszBuf)];
    RTStrPrintf(pszBuf, sizeof(g_aszBuf[0]), "INT64_C(%RI64)", i64);
    return pszBuf;
}


const char *GenFormatI32(int32_t i32)
{
    if (i32 == INT32_MIN) /* This one is problematic */
        return "INT32_MIN";
    if (i32 == INT32_MAX)
        return "INT32_MAX";
    char *pszBuf = g_aszBuf[g_idxBuf++ % RT_ELEMENTS(g_aszBuf)];
    RTStrPrintf(pszBuf, sizeof(g_aszBuf[0]), "INT32_C(%RI32)", i32);
    return pszBuf;
}


const char *GenFormatI16(int16_t i16)
{
    if (i16 == INT16_MIN) /* This one is problematic */
        return "INT16_MIN";
    if (i16 == INT16_MAX)
        return "INT16_MAX";
    char *pszBuf = g_aszBuf[g_idxBuf++ % RT_ELEMENTS(g_aszBuf)];
    RTStrPrintf(pszBuf, sizeof(g_aszBuf[0]), "INT16_C(%RI16)", i16);
    return pszBuf;
}


static void GenerateHeader(PRTSTREAM pOut, const char *pszFileInfix,
                           const char *pszCpuDesc, const char *pszCpuType, const char *pszCpuSuffU)
{
    /* We want to tag the generated source code with the revision that produced it. */
    static char s_szRev[] = "$Revision$";
    const char *pszRev = RTStrStripL(strchr(s_szRev, ':') + 1);
    size_t      cchRev = 0;
    while (RT_C_IS_DIGIT(pszRev[cchRev]))
        cchRev++;

    RTStrmPrintf(pOut,
                 "/* $Id$ */\n"
                 "/** @file\n"
                 " * IEM Assembly Instruction Helper Testcase Data%s%s - r%.*s on %s.\n"
                 " */\n"
                 "\n"
                 "/*\n"
                 " * Copyright (C) 2022 Oracle Corporation\n"
                 " *\n"
                 " * This file is part of VirtualBox Open Source Edition (OSE), as\n"
                 " * available from http://www.virtualbox.org. This file is free software;\n"
                 " * you can redistribute it and/or modify it under the terms of the GNU\n"
                 " * General Public License (GPL) as published by the Free Software\n"
                 " * Foundation, in version 2 as it comes in the \"COPYING\" file of the\n"
                 " * VirtualBox OSE distribution. VirtualBox OSE is distributed in the\n"
                 " * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.\n"
                 " */\n"
                 "\n"
                 "#ifndef VMM_INCLUDED_SRC_testcase_tstIEMAImplData%s%s_h\n"
                 "#define VMM_INCLUDED_SRC_testcase_tstIEMAImplData%s%s_h\n"
                 "#ifndef RT_WITHOUT_PRAGMA_ONCE\n"
                 "# pragma once\n"
                 "#endif\n"
                 ,
                 pszCpuType ? " " : "", pszCpuType ? pszCpuType : "", cchRev, pszRev, pszCpuDesc,
                 pszFileInfix, pszCpuSuffU,
                 pszFileInfix, pszCpuSuffU);
}


static RTEXITCODE GenerateFooterAndClose(PRTSTREAM pOut, const char *pszFilename, const char *pszFileInfix,
                                         const char *pszCpuSuff, RTEXITCODE rcExit)
{
    RTStrmPrintf(pOut,
                 "\n"
                 "#endif /* !VMM_INCLUDED_SRC_testcase_tstIEMAImplData%s%s_h */\n", pszFileInfix, pszCpuSuff);
    int rc = RTStrmClose(pOut);
    if (RT_SUCCESS(rc))
        return rcExit;
    return RTMsgErrorExitFailure("RTStrmClose failed on %s: %Rrc", pszFilename, rc);
}

#endif


/*
 * Test helpers.
 */
static const char *EFlagsDiff(uint32_t fActual, uint32_t fExpected)
{
    if (fActual == fExpected)
        return "";

    uint32_t const fXor = fActual ^ fExpected;
    char *pszBuf = g_aszBuf[g_idxBuf++ % RT_ELEMENTS(g_aszBuf)];
    size_t cch = RTStrPrintf(pszBuf, sizeof(g_aszBuf[0]), " - %#x", fXor);

    static struct
    {
        const char *pszName;
        uint32_t    fFlag;
    } const s_aFlags[] =
    {
#define EFL_ENTRY(a_Flags) { #a_Flags, X86_EFL_ ## a_Flags }
        EFL_ENTRY(CF),
        EFL_ENTRY(PF),
        EFL_ENTRY(AF),
        EFL_ENTRY(ZF),
        EFL_ENTRY(SF),
        EFL_ENTRY(TF),
        EFL_ENTRY(IF),
        EFL_ENTRY(DF),
        EFL_ENTRY(OF),
        EFL_ENTRY(IOPL),
        EFL_ENTRY(NT),
        EFL_ENTRY(RF),
        EFL_ENTRY(VM),
        EFL_ENTRY(AC),
        EFL_ENTRY(VIF),
        EFL_ENTRY(VIP),
        EFL_ENTRY(ID),
    };
    for (size_t i = 0; i < RT_ELEMENTS(s_aFlags); i++)
        if (s_aFlags[i].fFlag & fXor)
            cch += RTStrPrintf(&pszBuf[cch], sizeof(g_aszBuf[0]) - cch,
                               s_aFlags[i].fFlag & fActual ? "/%s" : "/!%s", s_aFlags[i].pszName);
    RTStrPrintf(&pszBuf[cch], sizeof(g_aszBuf[0]) - cch, "");
    return pszBuf;
}


static const char *FswDiff(uint16_t fActual, uint16_t fExpected)
{
    if (fActual == fExpected)
        return "";

    uint16_t const fXor = fActual ^ fExpected;
    char *pszBuf = g_aszBuf[g_idxBuf++ % RT_ELEMENTS(g_aszBuf)];
    size_t cch = RTStrPrintf(pszBuf, sizeof(g_aszBuf[0]), " - %#x", fXor);

    static struct
    {
        const char *pszName;
        uint32_t    fFlag;
    } const s_aFlags[] =
    {
#define FSW_ENTRY(a_Flags) { #a_Flags, X86_FSW_ ## a_Flags }
        FSW_ENTRY(IE),
        FSW_ENTRY(DE),
        FSW_ENTRY(ZE),
        FSW_ENTRY(OE),
        FSW_ENTRY(UE),
        FSW_ENTRY(PE),
        FSW_ENTRY(SF),
        FSW_ENTRY(ES),
        FSW_ENTRY(C0),
        FSW_ENTRY(C1),
        FSW_ENTRY(C2),
        FSW_ENTRY(C3),
        FSW_ENTRY(B),
    };
    for (size_t i = 0; i < RT_ELEMENTS(s_aFlags); i++)
        if (s_aFlags[i].fFlag & fXor)
            cch += RTStrPrintf(&pszBuf[cch], sizeof(g_aszBuf[0]) - cch,
                               s_aFlags[i].fFlag & fActual ? "/%s" : "/!%s", s_aFlags[i].pszName);
    if (fXor & X86_FSW_TOP_MASK)
        cch += RTStrPrintf(&pszBuf[cch], sizeof(g_aszBuf[0]) - cch, "/TOP%u!%u",
                           X86_FSW_TOP_GET(fActual), X86_FSW_TOP_GET(fExpected));
    RTStrPrintf(&pszBuf[cch], sizeof(g_aszBuf[0]) - cch, "");
    return pszBuf;
}


static const char *FormatFcw(uint16_t fFcw)
{
    char *pszBuf = g_aszBuf[g_idxBuf++ % RT_ELEMENTS(g_aszBuf)];

    const char *pszPC = NULL; /* (msc+gcc are too stupid) */
    switch (fFcw & X86_FCW_PC_MASK)
    {
        case X86_FCW_PC_24:     pszPC = "PC24"; break;
        case X86_FCW_PC_RSVD:   pszPC = "PCRSVD!"; break;
        case X86_FCW_PC_53:     pszPC = "PC53"; break;
        case X86_FCW_PC_64:     pszPC = "PC64"; break;
    }

    const char *pszRC = NULL; /* (msc+gcc are too stupid) */
    switch (fFcw & X86_FCW_RC_MASK)
    {
        case X86_FCW_RC_NEAREST:    pszRC = "NEAR"; break;
        case X86_FCW_RC_DOWN:       pszRC = "DOWN"; break;
        case X86_FCW_RC_UP:         pszRC = "UP"; break;
        case X86_FCW_RC_ZERO:       pszRC = "ZERO"; break;
    }
    size_t cch = RTStrPrintf(&pszBuf[0], sizeof(g_aszBuf[0]), "%s %s", pszPC, pszRC);

    static struct
    {
        const char *pszName;
        uint32_t    fFlag;
    } const s_aFlags[] =
    {
#define FCW_ENTRY(a_Flags) { #a_Flags, X86_FCW_ ## a_Flags }
        FCW_ENTRY(IM),
        FCW_ENTRY(DM),
        FCW_ENTRY(ZM),
        FCW_ENTRY(OM),
        FCW_ENTRY(UM),
        FCW_ENTRY(PM),
        { "6M", 64 },
    };
    for (size_t i = 0; i < RT_ELEMENTS(s_aFlags); i++)
        if (fFcw & s_aFlags[i].fFlag)
            cch += RTStrPrintf(&pszBuf[cch], sizeof(g_aszBuf[0]) - cch, " %s", s_aFlags[i].pszName);

    RTStrPrintf(&pszBuf[cch], sizeof(g_aszBuf[0]) - cch, "");
    return pszBuf;
}


static const char *FormatR80(PCRTFLOAT80U pr80)
{
    char *pszBuf = g_aszBuf[g_idxBuf++ % RT_ELEMENTS(g_aszBuf)];
    RTStrFormatR80(pszBuf, sizeof(g_aszBuf[0]), pr80, 0, 0, RTSTR_F_SPECIAL);
    return pszBuf;
}


static const char *FormatR64(PCRTFLOAT64U pr64)
{
    char *pszBuf = g_aszBuf[g_idxBuf++ % RT_ELEMENTS(g_aszBuf)];
    RTStrFormatR64(pszBuf, sizeof(g_aszBuf[0]), pr64, 0, 0, RTSTR_F_SPECIAL);
    return pszBuf;
}


static const char *FormatR32(PCRTFLOAT32U pr32)
{
    char *pszBuf = g_aszBuf[g_idxBuf++ % RT_ELEMENTS(g_aszBuf)];
    RTStrFormatR32(pszBuf, sizeof(g_aszBuf[0]), pr32, 0, 0, RTSTR_F_SPECIAL);
    return pszBuf;
}


static const char *FormatD80(PCRTPBCD80U pd80)
{
    /* There is only one indefinite endcoding (same as for 80-bit
       floating point), so get it out of the way first: */
    if (RTPBCD80U_IS_INDEFINITE(pd80))
        return "Ind";

    char *pszBuf = g_aszBuf[g_idxBuf++ % RT_ELEMENTS(g_aszBuf)];
    size_t off = 0;
    pszBuf[off++] = pd80->s.fSign ? '-' : '+';
    unsigned cBadDigits = 0;
    size_t   iPair      = RT_ELEMENTS(pd80->s.abPairs);
    while (iPair-- > 0)
    {
        static const char    s_szDigits[]   = "0123456789abcdef";
        static const uint8_t s_bBadDigits[] = { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 1, 1,  1, 1, 1, 1 };
        pszBuf[off++] = s_szDigits[RTPBCD80U_HI_DIGIT(pd80->s.abPairs[iPair])];
        pszBuf[off++] = s_szDigits[RTPBCD80U_LO_DIGIT(pd80->s.abPairs[iPair])];
        cBadDigits += s_bBadDigits[RTPBCD80U_HI_DIGIT(pd80->s.abPairs[iPair])]
                    + s_bBadDigits[RTPBCD80U_LO_DIGIT(pd80->s.abPairs[iPair])];
    }
    if (cBadDigits || pd80->s.uPad != 0)
        off += RTStrPrintf(&pszBuf[off], sizeof(g_aszBuf[0]) - off, "[%u,%#x]", cBadDigits, pd80->s.uPad);
    pszBuf[off] = '\0';
    return pszBuf;
}


/*
 * Binary operations.
 */
#ifdef TSTIEMAIMPL_WITH_GENERATOR
# define GEN_BINARY_TESTS(a_cBits, a_Fmt) \
static void BinU ## a_cBits ## Generate(PRTSTREAM pOut, PRTSTREAM pOutCpu, const char *pszCpuSuffU, uint32_t cTests) \
{ \
    RTStrmPrintf(pOut, "\n\n#define HAVE_BINU%u_TESTS\n", a_cBits); \
    RTStrmPrintf(pOutCpu, "\n\n#define HAVE_BINU%u_TESTS%s\n", a_cBits, pszCpuSuffU); \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aBinU ## a_cBits); iFn++) \
    { \
        PFNIEMAIMPLBINU ## a_cBits const pfn    = g_aBinU ## a_cBits[iFn].pfnNative \
                                                ? g_aBinU ## a_cBits[iFn].pfnNative : g_aBinU ## a_cBits[iFn].pfn; \
        PRTSTREAM                        pOutFn = pOut; \
        if (g_aBinU ## a_cBits[iFn].idxCpuEflFlavour != IEMTARGETCPU_EFL_BEHAVIOR_NATIVE) \
        { \
            if (g_aBinU ## a_cBits[iFn].idxCpuEflFlavour != g_idxCpuEflFlavour) \
                continue; \
            pOutFn = pOutCpu; \
        } \
        \
        RTStrmPrintf(pOutFn, "static const BINU%u_TEST_T g_aTests_%s[] =\n{\n", a_cBits, g_aBinU ## a_cBits[iFn].pszName); \
        for (uint32_t iTest = 0; iTest < cTests; iTest++ ) \
        { \
            BINU ## a_cBits ## _TEST_T Test; \
            Test.fEflIn    = RandEFlags(); \
            Test.fEflOut   = Test.fEflIn; \
            Test.uDstIn    = RandU ## a_cBits ## Dst(iTest); \
            Test.uDstOut   = Test.uDstIn; \
            Test.uSrcIn    = RandU ## a_cBits ## Src(iTest); \
            if (g_aBinU ## a_cBits[iFn].uExtra) \
                Test.uSrcIn &= a_cBits - 1; /* Restrict bit index according to operand width */ \
            Test.uMisc     = 0; \
            pfn(&Test.uDstOut, Test.uSrcIn, &Test.fEflOut); \
            RTStrmPrintf(pOutFn, "    { %#08x, %#08x, " a_Fmt ", " a_Fmt ", " a_Fmt ", %#x }, /* #%u */\n", \
                         Test.fEflIn, Test.fEflOut, Test.uDstIn, Test.uDstOut, Test.uSrcIn, Test.uMisc, iTest); \
        } \
        RTStrmPrintf(pOutFn, "};\n"); \
    } \
}
#else
# define GEN_BINARY_TESTS(a_cBits, a_Fmt)
#endif

#define TEST_BINARY_OPS(a_cBits, a_uType, a_Fmt, a_aSubTests) \
GEN_BINARY_TESTS(a_cBits, a_Fmt) \
\
static void BinU ## a_cBits ## Test(void) \
{ \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(a_aSubTests); iFn++) \
    { \
        RTTestSub(g_hTest, a_aSubTests[iFn].pszName); \
        BINU ## a_cBits ## _TEST_T const * const paTests = a_aSubTests[iFn].paTests; \
        uint32_t const                           cTests  = a_aSubTests[iFn].cTests; \
        PFNIEMAIMPLBINU ## a_cBits               pfn     = a_aSubTests[iFn].pfn; \
        uint32_t const cVars = 1 + (a_aSubTests[iFn].idxCpuEflFlavour == g_idxCpuEflFlavour && a_aSubTests[iFn].pfnNative); \
        for (uint32_t iVar = 0; iVar < cVars; iVar++) \
        { \
            for (uint32_t iTest = 0; iTest < cTests; iTest++ ) \
            { \
                uint32_t fEfl = paTests[iTest].fEflIn; \
                a_uType  uDst = paTests[iTest].uDstIn; \
                pfn(&uDst, paTests[iTest].uSrcIn, &fEfl); \
                if (   uDst != paTests[iTest].uDstOut \
                    || fEfl != paTests[iTest].fEflOut) \
                    RTTestFailed(g_hTest, "#%u%s: efl=%#08x dst=" a_Fmt " src=" a_Fmt " -> efl=%#08x dst=" a_Fmt ", expected %#08x & " a_Fmt "%s - %s\n", \
                                 iTest, !iVar ? "" : "/n", paTests[iTest].fEflIn, paTests[iTest].uDstIn, paTests[iTest].uSrcIn, \
                                 fEfl, uDst, paTests[iTest].fEflOut, paTests[iTest].uDstOut, \
                                 EFlagsDiff(fEfl, paTests[iTest].fEflOut), \
                                 uDst == paTests[iTest].uDstOut ? "eflags" : fEfl == paTests[iTest].fEflOut ? "dst" : "both"); \
                else \
                { \
                     *g_pu ## a_cBits  = paTests[iTest].uDstIn; \
                     *g_pfEfl = paTests[iTest].fEflIn; \
                     pfn(g_pu ## a_cBits, paTests[iTest].uSrcIn, g_pfEfl); \
                     RTTEST_CHECK(g_hTest, *g_pu ## a_cBits == paTests[iTest].uDstOut); \
                     RTTEST_CHECK(g_hTest, *g_pfEfl         == paTests[iTest].fEflOut); \
                } \
            } \
            pfn = a_aSubTests[iFn].pfnNative; \
        } \
    } \
}


/*
 * 8-bit binary operations.
 */

#ifndef HAVE_BINU8_TESTS
static const BINU8_TEST_T g_aTests_add_u8[]         = { {0} };
static const BINU8_TEST_T g_aTests_add_u8_locked[]  = { {0} };
static const BINU8_TEST_T g_aTests_adc_u8[]         = { {0} };
static const BINU8_TEST_T g_aTests_adc_u8_locked[]  = { {0} };
static const BINU8_TEST_T g_aTests_sub_u8[]         = { {0} };
static const BINU8_TEST_T g_aTests_sub_u8_locked[]  = { {0} };
static const BINU8_TEST_T g_aTests_sbb_u8[]         = { {0} };
static const BINU8_TEST_T g_aTests_sbb_u8_locked[]  = { {0} };
static const BINU8_TEST_T g_aTests_or_u8[]          = { {0} };
static const BINU8_TEST_T g_aTests_or_u8_locked[]   = { {0} };
static const BINU8_TEST_T g_aTests_xor_u8[]         = { {0} };
static const BINU8_TEST_T g_aTests_xor_u8_locked[]  = { {0} };
static const BINU8_TEST_T g_aTests_and_u8[]         = { {0} };
static const BINU8_TEST_T g_aTests_and_u8_locked[]  = { {0} };
static const BINU8_TEST_T g_aTests_cmp_u8[]         = { {0} };
static const BINU8_TEST_T g_aTests_test_u8[]        = { {0} };
#endif

static const BINU8_T g_aBinU8[] =
{
    ENTRY(add_u8),
    ENTRY(add_u8_locked),
    ENTRY(adc_u8),
    ENTRY(adc_u8_locked),
    ENTRY(sub_u8),
    ENTRY(sub_u8_locked),
    ENTRY(sbb_u8),
    ENTRY(sbb_u8_locked),
    ENTRY(or_u8),
    ENTRY(or_u8_locked),
    ENTRY(xor_u8),
    ENTRY(xor_u8_locked),
    ENTRY(and_u8),
    ENTRY(and_u8_locked),
    ENTRY(cmp_u8),
    ENTRY(test_u8),
};

TEST_BINARY_OPS(8, uint8_t, "%#04x", g_aBinU8)


/*
 * 16-bit binary operations.
 */

#ifndef HAVE_BINU16_TESTS
static const BINU16_TEST_T g_aTests_add_u16[]        = { {0} };
static const BINU16_TEST_T g_aTests_add_u16_locked[] = { {0} };
static const BINU16_TEST_T g_aTests_adc_u16[]        = { {0} };
static const BINU16_TEST_T g_aTests_adc_u16_locked[] = { {0} };
static const BINU16_TEST_T g_aTests_sub_u16[]        = { {0} };
static const BINU16_TEST_T g_aTests_sub_u16_locked[] = { {0} };
static const BINU16_TEST_T g_aTests_sbb_u16[]        = { {0} };
static const BINU16_TEST_T g_aTests_sbb_u16_locked[] = { {0} };
static const BINU16_TEST_T g_aTests_or_u16[]         = { {0} };
static const BINU16_TEST_T g_aTests_or_u16_locked[]  = { {0} };
static const BINU16_TEST_T g_aTests_xor_u16[]        = { {0} };
static const BINU16_TEST_T g_aTests_xor_u16_locked[] = { {0} };
static const BINU16_TEST_T g_aTests_and_u16[]        = { {0} };
static const BINU16_TEST_T g_aTests_and_u16_locked[] = { {0} };
static const BINU16_TEST_T g_aTests_cmp_u16[]        = { {0} };
static const BINU16_TEST_T g_aTests_test_u16[]       = { {0} };
static const BINU16_TEST_T g_aTests_bt_u16[]         = { {0} };
static const BINU16_TEST_T g_aTests_btc_u16[]        = { {0} };
static const BINU16_TEST_T g_aTests_btc_u16_locked[] = { {0} };
static const BINU16_TEST_T g_aTests_btr_u16[]        = { {0} };
static const BINU16_TEST_T g_aTests_btr_u16_locked[] = { {0} };
static const BINU16_TEST_T g_aTests_bts_u16[]        = { {0} };
static const BINU16_TEST_T g_aTests_bts_u16_locked[] = { {0} };
static const BINU16_TEST_T g_aTests_arpl[]           = { {0} };
#endif
#ifndef HAVE_BINU16_TESTS_AMD
static const BINU16_TEST_T g_aTests_bsf_u16_amd[]           = { {0} };
static const BINU16_TEST_T g_aTests_bsr_u16_amd[]           = { {0} };
static const BINU16_TEST_T g_aTests_imul_two_u16_amd[]      = { {0} };
#endif
#ifndef HAVE_BINU16_TESTS_INTEL
static const BINU16_TEST_T g_aTests_bsf_u16_intel[]         = { {0} };
static const BINU16_TEST_T g_aTests_bsr_u16_intel[]         = { {0} };
static const BINU16_TEST_T g_aTests_imul_two_u16_intel[]    = { {0} };
#endif

static const BINU16_T g_aBinU16[] =
{
    ENTRY(add_u16),
    ENTRY(add_u16_locked),
    ENTRY(adc_u16),
    ENTRY(adc_u16_locked),
    ENTRY(sub_u16),
    ENTRY(sub_u16_locked),
    ENTRY(sbb_u16),
    ENTRY(sbb_u16_locked),
    ENTRY(or_u16),
    ENTRY(or_u16_locked),
    ENTRY(xor_u16),
    ENTRY(xor_u16_locked),
    ENTRY(and_u16),
    ENTRY(and_u16_locked),
    ENTRY(cmp_u16),
    ENTRY(test_u16),
    ENTRY_EX(bt_u16, 1),
    ENTRY_EX(btc_u16, 1),
    ENTRY_EX(btc_u16_locked, 1),
    ENTRY_EX(btr_u16, 1),
    ENTRY_EX(btr_u16_locked, 1),
    ENTRY_EX(bts_u16, 1),
    ENTRY_EX(bts_u16_locked, 1),
    ENTRY_AMD(  bsf_u16, X86_EFL_CF | X86_EFL_PF | X86_EFL_AF | X86_EFL_SF | X86_EFL_OF),
    ENTRY_INTEL(bsf_u16, X86_EFL_CF | X86_EFL_PF | X86_EFL_AF | X86_EFL_SF | X86_EFL_OF),
    ENTRY_AMD(  bsr_u16, X86_EFL_CF | X86_EFL_PF | X86_EFL_AF | X86_EFL_SF | X86_EFL_OF),
    ENTRY_INTEL(bsr_u16, X86_EFL_CF | X86_EFL_PF | X86_EFL_AF | X86_EFL_SF | X86_EFL_OF),
    ENTRY_AMD(  imul_two_u16, X86_EFL_PF | X86_EFL_AF | X86_EFL_ZF | X86_EFL_SF),
    ENTRY_INTEL(imul_two_u16, X86_EFL_PF | X86_EFL_AF | X86_EFL_ZF | X86_EFL_SF),
    ENTRY(arpl),
};

TEST_BINARY_OPS(16, uint16_t, "%#06x", g_aBinU16)


/*
 * 32-bit binary operations.
 */

#ifndef HAVE_BINU32_TESTS
static const BINU32_TEST_T g_aTests_add_u32[]        = { {0} };
static const BINU32_TEST_T g_aTests_add_u32_locked[] = { {0} };
static const BINU32_TEST_T g_aTests_adc_u32[]        = { {0} };
static const BINU32_TEST_T g_aTests_adc_u32_locked[] = { {0} };
static const BINU32_TEST_T g_aTests_sub_u32[]        = { {0} };
static const BINU32_TEST_T g_aTests_sub_u32_locked[] = { {0} };
static const BINU32_TEST_T g_aTests_sbb_u32[]        = { {0} };
static const BINU32_TEST_T g_aTests_sbb_u32_locked[] = { {0} };
static const BINU32_TEST_T g_aTests_or_u32[]         = { {0} };
static const BINU32_TEST_T g_aTests_or_u32_locked[]  = { {0} };
static const BINU32_TEST_T g_aTests_xor_u32[]        = { {0} };
static const BINU32_TEST_T g_aTests_xor_u32_locked[] = { {0} };
static const BINU32_TEST_T g_aTests_and_u32[]        = { {0} };
static const BINU32_TEST_T g_aTests_and_u32_locked[] = { {0} };
static const BINU32_TEST_T g_aTests_cmp_u32[]        = { {0} };
static const BINU32_TEST_T g_aTests_test_u32[]       = { {0} };
static const BINU32_TEST_T g_aTests_bt_u32[]         = { {0} };
static const BINU32_TEST_T g_aTests_btc_u32[]        = { {0} };
static const BINU32_TEST_T g_aTests_btc_u32_locked[] = { {0} };
static const BINU32_TEST_T g_aTests_btr_u32[]        = { {0} };
static const BINU32_TEST_T g_aTests_btr_u32_locked[] = { {0} };
static const BINU32_TEST_T g_aTests_bts_u32[]        = { {0} };
static const BINU32_TEST_T g_aTests_bts_u32_locked[] = { {0} };
#endif
#ifndef HAVE_BINU32_TESTS_AMD
static const BINU32_TEST_T g_aTests_bsf_u32_amd[]           = { {0} };
static const BINU32_TEST_T g_aTests_bsr_u32_amd[]           = { {0} };
static const BINU32_TEST_T g_aTests_imul_two_u32_amd[]      = { {0} };
#endif
#ifndef HAVE_BINU32_TESTS_INTEL
static const BINU32_TEST_T g_aTests_bsf_u32_intel[]         = { {0} };
static const BINU32_TEST_T g_aTests_bsr_u32_intel[]         = { {0} };
static const BINU32_TEST_T g_aTests_imul_two_u32_intel[]    = { {0} };
#endif

static const BINU32_T g_aBinU32[] =
{
    ENTRY(add_u32),
    ENTRY(add_u32_locked),
    ENTRY(adc_u32),
    ENTRY(adc_u32_locked),
    ENTRY(sub_u32),
    ENTRY(sub_u32_locked),
    ENTRY(sbb_u32),
    ENTRY(sbb_u32_locked),
    ENTRY(or_u32),
    ENTRY(or_u32_locked),
    ENTRY(xor_u32),
    ENTRY(xor_u32_locked),
    ENTRY(and_u32),
    ENTRY(and_u32_locked),
    ENTRY(cmp_u32),
    ENTRY(test_u32),
    ENTRY_EX(bt_u32, 1),
    ENTRY_EX(btc_u32, 1),
    ENTRY_EX(btc_u32_locked, 1),
    ENTRY_EX(btr_u32, 1),
    ENTRY_EX(btr_u32_locked, 1),
    ENTRY_EX(bts_u32, 1),
    ENTRY_EX(bts_u32_locked, 1),
    ENTRY_AMD(  bsf_u32, X86_EFL_CF | X86_EFL_PF | X86_EFL_AF | X86_EFL_SF | X86_EFL_OF),
    ENTRY_INTEL(bsf_u32, X86_EFL_CF | X86_EFL_PF | X86_EFL_AF | X86_EFL_SF | X86_EFL_OF),
    ENTRY_AMD(  bsr_u32, X86_EFL_CF | X86_EFL_PF | X86_EFL_AF | X86_EFL_SF | X86_EFL_OF),
    ENTRY_INTEL(bsr_u32, X86_EFL_CF | X86_EFL_PF | X86_EFL_AF | X86_EFL_SF | X86_EFL_OF),
    ENTRY_AMD(  imul_two_u32, X86_EFL_PF | X86_EFL_AF | X86_EFL_ZF | X86_EFL_SF),
    ENTRY_INTEL(imul_two_u32, X86_EFL_PF | X86_EFL_AF | X86_EFL_ZF | X86_EFL_SF),
};

TEST_BINARY_OPS(32, uint32_t, "%#010RX32", g_aBinU32)


/*
 * 64-bit binary operations.
 */

#ifndef HAVE_BINU64_TESTS
static const BINU64_TEST_T g_aTests_add_u64[]        = { {0} };
static const BINU64_TEST_T g_aTests_add_u64_locked[] = { {0} };
static const BINU64_TEST_T g_aTests_adc_u64[]        = { {0} };
static const BINU64_TEST_T g_aTests_adc_u64_locked[] = { {0} };
static const BINU64_TEST_T g_aTests_sub_u64[]        = { {0} };
static const BINU64_TEST_T g_aTests_sub_u64_locked[] = { {0} };
static const BINU64_TEST_T g_aTests_sbb_u64[]        = { {0} };
static const BINU64_TEST_T g_aTests_sbb_u64_locked[] = { {0} };
static const BINU64_TEST_T g_aTests_or_u64[]         = { {0} };
static const BINU64_TEST_T g_aTests_or_u64_locked[]  = { {0} };
static const BINU64_TEST_T g_aTests_xor_u64[]        = { {0} };
static const BINU64_TEST_T g_aTests_xor_u64_locked[] = { {0} };
static const BINU64_TEST_T g_aTests_and_u64[]        = { {0} };
static const BINU64_TEST_T g_aTests_and_u64_locked[] = { {0} };
static const BINU64_TEST_T g_aTests_cmp_u64[]        = { {0} };
static const BINU64_TEST_T g_aTests_test_u64[]       = { {0} };
static const BINU64_TEST_T g_aTests_bt_u64[]         = { {0} };
static const BINU64_TEST_T g_aTests_btc_u64[]        = { {0} };
static const BINU64_TEST_T g_aTests_btc_u64_locked[] = { {0} };
static const BINU64_TEST_T g_aTests_btr_u64[]        = { {0} };
static const BINU64_TEST_T g_aTests_btr_u64_locked[] = { {0} };
static const BINU64_TEST_T g_aTests_bts_u64[]        = { {0} };
static const BINU64_TEST_T g_aTests_bts_u64_locked[] = { {0} };
#endif
#ifndef HAVE_BINU64_TESTS_AMD
static const BINU64_TEST_T g_aTests_bsf_u64_amd[]           = { {0} };
static const BINU64_TEST_T g_aTests_bsr_u64_amd[]           = { {0} };
static const BINU64_TEST_T g_aTests_imul_two_u64_amd[]      = { {0} };
#endif
#ifndef HAVE_BINU64_TESTS_INTEL
static const BINU64_TEST_T g_aTests_bsf_u64_intel[]         = { {0} };
static const BINU64_TEST_T g_aTests_bsr_u64_intel[]         = { {0} };
static const BINU64_TEST_T g_aTests_imul_two_u64_intel[]    = { {0} };
#endif

static const BINU64_T g_aBinU64[] =
{
    ENTRY(add_u64),
    ENTRY(add_u64_locked),
    ENTRY(adc_u64),
    ENTRY(adc_u64_locked),
    ENTRY(sub_u64),
    ENTRY(sub_u64_locked),
    ENTRY(sbb_u64),
    ENTRY(sbb_u64_locked),
    ENTRY(or_u64),
    ENTRY(or_u64_locked),
    ENTRY(xor_u64),
    ENTRY(xor_u64_locked),
    ENTRY(and_u64),
    ENTRY(and_u64_locked),
    ENTRY(cmp_u64),
    ENTRY(test_u64),
    ENTRY_EX(bt_u64, 1),
    ENTRY_EX(btc_u64, 1),
    ENTRY_EX(btc_u64_locked, 1),
    ENTRY_EX(btr_u64, 1),
    ENTRY_EX(btr_u64_locked, 1),
    ENTRY_EX(bts_u64, 1),
    ENTRY_EX(bts_u64_locked, 1),
    ENTRY_AMD(  bsf_u64, X86_EFL_CF | X86_EFL_PF | X86_EFL_AF | X86_EFL_SF | X86_EFL_OF),
    ENTRY_INTEL(bsf_u64, X86_EFL_CF | X86_EFL_PF | X86_EFL_AF | X86_EFL_SF | X86_EFL_OF),
    ENTRY_AMD(  bsr_u64, X86_EFL_CF | X86_EFL_PF | X86_EFL_AF | X86_EFL_SF | X86_EFL_OF),
    ENTRY_INTEL(bsr_u64, X86_EFL_CF | X86_EFL_PF | X86_EFL_AF | X86_EFL_SF | X86_EFL_OF),
    ENTRY_AMD(  imul_two_u64, X86_EFL_PF | X86_EFL_AF | X86_EFL_ZF | X86_EFL_SF),
    ENTRY_INTEL(imul_two_u64, X86_EFL_PF | X86_EFL_AF | X86_EFL_ZF | X86_EFL_SF),
};

TEST_BINARY_OPS(64, uint64_t, "%#018RX64", g_aBinU64)


/*
 * XCHG
 */
static void XchgTest(void)
{
    RTTestSub(g_hTest, "xchg");
    typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLXCHGU8, (uint8_t  *pu8Mem,  uint8_t  *pu8Reg));
    typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLXCHGU16,(uint16_t *pu16Mem, uint16_t *pu16Reg));
    typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLXCHGU32,(uint32_t *pu32Mem, uint32_t *pu32Reg));
    typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLXCHGU64,(uint64_t *pu64Mem, uint64_t *pu64Reg));

    static struct
    {
        uint8_t cb; uint64_t fMask;
        union
        {
            uintptr_t           pfn;
            FNIEMAIMPLXCHGU8   *pfnU8;
            FNIEMAIMPLXCHGU16  *pfnU16;
            FNIEMAIMPLXCHGU32  *pfnU32;
            FNIEMAIMPLXCHGU64  *pfnU64;
        } u;
    }
    s_aXchgWorkers[] =
    {
        { 1, UINT8_MAX,  { (uintptr_t)iemAImpl_xchg_u8_locked    } },
        { 2, UINT16_MAX, { (uintptr_t)iemAImpl_xchg_u16_locked   } },
        { 4, UINT32_MAX, { (uintptr_t)iemAImpl_xchg_u32_locked   } },
        { 8, UINT64_MAX, { (uintptr_t)iemAImpl_xchg_u64_locked   } },
        { 1, UINT8_MAX,  { (uintptr_t)iemAImpl_xchg_u8_unlocked  } },
        { 2, UINT16_MAX, { (uintptr_t)iemAImpl_xchg_u16_unlocked } },
        { 4, UINT32_MAX, { (uintptr_t)iemAImpl_xchg_u32_unlocked } },
        { 8, UINT64_MAX, { (uintptr_t)iemAImpl_xchg_u64_unlocked } },
    };
    for (size_t i = 0; i < RT_ELEMENTS(s_aXchgWorkers); i++)
    {
        RTUINT64U uIn1, uIn2, uMem, uDst;
        uMem.u = uIn1.u = RTRandU64Ex(0, s_aXchgWorkers[i].fMask);
        uDst.u = uIn2.u = RTRandU64Ex(0, s_aXchgWorkers[i].fMask);
        if (uIn1.u == uIn2.u)
            uDst.u = uIn2.u = ~uIn2.u;

        switch (s_aXchgWorkers[i].cb)
        {
            case 1:
                s_aXchgWorkers[i].u.pfnU8(g_pu8, g_pu8Two);
                s_aXchgWorkers[i].u.pfnU8(&uMem.au8[0], &uDst.au8[0]);
                break;
            case 2:
                s_aXchgWorkers[i].u.pfnU16(g_pu16, g_pu16Two);
                s_aXchgWorkers[i].u.pfnU16(&uMem.Words.w0, &uDst.Words.w0);
                break;
            case 4:
                s_aXchgWorkers[i].u.pfnU32(g_pu32, g_pu32Two);
                s_aXchgWorkers[i].u.pfnU32(&uMem.DWords.dw0, &uDst.DWords.dw0);
                break;
            case 8:
                s_aXchgWorkers[i].u.pfnU64(g_pu64, g_pu64Two);
                s_aXchgWorkers[i].u.pfnU64(&uMem.u, &uDst.u);
                break;
            default: RTTestFailed(g_hTest, "%d\n", s_aXchgWorkers[i].cb); break;
        }

        if (uMem.u != uIn2.u || uDst.u != uIn1.u)
            RTTestFailed(g_hTest, "i=%u: %#RX64, %#RX64 -> %#RX64, %#RX64\n", i,  uIn1.u, uIn2.u, uMem.u, uDst.u);
    }
}


/*
 * XADD
 */
static void XaddTest(void)
{
#define TEST_XADD(a_cBits, a_Type, a_Fmt) do { \
        typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLXADDU ## a_cBits, (a_Type *, a_Type *, uint32_t *)); \
        static struct \
        { \
            const char                         *pszName; \
            FNIEMAIMPLXADDU ## a_cBits         *pfn; \
            BINU ## a_cBits ## _TEST_T const   *paTests; \
            uint32_t                            cTests; \
        } const s_aFuncs[] = \
        { \
            { "xadd_u" # a_cBits,            iemAImpl_xadd_u ## a_cBits, \
              g_aTests_add_u ## a_cBits, RT_ELEMENTS(g_aTests_add_u ## a_cBits) }, \
            { "xadd_u" # a_cBits "8_locked", iemAImpl_xadd_u ## a_cBits ## _locked, \
              g_aTests_add_u ## a_cBits, RT_ELEMENTS(g_aTests_add_u ## a_cBits) }, \
        }; \
        for (size_t iFn = 0; iFn < RT_ELEMENTS(s_aFuncs); iFn++) \
        { \
            RTTestSub(g_hTest, s_aFuncs[iFn].pszName); \
            BINU ## a_cBits ## _TEST_T const * const paTests = s_aFuncs[iFn].paTests; \
            uint32_t const                           cTests  = s_aFuncs[iFn].cTests; \
            for (uint32_t iTest = 0; iTest < cTests; iTest++) \
            { \
                uint32_t fEfl = paTests[iTest].fEflIn; \
                a_Type   uSrc = paTests[iTest].uSrcIn; \
                *g_pu ## a_cBits = paTests[iTest].uDstIn; \
                s_aFuncs[iFn].pfn(g_pu ## a_cBits, &uSrc, &fEfl); \
                if (   fEfl             != paTests[iTest].fEflOut \
                    || *g_pu ## a_cBits != paTests[iTest].uDstOut \
                    || uSrc             != paTests[iTest].uDstIn) \
                    RTTestFailed(g_hTest, "%s/#%u: efl=%#08x dst=" a_Fmt " src=" a_Fmt " -> efl=%#08x dst=" a_Fmt " src=" a_Fmt ", expected %#08x, " a_Fmt ", " a_Fmt "%s\n", \
                                 s_aFuncs[iFn].pszName, iTest, paTests[iTest].fEflIn, paTests[iTest].uDstIn, paTests[iTest].uSrcIn, \
                                 fEfl, *g_pu ## a_cBits, uSrc, paTests[iTest].fEflOut, paTests[iTest].uDstOut, paTests[iTest].uDstIn, \
                                 EFlagsDiff(fEfl, paTests[iTest].fEflOut)); \
            } \
        } \
    } while(0)
    TEST_XADD(8, uint8_t, "%#04x");
    TEST_XADD(16, uint16_t, "%#06x");
    TEST_XADD(32, uint32_t, "%#010RX32");
    TEST_XADD(64, uint64_t, "%#010RX64");
}


/*
 * CMPXCHG
 */

static void CmpXchgTest(void)
{
#define TEST_CMPXCHG(a_cBits, a_Type, a_Fmt) do {\
        typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLCMPXCHGU ## a_cBits, (a_Type *, a_Type *, a_Type, uint32_t *)); \
        static struct \
        { \
            const char                         *pszName; \
            FNIEMAIMPLCMPXCHGU ## a_cBits      *pfn; \
            PFNIEMAIMPLBINU ## a_cBits          pfnSub; \
            BINU ## a_cBits ## _TEST_T const   *paTests; \
            uint32_t                            cTests; \
        } const s_aFuncs[] = \
        { \
            { "cmpxchg_u" # a_cBits,           iemAImpl_cmpxchg_u ## a_cBits, iemAImpl_sub_u ## a_cBits, \
              g_aTests_cmp_u ## a_cBits, RT_ELEMENTS(g_aTests_cmp_u ## a_cBits) }, \
            { "cmpxchg_u" # a_cBits "_locked", iemAImpl_cmpxchg_u ## a_cBits ## _locked, iemAImpl_sub_u ## a_cBits, \
              g_aTests_cmp_u ## a_cBits, RT_ELEMENTS(g_aTests_cmp_u ## a_cBits) }, \
        }; \
        for (size_t iFn = 0; iFn < RT_ELEMENTS(s_aFuncs); iFn++) \
        { \
            RTTestSub(g_hTest, s_aFuncs[iFn].pszName); \
            BINU ## a_cBits ## _TEST_T const * const paTests = s_aFuncs[iFn].paTests; \
            uint32_t const                           cTests  = s_aFuncs[iFn].cTests; \
            for (uint32_t iTest = 0; iTest < cTests; iTest++) \
            { \
                /* as is (99% likely to be negative). */ \
                uint32_t      fEfl    = paTests[iTest].fEflIn; \
                a_Type const  uNew    = paTests[iTest].uSrcIn + 0x42; \
                a_Type        uA      = paTests[iTest].uDstIn; \
                *g_pu ## a_cBits      = paTests[iTest].uSrcIn; \
                a_Type const  uExpect = uA != paTests[iTest].uSrcIn ? paTests[iTest].uSrcIn : uNew; \
                s_aFuncs[iFn].pfn(g_pu ## a_cBits, &uA, uNew, &fEfl); \
                if (   fEfl             != paTests[iTest].fEflOut \
                    || *g_pu ## a_cBits != uExpect \
                    || uA               != paTests[iTest].uSrcIn) \
                    RTTestFailed(g_hTest, "%s/#%ua: efl=%#08x dst=" a_Fmt " cmp=" a_Fmt " new=" a_Fmt " -> efl=%#08x dst=" a_Fmt " old=" a_Fmt ", expected %#08x, " a_Fmt ", " a_Fmt "%s\n", \
                                 s_aFuncs[iFn].pszName, iTest, paTests[iTest].fEflIn, paTests[iTest].uSrcIn, paTests[iTest].uDstIn, \
                                 uNew, fEfl, *g_pu ## a_cBits, uA, paTests[iTest].fEflOut, uExpect, paTests[iTest].uSrcIn, \
                                 EFlagsDiff(fEfl, paTests[iTest].fEflOut)); \
                /* positive */ \
                uint32_t fEflExpect = paTests[iTest].fEflIn; \
                uA                  = paTests[iTest].uDstIn; \
                s_aFuncs[iFn].pfnSub(&uA, uA, &fEflExpect); \
                fEfl                = paTests[iTest].fEflIn; \
                uA                  = paTests[iTest].uDstIn; \
                *g_pu ## a_cBits    = uA; \
                s_aFuncs[iFn].pfn(g_pu ## a_cBits, &uA, uNew, &fEfl); \
                if (   fEfl             != fEflExpect \
                    || *g_pu ## a_cBits != uNew \
                    || uA               != paTests[iTest].uDstIn) \
                    RTTestFailed(g_hTest, "%s/#%ua: efl=%#08x dst=" a_Fmt " cmp=" a_Fmt " new=" a_Fmt " -> efl=%#08x dst=" a_Fmt " old=" a_Fmt ", expected %#08x, " a_Fmt ", " a_Fmt "%s\n", \
                                 s_aFuncs[iFn].pszName, iTest, paTests[iTest].fEflIn, paTests[iTest].uDstIn, paTests[iTest].uDstIn, \
                                 uNew, fEfl, *g_pu ## a_cBits, uA, fEflExpect, uNew, paTests[iTest].uDstIn, \
                                 EFlagsDiff(fEfl, fEflExpect)); \
            } \
        } \
    } while(0)
    TEST_CMPXCHG(8, uint8_t, "%#04RX8");
    TEST_CMPXCHG(16, uint16_t, "%#06x");
    TEST_CMPXCHG(32, uint32_t, "%#010RX32");
#if ARCH_BITS != 32 /* calling convension issue, skipping as it's an unsupported host  */
    TEST_CMPXCHG(64, uint64_t, "%#010RX64");
#endif
}

static void CmpXchg8bTest(void)
{
    typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLCMPXCHG8B,(uint64_t *, PRTUINT64U, PRTUINT64U, uint32_t *));
    static struct
    {
        const char           *pszName;
        FNIEMAIMPLCMPXCHG8B  *pfn;
    } const s_aFuncs[] =
    {
        { "cmpxchg8b",        iemAImpl_cmpxchg8b },
        { "cmpxchg8b_locked", iemAImpl_cmpxchg8b_locked },
    };
    for (size_t iFn = 0; iFn < RT_ELEMENTS(s_aFuncs); iFn++)
    {
        RTTestSub(g_hTest, s_aFuncs[iFn].pszName);
        for (uint32_t iTest = 0; iTest < 4; iTest += 2)
        {
            uint64_t const uOldValue = RandU64();
            uint64_t const uNewValue = RandU64();

            /* positive test. */
            RTUINT64U uA, uB;
            uB.u             = uNewValue;
            uA.u             = uOldValue;
            *g_pu64          = uOldValue;
            uint32_t fEflIn  = RandEFlags();
            uint32_t fEfl    = fEflIn;
            s_aFuncs[iFn].pfn(g_pu64, &uA, &uB, &fEfl);
            if (   fEfl    != (fEflIn | X86_EFL_ZF)
                || *g_pu64 != uNewValue
                || uA.u    != uOldValue)
                RTTestFailed(g_hTest, "#%u: efl=%#08x dst=%#018RX64 cmp=%#018RX64 new=%#018RX64\n -> efl=%#08x dst=%#018RX64 old=%#018RX64,\n wanted %#08x,    %#018RX64,    %#018RX64%s\n",
                             iTest, fEflIn, uOldValue, uOldValue, uNewValue,
                             fEfl, *g_pu64, uA.u,
                             (fEflIn | X86_EFL_ZF), uNewValue, uOldValue, EFlagsDiff(fEfl, fEflIn | X86_EFL_ZF));
            RTTEST_CHECK(g_hTest, uB.u == uNewValue);

            /* negative */
            uint64_t const uExpect = ~uOldValue;
            *g_pu64 = uExpect;
            uA.u = uOldValue;
            uB.u = uNewValue;
            fEfl = fEflIn = RandEFlags();
            s_aFuncs[iFn].pfn(g_pu64, &uA, &uB, &fEfl);
            if (   fEfl    != (fEflIn & ~X86_EFL_ZF)
                || *g_pu64 != uExpect
                || uA.u    != uExpect)
                RTTestFailed(g_hTest, "#%u: efl=%#08x dst=%#018RX64 cmp=%#018RX64 new=%#018RX64\n -> efl=%#08x dst=%#018RX64 old=%#018RX64,\n wanted %#08x,    %#018RX64,    %#018RX64%s\n",
                             iTest + 1, fEflIn, uExpect, uOldValue, uNewValue,
                             fEfl, *g_pu64, uA.u,
                             (fEflIn & ~X86_EFL_ZF), uExpect, uExpect, EFlagsDiff(fEfl, fEflIn & ~X86_EFL_ZF));
            RTTEST_CHECK(g_hTest, uB.u == uNewValue);
        }
    }
}

static void CmpXchg16bTest(void)
{
    typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLCMPXCHG16B,(PRTUINT128U, PRTUINT128U, PRTUINT128U, uint32_t *));
    static struct
    {
        const char           *pszName;
        FNIEMAIMPLCMPXCHG16B *pfn;
    } const s_aFuncs[] =
    {
        { "cmpxchg16b",          iemAImpl_cmpxchg16b },
        { "cmpxchg16b_locked",   iemAImpl_cmpxchg16b_locked },
#if !defined(RT_ARCH_ARM64)
        { "cmpxchg16b_fallback", iemAImpl_cmpxchg16b_fallback },
#endif
    };
    for (size_t iFn = 0; iFn < RT_ELEMENTS(s_aFuncs); iFn++)
    {
#if !defined(IEM_WITHOUT_ASSEMBLY) && defined(RT_ARCH_AMD64)
        if (!(ASMCpuId_ECX(1) & X86_CPUID_FEATURE_ECX_CX16))
            continue;
#endif
        RTTestSub(g_hTest, s_aFuncs[iFn].pszName);
        for (uint32_t iTest = 0; iTest < 4; iTest += 2)
        {
            RTUINT128U const uOldValue = RandU128();
            RTUINT128U const uNewValue = RandU128();

            /* positive test. */
            RTUINT128U uA, uB;
            uB               = uNewValue;
            uA               = uOldValue;
            *g_pu128         = uOldValue;
            uint32_t fEflIn  = RandEFlags();
            uint32_t fEfl    = fEflIn;
            s_aFuncs[iFn].pfn(g_pu128, &uA, &uB, &fEfl);
            if (   fEfl    != (fEflIn | X86_EFL_ZF)
                || g_pu128->s.Lo != uNewValue.s.Lo
                || g_pu128->s.Hi != uNewValue.s.Hi
                || uA.s.Lo       != uOldValue.s.Lo
                || uA.s.Hi       != uOldValue.s.Hi)
                RTTestFailed(g_hTest, "#%u: efl=%#08x dst=%#018RX64'%016RX64 cmp=%#018RX64'%016RX64 new=%#018RX64'%016RX64\n"
                                      " -> efl=%#08x dst=%#018RX64'%016RX64 old=%#018RX64'%016RX64,\n"
                                      " wanted %#08x,    %#018RX64'%016RX64,    %#018RX64'%016RX64%s\n",
                             iTest, fEflIn, uOldValue.s.Hi, uOldValue.s.Lo, uOldValue.s.Hi, uOldValue.s.Lo, uNewValue.s.Hi, uNewValue.s.Lo,
                             fEfl, g_pu128->s.Hi, g_pu128->s.Lo, uA.s.Hi, uA.s.Lo,
                             (fEflIn | X86_EFL_ZF), uNewValue.s.Hi, uNewValue.s.Lo, uOldValue.s.Hi, uOldValue.s.Lo,
                             EFlagsDiff(fEfl, fEflIn | X86_EFL_ZF));
            RTTEST_CHECK(g_hTest, uB.s.Lo == uNewValue.s.Lo && uB.s.Hi == uNewValue.s.Hi);

            /* negative */
            RTUINT128U const uExpect = RTUINT128_INIT(~uOldValue.s.Hi, ~uOldValue.s.Lo);
            *g_pu128 = uExpect;
            uA       = uOldValue;
            uB       = uNewValue;
            fEfl = fEflIn = RandEFlags();
            s_aFuncs[iFn].pfn(g_pu128, &uA, &uB, &fEfl);
            if (   fEfl          != (fEflIn & ~X86_EFL_ZF)
                || g_pu128->s.Lo != uExpect.s.Lo
                || g_pu128->s.Hi != uExpect.s.Hi
                || uA.s.Lo       != uExpect.s.Lo
                || uA.s.Hi       != uExpect.s.Hi)
                RTTestFailed(g_hTest, "#%u: efl=%#08x dst=%#018RX64'%016RX64 cmp=%#018RX64'%016RX64 new=%#018RX64'%016RX64\n"
                                      " -> efl=%#08x dst=%#018RX64'%016RX64 old=%#018RX64'%016RX64,\n"
                                      " wanted %#08x,    %#018RX64'%016RX64,    %#018RX64'%016RX64%s\n",
                             iTest + 1, fEflIn, uExpect.s.Hi, uExpect.s.Lo, uOldValue.s.Hi, uOldValue.s.Lo, uNewValue.s.Hi, uNewValue.s.Lo,
                             fEfl, g_pu128->s.Hi, g_pu128->s.Lo, uA.s.Hi, uA.s.Lo,
                             (fEflIn & ~X86_EFL_ZF), uExpect.s.Hi, uExpect.s.Lo, uExpect.s.Hi, uExpect.s.Lo,
                             EFlagsDiff(fEfl, fEflIn & ~X86_EFL_ZF));
            RTTEST_CHECK(g_hTest, uB.s.Lo == uNewValue.s.Lo && uB.s.Hi == uNewValue.s.Hi);
        }
    }
}


/*
 * Double shifts.
 *
 * Note! We use BINUxx_TEST_T with the shift value in the uMisc field.
 */

#ifndef HAVE_SHIFT_DBL_TESTS_AMD
static const BINU16_TEST_T g_aTests_shrd_u16_amd[] = { {0} };
static const BINU16_TEST_T g_aTests_shld_u16_amd[] = { {0} };
static const BINU32_TEST_T g_aTests_shrd_u32_amd[] = { {0} };
static const BINU32_TEST_T g_aTests_shld_u32_amd[] = { {0} };
static const BINU64_TEST_T g_aTests_shrd_u64_amd[] = { {0} };
static const BINU64_TEST_T g_aTests_shld_u64_amd[] = { {0} };
#endif
#ifndef HAVE_SHIFT_DBL_TESTS_INTEL
static const BINU16_TEST_T g_aTests_shrd_u16_intel[] = { {0} };
static const BINU16_TEST_T g_aTests_shld_u16_intel[] = { {0} };
static const BINU32_TEST_T g_aTests_shrd_u32_intel[] = { {0} };
static const BINU32_TEST_T g_aTests_shld_u32_intel[] = { {0} };
static const BINU64_TEST_T g_aTests_shrd_u64_intel[] = { {0} };
static const BINU64_TEST_T g_aTests_shld_u64_intel[] = { {0} };
#endif

#ifdef TSTIEMAIMPL_WITH_GENERATOR
# define GEN_SHIFT_DBL(a_cBits, a_Fmt, a_aSubTests) \
void ShiftDblU ## a_cBits ## Generate(PRTSTREAM pOut, uint32_t cTests) \
{ \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(a_aSubTests); iFn++) \
    { \
        if (   a_aSubTests[iFn].idxCpuEflFlavour != IEMTARGETCPU_EFL_BEHAVIOR_NATIVE \
            && a_aSubTests[iFn].idxCpuEflFlavour != g_idxCpuEflFlavour) \
            continue; \
        RTStrmPrintf(pOut, "static const BINU" #a_cBits "_TEST_T g_aTests_%s[] =\n{\n", a_aSubTests[iFn].pszName); \
        for (uint32_t iTest = 0; iTest < cTests; iTest++ ) \
        { \
            BINU ## a_cBits ## _TEST_T Test; \
            Test.fEflIn    = RandEFlags(); \
            Test.fEflOut   = Test.fEflIn; \
            Test.uDstIn    = RandU ## a_cBits ## Dst(iTest); \
            Test.uDstOut   = Test.uDstIn; \
            Test.uSrcIn    = RandU ## a_cBits ## Src(iTest); \
            Test.uMisc     = RandU8() & (a_cBits * 4 - 1); /* need to go way beyond the a_cBits limit */ \
            a_aSubTests[iFn].pfnNative(&Test.uDstOut, Test.uSrcIn, Test.uMisc, &Test.fEflOut); \
            RTStrmPrintf(pOut, "    { %#08x, %#08x, " a_Fmt ", " a_Fmt ", " a_Fmt ", %2u }, /* #%u */\n", \
                        Test.fEflIn, Test.fEflOut, Test.uDstIn, Test.uDstOut, Test.uSrcIn, Test.uMisc, iTest); \
        } \
        RTStrmPrintf(pOut, "};\n"); \
    } \
}
#else
# define GEN_SHIFT_DBL(a_cBits, a_Fmt, a_aSubTests)
#endif

#define TEST_SHIFT_DBL(a_cBits, a_Type, a_Fmt, a_aSubTests) \
static const struct \
{ \
    const char                       *pszName; \
    PFNIEMAIMPLSHIFTDBLU ## a_cBits   pfn; \
    PFNIEMAIMPLSHIFTDBLU ## a_cBits   pfnNative; \
    BINU ## a_cBits ## _TEST_T const *paTests; \
    uint32_t                          cTests, uExtra; \
    uint8_t                           idxCpuEflFlavour; \
} a_aSubTests[] = \
{ \
    ENTRY_AMD(shld_u ## a_cBits,   X86_EFL_OF | X86_EFL_CF), \
    ENTRY_INTEL(shld_u ## a_cBits, X86_EFL_OF | X86_EFL_CF), \
    ENTRY_AMD(shrd_u ## a_cBits,   X86_EFL_OF | X86_EFL_CF), \
    ENTRY_INTEL(shrd_u ## a_cBits, X86_EFL_OF | X86_EFL_CF), \
}; \
\
GEN_SHIFT_DBL(a_cBits, a_Fmt, a_aSubTests) \
\
static void ShiftDblU ## a_cBits ## Test(void) \
{ \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(a_aSubTests); iFn++) \
    { \
        RTTestSub(g_hTest, a_aSubTests[iFn].pszName); \
        BINU ## a_cBits ## _TEST_T const * const paTests = a_aSubTests[iFn].paTests; \
        uint32_t const                           cTests  = a_aSubTests[iFn].cTests; \
        PFNIEMAIMPLSHIFTDBLU ## a_cBits          pfn     = a_aSubTests[iFn].pfn; \
        uint32_t const cVars = 1 + (a_aSubTests[iFn].idxCpuEflFlavour == g_idxCpuEflFlavour && a_aSubTests[iFn].pfnNative); \
        for (uint32_t iVar = 0; iVar < cVars; iVar++) \
        { \
            for (uint32_t iTest = 0; iTest < cTests; iTest++ ) \
            { \
                uint32_t fEfl = paTests[iTest].fEflIn; \
                a_Type   uDst = paTests[iTest].uDstIn; \
                pfn(&uDst, paTests[iTest].uSrcIn, paTests[iTest].uMisc, &fEfl); \
                if (   uDst != paTests[iTest].uDstOut \
                    || fEfl != paTests[iTest].fEflOut) \
                    RTTestFailed(g_hTest, "#%03u%s: efl=%#08x dst=" a_Fmt " src=" a_Fmt " shift=%-2u -> efl=%#08x dst=" a_Fmt ", expected %#08x & " a_Fmt "%s%s\n", \
                                 iTest, iVar == 0 ? "" : "/n", paTests[iTest].fEflIn, \
                                 paTests[iTest].uDstIn, paTests[iTest].uSrcIn, (unsigned)paTests[iTest].uMisc, \
                                 fEfl, uDst, paTests[iTest].fEflOut, paTests[iTest].uDstOut, \
                                 EFlagsDiff(fEfl, paTests[iTest].fEflOut), uDst == paTests[iTest].uDstOut ? "" : " dst!"); \
                else \
                { \
                     *g_pu ## a_cBits  = paTests[iTest].uDstIn; \
                     *g_pfEfl          = paTests[iTest].fEflIn; \
                     pfn(g_pu ## a_cBits, paTests[iTest].uSrcIn, paTests[iTest].uMisc, g_pfEfl); \
                     RTTEST_CHECK(g_hTest, *g_pu ## a_cBits == paTests[iTest].uDstOut); \
                     RTTEST_CHECK(g_hTest, *g_pfEfl == paTests[iTest].fEflOut); \
                } \
            } \
            pfn = a_aSubTests[iFn].pfnNative; \
        } \
    } \
}
TEST_SHIFT_DBL(16, uint16_t, "%#06RX16",  g_aShiftDblU16)
TEST_SHIFT_DBL(32, uint32_t, "%#010RX32", g_aShiftDblU32)
TEST_SHIFT_DBL(64, uint64_t, "%#018RX64", g_aShiftDblU64)

#ifdef TSTIEMAIMPL_WITH_GENERATOR
static void ShiftDblGenerate(PRTSTREAM pOut, const char *pszCpuSuffU, uint32_t cTests)
{
    RTStrmPrintf(pOut, "\n\n#define HAVE_SHIFT_DBL_TESTS%s\n", pszCpuSuffU);
    ShiftDblU16Generate(pOut, cTests);
    ShiftDblU32Generate(pOut, cTests);
    ShiftDblU64Generate(pOut, cTests);
}
#endif

static void ShiftDblTest(void)
{
    ShiftDblU16Test();
    ShiftDblU32Test();
    ShiftDblU64Test();
}


/*
 * Unary operators.
 *
 * Note! We use BINUxx_TEST_T ignoreing uSrcIn and uMisc.
 */

#ifndef HAVE_UNARY_TESTS
# define DUMMY_UNARY_TESTS(a_cBits, a_Type) \
    static const a_Type g_aTests_inc_u ## a_cBits[] = { {0} }; \
    static const a_Type g_aTests_inc_u ## a_cBits ## _locked[] = { {0} }; \
    static const a_Type g_aTests_dec_u ## a_cBits[] = { {0} }; \
    static const a_Type g_aTests_dec_u ## a_cBits ## _locked[] = { {0} }; \
    static const a_Type g_aTests_not_u ## a_cBits[] = { {0} }; \
    static const a_Type g_aTests_not_u ## a_cBits ## _locked[] = { {0} }; \
    static const a_Type g_aTests_neg_u ## a_cBits[] = { {0} }; \
    static const a_Type g_aTests_neg_u ## a_cBits ## _locked[] = { {0} }
DUMMY_UNARY_TESTS(8,  BINU8_TEST_T);
DUMMY_UNARY_TESTS(16, BINU16_TEST_T);
DUMMY_UNARY_TESTS(32, BINU32_TEST_T);
DUMMY_UNARY_TESTS(64, BINU64_TEST_T);
#endif

#define TEST_UNARY(a_cBits, a_Type, a_Fmt, a_TestType) \
static const struct \
{ \
    const char                  *pszName; \
    PFNIEMAIMPLUNARYU ## a_cBits pfn; \
    PFNIEMAIMPLUNARYU ## a_cBits pfnNative; \
    a_TestType const            *paTests; \
    uint32_t                     cTests, uExtra; \
    uint8_t                      idxCpuEflFlavour; \
} g_aUnaryU ## a_cBits [] = \
{ \
    ENTRY(inc_u ## a_cBits), \
    ENTRY(inc_u ## a_cBits ## _locked), \
    ENTRY(dec_u ## a_cBits), \
    ENTRY(dec_u ## a_cBits ## _locked), \
    ENTRY(not_u ## a_cBits), \
    ENTRY(not_u ## a_cBits ## _locked), \
    ENTRY(neg_u ## a_cBits), \
    ENTRY(neg_u ## a_cBits ## _locked), \
}; \
\
void UnaryU ## a_cBits ## Generate(PRTSTREAM pOut, uint32_t cTests) \
{ \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aUnaryU ## a_cBits); iFn++) \
    { \
        RTStrmPrintf(pOut, "static const BINU" #a_cBits "_TEST_T g_aTests_%s[] =\n{\n", g_aUnaryU ## a_cBits[iFn].pszName); \
        for (uint32_t iTest = 0; iTest < cTests; iTest++ ) \
        { \
            a_TestType Test; \
            Test.fEflIn    = RandEFlags(); \
            Test.fEflOut   = Test.fEflIn; \
            Test.uDstIn    = RandU ## a_cBits(); \
            Test.uDstOut   = Test.uDstIn; \
            Test.uSrcIn    = 0; \
            Test.uMisc     = 0; \
            g_aUnaryU ## a_cBits[iFn].pfn(&Test.uDstOut, &Test.fEflOut); \
            RTStrmPrintf(pOut, "    { %#08x, %#08x, " a_Fmt ", " a_Fmt ", 0, 0 }, /* #%u */\n", \
                        Test.fEflIn, Test.fEflOut, Test.uDstIn, Test.uDstOut, iTest); \
        } \
        RTStrmPrintf(pOut, "};\n"); \
    } \
} \
\
static void UnaryU ## a_cBits ## Test(void) \
{ \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aUnaryU ## a_cBits); iFn++) \
    { \
        RTTestSub(g_hTest, g_aUnaryU ## a_cBits[iFn].pszName); \
        a_TestType const * const paTests = g_aUnaryU ## a_cBits[iFn].paTests; \
        uint32_t const           cTests  = g_aUnaryU ## a_cBits[iFn].cTests; \
        for (uint32_t iTest = 0; iTest < cTests; iTest++ ) \
        { \
            uint32_t fEfl = paTests[iTest].fEflIn; \
            a_Type   uDst = paTests[iTest].uDstIn; \
            g_aUnaryU ## a_cBits[iFn].pfn(&uDst, &fEfl); \
            if (   uDst != paTests[iTest].uDstOut \
                || fEfl != paTests[iTest].fEflOut) \
                RTTestFailed(g_hTest, "#%u: efl=%#08x dst=" a_Fmt " -> efl=%#08x dst=" a_Fmt ", expected %#08x & " a_Fmt "%s\n", \
                             iTest, paTests[iTest].fEflIn, paTests[iTest].uDstIn, \
                             fEfl, uDst, paTests[iTest].fEflOut, paTests[iTest].uDstOut, \
                             EFlagsDiff(fEfl, paTests[iTest].fEflOut)); \
            else \
            { \
                 *g_pu ## a_cBits  = paTests[iTest].uDstIn; \
                 *g_pfEfl          = paTests[iTest].fEflIn; \
                 g_aUnaryU ## a_cBits[iFn].pfn(g_pu ## a_cBits, g_pfEfl); \
                 RTTEST_CHECK(g_hTest, *g_pu ## a_cBits == paTests[iTest].uDstOut); \
                 RTTEST_CHECK(g_hTest, *g_pfEfl == paTests[iTest].fEflOut); \
            } \
        } \
    } \
}
TEST_UNARY(8,  uint8_t,  "%#04RX8",   BINU8_TEST_T)
TEST_UNARY(16, uint16_t, "%#06RX16",  BINU16_TEST_T)
TEST_UNARY(32, uint32_t, "%#010RX32", BINU32_TEST_T)
TEST_UNARY(64, uint64_t, "%#018RX64", BINU64_TEST_T)

#ifdef TSTIEMAIMPL_WITH_GENERATOR
static void UnaryGenerate(PRTSTREAM pOut, uint32_t cTests)
{
    RTStrmPrintf(pOut, "\n\n#define HAVE_UNARY_TESTS\n");
    UnaryU8Generate(pOut, cTests);
    UnaryU16Generate(pOut, cTests);
    UnaryU32Generate(pOut, cTests);
    UnaryU64Generate(pOut, cTests);
}
#endif

static void UnaryTest(void)
{
    UnaryU8Test();
    UnaryU16Test();
    UnaryU32Test();
    UnaryU64Test();
}


/*
 * Shifts.
 *
 * Note! We use BINUxx_TEST_T with the shift count in uMisc and uSrcIn unused.
 */
#define DUMMY_SHIFT_TESTS(a_cBits, a_Type, a_Vendor) \
    static const a_Type  g_aTests_rol_u ## a_cBits ## a_Vendor[] = { {0} }; \
    static const a_Type  g_aTests_ror_u ## a_cBits ## a_Vendor[] = { {0} }; \
    static const a_Type  g_aTests_rcl_u ## a_cBits ## a_Vendor[] = { {0} }; \
    static const a_Type  g_aTests_rcr_u ## a_cBits ## a_Vendor[] = { {0} }; \
    static const a_Type  g_aTests_shl_u ## a_cBits ## a_Vendor[] = { {0} }; \
    static const a_Type  g_aTests_shr_u ## a_cBits ## a_Vendor[] = { {0} }; \
    static const a_Type  g_aTests_sar_u ## a_cBits ## a_Vendor[] = { {0} }
#ifndef HAVE_SHIFT_TESTS_AMD
DUMMY_SHIFT_TESTS(8,  BINU8_TEST_T,  _amd);
DUMMY_SHIFT_TESTS(16, BINU16_TEST_T, _amd);
DUMMY_SHIFT_TESTS(32, BINU32_TEST_T, _amd);
DUMMY_SHIFT_TESTS(64, BINU64_TEST_T, _amd);
#endif
#ifndef HAVE_SHIFT_TESTS_INTEL
DUMMY_SHIFT_TESTS(8,  BINU8_TEST_T,  _intel);
DUMMY_SHIFT_TESTS(16, BINU16_TEST_T, _intel);
DUMMY_SHIFT_TESTS(32, BINU32_TEST_T, _intel);
DUMMY_SHIFT_TESTS(64, BINU64_TEST_T, _intel);
#endif

#ifdef TSTIEMAIMPL_WITH_GENERATOR
# define GEN_SHIFT(a_cBits, a_Fmt, a_TestType, a_aSubTests) \
void ShiftU ## a_cBits ## Generate(PRTSTREAM pOut, uint32_t cTests) \
{ \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(a_aSubTests); iFn++) \
    { \
        if (   a_aSubTests[iFn].idxCpuEflFlavour != IEMTARGETCPU_EFL_BEHAVIOR_NATIVE \
            && a_aSubTests[iFn].idxCpuEflFlavour != g_idxCpuEflFlavour) \
            continue; \
        RTStrmPrintf(pOut, "static const BINU" #a_cBits "_TEST_T g_aTests_%s[] =\n{\n", a_aSubTests[iFn].pszName); \
        for (uint32_t iTest = 0; iTest < cTests; iTest++ ) \
        { \
            a_TestType Test; \
            Test.fEflIn    = RandEFlags(); \
            Test.fEflOut   = Test.fEflIn; \
            Test.uDstIn    = RandU ## a_cBits ## Dst(iTest); \
            Test.uDstOut   = Test.uDstIn; \
            Test.uSrcIn    = 0; \
            Test.uMisc     = RandU8() & (a_cBits * 4 - 1); /* need to go way beyond the a_cBits limit */ \
            a_aSubTests[iFn].pfnNative(&Test.uDstOut, Test.uMisc, &Test.fEflOut); \
            RTStrmPrintf(pOut, "    { %#08x, %#08x, " a_Fmt ", " a_Fmt ", 0, %-2u }, /* #%u */\n", \
                         Test.fEflIn, Test.fEflOut, Test.uDstIn, Test.uDstOut, Test.uMisc, iTest); \
            \
            Test.fEflIn    = (~Test.fEflIn & X86_EFL_LIVE_MASK) | X86_EFL_RA1_MASK; \
            Test.fEflOut   = Test.fEflIn; \
            Test.uDstOut   = Test.uDstIn; \
            a_aSubTests[iFn].pfnNative(&Test.uDstOut, Test.uMisc, &Test.fEflOut); \
            RTStrmPrintf(pOut, "    { %#08x, %#08x, " a_Fmt ", " a_Fmt ", 0, %-2u }, /* #%u b */\n", \
                         Test.fEflIn, Test.fEflOut, Test.uDstIn, Test.uDstOut, Test.uMisc, iTest); \
        } \
        RTStrmPrintf(pOut, "};\n"); \
    } \
}
#else
# define GEN_SHIFT(a_cBits, a_Fmt, a_TestType, a_aSubTests)
#endif

#define TEST_SHIFT(a_cBits, a_Type, a_Fmt, a_TestType, a_aSubTests) \
static const struct \
{ \
    const char                  *pszName; \
    PFNIEMAIMPLSHIFTU ## a_cBits pfn; \
    PFNIEMAIMPLSHIFTU ## a_cBits pfnNative; \
    a_TestType const            *paTests; \
    uint32_t                     cTests, uExtra; \
    uint8_t                      idxCpuEflFlavour; \
} a_aSubTests[] = \
{ \
    ENTRY_AMD(  rol_u ## a_cBits, X86_EFL_OF), \
    ENTRY_INTEL(rol_u ## a_cBits, X86_EFL_OF), \
    ENTRY_AMD(  ror_u ## a_cBits, X86_EFL_OF), \
    ENTRY_INTEL(ror_u ## a_cBits, X86_EFL_OF), \
    ENTRY_AMD(  rcl_u ## a_cBits, X86_EFL_OF), \
    ENTRY_INTEL(rcl_u ## a_cBits, X86_EFL_OF), \
    ENTRY_AMD(  rcr_u ## a_cBits, X86_EFL_OF), \
    ENTRY_INTEL(rcr_u ## a_cBits, X86_EFL_OF), \
    ENTRY_AMD(  shl_u ## a_cBits, X86_EFL_OF | X86_EFL_AF), \
    ENTRY_INTEL(shl_u ## a_cBits, X86_EFL_OF | X86_EFL_AF), \
    ENTRY_AMD(  shr_u ## a_cBits, X86_EFL_OF | X86_EFL_AF), \
    ENTRY_INTEL(shr_u ## a_cBits, X86_EFL_OF | X86_EFL_AF), \
    ENTRY_AMD(  sar_u ## a_cBits, X86_EFL_OF | X86_EFL_AF), \
    ENTRY_INTEL(sar_u ## a_cBits, X86_EFL_OF | X86_EFL_AF), \
}; \
\
GEN_SHIFT(a_cBits, a_Fmt, a_TestType, a_aSubTests) \
\
static void ShiftU ## a_cBits ## Test(void) \
{ \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(a_aSubTests); iFn++) \
    { \
        RTTestSub(g_hTest, a_aSubTests[iFn].pszName); \
        a_TestType const * const     paTests = a_aSubTests[iFn].paTests; \
        uint32_t const               cTests  = a_aSubTests[iFn].cTests; \
        PFNIEMAIMPLSHIFTU ## a_cBits pfn     = a_aSubTests[iFn].pfn; \
        uint32_t const cVars = 1 + (a_aSubTests[iFn].idxCpuEflFlavour == g_idxCpuEflFlavour && a_aSubTests[iFn].pfnNative); \
        for (uint32_t iVar = 0; iVar < cVars; iVar++) \
        { \
            for (uint32_t iTest = 0; iTest < cTests; iTest++ ) \
            { \
                uint32_t fEfl = paTests[iTest].fEflIn; \
                a_Type   uDst = paTests[iTest].uDstIn; \
                pfn(&uDst, paTests[iTest].uMisc, &fEfl); \
                if (   uDst != paTests[iTest].uDstOut \
                    || fEfl != paTests[iTest].fEflOut ) \
                    RTTestFailed(g_hTest, "#%u%s: efl=%#08x dst=" a_Fmt " shift=%2u -> efl=%#08x dst=" a_Fmt ", expected %#08x & " a_Fmt "%s\n", \
                                 iTest, iVar == 0 ? "" : "/n", \
                                 paTests[iTest].fEflIn, paTests[iTest].uDstIn, paTests[iTest].uMisc, \
                                 fEfl, uDst, paTests[iTest].fEflOut, paTests[iTest].uDstOut, \
                                 EFlagsDiff(fEfl, paTests[iTest].fEflOut)); \
                else \
                { \
                     *g_pu ## a_cBits  = paTests[iTest].uDstIn; \
                     *g_pfEfl          = paTests[iTest].fEflIn; \
                     pfn(g_pu ## a_cBits, paTests[iTest].uMisc, g_pfEfl); \
                     RTTEST_CHECK(g_hTest, *g_pu ## a_cBits == paTests[iTest].uDstOut); \
                     RTTEST_CHECK(g_hTest, *g_pfEfl == paTests[iTest].fEflOut); \
                } \
            } \
            pfn = a_aSubTests[iFn].pfnNative; \
        } \
    } \
}
TEST_SHIFT(8,  uint8_t,  "%#04RX8",   BINU8_TEST_T,  g_aShiftU8)
TEST_SHIFT(16, uint16_t, "%#06RX16",  BINU16_TEST_T, g_aShiftU16)
TEST_SHIFT(32, uint32_t, "%#010RX32", BINU32_TEST_T, g_aShiftU32)
TEST_SHIFT(64, uint64_t, "%#018RX64", BINU64_TEST_T, g_aShiftU64)

#ifdef TSTIEMAIMPL_WITH_GENERATOR
static void ShiftGenerate(PRTSTREAM pOut, const char *pszCpuSuffU, uint32_t cTests)
{
    RTStrmPrintf(pOut, "\n\n#define HAVE_SHIFT_TESTS%s\n", pszCpuSuffU);
    ShiftU8Generate(pOut, cTests);
    ShiftU16Generate(pOut, cTests);
    ShiftU32Generate(pOut, cTests);
    ShiftU64Generate(pOut, cTests);
}
#endif

static void ShiftTest(void)
{
    ShiftU8Test();
    ShiftU16Test();
    ShiftU32Test();
    ShiftU64Test();
}


/*
 * Multiplication and division.
 *
 * Note! The 8-bit functions has a different format, so we need to duplicate things.
 * Note! Currently ignoring undefined bits.
 */

# define DUMMY_MULDIV_TESTS(a_cBits, a_Type, a_Vendor) \
    static const a_Type g_aTests_mul_u ## a_cBits ## a_Vendor[]  = { {0} }; \
    static const a_Type g_aTests_imul_u ## a_cBits ## a_Vendor[] = { {0} }; \
    static const a_Type g_aTests_div_u ## a_cBits ## a_Vendor[]  = { {0} }; \
    static const a_Type g_aTests_idiv_u ## a_cBits ## a_Vendor[] = { {0} }

#ifndef HAVE_MULDIV_TESTS_AMD
DUMMY_MULDIV_TESTS(8,  MULDIVU8_TEST_T,  _amd);
DUMMY_MULDIV_TESTS(16, MULDIVU16_TEST_T, _amd);
DUMMY_MULDIV_TESTS(32, MULDIVU32_TEST_T, _amd);
DUMMY_MULDIV_TESTS(64, MULDIVU64_TEST_T, _amd);
#endif

#ifndef HAVE_MULDIV_TESTS_INTEL
DUMMY_MULDIV_TESTS(8,  MULDIVU8_TEST_T,  _intel);
DUMMY_MULDIV_TESTS(16, MULDIVU16_TEST_T, _intel);
DUMMY_MULDIV_TESTS(32, MULDIVU32_TEST_T, _intel);
DUMMY_MULDIV_TESTS(64, MULDIVU64_TEST_T, _intel);
#endif

/* U8 */
static const struct
{
    const char                     *pszName;
    PFNIEMAIMPLMULDIVU8             pfn;
    PFNIEMAIMPLMULDIVU8             pfnNative;
    MULDIVU8_TEST_T const          *paTests;
    uint32_t                        cTests, uExtra;
    uint8_t                         idxCpuEflFlavour;
} g_aMulDivU8[] =
{
    ENTRY_AMD_EX(mul_u8,    X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF,
                            X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF),
    ENTRY_INTEL_EX(mul_u8,  X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF, 0),
    ENTRY_AMD_EX(imul_u8,   X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF,
                            X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF),
    ENTRY_INTEL_EX(imul_u8, X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF, 0),
    ENTRY_AMD_EX(div_u8,    X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF | X86_EFL_CF | X86_EFL_OF, 0),
    ENTRY_INTEL_EX(div_u8,  X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF | X86_EFL_CF | X86_EFL_OF, 0),
    ENTRY_AMD_EX(idiv_u8,   X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF | X86_EFL_CF | X86_EFL_OF, 0),
    ENTRY_INTEL_EX(idiv_u8, X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF | X86_EFL_CF | X86_EFL_OF, 0),
};

#ifdef TSTIEMAIMPL_WITH_GENERATOR
static void MulDivU8Generate(PRTSTREAM pOut, uint32_t cTests)
{
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aMulDivU8); iFn++)
    {
        if (   g_aMulDivU8[iFn].idxCpuEflFlavour != IEMTARGETCPU_EFL_BEHAVIOR_NATIVE
            && g_aMulDivU8[iFn].idxCpuEflFlavour != g_idxCpuEflFlavour)
            continue;
        RTStrmPrintf(pOut, "static const MULDIVU8_TEST_T g_aTests_%s[] =\n{\n", g_aMulDivU8[iFn].pszName);
        for (uint32_t iTest = 0; iTest < cTests; iTest++ )
        {
            MULDIVU8_TEST_T Test;
            Test.fEflIn    = RandEFlags();
            Test.fEflOut   = Test.fEflIn;
            Test.uDstIn    = RandU16Dst(iTest);
            Test.uDstOut   = Test.uDstIn;
            Test.uSrcIn    = RandU8Src(iTest);
            Test.rc        = g_aMulDivU8[iFn].pfnNative(&Test.uDstOut, Test.uSrcIn, &Test.fEflOut);
            RTStrmPrintf(pOut, "    { %#08x, %#08x, %#06RX16, %#06RX16, %#04RX8, %d }, /* #%u */\n",
                         Test.fEflIn, Test.fEflOut, Test.uDstIn, Test.uDstOut, Test.uSrcIn, Test.rc, iTest);
        }
        RTStrmPrintf(pOut, "};\n");
    }
}
#endif

static void MulDivU8Test(void)
{
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aMulDivU8); iFn++)
    {
        RTTestSub(g_hTest, g_aMulDivU8[iFn].pszName);
        MULDIVU8_TEST_T const * const paTests = g_aMulDivU8[iFn].paTests;
        uint32_t const                cTests  = g_aMulDivU8[iFn].cTests;
        uint32_t const                fEflIgn = g_aMulDivU8[iFn].uExtra;
        PFNIEMAIMPLMULDIVU8           pfn     = g_aMulDivU8[iFn].pfn;
        uint32_t const cVars = 1 + (g_aMulDivU8[iFn].idxCpuEflFlavour == g_idxCpuEflFlavour && g_aMulDivU8[iFn].pfnNative);
        for (uint32_t iVar = 0; iVar < cVars; iVar++)
        {
            for (uint32_t iTest = 0; iTest < cTests; iTest++ )
            {
                uint32_t fEfl  = paTests[iTest].fEflIn;
                uint16_t uDst  = paTests[iTest].uDstIn;
                int      rc    = g_aMulDivU8[iFn].pfn(&uDst, paTests[iTest].uSrcIn, &fEfl);
                if (   uDst != paTests[iTest].uDstOut
                    || (fEfl | fEflIgn) != (paTests[iTest].fEflOut | fEflIgn)
                    || rc != paTests[iTest].rc)
                    RTTestFailed(g_hTest, "#%02u%s: efl=%#08x dst=%#06RX16 src=%#04RX8\n"
                                          "  %s-> efl=%#08x dst=%#06RX16 rc=%d\n"
                                          "%sexpected %#08x     %#06RX16    %d%s\n",
                                 iTest, iVar ? "/n" : "", paTests[iTest].fEflIn, paTests[iTest].uDstIn, paTests[iTest].uSrcIn,
                                 iVar ? "  " : "", fEfl, uDst, rc,
                                 iVar ? "  " : "", paTests[iTest].fEflOut, paTests[iTest].uDstOut, paTests[iTest].rc,
                                 EFlagsDiff(fEfl | fEflIgn, paTests[iTest].fEflOut | fEflIgn));
                else
                {
                     *g_pu16  = paTests[iTest].uDstIn;
                     *g_pfEfl = paTests[iTest].fEflIn;
                     rc = g_aMulDivU8[iFn].pfn(g_pu16, paTests[iTest].uSrcIn, g_pfEfl);
                     RTTEST_CHECK(g_hTest, *g_pu16  == paTests[iTest].uDstOut);
                     RTTEST_CHECK(g_hTest, (*g_pfEfl | fEflIgn) == (paTests[iTest].fEflOut | fEflIgn));
                     RTTEST_CHECK(g_hTest, rc  == paTests[iTest].rc);
                }
            }
            pfn = g_aMulDivU8[iFn].pfnNative;
        }
    }
}

#ifdef TSTIEMAIMPL_WITH_GENERATOR
# define GEN_MULDIV(a_cBits, a_Fmt, a_TestType, a_aSubTests) \
void MulDivU ## a_cBits ## Generate(PRTSTREAM pOut, uint32_t cTests) \
{ \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(a_aSubTests); iFn++) \
    { \
        if (   a_aSubTests[iFn].idxCpuEflFlavour != IEMTARGETCPU_EFL_BEHAVIOR_NATIVE \
            && a_aSubTests[iFn].idxCpuEflFlavour != g_idxCpuEflFlavour) \
            continue; \
        RTStrmPrintf(pOut, "static const MULDIVU" #a_cBits "_TEST_T g_aTests_%s[] =\n{\n", a_aSubTests[iFn].pszName); \
        for (uint32_t iTest = 0; iTest < cTests; iTest++ ) \
        { \
            a_TestType Test; \
            Test.fEflIn    = RandEFlags(); \
            Test.fEflOut   = Test.fEflIn; \
            Test.uDst1In   = RandU ## a_cBits ## Dst(iTest); \
            Test.uDst1Out  = Test.uDst1In; \
            Test.uDst2In   = RandU ## a_cBits ## Dst(iTest); \
            Test.uDst2Out  = Test.uDst2In; \
            Test.uSrcIn    = RandU ## a_cBits ## Src(iTest); \
            Test.rc        = a_aSubTests[iFn].pfnNative(&Test.uDst1Out, &Test.uDst2Out, Test.uSrcIn, &Test.fEflOut); \
            RTStrmPrintf(pOut, "    { %#08x, %#08x, " a_Fmt ", " a_Fmt ", " a_Fmt ", " a_Fmt ", " a_Fmt ", %d }, /* #%u */\n", \
                        Test.fEflIn, Test.fEflOut, Test.uDst1In, Test.uDst1Out, Test.uDst2In, Test.uDst2Out, Test.uSrcIn, \
                        Test.rc, iTest); \
        } \
        RTStrmPrintf(pOut, "};\n"); \
    } \
}
#else
# define GEN_MULDIV(a_cBits, a_Fmt, a_TestType, a_aSubTests)
#endif

#define TEST_MULDIV(a_cBits, a_Type, a_Fmt, a_TestType, a_aSubTests) \
static const struct \
{ \
    const char                     *pszName; \
    PFNIEMAIMPLMULDIVU ## a_cBits   pfn; \
    PFNIEMAIMPLMULDIVU ## a_cBits   pfnNative; \
    a_TestType const               *paTests; \
    uint32_t                        cTests, uExtra; \
    uint8_t                         idxCpuEflFlavour; \
} a_aSubTests [] = \
{ \
    ENTRY_AMD_EX(mul_u ## a_cBits,    X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF, 0), \
    ENTRY_INTEL_EX(mul_u ## a_cBits,  X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF, 0), \
    ENTRY_AMD_EX(imul_u ## a_cBits,   X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF, 0), \
    ENTRY_INTEL_EX(imul_u ## a_cBits, X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF, 0), \
    ENTRY_AMD_EX(div_u ## a_cBits,    X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF | X86_EFL_CF | X86_EFL_OF, 0), \
    ENTRY_INTEL_EX(div_u ## a_cBits,  X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF | X86_EFL_CF | X86_EFL_OF, 0), \
    ENTRY_AMD_EX(idiv_u ## a_cBits,   X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF | X86_EFL_CF | X86_EFL_OF, 0), \
    ENTRY_INTEL_EX(idiv_u ## a_cBits, X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF | X86_EFL_CF | X86_EFL_OF, 0), \
}; \
\
GEN_MULDIV(a_cBits, a_Fmt, a_TestType, a_aSubTests) \
\
static void MulDivU ## a_cBits ## Test(void) \
{ \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(a_aSubTests); iFn++) \
    { \
        RTTestSub(g_hTest, a_aSubTests[iFn].pszName); \
        a_TestType const * const      paTests = a_aSubTests[iFn].paTests; \
        uint32_t const                cTests  = a_aSubTests[iFn].cTests; \
        uint32_t const                fEflIgn = a_aSubTests[iFn].uExtra; \
        PFNIEMAIMPLMULDIVU ## a_cBits pfn     = a_aSubTests[iFn].pfn; \
        uint32_t const cVars = 1 + (a_aSubTests[iFn].idxCpuEflFlavour == g_idxCpuEflFlavour && a_aSubTests[iFn].pfnNative); \
        for (uint32_t iVar = 0; iVar < cVars; iVar++) \
        { \
            for (uint32_t iTest = 0; iTest < cTests; iTest++ ) \
            { \
                uint32_t fEfl  = paTests[iTest].fEflIn; \
                a_Type   uDst1 = paTests[iTest].uDst1In; \
                a_Type   uDst2 = paTests[iTest].uDst2In; \
                int rc = pfn(&uDst1, &uDst2, paTests[iTest].uSrcIn, &fEfl); \
                if (   uDst1 != paTests[iTest].uDst1Out \
                    || uDst2 != paTests[iTest].uDst2Out \
                    || (fEfl | fEflIgn) != (paTests[iTest].fEflOut | fEflIgn)\
                    || rc    != paTests[iTest].rc) \
                    RTTestFailed(g_hTest, "#%02u%s: efl=%#08x dst1=" a_Fmt " dst2=" a_Fmt " src=" a_Fmt "\n" \
                                           "  -> efl=%#08x dst1=" a_Fmt  " dst2=" a_Fmt " rc=%d\n" \
                                           "expected %#08x      " a_Fmt  "      " a_Fmt "    %d%s -%s%s%s\n", \
                                 iTest, iVar == 0 ? "" : "/n", \
                                 paTests[iTest].fEflIn, paTests[iTest].uDst1In, paTests[iTest].uDst2In, paTests[iTest].uSrcIn, \
                                 fEfl, uDst1, uDst2, rc, \
                                 paTests[iTest].fEflOut, paTests[iTest].uDst1Out, paTests[iTest].uDst2Out, paTests[iTest].rc, \
                                 EFlagsDiff(fEfl | fEflIgn, paTests[iTest].fEflOut | fEflIgn), \
                                 uDst1 != paTests[iTest].uDst1Out ? " dst1" : "", uDst2 != paTests[iTest].uDst2Out ? " dst2" : "", \
                                 (fEfl | fEflIgn) != (paTests[iTest].fEflOut | fEflIgn) ? " eflags" : ""); \
                else \
                { \
                     *g_pu ## a_cBits        = paTests[iTest].uDst1In; \
                     *g_pu ## a_cBits ## Two = paTests[iTest].uDst2In; \
                     *g_pfEfl                = paTests[iTest].fEflIn; \
                     rc  = pfn(g_pu ## a_cBits, g_pu ## a_cBits ## Two, paTests[iTest].uSrcIn, g_pfEfl); \
                     RTTEST_CHECK(g_hTest, *g_pu ## a_cBits        == paTests[iTest].uDst1Out); \
                     RTTEST_CHECK(g_hTest, *g_pu ## a_cBits ## Two == paTests[iTest].uDst2Out); \
                     RTTEST_CHECK(g_hTest, (*g_pfEfl | fEflIgn)    == (paTests[iTest].fEflOut | fEflIgn)); \
                     RTTEST_CHECK(g_hTest, rc                      == paTests[iTest].rc); \
                } \
            } \
            pfn = a_aSubTests[iFn].pfnNative; \
        } \
    } \
}
TEST_MULDIV(16, uint16_t, "%#06RX16",  MULDIVU16_TEST_T, g_aMulDivU16)
TEST_MULDIV(32, uint32_t, "%#010RX32", MULDIVU32_TEST_T, g_aMulDivU32)
TEST_MULDIV(64, uint64_t, "%#018RX64", MULDIVU64_TEST_T, g_aMulDivU64)

#ifdef TSTIEMAIMPL_WITH_GENERATOR
static void MulDivGenerate(PRTSTREAM pOut, const char *pszCpuSuffU, uint32_t cTests)
{
    RTStrmPrintf(pOut, "\n\n#define HAVE_MULDIV_TESTS%s\n", pszCpuSuffU);
    MulDivU8Generate(pOut, cTests);
    MulDivU16Generate(pOut, cTests);
    MulDivU32Generate(pOut, cTests);
    MulDivU64Generate(pOut, cTests);
}
#endif

static void MulDivTest(void)
{
    MulDivU8Test();
    MulDivU16Test();
    MulDivU32Test();
    MulDivU64Test();
}


/*
 * BSWAP
 */
static void BswapTest(void)
{
    RTTestSub(g_hTest, "bswap_u16");
    *g_pu32 = UINT32_C(0x12345678);
    iemAImpl_bswap_u16(g_pu32);
#if 0
    RTTEST_CHECK_MSG(g_hTest, *g_pu32 == UINT32_C(0x12347856), (g_hTest, "*g_pu32=%#RX32\n", *g_pu32));
#else
    RTTEST_CHECK_MSG(g_hTest, *g_pu32 == UINT32_C(0x12340000), (g_hTest, "*g_pu32=%#RX32\n", *g_pu32));
#endif
    *g_pu32 = UINT32_C(0xffff1122);
    iemAImpl_bswap_u16(g_pu32);
#if 0
    RTTEST_CHECK_MSG(g_hTest, *g_pu32 == UINT32_C(0xffff2211), (g_hTest, "*g_pu32=%#RX32\n", *g_pu32));
#else
    RTTEST_CHECK_MSG(g_hTest, *g_pu32 == UINT32_C(0xffff0000), (g_hTest, "*g_pu32=%#RX32\n", *g_pu32));
#endif

    RTTestSub(g_hTest, "bswap_u32");
    *g_pu32 = UINT32_C(0x12345678);
    iemAImpl_bswap_u32(g_pu32);
    RTTEST_CHECK(g_hTest, *g_pu32 == UINT32_C(0x78563412));

    RTTestSub(g_hTest, "bswap_u64");
    *g_pu64 = UINT64_C(0x0123456789abcdef);
    iemAImpl_bswap_u64(g_pu64);
    RTTEST_CHECK(g_hTest, *g_pu64 == UINT64_C(0xefcdab8967452301));
}



/*********************************************************************************************************************************
*   Floating point (x87 style)                                                                                                   *
*********************************************************************************************************************************/

typedef struct FPU_LD_CONST_TEST_T
{
    uint16_t                fFcw;
    uint16_t                fFswIn;
    uint16_t                fFswOut;
    RTFLOAT80U              rdResult;
} FPU_LD_CONST_TEST_T;

typedef struct FPU_R32_IN_TEST_T
{
    uint16_t                fFcw;
    uint16_t                fFswIn;
    uint16_t                fFswOut;
    RTFLOAT80U              rdResult;
    RTFLOAT32U              InVal;
} FPU_R32_IN_TEST_T;

typedef struct FPU_R64_IN_TEST_T
{
    uint16_t                fFcw;
    uint16_t                fFswIn;
    uint16_t                fFswOut;
    RTFLOAT80U              rdResult;
    RTFLOAT64U              InVal;
} FPU_R64_IN_TEST_T;

typedef struct FPU_R80_IN_TEST_T
{
    uint16_t                fFcw;
    uint16_t                fFswIn;
    uint16_t                fFswOut;
    RTFLOAT80U              rdResult;
    RTFLOAT80U              InVal;
} FPU_R80_IN_TEST_T;

typedef struct FPU_I16_IN_TEST_T
{
    uint16_t                fFcw;
    uint16_t                fFswIn;
    uint16_t                fFswOut;
    RTFLOAT80U              rdResult;
    int16_t                 iInVal;
} FPU_I16_IN_TEST_T;

typedef struct FPU_I32_IN_TEST_T
{
    uint16_t                fFcw;
    uint16_t                fFswIn;
    uint16_t                fFswOut;
    RTFLOAT80U              rdResult;
    int32_t                 iInVal;
} FPU_I32_IN_TEST_T;

typedef struct FPU_I64_IN_TEST_T
{
    uint16_t                fFcw;
    uint16_t                fFswIn;
    uint16_t                fFswOut;
    RTFLOAT80U              rdResult;
    int64_t                 iInVal;
} FPU_I64_IN_TEST_T;

typedef struct FPU_D80_IN_TEST_T
{
    uint16_t                fFcw;
    uint16_t                fFswIn;
    uint16_t                fFswOut;
    RTFLOAT80U              rdResult;
    RTPBCD80U               InVal;
} FPU_D80_IN_TEST_T;

typedef struct FPU_ST_R32_TEST_T
{
    uint16_t                fFcw;
    uint16_t                fFswIn;
    uint16_t                fFswOut;
    RTFLOAT80U              InVal;
    RTFLOAT32U              OutVal;
} FPU_ST_R32_TEST_T;

typedef struct FPU_ST_R64_TEST_T
{
    uint16_t                fFcw;
    uint16_t                fFswIn;
    uint16_t                fFswOut;
    RTFLOAT80U              InVal;
    RTFLOAT64U              OutVal;
} FPU_ST_R64_TEST_T;

typedef struct FPU_ST_R80_TEST_T
{
    uint16_t                fFcw;
    uint16_t                fFswIn;
    uint16_t                fFswOut;
    RTFLOAT80U              InVal;
    RTFLOAT80U              OutVal;
} FPU_ST_R80_TEST_T;

typedef struct FPU_ST_I16_TEST_T
{
    uint16_t                fFcw;
    uint16_t                fFswIn;
    uint16_t                fFswOut;
    RTFLOAT80U              InVal;
    int16_t                 iOutVal;
} FPU_ST_I16_TEST_T;

typedef struct FPU_ST_I32_TEST_T
{
    uint16_t                fFcw;
    uint16_t                fFswIn;
    uint16_t                fFswOut;
    RTFLOAT80U              InVal;
    int32_t                 iOutVal;
} FPU_ST_I32_TEST_T;

typedef struct FPU_ST_I64_TEST_T
{
    uint16_t                fFcw;
    uint16_t                fFswIn;
    uint16_t                fFswOut;
    RTFLOAT80U              InVal;
    int64_t                 iOutVal;
} FPU_ST_I64_TEST_T;

typedef struct FPU_ST_D80_TEST_T
{
    uint16_t                fFcw;
    uint16_t                fFswIn;
    uint16_t                fFswOut;
    RTFLOAT80U              InVal;
    RTPBCD80U               OutVal;
} FPU_ST_D80_TEST_T;

#include "tstIEMAImplDataFpuLdSt.h"


/*
 * FPU constant loading.
 */

#ifndef HAVE_FPU_LOAD_CONST_TESTS
static const FPU_LD_CONST_TEST_T g_aTests_fld1[]   = { {0} };
static const FPU_LD_CONST_TEST_T g_aTests_fldl2t[] = { {0} };
static const FPU_LD_CONST_TEST_T g_aTests_fldl2e[] = { {0} };
static const FPU_LD_CONST_TEST_T g_aTests_fldpi[]  = { {0} };
static const FPU_LD_CONST_TEST_T g_aTests_fldlg2[] = { {0} };
static const FPU_LD_CONST_TEST_T g_aTests_fldln2[] = { {0} };
static const FPU_LD_CONST_TEST_T g_aTests_fldz[]   = { {0} };
#endif

typedef struct FPU_LD_CONST_T
{
    const char                 *pszName;
    PFNIEMAIMPLFPUR80LDCONST    pfn;
    PFNIEMAIMPLFPUR80LDCONST    pfnNative;
    FPU_LD_CONST_TEST_T const  *paTests;
    uint32_t                    cTests;
    uint32_t                    uExtra;
    uint8_t                     idxCpuEflFlavour;
} FPU_LD_CONST_T;

static const FPU_LD_CONST_T g_aFpuLdConst[] =
{
    ENTRY(fld1),
    ENTRY(fldl2t),
    ENTRY(fldl2e),
    ENTRY(fldpi),
    ENTRY(fldlg2),
    ENTRY(fldln2),
    ENTRY(fldz),
};

#ifdef TSTIEMAIMPL_WITH_GENERATOR
static void FpuLdConstGenerate(PRTSTREAM pOut, uint32_t cTests)
{
    RTStrmPrintf(pOut, "\n\n#define HAVE_FPU_LOAD_CONST_TESTS\n");
    X86FXSTATE State;
    RT_ZERO(State);
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aFpuLdConst); iFn++)
    {
        RTStrmPrintf(pOut, "static const FPU_LD_CONST_TEST_T g_aTests_%s[] =\n{\n", g_aFpuLdConst[iFn].pszName);
        for (uint32_t iTest = 0; iTest < cTests; iTest += 4)
        {
            State.FCW = RandFcw();
            State.FSW = RandFsw();

            for (uint16_t iRounding = 0; iRounding < 4; iRounding++)
            {
                IEMFPURESULT Res = { RTFLOAT80U_INIT(0, 0, 0), 0 };
                State.FCW = (State.FCW & ~X86_FCW_RC_MASK) | (iRounding << X86_FCW_RC_SHIFT);
                g_aFpuLdConst[iFn].pfn(&State, &Res);
                RTStrmPrintf(pOut, "    { %#06x, %#06x, %#06x, %s }, /* #%u */\n",
                             State.FCW, State.FSW, Res.FSW, GenFormatR80(&Res.r80Result), iTest + iRounding);
            }
        }
        RTStrmPrintf(pOut, "};\n");
    }
}
#endif

static void FpuLoadConstTest(void)
{
    /*
     * Inputs:
     *      - FSW: C0, C1, C2, C3
     *      - FCW: Exception masks, Precision control, Rounding control.
     *
     * C1 set to 1 on stack overflow, zero otherwise.  C0, C2, and C3 are "undefined".
     */
    X86FXSTATE State;
    RT_ZERO(State);
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aFpuLdConst); iFn++)
    {
        RTTestSub(g_hTest, g_aFpuLdConst[iFn].pszName);

        uint32_t const              cTests  = g_aFpuLdConst[iFn].cTests;
        FPU_LD_CONST_TEST_T const  *paTests = g_aFpuLdConst[iFn].paTests;
        PFNIEMAIMPLFPUR80LDCONST    pfn     = g_aFpuLdConst[iFn].pfn;
        uint32_t const cVars = 1 + (g_aFpuLdConst[iFn].idxCpuEflFlavour == g_idxCpuEflFlavour && g_aFpuLdConst[iFn].pfnNative);
        for (uint32_t iVar = 0; iVar < cVars; iVar++)
        {
            for (uint32_t iTest = 0; iTest < cTests; iTest++)
            {
                State.FCW = paTests[iTest].fFcw;
                State.FSW = paTests[iTest].fFswIn;
                IEMFPURESULT Res = { RTFLOAT80U_INIT(0, 0, 0), 0 };
                pfn(&State, &Res);
                if (   Res.FSW != paTests[iTest].fFswOut
                    || !RTFLOAT80U_ARE_IDENTICAL(&Res.r80Result, &paTests[iTest].rdResult))
                    RTTestFailed(g_hTest, "#%u%s: fcw=%#06x fsw=%#06x -> fsw=%#06x %s, expected %#06x %s%s%s (%s)\n",
                                 iTest, iVar ? "/n" : "", paTests[iTest].fFcw, paTests[iTest].fFswIn,
                                 Res.FSW, FormatR80(&Res.r80Result),
                                 paTests[iTest].fFswOut, FormatR80(&paTests[iTest].rdResult),
                                 FswDiff(Res.FSW, paTests[iTest].fFswOut),
                                 !RTFLOAT80U_ARE_IDENTICAL(&Res.r80Result, &paTests[iTest].rdResult) ? " - val" : "",
                                 FormatFcw(paTests[iTest].fFcw) );
            }
            pfn = g_aFpuLdConst[iFn].pfnNative;
        }
    }
}


/*
 * Load floating point values from memory.
 */

#ifndef HAVE_FPU_LD_MEM
static FPU_R80_IN_TEST_T const g_aTests_fld_r80_from_r80[] = { {0} };
static FPU_R64_IN_TEST_T const g_aTests_fld_r80_from_r64[] = { {0} };
static FPU_R32_IN_TEST_T const g_aTests_fld_r80_from_r32[] = { {0} };
#endif

#ifdef TSTIEMAIMPL_WITH_GENERATOR
# define GEN_FPU_LOAD(a_cBits, a_rdTypeIn, a_aSubTests, a_TestType) \
static void FpuLdR ## a_cBits ## Generate(PRTSTREAM pOut, uint32_t cTests) \
{ \
    X86FXSTATE State; \
    RT_ZERO(State); \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(a_aSubTests); iFn++) \
    { \
        RTStrmPrintf(pOut, "static const " #a_TestType " g_aTests_%s[] =\n{\n", a_aSubTests[iFn].pszName); \
        for (uint32_t iTest = 0; iTest < cTests; iTest++) \
        { \
            State.FCW = RandFcw(); \
            State.FSW = RandFsw(); \
            a_rdTypeIn InVal = RandR ## a_cBits ## Src(iTest); \
            \
            for (uint16_t iRounding = 0; iRounding < 4; iRounding++) \
            { \
                IEMFPURESULT Res = { RTFLOAT80U_INIT(0, 0, 0), 0 }; \
                State.FCW = (State.FCW & ~X86_FCW_RC_MASK) | (iRounding << X86_FCW_RC_SHIFT); \
                a_aSubTests[iFn].pfn(&State, &Res, &InVal); \
                RTStrmPrintf(pOut, "    { %#06x, %#06x, %#06x, %s, %s }, /* #%u/%u */\n", \
                             State.FCW, State.FSW, Res.FSW, GenFormatR80(&Res.r80Result), \
                             GenFormatR ## a_cBits(&InVal), iTest, iRounding); \
            } \
        } \
        RTStrmPrintf(pOut, "};\n"); \
    } \
}
#else
# define GEN_FPU_LOAD(a_cBits, a_rdTypeIn, a_aSubTests, a_TestType)
#endif

#define TEST_FPU_LOAD(a_cBits, a_rdTypeIn, a_SubTestType, a_aSubTests, a_TestType) \
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLFPULDR80FROM ## a_cBits,(PCX86FXSTATE, PIEMFPURESULT, PC ## a_rdTypeIn)); \
typedef FNIEMAIMPLFPULDR80FROM ## a_cBits *PFNIEMAIMPLFPULDR80FROM ## a_cBits; \
typedef struct a_SubTestType \
{ \
    const char                             *pszName; \
    PFNIEMAIMPLFPULDR80FROM ## a_cBits      pfn, pfnNative; \
    a_TestType const                       *paTests; \
    uint32_t                                cTests; \
    uint32_t                                uExtra; \
    uint8_t                                 idxCpuEflFlavour; \
} a_SubTestType; \
\
static const a_SubTestType a_aSubTests[] = \
{ \
    ENTRY(RT_CONCAT(fld_r80_from_r,a_cBits)) \
}; \
GEN_FPU_LOAD(a_cBits, a_rdTypeIn, a_aSubTests, a_TestType) \
\
static void FpuLdR ## a_cBits ## Test(void) \
{ \
    X86FXSTATE State; \
    RT_ZERO(State); \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(a_aSubTests); iFn++) \
    { \
        RTTestSub(g_hTest, a_aSubTests[iFn].pszName); \
        \
        uint32_t const                     cTests  = a_aSubTests[iFn].cTests; \
        a_TestType const           * const paTests = a_aSubTests[iFn].paTests; \
        PFNIEMAIMPLFPULDR80FROM ## a_cBits pfn     = a_aSubTests[iFn].pfn; \
        uint32_t const cVars = 1 + (a_aSubTests[iFn].idxCpuEflFlavour == g_idxCpuEflFlavour && a_aSubTests[iFn].pfnNative); \
        for (uint32_t iVar = 0; iVar < cVars; iVar++) \
        { \
            for (uint32_t iTest = 0; iTest < cTests; iTest++) \
            { \
                a_rdTypeIn const InVal = paTests[iTest].InVal; \
                State.FCW = paTests[iTest].fFcw; \
                State.FSW = paTests[iTest].fFswIn; \
                IEMFPURESULT Res = { RTFLOAT80U_INIT(0, 0, 0), 0 }; \
                pfn(&State, &Res, &InVal); \
                if (   Res.FSW != paTests[iTest].fFswOut \
                    || !RTFLOAT80U_ARE_IDENTICAL(&Res.r80Result, &paTests[iTest].rdResult)) \
                    RTTestFailed(g_hTest, "#%03u%s: fcw=%#06x fsw=%#06x in=%s\n" \
                                          "%s              -> fsw=%#06x    %s\n" \
                                          "%s            expected %#06x    %s%s%s (%s)\n", \
                                 iTest, iVar ? "/n" : "", paTests[iTest].fFcw, paTests[iTest].fFswIn, \
                                 FormatR ## a_cBits(&paTests[iTest].InVal), \
                                 iVar ? "  " : "", Res.FSW, FormatR80(&Res.r80Result), \
                                 iVar ? "  " : "", paTests[iTest].fFswOut, FormatR80(&paTests[iTest].rdResult), \
                                 FswDiff(Res.FSW, paTests[iTest].fFswOut), \
                                 !RTFLOAT80U_ARE_IDENTICAL(&Res.r80Result, &paTests[iTest].rdResult) ? " - val" : "", \
                                 FormatFcw(paTests[iTest].fFcw) ); \
            } \
            pfn = a_aSubTests[iFn].pfnNative; \
        } \
    } \
}

TEST_FPU_LOAD(80, RTFLOAT80U, FPU_LD_R80_T, g_aFpuLdR80, FPU_R80_IN_TEST_T)
TEST_FPU_LOAD(64, RTFLOAT64U, FPU_LD_R64_T, g_aFpuLdR64, FPU_R64_IN_TEST_T)
TEST_FPU_LOAD(32, RTFLOAT32U, FPU_LD_R32_T, g_aFpuLdR32, FPU_R32_IN_TEST_T)

#ifdef TSTIEMAIMPL_WITH_GENERATOR
static void FpuLdMemGenerate(PRTSTREAM pOut, uint32_t cTests)
{
    RTStrmPrintf(pOut, "\n\n#define HAVE_FPU_LD_MEM\n");
    FpuLdR80Generate(pOut, cTests);
    FpuLdR64Generate(pOut, cTests);
    FpuLdR32Generate(pOut, cTests);
}
#endif

static void FpuLdMemTest(void)
{
    FpuLdR80Test();
    FpuLdR64Test();
    FpuLdR32Test();
}


/*
 * Load integer values from memory.
 */

#ifndef HAVE_FPU_LD_INT
static FPU_I64_IN_TEST_T const g_aTests_fild_r80_from_i64[] = { {0} };
static FPU_I32_IN_TEST_T const g_aTests_fild_r80_from_i32[] = { {0} };
static FPU_I16_IN_TEST_T const g_aTests_fild_r80_from_i16[] = { {0} };
#endif

#ifdef TSTIEMAIMPL_WITH_GENERATOR
# define GEN_FPU_LOAD_INT(a_cBits, a_iTypeIn, a_szFmtIn, a_aSubTests, a_TestType) \
static void FpuLdI ## a_cBits ## Generate(PRTSTREAM pOut, uint32_t cTests) \
{ \
    X86FXSTATE State; \
    RT_ZERO(State); \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(a_aSubTests); iFn++) \
    { \
        RTStrmPrintf(pOut, "static const " #a_TestType " g_aTests_%s[] =\n{\n", a_aSubTests[iFn].pszName); \
        for (uint32_t iTest = 0; iTest < cTests; iTest++) \
        { \
            State.FCW = RandFcw(); \
            State.FSW = RandFsw(); \
            a_iTypeIn InVal = (a_iTypeIn)RandU ## a_cBits ## Src(iTest); \
            \
            for (uint16_t iRounding = 0; iRounding < 4; iRounding++) \
            { \
                IEMFPURESULT Res = { RTFLOAT80U_INIT(0, 0, 0), 0 }; \
                State.FCW = (State.FCW & ~X86_FCW_RC_MASK) | (iRounding << X86_FCW_RC_SHIFT); \
                a_aSubTests[iFn].pfn(&State, &Res, &InVal); \
                RTStrmPrintf(pOut, "    { %#06x, %#06x, %#06x, %s, " a_szFmtIn " }, /* #%u/%u */\n", \
                             State.FCW, State.FSW, Res.FSW, GenFormatR80(&Res.r80Result), InVal, iTest, iRounding); \
            } \
        } \
        RTStrmPrintf(pOut, "};\n"); \
    } \
}
#else
# define GEN_FPU_LOAD_INT(a_cBits, a_iTypeIn, a_szFmtIn, a_aSubTests, a_TestType)
#endif

#define TEST_FPU_LOAD_INT(a_cBits, a_iTypeIn, a_szFmtIn, a_SubTestType, a_aSubTests, a_TestType) \
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLFPULDR80FROMI ## a_cBits,(PCX86FXSTATE, PIEMFPURESULT, a_iTypeIn const *)); \
typedef FNIEMAIMPLFPULDR80FROMI ## a_cBits *PFNIEMAIMPLFPULDR80FROMI ## a_cBits; \
typedef struct a_SubTestType \
{ \
    const char                             *pszName; \
    PFNIEMAIMPLFPULDR80FROMI ## a_cBits      pfn, pfnNative; \
    a_TestType const                       *paTests; \
    uint32_t                                cTests; \
    uint32_t                                uExtra; \
    uint8_t                                 idxCpuEflFlavour; \
} a_SubTestType; \
\
static const a_SubTestType a_aSubTests[] = \
{ \
    ENTRY(RT_CONCAT(fild_r80_from_i,a_cBits)) \
}; \
GEN_FPU_LOAD_INT(a_cBits, a_iTypeIn, a_szFmtIn, a_aSubTests, a_TestType) \
\
static void FpuLdI ## a_cBits ## Test(void) \
{ \
    X86FXSTATE State; \
    RT_ZERO(State); \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(a_aSubTests); iFn++) \
    { \
        RTTestSub(g_hTest, a_aSubTests[iFn].pszName); \
        \
        uint32_t const                      cTests  = a_aSubTests[iFn].cTests; \
        a_TestType const            * const paTests = a_aSubTests[iFn].paTests; \
        PFNIEMAIMPLFPULDR80FROMI ## a_cBits pfn     = a_aSubTests[iFn].pfn; \
        uint32_t const cVars = 1 + (a_aSubTests[iFn].idxCpuEflFlavour == g_idxCpuEflFlavour && a_aSubTests[iFn].pfnNative); \
        for (uint32_t iVar = 0; iVar < cVars; iVar++) \
        { \
            for (uint32_t iTest = 0; iTest < cTests; iTest++) \
            { \
                a_iTypeIn const iInVal = paTests[iTest].iInVal; \
                State.FCW = paTests[iTest].fFcw; \
                State.FSW = paTests[iTest].fFswIn; \
                IEMFPURESULT Res = { RTFLOAT80U_INIT(0, 0, 0), 0 }; \
                pfn(&State, &Res, &iInVal); \
                if (   Res.FSW != paTests[iTest].fFswOut \
                    || !RTFLOAT80U_ARE_IDENTICAL(&Res.r80Result, &paTests[iTest].rdResult)) \
                    RTTestFailed(g_hTest, "#%03u%s: fcw=%#06x fsw=%#06x in=" a_szFmtIn "\n" \
                                          "%s              -> fsw=%#06x    %s\n" \
                                          "%s            expected %#06x    %s%s%s (%s)\n", \
                                 iTest, iVar ? "/n" : "", paTests[iTest].fFcw, paTests[iTest].fFswIn, paTests[iTest].iInVal, \
                                 iVar ? "  " : "", Res.FSW, FormatR80(&Res.r80Result), \
                                 iVar ? "  " : "", paTests[iTest].fFswOut, FormatR80(&paTests[iTest].rdResult), \
                                 FswDiff(Res.FSW, paTests[iTest].fFswOut), \
                                 !RTFLOAT80U_ARE_IDENTICAL(&Res.r80Result, &paTests[iTest].rdResult) ? " - val" : "", \
                                 FormatFcw(paTests[iTest].fFcw) ); \
            } \
            pfn = a_aSubTests[iFn].pfnNative; \
        } \
    } \
}

TEST_FPU_LOAD_INT(64, int64_t, "%RI64", FPU_LD_I64_T, g_aFpuLdU64, FPU_I64_IN_TEST_T)
TEST_FPU_LOAD_INT(32, int32_t, "%RI32", FPU_LD_I32_T, g_aFpuLdU32, FPU_I32_IN_TEST_T)
TEST_FPU_LOAD_INT(16, int16_t, "%RI16", FPU_LD_I16_T, g_aFpuLdU16, FPU_I16_IN_TEST_T)

#ifdef TSTIEMAIMPL_WITH_GENERATOR
static void FpuLdIntGenerate(PRTSTREAM pOut, uint32_t cTests)
{
    RTStrmPrintf(pOut, "\n\n#define HAVE_FPU_LD_INT\n");
    FpuLdI64Generate(pOut, cTests);
    FpuLdI32Generate(pOut, cTests);
    FpuLdI16Generate(pOut, cTests);
}
#endif

static void FpuLdIntTest(void)
{
    FpuLdI64Test();
    FpuLdI32Test();
    FpuLdI16Test();
}


/*
 * Load binary coded decimal values from memory.
 */

#ifndef HAVE_FPU_LD_BCD
static FPU_D80_IN_TEST_T const g_aTests_fld_r80_from_d80[] = { {0} };
#endif

typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLFPULDR80FROMD80,(PCX86FXSTATE, PIEMFPURESULT, PCRTPBCD80U));
typedef FNIEMAIMPLFPULDR80FROMD80 *PFNIEMAIMPLFPULDR80FROMD80;
typedef struct FPU_LD_D80_T
{
    const char                 *pszName;
    PFNIEMAIMPLFPULDR80FROMD80  pfn, pfnNative;
    FPU_D80_IN_TEST_T const    *paTests;
    uint32_t                    cTests;
    uint32_t                    uExtra;
    uint8_t                     idxCpuEflFlavour;
} FPU_LD_D80_T;

static const FPU_LD_D80_T g_aFpuLdD80[] =
{
    ENTRY(fld_r80_from_d80)
};


#ifdef TSTIEMAIMPL_WITH_GENERATOR
static void FpuLdD80Generate(PRTSTREAM pOut, uint32_t cTests)
{
    RTStrmPrintf(pOut, "\n\n#define HAVE_FPU_LD_BCD\n");
    X86FXSTATE State;
    RT_ZERO(State);
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aFpuLdD80); iFn++)
    {
        RTStrmPrintf(pOut, "static const FPU_D80_IN_TEST_T g_aTests_%s[] =\n{\n", g_aFpuLdD80[iFn].pszName); \
        for (uint32_t iTest = 0; iTest < cTests; iTest++)
        {
            State.FCW = RandFcw();
            State.FSW = RandFsw();
            RTPBCD80U InVal = RandD80Src(iTest);

            for (uint16_t iRounding = 0; iRounding < 4; iRounding++)
            {
                IEMFPURESULT Res = { RTFLOAT80U_INIT(0, 0, 0), 0 };
                State.FCW = (State.FCW & ~X86_FCW_RC_MASK) | (iRounding << X86_FCW_RC_SHIFT);
                g_aFpuLdD80[iFn].pfn(&State, &Res, &InVal);
                RTStrmPrintf(pOut, "    { %#06x, %#06x, %#06x, %s, %s }, /* #%u/%u */\n",
                             State.FCW, State.FSW, Res.FSW, GenFormatR80(&Res.r80Result), GenFormatD80(&InVal),
                             iTest, iRounding);
            }
        }
        RTStrmPrintf(pOut, "};\n");
    }
}
#endif

static void FpuLdD80Test(void)
{
    X86FXSTATE State;
    RT_ZERO(State);
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aFpuLdD80); iFn++)
    {
        RTTestSub(g_hTest, g_aFpuLdD80[iFn].pszName);

        uint32_t const                  cTests  = g_aFpuLdD80[iFn].cTests;
        FPU_D80_IN_TEST_T const * const paTests = g_aFpuLdD80[iFn].paTests;
        PFNIEMAIMPLFPULDR80FROMD80      pfn     = g_aFpuLdD80[iFn].pfn;
        uint32_t const cVars = 1 + (g_aFpuLdD80[iFn].idxCpuEflFlavour == g_idxCpuEflFlavour && g_aFpuLdD80[iFn].pfnNative);
        for (uint32_t iVar = 0; iVar < cVars; iVar++)
        {
            for (uint32_t iTest = 0; iTest < cTests; iTest++)
            {
                RTPBCD80U const InVal = paTests[iTest].InVal;
                State.FCW = paTests[iTest].fFcw;
                State.FSW = paTests[iTest].fFswIn;
                IEMFPURESULT Res = { RTFLOAT80U_INIT(0, 0, 0), 0 };
                pfn(&State, &Res, &InVal);
                if (   Res.FSW != paTests[iTest].fFswOut
                    || !RTFLOAT80U_ARE_IDENTICAL(&Res.r80Result, &paTests[iTest].rdResult))
                    RTTestFailed(g_hTest, "#%03u%s: fcw=%#06x fsw=%#06x in=%s\n"
                                          "%s              -> fsw=%#06x    %s\n"
                                          "%s            expected %#06x    %s%s%s (%s)\n",
                                 iTest, iVar ? "/n" : "", paTests[iTest].fFcw, paTests[iTest].fFswIn,
                                 FormatD80(&paTests[iTest].InVal),
                                 iVar ? "  " : "", Res.FSW, FormatR80(&Res.r80Result),
                                 iVar ? "  " : "", paTests[iTest].fFswOut, FormatR80(&paTests[iTest].rdResult),
                                 FswDiff(Res.FSW, paTests[iTest].fFswOut),
                                 !RTFLOAT80U_ARE_IDENTICAL(&Res.r80Result, &paTests[iTest].rdResult) ? " - val" : "",
                                 FormatFcw(paTests[iTest].fFcw) );
            }
            pfn = g_aFpuLdD80[iFn].pfnNative;
        }
    }
}


/*
 * Store values floating point values to memory.
 */

#ifndef HAVE_FPU_ST_MEM
static FPU_ST_R80_TEST_T const g_aTests_fst_r80_to_r80[] = { {0} };
static FPU_ST_R64_TEST_T const g_aTests_fst_r80_to_r64[] = { {0} };
static FPU_ST_R32_TEST_T const g_aTests_fst_r80_to_r32[] = { {0} };
#endif

#ifdef TSTIEMAIMPL_WITH_GENERATOR
# define GEN_FPU_STORE(a_cBits, a_rdType, a_aSubTests, a_TestType) \
static void FpuStR ## a_cBits ## Generate(PRTSTREAM pOut, uint32_t cTests) \
{ \
    X86FXSTATE State; \
    RT_ZERO(State); \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(a_aSubTests); iFn++) \
    { \
        RTStrmPrintf(pOut, "static const " #a_TestType " g_aTests_%s[] =\n{\n", a_aSubTests[iFn].pszName); \
        for (uint32_t iTest = 0; iTest < cTests; iTest += 1) \
        { \
            uint16_t const fFcw = RandFcw(); \
            State.FSW = RandFsw(); \
            RTFLOAT80U const InVal = RandR80Src(iTest); \
            \
            for (uint16_t iRounding = 0; iRounding < 4; iRounding++) \
            { \
                /* PC doesn't influence these, so leave as is. */ \
                AssertCompile(X86_FCW_OM_BIT + 1 == X86_FCW_UM_BIT && X86_FCW_UM_BIT + 1 == X86_FCW_PM_BIT); \
                for (uint16_t iMask = 0; iMask < 16; iMask += 2 /*1*/) \
                { \
                    uint16_t uFswOut = 0; \
                    a_rdType OutVal; \
                    RT_ZERO(OutVal); \
                    memset(&OutVal, 0xfe, sizeof(OutVal)); \
                    State.FCW = (fFcw & ~(X86_FCW_RC_MASK | X86_FCW_OM | X86_FCW_UM | X86_FCW_PM)) \
                              | (iRounding  << X86_FCW_RC_SHIFT); \
                    /*if (iMask & 1) State.FCW ^= X86_FCW_MASK_ALL;*/ \
                    State.FCW |= (iMask >> 1) << X86_FCW_OM_BIT; \
                    a_aSubTests[iFn].pfn(&State, &uFswOut, &OutVal, &InVal); \
                    RTStrmPrintf(pOut, "    { %#06x, %#06x, %#06x, %s, %s }, /* #%u/%u/%u */\n", \
                                 State.FCW, State.FSW, uFswOut, GenFormatR80(&InVal), \
                                 GenFormatR ## a_cBits(&OutVal), iTest, iRounding, iMask); \
                } \
            } \
        } \
        RTStrmPrintf(pOut, "};\n"); \
    } \
}
#else
# define GEN_FPU_STORE(a_cBits, a_rdType, a_aSubTests, a_TestType)
#endif

#define TEST_FPU_STORE(a_cBits, a_rdType, a_SubTestType, a_aSubTests, a_TestType) \
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLFPUSTR80TOR ## a_cBits,(PCX86FXSTATE, uint16_t *, \
                                                                   PRTFLOAT ## a_cBits ## U, PCRTFLOAT80U)); \
typedef FNIEMAIMPLFPUSTR80TOR ## a_cBits *PFNIEMAIMPLFPUSTR80TOR ## a_cBits; \
typedef struct a_SubTestType \
{ \
    const char                             *pszName; \
    PFNIEMAIMPLFPUSTR80TOR ## a_cBits       pfn, pfnNative; \
    a_TestType const                       *paTests; \
    uint32_t                                cTests; \
    uint32_t                                uExtra; \
    uint8_t                                 idxCpuEflFlavour; \
} a_SubTestType; \
\
static const a_SubTestType a_aSubTests[] = \
{ \
    ENTRY(RT_CONCAT(fst_r80_to_r,a_cBits)) \
}; \
GEN_FPU_STORE(a_cBits, a_rdType, a_aSubTests, a_TestType) \
\
static void FpuStR ## a_cBits ## Test(void) \
{ \
    X86FXSTATE State; \
    RT_ZERO(State); \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(a_aSubTests); iFn++) \
    { \
        RTTestSub(g_hTest, a_aSubTests[iFn].pszName); \
        \
        uint32_t const                    cTests  = a_aSubTests[iFn].cTests; \
        a_TestType const          * const paTests = a_aSubTests[iFn].paTests; \
        PFNIEMAIMPLFPUSTR80TOR ## a_cBits pfn     = a_aSubTests[iFn].pfn; \
        uint32_t const cVars = 1 + (a_aSubTests[iFn].idxCpuEflFlavour == g_idxCpuEflFlavour && a_aSubTests[iFn].pfnNative); \
        for (uint32_t iVar = 0; iVar < cVars; iVar++) \
        { \
            for (uint32_t iTest = 0; iTest < cTests; iTest++) \
            { \
                RTFLOAT80U const InVal   = paTests[iTest].InVal; \
                uint16_t         uFswOut = 0; \
                a_rdType         OutVal; \
                RT_ZERO(OutVal); \
                memset(&OutVal, 0xfe, sizeof(OutVal)); \
                State.FCW = paTests[iTest].fFcw; \
                State.FSW = paTests[iTest].fFswIn; \
                pfn(&State, &uFswOut, &OutVal, &InVal); \
                if (   uFswOut != paTests[iTest].fFswOut \
                    || !RTFLOAT ## a_cBits ## U_ARE_IDENTICAL(&OutVal, &paTests[iTest].OutVal)) \
                    RTTestFailed(g_hTest, "#%04u%s: fcw=%#06x fsw=%#06x in=%s\n" \
                                          "%s               -> fsw=%#06x    %s\n" \
                                          "%s             expected %#06x    %s%s%s (%s)\n", \
                                 iTest, iVar ? "/n" : "", paTests[iTest].fFcw, paTests[iTest].fFswIn, \
                                 FormatR80(&paTests[iTest].InVal), \
                                 iVar ? "  " : "", uFswOut, FormatR ## a_cBits(&OutVal), \
                                 iVar ? "  " : "", paTests[iTest].fFswOut, FormatR ## a_cBits(&paTests[iTest].OutVal), \
                                 FswDiff(uFswOut, paTests[iTest].fFswOut), \
                                 !RTFLOAT ## a_cBits ## U_ARE_IDENTICAL(&OutVal, &paTests[iTest].OutVal) ? " - val" : "", \
                                 FormatFcw(paTests[iTest].fFcw) ); \
            } \
            pfn = a_aSubTests[iFn].pfnNative; \
        } \
    } \
}

TEST_FPU_STORE(80, RTFLOAT80U, FPU_ST_R80_T, g_aFpuStR80, FPU_ST_R80_TEST_T)
TEST_FPU_STORE(64, RTFLOAT64U, FPU_ST_R64_T, g_aFpuStR64, FPU_ST_R64_TEST_T)
TEST_FPU_STORE(32, RTFLOAT32U, FPU_ST_R32_T, g_aFpuStR32, FPU_ST_R32_TEST_T)

#ifdef TSTIEMAIMPL_WITH_GENERATOR
static void FpuStMemGenerate(PRTSTREAM pOut, uint32_t cTests)
{
    RTStrmPrintf(pOut, "\n\n#define HAVE_FPU_ST_MEM\n");
    FpuStR80Generate(pOut, cTests);
    FpuStR64Generate(pOut, cTests);
    FpuStR32Generate(pOut, cTests);
}
#endif

static void FpuStMemTest(void)
{
    FpuStR80Test();
    FpuStR64Test();
    FpuStR32Test();
}


/*
 * Store integer values to memory or register.
 */

#ifndef HAVE_FPU_ST_INT
static FPU_ST_I64_TEST_T const g_aTests_fist_r80_to_i64[] = { {0} };
static FPU_ST_I32_TEST_T const g_aTests_fist_r80_to_i32[] = { {0} };
static FPU_ST_I16_TEST_T const g_aTests_fist_r80_to_i16[] = { {0} };
static FPU_ST_I64_TEST_T const g_aTests_fistt_r80_to_i64[] = { {0} };
static FPU_ST_I32_TEST_T const g_aTests_fistt_r80_to_i32[] = { {0} };
static FPU_ST_I16_TEST_T const g_aTests_fistt_r80_to_i16[] = { {0} };
#endif

#ifdef TSTIEMAIMPL_WITH_GENERATOR
# define GEN_FPU_STORE_INT(a_cBits, a_iType, a_szFmt, a_aSubTests, a_TestType) \
static void FpuStI ## a_cBits ## Generate(PRTSTREAM pOut, uint32_t cTests) \
{ \
    X86FXSTATE State; \
    RT_ZERO(State); \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(a_aSubTests); iFn++) \
    { \
        RTStrmPrintf(pOut, "static const " #a_TestType " g_aTests_%s[] =\n{\n", a_aSubTests[iFn].pszName); \
        for (uint32_t iTest = 0; iTest < cTests; iTest += 1) \
        { \
            uint16_t const fFcw = RandFcw(); \
            State.FSW = RandFsw(); \
            RTFLOAT80U const InVal = RandR80Src(iTest); \
            \
            for (uint16_t iRounding = 0; iRounding < 4; iRounding++) \
            { \
                /* PC doesn't influence these, so leave as is. */ \
                AssertCompile(X86_FCW_OM_BIT + 1 == X86_FCW_UM_BIT && X86_FCW_UM_BIT + 1 == X86_FCW_PM_BIT); \
                for (uint16_t iMask = 0; iMask < 16; iMask += 2 /*1*/) \
                { \
                    uint16_t uFswOut = 0; \
                    a_iType  iOutVal = ~(a_iType)2; \
                    State.FCW = (fFcw & ~(X86_FCW_RC_MASK | X86_FCW_OM | X86_FCW_UM | X86_FCW_PM)) \
                              | (iRounding  << X86_FCW_RC_SHIFT); \
                    /*if (iMask & 1) State.FCW ^= X86_FCW_MASK_ALL;*/ \
                    State.FCW |= (iMask >> 1) << X86_FCW_OM_BIT; \
                    a_aSubTests[iFn].pfn(&State, &uFswOut, &iOutVal, &InVal); \
                    RTStrmPrintf(pOut, "    { %#06x, %#06x, %#06x, %s, %s }, /* #%u/%u/%u */\n", \
                                 State.FCW, State.FSW, uFswOut, GenFormatR80(&InVal), \
                                 GenFormatI ## a_cBits(iOutVal), iTest, iRounding, iMask); \
                } \
            } \
        } \
        RTStrmPrintf(pOut, "};\n"); \
    } \
}
#else
# define GEN_FPU_STORE_INT(a_cBits, a_iType, a_szFmt, a_aSubTests, a_TestType)
#endif

#define TEST_FPU_STORE_INT(a_cBits, a_iType, a_szFmt, a_SubTestType, a_aSubTests, a_TestType) \
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLFPUSTR80TOI ## a_cBits,(PCX86FXSTATE, uint16_t *, a_iType *, PCRTFLOAT80U)); \
typedef FNIEMAIMPLFPUSTR80TOI ## a_cBits *PFNIEMAIMPLFPUSTR80TOI ## a_cBits; \
typedef struct a_SubTestType \
{ \
    const char                             *pszName; \
    PFNIEMAIMPLFPUSTR80TOI ## a_cBits       pfn, pfnNative; \
    a_TestType const                       *paTests; \
    uint32_t                                cTests; \
    uint32_t                                uExtra; \
    uint8_t                                 idxCpuEflFlavour; \
} a_SubTestType; \
\
static const a_SubTestType a_aSubTests[] = \
{ \
    ENTRY(RT_CONCAT(fist_r80_to_i,a_cBits)), \
    ENTRY(RT_CONCAT(fistt_r80_to_i,a_cBits)) \
}; \
GEN_FPU_STORE_INT(a_cBits, a_iType, a_szFmt, a_aSubTests, a_TestType) \
\
static void FpuStI ## a_cBits ## Test(void) \
{ \
    X86FXSTATE State; \
    RT_ZERO(State); \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(a_aSubTests); iFn++) \
    { \
        RTTestSub(g_hTest, a_aSubTests[iFn].pszName); \
        \
        uint32_t const                    cTests  = a_aSubTests[iFn].cTests; \
        a_TestType const          * const paTests = a_aSubTests[iFn].paTests; \
        PFNIEMAIMPLFPUSTR80TOI ## a_cBits pfn     = a_aSubTests[iFn].pfn; \
        uint32_t const cVars = 1 + (a_aSubTests[iFn].idxCpuEflFlavour == g_idxCpuEflFlavour && a_aSubTests[iFn].pfnNative); \
        for (uint32_t iVar = 0; iVar < cVars; iVar++) \
        { \
            for (uint32_t iTest = 0; iTest < cTests; iTest++) \
            { \
                RTFLOAT80U const InVal   = paTests[iTest].InVal; \
                uint16_t         uFswOut = 0; \
                a_iType          iOutVal = ~(a_iType)2; \
                State.FCW = paTests[iTest].fFcw; \
                State.FSW = paTests[iTest].fFswIn; \
                pfn(&State, &uFswOut, &iOutVal, &InVal); \
                if (   uFswOut != paTests[iTest].fFswOut \
                    || iOutVal != paTests[iTest].iOutVal) \
                    RTTestFailed(g_hTest, "#%04u%s: fcw=%#06x fsw=%#06x in=%s\n" \
                                          "%s               -> fsw=%#06x    " a_szFmt "\n" \
                                          "%s             expected %#06x    " a_szFmt "%s%s (%s)\n", \
                                 iTest, iVar ? "/n" : "", paTests[iTest].fFcw, paTests[iTest].fFswIn, \
                                 FormatR80(&paTests[iTest].InVal), \
                                 iVar ? "  " : "", uFswOut, iOutVal, \
                                 iVar ? "  " : "", paTests[iTest].fFswOut, paTests[iTest].iOutVal, \
                                 FswDiff(uFswOut, paTests[iTest].fFswOut), \
                                 iOutVal != paTests[iTest].iOutVal ? " - val" : "", FormatFcw(paTests[iTest].fFcw) ); \
            } \
            pfn = a_aSubTests[iFn].pfnNative; \
        } \
    } \
}

TEST_FPU_STORE_INT(64, int64_t, "%RI64", FPU_ST_I64_T, g_aFpuStI64, FPU_ST_I64_TEST_T)
TEST_FPU_STORE_INT(32, int32_t, "%RI32", FPU_ST_I32_T, g_aFpuStI32, FPU_ST_I32_TEST_T)
TEST_FPU_STORE_INT(16, int16_t, "%RI16", FPU_ST_I16_T, g_aFpuStI16, FPU_ST_I16_TEST_T)

#ifdef TSTIEMAIMPL_WITH_GENERATOR
static void FpuStIntGenerate(PRTSTREAM pOut, uint32_t cTests)
{
    RTStrmPrintf(pOut, "\n\n#define HAVE_FPU_ST_INT\n");
    FpuStI64Generate(pOut, cTests);
    FpuStI32Generate(pOut, cTests);
    FpuStI16Generate(pOut, cTests);
}
#endif

static void FpuStIntTest(void)
{
    FpuStI64Test();
    FpuStI32Test();
    FpuStI16Test();
}


/*
 * Store as packed BCD value (memory).
 */

#ifndef HAVE_FPU_ST_BCD
static FPU_ST_D80_TEST_T const g_aTests_fst_r80_to_d80[] = { {0} };
#endif

typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLFPUSTR80TOD80,(PCX86FXSTATE, uint16_t *, PRTPBCD80U, PCRTFLOAT80U));
typedef FNIEMAIMPLFPUSTR80TOD80 *PFNIEMAIMPLFPUSTR80TOD80;
typedef struct FPU_ST_D80_T
{
    const char                 *pszName;
    PFNIEMAIMPLFPUSTR80TOD80    pfn, pfnNative;
    FPU_ST_D80_TEST_T const    *paTests;
    uint32_t                    cTests;
    uint32_t                    uExtra;
    uint8_t                     idxCpuEflFlavour;
} FPU_ST_D80_T;

static const FPU_ST_D80_T g_aFpuStD80[] =
{
    ENTRY(fst_r80_to_d80),
};

#ifdef TSTIEMAIMPL_WITH_GENERATOR
static void FpuStD80Generate(PRTSTREAM pOut, uint32_t cTests)
{
    RTStrmPrintf(pOut, "\n\n#define HAVE_FPU_ST_BCD\n");
    X86FXSTATE State;
    RT_ZERO(State);
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aFpuStD80); iFn++)
    {
        RTStrmPrintf(pOut, "static const FPU_ST_D80_TEST_T g_aTests_%s[] =\n{\n", g_aFpuStD80[iFn].pszName);
        for (uint32_t iTest = 0; iTest < cTests; iTest += 1)
        {
            uint16_t const fFcw = RandFcw();
            State.FSW = RandFsw();
            RTFLOAT80U const InVal = RandR80Src(iTest);

            for (uint16_t iRounding = 0; iRounding < 4; iRounding++)
            {
                /* PC doesn't influence these, so leave as is. */
                AssertCompile(X86_FCW_OM_BIT + 1 == X86_FCW_UM_BIT && X86_FCW_UM_BIT + 1 == X86_FCW_PM_BIT);
                for (uint16_t iMask = 0; iMask < 16; iMask += 2 /*1*/)
                {
                    uint16_t  uFswOut = 0;
                    RTPBCD80U OutVal  = RTPBCD80U_INIT_ZERO(0);
                    State.FCW = (fFcw & ~(X86_FCW_RC_MASK | X86_FCW_OM | X86_FCW_UM | X86_FCW_PM))
                              | (iRounding  << X86_FCW_RC_SHIFT);
                    /*if (iMask & 1) State.FCW ^= X86_FCW_MASK_ALL;*/
                    State.FCW |= (iMask >> 1) << X86_FCW_OM_BIT;
                    g_aFpuStD80[iFn].pfn(&State, &uFswOut, &OutVal, &InVal);
                    RTStrmPrintf(pOut, "    { %#06x, %#06x, %#06x, %s, %s }, /* #%u/%u/%u */\n",
                                 State.FCW, State.FSW, uFswOut, GenFormatR80(&InVal),
                                 GenFormatD80(&OutVal), iTest, iRounding, iMask);
                }
            }
        }
        RTStrmPrintf(pOut, "};\n");
    }
}
#endif


static void FpuStD80Test(void)
{
    X86FXSTATE State;
    RT_ZERO(State);
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aFpuStD80); iFn++)
    {
        RTTestSub(g_hTest, g_aFpuStD80[iFn].pszName);

        uint32_t const                  cTests  = g_aFpuStD80[iFn].cTests;
        FPU_ST_D80_TEST_T const * const paTests = g_aFpuStD80[iFn].paTests;
        PFNIEMAIMPLFPUSTR80TOD80        pfn     = g_aFpuStD80[iFn].pfn;
        uint32_t const cVars = 1 + (g_aFpuStD80[iFn].idxCpuEflFlavour == g_idxCpuEflFlavour && g_aFpuStD80[iFn].pfnNative);
        for (uint32_t iVar = 0; iVar < cVars; iVar++)
        {
            for (uint32_t iTest = 0; iTest < cTests; iTest++)
            {
                RTFLOAT80U const InVal   = paTests[iTest].InVal;
                uint16_t         uFswOut = 0;
                RTPBCD80U        OutVal  = RTPBCD80U_INIT_ZERO(0);
                State.FCW = paTests[iTest].fFcw;
                State.FSW = paTests[iTest].fFswIn;
                pfn(&State, &uFswOut, &OutVal, &InVal);
                if (   uFswOut != paTests[iTest].fFswOut
                    || !RTPBCD80U_ARE_IDENTICAL(&OutVal, &paTests[iTest].OutVal))
                    RTTestFailed(g_hTest, "#%04u%s: fcw=%#06x fsw=%#06x in=%s\n"
                                          "%s               -> fsw=%#06x    %s\n"
                                          "%s             expected %#06x    %s%s%s (%s)\n",
                                 iTest, iVar ? "/n" : "", paTests[iTest].fFcw, paTests[iTest].fFswIn,
                                 FormatR80(&paTests[iTest].InVal),
                                 iVar ? "  " : "", uFswOut, FormatD80(&OutVal),
                                 iVar ? "  " : "", paTests[iTest].fFswOut, FormatD80(&paTests[iTest].OutVal),
                                 FswDiff(uFswOut, paTests[iTest].fFswOut),
                                 RTPBCD80U_ARE_IDENTICAL(&OutVal, &paTests[iTest].OutVal) ? " - val" : "",
                                 FormatFcw(paTests[iTest].fFcw) );
            }
            pfn = g_aFpuStD80[iFn].pfnNative;
        }
    }
}


int main(int argc, char **argv)
{
    int rc = RTR3InitExe(argc, &argv, 0);
    if (RT_FAILURE(rc))
        return RTMsgInitFailure(rc);

    /*
     * Determin the host CPU.
     * If not using the IEMAllAImpl.asm code, this will be set to Intel.
     */
#if (defined(RT_ARCH_X86) || defined(RT_ARCH_AMD64)) && !defined(IEM_WITHOUT_ASSEMBLY)
    g_idxCpuEflFlavour = ASMIsAmdCpu() || ASMIsHygonCpu()
                       ? IEMTARGETCPU_EFL_BEHAVIOR_AMD
                       : IEMTARGETCPU_EFL_BEHAVIOR_INTEL;
#else
    g_idxCpuEflFlavour = IEMTARGETCPU_EFL_BEHAVIOR_INTEL;
#endif

    /*
     * Parse arguments.
     */
    enum { kModeNotSet, kModeTest, kModeGenerate }
                        enmMode       = kModeNotSet;
    bool                fInt          = true;
    bool                fFpuLdSt      = true;
    bool                fFpuOther     = true;
    bool                fCpuData      = true;
    bool                fCommonData   = true;
    uint32_t const      cDefaultTests = 96;
    uint32_t            cTests        = cDefaultTests;
    RTGETOPTDEF const   s_aOptions[]  =
    {
        // mode:
        { "--generate",             'g', RTGETOPT_REQ_NOTHING },
        { "--test",                 't', RTGETOPT_REQ_NOTHING },
        // test selection (both)
        { "--all",                  'a', RTGETOPT_REQ_NOTHING },
        { "--none",                 'z', RTGETOPT_REQ_NOTHING },
        { "--zap",                  'z', RTGETOPT_REQ_NOTHING },
        { "--fpu-ld-st",            'f', RTGETOPT_REQ_NOTHING },
        { "--fpu-load-store",       'f', RTGETOPT_REQ_NOTHING },
        { "--fpu-other",            'F', RTGETOPT_REQ_NOTHING },
        { "--int",                  'i', RTGETOPT_REQ_NOTHING },
        // generation parameters
        { "--common",               'm', RTGETOPT_REQ_NOTHING },
        { "--cpu",                  'c', RTGETOPT_REQ_NOTHING },
        { "--number-of-tests",      'n', RTGETOPT_REQ_UINT32  },
    };

    RTGETOPTSTATE State;
    rc = RTGetOptInit(&State, argc, argv, s_aOptions, RT_ELEMENTS(s_aOptions), 1, 0);
    AssertRCReturn(rc, RTEXITCODE_FAILURE);

    RTGETOPTUNION ValueUnion;
    while ((rc = RTGetOpt(&State, &ValueUnion)))
    {
        switch (rc)
        {
            case 'g':
                enmMode     = kModeGenerate;
                break;
            case 't':
                enmMode     = kModeTest;
                break;
            case 'a':
                fCpuData    = true;
                fCommonData = true;
                fInt        = true;
                fFpuLdSt    = true;
                fFpuOther   = true;
                break;
            case 'z':
                fCpuData    = false;
                fCommonData = false;
                fInt        = false;
                fFpuLdSt    = false;
                fFpuOther   = false;
                break;
            case 'f':
                fFpuLdSt    = true;
                break;
            case 'F':
                fFpuOther   = true;
                break;
            case 'i':
                fInt        = true;
                break;
            case 'm':
                fCommonData = true;
                break;
            case 'c':
                fCpuData    = true;
                break;
            case 'n':
                cTests      = ValueUnion.u32;
                break;
            case 'h':
                RTPrintf("usage: %s <-g|-t> [options]\n"
                         "\n"
                         "Mode:\n"
                         "  -g, --generate\n"
                         "    Generate test data.\n"
                         "  -t, --test\n"
                         "    Execute tests.\n"
                         "\n"
                         "Test selection (both modes):\n"
                         "  -a, --all\n"
                         "    Enable all tests and generated test data. (default)\n"
                         "  -z, --zap, --none\n"
                         "    Disable all tests and test data types.\n"
                         "  -i, --int\n"
                         "    Enable non-FPU tests.\n"
                         "  -f, --fpu-ld-st\n"
                         "    Enable FPU load and store tests.\n"
                         "  -f, --fpu-other\n"
                         "    Enable other FPU tests.\n"
                         "\n"
                         "Generation:\n"
                         "  -m, --common\n"
                         "    Enable generating common test data.\n"
                         "  -c, --only-cpu\n"
                         "    Enable generating CPU specific test data.\n"
                         "  -n, --number-of-test <count>\n"
                         "    Number of tests to generate. Default: %u\n"
                         , argv[0], cDefaultTests);
                return RTEXITCODE_SUCCESS;
            default:
                return RTGetOptPrintError(rc, &ValueUnion);
        }
    }

    /*
     * Generate data?
     */
    if (enmMode == kModeGenerate)
    {
#ifdef TSTIEMAIMPL_WITH_GENERATOR
        char szCpuDesc[256] = {0};
        RTMpGetDescription(NIL_RTCPUID, szCpuDesc, sizeof(szCpuDesc));
        const char * const pszCpuType  = g_idxCpuEflFlavour == IEMTARGETCPU_EFL_BEHAVIOR_AMD ? "Amd"  : "Intel";
        const char * const pszCpuSuff  = g_idxCpuEflFlavour == IEMTARGETCPU_EFL_BEHAVIOR_AMD ? "_Amd" : "_Intel";
        const char * const pszCpuSuffU = g_idxCpuEflFlavour == IEMTARGETCPU_EFL_BEHAVIOR_AMD ? "_AMD" : "_INTEL";
# if defined(RT_OS_WINDOWS) || defined(RT_OS_OS2)
        const char * const pszBitBucket = "NUL";
# else
        const char * const pszBitBucket = "/dev/null";
# endif

        if (cTests == 0)
            cTests = cDefaultTests;
        g_cZeroDstTests = RT_MIN(cTests / 16, 32);
        g_cZeroSrcTests = g_cZeroDstTests * 2;

        if (fInt)
        {
            const char *pszDataFile = fCommonData ? "tstIEMAImplData.h" : pszBitBucket;
            PRTSTREAM   pStrmData   = NULL;
            rc = RTStrmOpen(pszDataFile, "w", &pStrmData);
            if (!pStrmData)
                return RTMsgErrorExitFailure("Failed to open %s for writing: %Rrc", pszDataFile, rc);

            const char *pszDataCpuFile = !fCpuData ? pszBitBucket : g_idxCpuEflFlavour == IEMTARGETCPU_EFL_BEHAVIOR_AMD
                                       ? "tstIEMAImplData-Amd.h" : "tstIEMAImplData-Intel.h";
            PRTSTREAM   pStrmDataCpu   = NULL;
            rc = RTStrmOpen(pszDataCpuFile, "w", &pStrmDataCpu);
            if (!pStrmData)
                return RTMsgErrorExitFailure("Failed to open %s for writing: %Rrc", pszDataCpuFile, rc);

            GenerateHeader(pStrmData, "", szCpuDesc, NULL, "");
            GenerateHeader(pStrmDataCpu, "", szCpuDesc, pszCpuType, pszCpuSuff);

            BinU8Generate( pStrmData, pStrmDataCpu, pszCpuSuffU, cTests);
            BinU16Generate(pStrmData, pStrmDataCpu, pszCpuSuffU, cTests);
            BinU32Generate(pStrmData, pStrmDataCpu, pszCpuSuffU, cTests);
            BinU64Generate(pStrmData, pStrmDataCpu, pszCpuSuffU, cTests);
            ShiftDblGenerate(pStrmDataCpu, pszCpuSuffU, RT_MAX(cTests, 128));
            UnaryGenerate(pStrmData, cTests);
            ShiftGenerate(pStrmDataCpu, pszCpuSuffU, cTests);
            MulDivGenerate(pStrmDataCpu, pszCpuSuffU, cTests);

            RTEXITCODE rcExit = GenerateFooterAndClose(pStrmDataCpu, pszDataCpuFile, "", pszCpuSuff,
                                                       GenerateFooterAndClose(pStrmData, pszDataFile, "", "",
                                                                              RTEXITCODE_SUCCESS));
            if (rcExit != RTEXITCODE_SUCCESS)
                return rcExit;
        }

        if (fFpuLdSt)
        {
            const char *pszDataFile = fCommonData ? "tstIEMAImplDataFpuLdSt.h" : pszBitBucket;
            PRTSTREAM   pStrmData   = NULL;
            rc = RTStrmOpen(pszDataFile, "w", &pStrmData);
            if (!pStrmData)
                return RTMsgErrorExitFailure("Failed to open %s for writing: %Rrc", pszDataFile, rc);

            const char *pszDataCpuFile = !fCpuData ? pszBitBucket : g_idxCpuEflFlavour == IEMTARGETCPU_EFL_BEHAVIOR_AMD
                                       ? "tstIEMAImplDataFpuLdSt-Amd.h" : "tstIEMAImplDataFpuLdSt-Intel.h";
            PRTSTREAM   pStrmDataCpu   = NULL;
            rc = RTStrmOpen(pszDataCpuFile, "w", &pStrmDataCpu);
            if (!pStrmData)
                return RTMsgErrorExitFailure("Failed to open %s for writing: %Rrc", pszDataCpuFile, rc);

            GenerateHeader(pStrmData, "Fpu", szCpuDesc, NULL, "");
            GenerateHeader(pStrmDataCpu, "Fpu", szCpuDesc, pszCpuType, pszCpuSuff);

            FpuLdConstGenerate(pStrmData, cTests);
            FpuLdIntGenerate(pStrmData, cTests);
            FpuLdD80Generate(pStrmData, cTests);
            FpuStIntGenerate(pStrmData, cTests);
            FpuStD80Generate(pStrmData, cTests);
            cTests = RT_MAX(cTests, 384); /* need better coverage for the next ones. */
            FpuLdMemGenerate(pStrmData, cTests);
            FpuStMemGenerate(pStrmData, cTests);

            RTEXITCODE rcExit = GenerateFooterAndClose(pStrmDataCpu, pszDataCpuFile, "FpuLdSt", pszCpuSuff,
                                                       GenerateFooterAndClose(pStrmData, pszDataFile, "FpuLdSt", "",
                                                                              RTEXITCODE_SUCCESS));
            if (rcExit != RTEXITCODE_SUCCESS)
                return rcExit;
        }

        if (fFpuOther)
        {
# if 0
            const char *pszDataFile = fCommonData ? "tstIEMAImplDataFpuOther.h" : pszBitBucket;
            PRTSTREAM   pStrmData   = NULL;
            rc = RTStrmOpen(pszDataFile, "w", &pStrmData);
            if (!pStrmData)
                return RTMsgErrorExitFailure("Failed to open %s for writing: %Rrc", pszDataFile, rc);

            const char *pszDataCpuFile = !fCpuData ? pszBitBucket : g_idxCpuEflFlavour == IEMTARGETCPU_EFL_BEHAVIOR_AMD
                                       ? "tstIEMAImplDataFpuOther-Amd.h" : "tstIEMAImplDataFpuOther-Intel.h";
            PRTSTREAM   pStrmDataCpu   = NULL;
            rc = RTStrmOpen(pszDataCpuFile, "w", &pStrmDataCpu);
            if (!pStrmData)
                return RTMsgErrorExitFailure("Failed to open %s for writing: %Rrc", pszDataCpuFile, rc);

            GenerateHeader(pStrmData, "Fpu", szCpuDesc, NULL, "");
            GenerateHeader(pStrmDataCpu, "Fpu", szCpuDesc, pszCpuType, pszCpuSuff);

            /* later */

            RTEXITCODE rcExit = GenerateFooterAndClose(pStrmDataCpu, pszDataCpuFile, "FpuOther", pszCpuSuff,
                                                       GenerateFooterAndClose(pStrmData, pszDataFile, "FpuOther", "",
                                                                              RTEXITCODE_SUCCESS));
            if (rcExit != RTEXITCODE_SUCCESS)
                return rcExit;
# endif
        }

        return RTEXITCODE_SUCCESS;
#else
        return RTMsgErrorExitFailure("Test data generator not compiled in!");
#endif
    }

    /*
     * Do testing.  Currrently disabled by default as data needs to be checked
     * on both intel and AMD systems first.
     */
    rc = RTTestCreate("tstIEMAimpl", &g_hTest);
    AssertRCReturn(rc, RTEXITCODE_FAILURE);
    if (enmMode == kModeTest)
    {
        RTTestBanner(g_hTest);

        /* Allocate guarded memory for use in the tests. */
#define ALLOC_GUARDED_VAR(a_puVar) do { \
            rc = RTTestGuardedAlloc(g_hTest, sizeof(*a_puVar), sizeof(*a_puVar), false /*fHead*/, (void **)&a_puVar); \
            if (RT_FAILURE(rc)) RTTestFailed(g_hTest, "Failed to allocate guarded mem: " #a_puVar); \
        } while (0)
        ALLOC_GUARDED_VAR(g_pu8);
        ALLOC_GUARDED_VAR(g_pu16);
        ALLOC_GUARDED_VAR(g_pu32);
        ALLOC_GUARDED_VAR(g_pu64);
        ALLOC_GUARDED_VAR(g_pu128);
        ALLOC_GUARDED_VAR(g_pu8Two);
        ALLOC_GUARDED_VAR(g_pu16Two);
        ALLOC_GUARDED_VAR(g_pu32Two);
        ALLOC_GUARDED_VAR(g_pu64Two);
        ALLOC_GUARDED_VAR(g_pu128Two);
        ALLOC_GUARDED_VAR(g_pfEfl);
        if (RTTestErrorCount(g_hTest) == 0)
        {
            if (fInt)
            {
                BinU8Test();
                BinU16Test();
                BinU32Test();
                BinU64Test();
                XchgTest();
                XaddTest();
                CmpXchgTest();
                CmpXchg8bTest();
                CmpXchg16bTest();
                ShiftDblTest();
                UnaryTest();
                ShiftTest();
                MulDivTest();
                BswapTest();
            }

            if (fFpuLdSt)
            {
                FpuLoadConstTest();
                FpuLdMemTest();
                FpuLdIntTest();
                FpuLdD80Test();
                FpuStMemTest();
                FpuStIntTest();
                FpuStD80Test();
            }
        }
        return RTTestSummaryAndDestroy(g_hTest);
    }
    return RTTestSkipAndDestroy(g_hTest, "unfinished testcase");
}
