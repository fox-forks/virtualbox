/* $Id$ */
/** @file
 * Audio testing routines.
 * Common code which is being used by the ValidationKit and the debug / ValdikationKit audio driver(s).
 */

/*
 * Copyright (C) 2021 Oracle Corporation
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

#include <package-generated.h>
#include "product-generated.h"

#include <iprt/buildconfig.h>
#include <iprt/dir.h>
#include <iprt/file.h>
#include <iprt/inifile.h>
#include <iprt/list.h>
#include <iprt/rand.h>
#include <iprt/system.h>
#include <iprt/uuid.h>
#include <iprt/vfs.h>
#include <iprt/zip.h>

#define _USE_MATH_DEFINES
#include <math.h> /* sin, M_PI */

#include <VBox/version.h>
#include <VBox/vmm/pdmaudioifs.h>
#include <VBox/vmm/pdmaudioinline.h>

#include "AudioTest.h"


/*********************************************************************************************************************************
*   Defines                                                                                                                      *
*********************************************************************************************************************************/
/** The test manifest file name. */
#define AUDIOTEST_MANIFEST_FILE_STR "vkat_manifest.ini"
/** The current test manifest version. */
#define AUDIOTEST_MANIFEST_VER      1

/** Test manifest header name. */
#define AUDIOTEST_INI_SEC_HDR_STR   "header"


/*********************************************************************************************************************************
*   Structures and Typedefs                                                                                                      *
*********************************************************************************************************************************/


/*********************************************************************************************************************************
*   Global Variables                                                                                                             *
*********************************************************************************************************************************/
/** Well-known frequency selection test tones. */
static const double s_aAudioTestToneFreqsHz[] =
{
     349.2282 /*F4*/,
     440.0000 /*A4*/,
     523.2511 /*C5*/,
     698.4565 /*F5*/,
     880.0000 /*A5*/,
    1046.502  /*C6*/,
    1174.659  /*D6*/,
    1396.913  /*F6*/,
    1760.0000 /*A6*/
};

/**
 * Initializes a test tone by picking a random but well-known frequency (in Hz).
 *
 * @returns Randomly picked frequency (in Hz).
 * @param   pTone               Pointer to test tone to initialize.
 * @param   pProps              PCM properties to use for the test tone.
 */
double AudioTestToneInitRandom(PAUDIOTESTTONE pTone, PPDMAUDIOPCMPROPS pProps)
{
    /* Pick a frequency from our selection, so that every time a recording starts
     * we'll hopfully generate a different note. */
    pTone->rdFreqHz = s_aAudioTestToneFreqsHz[RTRandU32Ex(0, RT_ELEMENTS(s_aAudioTestToneFreqsHz) - 1)];
    pTone->rdFixed  = 2.0 * M_PI * pTone->rdFreqHz / PDMAudioPropsHz(pProps);
    pTone->uSample  = 0;

    memcpy(&pTone->Props, pProps, sizeof(PDMAUDIOPCMPROPS));

    pTone->enmType = AUDIOTESTTONETYPE_SINE; /* Only type implemented so far. */

    return pTone->rdFreqHz;
}

/**
 * Writes (and iterates) a given test tone to an output buffer.
 *
 * @returns VBox status code.
 * @param   pTone               Pointer to test tone to write.
 * @param   pvBuf               Pointer to output buffer to write test tone to.
 * @param   cbBuf               Size (in bytes) of output buffer.
 * @param   pcbWritten          How many bytes were written on success.
 */
