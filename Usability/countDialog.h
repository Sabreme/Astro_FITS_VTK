#ifndef COUNTDIALOG_H
#define COUNTDIALOG_H

#include <QDialog>
#include <QTimer>
#include <iostream>

namespace Ui {
class countDialog;
}

class countDialog : public QDialog
{
    Q_OBJECT

public:
    explicit countDialog(QWidget *parent = 0);
    ~countDialog();

public slots:
    void updateClock();

signals:
    void stopTest();


private:
    Ui::countDialog *ui;
    QTimer *timer ;


};

#endif // COUNTDIALOG_H
