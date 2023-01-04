/* $Id$ */
/** @file
 * VBox Qt GUI - UIMachineViewFullscreen class implementation.
 */

/*
 * Copyright (C) 2010-2022 Oracle and/or its affiliates.
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

/* Qt includes: */
#include <QApplication>
#include <QMainWindow>
#include <QTimer>
#ifdef VBOX_WS_MAC
# include <QMenuBar>
#endif /* VBOX_WS_MAC */

/* GUI includes: */
#include "UISession.h"
#include "UIActionPoolRuntime.h"
#include "UIMachineLogicFullscreen.h"
#include "UIMachineWindow.h"
#include "UIMachineViewFullscreen.h"
#include "UIFrameBuffer.h"
#include "UIExtraDataManager.h"
#include "UIDesktopWidgetWatchdog.h"

/* Other VBox includes: */
#include "VBox/log.h"

/* External includes: */
#ifdef VBOX_WS_X11
# include <limits.h>
#endif /* VBOX_WS_X11 */


UIMachineViewFullscreen::UIMachineViewFullscreen(UIMachineWindow *pMachineWindow, ulong uScreenId)
    : UIMachineView(pMachineWindow, uScreenId)
    , m_fGuestAutoresizeEnabled(actionPool()->action(UIActionIndexRT_M_View_T_GuestAutoresize)->isChecked())
{
}

void UIMachineViewFullscreen::sltAdditionsStateChanged()
{
    adjustGuestScreenSize();
}

bool UIMachineViewFullscreen::eventFilter(QObject *pWatched, QEvent *pEvent)
{
    if (pWatched != 0 && pWatched == machineWindow())
    {
        switch (pEvent->type())
        {
            case QEvent::Resize:
            {
                /* Send guest-resize hint only if top window resizing to required dimension: */
                QResizeEvent *pResizeEvent = static_cast<QResizeEvent*>(pEvent);
                if (pResizeEvent->size() != calculateMaxGuestSize())
                    break;

                /* Recalculate maximum guest size: */
                setMaximumGuestSize();

                break;
            }
            default:
                break;
        }
    }

    return UIMachineView::eventFilter(pWatched, pEvent);
}

void UIMachineViewFullscreen::prepareCommon()
{
    /* Base class common settings: */
    UIMachineView::prepareCommon();

    /* Setup size-policy: */
    setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
    /* Maximum size to sizehint: */
    setMaximumSize(sizeHint());
    /* Minimum size is ignored: */
    setMinimumSize(0, 0);
    /* No scrollbars: */
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void UIMachineViewFullscreen::prepareFilters()
{
    /* Base class filters: */
    UIMachineView::prepareFilters();
}

void UIMachineViewFullscreen::prepareConsoleConnections()
{
    /* Base class connections: */
    UIMachineView::prepareConsoleConnections();

    /* Guest additions state-change updater: */
    connect(uisession(), &UISession::sigAdditionsStateActualChange, this, &UIMachineViewFullscreen::sltAdditionsStateChanged);
}

void UIMachineViewFullscreen::setGuestAutoresizeEnabled(bool fEnabled)
{
    if (m_fGuestAutoresizeEnabled != fEnabled)
    {
        m_fGuestAutoresizeEnabled = fEnabled;

        if (m_fGuestAutoresizeEnabled && uisession()->isGuestSupportsGraphics())
            sltPerformGuestResize();
    }
}

void UIMachineViewFullscreen::adjustGuestScreenSize()
{
    /* Should we adjust guest-screen size? Logging paranoia is required here to reveal the truth. */
    LogRel(("GUI: UIMachineViewFullscreen::adjustGuestScreenSize: Adjust guest-screen size if necessary.\n"));
    bool fAdjust = false;

    /* Step 1: Was the guest-screen enabled automatically? */
    if (!fAdjust)
    {
        if (frameBuffer()->isAutoEnabled())
        {
            LogRel2(("GUI: UIMachineViewFullscreen::adjustGuestScreenSize: Guest-screen was enabled automatically, adjustment is required.\n"));
            fAdjust = true;
        }
    }
    /* Step 2: Is the guest-screen of another size than necessary? */
    if (!fAdjust)
    {
        /* Acquire requested guest-screen size-hint or at least actual frame-buffer size: */
        QSize guestScreenSizeHint = requestedGuestScreenSizeHint();
        /* Take the scale-factor(s) into account: */
        guestScreenSizeHint = scaledForward(guestScreenSizeHint);

        /* Calculate maximum possible guest screen size: */
        const QSize maximumGuestScreenSize = calculateMaxGuestSize();

        if (guestScreenSizeHint != maximumGuestScreenSize)
        {
            LogRel2(("GUI: UIMachineViewFullscreen::adjustGuestScreenSize: Guest-screen is of another size than necessary, adjustment is required.\n"));
            fAdjust = true;
        }
    }

    /* Step 3: Is guest-additions supports graphics? */
    if (fAdjust)
    {
        if (!uisession()->isGuestSupportsGraphics())
        {
            LogRel2(("GUI: UIMachineViewFullscreen::adjustGuestScreenSize: Guest-additions are not supporting graphics, adjustment is omitted.\n"));
            fAdjust = false;
        }
    }
    /* Step 4: Is guest-screen visible? */
    if (fAdjust)
    {
        if (!uisession()->isScreenVisible(screenId()))
        {
            LogRel2(("GUI: UIMachineViewFullscreen::adjustGuestScreenSize: Guest-screen is not visible, adjustment is omitted.\n"));
            fAdjust = false;
        }
    }
    /* Step 5: Is guest-screen auto-resize enabled? */
    if (fAdjust)
    {
        if (!m_fGuestAutoresizeEnabled)
        {
            LogRel2(("GUI: UIMachineViewFullscreen::adjustGuestScreenSize: Guest-screen auto-resize is disabled, adjustment is omitted.\n"));
            fAdjust = false;
        }
    }

    /* Final step: Adjust if requested/allowed. */
    if (fAdjust)
    {
        frameBuffer()->setAutoEnabled(false);
        sltPerformGuestResize(calculateMaxGuestSize());
        /* And remember the size to know what we are resizing out of when we exit: */
        uisession()->setLastFullScreenSize(screenId(), scaledForward(scaledBackward(calculateMaxGuestSize())));
    }
}

QRect UIMachineViewFullscreen::workingArea() const
{
    /* Get corresponding screen: */
    int iScreen = static_cast<UIMachineLogicFullscreen*>(machineLogic())->hostScreenForGuestScreen(screenId());
    /* Return available geometry for that screen: */
    return gpDesktop->screenGeometry(iScreen);
}

QSize UIMachineViewFullscreen::calculateMaxGuestSize() const
{
    return workingArea().size();
}
