#include "infobardialog.h"
#include "ui_InfoBarDialog.h"
#include <string.h>
#include <iostream>

InfoBarDialog::InfoBarDialog(QWidget *parent) :
    QDialog(parent),
    ui (new Ui::InfoBarDialog)
{
    ui->setupUi(this);
}

InfoBarDialog::~InfoBarDialog()
{
    delete ui;
}

void InfoBarDialog::UpdateFitsInfo(vtkFitsReader* reader)
{

    this->global_Reader = reader;

    this->ui->lineInfoObject->setText(QString("%1").arg(this->global_Reader->GetObjectSky()).trimmed());
    this->ui->lineInfoTelescope->setText(QString("%1").arg(this->global_Reader->GetTelescope()).trimmed());
    this->ui->lineInfoEpoch->setText(QString("%1").arg(this->global_Reader->GetEpoch()));

    this->ui->lineInfoNAxis->setText(QString("%1").arg(this->global_Reader->GetNAxis()));
    this->ui->lineInfoNAxis1->setText(QString("%1").arg(this->global_Reader->GetDimensions()[0]));
    this->ui->lineInfoNAxis2->setText(QString("%1").arg(this->global_Reader->GetDimensions()[1]));
    this->ui->lineInfoNAxis3->setText(QString("%1").arg(this->global_Reader->GetDimensions()[2]));

    this->ui->lineInfoPoints->setText(QString("%1").arg(this->global_Reader->GetPoints()));
    this->ui->lineInfoDataMin->setText(QString("%1").arg(this->global_Reader->GetDataMin()));
    this->ui->lineInfoDataMax->setText(QString("%1").arg(this->global_Reader->GetDataMax()));

    this->ui->lineInfoXAxis->setText(QString("%1").arg(this->global_Reader->GetXAxisInfo()).trimmed());
    this->ui->lineInfoYAxis->setText(QString("%1").arg(this->global_Reader->GetYAxisInfo()).trimmed());
    this->ui->lineInfoZAxis->setText(QString("%1").arg(this->global_Reader->GetZAxisInfo()).trimmed());
}

void InfoBarDialog::on_buttonClose_clicked()
{
    this->close();
}
