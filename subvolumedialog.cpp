#include "subvolumedialog.h"
#include "ui_subvolumedialog.h"

#include "vtkAxesActor.h"
#include "vtkOrientationMarkerWidget.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCubeAxesActor.h"

#include "vtkExtractVOI.h"
#include "vtkOutlineFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkMarchingCubes.h"
#include "vtkImageDataGeometryFilter.h"
#include "vtkDecimatePro.h"
#include "vtkProperty.h"
#include "vtkImageResample.h"
#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkImageData.h"
#include "vtkVolumeProperty.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkTextProperty.h"
#include "vtkSmartPointer.h"


SubVolumeDialog::SubVolumeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SubVolumeDialog)
{
    ui->setupUi(this);    
    //this->ui-> = ui->qvtkWidgetLeft;
    //ui->qvtkWidgetLeft = extractedSource;
}

SubVolumeDialog::~SubVolumeDialog()
{
    emit this->windowClosing(true);
    delete ui;
}

void SubVolumeDialog::UpdateQVTKWidget(QVTKWidget *qvtkWidget, vtkFitsReader *source, double subVolBounds[6])
{

    this->ui->lineMinX->setText(QString::number(subVolBounds[0], 'f', 2));
    this->ui->lineMaxX->setText(QString::number(subVolBounds[1], 'f', 2));
    this->ui->lineMinY->setText(QString::number(subVolBounds[2], 'f', 2));
    this->ui->lineMaxY->setText(QString::number(subVolBounds[3], 'f', 2));
    this->ui->lineMinZ->setText(QString::number(subVolBounds[4], 'f', 2));
    this->ui->lineMaxZ->setText(QString::number(subVolBounds[5], 'f', 2));

    //this->ui->qvtkWidgetLeft->SetRenderWindow(updatedVolume);
   // double bounds[6];

   // subVolume->GetOutput()->GetBounds(bounds);


    vtkSmartPointer<vtkExtractVOI> extractVOI =
            vtkSmartPointer<vtkExtractVOI>::New();
    //extractVOI->SetInputConnection(source->GetOutputPort());
    extractVOI->SetInputConnection(source->GetOutputPort());
    extractVOI->SetVOI(
            subVolBounds[0],
            subVolBounds[1],
            subVolBounds[2],
            subVolBounds[3],
            subVolBounds[4],
            subVolBounds[5]);
    extractVOI->Update();

    vtkImageData* extracted = extractVOI->GetOutput();

    /////////////////////////////
    /// \brief Outline Object
    ///
    qDebug() << "Adding Outline" << endl;
    vtkOutlineFilter *outlineF = vtkOutlineFilter::New();
    outlineF->SetInputConnection(extractVOI->GetOutputPort());

    vtkPolyDataMapper *outlineMapper = vtkPolyDataMapper::New();
    outlineMapper->SetInputConnection(outlineF->GetOutputPort());

    vtkActor *outlineActor = vtkActor::New();
    outlineActor->SetMapper(outlineMapper);
    outlineActor->GetProperty()->SetColor(0.5,0.5,0.5);


//    ///
//    /// \brief resample
//    ///
//    vtkImageResample *resample = vtkImageResample::New();

//    resample->SetInputConnection(extractVOI->GetOutputPort() );
//    resample->SetAxisMagnificationFactor(0, 1.0);
//    resample->SetAxisMagnificationFactor(1, 1.0);
//    resample->SetAxisMagnificationFactor(2, 1.0);

    /// Create the Volume & mapper

    vtkVolume *volume = vtkVolume::New();
    vtkGPUVolumeRayCastMapper *mapper = vtkGPUVolumeRayCastMapper::New();

    //mapper->SetInputConnection(fitsReader->GetOutputPort());
    mapper->SetInputConnection(extractVOI->GetOutputPort());

    /// Create out transfer function
    ///
    double opacityWindow = 4096;
    double opacityLevel = 2048;

    // Create our transfer function
    vtkPiecewiseFunction *opacityFun = vtkPiecewiseFunction::New();
    vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();

    // Create the property and attach the transfer functions

    vtkVolumeProperty * property = vtkVolumeProperty::New();
    property->SetColor(colorFun);
    property->SetScalarOpacity(opacityFun);
    property->SetInterpolationTypeToLinear();

    colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
    colorFun->AddRGBPoint( -1000, .62, .36, .18, 0.5, 0.0 );
    colorFun->AddRGBPoint( -500, .88, .60, .29, 0.33, 0.45 );
    colorFun->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );

    //    colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
    //    colorFun->AddRGBPoint( -155, .55, .25, .15, 0.5, .92 );
    //    colorFun->AddRGBPoint( 217, .88, .60, .29, 0.33, 0.45 );
    //    colorFun->AddRGBPoint( 420, 1, .94, .95, 0.5, 0.0 );
    //    colorFun->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );

    opacityFun->AddPoint(0,  0.0);
    opacityFun->AddPoint(50,  0.5);
    opacityFun->AddPoint(1000,  1.0);
    //    opacityFun->AddSegment(0, 1.0, 255, 1.0);
    //    opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
    //    opacityFun->AddPoint(-1000, 0, 0.5, 0.0 );
    //    opacityFun->AddPoint(-500, 1.0, 0.33, 0.45 );
    //    opacityFun->AddPoint(3071, 1.0, 0.5, 0.0);

    mapper->SetBlendModeToComposite();
    //mapper->SetBlendModeToMaximumIntensity();
    property->ShadeOn();
    property->SetAmbient(0.4);
    property->SetDiffuse(0.6);
    property->SetSpecular(0.2);
    property->SetSpecularPower(10.0);
    property->SetScalarOpacityUnitDistance(0.8919);

    // connect up the volume to the property and the mapper
    volume->SetProperty( property );
    volume->SetMapper( mapper );


    ////////////////////////////////////
    /// \brief RenderWindow
    ////
    qDebug() << "Adding RenderWindow" << endl;

    vtkRenderer *ren1 = vtkRenderer::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();

    renderWindow->AddRenderer(ren1);

    // Set the vtkInteractorStyleSwitch for renderWindowInteractor
    vtkSmartPointer<vtkInteractorStyleSwitch> style =
            vtkSmartPointer<vtkInteractorStyleSwitch>::New();

    ////////////////////////////////////
    /// \brief Cube Axes Labels
    ////

    /// Create the Actor
    vtkSmartPointer<vtkCubeAxesActor> cubeAxesActor =
            vtkSmartPointer<vtkCubeAxesActor>::New();

    qDebug() << "Adding Axes Labels" << endl;

    cubeAxesActor->SetCamera(ren1->GetActiveCamera());
    cubeAxesActor->SetBounds(volume->GetBounds());
    #if VTK_MAJOR_VERSION > 5
    cubeAxesActor->SetGridLineLocation(VTK_GRID_LINES_FURTHEST);
    #endif
    cubeAxesActor->GetTitleTextProperty(0)->SetColor(1.0, 0.0, 0.0);
    cubeAxesActor->GetLabelTextProperty(0)->SetColor(1.0, 0.0, 0.0);

    cubeAxesActor->GetTitleTextProperty(1)->SetColor(0.0, 1.0, 0.0);
    cubeAxesActor->GetLabelTextProperty(1)->SetColor(0.0, 1.0, 0.0);

    cubeAxesActor->GetTitleTextProperty(2)->SetColor(0.0, 0.0, 1.0);
    cubeAxesActor->GetLabelTextProperty(2)->SetColor(0.0, 0.0, 1.0);



    // add actors to renderer
    ///Add CubeAxesActor to RenderWindow
    ren1->AddActor(cubeAxesActor);

    /// Add OutlineActor to REnderWindow
    ren1->AddActor(outlineActor);

    qDebug() << "Adding Objects to RenderWindow" << endl;

    ren1->AddVolume(volume);


    this->ui->qvtkWidgetRight->GetRenderWindow()->AddRenderer(ren1);


    ////////////////////////////////////
    /// \brief OrientationMarker Widget
    ////
    qDebug() << "Adding Orientation Marker" << endl;

    vtkAxesActor * axesActor = vtkAxesActor::New();
    axesActor->SetNormalizedTipLength(0.4, 0.4, 0.4);
    axesActor->SetNormalizedShaftLength(0.6, 0.6, 0.6);
    axesActor->SetShaftTypeToCylinder();

    vtkOrientationMarkerWidget * marker = vtkOrientationMarkerWidget::New();
    marker->SetInteractor(this->ui->qvtkWidgetRight->GetInteractor());
    marker->SetOrientationMarker(axesActor);
    marker->SetEnabled(true);
    marker->InteractiveOn();

    ren1->ResetCamera();

}

//void SubVolumeDialog::on_pushButton_clicked()
//{
//    QFuture<void> future = QtConcurrent::
//}

void SubVolumeDialog::on_buttonBox_accepted()
{
    emit this->windowClosing(true);
}
