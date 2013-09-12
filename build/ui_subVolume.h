/********************************************************************************
** Form generated from reading UI file 'subVolume.ui'
**
** Created: Wed Aug 21 12:10:43 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SUBVOLUME_H
#define UI_SUBVOLUME_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>
#include "QVTKWidget.h"

QT_BEGIN_NAMESPACE

class Ui_SubVolume
{
public:
    QVTKWidget *qvtkWidgetFloat;

    void setupUi(QWidget *SubVolume)
    {
        if (SubVolume->objectName().isEmpty())
            SubVolume->setObjectName(QString::fromUtf8("SubVolume"));
        SubVolume->resize(540, 522);
        qvtkWidgetFloat = new QVTKWidget(SubVolume);
        qvtkWidgetFloat->setObjectName(QString::fromUtf8("qvtkWidgetFloat"));
        qvtkWidgetFloat->setGeometry(QRect(100, 40, 300, 300));
        qvtkWidgetFloat->setAutoFillBackground(true);

        retranslateUi(SubVolume);

        QMetaObject::connectSlotsByName(SubVolume);
    } // setupUi

    void retranslateUi(QWidget *SubVolume)
    {
        SubVolume->setWindowTitle(QApplication::translate("SubVolume", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SubVolume: public Ui_SubVolume {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SUBVOLUME_H
