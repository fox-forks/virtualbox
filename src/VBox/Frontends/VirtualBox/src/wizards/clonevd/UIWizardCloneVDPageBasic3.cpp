/* $Id$ */
/** @file
 * VBox Qt GUI - UIWizardCloneVDPageBasic3 class implementation.
 */

/*
 * Copyright (C) 2006-2020 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

/* Qt includes: */
#include <QVBoxLayout>

/* GUI includes: */
#include "UIWizardCloneVDPageBasic3.h"
#include "UIWizardDiskEditors.h"
#include "UIWizardCloneVD.h"
#include "UICommon.h"
#include "UIMessageCenter.h"
#include "UIIconPool.h"
#include "QIFileDialog.h"
#include "QIRichTextLabel.h"
#include "QIToolButton.h"

/* COM includes: */
#include "CMediumFormat.h"


// UIWizardCloneVDPage3::UIWizardCloneVDPage3()
// {
// }

// void UIWizardCloneVDPage3::onSelectLocationButtonClicked()
// {
//     // /* Get current folder and filename: */
//     // QFileInfo fullFilePath(mediumPath());
//     // QDir folder = fullFilePath.path();
//     // QString strFileName = fullFilePath.fileName();

//     // /* Set the first parent folder that exists as the current: */
//     // while (!folder.exists() && !folder.isRoot())
//     // {
//     //     QFileInfo folderInfo(folder.absolutePath());
//     //     if (folder == QDir(folderInfo.absolutePath()))
//     //         break;
//     //     folder = folderInfo.absolutePath();
//     // }

//     // /* But if it doesn't exists at all: */
//     // if (!folder.exists() || folder.isRoot())
//     // {
//     //     /* Use recommended one folder: */
//     //     QFileInfo defaultFilePath(absoluteFilePath(strFileName, m_strDefaultPath));
//     //     folder = defaultFilePath.path();
//     // }

//     // /* Prepare backends list: */
//     // QVector<QString> fileExtensions;
//     // QVector<KDeviceType> deviceTypes;
//     // CMediumFormat mediumFormat = fieldImp("mediumFormat").value<CMediumFormat>();
//     // mediumFormat.DescribeFileExtensions(fileExtensions, deviceTypes);
//     // QStringList validExtensionList;
//     // for (int i = 0; i < fileExtensions.size(); ++i)
//     //     if (deviceTypes[i] == static_cast<UIWizardCloneVD*>(wizardImp())->sourceVirtualDiskDeviceType())
//     //         validExtensionList << QString("*.%1").arg(fileExtensions[i]);
//     // /* Compose full filter list: */
//     // QString strBackendsList = QString("%1 (%2)").arg(mediumFormat.GetName()).arg(validExtensionList.join(" "));

//     // /* Open corresponding file-dialog: */
//     // QString strChosenFilePath = QIFileDialog::getSaveFileName(folder.absoluteFilePath(strFileName),
//     //                                                           strBackendsList, thisImp(),
//     //                                                           UIWizardCloneVD::tr("Please choose a location for new virtual disk image file"));

//     // /* If there was something really chosen: */
//     // if (!strChosenFilePath.isEmpty())
//     // {
//     //     /* If valid file extension is missed, append it: */
//     //     if (QFileInfo(strChosenFilePath).suffix().isEmpty())
//     //         strChosenFilePath += QString(".%1").arg(m_strDefaultExtension);
//     //     m_pDestinationDiskEditor->setText(QDir::toNativeSeparators(strChosenFilePath));
//     //     m_pDestinationDiskEditor->selectAll();
//     //     m_pDestinationDiskEditor->setFocus();
//     // }
// }

// /* static */
// QString UIWizardCloneVDPage3::toFileName(const QString &strName, const QString &strExtension)
// {
//     /* Convert passed name to native separators (it can be full, actually): */
//     QString strFileName = QDir::toNativeSeparators(strName);

//     /* Remove all trailing dots to avoid multiple dots before extension: */
//     int iLen;
//     while (iLen = strFileName.length(), iLen > 0 && strFileName[iLen - 1] == '.')
//         strFileName.truncate(iLen - 1);

//     /* Add passed extension if its not done yet: */
//     if (QFileInfo(strFileName).suffix().toLower() != strExtension)
//         strFileName += QString(".%1").arg(strExtension);

