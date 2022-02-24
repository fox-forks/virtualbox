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
    BINU8_TEST_T const     *paTests;
    uint32_t                cTests;
    uint32_t                uExtra;
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
    BINU16_TEST_T const    *paTests;
    uint32_t                cTests;
    uint32_t                uExtra;
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
    BINU32_TEST_T const    *paTests;
    uint32_t                cTests;
    uint32_t                uExtra;
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
    BINU64_TEST_T const    *paTests;
    uint32_t                cTests;
    uint32_t                uExtra;
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
    { #a_Name, iemAImpl_ ## a_Name, g_aTests_ ## a_Name, RT_ELEMENTS(g_aTests_ ## a_Name), a_uExtra }


/*********************************************************************************************************************************
*   Internal Functions                                                                                                           *
*********************************************************************************************************************************/
static uint32_t     RandEFlags(void);
static uint8_t      RandU8(void);
static uint16_t     RandU16(void);
static uint32_t     RandU32(void);
static uint64_t     RandU64(void);
static RTUINT128U   RandU128(void);


/*********************************************************************************************************************************
*   Global Variables                                                                                                             *
*********************************************************************************************************************************/
static RTTEST       g_hTest;
static uint8_t     *g_pu8,   *g_pu8Two;
static uint16_t    *g_pu16,  *g_pu16Two;
static uint32_t    *g_pu32,  *g_pu32Two,  *g_pfEfl;
static uint64_t    *g_pu64,  *g_pu64Two;
static RTUINT128U  *g_pu128, *g_pu128Two;


#include "tstIEMAImplData.h"

/*
 * Test helpers.
 */
static const char *EFlagsDiff(uint32_t fActual, uint32_t fExpected)
{
    if (fActual == fExpected)
        return "";

    uint32_t const fXor = fActual ^ fExpected;
    static char    s_szBuf[256];
    size_t cch = RTStrPrintf(s_szBuf, sizeof(s_szBuf), " - %#x", fXor);

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
            cch += RTStrPrintf(&s_szBuf[cch], sizeof(s_szBuf) - cch,
                               s_aFlags[i].fFlag & fActual ? "/%s" : "/!%s", s_aFlags[i].pszName);
    RTStrPrintf(&s_szBuf[cch], sizeof(s_szBuf) - cch, "");
    return s_szBuf;
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


static void BinU8Generate(uint32_t cTests)
{
    RTPrintf("\n\n#define HAVE_BINU8_TESTS\n");
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aBinU8); iFn++)
    {
        RTPrintf("static const BINU8_TEST_T g_aTests_%s[] =\n{\n", g_aBinU8[iFn].pszName);
        for (uint32_t iTest = 0; iTest < cTests; iTest++ )
        {
            BINU8_TEST_T Test;
            Test.fEflIn    = RandEFlags();
            Test.fEflOut   = Test.fEflIn;
            Test.uDstIn    = RandU8();
            Test.uDstOut   = Test.uDstIn;
            Test.uSrcIn    = RandU8();
            Test.uMisc     = 0;
            g_aBinU8[iFn].pfn(&Test.uDstOut, Test.uSrcIn, &Test.fEflOut);
            RTPrintf("    { %#08x, %#08x, %#04x, %#04x, %#04x, %#x }, /* #%u */\n",
                     Test.fEflIn, Test.fEflOut, Test.uDstIn, Test.uDstOut, Test.uSrcIn, Test.uMisc, iTest);
        }
        RTPrintf("};\n");
    }
}

static void BinU8Test(void)
{
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aBinU8); iFn++)
    {
        RTTestSub(g_hTest, g_aBinU8[iFn].pszName);

        BINU8_TEST_T const * const paTests = g_aBinU8[iFn].paTests;
        uint32_t const             cTests  = g_aBinU8[iFn].cTests;
        for (uint32_t iTest = 0; iTest < cTests; iTest++ )
        {
            uint32_t fEfl = paTests[iTest].fEflIn;
            uint8_t  uDst = paTests[iTest].uDstIn;
            g_aBinU8[iFn].pfn(&uDst, paTests[iTest].uSrcIn, &fEfl);
            if (   uDst != paTests[iTest].uDstOut
                || fEfl != paTests[iTest].fEflOut)
                RTTestFailed(g_hTest, "#%u: efl=%#08x dst=%#04x src=%#04x -> efl=%#08x dst=%#04x, expected %#08x & %#04x%s - %s\n",
                             iTest, paTests[iTest].fEflIn, paTests[iTest].uDstIn, paTests[iTest].uSrcIn,
                             fEfl, uDst, paTests[iTest].fEflOut, paTests[iTest].uDstOut,
                             EFlagsDiff(fEfl, paTests[iTest].fEflOut),
                             uDst == paTests[iTest].uDstOut ? "eflags" : fEfl == paTests[iTest].fEflOut ? "dst" : "both");
            else
            {
                 *g_pu8   = paTests[iTest].uDstIn;
                 *g_pfEfl = paTests[iTest].fEflIn;
                 g_aBinU8[iFn].pfn(g_pu8, paTests[iTest].uSrcIn, g_pfEfl);
                 RTTEST_CHECK(g_hTest, *g_pu8   == paTests[iTest].uDstOut);
                 RTTEST_CHECK(g_hTest, *g_pfEfl == paTests[iTest].fEflOut);
            }
        }
    }
}


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
static const BINU16_TEST_T g_aTests_bsf_u16[]        = { {0} };
static const BINU16_TEST_T g_aTests_bsr_u16[]        = { {0} };
static const BINU16_TEST_T g_aTests_imul_two_u16[]   = { {0} };
static const BINU16_TEST_T g_aTests_arpl[]           = { {0} };
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
    ENTRY(bsf_u16),
    ENTRY(bsr_u16),
    ENTRY(imul_two_u16),
    ENTRY(arpl),
};

