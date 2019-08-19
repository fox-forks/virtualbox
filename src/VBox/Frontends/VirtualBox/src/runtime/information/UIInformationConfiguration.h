/* $Id$ */
/** @file
 * VBox Qt GUI - UIInformationConfiguration class declaration.
 */

/*
 * Copyright (C) 2016-2019 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

#ifndef FEQT_INCLUDED_SRC_runtime_information_UIInformationConfiguration_h
#define FEQT_INCLUDED_SRC_runtime_information_UIInformationConfiguration_h
#ifndef RT_WITHOUT_PRAGMA_ONCE
# pragma once
#endif

/* Qt includes: */
#include <QWidget>

/* COM includes: */
#include "COMEnums.h"
#include "CGuest.h"

#include "CMachine.h"
#include "CConsole.h"

/* GUI includes: */
#include "QIWithRetranslateUI.h"

/* Forward declarations: */
class QVBoxLayout;
class UIInformationView;
class UIInformationModel;
class QTableWidget;
class QTableWidgetItem;
class UITextTableLine;

/** QWidget extension
  * providing GUI with configuration-information tab in session-information window. */
class UIInformationConfiguration : public QIWithRetranslateUI<QWidget>
{
    Q_OBJECT;

public:

    /** Constructs information-tab passing @a pParent to the QWidget base-class constructor.
      * @param machine is machine reference.
      * @param console is machine console reference. */
    UIInformationConfiguration(QWidget *pParent, const CMachine &machine, const CConsole &console);
    ~UIInformationConfiguration();

protected:

    void retranslateUi() /* override */;

private:
    /** Prepares model. */
    void prepareModel();
    void prepareObjects();
    void createTableItems();

    void updateTable();
    void insertTitleRow(int iRow, const QString &strTitle, const QIcon &icon);
    void insertInfoRow(int iRow, const QString strText1, const QString &strText2,
                       QFontMetrics &fontMetrics, int &iMaxColumn1Length);

    /** Holds the machine instance. */
    CMachine m_machine;
    /** Holds the console instance. */
    CConsole m_console;
    /** Holds the instance of layout we create. */
    QVBoxLayout *m_pMainLayout;
    /** Holds the instance of model we create. */
    UIInformationModel *m_pModel;
    /** Holds the instance of view we create. */
    UIInformationView *m_pView;
    QTableWidget *m_pTableWidget;
    //QMap<TableRow, UIInformationTableRow*> m_rows;
    QList<QTableWidgetItem*> m_tableItems;
   /** @name Cached translated string.
      * @{ */
        QString m_strError;

        /** General section. */
        QString m_strGeneralTitle;
        QString m_strGeneralName;
        QString m_strGeneralOSType;
        /** System section. */
        QString m_strSystemTitle;

    /** @} */

};

#endif /* !FEQT_INCLUDED_SRC_runtime_information_UIInformationConfiguration_h */
