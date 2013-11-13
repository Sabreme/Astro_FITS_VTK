#ifndef PROCESSFILE_H
#define PROCESSFILE_H

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

class ProcessFile
{
public:
    ProcessFile();

    void loadFitsFile(QString & fileName);


};

#endif // PROCESSFILE_H
