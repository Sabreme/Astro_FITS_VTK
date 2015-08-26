#ifndef USERTESTDIALOG_H
#define USERTESTDIALOG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class UserTestDialog;
}

class UserTestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserTestDialog(QWidget *parent = 0);
    ~UserTestDialog();

public slots:
    void updateCaption();

private:
    Ui::UserTestDialog *ui;
    QTimer *timer ;
};

#endif // USERTESTDIALOG_H
