/* $Id$ */
/** @file
 * VBox Qt GUI - UISession class declaration.
 */

/*
 * Copyright (C) 2010-2023 Oracle and/or its affiliates.
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

#ifndef FEQT_INCLUDED_SRC_runtime_UISession_h
#define FEQT_INCLUDED_SRC_runtime_UISession_h
#ifndef RT_WITHOUT_PRAGMA_ONCE
# pragma once
#endif

/* Qt includes: */
#include <QObject>
#include <QCursor>
#include <QEvent>
#include <QMap>

/* GUI includes: */
#include "UIExtraDataDefs.h"
#include "UIMediumDefs.h"
#include "UIMousePointerShapeData.h"

/* COM includes: */
#include "COMEnums.h"
#include "CSession.h"
#include "CMachine.h"
#include "CConsole.h"
#include "CDisplay.h"
#include "CGuest.h"
#include "CMouse.h"
#include "CKeyboard.h"
#include "CMachineDebugger.h"
#include "CMedium.h"

/* Forward declarations: */
class QMenu;
class UIConsoleEventHandler;
class UIFrameBuffer;
class UIMachine;
class UIMachineLogic;
class UIMachineWindow;
class UIActionPool;
class CUSBDevice;
class CNetworkAdapter;
class CMediumAttachment;
#ifdef VBOX_WS_MAC
class QMenuBar;
#else /* !VBOX_WS_MAC */
class QIcon;
#endif /* !VBOX_WS_MAC */

class UISession : public QObject
{
    Q_OBJECT;

signals:

    /** Notifies listeners about session initialized. */
    void sigInitialized();

    /** Notifies about additions state change. */
    void sigAdditionsStateChange();
    /** Notifies about additions state actually change. */
    void sigAdditionsStateActualChange();
    /** Notifies about additions state actually change. */
    void sigAudioAdapterChange();
    /** Notifies about clipboard mode change. */
    void sigClipboardModeChange(KClipboardMode enmMode);
    /** Notifies about CPU execution cap change. */
    void sigCPUExecutionCapChange();
    /** Notifies about DnD mode change. */
    void sigDnDModeChange(KDnDMode enmMode);
    /** Notifies about guest monitor change. */
    void sigGuestMonitorChange(KGuestMonitorChangedEventType enmChangeType, ulong uScreenId, QRect screenGeo);
    /** Notifies about machine change. */
    void sigMachineStateChange();
    /** Notifies about medium change. */
    void sigMediumChange(const CMediumAttachment &comMediumAttachment);
    /** Notifies about network adapter change. */
    void sigNetworkAdapterChange(const CNetworkAdapter &comNetworkAdapter);
    /** Notifies about recording change. */
    void sigRecordingChange();
    /** Notifies about shared folder change. */
    void sigSharedFolderChange();
    /** Notifies about storage device change for @a attachment, which was @a fRemoved and it was @a fSilent for guest. */
    void sigStorageDeviceChange(const CMediumAttachment &comAttachment, bool fRemoved, bool fSilent);
    /** Handles USB controller change signal. */
    void sigUSBControllerChange();
    /** Handles USB device state change signal. */
    void sigUSBDeviceStateChange(const CUSBDevice &comDevice, bool fAttached, const CVirtualBoxErrorInfo &comError);
    /** Notifies about VRDE change. */
    void sigVRDEChange();

    /** Notifies about runtime error happened. */
    void sigRuntimeError(bool fFatal, const QString &strErrorId, const QString &strMessage);

#ifdef VBOX_WS_MAC
    /** Notifies about VM window should be shown. */
    void sigShowWindows();
#endif

    /** Notifies about keyboard LEDs change. */
    void sigKeyboardLedsChange(bool fNumLock, bool fCapsLock, bool fScrollLock);

    /** Notifies listeners about mouse pointer shape change. */
    void sigMousePointerShapeChange(const UIMousePointerShapeData &shapeData);
    /** Notifies listeners about mouse capability change. */
    void sigMouseCapabilityChange(bool fSupportsAbsolute, bool fSupportsRelative,
                                  bool fSupportsTouchScreen, bool fSupportsTouchPad,
                                  bool fNeedsHostCursor);
    /** Notifies listeners about cursor position change. */
    void sigCursorPositionChange(bool fContainsData, unsigned long uX, unsigned long uY);

    /** Notifies about frame-buffer resize. */
    void sigFrameBufferResize();

public:

    /** Factory constructor. */
    static bool create(UISession *&pSession, UIMachine *pMachine);
    /** Factory destructor. */
    static void destroy(UISession *&pSession);

    /* API: Runtime UI stuff: */
    bool initialize();
    /** Powers VM up. */
    bool powerUp();

