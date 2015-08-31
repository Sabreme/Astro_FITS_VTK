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
     connect(ui->btnStop,SIGNAL(clicked()), this, SLOT(collectResults()));

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

void UserTestDialog::updateRotation()
{
    this->countRotation++;
  //  std::cout << "Rotation Counter: " << countRotation << std::endl;
}

void UserTestDialog::collectResults()
{
    std::cout   << "Rotations: " << countRotate << ", "
                  << "Translations: " << countTranslate  << ", "
                  << "Scaling: " << countScale << ", "
                  << "SubVolResize: " << countSubVolResize << ","
                  << "Resets: " << countResets << ","
                  << "SubVolReset: " << countSubVolReset << ","
                 << std::endl;
}

void UserTestDialog::incRotation()
{
    this->countRotate++;
 //   std::cout << "rotation: " << countRotate;
}

void UserTestDialog::incTranslation()
{
    this->countTranslate++;
 //   std::cout << "Translate: " << countTranslate;
}

void UserTestDialog::incScaling()
{
    this->countScale++;
    //   std::cout << "Scale: " << countScale;
}

void UserTestDialog::incReset()
{
    this->countResets++;
}

void UserTestDialog::incSubVolResize()
{
    this->countSubVolResize++;
}

void UserTestDialog::incSubVolReset()
{
    this->countSubVolReset++;
}