int AudioTestToneGenerate(PAUDIOTESTTONE pTone, void *pvBuf, uint32_t cbBuf, uint32_t *pcbWritten)
{
    /*
     * Clear the buffer first so we don't need to think about additional channels.
     */
    uint32_t cFrames   = PDMAudioPropsBytesToFrames(&pTone->Props, cbBuf);

    /* Input cbBuf not necessarily is aligned to the frames, so re-calculate it. */
    const uint32_t cbToWrite = PDMAudioPropsFramesToBytes(&pTone->Props, cFrames);

    PDMAudioPropsClearBuffer(&pTone->Props, pvBuf, cbBuf, cFrames);

    /*
     * Generate the select sin wave in the first channel:
     */
    uint32_t const cbFrame   = PDMAudioPropsFrameSize(&pTone->Props);
    double const   rdFixed   = pTone->rdFixed;
    uint64_t       iSrcFrame = pTone->uSample;
    switch (PDMAudioPropsSampleSize(&pTone->Props))
    {
        case 1:
            /* untested */
            if (PDMAudioPropsIsSigned(&pTone->Props))
            {
                int8_t *piSample = (int8_t *)pvBuf;
                while (cFrames-- > 0)
                {
                    *piSample = (int8_t)(126 /*Amplitude*/ * sin(rdFixed * iSrcFrame));
                    iSrcFrame++;
                    piSample += cbFrame;
                }
            }
            else
            {
                /* untested */
                uint8_t *pbSample = (uint8_t *)pvBuf;
                while (cFrames-- > 0)
                {
                    *pbSample = (uint8_t)(126 /*Amplitude*/ * sin(rdFixed * iSrcFrame) + 0x80);
                    iSrcFrame++;
                    pbSample += cbFrame;
                }
            }
            break;

        case 2:
            if (PDMAudioPropsIsSigned(&pTone->Props))
            {
                int16_t *piSample = (int16_t *)pvBuf;
                while (cFrames-- > 0)
                {
                    *piSample = (int16_t)(32760 /*Amplitude*/ * sin(rdFixed * iSrcFrame));
                    iSrcFrame++;
                    piSample = (int16_t *)((uint8_t *)piSample + cbFrame);
                }
            }
            else
            {
                /* untested */
                uint16_t *puSample = (uint16_t *)pvBuf;
                while (cFrames-- > 0)
                {
                    *puSample = (uint16_t)(32760 /*Amplitude*/ * sin(rdFixed * iSrcFrame) + 0x8000);
                    iSrcFrame++;
                    puSample = (uint16_t *)((uint8_t *)puSample + cbFrame);
                }
            }
            break;

        case 4:
            /* untested */
            if (PDMAudioPropsIsSigned(&pTone->Props))
            {
                int32_t *piSample = (int32_t *)pvBuf;
                while (cFrames-- > 0)
                {
                    *piSample = (int32_t)((32760 << 16) /*Amplitude*/ * sin(rdFixed * iSrcFrame));
                    iSrcFrame++;
                    piSample = (int32_t *)((uint8_t *)piSample + cbFrame);
                }
            }
            else
            {
                uint32_t *puSample = (uint32_t *)pvBuf;
                while (cFrames-- > 0)
                {
                    *puSample = (uint32_t)((32760 << 16) /*Amplitude*/ * sin(rdFixed * iSrcFrame) + UINT32_C(0x80000000));
                    iSrcFrame++;
                    puSample = (uint32_t *)((uint8_t *)puSample + cbFrame);
                }
            }
            break;

        default:
            AssertFailedReturn(VERR_NOT_SUPPORTED);
    }

    pTone->uSample = iSrcFrame;

    if (pcbWritten)
        *pcbWritten = cbToWrite;

    return VINF_SUCCESS;
}

/**
 * Initializes an audio test tone parameters struct with random values.
 * @param   pToneParams         Test tone parameters to initialize.
 * @param   pProps              PCM properties to use for the test tone.
 */
int AudioTestToneParamsInitRandom(PAUDIOTESTTONEPARMS pToneParams, PPDMAUDIOPCMPROPS pProps)
{
    AssertReturn(PDMAudioPropsAreValid(pProps), VERR_INVALID_PARAMETER);

    memcpy(&pToneParams->Props, pProps, sizeof(PDMAUDIOPCMPROPS));

    /** @todo Make this a bit more sophisticated later, e.g. muting and prequel/sequel are not very balanced. */

    pToneParams->msPrequel      = RTRandU32Ex(0, RT_MS_5SEC);
#ifdef DEBUG_andy
    pToneParams->msDuration     = RTRandU32Ex(0, RT_MS_1SEC);
#else
    pToneParams->msDuration     = RTRandU32Ex(0, RT_MS_10SEC); /** @todo Probably a bit too long, but let's see. */
#endif
    pToneParams->msSequel       = RTRandU32Ex(0, RT_MS_5SEC);
    pToneParams->uVolumePercent = RTRandU32Ex(0, 100);

    return VINF_SUCCESS;
}

