#include "usertestdialog.h"
#include "ui_usertestdialog.h"

UserTestDialog::UserTestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserTestDialog)
{  
     ui->setupUi(this);
     this->ui->timeEdit->setDisplayFormat("mm:ss");
     this->ui->timeEdit->setTime(QTime(0,0,0,0));
     timer =  new QTimer(this);
    connect (timer, SIGNAL(timeout()), this, SLOT(updateCaption()));
     connect(ui->btnStop, SIGNAL(clicked()),this,SIGNAL(stopTest()));

    timer->start(1000);

    /// Reset the Timer to ZERO
    this->ui->timeEdit->setTime(QTime(0,0,0,0));    
}

UserTestDialog::~UserTestDialog()
{
    delete ui;
}

void UserTestDialog::updateCaption()
{
   this->ui->timeEdit->setTime(this->ui->timeEdit->time().addSecs(1));
}
