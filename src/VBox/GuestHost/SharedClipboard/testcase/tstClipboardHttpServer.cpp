/* $Id$ */
/** @file
 * Shared Clipboard HTTP server test case.
 */

/*
 * Copyright (C) 2023 Oracle and/or its affiliates.
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
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <iprt/assert.h>
#include <iprt/dir.h>
#include <iprt/file.h>
#include <iprt/getopt.h>
#include <iprt/http.h>
#include <iprt/message.h>
#include <iprt/path.h>
#include <iprt/rand.h>
#include <iprt/string.h>
#include <iprt/test.h>

#include <VBox/GuestHost/SharedClipboard-transfers.h>


/** The release logger. */
static PRTLOGGER g_pRelLogger;
/** The current logging verbosity level. */
static unsigned  g_uVerbosity = 0;
/** Maximum HTTP server runtime (in ms). */
static RTMSINTERVAL g_msRuntime     = RT_MS_30SEC;
/** Shutdown indicator. */
static bool         g_fShutdown     = false;
/** Manual mode indicator; allows manual testing w/ other HTTP clients. */
static bool         g_fManual       = false;


/* Worker thread for the HTTP server. */
static DECLCALLBACK(int) tstSrvWorker(RTTHREAD hThread, void *pvUser)
{
    RT_NOREF(pvUser);

    int rc = RTThreadUserSignal(hThread);
    AssertRCReturn(rc, rc);

    uint32_t const msStartTS = RTTimeMilliTS();
    while (RTTimeMilliTS() - msStartTS < g_msRuntime)
    {
        if (g_fShutdown)
            break;
        RTThreadSleep(100); /* Wait a little. */
    }

    return RTTimeMilliTS() - msStartTS <= g_msRuntime ? VINF_SUCCESS : VERR_TIMEOUT;
}

static void tstCreateTransferSingle(RTTEST hTest, PSHCLTRANSFERCTX pTransferCtx, PSHCLHTTPSERVER pSrv,
                                    const char *pszFile, PSHCLTXPROVIDER pProvider)
{
    PSHCLTRANSFER pTx;
    RTTEST_CHECK_RC_OK(hTest, ShClTransferCreate(SHCLTRANSFERDIR_TO_REMOTE, SHCLSOURCE_LOCAL, NULL /* Callbacks */, &pTx));
    RTTEST_CHECK_RC_OK(hTest, ShClTransferSetProvider(pTx, pProvider));
    RTTEST_CHECK_RC_OK(hTest, ShClTransferInit(pTx));
    RTTEST_CHECK_RC_OK(hTest, ShClTransferRootsInitFromFile(pTx, pszFile));
    RTTEST_CHECK_RC_OK(hTest, ShClTransferCtxRegister(pTransferCtx, pTx, NULL));
    RTTEST_CHECK_RC_OK(hTest, ShClTransferHttpServerRegisterTransfer(pSrv, pTx));
}