/**
 * Creates a new path (directory) for a specific audio test set tag.
 *
 * @returns VBox status code.
 * @param   pszPath             On input, specifies the absolute base path where to create the test set path.
 *                              On output this specifies the absolute path created.
 * @param   cbPath              Size (in bytes) of \a pszPath.
 * @param   pszTag              Tag to use for path creation.
 *
 * @note    Can be used multiple times with the same tag; a sub directory with an ISO time string will be used
 *          on each call.
 */
int AudioTestPathCreate(char *pszPath, size_t cbPath, const char *pszTag)
{
    AssertReturn(strlen(pszTag) <= AUDIOTEST_TAG_MAX, VERR_INVALID_PARAMETER);

    int rc;

    char szTag[RTUUID_STR_LENGTH + 1];
    if (pszTag)
    {
        rc = RTStrCopy(szTag, sizeof(szTag), pszTag);
        AssertRCReturn(rc, rc);
    }
    else /* Create an UUID if no tag is specified. */
    {
        RTUUID UUID;
        rc = RTUuidCreate(&UUID);
        AssertRCReturn(rc, rc);
        rc = RTUuidToStr(&UUID, szTag, sizeof(szTag));
        AssertRCReturn(rc, rc);
    }

    char szName[RT_ELEMENTS(AUDIOTEST_PATH_PREFIX_STR) + AUDIOTEST_TAG_MAX + 4];
    if (RTStrPrintf2(szName, sizeof(szName), "%s-%s", AUDIOTEST_PATH_PREFIX_STR, szTag) < 0)
        AssertFailedReturn(VERR_BUFFER_OVERFLOW);

    rc = RTPathAppend(pszPath, cbPath, szName);
    AssertRCReturn(rc, rc);

    char szTime[64];
    RTTIMESPEC time;
    if (!RTTimeSpecToString(RTTimeNow(&time), szTime, sizeof(szTime)))
        return VERR_BUFFER_UNDERFLOW;

    rc = RTPathAppend(pszPath, cbPath, szTime);
    AssertRCReturn(rc, rc);

    return RTDirCreateFullPath(pszPath, RTFS_UNIX_IRWXU);
}

/**
 * Writes string data to a test set manifest.
 *
 * @returns VBox status code.
 * @param   pSet                Test set to write manifest for.
 * @param   pszFormat           Format string to write.
 * @param   args                Variable arguments for \a pszFormat.
 */
static int audioTestManifestWriteV(PAUDIOTESTSET pSet, const char *pszFormat, va_list args)
{
    char *psz = NULL;
    RTStrAPrintfV(&psz, pszFormat, args);
    AssertPtr(psz);

    /** @todo Use RTIniFileWrite once its implemented. */
    int rc = RTFileWrite(pSet->f.hFile, psz, strlen(psz), NULL);
    AssertRC(rc);

    RTStrFree(psz);

    return rc;
}

/**
 * Writes a terminated string line to a test set manifest.
 * Convenience function.
 *
 * @returns VBox status code.
 * @param   pSet                Test set to write manifest for.
 * @param   pszFormat           Format string to write.
 * @param   ...                 Variable arguments for \a pszFormat. Optional.
 */
static int audioTestManifestWriteLn(PAUDIOTESTSET pSet, const char *pszFormat, ...)
{
    va_list va;
    va_start(va, pszFormat);

    int rc = audioTestManifestWriteV(pSet, pszFormat, va);
    AssertRC(rc);

    va_end(va);

    /** @todo Keep it as simple as possible for now. Improve this later. */
    rc = RTFileWrite(pSet->f.hFile, "\n", strlen("\n"), NULL);
    AssertRC(rc);

    return rc;
}

