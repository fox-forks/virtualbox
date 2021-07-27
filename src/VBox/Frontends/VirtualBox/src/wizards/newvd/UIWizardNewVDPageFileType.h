/* $Id$ */
/** @file
 * VBox Qt GUI - UIWizardNewVDPageFileType class declaration.
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

#ifndef FEQT_INCLUDED_SRC_wizards_newvd_UIWizardNewVDPageFileType_h
#define FEQT_INCLUDED_SRC_wizards_newvd_UIWizardNewVDPageFileType_h
#ifndef RT_WITHOUT_PRAGMA_ONCE
# pragma once
#endif

/* GUI includes: */
#include "UINativeWizardPage.h"

/* COM includes: */
#include "COMEnums.h"
#include "CMediumFormat.h"

/* Forward declarations: */
class QVBoxLayout;
class QButtonGroup;
class QRadioButton;
class QIRichTextLabel;
class UIDiskFormatsGroupBox;

namespace UIWizardNewVDFileType
{
//     void addFormatButton(QWidget *pParent, QVBoxLayout *pFormatsLayout, CMediumFormat medFormat, bool fPreferred = false);

   // QWidget *createFormatButtonGroup(QButtonGroup *&pFormatButtonGroup, bool fExperMode);

//     CMediumFormat mediumFormat() const;
//     void setMediumFormat(const CMediumFormat &mediumFormat);
//     void retranslateWidgets();


//     QList<CMediumFormat> m_formats;
//     QStringList m_formatNames;
//     QStringList m_formatExtensions;
}


/* 1st page of the New Virtual Hard Drive wizard (basic extension): */
class SHARED_LIBRARY_STUFF UIWizardNewVDPageFileType : public UINativeWizardPage
{
    Q_OBJECT;

public:

    /* Constructor: */
    UIWizardNewVDPageFileType();

private:

    void retranslateUi();
    void prepare();
    void initializePage();

    /* Validation stuff: */
    bool isComplete() const;


    QIRichTextLabel *m_pLabel;
    UIDiskFormatsGroupBox *m_pFormatButtonGroup;
};


#endif /* !FEQT_INCLUDED_SRC_wizards_newvd_UIWizardNewVDPageFileType_h */
