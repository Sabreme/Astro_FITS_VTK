#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolume16Reader.h>
#include <vtkPolyDataMapper.h>
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

#include <vtkOrientationMarkerWidget.h>
#include <vtkAxesActor.h>
#include <vtkPropAssembly.h>
#include <vtkExtractVOI.h>

#include <QMainWindow>
#include <QApplication>
#include <QVTKWidget.h>

#include "subvolumedialog.h"
#include "subVolume.h"


namespace Ui {
    class MainWindow;    
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void log(QString message);    

    void loadFitsFile(QString filename);

    void loadSubVolume(QVTKWidget *qvtkWidget, vtkFitsReader *source );

public slots:
    virtual void slotExit();

protected:
        void ModifiedHandler();

private slots:
        void on_button_Exit_clicked();

        void on_button_Open_clicked();

        void mySlot();

        void on_Buttom_SubVol_clicked();        

        void on_actionSubVolume_triggered();

        void on_actionOpen_triggered();

        void on_actionExit_triggered();

        void on_actionReset_Camera_triggered();

private:
    Ui::MainWindow *ui;                     // Global pointer for the Mainwindow
    SubVolumeDialog *subVolDialog;          // Global pointer for the SubVolume

    vtkFitsReader *global_Reader;           // Global Pointer for the loaded FitsReader
    vtkStructuredPoints *global_Points;     // Global Pointer for the point Dataset
};

#endif // MAINWINDOW_H