/**
 * Writes a section entry to a test set manifest.
 *
 * @returns VBox status code.
 * @param   pSet                Test set to write manifest for.
 * @param   pszSection          Format string of section to write.
 * @param   ...                 Variable arguments for \a pszSection. Optional.
 */
static int audioTestManifestWriteSection(PAUDIOTESTSET pSet, const char *pszSection, ...)
{
    va_list va;
    va_start(va, pszSection);

    /** @todo Keep it as simple as possible for now. Improve this later. */
    int rc = RTFileWrite(pSet->f.hFile, "[", strlen("["), NULL);
    AssertRC(rc);

    rc = audioTestManifestWriteV(pSet, pszSection, va);
    AssertRC(rc);

    rc = RTFileWrite(pSet->f.hFile, "]\n", strlen("]\n"), NULL);
    AssertRC(rc);

    va_end(va);

    return rc;
}

/**
 * Initializes an audio test set, internal function.
 * @param   pSet                Test set to initialize.
 */
static void audioTestSetInitInternal(PAUDIOTESTSET pSet)
{
    pSet->f.hFile = NIL_RTFILE;

    RTListInit(&pSet->lstObj);
}

/**
 * Returns whether a test set's manifest file is open (and thus ready) or not.
 *
 * @returns \c true if open (and ready), or \c false if not.
 * @retval  VERR_
 * @param   pSet                Test set to return open status for.
 */
static bool audioTestManifestIsOpen(PAUDIOTESTSET pSet)
{
    if (   pSet->enmMode == AUDIOTESTSETMODE_TEST
        && pSet->f.hFile != NIL_RTFILE)
        return true;
    else if (   pSet->enmMode    == AUDIOTESTSETMODE_VERIFY
             && pSet->f.hIniFile != NIL_RTINIFILE)
        return true;

    return false;
}

/**
 * Initializes an audio test error description.
 *
 * @param   pErr                Test error description to initialize.
 */
static void audioTestErrorDescInit(PAUDIOTESTERRORDESC pErr)
{
    RTListInit(&pErr->List);
    pErr->cErrors = 0;
}

/**
 * Destroys an audio test error description.
 *
 * @param   pErr                Test error description to destroy.
 */
void AudioTestErrorDescDestroy(PAUDIOTESTERRORDESC pErr)
{
    if (!pErr)
        return;

    PAUDIOTESTERRORENTRY pErrEntry, pErrEntryNext;
    RTListForEachSafe(&pErr->List, pErrEntry, pErrEntryNext, AUDIOTESTERRORENTRY, Node)
    {
        RTListNodeRemove(&pErrEntry->Node);

        RTMemFree(pErrEntry);

        Assert(pErr->cErrors);
        pErr->cErrors--;
    }

    Assert(pErr->cErrors == 0);
}

/**
 * Returns if an audio test error description contains any errors or not.
 *
 * @returns \c true if it contains errors, or \c false if not.
 *
 * @param   pErr                Test error description to return error status for.
 */
bool AudioTestErrorDescFailed(PAUDIOTESTERRORDESC pErr)
{
    if (pErr->cErrors)
    {
        Assert(!RTListIsEmpty(&pErr->List));
        return true;
    }

    return false;
}

/**
 * Adds a single error entry to an audio test error description, va_list version.
 *
 * @returns VBox status code.
 * @param   pErr                Test error description to add entry for.
 * @param   rc                  Result code of entry to add.
 * @param   pszDesc             Error description format string to add.
 * @param   args                Optional format arguments of \a pszDesc to add.
 */
static int audioTestErrorDescAddV(PAUDIOTESTERRORDESC pErr, int rc, const char *pszDesc, va_list args)
{
    PAUDIOTESTERRORENTRY pEntry = (PAUDIOTESTERRORENTRY)RTMemAlloc(sizeof(AUDIOTESTERRORENTRY));
    AssertReturn(pEntry, VERR_NO_MEMORY);

    if (RTStrPrintf2V(pEntry->szDesc, sizeof(pEntry->szDesc), pszDesc, args) < 0)
        AssertFailedReturn(VERR_BUFFER_OVERFLOW);

    pEntry->rc = rc;

    RTListAppend(&pErr->List, &pEntry->Node);

    pErr->cErrors++;

    return VINF_SUCCESS;
}

