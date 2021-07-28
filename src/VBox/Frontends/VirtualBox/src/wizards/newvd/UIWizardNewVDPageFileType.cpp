/* $Id$ */
/** @file
 * VBox Qt GUI - UIWizardNewVDPageFileType class implementation.
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
#include <QButtonGroup>
#include <QRadioButton>

/* GUI includes: */
#include "UIConverter.h"
#include "UIWizardDiskEditors.h"
#include "UIWizardNewVDPageFileType.h"
#include "UIWizardNewVDPageSizeLocation.h"
#include "UIWizardNewVD.h"
#include "UICommon.h"
#include "QIRichTextLabel.h"

/* COM includes: */
#include "CSystemProperties.h"

// void UIWizardNewVDPageBaseFileType::setMediumFormat(const CMediumFormat &mediumFormat)
// {
//     int iPosition = m_formats.indexOf(mediumFormat);
//     if (iPosition >= 0)
//     {
//         m_pFormatButtonGroup->button(iPosition)->click();
//         m_pFormatButtonGroup->button(iPosition)->setFocus();
//     }
// }


UIWizardNewVDPageFileType::UIWizardNewVDPageFileType()
    : m_pLabel(0)
    , m_pFormatButtonGroup(0)
{
    prepare();
    /* Create widgets: */

    // /* Setup connections: */
    // connect(m_pFormatButtonGroup, static_cast<void(QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
    //         this, &UIWizardNewVDPageFileType::completeChanged);

    // /* Register classes: */
    // qRegisterMetaType<CMediumFormat>();
    // /* Register fields: */
    // registerField("mediumFormat", this, "mediumFormat");
}

void UIWizardNewVDPageFileType::prepare()
{
    QVBoxLayout *pMainLayout = new QVBoxLayout(this);
    m_pLabel = new QIRichTextLabel(this);
    pMainLayout->addWidget(m_pLabel);
    m_pFormatButtonGroup = new UIDiskFormatsGroupBox(false, 0);
    pMainLayout->addWidget(m_pFormatButtonGroup, false);

    pMainLayout->addStretch();
    connect(m_pFormatButtonGroup, &UIDiskFormatsGroupBox::sigMediumFormatChanged,
            this, &UIWizardNewVDPageFileType::sltMediumFormatChanged);
    retranslateUi();
}

void UIWizardNewVDPageFileType::sltMediumFormatChanged()
{
    AssertReturnVoid(m_pFormatButtonGroup);
    newVDWizardPropertySet(MediumFormat, m_pFormatButtonGroup->mediumFormat());
}

void UIWizardNewVDPageFileType::retranslateUi()
{
    setTitle(UIWizardNewVD::tr("Virtual Hard disk file type"));

    m_pLabel->setText(UIWizardNewVD::tr("Please choose the type of file that you would like to use "
                                        "for the new virtual hard disk. If you do not need to use it "
                                        "with other virtualization software you can leave this setting unchanged."));
}

void UIWizardNewVDPageFileType::initializePage()
{
    retranslateUi();
    if (m_pFormatButtonGroup)
        newVDWizardPropertySet(MediumFormat, m_pFormatButtonGroup->mediumFormat());
}

bool UIWizardNewVDPageFileType::isComplete() const
{
    /* Make sure medium format is correct: */
    //return !mediumFormat().isNull();
    return true;
}

// int UIWizardNewVDPageFileType::nextId() const
// {
//     /* Show variant page only if there is something to show: */
//     CMediumFormat mf = mediumFormat();
//     if (mf.isNull())
//     {
//         AssertMsgFailed(("No medium format set!"));
//     }
//     else
//     {
//         ULONG uCapabilities = 0;
//         QVector<KMediumFormatCapabilities> capabilities;
//         capabilities = mf.GetCapabilities();
//         for (int i = 0; i < capabilities.size(); i++)
//             uCapabilities |= capabilities[i];

//         int cTest = 0;
//         if (uCapabilities & KMediumFormatCapabilities_CreateDynamic)
//             ++cTest;
//         if (uCapabilities & KMediumFormatCapabilities_CreateFixed)
//             ++cTest;
//         if (uCapabilities & KMediumFormatCapabilities_CreateSplit2G)
//             ++cTest;
//         if (cTest > 1)
//             return UIWizardNewVD::Page2;
//     }
//     /* Skip otherwise: */
//     return UIWizardNewVD::Page3;
// }