    /** Returns the session instance. */
    CSession& session() { return m_session; }
    /** Returns the session's machine instance. */
    CMachine& machine() { return m_machine; }
    /** Returns the session's console instance. */
    CConsole& console() { return m_console; }
    /** Returns the console's display instance. */
    CDisplay& display() { return m_display; }
    /** Returns the console's guest instance. */
    CGuest& guest() { return m_guest; }
    /** Returns the console's mouse instance. */
    CMouse& mouse() { return m_mouse; }
    /** Returns the console's keyboard instance. */
    CKeyboard& keyboard() { return m_keyboard; }
    /** Returns the console's debugger instance. */
    CMachineDebugger& debugger() { return m_debugger; }

    /** Returns the machine name. */
    const QString& machineName() const { return m_strMachineName; }
    KMachineState machineStatePrevious() const { return m_machineStatePrevious; }
    KMachineState machineState() const { return m_machineState; }

    UIMachineLogic* machineLogic() const;
    QWidget* mainMachineWindow() const;
    WId mainMachineWindowId() const;
    UIMachineWindow *activeMachineWindow() const;

    /** Returns whether visual @a state is allowed. */
    bool isVisualStateAllowed(UIVisualStateType state) const;
    /** Requests visual-state change. */
    void changeVisualState(UIVisualStateType visualStateType);
    /** Requests visual-state to be entered when possible. */
    void setRequestedVisualState(UIVisualStateType visualStateType);
    /** Returns requested visual-state to be entered when possible. */
    UIVisualStateType requestedVisualState() const;

    bool isSaved() const { return machineState() == KMachineState_Saved ||
                                  machineState() == KMachineState_AbortedSaved; }
    bool isTurnedOff() const { return machineState() == KMachineState_PoweredOff ||
                                      machineState() == KMachineState_Saved ||
                                      machineState() == KMachineState_Teleported ||
                                      machineState() == KMachineState_Aborted ||
                                      machineState() == KMachineState_AbortedSaved; }
    bool isPaused() const { return machineState() == KMachineState_Paused ||
                                   machineState() == KMachineState_TeleportingPausedVM; }
    bool isRunning() const { return machineState() == KMachineState_Running ||
                                    machineState() == KMachineState_Teleporting ||
                                    machineState() == KMachineState_LiveSnapshotting; }
    bool isStuck() const { return machineState() == KMachineState_Stuck; }
    bool wasPaused() const { return machineStatePrevious() == KMachineState_Paused ||
                                    machineStatePrevious() == KMachineState_TeleportingPausedVM; }
    bool isInitialized() const { return m_fInitialized; }
    bool isGuestResizeIgnored() const { return m_fIsGuestResizeIgnored; }
    bool isAutoCaptureDisabled() const { return m_fIsAutoCaptureDisabled; }

    /* Guest additions state getters: */
    bool isGuestAdditionsActive() const { return (m_ulGuestAdditionsRunLevel > KAdditionsRunLevelType_None); }
    bool isGuestSupportsGraphics() const { return m_fIsGuestSupportsGraphics; }
    /* The double check below is correct, even though it is an implementation
     * detail of the Additions which the GUI should not ideally have to know. */
    bool isGuestSupportsSeamless() const { return isGuestSupportsGraphics() && m_fIsGuestSupportsSeamless; }
    /** Returns whether GA can be upgraded. */
    bool guestAdditionsUpgradable();

    /* Common setters: */
    bool pause() { return setPause(true); }
    bool unpause() { return setPause(false); }
    bool setPause(bool fOn);
    void setGuestResizeIgnored(bool fIsGuestResizeIgnored) { m_fIsGuestResizeIgnored = fIsGuestResizeIgnored; }
    void setAutoCaptureDisabled(bool fIsAutoCaptureDisabled) { m_fIsAutoCaptureDisabled = fIsAutoCaptureDisabled; }
    void forgetPreviousMachineState() { m_machineStatePrevious = m_machineState; }

    /** Returns whether guest-screen is undrawable.
     *  @todo: extend this method to all the states when guest-screen is undrawable. */
    bool isGuestScreenUnDrawable() const { return machineState() == KMachineState_Stopping ||
                                                  machineState() == KMachineState_Saving; }

    /* Returns existing framebuffer for the given screen-number;
     * Returns 0 (asserts) if screen-number attribute is out of bounds: */
    UIFrameBuffer* frameBuffer(ulong uScreenId) const;
    /* Sets framebuffer for the given screen-number;
     * Ignores (asserts) if screen-number attribute is out of bounds: */
    void setFrameBuffer(ulong uScreenId, UIFrameBuffer* pFrameBuffer);
    /** Returns existing frame-buffer vector. */
    const QVector<UIFrameBuffer*>& frameBuffers() const { return m_frameBufferVector; }