//     /* Return result: */
//     return strFileName;
// }

// /* static */
// QString UIWizardCloneVDPage3::absoluteFilePath(const QString &strFileName, const QString &strDefaultPath)
// {
//     /* Wrap file-info around received file name: */
//     QFileInfo fileInfo(strFileName);
//     /* If path-info is relative or there is no path-info at all: */
//     if (fileInfo.fileName() == strFileName || fileInfo.isRelative())
//     {
//         /* Resolve path on the basis of default path we have: */
//         fileInfo = QFileInfo(strDefaultPath, strFileName);
//     }
//     /* Return full absolute disk image file path: */
//     return QDir::toNativeSeparators(fileInfo.absoluteFilePath());
// }

// /* static */
// void UIWizardCloneVDPage3::acquireExtensions(const CMediumFormat &comMediumFormat, KDeviceType enmDeviceType,
//                                              QStringList &aAllowedExtensions, QString &strDefaultExtension)
// {
//     /* Load extension / device list: */
//     QVector<QString> fileExtensions;
//     QVector<KDeviceType> deviceTypes;
//     CMediumFormat mediumFormat(comMediumFormat);
//     mediumFormat.DescribeFileExtensions(fileExtensions, deviceTypes);
//     for (int i = 0; i < fileExtensions.size(); ++i)
//         if (deviceTypes[i] == enmDeviceType)
//             aAllowedExtensions << fileExtensions[i].toLower();
//     AssertReturnVoid(!aAllowedExtensions.isEmpty());
//     strDefaultExtension = aAllowedExtensions.first();
// }

// QString UIWizardCloneVDPage3::mediumPath() const
// {
//     /* Acquire chosen file path, and what is important user suffix: */
//     const QString strChosenFilePath = m_pDestinationDiskEditor->text();
//     QString strSuffix = QFileInfo(strChosenFilePath).suffix().toLower();
//     /* If there is no suffix of it's not allowed: */
//     if (   strSuffix.isEmpty()
//         || !m_aAllowedExtensions.contains(strSuffix))
//         strSuffix = m_strDefaultExtension;
//     /* Compose full file path finally: */
//     return absoluteFilePath(toFileName(m_pDestinationDiskEditor->text(), strSuffix), m_strDefaultPath);
// }

// qulonglong UIWizardCloneVDPage3::mediumSize()
// {
//     // UIWizardCloneVD *pWizard = qobject_cast<UIWizardCloneVD*>(wizardImp());
//     // if (!pWizard)
//     //     return 0;
//     // const CMedium &sourceVirtualDisk = pWizard->sourceVirtualDisk();
//     // return sourceVirtualDisk.isNull() ? 0 : sourceVirtualDisk.GetLogicalSize();
//     return 0;
// }

UIWizardCloneVDPageBasic3::UIWizardCloneVDPageBasic3(qulonglong uSourceDiskLogicaSize)
    : m_pMediumSizePathGroupBox(0)
{
    prepare(uSourceDiskLogicaSize);
}

void UIWizardCloneVDPageBasic3::prepare(qulonglong uSourceDiskLogicaSize)
{
    QVBoxLayout *pMainLayout = new QVBoxLayout(this);

    m_pMediumSizePathGroupBox = new UIMediumSizeAndPathGroupBox(false /* expert mode */, 0 /* parent */, uSourceDiskLogicaSize);
    if (m_pMediumSizePathGroupBox)
        pMainLayout->addWidget(m_pMediumSizePathGroupBox);

    pMainLayout->addStretch();

    connect(m_pMediumSizePathGroupBox, &UIMediumSizeAndPathGroupBox::sigMediumLocationButtonClicked,
            this, &UIWizardCloneVDPageBasic3::sltSelectLocationButtonClicked);
    connect(m_pMediumSizePathGroupBox, &UIMediumSizeAndPathGroupBox::sigMediumPathChanged,
            this, &UIWizardCloneVDPageBasic3::sltMediumPathChanged);

    retranslateUi();
}

void UIWizardCloneVDPageBasic3::retranslateUi()
{
    /* Translate page: */
    setTitle(UIWizardCloneVD::tr("Location and size of the disk image"));
}

