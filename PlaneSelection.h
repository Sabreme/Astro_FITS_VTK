#ifndef PLANESELECTION_H
#define PLANESELECTION_H

#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkSphereSource.h>
#include <vtkClipPolyData.h>
#include <vtkPlane.h>

#include <vtkCommand.h>
#include <vtkImplicitPlaneWidget2.h>
#include <vtkImplicitPlaneRepresentation.h>

#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include <QVTKWidget.h>
#include <vtkfitsreader.h>


#endif // PLANESELECTION_H

class vtkIPWCallback : public vtkCommand
{
public:
    static vtkIPWCallback * New()
    {   return new vtkIPWCallback; }
    virtual void Execute (vtkObject *caller, unsigned long, void *)
    {
        vtkImplicitPlaneWidget2 *planeWidget =
                reinterpret_cast<vtkImplicitPlaneWidget2*>(caller);
        vtkImplicitPlaneRepresentation *rep =
                reinterpret_cast<vtkImplicitPlaneRepresentation*>(planeWidget->GetRepresentation());
        rep->GetPlane(this->Plane);
    }
    vtkIPWCallback():Plane(0),Actor(0) {}
    vtkPlane *Plane;
    vtkActor *Actor;
};

void PlaneSelection(QVTKWidget *qvtkWidget, vtkFitsReader *fitsSource)
{
    vtkSmartPointer<vtkPlane> plane =
            vtkSmartPointer<vtkPlane>::New();
    vtkSmartPointer<vtkClipPolyData> clipper =
            vtkSmartPointer<vtkClipPolyData>::New();
    clipper->SetClipFunction(plane);
    clipper->InsideOutOn();
    clipper->SetInputConnection(fitsSource->GetOutputPort());

    //Create a mapper and actor
    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(clipper->GetOutputPort());
    vtkSmartPointer<vtkActor> someActor =
            vtkSmartPointer<vtkActor>::New();
    someActor->SetMapper(mapper);

    vtkSmartPointer<vtkProperty> backFaces =
            vtkSmartPointer<vtkProperty>::New();
    backFaces->SetDiffuseColor(.8, .8, .4);

    /////////////////////////////////
    /// \brief Get the Window and Interactors from qvtkWIDGET
    ///

    //We get the renderer
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();

    renderer = qvtkWidget->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

    //We Get the Interactor
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();

    renderWindowInteractor = qvtkWidget->GetInteractor();

    vtkSmartPointer<vtkIPWCallback> myCallback =
            vtkSmartPointer<vtkIPWCallback>::New();
    myCallback->Plane = plane;
    myCallback->Actor = someActor;

    vtkSmartPointer<vtkImplicitPlaneRepresentation> rep =
            vtkSmartPointer<vtkImplicitPlaneRepresentation>::New();
    rep->SetPlaceFactor(1.25);  //This Must be set prior to placing the widget
    rep->PlaceWidget(someActor->GetBounds());
    rep->SetNormal(plane->GetNormal());
    rep->SetOrigin(0,0,50);     // This doesnt seem to work?

    vtkSmartPointer<vtkImplicitPlaneWidget2> planeWidget =
            vtkSmartPointer<vtkImplicitPlaneWidget2>::New();
    planeWidget->SetInteractor(renderWindowInteractor);
    planeWidget->SetRepresentation(rep);
    planeWidget->AddObserver(vtkCommand::InteractionEvent,myCallback);

    planeWidget->On();


}


