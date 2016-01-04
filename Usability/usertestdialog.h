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

    void setTaskLabel(QString task);

public slots:
    void updateClock();
    void updateRotation();
    void setCurrentJob(int job);
    void setCurrentPrototype(int protoType);


    void collectResults();    

    void incRotation();
    void incZRotation();
    void incTranslation();
    void incScaling();
    void incReset();

    void incSubVolResize();
    void incSubVolReset();
    void incSubVolPoint1();
    void incSubVolPoint2();

    void incSliceReSize();
    void incSliceReset();

    QString getUserResults();



signals:
    void stopTest();
    void saveScreen();


private:
    Ui::UserTestDialog *ui;
    QTimer *timer ;

    int currentJob = -1;
    int currentPrototype = -1;

    int countRotate = 0;
    int countTranslate = 0;
    int countScale = 0;
    int countZRotate = 0;

    int countLeapStarts = 0;
    int countResets = 0;

    int countSubVolResize = 0 ;
    int countSubVolReset = 0;    

    int countSubVolPnt1 = 0;
    int countSubVolPnt2 = 0;

    int countSliceResize = 0;
    int countSliceReset = 0;

};

#endif // USERTESTDIALOG_H
