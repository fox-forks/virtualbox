/* $Id$ */
/** @file
 *
 * VBox frontends: Qt4 GUI ("VirtualBox"):
 * VBoxGLSettingsGeneral class implementation
 */

/*
 * Copyright (C) 2006-2008 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

/* Global includes */
#include <QDir>

/* Local includes */
#include "VBoxGLSettingsGeneral.h"
#include "VBoxGlobal.h"

VBoxGLSettingsGeneral::VBoxGLSettingsGeneral()
{
    /* Apply UI decorations */
    Ui::VBoxGLSettingsGeneral::setupUi (this);

#ifndef VBOX_GUI_WITH_SYSTRAY
    mCbCheckTrayIcon->hide();
    mWtSpacer1->hide();
#endif /* !VBOX_GUI_WITH_SYSTRAY */
#ifndef Q_WS_MAC
    mCbCheckPresentationMode->hide();
    mWtSpacer2->hide();
#endif /* !Q_WS_MAC */
//#ifndef Q_WS_WIN /* Checkbox hidden for now! */
    mCbDisableHostScreenSaver->hide();
    mWtSpacer3->hide();
//#endif /* !Q_WS_WIN */

    if (mCbCheckTrayIcon->isHidden() &&
        mCbCheckPresentationMode->isHidden() &&
        mCbDisableHostScreenSaver->isHidden())
        mLnSeparator2->hide();

    mPsHardDisk->setHomeDir (vboxGlobal().virtualBox().GetHomeFolder());
    mPsMach->setHomeDir (vboxGlobal().virtualBox().GetHomeFolder());
    mPsVRDP->setHomeDir (vboxGlobal().virtualBox().GetHomeFolder());
    mPsVRDP->setMode (VBoxFilePathSelectorWidget::Mode_File_Open);

    /* Applying language settings */
    retranslateUi();
}

void VBoxGLSettingsGeneral::getFrom (const CSystemProperties &aProps,
                                     const VBoxGlobalSettings &aGs)
{
    mPsMach->setPath (aProps.GetDefaultMachineFolder());
    mPsVRDP->setPath (aProps.GetRemoteDisplayAuthLibrary());
    mCbCheckTrayIcon->setChecked (aGs.trayIconEnabled());
#ifdef Q_WS_MAC
    mCbCheckPresentationMode->setChecked (aGs.presentationModeEnabled());
#endif /* Q_WS_MAC */
    mCbDisableHostScreenSaver->setChecked (aGs.hostScreenSaverDisabled());
}

void VBoxGLSettingsGeneral::putBackTo (CSystemProperties &aProps,
                                       VBoxGlobalSettings &aGs)
{
    if (aProps.isOk() && mPsMach->isModified())
        aProps.SetDefaultMachineFolder (mPsMach->path());
    if (aProps.isOk() && mPsVRDP->isModified())
        aProps.SetRemoteDisplayAuthLibrary (mPsVRDP->path());
    aGs.setTrayIconEnabled (mCbCheckTrayIcon->isChecked());
#ifdef Q_WS_MAC
    aGs.setPresentationModeEnabled (mCbCheckPresentationMode->isChecked());
#endif /* Q_WS_MAC */
    aGs.setHostScreenSaverDisabled (mCbDisableHostScreenSaver->isChecked());
}

void VBoxGLSettingsGeneral::retranslateUi()
{
    /* Translate uic generated strings */
    Ui::VBoxGLSettingsGeneral::retranslateUi (this);

    mPsHardDisk->setWhatsThis (tr ("Displays the path to the default hard disk "
                                   "folder. This folder is used, if not explicitly "
                                   "specified otherwise, when adding existing or "
                                   "creating new virtual hard disks."));
    mPsMach->setWhatsThis (tr ("Displays the path to the default virtual "
                               "machine folder. This folder is used, if not "
                               "explicitly specified otherwise, when creating "
                               "new virtual machines."));
    mPsVRDP->setWhatsThis (tr ("Displays the path to the library that "
                               "provides authentication for Remote Display "
                               "(VRDP) clients."));
}

