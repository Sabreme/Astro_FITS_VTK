#include <QtGui/QApplication>
#include "main.h"

#include <iostream>
#include <QDebug>
#include <QObject>
#include <QThread>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkRendererCollection.h>

MainWindow * _mainWindow;
vtkFrameRateWidget* _vtkFrameRateWidget;

QString backlog;

void myMsgHandler(QtMsgType, const char * message)
{
    //std::cerr << message;
    if(_mainWindow != NULL) {
        if (backlog.size() != 0){
            backlog += message;
            _mainWindow->log(backlog);
        }
        else
        {
            _mainWindow->log(message);
        }
    }
    else {
        //std::cerr << message;
        backlog += message;
    }
}

void loadFrameRate(vtkFrameRateWidget* frameRateWidget)
{
    frameRateWidget->SetInteractor(_mainWindow->vtkWidget->GetInteractor());
    frameRateWidget->SetRenderer(_mainWindow->vtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
    frameRateWidget->SelectableOff();
    frameRateWidget->Init();
    //frameRateWidget->On();

}

void reloadFrameRate()
{
    loadFrameRate(_vtkFrameRateWidget);
}

int main(int argc, char *argv[])
{
    qInstallMsgHandler(myMsgHandler);
    QApplication a(argc, argv);
    MainWindow astroFitsVTK;
    _mainWindow = &astroFitsVTK;

    /// Frame Rate Calculation
    vtkSmartPointer<vtkFrameRateWidget> frameRateWidget =
            vtkSmartPointer<vtkFrameRateWidget>::New();

    _vtkFrameRateWidget = frameRateWidget;
    //loadFrameRate(frameRateWidget, _mainWindow);

    qDebug();
    astroFitsVTK.show();

    return a.exec();
}


void frameRateToggle(int show)
{
    _vtkFrameRateWidget->SetEnabled(show);    
}
