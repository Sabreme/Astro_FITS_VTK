#include "UserTesting.h"
#include "ui_UserTesting.h"
#include "QDebug"
#include "QStringBuilder"
#include "QStringList"
#include "QTextStream"
#include "QFile"
#include "QTextStream"
#include "QMessageBox"
#include <iostream>

UserTesting::UserTesting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserTesting)
{
    ui->setupUi(this);
    this->ui->timeEdit->setDisplayFormat("mm:ss");
    this->ui->timeEdit->setTime(QTime(0,0,0,0));
    timer =  new QTimer(this);

    QButtonGroup * groupButtons = new QButtonGroup(this->ui->frameButtons);
    groupButtons->addButton(this->ui->btnStart);
    groupButtons->addButton(this->ui->btnStop);
    groupButtons->addButton(this->ui->btnDone);
    groupButtons->setExclusive(true);

    connect(ui->btnStart, SIGNAL(clicked()),this,SIGNAL(startTest()));

}

UserTesting::~UserTesting()
{
    delete ui;
}

void UserTesting::testingMode(bool testing)
{
    ///We set the frames off when we are testing and leave the stop button active
    this->ui->frame_DEVICES->setEnabled(!testing);
    this->ui->frame_TASKS->setEnabled(!testing);
}


void UserTesting::on_btnStart_clicked()
{

    connect (timer, SIGNAL(timeout()), this, SLOT(updateCaption()));

    timer->start(1000);

    /// Reset the Timer to ZERO
    this->ui->timeEdit->setTime(QTime(0,0,0,0));

    /// Dissable the various buttons and Panels
    testingMode(true);

    initiateTest(this->ui->spinBoxUSERID->value() -1);
    SetCurrentTask(this->ui->spinBoxUSERID->value() -1);

    this->hide();
}

void UserTesting::updateCaption()
{
    this->ui->timeEdit->setTime(this->ui->timeEdit->time().addSecs(1));
}

void UserTesting::on_btnStop_clicked()
{
    timer->stop();
    testingMode(false);



    //printResult(" and Task Open");
    //printResult(" and Task Stopped");

}

void UserTesting::on_btnDone_clicked()
{    
    testingMode(false);

    SetCurrentPrototype(this->ui->spinBoxUSERID->value()-1);
}

void UserTesting::importSystemResults(QString results)
{
    QString task;
    QString medium;
    QString taskNo;

    ///////    METHOD RADIO BUTTONS   /////////////
    if (this->ui->radioLeap->isChecked() )
        medium = "Leap";
    if (this->ui->radioMouse->isChecked() )
        medium = "Mouse";
    if (this->ui->radioTouch->isChecked() )
        medium = "Touch";

    ///////    TASK RADIO BUTTONS   /////////////
    if (this->ui->radioTask1->isChecked() )
        taskNo = "1";
    if (this->ui->radioTask2->isChecked() )
        taskNo = "2";
    if (this->ui->radioTask3->isChecked() )
        taskNo = "3";

    int seconds = this->ui->timeEdit->time().second();
    int minutes = this->ui->timeEdit->time().minute();

    task = GetCurrentTask(this->ui->spinBoxUSERID->value() - 1);

    int userID = this->ui->spinBoxUSERID->value();
    int sessionID = userID / 3 ;
    if (userID % 3 != 0)
        sessionID++;
    QString result =    QString("UserID: %1 ,TaskNo: %2 ,Task: %3 ,Prototype: %4 ,Time: %5:%6 ," + results % "\n")
                                .arg(userID).arg(taskNo).arg(task).arg(medium).arg(minutes).arg(seconds);

    /////////////////////////////////////////////////////
    ///////////////////////
    /// SAVE RESULT FOR SYSTEM RESULTS
    ///
    ///
    /// ////////////////////////////////////////////////
#ifdef __WIN32__
    QDir dir("F:/Projects/build-Astro_FITS_VTK-Desktop-Debug");
    QString userIDString = QString("UserTesting/UserID_%1").arg(userID);
    dir.mkpath(userIDString);
    if (!dir.exists())
        dir.mkpath(".");
    QString outputDir = dir.absolutePath() + "/" + userIDString + "/";
#elif __linux__
        QDir dir(QDir::homePath());
        QString userIDString = QString("UserTesting/UserID_%1").arg(userID);
        dir.mkpath(userIDString);
        if (!dir.exists())
            dir.mkpath(".");
        QString outputDir = dir.absolutePath() + "/" + userIDString + "/";
#endif

     QString fileName = QString("UserID_%1_Task_%2_%3_SYSTEM.txt").arg(userID).arg(taskNo).arg(medium);

    QString output = outputDir +  fileName;


    QFile file(output);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << result;
    file.close();
}

