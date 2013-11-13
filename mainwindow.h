#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolume16Reader.h>
#include <vtkPolyDataMapper.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkActor.h>
#include <vtkOutlineFilter.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkPolyDataNormals.h>
#include <vtkContourFilter.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkCubeSource.h>
#include <vtkOutlineFilter.h>
#include <vtkDecimatePro.h>

#include <vtkfitsreader.h>
#include <vtkMarchingCubes.h>
#include <vtkSliceCubes.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkStructuredPoints.h>
#include <vtkActor.h>
#include <vtkLookupTable.h>

#include <vtkOrientationMarkerWidget.h>
#include <vtkAxesActor.h>
#include <vtkPropAssembly.h>
#include <vtkExtractVOI.h>

#include <QMainWindow>
#include <QApplication>
#include <QVTKWidget.h>

#include <QProgressDialog>
#include <QTime>

#include "subvolumedialog.h"
#include "subVolume.h"

#include <QFutureWatcher>
#include <QtGui>
#include "processfile.h"
#include "time.h"
#include <QThread>

namespace Ui {
    class MainWindow;    
}

class Thread :public QThread{
public:
    Thread(QObject *parent = 0) : QThread(parent)
    {

    }

protected:
    void run(){
        exec();        
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void log(QString message);    

    void loadFitsFile(QString filename);

    void loadSubVolume(QVTKWidget *qvtkWidget, vtkFitsReader *source );

//    void sleep(timer_t delay);

//    void LongFunction(int total);

    QVTKWidget* vtkWidget;      // Used for Main loop for FrameRate Calculation


public slots:
    virtual void slotExit();

protected:
        void ModifiedHandler();

private slots:
        void updateme();
        void button_Exit_clicked();

        void button_Open_clicked();

        void mySlot();

        void Buttom_SubVol_clicked();        

        void on_actionOpen_triggered();

        void on_actionExit_triggered();

        void on_actionReset_Camera_triggered();

        void on_actionLeft_Side_View_triggered();

        void on_actionRight_Side_View_triggered();

        void on_actionTop_Side_View_triggered();

        void on_actionBottom_Side_View_triggered();

        void on_actionSliceSelection_triggered();

        void on_actionBlack_White_triggered();

        void actionDefault_triggered();

        void actionBlue_Red_triggered();

        void slot_LoadFile();

        void slot_Load_Finished();        

        void on_actionSubVolumeXY_triggered();

        void on_action_SubVolume_Z_triggered();

        void on_actionPreview_triggered();

        void on_actionSubVol_Export_triggered();

private:

    SubVolumeDialog *subVolDialog;          // Global pointer for the SubVolume
    Ui::MainWindow *ui;                     // Global pointer for the Mainwindow

    vtkFitsReader *global_Reader;           // Global Pointer for the loaded FitsReader
    vtkStructuredPoints *global_Points;     // Global Pointer for the point Dataset
    vtkVolume * global_Volume;               // Global Pointer for the Current Volume
    vtkCubeSource *global_subVolume;              // Global Pointer for the Sub-Volume

    vtkLookupTable * default_Volume_Colours; // Global Pointer for the Default Volume schema

    QTime Timer;
    QProgressDialog* ProgressDialog;
    QFutureWatcher<void> FutureWatcher;

    //ProcessFile ProcessFileObject;

};

#endif // MAINWINDOW_H
