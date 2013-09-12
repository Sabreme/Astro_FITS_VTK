/********************************************************************************
** Form generated from reading UI file 'subvolumedialog.ui'
**
** Created: Tue Sep 10 17:57:12 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SUBVOLUMEDIALOG_H
#define UI_SUBVOLUMEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QTextEdit>
#include "QVTKWidget.h"

QT_BEGIN_NAMESPACE

class Ui_SubVolumeDialog
{
public:
    QDialogButtonBox *buttonBox;
    QTextEdit *textEdit_X_Toal;
    QTextEdit *textEdit_2;
    QSlider *horizontal_Y_Axis;
    QSlider *horizontal_X_Axis;
    QTextEdit *textEdit_3;
    QSlider *horizontalSlider_Z_Axis;
    QVTKWidget *qvtkWidgetRight;
    QLabel *label_2;
    QLabel *label;
    QLabel *label_3;

    void setupUi(QDialog *SubVolumeDialog)
    {
        if (SubVolumeDialog->objectName().isEmpty())
            SubVolumeDialog->setObjectName(QString::fromUtf8("SubVolumeDialog"));
        SubVolumeDialog->resize(610, 597);
        buttonBox = new QDialogButtonBox(SubVolumeDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(190, 550, 181, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        textEdit_X_Toal = new QTextEdit(SubVolumeDialog);
        textEdit_X_Toal->setObjectName(QString::fromUtf8("textEdit_X_Toal"));
        textEdit_X_Toal->setGeometry(QRect(150, 480, 41, 31));
        textEdit_2 = new QTextEdit(SubVolumeDialog);
        textEdit_2->setObjectName(QString::fromUtf8("textEdit_2"));
        textEdit_2->setGeometry(QRect(350, 480, 41, 31));
        horizontal_Y_Axis = new QSlider(SubVolumeDialog);
        horizontal_Y_Axis->setObjectName(QString::fromUtf8("horizontal_Y_Axis"));
        horizontal_Y_Axis->setGeometry(QRect(220, 480, 121, 29));
        horizontal_Y_Axis->setOrientation(Qt::Horizontal);
        horizontal_X_Axis = new QSlider(SubVolumeDialog);
        horizontal_X_Axis->setObjectName(QString::fromUtf8("horizontal_X_Axis"));
        horizontal_X_Axis->setGeometry(QRect(20, 480, 121, 31));
        horizontal_X_Axis->setOrientation(Qt::Horizontal);
        textEdit_3 = new QTextEdit(SubVolumeDialog);
        textEdit_3->setObjectName(QString::fromUtf8("textEdit_3"));
        textEdit_3->setGeometry(QRect(560, 480, 41, 31));
        horizontalSlider_Z_Axis = new QSlider(SubVolumeDialog);
        horizontalSlider_Z_Axis->setObjectName(QString::fromUtf8("horizontalSlider_Z_Axis"));
        horizontalSlider_Z_Axis->setGeometry(QRect(430, 480, 121, 29));
        horizontalSlider_Z_Axis->setOrientation(Qt::Horizontal);
        qvtkWidgetRight = new QVTKWidget(SubVolumeDialog);
        qvtkWidgetRight->setObjectName(QString::fromUtf8("qvtkWidgetRight"));
        qvtkWidgetRight->setGeometry(QRect(20, 20, 561, 421));
        label_2 = new QLabel(SubVolumeDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(260, 520, 67, 17));
        label = new QLabel(SubVolumeDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(30, 520, 67, 17));
        label_3 = new QLabel(SubVolumeDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(490, 520, 67, 17));

        retranslateUi(SubVolumeDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SubVolumeDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SubVolumeDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SubVolumeDialog);
    } // setupUi

    void retranslateUi(QDialog *SubVolumeDialog)
    {
        SubVolumeDialog->setWindowTitle(QApplication::translate("SubVolumeDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("SubVolumeDialog", "Y-AXIS", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SubVolumeDialog", "X-AXIS", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("SubVolumeDialog", "Z-AXIS", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SubVolumeDialog: public Ui_SubVolumeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SUBVOLUMEDIALOG_H
