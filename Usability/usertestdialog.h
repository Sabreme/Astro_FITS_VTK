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

    int countRotation ;

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

    int currentJob ;
    int currentPrototype ;

    int countRotate ;
    int countTranslate ;
    int countScale ;
    int countZRotate ;

    int countLeapStarts ;
    int countResets ;

    int countSubVolResize ;
    int countSubVolReset ;

    int countSubVolPnt1 ;
    int countSubVolPnt2 ;

    int countSliceResize ;
    int countSliceReset ;

};

#endif // USERTESTDIALOG_H
