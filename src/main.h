#ifndef MAIN_H
#define MAIN_H
#include "mainwindow.h"
#include "System/vtkFrameRateWidget.h"

#endif // MAIN_H

void myMsgHandler(QtMsgType, const char * message);

void loadFrameRate(vtkFrameRateWidget* frameRateWidget);

void reloadFrameRate();

void frameRateToggle(int show);

int main(int argc, char *argv[]);


