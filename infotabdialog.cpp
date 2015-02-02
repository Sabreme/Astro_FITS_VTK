#include "infotabdialog.h"
#include "ui_infotabdialog.h"
#include "iostream"

InfoTabDialog::InfoTabDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoTabDialog)
{
    ui->setupUi(this);

    Ui::InfoTabDialog *ui;

//    connect(this->ui->checkBoxOpen, SIGNAL(clicked()), this->parent(), SLOT());
    this->opened = true;
}

//InfoTabDialog::~InfoTabDialog()
//{

//    //std::cout <<  "CLosing WIndow" << std::endl;

//    delete ui;
//     emit this->valueChanged(true);
//}

bool InfoTabDialog::dialogClosing()
{
    on_checkBoxOpen_clicked();
    return this->opened;
}



void InfoTabDialog::on_checkBoxOpen_clicked()
{
    this->opened = this->ui->checkBoxOpen->isChecked();
    emit this->valueChanged(true);
}

void InfoTabDialog::on_InfoTabDialog_destroyed(QObject *arg1)
{
    std::cout << "Destroyed" << std::endl;
    emit this->valueChanged(true);
}

void InfoTabDialog::on_InfoTabDialog_destroyed()
{
    emit this->valueChanged(true);
}

void InfoTabDialog::on_buttonClose_clicked()
{
    this->close();
}