void UserTesting::importUserResults(QString results)
{
    {
        QString task;
        QString medium;
        QString taskNo;

        ///////    METHOD RADIO BUTTONS   /////////////
        if (this->ui->radioLeap->isChecked() )
            medium = "Leap";
        if (this->ui->radioMouse->isChecked() )
            medium = "Mouse";
        if (this->ui->radioTouch->isChecked() )
            medium = "Touch";

        ///////    TASK RADIO BUTTONS   /////////////
        if (this->ui->radioTask1->isChecked() )
            taskNo = "1";
        if (this->ui->radioTask2->isChecked() )
            taskNo = "2";
        if (this->ui->radioTask3->isChecked() )
            taskNo = "3";

        int seconds = this->ui->timeEdit->time().second();
        int minutes = this->ui->timeEdit->time().minute();

        task = GetCurrentTask(this->ui->spinBoxUSERID->value() - 1);

        int userID = this->ui->spinBoxUSERID->value();
        int sessionID = userID / 3 ;
        if (userID % 3 != 0)
            sessionID++;
        QString result =    QString("UserID: %1 ,TaskNo: %2 ,Task: %3 ,Prototype: %4 ,Time: %5:%6 ," + results % "\n")
                                    .arg(userID).arg(taskNo).arg(task).arg(medium).arg(minutes).arg(seconds);

        /////////////////////////////////////////////////////
        ///////////////////////
        /// SAVE RESULT FOR SYSTEM RESULTS
        ///
        ///
#ifdef __WIN32__
    QDir dir("F:/Projects/build-Astro_FITS_VTK-Desktop-Debug");
    QString userIDString = QString("UserTesting/UserID_%1").arg(userID);
    dir.mkpath(userIDString);
    if (!dir.exists())
        dir.mkpath(".");
    QString outputDir = dir.absolutePath() + "/" + userIDString + "/";
#elif __linux__
        QDir dir(QDir::homePath());
        QString userIDString = QString("UserTesting/UserID_%1").arg(userID);
        dir.mkpath(userIDString);
        if (!dir.exists())
            dir.mkpath(".");
        QString outputDir = dir.absolutePath() + "/" + userIDString + "/";
#endif

     QString fileName = QString("UserID_%1_Task_%2_%3_USER.txt").arg(userID).arg(taskNo).arg(medium);

    QString output = outputDir +  fileName;

        QFile file(output);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << result;
        file.close();
    }
}

void UserTesting::importSurveyResults(QString results)
{
    QString task;
    QString medium;
    QString taskNo;

    ///////    METHOD RADIO BUTTONS   /////////////
    if (this->ui->radioLeap->isChecked() )
        medium = "Leap";
    if (this->ui->radioMouse->isChecked() )
        medium = "Mouse";
    if (this->ui->radioTouch->isChecked() )
        medium = "Touch";

    ///////    TASK RADIO BUTTONS   /////////////
    if (this->ui->radioTask1->isChecked() )
        taskNo = "1";
    if (this->ui->radioTask2->isChecked() )
        taskNo = "2";
    if (this->ui->radioTask3->isChecked() )
        taskNo = "3";

//    int seconds = this->ui->timeEdit->time().second();
//    int minutes = this->ui->timeEdit->time().minute();

    task = GetCurrentTask(this->ui->spinBoxUSERID->value() - 1);

    int userID = this->ui->spinBoxUSERID->value();
    int sessionID = userID / 3 ;
    if (userID % 3 != 0)
        sessionID++;
    QString result =    QString("UserID: %1 ,TaskNo: %2 ,Task: %3 ,Prototype: %4 ," + results % "\n")
                                .arg(userID).arg(taskNo).arg(task).arg(medium);

    /////////////////////////////////////////////////////
    ///////////////////////
    /// SAVE RESULT FOR SURVEY
    ///
    ///
#ifdef __WIN32__
QDir dir("F:/Projects/build-Astro_FITS_VTK-Desktop-Debug");
QString userIDString = QString("UserTesting/UserID_%1").arg(userID);
dir.mkpath(userIDString);
if (!dir.exists())
    dir.mkpath(".");
QString outputDir = dir.absolutePath() + "/" + userIDString + "/";
#elif __linux__
    QDir dir(QDir::homePath());
    QString userIDString = QString("UserTesting/UserID_%1").arg(userID);
    dir.mkpath(userIDString);
    if (!dir.exists())
        dir.mkpath(".");
    QString outputDir = dir.absolutePath() + "/" + userIDString + "/";
#endif

 QString fileName = QString("UserID_%1_Task_%2_%3_Survey.txt").arg(userID).arg(taskNo).arg(medium);

QString output = outputDir +  fileName;


    QFile file(output);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << result;
    file.close();
}