    /** @name CPU hardware virtualization features for VM.
     ** @{ */
    /** Returns whether CPU hardware virtualization extension is enabled. */
    KVMExecutionEngine getVMExecutionEngine() const { return m_enmVMExecutionEngine; }
    /** Returns whether nested-paging CPU hardware virtualization extension is enabled. */
    bool isHWVirtExNestedPagingEnabled() const { return m_fIsHWVirtExNestedPagingEnabled; }
    /** Returns whether the VM is currently making use of the unrestricted execution feature of VT-x. */
    bool isHWVirtExUXEnabled() const { return m_fIsHWVirtExUXEnabled; }
    /** @} */

    /** Returns VM's effective paravirtualization provider. */
    KParavirtProvider paraVirtProvider() const { return m_paraVirtProvider; }

    /** Returns a vector of media attached to the machine. */
    CMediumVector machineMedia() const;

    /** Prepares VM to be saved. */
    bool prepareToBeSaved();
    /** Returns whether VM can be shutdowned. */
    bool prepareToBeShutdowned();

public slots:

    /** Handles request to install guest additions image.
      * @param  strSource  Brings the source of image being installed. */
    void sltInstallGuestAdditionsFrom(const QString &strSource);
    /** Mounts DVD adhoc.
      * @param  strSource  Brings the source of image being mounted. */
    void sltMountDVDAdHoc(const QString &strSource);

private slots:

    /** Detaches COM. */
    void sltDetachCOM();

    /* Console events slots */
    void sltStateChange(KMachineState state);
    void sltAdditionsChange();

private:

    /** Constructor. */
    UISession(UIMachine *pMachine);
    /** Destructor. */
    ~UISession();

    /* Private getters: */
    UIMachine* uimachine() const { return m_pMachine; }

    /* Prepare helpers: */
    bool prepare();
    bool prepareSession();
    void prepareNotificationCenter();
    void prepareConsoleEventHandlers();
    void prepareFramebuffers();
    void prepareConnections();
    void prepareSignalHandling();

    /* Cleanup helpers: */
    void cleanupFramebuffers();
    void cleanupConsoleEventHandlers();
    void cleanupNotificationCenter();
    void cleanupSession();

    /* Common helpers: */
    bool preprocessInitialization();
    bool mountAdHocImage(KDeviceType enmDeviceType, UIMediumDeviceType enmMediumType, const QString &strMediumName);
    bool postprocessInitialization();
    /** Loads VM settings. */
    void loadVMSettings();

    /* Private variables: */
    UIMachine *m_pMachine;

    /** Holds the CConsole event handler instance. */
    UIConsoleEventHandler *m_pConsoleEventhandler;

    /** Holds the session instance. */
    CSession m_session;
    /** Holds the session's machine instance. */
    CMachine m_machine;
    /** Holds the session's console instance. */
    CConsole m_console;
    /** Holds the console's display instance. */
    CDisplay m_display;
    /** Holds the console's guest instance. */
    CGuest m_guest;
    /** Holds the console's mouse instance. */
    CMouse m_mouse;
    /** Holds the console's keyboard instance. */
    CKeyboard m_keyboard;
    /** Holds the console's debugger instance. */
    CMachineDebugger m_debugger;

    /** Holds the machine name. */
    QString m_strMachineName;
    /** Holds the previous machine state. */
    KMachineState m_machineStatePrevious;
    /** Holds the actual machine state. */
    KMachineState m_machineState;

    /* Frame-buffers vector: */
    QVector<UIFrameBuffer*> m_frameBufferVector;

    /* Common flags: */
    bool m_fInitialized : 1;
    bool m_fIsGuestResizeIgnored : 1;
    bool m_fIsAutoCaptureDisabled : 1;

    /* Guest additions flags: */
    ULONG m_ulGuestAdditionsRunLevel;
    bool  m_fIsGuestSupportsGraphics : 1;
    bool  m_fIsGuestSupportsSeamless : 1;

    /** Copy of IMachineDebugger::ExecutionEngine */
    KVMExecutionEngine m_enmVMExecutionEngine;

    /** @name CPU hardware virtualization features for VM.
     ** @{ */
    /** Holds whether nested-paging CPU hardware virtualization extension is enabled. */
    bool m_fIsHWVirtExNestedPagingEnabled;
    /** Holds whether the VM is currently making use of the unrestricted execution feature of VT-x. */
    bool m_fIsHWVirtExUXEnabled;
    /** @} */

    /** Holds VM's effective paravirtualization provider. */
    KParavirtProvider m_paraVirtProvider;
};

#endif /* !FEQT_INCLUDED_SRC_runtime_UISession_h */
