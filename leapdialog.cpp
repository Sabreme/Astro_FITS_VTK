#include "leapdialog.h"
#include "ui_leapdialog.h"

#include "vtkSmartPointer.h"
#include "vtkPolyDataMapper.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkCubeSource.h"
#include "vtkGlyph3D.h"
#include "vtkProbeFilter.h"
#include "vtkProperty.h"
//#include "PointerTracker.h"

#include "vtkTextActor.h"
#include "sstream"
#include "vtkCommand.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

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

LeapDialog::LeapDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LeapDialog)
{
    ui->setupUi(this);
}

LeapDialog::~LeapDialog()
{
    delete ui;
}

void LeapDialog::leapDiagnostic()
{

    double sphereCenter[3];
    double sphereRadius;

    ////////////////////////////////////////////////
    ///////////////////////////////////////////////
    /// SPHERE ACTOR
    ///////////////////////////////////////////////
    ///////////////////////////////////////////////
    vtkSmartPointer<vtkPolyData> inputPolyData ;

    vtkSmartPointer<vtkSphereSource> sphereSource =
            vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetPhiResolution(15);
    sphereSource->SetThetaResolution(15);
    sphereSource->Update();
    inputPolyData = sphereSource->GetOutput();

    vtkSmartPointer<vtkPolyDataMapper> mapperSphere =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperSphere->SetInput(inputPolyData);

    vtkSmartPointer<vtkActor> sphereActor =
            vtkSmartPointer<vtkActor>::New();
    sphereActor->SetMapper(mapperSphere);
    sphereActor->GetProperty()->SetRepresentationToWireframe();
    sphereActor->GetProperty()->SetColor(1.0, 1.0, 1.0);

    /// Create the Renderer
    vtkRenderer * renderer = vtkRenderer::New();

    /// Create the RenderWindow
    vtkRenderWindow * renWin = vtkRenderWindow::New();

    renWin->AddRenderer(renderer);

    renderer->SetBackground(0.1, 0.2, 0.4);

    this->ui->widgetLeapViz->SetRenderWindow(renWin);

    renderer->AddActor(sphereActor);

    sphereSource->GetCenter(sphereCenter);
    sphereRadius =  sphereSource->GetRadius();

    leapDbgSphere = sphereSource;
    leapDbgSphereActor = sphereActor;
    sphereActor->RotateY(-90);

    /// Get the Interactor
    vtkRenderWindowInteractor * interactor = this->ui->widgetLeapViz->GetInteractor();

    interactor->MouseWheelBackwardEvent();
    interactor->MouseWheelBackwardEvent();
    interactor->MouseWheelBackwardEvent();
    interactor->MouseWheelBackwardEvent();
    interactor->MouseWheelBackwardEvent();
    interactor->MouseWheelBackwardEvent();
    interactor->MouseWheelBackwardEvent();
    interactor->MouseWheelBackwardEvent();
    interactor->MouseWheelBackwardEvent();


    ////////////////////////////////////////////////
    ///////////////////////////////////////////////
    /// ARROW ACTOR
    ///////////////////////////////////////////////
    ///////////////////////////////////////////////

    vtkSmartPointer<vtkArrowSource> arrowSource =
            vtkSmartPointer<vtkArrowSource>::New();

    double startPoint[3], endPoint[3];
#ifndef main
    vtkMath::RandomSeed(time(NULL));
#else
    vtkMath::RandomSeed(8775070);
#endif

    startPoint[0] = 0;
    startPoint[1] = 0;
    startPoint[2] = 0.5;

    endPoint[0] = 0;
    endPoint[1] = 0;
    endPoint[2] = -0.5;

    // Compute a basis
    double normalizedX[3];
    double normalizedY[3];
    double normalizedZ[3];

    //THe X axis is a vector from start to end

    vtkMath::Subtract(endPoint, startPoint, normalizedX);
    ///double length = vtkMath::Norm(normalizedX);
    double length = sphereRadius * 2;


    vtkMath::Normalize(normalizedX);

    // THe Z axis is an arbitrary vector cross X

    double arbitratry[3];

    arbitratry[0]  = vtkMath::Random(-10, 10);
    arbitratry[1]  = vtkMath::Random(-10, 10);
    arbitratry[2]  = vtkMath::Random(-10, 10);
    vtkMath::Cross(normalizedX,arbitratry,normalizedZ);
    vtkMath::Normalize(normalizedZ);

    // The Y axis is Z cross X
    vtkMath::Cross(normalizedZ,normalizedX,normalizedY);
    vtkSmartPointer<vtkMatrix4x4> matrix =
            vtkSmartPointer<vtkMatrix4x4>::New();


    //Create the direction cosine matrix
    matrix->Identity();
    for (unsigned int i = 0; i < 3; i++)
    {
        matrix->SetElement(i, 0,normalizedX[i]);
        matrix->SetElement(i, 1, normalizedY[i]);
        matrix->SetElement(i, 2, normalizedZ[i]);
    }

    // Apply the transforms
    vtkSmartPointer<vtkTransform> transform =
            vtkSmartPointer<vtkTransform>::New();

    transform->Translate(startPoint);
    transform->Concatenate(matrix);
    transform->Scale(length, length, length);

    // Transform the polydata
    vtkSmartPointer<vtkTransformPolyDataFilter> transformPD =
            vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformPD->SetTransform(transform);
    transformPD->SetInputConnection(arrowSource->GetOutputPort());

    //Create a mapper and actor for the arrow
    vtkSmartPointer<vtkPolyDataMapper> mapperArrow =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    vtkSmartPointer<vtkActor> arrowActor =
            vtkSmartPointer<vtkActor>::New();

#ifdef USER_MATRIX
    mapperArrow->SetInputConnection(arrowSource->GetOutputPort());
    arrowActor->SetUserMatrix(transform->GetMatrix());
#else
    mapperArrow->SetInputConnection(transformPD->GetOutputPort());
#endif
    arrowActor->SetMapper(mapperArrow);


    renderer->AddActor(arrowActor);

    leapDbgArrow = arrowSource;
    leapDbgArrowActor = arrowActor;
    //global_Arrow = arrowSource;


    /// Align the arrow with the Widget
    arrowActor->RotateY(-90);


    ////////////////////////////////////////////////
    ///////////////////////////////////////////////
    /// PLANE ACTOR
    ///////////////////////////////////////////////
    ///////////////////////////////////////////////

    leapDbgPlaneWidget = vtkPlaneWidget::New();
    leapDbgPlaneWidget->SetInput(leapDbgSphere->GetOutput());
    leapDbgPlaneWidget->NormalToXAxisOn();
    leapDbgPlaneWidget->SetResolution(20);
    leapDbgPlaneWidget->SetRepresentationToOutline();
    leapDbgPlaneWidget->PlaceWidget();

    leapDbgPlaneWidget->SetInteractor(this->ui->widgetLeapViz->GetInteractor());
    leapDbgPlaneWidget->EnabledOn();

    ////////////////////////////////////////////////
    ///////////////////////////////////////////////
    /// POINTWIDGET ACTOR
    ///////////////////////////////////////////////
    ///////////////////////////////////////////////


    vtkSmartPointer<vtkCubeSource> cubeSource =
            vtkSmartPointer<vtkCubeSource>::New();
    //cubeSource->SetBounds(global_Renderer->ComputeVisiblePropBounds());
    cubeSource->SetBounds(-1, 1, -1, 1, -1, 1);
    cubeSource->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapperCube =
        vtkSmartPointer<vtkPolyDataMapper>::New();
//    mapper->SetInput();


    /// The plane widget is used probe the dataset.
    vtkSmartPointer<vtkPolyData> point =
            vtkSmartPointer<vtkPolyData>::New();

    vtkSmartPointer<vtkProbeFilter> probe =
            vtkSmartPointer<vtkProbeFilter>::New();
    probe->SetInput(point);
    probe->SetSource(cubeSource->GetOutput());

    ///
    /// create glyph
    ///

    vtkSmartPointer<vtkSphereSource> pointMarker =
            vtkSmartPointer<vtkSphereSource>::New();
    pointMarker->SetRadius(0.1);

    vtkSmartPointer<vtkGlyph3D> glyph =
            vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetInputConnection(probe->GetOutputPort());
    glyph->SetSourceConnection(pointMarker->GetOutputPort());
    glyph->SetVectorModeToUseVector();
    glyph->SetScaleModeToDataScalingOff();
//    glyph->SetScaleFactor(global_Volume->GetLength() * 0.1);

    vtkSmartPointer<vtkPolyDataMapper> glyphMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    glyphMapper->SetInputConnection(glyph->GetOutputPort());

    vtkSmartPointer<vtkActor> glyphActor =
            vtkSmartPointer<vtkActor>::New();
    glyphActor->SetMapper(glyphMapper);
    glyphActor->VisibilityOn();


    // The SetInteractor method is how 3D widgets are associated with the render
    // window interactor. Internally, SetInteractor sets up a bunch of callbacks
    // using the Command/Observer mechanism (AddObserver()).
    vtkSmartPointer<vtkmyPWCallback> myCallback =
            vtkSmartPointer<vtkmyPWCallback>::New();
    myCallback->PolyData = point;
    //myCallback->CursorActor = glyphActor;
//    myCallback->PositionActor = textActor;

    leapDbgPointWidget = vtkPointWidget::New();
    leapDbgPointWidget->SetInteractor(this->ui->widgetLeapViz->GetInteractor());
    //pointWidget->SetInput(global_Reader->GetOutput());
    leapDbgPointWidget->AllOff();
    leapDbgPointWidget->PlaceWidget(-1, 1, -1, 1, -1, 1);
    leapDbgPointWidget->GetPolyData(point);
    leapDbgPointWidget->EnabledOff();
    //vtkEventConnector->Connect(pointWidget_, vtkCommand::InteractionEvent, this, SLOT(pointWidgetCallBack()));
    leapDbgPointWidget->AddObserver(vtkCommand::InteractionEvent  ,myCallback);

    leapDbgPointWidget->GetProperty()->SetLineWidth(1.5);

    // Set the widget colour to GREEN
    //pointWidget->GetProperty()->SetColor(0.0, 1.0, 0.0);

    renderer->AddActor(glyphActor);
//    global_Renderer->AddActor2D(textActor);

    leapDbgPointWidget->EnabledOn();
    //global_Initialised = true;


    //renderer->GetActiveCamera()->SetClippingRange(3.5, 5.0);
//    std::cout << "-------------------------- CAMERA -------------------------------" << endl;
//    renderer->GetActiveCamera()->Print(std::cout);
//    std::cout << "-------------------------- RENWIN -------------------------------" << endl;
//    renWin->Print(std::cout);
//    std::cout << "-------------------------- RENDER -------------------------------" << endl;
//    renderer->Print(std::cout);

}
