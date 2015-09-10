#ifndef USERTESTDIALOG_H
#define USERTESTDIALOG_H

#include <QDialog>
#include <QTimer>
#include <iostream>

namespace Ui {
class UserTestDialog;
}

class UserTestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserTestDialog(QWidget *parent = 0);
    ~UserTestDialog();

    int countRotation = 0;

public slots:
    void updateCaption();
    void updateRotation();

    void collectResults();

    void incRotation();
    void incTranslation();
    void incScaling();
    void incReset();

    void incSubVolResize();
    void incSubVolReset();
    void incSubVolPointLeft();
    void incSubVolPointRight();

    void incSliceReSize();
    void incSliceReset();    



signals:
    void stopTest();
    void saveScreen();


private:
    Ui::UserTestDialog *ui;
    QTimer *timer ;

    int countRotate = 0;
    int countTranslate = 0;
    int countScale = 0;

    int countLeapStarts;
    int countResets;

    int countSubVolResize = 0 ;
    int countSubVolReset = 0;
    int countSubVolPnt1 = 0;
    int countSubVolPnt2 = 0 ;
    int countSubVolPntLeft = 0;
    int countSubVolPntRight = 0;

    int countSliceResize = 0;
    int countSliceReset = 0;

};

#endif // USERTESTDIALOG_H