/**
 * Adds a single error entry to an audio test error description, va_list version.
 *
 * @returns VBox status code.
 * @param   pErr                Test error description to add entry for.
 * @param   pszDesc             Error description format string to add.
 * @param   ...                 Optional format arguments of \a pszDesc to add.
 */
static int audioTestErrorDescAdd(PAUDIOTESTERRORDESC pErr, const char *pszDesc, ...)
{
    va_list va;
    va_start(va, pszDesc);

    int rc = audioTestErrorDescAddV(pErr, VERR_GENERAL_FAILURE /** @todo Fudge! */, pszDesc, va);

    va_end(va);
    return rc;
}

#if 0
static int audioTestErrorDescAddRc(PAUDIOTESTERRORDESC pErr, int rc, const char *pszFormat, ...)
{
    va_list va;
    va_start(va, pszFormat);

    int rc2 = audioTestErrorDescAddV(pErr, rc, pszFormat, va);

    va_end(va);
    return rc2;
}
#endif

/**
 * Creates a new temporary directory with a specific (test) tag.
 *
 * @returns VBox status code.
 * @param   pszPath             Where to return the absolute path of the created directory on success.
 * @param   cbPath              Size (in bytes) of \a pszPath.
 * @param   pszTag              Tag name to use for directory creation.
 *
 * @note    Can be used multiple times with the same tag; a sub directory with an ISO time string will be used
 *          on each call.
 */
int AudioTestPathCreateTemp(char *pszPath, size_t cbPath, const char *pszTag)
{
    AssertReturn(strlen(pszTag) <= AUDIOTEST_TAG_MAX, VERR_INVALID_PARAMETER);

    char szPath[RTPATH_MAX];

    int rc = RTPathTemp(szPath, sizeof(szPath));
    AssertRCReturn(rc, rc);
    rc = AudioTestPathCreate(szPath, sizeof(szPath), pszTag);
    AssertRCReturn(rc, rc);

    return RTStrCopy(pszPath, cbPath, szPath);
}

/**
 * Creates a new audio test set.
 *
 * @returns VBox status code.
 * @param   pSet                Test set to create.
 * @param   pszPath             Absolute path to use for the test set's temporary directory.
 *                              If NULL, the OS' temporary directory will be used.
 * @param   pszTag              Tag name to use for this test set.
 */
