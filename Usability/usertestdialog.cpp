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
     connect (timer, SIGNAL(timeout()), this, SLOT(updateClock()));
     connect(ui->btnStop, SIGNAL(clicked()),this,SIGNAL(stopTest()));
     connect(ui->btnStop,SIGNAL(clicked()), this, SLOT(collectResults()));
     connect(ui->buttonSaveScreen, SIGNAL(clicked()),this,SIGNAL(saveScreen()));

     ui->buttonSaveScreen->setVisible(false);

    timer->start(1000);

    /// Reset the Timer to ZERO
    this->ui->timeEdit->setTime(QTime(0,0,0,0));
    this->ui->timeEdit->setVisible(false);
}

UserTestDialog::~UserTestDialog()
{
    delete ui;
}

void UserTestDialog::setTaskLabel(QString task)
{
    this->ui->taskLabel->setText(task);
}

void UserTestDialog::updateClock()
{
    this->ui->timeEdit->setTime(this->ui->timeEdit->time().addSecs(1));    
}

void UserTestDialog::updateRotation()
{
    this->countRotation++;
    //  std::cout << "Rotation Counter: " << countRotation << std::endl;
}

void UserTestDialog::setCurrentJob(int job)
{
    this->currentJob = job;
}

void UserTestDialog::setCurrentPrototype(int protoType)
{
    this->currentPrototype = protoType;
}

void UserTestDialog::collectResults()
{
    std::cout     << "Rotations: " << countRotate << ", "
                  << "Translations: " << countTranslate  << ", "
                  << "Scaling: " << countScale << ", "
                  << "ZRotations: " << countZRotate << ", "
                  << "SubVolResize: " << countSubVolResize << ","
                  << "Resets: " << countResets << ","
                  << "SubVolReset: " << countSubVolReset << ","
                  << "SliceResize:" << countSliceResize << ","
                  << "SliceReset:" << countSliceReset << ",'"
                  << "SubVolPntLeft:" << countSubVolPnt1 << ","
                  << "SubVolPntRight:" << countSubVolPnt2 << ","
                 << std::endl;
}

void UserTestDialog::incRotation()
{
    this->countRotate++;
    //   std::cout << "rotation: " << countRotate;
}

void UserTestDialog::incZRotation()
{
    this->countZRotate++;
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

void UserTestDialog::incSubVolPoint1()
{
    this->countSubVolPnt1++;
}

void UserTestDialog::incSubVolPoint2()
{
    this->countSubVolPnt2++;
}

void UserTestDialog::incSliceReSize()
{
    this->countSliceResize++;
}

void UserTestDialog::incSliceReset()
{
    this->countSliceReset++;
}

QString UserTestDialog::getUserResults()
{
    QString result;
//    std::cout   << "Rotations: " << countRotate << ", "
//                  << "Translations: " << countTranslate  << ", "
//                  << "Scaling: " << countScale << ", "
//                  << "SubVolResize: " << countSubVolResize << ","
//                  << "Resets: " << countResets << ","
//                  << "SubVolReset: " << countSubVolReset << ","
//                  << "SliceResize:" << countSliceResize << ","
//                  << "SliceReset:" << countSliceReset << ",'"
//                  << "SubVolPntLeft:" << countSubVolPntLeft << ","
//                  << "SubVolPntRight:" << countSubVolPntRight << ","
//                 << std::endl;


    switch(currentJob)
    {
    case 0 : result = QString("rotation=%1 ,translation=%2 ,scale=%3, Zrotation=%4, resets=%5")
                                .arg(countRotate)
                                .arg(countTranslate)
                                .arg(countScale)
                                .arg(countZRotate)
                                .arg(countResets); break;
    case 1 :
        {
            if (currentPrototype == 1)
            {
                result = QString("rotation=%1 ,translation=%2 ,scale=%3, ZRotation=%4, resets=%5, subVolResize=%6, subVolReset%7")
                                    .arg(countRotate)
                                    .arg(countTranslate)
                                    .arg(countScale)
                                    .arg(countZRotate)
                                    .arg(countResets)
                                    .arg(countSubVolResize)
                                    .arg(countSubVolReset);
            }
            else
            {
                result = QString("rotation=%1 ,translation=%2 ,scale=%3, ZRotation=%4, resets=%5, subVolPnt1=%6, subVolPnt2=%7, subVolReset%8")
                                .arg(countRotate)
                                .arg(countTranslate)
                                .arg(countScale)
                                .arg(countZRotate)
                                .arg(countResets)
                                .arg(countSubVolPnt1)
                                .arg(countSubVolPnt2)
                                .arg(countSubVolReset);
            }
            break;
        }
    case 2 :
        {
        result = QString("rotation=%1 ,translation=%2 ,scale=%3, ZRotation=%4, resets=%5, sliceResize=%6 ,sliceReset=%7")
                                    .arg(countRotate).arg(countTranslate).arg(countScale).arg(countZRotate).arg(countResets).arg(countSliceResize).arg(countSliceReset); break;
        }
    }

//    if (countSubVolPnt1 > 0)
//        result += QString(" ,subVolPoint1=%1").arg(countSubVolPnt1);

//    if (countSubVolPnt2 > 0)
//        result += QString(" ,subVolPoint2=%1").arg(countSubVolPnt2);



    return result;
}


