#ifndef POINTERTRACKER_H
#define POINTERTRACKER_H

#include <vtkSmartPointer.h>
#include <vtkRendererCollection.h>
#include <vtkPointPicker.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkProperty.h>

#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkProbeFilter.h>
#include <vtkGlyph3D.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkConeSource.h>
#include <vtkPointSource.h>
#include <vtkPointWidget.h>
#include <vtkCommand.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <sstream>

#include <QVTKWidget.h>
#include <vtkVolume.h>
#include <vtkfitsreader.h>

#endif // POINTERTRACKER_H

// This does the actual work: updates the probe.
// Callback for the interaction
class vtkmyPWCallback : public vtkCommand
{
public:
    static vtkmyPWCallback *New()
    { return new vtkmyPWCallback; }
    virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
        vtkPointWidget *pointWidget = reinterpret_cast<vtkPointWidget*>(caller);
        pointWidget->GetPolyData(this->PolyData);
        double position[3];

        pointWidget->GetPosition(position);
        std::ostringstream text;
        text    << "Point: "
                << std::fixed << std::setprecision(1)
                << position[0] << ", " << position[1] << ", " << position[2];
        this->TextActor->SetInput(text.str().c_str());        
        //this->Actor->VisibilityOn();
    }
    vtkmyPWCallback():PolyData(0),Actor(0) {}
    vtkPolyData *PolyData;
    vtkActor *Actor;
    vtkTextActor* TextActor;
};

void AddPointerTracker(QVTKWidget *qvtkWidget, vtkVolume *volume, vtkFitsReader *fitsSource )
{


    //vtkSmartPointer<vtkPolyData> inputPolyData =
      //  vtkPolyData::SafeDownCast(fitsSource->GetOutput()->);

    /// The plane widget is used probe the dataset.
    vtkSmartPointer<vtkPolyData> point =
            vtkSmartPointer<vtkPolyData>::New();

    vtkSmartPointer<vtkProbeFilter> probe =
            vtkSmartPointer<vtkProbeFilter>::New();
    probe->SetInput(point);
    probe->SetSource(fitsSource->GetOutput());

    /// create glyph
    ///
    ///
    vtkSmartPointer<vtkConeSource> cone =
            vtkSmartPointer<vtkConeSource>::New();
    cone->SetResolution(16);

    vtkSmartPointer<vtkGlyph3D> glyph =
            vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetInputConnection(probe->GetOutputPort());
    glyph->SetSourceConnection(cone->GetOutputPort());
    glyph->SetVectorModeToUseVector();
    glyph->SetScaleModeToDataScalingOff();
    glyph->SetScaleFactor(volume->GetLength() * 0.1);

    vtkSmartPointer<vtkPolyDataMapper> glyphMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    glyphMapper->SetInputConnection(glyph->GetOutputPort());

    vtkSmartPointer<vtkActor> glyphActor =
            vtkSmartPointer<vtkActor>::New();
    glyphActor->SetMapper(glyphMapper);
    glyphActor->VisibilityOn();

    /// An outline is shown for context.
    ///
    ///
    vtkSmartPointer<vtkStructuredGridOutlineFilter> outline =
            vtkSmartPointer<vtkStructuredGridOutlineFilter>::New();
    outline->SetInput(fitsSource->GetOutput());


    vtkSmartPointer<vtkPolyDataMapper> outlineMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    outlineMapper->SetInputConnection(outline->GetOutputPort());

    vtkSmartPointer<vtkActor> outlineActor =
            vtkSmartPointer<vtkActor>::New();
    outlineActor->SetMapper(outlineMapper);

    /// A Text Widget showing the point location
    ///
    ///
    vtkSmartPointer<vtkTextActor> textActor =
            vtkSmartPointer<vtkTextActor>::New();
    textActor->GetTextProperty()->SetFontSize(16);
    textActor->SetPosition(10, 20);
    textActor->SetInput("cursor:");
    textActor->GetTextProperty()->SetColor(0.8900, 0.8100, 0.3400);

    /// Extract the RenderWindow, Renderer and  add both Actors
    ///
    ///

    vtkSmartPointer<vtkRenderer> renderer =
            qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
            qvtkWidget->GetInteractor();

    // The SetInteractor method is how 3D widgets are associated with the render
    // window interactor. Internally, SetInteractor sets up a bunch of callbacks
    // using the Command/Observer mechanism (AddObserver()).
    vtkSmartPointer<vtkmyPWCallback> myCallback =
            vtkSmartPointer<vtkmyPWCallback>::New();
    myCallback->PolyData = point;
    myCallback->Actor = glyphActor;
    myCallback->TextActor = textActor;

    vtkSmartPointer<vtkPointWidget> pointWidget =
      vtkSmartPointer<vtkPointWidget>::New();
    pointWidget->SetInteractor(interactor);
    pointWidget->SetInput(fitsSource->GetOutput());
    pointWidget->AllOff();
    pointWidget->PlaceWidget();
    pointWidget->GetPolyData(point);
    pointWidget->AddObserver(vtkCommand::InteractionEvent,myCallback);

    renderer->AddActor(outlineActor);
    renderer->AddActor(glyphActor);
    renderer->AddActor2D(textActor);

    pointWidget->On();

}