int AudioTestSetCreate(PAUDIOTESTSET pSet, const char *pszPath, const char *pszTag)
{
    AssertReturn(strlen(pszTag) <= AUDIOTEST_TAG_MAX, VERR_INVALID_PARAMETER);

    int rc;

    audioTestSetInitInternal(pSet);

    if (pszPath)
    {
        rc = RTStrCopy(pSet->szPathAbs, sizeof(pSet->szPathAbs), pszPath);
        AssertRCReturn(rc, rc);

        rc = AudioTestPathCreate(pSet->szPathAbs, sizeof(pSet->szPathAbs), pszTag);
    }
    else
        rc = AudioTestPathCreateTemp(pSet->szPathAbs, sizeof(pSet->szPathAbs), pszTag);
    AssertRCReturn(rc, rc);

    if (RT_SUCCESS(rc))
    {
        char szManifest[RTPATH_MAX];
        rc = RTStrCopy(szManifest, sizeof(szManifest), pSet->szPathAbs);
        AssertRCReturn(rc, rc);

        rc = RTPathAppend(szManifest, sizeof(szManifest), AUDIOTEST_MANIFEST_FILE_STR);
        AssertRCReturn(rc, rc);

        rc = RTFileOpen(&pSet->f.hFile, szManifest,
                        RTFILE_O_CREATE | RTFILE_O_WRITE | RTFILE_O_DENY_WRITE);
        AssertRCReturn(rc, rc);

        rc = audioTestManifestWriteSection(pSet, "header");
        AssertRCReturn(rc, rc);

        rc = audioTestManifestWriteLn(pSet, "magic=vkat_ini"); /* VKAT Manifest, .INI-style. */
        AssertRCReturn(rc, rc);
        rc = audioTestManifestWriteLn(pSet, "ver=%d", AUDIOTEST_MANIFEST_VER);
        AssertRCReturn(rc, rc);
        rc = audioTestManifestWriteLn(pSet, "tag=%s", pszTag);
        AssertRCReturn(rc, rc);

        char szVal[64];
        RTTIMESPEC time;
        if (!RTTimeSpecToString(RTTimeNow(&time), szVal, sizeof(szVal)))
            AssertFailedReturn(VERR_BUFFER_OVERFLOW);

        rc = audioTestManifestWriteLn(pSet, "date_created=%s", szVal);
        AssertRCReturn(rc, rc);

        rc = RTSystemQueryOSInfo(RTSYSOSINFO_PRODUCT, szVal, sizeof(szVal));
        AssertRCReturn(rc, rc);
        rc = audioTestManifestWriteLn(pSet, "os_product=%s", szVal);
        AssertRCReturn(rc, rc);
        rc = RTSystemQueryOSInfo(RTSYSOSINFO_RELEASE, szVal, sizeof(szVal));
        AssertRCReturn(rc, rc);
        rc = audioTestManifestWriteLn(pSet, "os_rel=%s", szVal);
        AssertRCReturn(rc, rc);
        rc = RTSystemQueryOSInfo(RTSYSOSINFO_VERSION, szVal, sizeof(szVal));
        AssertRCReturn(rc, rc);
        rc = audioTestManifestWriteLn(pSet, "os_ver=%s", szVal);
        AssertRCReturn(rc, rc);

        rc = audioTestManifestWriteLn(pSet, "vbox_ver=%s r%u %s (%s %s)",
                                      VBOX_VERSION_STRING, RTBldCfgRevision(),
                                      RTBldCfgTargetDotArch(), __DATE__, __TIME__);
        AssertRCReturn(rc, rc);

        pSet->enmMode = AUDIOTESTSETMODE_TEST;

        rc = RTStrCopy(pSet->szTag, sizeof(pSet->szTag), pszTag);
        AssertRCReturn(rc, rc);
    }

    return rc;
}

/**
 * Destroys a test set.
 *
 * @returns VBox status code.
 * @param   pSet                Test set to destroy.
 */
int AudioTestSetDestroy(PAUDIOTESTSET pSet)
{
    if (!pSet)
        return VINF_SUCCESS;

    int rc = VINF_SUCCESS;

    PAUDIOTESTOBJ pObj, pObjNext;
    RTListForEachSafe(&pSet->lstObj, pObj, pObjNext, AUDIOTESTOBJ, Node)
    {
        rc = AudioTestSetObjClose(pObj);
        if (RT_SUCCESS(rc))
        {
            RTListNodeRemove(&pObj->Node);
            RTMemFree(pObj);

            Assert(pSet->cObj);
            pSet->cObj--;
        }
        else
            break;
    }

    if (RT_FAILURE(rc))
        return rc;

    Assert(pSet->cObj == 0);

    if (RTFileIsValid(pSet->f.hFile))
    {
        RTFileClose(pSet->f.hFile);
        pSet->f.hFile = NIL_RTFILE;
    }

    return rc;
}

/**
 * Opens an existing audio test set.
 *
 * @returns VBox status code.
 * @param   pSet                Test set to open.
 * @param   pszPath             Absolute path of the test set to open.
 */
