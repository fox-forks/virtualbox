/** @file
 * Shared Clipboard - Common X11 code.
 */

/*
 * Copyright (C) 2006-2019 Oracle Corporation
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

#ifndef VBOX_INCLUDED_GuestHost_SharedClipboard_x11_h
#define VBOX_INCLUDED_GuestHost_SharedClipboard_x11_h
#ifndef RT_WITHOUT_PRAGMA_ONCE
# pragma once
#endif

#include <X11/Intrinsic.h>

#include <iprt/thread.h>

#include <VBox/GuestHost/SharedClipboard.h>

/**
 * Enumeration for all clipboard formats which we support on X11.
 */
typedef enum _SHCLX11FMT
{
    SHCLX11FMT_INVALID = 0,
    SHCLX11FMT_TARGETS,
    SHCLX11FMT_TEXT,  /* Treat this as UTF-8, but it may really be ascii */
    SHCLX11FMT_UTF8,
    SHCLX11FMT_BMP,
    SHCLX11FMT_HTML
#ifdef VBOX_WITH_SHARED_CLIPBOARD_TRANSFERS
    , SHCLX11FMT_URI_LIST
#endif
} SHCLX11FMT;

/** Defines an index of the X11 clipboad format table. */
typedef unsigned SHCLX11FMTIDX;

/**
 * Structure for maintaining a Shared Clipboard context on X11 platforms.
 */
typedef struct _SHCLX11CTX
{
    /** Opaque data structure describing the front-end. */
    PSHCLCONTEXT pFrontend;
    /** Is an X server actually available? */
    bool fHaveX11;
    /** The X Toolkit application context structure */
    XtAppContext appContext;

    /** We have a separate thread to wait for window and clipboard events. */
    RTTHREAD Thread;
    /** The X Toolkit widget which we use as our clipboard client.  It is never made visible. */
    Widget pWidget;

    /** Should we try to grab the clipboard on startup? */
    bool fGrabClipboardOnStart;

    /** The best text format X11 has to offer, as an index into the formats table. */
    SHCLX11FMTIDX X11TextFormat;
    /** The best bitmap format X11 has to offer, as an index into the formats table. */
    SHCLX11FMTIDX X11BitmapFormat;
    /** The best HTML format X11 has to offer, as an index into the formats table. */
    SHCLX11FMTIDX X11HTMLFormat;
#ifdef VBOX_WITH_SHARED_CLIPBOARD_TRANSFERS
    /** The best HTML format X11 has to offer, as an index into the formats table. */
    SHCLX11FMTIDX X11URIListFormat;
#endif
    /** What kind of formats does VBox have to offer? */
    SHCLFORMATS vboxFormats;
    /** Cache of the last unicode data that we received. */
    void *pvUnicodeCache;
    /** Size of the unicode data in the cache. */
    uint32_t cbUnicodeCache;
    /** When we wish the clipboard to exit, we have to wake up the event
     * loop.  We do this by writing into a pipe.  This end of the pipe is
     * the end that another thread can write to. */
    int wakeupPipeWrite;
    /** The reader end of the pipe. */
    int wakeupPipeRead;
    /** A pointer to the XFixesSelectSelectionInput function. */
    void (*fixesSelectInput)(Display *, Window, Atom, unsigned long);
    /** The first XFixes event number. */
    int fixesEventBase;
    /** XtGetSelectionValue on some versions of libXt isn't re-entrant
     * so block overlapping requests on this flag. */
    bool fXtBusy;
    /** If a request is blocked on the previous flag, set this flag to request
     * an update later - the first callback should check and clear this flag
     * before processing the callback event. */
    bool fXtNeedsUpdate;
} SHCLX11CTX, *PSHCLX11CTX;

/** @name Shared Clipboard APIs for X11.
 *  @{
 */
int ShClX11Init(PSHCLX11CTX pCtx, PSHCLCONTEXT pParent, bool fHeadless);
void ShClX11Destroy(PSHCLX11CTX pCtx);
int ShClX11ThreadStart(PSHCLX11CTX pCtx, bool grab);
int ShClX11ThreadStop(PSHCLX11CTX pCtx);
int ShClX11ReportFormatsToX11(PSHCLX11CTX pCtx, SHCLFORMATS vboxFormats);
int ShClX11ReadDataFromX11(PSHCLX11CTX pCtx, SHCLFORMATS vboxFormat, CLIPREADCBREQ *pReq);
/** @} */

/** @name Shared Clipboard callbacks exported by the X11 APIs.
 *  @{
 */
DECLCALLBACK(int)  ShClX11RequestDataForX11Callback(SHCLCONTEXT *pCtx, SHCLFORMAT Format, void **ppv, uint32_t *pcb);
DECLCALLBACK(void) ShClX11ReportFormatsCallback(SHCLCONTEXT *pCtx, SHCLFORMATS Formats);
DECLCALLBACK(void) ShClX11RequestFromX11CompleteCallback(SHCLCONTEXT *pCtx, int rc, CLIPREADCBREQ *pReq, void *pv, uint32_t cb);
/** @} */

#endif /* !VBOX_INCLUDED_GuestHost_SharedClipboard_x11_h */