void UserTesting::loadCounterBalance(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Load Error");
        msgBox.setText("File Not Found !!!");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    else
    {
        QTextStream in(&file);                
        QString line = in.readLine();
        int lineNo = 0;
        while (!line.isNull())
        {
            stringConfig[lineNo] = line;

            SplitAndAdd(line,lineNo);

            line = in.readLine();
            lineNo++;
        }
    }
}

void UserTesting::SplitAndAdd(QString line, int lineNumber)
{
    QStringList tokens ;

    tokens = line.split(",");

    for (int index = 0; index < tokens.length(); index++)
    {
        counterBalance[lineNumber][index] = tokens.at(index);
        ///this->ui->plainTextEdit_Debug->insertPlainText(tokens.at(index));
    }
    QString lineNo = QString("%1").arg(lineNumber);
    QString tokenSize = QString("%1").arg(tokens.length());


    ///this->ui->plainTextEdit_Debug->insertPlainText("\n");
    tokens.clear();

}

///
/// \brief MainWindow::initiateTest
/// \param sessionID
/// \param userNumber
///  1. We look for the row with both the session ID and User ID,
///     Using an Optimised search parameter because the counterbalance
///     file is organised into Sessions in groups of 3 and users in order 1,2,3
///  2. We then extract the ordering of Tasks to a Local collection
///  3. We then Extract the ordering of Prototypes to a local collection
void UserTesting::initiateTest(int userID)
{
    QString task ;

    if (this->ui->radioTask1->isChecked() )
        task = convertTask(counterBalance[userID][one]);
    if (this->ui->radioTask2->isChecked() )
        task = convertTask(counterBalance[userID][two]);
    if (this->ui->radioTask3->isChecked() )
        task = convertTask(counterBalance[userID][three]);
    ///    this->ui->plainTextEdit_Debug->insertPlainText("Task = " + task );
}

int UserTesting::getCurrentJob()
{
    return this->currentJob;
}

int UserTesting::getCurrentTask()
{
    return this->currentTask;
}

int UserTesting::getCurrentPrototype()
{
    return this->currentPrototype;
}

void UserTesting::PrintConfig()
{
    QString output, word ;

    for (int line = 0; line < 3; line++)
    {
        output = "";
        for (int pos=0; pos < 7; pos++)
        {
            word = counterBalance[line][pos];
           switch (pos)
           {
               case 0: output += "User " + word ;
                   break;               
               case 1: output += " does [" + convertPrototype(word) ;
                   break;
               case 2: output += "] -> [ " + convertPrototype(word) ;
                   break;
               case 3: output += "] -> [ " + convertPrototype(word) ;
                   break;
               case 4: output += "] With Tasks [" + convertTask(word) ;
                   break;
               case 5: output += "] -> [ " + convertTask(word) ;
                   break;
               case 6: output += "] -> [ " + convertTask(word) ;
                   break;          
           }
        }
         output += "\n";         
    }
}

void UserTesting::printResult(QString status)
{
    QString task;
    QString medium;
    QString taskNo;

    ///////    METHOD RADIO BUTTONS   /////////////
    if (this->ui->radioLeap->isChecked() )
        medium = "Leap";
    if (this->ui->radioMouse->isChecked() )
        medium = "Mouse";
    if (this->ui->radioTouch->isChecked() )
        medium = "Touch";

    ///////    TASK RADIO BUTTONS   /////////////
    if (this->ui->radioTask1->isChecked() )
        taskNo = "1";
    if (this->ui->radioTask2->isChecked() )
        taskNo = "2";
    if (this->ui->radioTask3->isChecked() )
        taskNo = "3";

    task = GetCurrentTask(this->ui->spinBoxUSERID->value() - 1);


    int seconds = this->ui->timeEdit->time().second();
    int minutes = this->ui->timeEdit->time().minute();

    int userID = this->ui->spinBoxUSERID->value();
    int sessionID = userID / 3 ;
    if (userID % 3 != 0)
        sessionID++;
    QString result =    QString("UserID: %1 ,TaskNo: %2 ,Task: %3, Prototype: %4 ,Time: %5:%6" + status % "\n")
                                .arg(userID).arg(taskNo).arg(task).arg(medium).arg(minutes).arg(seconds);

    saveResult(result);

}

