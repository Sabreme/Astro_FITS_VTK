/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Sat Sep 14 13:35:50 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>
#include "QVTKWidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionClose;
    QAction *actionExit;
    QAction *actionSubVolume;
    QAction *actionReset_Camera;
    QAction *actionLeft_Side_View;
    QAction *actionRight_Side_View;
    QAction *actionTop_Side_View;
    QAction *actionBottom_Side_View;
    QAction *actionSliceSelection;
    QWidget *centralWidget;
    QVTKWidget *qvtkWidgetLeft;
    QPlainTextEdit *plainTextEdit;
    QMenuBar *menuBar;
    QMenu *menuFIle;
    QMenu *menuActions;
    QMenu *menuViews;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(780, 731);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionClose = new QAction(MainWindow);
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionSubVolume = new QAction(MainWindow);
        actionSubVolume->setObjectName(QString::fromUtf8("actionSubVolume"));
        actionReset_Camera = new QAction(MainWindow);
        actionReset_Camera->setObjectName(QString::fromUtf8("actionReset_Camera"));
        actionLeft_Side_View = new QAction(MainWindow);
        actionLeft_Side_View->setObjectName(QString::fromUtf8("actionLeft_Side_View"));
        actionRight_Side_View = new QAction(MainWindow);
        actionRight_Side_View->setObjectName(QString::fromUtf8("actionRight_Side_View"));
        actionTop_Side_View = new QAction(MainWindow);
        actionTop_Side_View->setObjectName(QString::fromUtf8("actionTop_Side_View"));
        actionBottom_Side_View = new QAction(MainWindow);
        actionBottom_Side_View->setObjectName(QString::fromUtf8("actionBottom_Side_View"));
        actionSliceSelection = new QAction(MainWindow);
        actionSliceSelection->setObjectName(QString::fromUtf8("actionSliceSelection"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        qvtkWidgetLeft = new QVTKWidget(centralWidget);
        qvtkWidgetLeft->setObjectName(QString::fromUtf8("qvtkWidgetLeft"));
        qvtkWidgetLeft->setGeometry(QRect(10, 20, 651, 531));
        plainTextEdit = new QPlainTextEdit(centralWidget);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(0, 570, 671, 101));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 780, 25));
        menuFIle = new QMenu(menuBar);
        menuFIle->setObjectName(QString::fromUtf8("menuFIle"));
        menuActions = new QMenu(menuBar);
        menuActions->setObjectName(QString::fromUtf8("menuActions"));
        menuViews = new QMenu(menuBar);
        menuViews->setObjectName(QString::fromUtf8("menuViews"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::LeftToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFIle->menuAction());
        menuBar->addAction(menuActions->menuAction());
        menuBar->addAction(menuViews->menuAction());
        menuFIle->addAction(actionOpen);
        menuFIle->addAction(actionClose);
        menuFIle->addAction(actionExit);
        menuActions->addAction(actionSubVolume);
        menuActions->addAction(actionSliceSelection);
        menuViews->addAction(actionReset_Camera);
        menuViews->addAction(actionLeft_Side_View);
        menuViews->addAction(actionRight_Side_View);
        menuViews->addAction(actionTop_Side_View);
        menuViews->addAction(actionBottom_Side_View);
        mainToolBar->addAction(actionOpen);
        mainToolBar->addAction(actionSubVolume);
        mainToolBar->addAction(actionExit);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("MainWindow", "Open", 0, QApplication::UnicodeUTF8));
        actionClose->setText(QApplication::translate("MainWindow", "Close", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8));
        actionSubVolume->setText(QApplication::translate("MainWindow", "SubVolume", 0, QApplication::UnicodeUTF8));
        actionReset_Camera->setText(QApplication::translate("MainWindow", "Reset Camera", 0, QApplication::UnicodeUTF8));
        actionLeft_Side_View->setText(QApplication::translate("MainWindow", "Left Side View", 0, QApplication::UnicodeUTF8));
        actionRight_Side_View->setText(QApplication::translate("MainWindow", "Right Side View", 0, QApplication::UnicodeUTF8));
        actionTop_Side_View->setText(QApplication::translate("MainWindow", "Top Side View", 0, QApplication::UnicodeUTF8));
        actionBottom_Side_View->setText(QApplication::translate("MainWindow", "Bottom Side View", 0, QApplication::UnicodeUTF8));
        actionSliceSelection->setText(QApplication::translate("MainWindow", "SliceSelection", 0, QApplication::UnicodeUTF8));
        menuFIle->setTitle(QApplication::translate("MainWindow", "FIle", 0, QApplication::UnicodeUTF8));
        menuActions->setTitle(QApplication::translate("MainWindow", "Actions", 0, QApplication::UnicodeUTF8));
        menuViews->setTitle(QApplication::translate("MainWindow", "Views", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
