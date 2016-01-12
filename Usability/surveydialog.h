#ifndef SURVEYDIALOG_H
#define SURVEYDIALOG_H

#include <QDialog>
#include <QFrame>
#include <QCheckBox>
#include <QSignalMapper>
#include <QButtonGroup>


namespace Ui {
class SurveyDialog;
}

class SurveyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SurveyDialog(QWidget *parent = 0);
    ~SurveyDialog();

    QString getUserResults();


private slots:
    void on_buttonSubmit_clicked();

    //void selectionMade(QString boxName);
    void selectionMade (QAbstractButton *radioButton);

    void selectionToValue(QString buttonName);

    void answerQuestion(char question);

    void addResult(char question, char answer);

    bool checkComplete();

    int calculateScore();





    void mapSignals();

signals:
    void  saveSurvey();


private:
    Ui::SurveyDialog *ui;

    QSignalMapper * mapper;
    QButtonGroup * group1, * group2, *group3, *group4, *group5;
    QButtonGroup * group6, * group7, *group8, *group9, *groupD;

    int userAnswers[10];

    int userScore =0;
};

#endif // SURVEYDIALOG_H