int AudioTestSetOpen(PAUDIOTESTSET pSet, const char *pszPath)
{
    audioTestSetInitInternal(pSet);

    char szManifest[RTPATH_MAX];
    int rc = RTStrCopy(szManifest, sizeof(szManifest), pszPath);
    AssertRCReturn(rc, rc);

    rc = RTPathAppend(szManifest, sizeof(szManifest), AUDIOTEST_MANIFEST_FILE_STR);
    AssertRCReturn(rc, rc);

    RTVFSFILE hVfsFile;
    rc = RTVfsFileOpenNormal(szManifest, RTFILE_O_OPEN | RTFILE_O_READ | RTFILE_O_DENY_WRITE, &hVfsFile);
    if (RT_FAILURE(rc))
        return rc;

    rc = RTIniFileCreateFromVfsFile(&pSet->f.hIniFile, hVfsFile, RTINIFILE_F_READONLY);
    RTVfsFileRelease(hVfsFile);
    AssertRCReturn(rc, rc);

    pSet->enmMode = AUDIOTESTSETMODE_VERIFY;

    return rc;
}

/**
 * Closes an opened audio test set.
 *
 * @param   pSet                Test set to close.
 */
void AudioTestSetClose(PAUDIOTESTSET pSet)
{
    AudioTestSetDestroy(pSet);
}

/**
 * Physically wipes all related test set files off the disk.
 *
 * @param   pSet                Test set to wipe.
 */
void AudioTestSetWipe(PAUDIOTESTSET pSet)
{
    RT_NOREF(pSet);
}

/**
 * Creates and registers a new audio test object to a test set.
 *
 * @returns VBox status code.
 * @param   pSet                Test set to create and register new object for.
 * @param   pszName             Name of new object to create.
 * @param   ppObj               Where to return the pointer to the newly created object on success.
 */
int AudioTestSetObjCreateAndRegister(PAUDIOTESTSET pSet, const char *pszName, PAUDIOTESTOBJ *ppObj)
{
    AssertPtrReturn(pszName, VERR_INVALID_POINTER);

    PAUDIOTESTOBJ pObj = (PAUDIOTESTOBJ)RTMemAlloc(sizeof(AUDIOTESTOBJ));
    AssertPtrReturn(pObj, VERR_NO_MEMORY);

    int rc = RTStrPrintf(pObj->szName, sizeof(pObj->szName), "%04RU32-%s", pSet->cObj, pszName);
    AssertRCReturn(rc, rc);

    /** @todo Generalize this function more once we have more object types. */

    char szFilePath[RTPATH_MAX];
    rc = RTPathJoin(szFilePath, sizeof(szFilePath), pSet->szPathAbs, pObj->szName);
    AssertRCReturn(rc, rc);

    rc = RTFileOpen(&pObj->File.hFile, szFilePath, RTFILE_O_CREATE | RTFILE_O_WRITE | RTFILE_O_DENY_WRITE);
    if (RT_SUCCESS(rc))
    {
        pObj->enmType = AUDIOTESTOBJTYPE_FILE;

        RTListAppend(&pSet->lstObj, &pObj->Node);
        pSet->cObj++;

        *ppObj = pObj;
    }

    if (RT_FAILURE(rc))
        RTMemFree(pObj);

    return rc;
}

/**
 * Writes to a created audio test object.
 *
 * @returns VBox status code.
 * @param   pObj                Audio test object to write to.
 */
int AudioTestSetObjWrite(PAUDIOTESTOBJ pObj, void *pvBuf, size_t cbBuf)
{
    /** @todo Generalize this function more once we have more object types. */
    AssertReturn(pObj->enmType == AUDIOTESTOBJTYPE_FILE, VERR_INVALID_PARAMETER);

    return RTFileWrite(pObj->File.hFile, pvBuf, cbBuf, NULL);
}

/**
 * Closes an opened audio test object.
 *
 * @returns VBox status code.
 * @param   pObj                Audio test object to close.
 */
int AudioTestSetObjClose(PAUDIOTESTOBJ pObj)
{
    if (!pObj)
        return VINF_SUCCESS;

    /** @todo Generalize this function more once we have more object types. */
    AssertReturn(pObj->enmType == AUDIOTESTOBJTYPE_FILE, VERR_INVALID_PARAMETER);

    int rc = VINF_SUCCESS;

    if (RTFileIsValid(pObj->File.hFile))
    {
        rc = RTFileClose(pObj->File.hFile);
        pObj->File.hFile = NIL_RTFILE;
    }

    return rc;
}