int main(int argc, char *argv[])
{
    /*
     * Init the runtime, test and say hello.
     */
    RTTEST hTest;
    RTEXITCODE rcExit = RTTestInitAndCreate("tstClipboardHttpServer", &hTest);
    if (rcExit != RTEXITCODE_SUCCESS)
        return rcExit;
    RTTestBanner(hTest);

    /*
     * Process options.
     */
    static const RTGETOPTDEF aOpts[] =
    {
        { "--port",             'p',               RTGETOPT_REQ_UINT16 },
        { "--manual",           'm',               RTGETOPT_REQ_NOTHING },
        { "--max-time",         't',               RTGETOPT_REQ_UINT32 },
        { "--verbose",          'v',               RTGETOPT_REQ_NOTHING }
    };

    RTGETOPTSTATE GetState;
    int rc = RTGetOptInit(&GetState, argc, argv, aOpts, RT_ELEMENTS(aOpts), 1 /*idxFirst*/, 0 /*fFlags - must not sort! */);
    AssertRCReturn(rc, RTEXITCODE_INIT);

    uint16_t     uPort = 0;

    int           ch;
    RTGETOPTUNION ValueUnion;
    while ((ch = RTGetOpt(&GetState, &ValueUnion)) != 0)
    {
        switch (ch)
        {
            case 'p':
                uPort = ValueUnion.u16;
                break;

            case 't':
                g_msRuntime = ValueUnion.u32 * RT_MS_1SEC; /* Convert s to ms. */
                break;

            case 'm':
                g_fManual = true;
                break;

            case 'v':
                g_uVerbosity++;
                break;

            case VINF_GETOPT_NOT_OPTION:
                continue;

            default:
                return RTGetOptPrintError(ch, &ValueUnion);
        }
    }

    /*
     * Configure release logging to go to stdout.
     */
    RTUINT fFlags = RTLOGFLAGS_PREFIX_THREAD | RTLOGFLAGS_PREFIX_TIME_PROG;
#if defined(RT_OS_WINDOWS) || defined(RT_OS_OS2)
    fFlags |= RTLOGFLAGS_USECRLF;
#endif
    static const char * const s_apszLogGroups[] = VBOX_LOGGROUP_NAMES;
    rc = RTLogCreate(&g_pRelLogger, fFlags, "all.e.l", "TST_CLIPBOARDR_HTTPSERVER_RELEASE_LOG",
                     RT_ELEMENTS(s_apszLogGroups), s_apszLogGroups, RTLOGDEST_STDOUT, NULL /*"vkat-release.log"*/);
    if (RT_SUCCESS(rc))
    {
        RTLogSetDefaultInstance(g_pRelLogger);
        if (g_uVerbosity)
        {
            RTMsgInfo("Setting verbosity logging to level %u\n", g_uVerbosity);
            switch (g_uVerbosity) /* Not very elegant, but has to do it for now. */
            {
                case 1:
                    rc = RTLogGroupSettings(g_pRelLogger, "shared_clipboard.e.l+http.e.l");
                    break;

                case 2:
                    rc = RTLogGroupSettings(g_pRelLogger, "shared_clipboard.e.l.l2+http.e.l.l2");
                    break;

                case 3:
                    rc = RTLogGroupSettings(g_pRelLogger, "shared_clipboard.e.l.l2.l3+http.e.l.l2.l3");
                    break;

                case 4:
                    RT_FALL_THROUGH();
                default:
                    rc = RTLogGroupSettings(g_pRelLogger, "shared_clipboard.e.l.l2.l3.l4.f+http.e.l.l2.l3.l4.f");
                    break;
            }
            if (RT_FAILURE(rc))
                RTMsgError("Setting debug logging failed, rc=%Rrc\n", rc);
        }
    }
    else
        RTMsgWarning("Failed to create release logger: %Rrc", rc);

    /*
     * Create HTTP server.
     */
    SHCLHTTPSERVER HttpSrv;
    ShClTransferHttpServerInit(&HttpSrv);
    ShClTransferHttpServerStop(&HttpSrv); /* Try to stop a non-running server twice. */
    ShClTransferHttpServerStop(&HttpSrv);
    RTTEST_CHECK(hTest, ShClTransferHttpServerIsRunning(&HttpSrv) == false);
    if (uPort)
        rc = ShClTransferHttpServerStartEx(&HttpSrv, uPort);
    else
        rc = ShClTransferHttpServerStart(&HttpSrv, 32 /* cMaxAttempts */, &uPort);
    RTTEST_CHECK_RC_OK(hTest, rc);
    RTTEST_CHECK(hTest, ShClTransferHttpServerGetTransfer(&HttpSrv, 0) == false);
    RTTEST_CHECK(hTest, ShClTransferHttpServerGetTransfer(&HttpSrv, 42) == false);

    char *pszSrvAddr = ShClTransferHttpServerGetAddressA(&HttpSrv);
    RTTEST_CHECK(hTest, pszSrvAddr != NULL);
    RTTestPrintf(hTest, RTTESTLVL_ALWAYS, "HTTP server running: %s (for %RU32ms) ...\n", pszSrvAddr, g_msRuntime);
    RTMemFree(pszSrvAddr);
    pszSrvAddr = NULL;

    SHCLTRANSFERCTX TxCtx;
    RTTEST_CHECK_RC_OK(hTest, ShClTransferCtxInit(&TxCtx));

    /* Query the local transfer provider. */
    SHCLTXPROVIDER Provider;
    RTTESTI_CHECK(ShClTransferProviderLocalQueryInterface(&Provider) != NULL);

    /* Parse options again, but this time we only fetch all files we want to serve.
     * Only can be done after we initialized the HTTP server above. */
    RT_ZERO(GetState);
    rc = RTGetOptInit(&GetState, argc, argv, aOpts, RT_ELEMENTS(aOpts), 1 /*idxFirst*/, 0 /*fFlags - must not sort! */);
    AssertRCReturn(rc, RTEXITCODE_INIT);
    while ((ch = RTGetOpt(&GetState, &ValueUnion)) != 0)
    {
        switch (ch)
        {
            case VINF_GETOPT_NOT_OPTION:
            {
                tstCreateTransferSingle(hTest, &TxCtx, &HttpSrv, ValueUnion.psz, &Provider);
                break;
            }

            default:
                continue;
        }
    }

    char szRandomTestFile[RTPATH_MAX] = { 0 };

    uint32_t const cTx = ShClTransferCtxGetTotalTransfers(&TxCtx);
    if (!cTx)
    {
        RTTEST_CHECK_RC_OK(hTest, RTPathTemp(szRandomTestFile, sizeof(szRandomTestFile)));
        RTTEST_CHECK_RC_OK(hTest, RTPathAppend(szRandomTestFile, sizeof(szRandomTestFile), "tstClipboardHttpServer-XXXXXX"));
        RTTEST_CHECK_RC_OK(hTest, RTFileCreateTemp(szRandomTestFile, 0600));

        size_t cbExist = RTRandU32Ex(0, _256M);

        RTTestPrintf(hTest, RTTESTLVL_ALWAYS, "Random test file (%zu bytes): %s\n", cbExist, szRandomTestFile);

        RTFILE hFile;
        rc = RTFileOpen(&hFile, szRandomTestFile, RTFILE_O_WRITE | RTFILE_O_CREATE_REPLACE | RTFILE_O_DENY_NONE);
        if (RT_SUCCESS(rc))
        {
            uint8_t abBuf[_64K] = { 42 };

            while (cbExist > 0)
            {
                size_t cbToWrite = sizeof(abBuf);
                if (cbToWrite > cbExist)
                    cbToWrite = cbExist;
                rc = RTFileWrite(hFile, abBuf, cbToWrite, NULL);
                if (RT_FAILURE(rc))
                {
                    RTTestIFailed("RTFileWrite(%#x) -> %Rrc\n", cbToWrite, rc);
                    break;
                }
                cbExist -= cbToWrite;
            }

            RTTESTI_CHECK_RC(RTFileClose(hFile), VINF_SUCCESS);

            if (RT_SUCCESS(rc))
            {
                tstCreateTransferSingle(hTest, &TxCtx, &HttpSrv, szRandomTestFile, &Provider);
            }
        }
        else
            RTTestIFailed("RTFileOpen(%s) -> %Rrc\n", szRandomTestFile, rc);
    }

    if (RTTestErrorCount(hTest))
        return RTTestSummaryAndDestroy(hTest);

    /* Create  thread for our HTTP server. */
    RTTHREAD hThread;
    rc = RTThreadCreate(&hThread, tstSrvWorker, NULL, 0, RTTHREADTYPE_DEFAULT, RTTHREADFLAGS_WAITABLE,
                        "tstClpHttpSrv");
    RTTEST_CHECK_RC_OK(hTest, rc);
    if (RT_SUCCESS(rc))
    {
        rc = RTThreadUserWait(hThread, RT_MS_30SEC);
        RTTEST_CHECK_RC_OK(hTest, rc);
    }

    if (RT_SUCCESS(rc))
    {
        if (g_fManual)
        {
            for (uint32_t i = 0; i < cTx; i++)
            {
                PSHCLTRANSFER pTx = ShClTransferCtxGetTransferByIndex(&TxCtx, i);

                uint16_t const uID    = ShClTransferGetID(pTx);
                char          *pszURL = ShClTransferHttpServerGetUrlA(&HttpSrv, uID, 0 /* Entry index */);
                RTTEST_CHECK(hTest, pszURL != NULL);
                RTTestPrintf(hTest, RTTESTLVL_ALWAYS, "URL #%02RU32: %s\n", i, pszURL);
                RTStrFree(pszURL);
            }
        }
        else /* Download all files to a temp file using our HTTP client. */
        {
            RTHTTP hClient;
            rc = RTHttpCreate(&hClient);
            if (RT_SUCCESS(rc))
            {
                char szFileTemp[RTPATH_MAX];
                RTTEST_CHECK_RC_OK(hTest, RTPathTemp(szFileTemp, sizeof(szFileTemp)));
                RTTEST_CHECK_RC_OK(hTest, RTPathAppend(szFileTemp, sizeof(szFileTemp), "tstClipboardHttpServer-XXXXXX"));
                RTTEST_CHECK_RC_OK(hTest, RTFileCreateTemp(szFileTemp, 0600));

                for (unsigned a = 0; a < 3; a++) /* Repeat downloads to stress things. */
                {
                    for (uint32_t i = 0; i < ShClTransferCtxGetTotalTransfers(&TxCtx); i++)
                    {
                        PSHCLTRANSFER pTx = ShClTransferCtxGetTransferByIndex(&TxCtx, i);

                        uint16_t const uID    = ShClTransferGetID(pTx);
                        char          *pszURL = ShClTransferHttpServerGetUrlA(&HttpSrv, uID, 0 /* Entry index */);
                        RTTEST_CHECK(hTest, pszURL != NULL);
                        RTTestPrintf(hTest, RTTESTLVL_ALWAYS, "Downloading: %s -> %s\n", pszURL, szFileTemp);
                        RTTEST_CHECK_RC_BREAK(hTest, RTHttpGetFile(hClient, pszURL, szFileTemp), VINF_SUCCESS);
                        RTStrFree(pszURL);
                    }
                }

                RTTEST_CHECK_RC_OK(hTest, RTFileDelete(szFileTemp));
                RTTEST_CHECK_RC_OK(hTest, RTHttpDestroy(hClient));
            }

            /* This is supposed to run unattended, so shutdown automatically. */
            ASMAtomicXchgBool(&g_fShutdown, true); /* Set shutdown indicator. */
        }
    }

    int rcThread;
    RTTEST_CHECK_RC_OK(hTest, RTThreadWait(hThread, g_msRuntime, &rcThread));
    RTTEST_CHECK_RC_OK(hTest, rcThread);

    RTTEST_CHECK_RC_OK(hTest, ShClTransferHttpServerDestroy(&HttpSrv));
    ShClTransferCtxDestroy(&TxCtx);

    if (strlen(szRandomTestFile))
        RTTEST_CHECK_RC_OK(hTest, RTFileDelete(szRandomTestFile));

    /*
     * Summary
     */
    return RTTestSummaryAndDestroy(hTest);
}

