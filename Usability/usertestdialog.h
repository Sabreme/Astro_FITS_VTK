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



signals:
    void stopTest();


private:
    Ui::UserTestDialog *ui;
    QTimer *timer ;

    int countRotate = 0;
    int countTranslate = 0;
    int countScale = 0;

    int countLeapStarts;
    int countResets;

    int countSubVolResize;
    int countSubVolReset;
    int countSubVolLeapPt1;
    int countSubVolLeapPt2;

    int countSliceNormal;
    int countSlicePlane;
    int countSliceReset;

};

#endif // USERTESTDIALOG_H
