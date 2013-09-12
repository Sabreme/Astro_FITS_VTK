#ifndef SUBVOLUME_CPP

#include "ui_subVolume.h"
#include "subVolume.h"
#define SUBVOLUME_CPP


SubVolume::SubVolume(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SubVolume)
{
    ui->setupUi(this);

    //ui->button_Open-

    //loadFitsFile("OMC.FITS");
}

SubVolume::~SubVolume()
{
    delete ui;
}

#endif // SUBVOLUME_CPP
