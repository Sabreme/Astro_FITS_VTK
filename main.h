#ifndef MAIN_H
#define MAIN_H
#include "vtkFrameRateWidget.h"
#include "mainwindow.h"
#endif // MAIN_H

void myMsgHandler(QtMsgType, const char * message);

void loadFrameRate(vtkFrameRateWidget* frameRateWidget, MainWindow* mainWindow);

int main(int argc, char *argv[]);

void reloadFrameRate();