/**
 * Packs an audio test so that it's ready for transmission.
 *
 * @returns VBox status code.
 * @param   pSet                Test set to pack.
 * @param   pszOutDir           Directory where to store the packed test set.
 * @param   pszFileName         Where to return the final name of the packed test set. Optional and can be NULL.
 * @param   cbFileName          Size (in bytes) of \a pszFileName.
 */
int AudioTestSetPack(PAUDIOTESTSET pSet, const char *pszOutDir, char *pszFileName, size_t cbFileName)
{
    AssertReturn(!pszFileName || cbFileName, VERR_INVALID_PARAMETER);
    AssertReturn(audioTestManifestIsOpen(pSet), VERR_WRONG_ORDER);

    /** @todo Check and deny if \a pszOutDir is part of the set's path. */

    char szOutName[RT_ELEMENTS(AUDIOTEST_PATH_PREFIX_STR) + AUDIOTEST_TAG_MAX + 16];
    int rc = RTStrPrintf(szOutName, sizeof(szOutName), "%s-%s.tar.gz", AUDIOTEST_PATH_PREFIX_STR, pSet->szTag);
    AssertRCReturn(rc, rc);

    char szOutPath[RTPATH_MAX];
    rc = RTPathJoin(szOutPath, sizeof(szOutPath), pszOutDir, szOutName);
    AssertRCReturn(rc, rc);

    const char *apszArgs[10];
    unsigned    cArgs = 0;

    apszArgs[cArgs++] = "AudioTest";
    apszArgs[cArgs++] = "--create";
    apszArgs[cArgs++] = "--gzip";
    apszArgs[cArgs++] = "--directory";
    apszArgs[cArgs++] = pSet->szPathAbs;
    apszArgs[cArgs++] = "--file";
    apszArgs[cArgs++] = szOutPath;
    apszArgs[cArgs++] = ".";

    RTEXITCODE rcExit = RTZipTarCmd(cArgs, (char **)apszArgs);
    if (rcExit != RTEXITCODE_SUCCESS)
        rc = VERR_GENERAL_FAILURE; /** @todo Fudge! */

    if (RT_SUCCESS(rc))
    {
        if (pszFileName)
            rc = RTStrCopy(pszFileName, cbFileName, szOutPath);
    }

    return rc;
}

/**
 * Unpacks a formerly packed audio test set.
 *
 * @returns VBox status code.
 * @param   pszFile             Test set file to unpack.
 * @param   pszOutDir           Directory where to unpack the test set into.
 *                              If the directory does not exist it will be created.
 */
int AudioTestSetUnpack(const char *pszFile, const char *pszOutDir)
{
    RT_NOREF(pszFile, pszOutDir);

    // RTZipTarCmd()

    return VERR_NOT_IMPLEMENTED;
}

/**
 * Verifies an opened audio test set.
 *
 * @returns VBox status code.
 * @param   pSet                Test set to verify.
 * @param   pszTag              Tag to use for verification purpose.
 * @param   pErrDesc            Where to return the test verification errors.
 *
 * @note    Test verification errors have to be checked for errors, regardless of the
 *          actual return code.
 */
int AudioTestSetVerify(PAUDIOTESTSET pSet, const char *pszTag, PAUDIOTESTERRORDESC pErrDesc)
{
    AssertReturn(audioTestManifestIsOpen(pSet), VERR_WRONG_ORDER);

    /* We ASSUME the caller has not init'd pErrDesc. */
    audioTestErrorDescInit(pErrDesc);

    char szVal[_1K]; /** @todo Enough, too much? */

    int rc2 = RTIniFileQueryValue(pSet->f.hIniFile, AUDIOTEST_INI_SEC_HDR_STR, "tag", szVal, sizeof(szVal), NULL);
    if (   RT_FAILURE(rc2)
        || RTStrICmp(pszTag, szVal))
        audioTestErrorDescAdd(pErrDesc, "Tag '%s' does not match with manifest's tag '%s'", pszTag, szVal);

    /* Only return critical stuff not related to actual testing here. */
    return VINF_SUCCESS;
}

