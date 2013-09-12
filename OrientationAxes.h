#ifndef ORIENTATIONAXES_H
#define ORIENTATIONAXES_H
#include <cassert>
#include "QVTKWidget.h"
#include "vtkAxesActor.h"
#include "vtkOrientationMarkerWidget.h"
#include "vtkRenderWindowInteractor.h"

#endif // ORIENTATIONAXES_H


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
    marker->InteractiveOn();
}