static void BinU16Generate(uint32_t cTests)
{
    RTPrintf("\n\n#define HAVE_BINU16_TESTS\n");
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aBinU16); iFn++)
    {
        RTPrintf("static const BINU16_TEST_T g_aTests_%s[] =\n{\n", g_aBinU16[iFn].pszName);
        for (uint32_t iTest = 0; iTest < cTests; iTest++ )
        {
            BINU16_TEST_T Test;
            Test.fEflIn    = RandEFlags();
            Test.fEflOut   = Test.fEflIn;
            Test.uDstIn    = RandU16();
            Test.uDstOut   = Test.uDstIn;
            Test.uSrcIn    = RandU16();
            if (g_aBinU16[iFn].uExtra)
                Test.uSrcIn &= 0xf; /* Restrict bit index to a word */
            Test.uMisc     = 0;
            g_aBinU16[iFn].pfn(&Test.uDstOut, Test.uSrcIn, &Test.fEflOut);
            RTPrintf("    { %#08x, %#08x, %#06x, %#06x, %#06x, %#x }, /* #%u */\n",
                     Test.fEflIn, Test.fEflOut, Test.uDstIn, Test.uDstOut, Test.uSrcIn, Test.uMisc, iTest);
        }
        RTPrintf("};\n");
    }
}

static void BinU16Test(void)
{
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aBinU16); iFn++)
    {
        RTTestSub(g_hTest, g_aBinU16[iFn].pszName);

        BINU16_TEST_T const * const paTests = g_aBinU16[iFn].paTests;
        uint32_t const              cTests  = g_aBinU16[iFn].cTests;
        for (uint32_t iTest = 0; iTest < cTests; iTest++ )
        {
            uint32_t fEfl = paTests[iTest].fEflIn;
            uint16_t uDst = paTests[iTest].uDstIn;
            g_aBinU16[iFn].pfn(&uDst, paTests[iTest].uSrcIn, &fEfl);
            if (   uDst != paTests[iTest].uDstOut
                || fEfl != paTests[iTest].fEflOut)
                RTTestFailed(g_hTest, "#%u: efl=%#08x dst=%#06x src=%#06x -> efl=%#08x dst=%#06x, expected %#08x & %#06x%s - %s\n",
                             iTest, paTests[iTest].fEflIn, paTests[iTest].uDstIn, paTests[iTest].uSrcIn,
                             fEfl, uDst, paTests[iTest].fEflOut, paTests[iTest].uDstOut,
                             EFlagsDiff(fEfl, paTests[iTest].fEflOut),
                             uDst == paTests[iTest].uDstOut ? "eflags" : fEfl == paTests[iTest].fEflOut ? "dst" : "both");
            else
            {
                 *g_pu16  = paTests[iTest].uDstIn;
                 *g_pfEfl = paTests[iTest].fEflIn;
                 g_aBinU16[iFn].pfn(g_pu16, paTests[iTest].uSrcIn, g_pfEfl);
                 RTTEST_CHECK(g_hTest, *g_pu16== paTests[iTest].uDstOut);
                 RTTEST_CHECK(g_hTest, *g_pfEfl == paTests[iTest].fEflOut);
            }
        }
    }
}


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
static const BINU32_TEST_T g_aTests_bsf_u32[]        = { {0} };
static const BINU32_TEST_T g_aTests_bsr_u32[]        = { {0} };
static const BINU32_TEST_T g_aTests_imul_two_u32[]   = { {0} };
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
    ENTRY(bsf_u32),
    ENTRY(bsr_u32),
    ENTRY(imul_two_u32),
};

