#include "surveydialog.h"
#include "ui_surveydialog.h"
#include <iostream>

SurveyDialog::SurveyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SurveyDialog)
{
    ui->setupUi(this);
    mapSignals();
}

SurveyDialog::~SurveyDialog()
{
    delete ui;
}

QString SurveyDialog::getUserResults()
{
    QString results = "Scores=[";

    for (int i = 0; i < 10; i++ )
    {
        if (i == 9)
         results += QString (" %1 ").arg(userAnswers[i]);
        else
         results += QString (" %1 ;").arg(userAnswers[i]);
    }

    results += QString("],SUS: %1").arg(userScore) ;

    return results;
}

void SurveyDialog::on_buttonSubmit_clicked()
{
    userScore = calculateScore();

    saveSurvey();
}


void SurveyDialog::selectionMade(QAbstractButton *radioButton)
{
    QString name = radioButton->objectName();
    ///std::cout << radioButton->objectName().toStdString() << std::endl;

#ifdef __APPLE__
    char question = name.at(5).toLatin1();
    char answer = name.at(9).toLatin1();
#else
    char question = name.at(5).toAscii();
    char answer = name.at(9).toAscii();
#endif
    answerQuestion(question);

    addResult(question, answer);

    if (checkComplete())
    {
        ui->buttonSubmit->setEnabled(true);
    }

}

void SurveyDialog::selectionToValue(QString buttonName)
{

}

void SurveyDialog::answerQuestion(char question)
{
    QString style = "QLabel {  padding : 2px; border : 4px solid white; border-radius : 4px; text-decoration: line-through;}";
    switch (question)
    {
        case  '1': ui->label_No01->setStyleSheet(style); break;
        case  '2': ui->label_No02->setStyleSheet(style); break;
        case  '3': ui->label_No03->setStyleSheet(style); break;
        case  '4': ui->label_No04->setStyleSheet(style); break;
        case  '5': ui->label_No05->setStyleSheet(style); break;
        case  '6': ui->label_No06->setStyleSheet(style); break;
        case  '7': ui->label_No07->setStyleSheet(style); break;
        case  '8': ui->label_No08->setStyleSheet(style); break;
        case  '9': ui->label_No09->setStyleSheet(style); break;
        case  'D': ui->label_No10->setStyleSheet(style); break;

        default :
            break;
    }
}

void SurveyDialog::addResult(char question, char answer)
{
    int position = (question - '0') - 1;    /// ASCII conversion and Array Index 0

    if (question =='D')
        position = 9;

    switch (answer)
    {
        case  '1': userAnswers[position] = 1; break;
        case  '2': userAnswers[position] = 2; break;
        case  '3': userAnswers[position] = 3; break;
        case  '4': userAnswers[position] = 4; break;
        case  '5': userAnswers[position] = 5; break;

        default :
            break;
    }
}

bool SurveyDialog::checkComplete()
{
    bool value = true;

    for (int i = 0; i < 10 ; i ++)
    {
        if (userAnswers[i] == 0 || userAnswers[i] > 6)
        {
            value = false;
        }
    }
    return value;
}

int SurveyDialog::calculateScore()
{
    int sum = 0;

    for (int i = 1; i < 11 ; i++)
    {
        if ((i%2) == 0) // EVEN NUMBERS = 5 - Answer
            sum += 5- userAnswers[i-1];
        else            // ODD NUMBERS = Answer  - 1
            sum += userAnswers[i-1] - 1;
    }
    return sum * 2.5;
}


void SurveyDialog::mapSignals()
{

    group1 = new QButtonGroup(ui->frame_Choice1);
    group1->addButton(ui->quest1Ans1);
    group1->addButton(ui->quest1Ans2);
    group1->addButton(ui->quest1Ans3);
    group1->addButton(ui->quest1Ans4);
    group1->addButton(ui->quest1Ans5);

    group2 = new QButtonGroup(ui->frame_Choice2);
    group2->addButton(ui->quest2Ans1);
    group2->addButton(ui->quest2Ans2);
    group2->addButton(ui->quest2Ans3);
    group2->addButton(ui->quest2Ans4);
    group2->addButton(ui->quest2Ans5);

    group3 = new QButtonGroup(ui->frame_Choice3);
    group3->addButton(ui->quest3Ans1);
    group3->addButton(ui->quest3Ans2);
    group3->addButton(ui->quest3Ans3);
    group3->addButton(ui->quest3Ans4);
    group3->addButton(ui->quest3Ans5);

    group4 = new QButtonGroup(ui->frame_Choice4);
    group4->addButton(ui->quest4Ans1);
    group4->addButton(ui->quest4Ans2);
    group4->addButton(ui->quest4Ans3);
    group4->addButton(ui->quest4Ans4);
    group4->addButton(ui->quest4Ans5);

    group5 = new QButtonGroup(ui->frame_Choice5);
    group5->addButton(ui->quest5Ans1);
    group5->addButton(ui->quest5Ans2);
    group5->addButton(ui->quest5Ans3);
    group5->addButton(ui->quest5Ans4);
    group5->addButton(ui->quest5Ans5);

    group6 = new QButtonGroup(ui->frame_Choice6);
    group6->addButton(ui->quest6Ans1);
    group6->addButton(ui->quest6Ans2);
    group6->addButton(ui->quest6Ans3);
    group6->addButton(ui->quest6Ans4);
    group6->addButton(ui->quest6Ans5);

    group7 = new QButtonGroup(ui->frame_Choice7);
    group7->addButton(ui->quest7Ans1);
    group7->addButton(ui->quest7Ans2);
    group7->addButton(ui->quest7Ans3);
    group7->addButton(ui->quest7Ans4);
    group7->addButton(ui->quest7Ans5);

    group8 = new QButtonGroup(ui->frame_Choice8);
    group8->addButton(ui->quest8Ans1);
    group8->addButton(ui->quest8Ans2);
    group8->addButton(ui->quest8Ans3);
    group8->addButton(ui->quest8Ans4);
    group8->addButton(ui->quest8Ans5);

    group9 = new QButtonGroup(ui->frame_Choice9);
    group9->addButton(ui->quest9Ans1);
    group9->addButton(ui->quest9Ans2);
    group9->addButton(ui->quest9Ans3);
    group9->addButton(ui->quest9Ans4);
    group9->addButton(ui->quest9Ans5);

    groupD = new QButtonGroup(ui->frame_ChoiceD);
    groupD->addButton(ui->questDAns1);
    groupD->addButton(ui->questDAns2);
    groupD->addButton(ui->questDAns3);
    groupD->addButton(ui->questDAns4);
    groupD->addButton(ui->questDAns5);

    QObject::connect(group1,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(selectionMade(QAbstractButton*)));
    QObject::connect(group2,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(selectionMade(QAbstractButton*)));
    QObject::connect(group3,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(selectionMade(QAbstractButton*)));
    QObject::connect(group4,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(selectionMade(QAbstractButton*)));
    QObject::connect(group5,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(selectionMade(QAbstractButton*)));
    QObject::connect(group6,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(selectionMade(QAbstractButton*)));
    QObject::connect(group7,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(selectionMade(QAbstractButton*)));
    QObject::connect(group8,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(selectionMade(QAbstractButton*)));
    QObject::connect(group9,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(selectionMade(QAbstractButton*)));
    QObject::connect(groupD,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(selectionMade(QAbstractButton*)));
}
