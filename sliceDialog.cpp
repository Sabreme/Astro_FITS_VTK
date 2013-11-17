#include "sliceDialog.h"
#include "ui_sliceDialog.h"

#include "vtkAxesActor.h"
#include "vtkOrientationMarkerWidget.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkExtractVOI.h"
#include "vtkOutlineFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
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

#include "vtkCellPicker.h"
#include "vtkImagePlaneWidget.h"
#include "vtkProperty.h"
#include "vtkVolume.h"
#include "vtkAlgorithm.h"
#include "vtkImageData.h"
#include "vtkRendererCollection.h"


SliceDialog::SliceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SliceDialog)
{
    ui->setupUi(this);    
    //this->ui-> = ui->qvtkWidgetLeft;
    //ui->qvtkWidgetLeft = extractedSource;
}

SliceDialog::~SliceDialog()
{
    delete ui;
}

void SliceDialog::UpdateQVTKWidget(QVTKWidget *qvtkWidget, vtkFitsReader *source)
{
    //char * dirname = NULL;
    //vtkAlgorithm * reader = 0;
    //vtkImageData * input = 0;
    //double reductionFactor = 1.0;

//    /////////////////////////////
//    /// \brief Outline Object
//    ///
//    qDebug() << "Adding Outline" << endl;
//    vtkOutlineFilter *outlineF = vtkOutlineFilter::New();
//    outlineF->SetInputConnection(source->GetOutputPort());

//    /// Create the Volume & mapper

//    vtkVolume *volume = vtkVolume::New();

//    vtkGPUVolumeRayCastMapper *mapper = vtkGPUVolumeRayCastMapper::New();

//    input= source->GetOutput();

//    int imageDims[3];
//    input->GetDimensions(imageDims);

//    vtkImageResample *resample = vtkImageResample::New();

//    resample->SetInputConnection( source->GetOutputPort() );
//    resample->SetAxisMagnificationFactor(0, reductionFactor);
//    resample->SetAxisMagnificationFactor(1, reductionFactor);
//    resample->SetAxisMagnificationFactor(2, reductionFactor);














    //mapper->SetInputConnection(fitsReader->GetOutputPort());
//    mapper->SetInputConnection(source->GetOutputPort());

//    /// Create out transfer function
//    ///
//    double opacityWindow = 4096;
//    double opacityLevel = 2048;

//    // Create our transfer function
//    vtkPiecewiseFunction *opacityFun = vtkPiecewiseFunction::New();
//    vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();

//    // Create the property and attach the transfer functions

//    vtkVolumeProperty * property = vtkVolumeProperty::New();
//    property->SetColor(colorFun);
//    property->SetScalarOpacity(opacityFun);
//    property->SetInterpolationTypeToLinear();

//    colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
//    colorFun->AddRGBPoint( -1000, .62, .36, .18, 0.5, 0.0 );
//    colorFun->AddRGBPoint( -500, .88, .60, .29, 0.33, 0.45 );
//    colorFun->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );

//    opacityFun->AddPoint(0,  0.0);
//    opacityFun->AddPoint(50,  0.5);
//    opacityFun->AddPoint(1000,  1.0);

//    //mapper->SetBlendModeToComposite();
//    //mapper->SetBlendModeToMaximumIntensity();
//    property->ShadeOn();
//    property->SetAmbient(0.4);
//    property->SetDiffuse(0.6);
//    property->SetSpecular(0.2);
//    property->SetSpecularPower(10.0);
//    property->SetScalarOpacityUnitDistance(0.8919);

//    // connect up the volume to the property and the mapper
//    volume->SetProperty( property );
//    volume->SetMapper(mapper);



    vtkSmartPointer<vtkCubeSource> cubeSource =
      vtkSmartPointer<vtkCubeSource>::New();
    cubeSource->Update();

    // Create a mapper and actor
    vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(cubeSource->GetOutputPort());
    vtkSmartPointer<vtkActor> actor =
      vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
















    ////////////////////////////////////
    /// \brief RenderWindow
    ////
    qDebug() << "Adding RenderWindow" << endl;

    vtkSmartPointer<vtkRenderer> ren1 =
            vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();

    renderWindow->AddRenderer(ren1);
    ren1->AddActor(actor);

    // Set the vtkInteractorStyleSwitch for renderWindowInteractor
    vtkSmartPointer<vtkInteractorStyleSwitch> style =
            vtkSmartPointer<vtkInteractorStyleSwitch>::New();

   // qDebug() << "Adding Objects to RenderWindow" << endl;



  //  ren1->AddVolume(volume);


    this->ui->qvtkWidgetRight->GetRenderWindow()->AddRenderer(ren1);
    vtkRenderWindowInteractor *iren = this->ui->qvtkWidgetRight->GetInteractor();

/////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////



    /// The Shared Picker enables us to use 3 planes at one time and gets the picking order right

//    vtkCellPicker* picker = vtkCellPicker::New();
//    picker->SetTolerance(0.005);



    /// The 3 image plane widgets are used to probe the dataset

    vtkSmartPointer<vtkImagePlaneWidget> planeWidgetX =
            vtkSmartPointer<vtkImagePlaneWidget>::New();
    //planeWidgetX->Off();
    planeWidgetX->SetInteractor(iren);

    //planeWidgetX->DisplayTextOn();
    //planeWidgetX->SetInput(resample->GetOutput());
    //planeWidgetX->SetPlaneOrientationToXAxes();
    //planeWidgetX->SetSliceIndex(2);
    //planeWidgetX->SetPicker(picker);
    //planeWidgetX->SetKeyPressActivationValue('x');
    //planeWidgetX->RestrictPlaneToVolumeOn();
    //vtkProperty* propX = vtkProperty::New();
    //propX = planeWidgetX->GetPlaneProperty();
    //propX->SetColor(1,0,0);

//  double* origin;
    //double origin[3] = {0,1,0};

//    origin = resample->GetOutput()->GetOrigin();


//    planeWidgetX->SetOrigin(origin);
    planeWidgetX->UpdatePlacement();

    //planeWidgetX->SetDefaultRenderer(this->ui->qvtkWidgetRight->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
//    iren->Print(std::cout);




    //planeWidgetX->Print(std::cout);

    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////

//    ////////////////////////////////////
//    /// \brief OrientationMarker Widget
//    ////
//    qDebug() << "Adding Orientation Marker" << endl;

//    vtkAxesActor * axesActor = vtkAxesActor::New();
//    axesActor->SetNormalizedTipLength(0.4, 0.4, 0.4);
//    axesActor->SetNormalizedShaftLength(0.6, 0.6, 0.6);
//    axesActor->SetShaftTypeToCylinder();

//    vtkOrientationMarkerWidget * marker = vtkOrientationMarkerWidget::New();
//    marker->SetInteractor(this->ui->qvtkWidgetRight->GetInteractor());
//    marker->SetOrientationMarker(axesActor);
//    marker->SetEnabled(true);
//    marker->InteractiveOn();


    planeWidgetX->On();
    //iren->Start();

    //ren1->ResetCamera();


}

//void SliceDialog::on_pushButton_clicked()
//{
//    QFuture<void> future = QtConcurrent::
//}