static void BinU32Generate(uint32_t cTests)
{
    RTPrintf("\n\n#define HAVE_BINU32_TESTS\n");
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aBinU32); iFn++)
    {
        RTPrintf("static const BINU32_TEST_T g_aTests_%s[] =\n{\n", g_aBinU32[iFn].pszName);
        for (uint32_t iTest = 0; iTest < cTests; iTest++ )
        {
            BINU32_TEST_T Test;
            Test.fEflIn    = RandEFlags();
            Test.fEflOut   = Test.fEflIn;
            Test.uDstIn    = RandU32();
            Test.uDstOut   = Test.uDstIn;
            Test.uSrcIn    = RandU32();
            if (g_aBinU32[iFn].uExtra)
                Test.uSrcIn &= 0x1f; /* Restrict bit index to a word */
            Test.uMisc     = 0;
            g_aBinU32[iFn].pfn(&Test.uDstOut, Test.uSrcIn, &Test.fEflOut);
            RTPrintf("    { %#08x, %#08x, %#010RX32, %#010RX32, %#010RX32, %#x }, /* #%u */\n",
                     Test.fEflIn, Test.fEflOut, Test.uDstIn, Test.uDstOut, Test.uSrcIn, Test.uMisc, iTest);
        }
        RTPrintf("};\n");
    }
}

static void BinU32Test(void)
{
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aBinU32); iFn++)
    {
        RTTestSub(g_hTest, g_aBinU32[iFn].pszName);

        BINU32_TEST_T const * const paTests = g_aBinU32[iFn].paTests;
        uint32_t const              cTests  = g_aBinU32[iFn].cTests;
        for (uint32_t iTest = 0; iTest < cTests; iTest++ )
        {
            uint32_t fEfl = paTests[iTest].fEflIn;
            uint32_t uDst = paTests[iTest].uDstIn;
            g_aBinU32[iFn].pfn(&uDst, paTests[iTest].uSrcIn, &fEfl);
            if (   uDst != paTests[iTest].uDstOut
                || fEfl != paTests[iTest].fEflOut)
                RTTestFailed(g_hTest, "#%u: efl=%#08x dst=%#010RX32 src=%#010RX32 -> efl=%#08x dst=%#010RX32, expected %#08x & %#010RX32%s - %s\n",
                             iTest, paTests[iTest].fEflIn, paTests[iTest].uDstIn, paTests[iTest].uSrcIn,
                             fEfl, uDst, paTests[iTest].fEflOut, paTests[iTest].uDstOut,
                             EFlagsDiff(fEfl, paTests[iTest].fEflOut),
                             uDst == paTests[iTest].uDstOut ? "eflags" : fEfl == paTests[iTest].fEflOut ? "dst" : "both");
            else
            {
                 *g_pu32  = paTests[iTest].uDstIn;
                 *g_pfEfl = paTests[iTest].fEflIn;
                 g_aBinU32[iFn].pfn(g_pu32, paTests[iTest].uSrcIn, g_pfEfl);
                 RTTEST_CHECK(g_hTest, *g_pu32  == paTests[iTest].uDstOut);
                 RTTEST_CHECK(g_hTest, *g_pfEfl == paTests[iTest].fEflOut);
            }
        }
    }
}


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
static const BINU64_TEST_T g_aTests_bsf_u64[]        = { {0} };
static const BINU64_TEST_T g_aTests_bsr_u64[]        = { {0} };
static const BINU64_TEST_T g_aTests_imul_two_u64[]   = { {0} };
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
    ENTRY(bsf_u64),
    ENTRY(bsr_u64),
    ENTRY(imul_two_u64),
};

static void BinU64Generate(uint32_t cTests)
{
    RTPrintf("\n\n#define HAVE_BINU64_TESTS\n");
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aBinU64); iFn++)
    {
        RTPrintf("static const BINU64_TEST_T g_aTests_%s[] =\n{\n", g_aBinU64[iFn].pszName);
        for (uint32_t iTest = 0; iTest < cTests; iTest++ )
        {
            BINU64_TEST_T Test;
            Test.fEflIn    = RandEFlags();
            Test.fEflOut   = Test.fEflIn;
            Test.uDstIn    = RandU64();
            Test.uDstOut   = Test.uDstIn;
            Test.uSrcIn    = RandU64();
            if (g_aBinU64[iFn].uExtra)
                Test.uSrcIn &= 0x3f; /* Restrict bit index to a word */
            Test.uMisc     = 0;
            g_aBinU64[iFn].pfn(&Test.uDstOut, Test.uSrcIn, &Test.fEflOut);
            RTPrintf("    { %#08x, %#08x, %#018RX64, %#018RX64, %#018RX64, %#x }, /* #%u */\n",
                     Test.fEflIn, Test.fEflOut, Test.uDstIn, Test.uDstOut, Test.uSrcIn, Test.uMisc, iTest);
        }
        RTPrintf("};\n");
    }
}