void UIWizardCloneVDPageBasic3::initializePage()
{
    AssertReturnVoid(cloneWizard() && m_pMediumSizePathGroupBox);
    /* Translate page: */
    retranslateUi();
    UIWizardCloneVD *pWizard = cloneWizard();
    m_pMediumSizePathGroupBox->blockSignals(true);

    /* Initialize medium size widget and wizard's medium size parameter: */
    if (!m_userModifiedParameters.contains("MediumSize"))
    {
        m_pMediumSizePathGroupBox->setMediumSize(pWizard->sourceDiskLogicalSize());
        pWizard->setMediumSize(m_pMediumSizePathGroupBox->mediumSize());
    }

    if (!m_userModifiedParameters.contains("MediumPath"))
    {
        const CMediumFormat comMediumFormat = pWizard->mediumFormat();
        AssertReturnVoid(!comMediumFormat.isNull());
        QString strExtension = UIDiskFormatsGroupBox::defaultExtension(comMediumFormat, KDeviceType_HardDisk);
        QString strSourceDiskPath = QDir::toNativeSeparators(QFileInfo(pWizard->sourceDiskFilePath()).absolutePath());
        /* Disk name without the format extension: */
        QString strDiskName = QString("%1_%2").arg(QFileInfo(pWizard->sourceDiskName()).completeBaseName()).arg(tr("copy"));

        QString strMediumFilePath =
            UIDiskEditorGroupBox::constructMediumFilePath(UIDiskVariantGroupBox::appendExtension(strDiskName,
                                                                                                 strExtension), strSourceDiskPath);
        m_pMediumSizePathGroupBox->setMediumPath(strMediumFilePath);
        pWizard->setMediumPath(strMediumFilePath);
    }
    m_pMediumSizePathGroupBox->blockSignals(false);
}

bool UIWizardCloneVDPageBasic3::isComplete() const
{
    /* Make sure current name is not empty: */
    /*return !m_pDestinationDiskEditor->text().trimmed().isEmpty();*/
    return true;
}

bool UIWizardCloneVDPageBasic3::validatePage()
{
    /* Initial result: */
    bool fResult = true;

    // /* Make sure such file doesn't exists already: */
    // QString strMediumPath(mediumPath());
    // fResult = !QFileInfo(strMediumPath).exists();
    // if (!fResult)
    //     msgCenter().cannotOverwriteHardDiskStorage(strMediumPath, this);

    // if (fResult)
    // {
    //     /* Lock finish button: */
    //     startProcessing();

    //     /* Try to copy virtual disk image file: */
    //     fResult = qobject_cast<UIWizardCloneVD*>(wizard())->copyVirtualDisk();

    //     /* Unlock finish button: */
    //     endProcessing();
    // }

    /* Return result: */
    return fResult;
}

UIWizardCloneVD *UIWizardCloneVDPageBasic3::cloneWizard() const
{
    return qobject_cast<UIWizardCloneVD*>(wizard());
}

void UIWizardCloneVDPageBasic3::sltSelectLocationButtonClicked()
{
    UIWizardCloneVD *pWizard = cloneWizard();
    AssertReturnVoid(pWizard);
    CMediumFormat comMediumFormat(pWizard->mediumFormat());
    QString strSelectedPath =
        UIDiskEditorGroupBox::openFileDialogForDiskFile(pWizard->mediumPath(), comMediumFormat, pWizard);

    if (strSelectedPath.isEmpty())
        return;
    QString strMediumPath =
        UIDiskEditorGroupBox::appendExtension(strSelectedPath,
                                              UIDiskFormatsGroupBox::defaultExtension(pWizard->mediumFormat(), KDeviceType_HardDisk));
    QFileInfo mediumPath(strMediumPath);
    m_pMediumSizePathGroupBox->setMediumPath(QDir::toNativeSeparators(mediumPath.absoluteFilePath()));
}

void UIWizardCloneVDPageBasic3::sltMediumPathChanged(const QString &strPath)
{
    UIWizardCloneVD *pWizard = cloneWizard();
    AssertReturnVoid(pWizard);
    m_userModifiedParameters << "MediumPath";
    QString strMediumPath =
        UIDiskEditorGroupBox::appendExtension(strPath,
                                              UIDiskFormatsGroupBox::defaultExtension(pWizard->mediumFormat(), pWizard->deviceType()));
    pWizard->setMediumPath(strMediumPath);
    emit completeChanged();
}
