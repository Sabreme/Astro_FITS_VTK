#ifndef ORIENTATIONAXES_H
#define ORIENTATIONAXES_H
#include <cassert>
#include "QVTKWidget.h"
#include "vtkAxesActor.h"
#include "vtkOrientationMarkerWidget.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkPolyDataMapper.h"
#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "Leap/vtkLeapMarkerWidget.h"
#include "vtkArrowSource.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkActor.h"
#include "vtkPlaneWidget.h"
#include "vtkCubeSource.h"
#include "vtkProbeFilter.h"
#include "vtkTransform.h"

#endif // ORIENTATIONAXES_H

vtkPlaneWidget * leapDbgPlaneWidget;


void AddOrientationAxes(QVTKWidget *qvtkWidget)
{
    assert (qvtkWidget != 0);

    vtkRenderWindowInteractor* interactor = qvtkWidget->GetInteractor();

    vtkAxesActor * axesActor = vtkAxesActor::New();
    axesActor->SetNormalizedTipLength(0.4, 0.4, 0.4);
    axesActor->SetNormalizedShaftLength(0.6, 0.6, 0.6);
    axesActor->SetShaftTypeToCylinder();

    vtkOrientationMarkerWidget * marker = vtkOrientationMarkerWidget::New();
    marker->SetInteractor(interactor);
    marker->SetOrientationMarker(axesActor);
    marker->SetEnabled(true);
    //marker->InteractiveOn();
    marker->InteractiveOff();
}

void AddLeapMarkerWidget(QVTKWidget * qvtkWidget)
{
    assert (qvtkWidget != 0);

    vtkRenderWindowInteractor* interactor = qvtkWidget->GetInteractor();

    vtkLeapMarkerWidget * marker = vtkLeapMarkerWidget::New();
    marker->SetInteractor(interactor);
    marker->SetEnabled(true);
    //marker->InteractiveOn();
    marker->InteractiveOff();

    marker->GeneratActors();




}