static void BinU64Test(void)
{
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aBinU64); iFn++)
    {
        RTTestSub(g_hTest, g_aBinU64[iFn].pszName);

        BINU64_TEST_T const * const paTests = g_aBinU64[iFn].paTests;
        uint32_t const              cTests  = g_aBinU64[iFn].cTests;
        for (uint32_t iTest = 0; iTest < cTests; iTest++ )
        {
            uint32_t fEfl = paTests[iTest].fEflIn;
            uint64_t uDst = paTests[iTest].uDstIn;
            g_aBinU64[iFn].pfn(&uDst, paTests[iTest].uSrcIn, &fEfl);
            if (   uDst != paTests[iTest].uDstOut
                || fEfl != paTests[iTest].fEflOut)
                RTTestFailed(g_hTest, "#%u: efl=%#08x dst=%#018RX64 src=%#018RX64 -> efl=%#08x dst=%#018RX64, expected %#08x & %#018RX64%s - %s\n",
                             iTest, paTests[iTest].fEflIn, paTests[iTest].uDstIn, paTests[iTest].uSrcIn,
                             fEfl, uDst, paTests[iTest].fEflOut, paTests[iTest].uDstOut,
                             EFlagsDiff(fEfl, paTests[iTest].fEflOut),
                             uDst == paTests[iTest].uDstOut ? "eflags" : fEfl == paTests[iTest].fEflOut ? "dst" : "both");
            else
            {
                 *g_pu64  = paTests[iTest].uDstIn;
                 *g_pfEfl = paTests[iTest].fEflIn;
                 g_aBinU64[iFn].pfn(g_pu64, paTests[iTest].uSrcIn, g_pfEfl);
                 RTTEST_CHECK(g_hTest, *g_pu64  == paTests[iTest].uDstOut);
                 RTTEST_CHECK(g_hTest, *g_pfEfl == paTests[iTest].fEflOut);
            }
        }
    }
}


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

#ifndef HAVE_SHIFT_DBL_TESTS
static const BINU16_TEST_T g_aTests_shrd_u16[] = { {0} };
static const BINU16_TEST_T g_aTests_shld_u16[] = { {0} };
static const BINU32_TEST_T g_aTests_shrd_u32[] = { {0} };
static const BINU32_TEST_T g_aTests_shld_u32[] = { {0} };
static const BINU64_TEST_T g_aTests_shrd_u64[] = { {0} };
static const BINU64_TEST_T g_aTests_shld_u64[] = { {0} };
#endif

