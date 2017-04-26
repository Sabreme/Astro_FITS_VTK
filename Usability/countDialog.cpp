#include "countDialog.h"
#include "ui_countdialog.h"

countDialog::countDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::countDialog)
{
    ui->setupUi(this);
    this->ui->timeEdit->setDisplayFormat("mm:ss");
    this->ui->timeEdit->setTime(QTime(0,0,0,0));
    timer =  new QTimer(this);
    connect (timer, SIGNAL(timeout()), this, SLOT(updateClock()));
    connect(ui->btnStop, SIGNAL(clicked()),this,SIGNAL(stopTest()));

   timer->start(1000);

   /// Reset the Timer to 5 Mins
   this->ui->timeEdit->setTime(QTime(0,5,0,0));   
}

countDialog::~countDialog()
{
    delete ui;
}

void countDialog::updateClock()
{
this->ui->timeEdit->setTime(this->ui->timeEdit->time().addSecs(-1));
}