void UserTesting::saveResult(QString result)
{
    QString taskNo;

    ///////    TASK RADIO BUTTONS   /////////////
    if (this->ui->radioTask1->isChecked() )
        taskNo = "1";
    if (this->ui->radioTask2->isChecked() )
        taskNo = "2";
    if (this->ui->radioTask3->isChecked() )
        taskNo = "3";

    int userID = this->ui->spinBoxUSERID->value();


#ifdef __WIN32__
        QString outputDir = QDir::currentPath();
#elif __linux__
        QDir dir ("/home/pmulumba/UserTesting");
        if (!dir.exists())
            dir.mkpath(".");
        QString outputDir = dir.absolutePath();
#endif
//    QString outputDir = QDir::currentPath();

    QString fileName = QString("TESTS/UserID_%1_Task_%2_.txt").arg(userID).arg(taskNo);

    QString output = outputDir + "/" + fileName;

//    QString fileName = QString("TESTS/UserID_%1_Task_%2.txt").arg(userID).arg(taskNo);

//    QString output = fileName;


    std::cout << output.toStdString() << endl;

    QFile file(output);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << result;
    file.close();
}

void UserTesting::on_btnCounterBal_clicked()
{
#ifdef __WIN32__
    loadCounterBalance("C:/UserTesting/counterbalance.txt");
#elif __linux__
    loadCounterBalance("counterbalance.txt");
#endif
}

void UserTesting::on_btnTest_clicked()
{

    SetCurrentPrototype(this->ui->spinBoxUSERID->value()-1);
}

QString UserTesting::convertTask(QString task)
{
    QString selectedTast = "";

    if (task.compare("a")==0)
            selectedTast =  "Transform";
    if (task.compare("b")==0)
             selectedTast = "Sub-Volume";
    if (task.compare("c")==0)
            selectedTast = "Slicing";

    return selectedTast;
}

QString UserTesting::convertPrototype(QString prototype)
{
    QString selectedPrototype = "";

    if (prototype.compare("L")==0)
            selectedPrototype = "Leap";
    if (prototype.compare("M")==0)
            selectedPrototype = "Mouse";
    if (prototype.compare("T")==0)
            selectedPrototype = "Touch";

    return selectedPrototype;

}

void UserTesting::SetCurrentTask(int userID)
{

    if (this->ui->radioTask1->isChecked() )
        currentTask = one;
    if (this->ui->radioTask2->isChecked() )
        currentTask = two;
    if (this->ui->radioTask3->isChecked() )
        currentTask = three;

    QString task = counterBalance[userID][currentTask];

    if (task.compare("a")==0)
            currentJob = Transformation;
    if (task.compare("b")==0)
             currentJob = SubVolume;
    if (task.compare("c")==0)
            currentJob = Slicing;
}

void UserTesting::SetCurrentPrototype(int userID)
{
    int part = this->ui->spinBoxPARTProto->value();

    if (part == 1)
        currentPrototype = first;

    if (part == 2)
        currentPrototype = second;

    if (part == 3)
        currentPrototype = third;

    QString prototype = counterBalance[userID][currentPrototype];

    if (prototype.compare("L")==0)
            this->ui->radioLeap->setChecked(true);
    if (prototype.compare("M")==0)
            this->ui->radioMouse->setChecked(true);
    if (prototype.compare("T")==0)
            this->ui->radioTouch->setChecked(true);
}

QString UserTesting::GetCurrentTask(int userID)
{
    QString task;

    switch (currentTask)
    {
        case one: task = convertTask(counterBalance[userID][one]);
        break;
        case two: task = convertTask(counterBalance[userID][two]);
        break;
        case three: task = convertTask(counterBalance[userID][three]);
        break;
    }
    return task;
}

QString UserTesting::GetCurrentPrototype(int userID)
{
    QString prototype;

    switch (currentPrototype)
    {
        case first: prototype = convertPrototype(counterBalance[userID][first]);
        break;
        case second: prototype = convertPrototype(counterBalance[userID][second]);
        break;
        case third: prototype = convertPrototype(counterBalance[userID][third]);
        break;
    }
    return prototype;
}

int UserTesting::GetCurrentUserID()
{
    return this->ui->spinBoxUSERID->value();
}

void UserTesting::extractProtOrder(QString line)
{
}