#define TEST_SHIFT_DBL(a_cBits, a_Type, a_Fmt) \
static const struct \
{ \
    const char                       *pszName; \
    PFNIEMAIMPLSHIFTDBLU ## a_cBits   pfn; \
    BINU ## a_cBits ## _TEST_T const *paTests; \
    uint32_t                          cTests, uExtra; \
} g_aShiftDblU ## a_cBits [] = \
{ \
    ENTRY(shld_u ## a_cBits), \
    ENTRY(shrd_u ## a_cBits), \
}; \
\
static void ShiftDblU ## a_cBits ## Generate(uint32_t cTests) \
{ \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aShiftDblU ## a_cBits); iFn++) \
    { \
        RTPrintf("static const BINU" #a_cBits "_TEST_T g_aTests_%s[] =\n{\n", g_aShiftDblU ## a_cBits[iFn].pszName); \
        for (uint32_t iTest = 0; iTest < cTests; iTest++ ) \
        { \
            BINU ## a_cBits ## _TEST_T Test; \
            Test.fEflIn    = RandEFlags(); \
            Test.fEflOut   = Test.fEflIn; \
            Test.uDstIn    = RandU ## a_cBits(); \
            Test.uDstOut   = Test.uDstIn; \
            Test.uSrcIn    = RandU ## a_cBits(); \
            Test.uMisc     = RandU8() & (a_cBits - 1); \
            g_aShiftDblU ## a_cBits[iFn].pfn(&Test.uDstOut, Test.uSrcIn, Test.uMisc, &Test.fEflOut); \
            RTPrintf("    { %#08x, %#08x, " a_Fmt ", " a_Fmt ", " a_Fmt ", %2u }, /* #%u */\n", \
                     Test.fEflIn, Test.fEflOut, Test.uDstIn, Test.uDstOut, Test.uSrcIn, Test.uMisc, iTest); \
        } \
        RTPrintf("};\n"); \
    } \
} \
\
static void ShiftDblU ## a_cBits ## Test(void) \
{ \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aShiftDblU ## a_cBits); iFn++) \
    { \
        RTTestSub(g_hTest, g_aShiftDblU ## a_cBits[iFn].pszName); \
        BINU ## a_cBits ## _TEST_T const * const paTests = g_aShiftDblU ## a_cBits[iFn].paTests; \
        uint32_t const                           cTests  = g_aShiftDblU ## a_cBits[iFn].cTests; \
        for (uint32_t iTest = 0; iTest < cTests; iTest++ ) \
        { \
            uint32_t fEfl = paTests[iTest].fEflIn; \
            a_Type   uDst = paTests[iTest].uDstIn; \
            g_aShiftDblU ## a_cBits[iFn].pfn(&uDst, paTests[iTest].uSrcIn, paTests[iTest].uMisc, &fEfl); \
            if (   uDst != paTests[iTest].uDstOut \
                || (fEfl /*| X86_EFL_AF*/) != (paTests[iTest].fEflOut /*| X86_EFL_AF*/)) \
                RTTestFailed(g_hTest, "#%u: efl=%#08x dst=" a_Fmt " src=" a_Fmt " shift=%-2u -> efl=%#08x dst=" a_Fmt ", expected %#08x & " a_Fmt "%s\n", \
                             iTest, paTests[iTest].fEflIn, paTests[iTest].uDstIn, paTests[iTest].uSrcIn, (unsigned)paTests[iTest].uMisc, \
                             fEfl, uDst, paTests[iTest].fEflOut, paTests[iTest].uDstOut, \
                             EFlagsDiff(fEfl /*| X86_EFL_AF*/, paTests[iTest].fEflOut /*| X86_EFL_AF*/)); \
            else \
            { \
                 *g_pu ## a_cBits  = paTests[iTest].uDstIn; \
                 *g_pfEfl          = paTests[iTest].fEflIn; \
                 g_aShiftDblU ## a_cBits[iFn].pfn(g_pu ## a_cBits, paTests[iTest].uSrcIn, paTests[iTest].uMisc, g_pfEfl); \
                 RTTEST_CHECK(g_hTest, *g_pu ## a_cBits == paTests[iTest].uDstOut); \
                 RTTEST_CHECK(g_hTest, *g_pfEfl == paTests[iTest].fEflOut); \
            } \
        } \
    } \
}
TEST_SHIFT_DBL(16, uint16_t, "%#06RX16")
TEST_SHIFT_DBL(32, uint32_t, "%#010RX32")
TEST_SHIFT_DBL(64, uint64_t, "%#018RX64")

static void ShiftDblGenerate(uint32_t cTests)
{
    RTPrintf("\n\n#define HAVE_SHIFT_DBL_TESTS\n");
    ShiftDblU16Generate(cTests);
    ShiftDblU32Generate(cTests);
    ShiftDblU64Generate(cTests);
}

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
    a_TestType const            *paTests; \
    uint32_t                     cTests, uExtra; \
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
static void UnaryU ## a_cBits ## Generate(uint32_t cTests) \
{ \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aUnaryU ## a_cBits); iFn++) \
    { \
        RTPrintf("static const BINU" #a_cBits "_TEST_T g_aTests_%s[] =\n{\n", g_aUnaryU ## a_cBits[iFn].pszName); \
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
            RTPrintf("    { %#08x, %#08x, " a_Fmt ", " a_Fmt ", 0, 0 }, /* #%u */\n", \
                     Test.fEflIn, Test.fEflOut, Test.uDstIn, Test.uDstOut, iTest); \
        } \
        RTPrintf("};\n"); \
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

static void UnaryGenerate(uint32_t cTests)
{
    RTPrintf("\n\n#define HAVE_UNARY_TESTS\n");
    UnaryU8Generate(cTests);
    UnaryU16Generate(cTests);
    UnaryU32Generate(cTests);
    UnaryU64Generate(cTests);
}

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

#ifndef HAVE_SHIFT_TESTS
# define DUMMY_SHIFT_TESTS(a_cBits, a_Type) \
    static const a_Type g_aTests_rol_u ## a_cBits[] = { {0} }; \
    static const a_Type g_aTests_ror_u ## a_cBits[] = { {0} }; \
    static const a_Type g_aTests_rcl_u ## a_cBits[] = { {0} }; \
    static const a_Type g_aTests_rcr_u ## a_cBits[] = { {0} }; \
    static const a_Type g_aTests_shl_u ## a_cBits[] = { {0} }; \
    static const a_Type g_aTests_shr_u ## a_cBits[] = { {0} }; \
    static const a_Type g_aTests_sar_u ## a_cBits[] = { {0} }
DUMMY_SHIFT_TESTS(8,  BINU8_TEST_T);
DUMMY_SHIFT_TESTS(16, BINU16_TEST_T);
DUMMY_SHIFT_TESTS(32, BINU32_TEST_T);
DUMMY_SHIFT_TESTS(64, BINU64_TEST_T);
#endif

#define TEST_SHIFT(a_cBits, a_Type, a_Fmt, a_TestType) \
static const struct \
{ \
    const char                  *pszName; \
    PFNIEMAIMPLSHIFTU ## a_cBits pfn; \
    a_TestType const            *paTests; \
    uint32_t                     cTests, uExtra; \
} g_aShiftU ## a_cBits [] = \
{ \
    ENTRY(rol_u ## a_cBits), \
    ENTRY(ror_u ## a_cBits), \
    ENTRY(rcl_u ## a_cBits), \
    ENTRY(rcr_u ## a_cBits), \
    ENTRY(shl_u ## a_cBits), \
    ENTRY(shr_u ## a_cBits), \
    ENTRY(sar_u ## a_cBits), \
}; \
\
static void ShiftU ## a_cBits ## Generate(uint32_t cTests) \
{ \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aShiftU ## a_cBits); iFn++) \
    { \
        RTPrintf("static const BINU" #a_cBits "_TEST_T g_aTests_%s[] =\n{\n", g_aShiftU ## a_cBits[iFn].pszName); \
        for (uint32_t iTest = 0; iTest < cTests; iTest++ ) \
        { \
            a_TestType Test; \
            Test.fEflIn    = RandEFlags(); \
            Test.fEflOut   = Test.fEflIn; \
            Test.uDstIn    = RandU ## a_cBits(); \
            Test.uDstOut   = Test.uDstIn; \
            Test.uSrcIn    = 0; \
            Test.uMisc     = RandU8() & (a_cBits - 1); \
            g_aShiftU ## a_cBits[iFn].pfn(&Test.uDstOut, Test.uMisc, &Test.fEflOut); \
            RTPrintf("    { %#08x, %#08x, " a_Fmt ", " a_Fmt ", 0, %-2u }, /* #%u */\n", \
                     Test.fEflIn, Test.fEflOut, Test.uDstIn, Test.uDstOut, Test.uMisc, iTest); \
        } \
        RTPrintf("};\n"); \
    } \
} \
\
static void ShiftU ## a_cBits ## Test(void) \
{ \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aShiftU ## a_cBits); iFn++) \
    { \
        RTTestSub(g_hTest, g_aShiftU ## a_cBits[iFn].pszName); \
        a_TestType const * const paTests = g_aShiftU ## a_cBits[iFn].paTests; \
        uint32_t const           cTests  = g_aShiftU ## a_cBits[iFn].cTests; \
        for (uint32_t iTest = 0; iTest < cTests; iTest++ ) \
        { \
            uint32_t fEfl = paTests[iTest].fEflIn; \
            a_Type   uDst = paTests[iTest].uDstIn; \
            g_aShiftU ## a_cBits[iFn].pfn(&uDst, paTests[iTest].uMisc, &fEfl); \
            if (   uDst != paTests[iTest].uDstOut \
                || fEfl != paTests[iTest].fEflOut) \
                RTTestFailed(g_hTest, "#%u: efl=%#08x dst=" a_Fmt " shift=%2u -> efl=%#08x dst=" a_Fmt ", expected %#08x & " a_Fmt "%s\n", \
                             iTest, paTests[iTest].fEflIn, paTests[iTest].uDstIn, paTests[iTest].uMisc, \
                             fEfl, uDst, paTests[iTest].fEflOut, paTests[iTest].uDstOut, \
                             EFlagsDiff(fEfl, paTests[iTest].fEflOut)); \
            else \
            { \
                 *g_pu ## a_cBits  = paTests[iTest].uDstIn; \
                 *g_pfEfl          = paTests[iTest].fEflIn; \
                 g_aShiftU ## a_cBits[iFn].pfn(g_pu ## a_cBits, paTests[iTest].uMisc, g_pfEfl); \
                 RTTEST_CHECK(g_hTest, *g_pu ## a_cBits == paTests[iTest].uDstOut); \
                 RTTEST_CHECK(g_hTest, *g_pfEfl == paTests[iTest].fEflOut); \
            } \
        } \
    } \
}
TEST_SHIFT(8,  uint8_t,  "%#04RX8",   BINU8_TEST_T)
TEST_SHIFT(16, uint16_t, "%#06RX16",  BINU16_TEST_T)
TEST_SHIFT(32, uint32_t, "%#010RX32", BINU32_TEST_T)
TEST_SHIFT(64, uint64_t, "%#018RX64", BINU64_TEST_T)

static void ShiftGenerate(uint32_t cTests)
{
    RTPrintf("\n\n#define HAVE_SHIFT_TESTS\n");
    ShiftU8Generate(cTests);
    ShiftU16Generate(cTests);
    ShiftU32Generate(cTests);
    ShiftU64Generate(cTests);
}

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

#ifndef HAVE_MULDIV_TESTS
# define DUMMY_MULDIV_TESTS(a_cBits, a_Type) \
    static const a_Type g_aTests_mul_u ## a_cBits[]  = { {0} }; \
    static const a_Type g_aTests_imul_u ## a_cBits[] = { {0} }; \
    static const a_Type g_aTests_div_u ## a_cBits[]  = { {0} }; \
    static const a_Type g_aTests_idiv_u ## a_cBits[] = { {0} }
DUMMY_MULDIV_TESTS(8,  MULDIVU8_TEST_T);
DUMMY_MULDIV_TESTS(16, MULDIVU16_TEST_T);
DUMMY_MULDIV_TESTS(32, MULDIVU32_TEST_T);
DUMMY_MULDIV_TESTS(64, MULDIVU64_TEST_T);
#endif

/* U8 */
static const struct
{
    const char                     *pszName;
    PFNIEMAIMPLMULDIVU8             pfn;
    MULDIVU8_TEST_T const          *paTests;
    uint32_t                        cTests, uExtra;
} g_aMulDivU8[] =
{
    ENTRY_EX(mul_u8,  X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF),
    ENTRY_EX(imul_u8, X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF),
    ENTRY_EX(div_u8,  X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF | X86_EFL_CF | X86_EFL_OF),
    ENTRY_EX(idiv_u8, X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF | X86_EFL_CF | X86_EFL_OF),
};

static void MulDivU8Generate(uint32_t cTests)
{
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aMulDivU8); iFn++)
    {
        RTPrintf("static const MULDIVU8_TEST_T g_aTests_%s[] =\n{\n", g_aMulDivU8[iFn].pszName);
        for (uint32_t iTest = 0; iTest < cTests; iTest++ )
        {
            MULDIVU8_TEST_T Test;
            Test.fEflIn    = RandEFlags();
            Test.fEflOut   = Test.fEflIn;
            Test.uDstIn    = RandU16();
            Test.uDstOut   = Test.uDstIn;
            Test.uSrcIn    = RandU8();
            Test.rc        = g_aMulDivU8[iFn].pfn(&Test.uDstOut, Test.uSrcIn, &Test.fEflOut);
            RTPrintf("    { %#08x, %#08x, %#06RX16, %#06RX16, %#04RX8, %d }, /* #%u */\n",
                     Test.fEflIn, Test.fEflOut, Test.uDstIn, Test.uDstOut, Test.uSrcIn, Test.rc, iTest);
        }
        RTPrintf("};\n");
    }
}

static void MulDivU8Test(void)
{
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aMulDivU8); iFn++)
    {
        RTTestSub(g_hTest, g_aMulDivU8[iFn].pszName);
        MULDIVU8_TEST_T const * const paTests = g_aMulDivU8[iFn].paTests;
        uint32_t const                cTests  = g_aMulDivU8[iFn].cTests;
        uint32_t const                fEflIgn = g_aMulDivU8[iFn].uExtra;
        for (uint32_t iTest = 0; iTest < cTests; iTest++ )
        {
            uint32_t fEfl  = paTests[iTest].fEflIn;
            uint16_t uDst  = paTests[iTest].uDstIn;
            int      rc    = g_aMulDivU8[iFn].pfn(&uDst, paTests[iTest].uSrcIn, &fEfl);
            if (   uDst != paTests[iTest].uDstOut
                || (fEfl | fEflIgn) != (paTests[iTest].fEflOut | fEflIgn)
                || rc != paTests[iTest].rc)
                RTTestFailed(g_hTest, "#%02u: efl=%#08x dst=%#06RX16 src=%#04RX8\n"
                                      "  -> efl=%#08x dst=%#06RX16 rc=%d\n"
                                      "expected %#08x     %#06RX16    %d%s\n",
                             iTest, paTests[iTest].fEflIn, paTests[iTest].uDstIn, paTests[iTest].uSrcIn,
                             fEfl, uDst, rc, paTests[iTest].fEflOut, paTests[iTest].uDstOut, paTests[iTest].rc,
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
    }
}

#define TEST_MULDIV(a_cBits, a_Type, a_Fmt, a_TestType) \
static const struct \
{ \
    const char                     *pszName; \
    PFNIEMAIMPLMULDIVU ## a_cBits   pfn; \
    a_TestType const               *paTests; \
    uint32_t                        cTests, uExtra; \
} g_aMulDivU ## a_cBits [] = \
{ \
    ENTRY_EX(mul_u ## a_cBits,  X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF), \
    ENTRY_EX(imul_u ## a_cBits, X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF), \
    ENTRY_EX(div_u ## a_cBits,  X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF | X86_EFL_CF | X86_EFL_OF), \
    ENTRY_EX(idiv_u ## a_cBits, X86_EFL_SF | X86_EFL_ZF | X86_EFL_AF | X86_EFL_PF | X86_EFL_CF | X86_EFL_OF), \
}; \
\
static void MulDivU ## a_cBits ## Generate(uint32_t cTests) \
{ \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aMulDivU ## a_cBits); iFn++) \
    { \
        RTPrintf("static const MULDIVU" #a_cBits "_TEST_T g_aTests_%s[] =\n{\n", g_aMulDivU ## a_cBits[iFn].pszName); \
        for (uint32_t iTest = 0; iTest < cTests; iTest++ ) \
        { \
            a_TestType Test; \
            Test.fEflIn    = RandEFlags(); \
            Test.fEflOut   = Test.fEflIn; \
            Test.uDst1In   = RandU ## a_cBits(); \
            Test.uDst1Out  = Test.uDst1In; \
            Test.uDst2In   = RandU ## a_cBits(); \
            Test.uDst2Out  = Test.uDst2In; \
            Test.uSrcIn    = RandU ## a_cBits(); \
            Test.rc        = g_aMulDivU ## a_cBits[iFn].pfn(&Test.uDst1Out, &Test.uDst2Out, Test.uSrcIn, &Test.fEflOut); \
            RTPrintf("    { %#08x, %#08x, " a_Fmt ", " a_Fmt ", " a_Fmt ", " a_Fmt ", " a_Fmt ", %d }, /* #%u */\n", \
                     Test.fEflIn, Test.fEflOut, Test.uDst1In, Test.uDst1Out, Test.uDst2In, Test.uDst2Out, Test.uSrcIn, \
                     Test.rc, iTest); \
        } \
        RTPrintf("};\n"); \
    } \
} \
\
static void MulDivU ## a_cBits ## Test(void) \
{ \
    for (size_t iFn = 0; iFn < RT_ELEMENTS(g_aMulDivU ## a_cBits); iFn++) \
    { \
        RTTestSub(g_hTest, g_aMulDivU ## a_cBits[iFn].pszName); \
        a_TestType const * const paTests = g_aMulDivU ## a_cBits[iFn].paTests; \
        uint32_t const           cTests  = g_aMulDivU ## a_cBits[iFn].cTests; \
        uint32_t const           fEflIgn = g_aMulDivU ## a_cBits[iFn].uExtra; \
        for (uint32_t iTest = 0; iTest < cTests; iTest++ ) \
        { \
            uint32_t fEfl  = paTests[iTest].fEflIn; \
            a_Type   uDst1 = paTests[iTest].uDst1In; \
            a_Type   uDst2 = paTests[iTest].uDst2In; \
            int rc = g_aMulDivU ## a_cBits[iFn].pfn(&uDst1, &uDst2, paTests[iTest].uSrcIn, &fEfl); \
            if (   uDst1 != paTests[iTest].uDst1Out \
                || uDst2 != paTests[iTest].uDst2Out \
                || (fEfl | fEflIgn) != (paTests[iTest].fEflOut | fEflIgn)\
                || rc    != paTests[iTest].rc) \
                RTTestFailed(g_hTest, "#%02u: efl=%#08x dst1=" a_Fmt " dst2=" a_Fmt " src=" a_Fmt "\n" \
                                       "  -> efl=%#08x dst1=" a_Fmt  " dst2=" a_Fmt " rc=%d\n" \
                                       "expected %#08x      " a_Fmt  "      " a_Fmt "    %d%s -%s%s%s\n", \
                             iTest, paTests[iTest].fEflIn, paTests[iTest].uDst1In, paTests[iTest].uDst2In, paTests[iTest].uSrcIn, \
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
                 rc  = g_aMulDivU ## a_cBits[iFn].pfn(g_pu ## a_cBits, g_pu ## a_cBits ## Two, paTests[iTest].uSrcIn, g_pfEfl); \
                 RTTEST_CHECK(g_hTest, *g_pu ## a_cBits        == paTests[iTest].uDst1Out); \
                 RTTEST_CHECK(g_hTest, *g_pu ## a_cBits ## Two == paTests[iTest].uDst2Out); \
                 RTTEST_CHECK(g_hTest, (*g_pfEfl | fEflIgn)    == (paTests[iTest].fEflOut | fEflIgn)); \
                 RTTEST_CHECK(g_hTest, rc                      == paTests[iTest].rc); \
            } \
        } \
    } \
}
TEST_MULDIV(16, uint16_t, "%#06RX16",  MULDIVU16_TEST_T)
TEST_MULDIV(32, uint32_t, "%#010RX32", MULDIVU32_TEST_T)
TEST_MULDIV(64, uint64_t, "%#018RX64", MULDIVU64_TEST_T)

static void MulDivGenerate(uint32_t cTests)
{
    RTPrintf("\n\n#define HAVE_MULDIV_TESTS\n");
    MulDivU8Generate(cTests);
    MulDivU16Generate(cTests);
    MulDivU32Generate(cTests);
    MulDivU64Generate(cTests);
}

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


int main(int argc, char **argv)
{
    int rc = RTR3InitExe(argc, &argv, 0);
    if (RT_FAILURE(rc))
        return RTMsgInitFailure(rc);

    /*
     * Generate data?
     */
    if (argc > 2)
    {
        char szCpuDesc[256] = {0};
        RTMpGetDescription(NIL_RTCPUID, szCpuDesc, sizeof(szCpuDesc));

        RTPrintf("/* $Id$ */\n"
                 "/** @file\n"
                 " * IEM Assembly Instruction Helper Testcase Data - %s.\n"
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
                 , szCpuDesc);
        uint32_t cTests = 64;
        BinU8Generate(cTests);
        BinU16Generate(cTests);
        BinU32Generate(cTests);
        BinU64Generate(cTests);
        ShiftDblGenerate(cTests);
        UnaryGenerate(cTests);
        ShiftGenerate(cTests);
        MulDivGenerate(cTests);
        return RTEXITCODE_SUCCESS;
    }

    /*
     * Do testing.  Currrently disabled by default as data needs to be checked
     * on both intel and AMD systems first.
     */
    rc = RTTestCreate("tstIEMAimpl", &g_hTest);
    AssertRCReturn(rc, RTEXITCODE_FAILURE);
    if (argc > 1)
    {
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
        return RTTestSummaryAndDestroy(g_hTest);
    }
    return RTTestSkipAndDestroy(g_hTest, "unfinished testcase");
}