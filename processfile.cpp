#include "processfile.h"

#include "ScalarBar.h"

#include "XYHighlightSelection.h"

#include "PlaneSelection.h"


#include <QFileDialog>
#include <string>
#include <vtkVersion.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkInteractorStyleSwitch.h>

#include <vtkActor2DCollection.h>
#include <vtkCubeAxesActor.h>
#include <vtkTextProperty.h>
#include <vtkActor.h>

#include <vtkTransform.h>
#include <vtkCamera.h>
#include <math.h>

#include <vtkVolume.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkStructuredPointsToPolyDataFilter.h>

#include <vtkStructuredPointsGeometryFilter.h>
#include <QDebug>

class vtkProgressCommand : public vtkCommand
{
public:
    static vtkProgressCommand * New() {return new vtkProgressCommand; }
    virtual void Execute (vtkObject * caller, unsigned long, void *callData)
    {
        double progress  = *(static_cast<double*>(callData));
        std::cout << "Progress at " << progress << std::endl;
    }

};

ProcessFile::ProcessFile()
{

}

void ProcessFile::loadFitsFile(QString &filename)
{
    //////////////////////////////////////////////////////////////////
    ///////////VTK FITS READER CODE FROM FITS.CXX ////////////////////
    //////////////////////////////////////////////////////////////////

    vtkCommand* pObserver = vtkProgressCommand::New();


    // vtk pipeline
    vtkFitsReader *fitsReader = vtkFitsReader::New();
    const char *newFileName = filename.toStdString().c_str();
    fitsReader->SetFileName(newFileName);
    //fitsReader->AddObserver(vtkCommand::ProgressEvent, pObserver );

    //fitsReader->Update();



    fitsReader->Execute();
    //this->ProgressDialog = new QProgressDialog("Openning FITS File...", "Abort", 0, fitsReader->)
    //ui->progressBar->setValue(fitsReader->GetProgress());
    //renWin->SetWindowName(fitsReader->GetFileName());

    //fitsReader->get

    /////////////////////////////////
    /// \brief Outline Object
    ///

    vtkOutlineFilter *outlineF = vtkOutlineFilter::New();
    outlineF->AddObserver(vtkCommand::ProgressEvent, pObserver );
    outlineF->SetInputConnection(fitsReader->GetOutputPort());


    ///////////////////////////////
    //// IsoSurface Changes
    ///

    //vtkVolume  *volume = vtkVolume::New();
    //vtkSmartVolumeMapper *outlineM = vtkSmartVolumeMapper::New();


    // Create the Mapper

    vtkPolyDataMapper *outlineM = vtkPolyDataMapper::New();
    //outlineM->AddObserver(vtkCommand::ProgressEvent, pObserver );
    outlineM->SetInput(outlineF->GetOutput());

    //outlineM->ScalarVisibilityOff();

    // Create the Actor

    vtkActor *outlineA = vtkActor::New();
    //outlineA->AddObserver(vtkCommand::ProgressEvent, pObserver );
    outlineA->SetMapper(outlineM);

    ///////////////////////////////
    /// \brief Lookup Table
    ///

    // create a b/w lookup Table
//    vtkLookupTable * bwLut = vtkLookupTable::New();
//    bwLut->SetTableRange( 0,1000);
//    bwLut->SetSaturationRange(0,0);
//    bwLut->SetHueRange(0,0);
//    bwLut->SetValueRange(0,1);

    ///////////////////////////////
    /// \brief Shell for Objects
    ///

    // isosurface
    vtkMarchingCubes *shellE = vtkMarchingCubes::New();

    shellE->SetInputConnection(fitsReader->GetOutputPort());
    shellE->SetValue(0, 10.0f);

//    // decimate
//    vtkDecimatePro *shellD = vtkDecimatePro::New();
//    shellD->SetInput(shellE->GetOutput());
//    shellD->SetTargetReduction(0.9);

    // Create the Mapper

    vtkPolyDataMapper *shellM = vtkPolyDataMapper::New();
    shellM->AddObserver(vtkCommand::ProgressEvent, pObserver );
    shellM->SetInput(shellE->GetOutput());
//    shellM->SetInput(shellD->GetOutput());
    shellM->ScalarVisibilityOn();

    // Create the Actor

    vtkActor *shellA = vtkActor::New();
    shellA->SetMapper(shellM);
   // shellA->GetProperty()->SetColor(0.5, 0.5, 1.0);
    //shellA->GetProperty()->SetColor(0.1, 0.1, 0.1);

    /////////////////////////////////////
    /// \brief Slice Object
    ///

    // slice
    vtkImageDataGeometryFilter *sliceE =
            vtkImageDataGeometryFilter::New();
    // This sets the [xmin, xmax,ymin, ymax, zmin, zmax]
    sliceE->SetExtent(0, 1000, 0, 1000, 0, 30);
    sliceE->SetInputConnection(fitsReader->GetOutputPort());

    // Create the Mapper

    vtkPolyDataMapper *sliceM = vtkPolyDataMapper::New();
    sliceM->SetInputConnection(sliceE->GetOutputPort());
    sliceM->ScalarVisibilityOn();
    //default_Volume_Colours = sliceM->GetLookupTable();

    //sliceM->SetLookupTable(bwLut);
    double *range;

    //vtkStructuredPoints *tempSet ;
    //tempSet = fitsReader->GetOutput();

    range = fitsReader->GetOutput()->GetScalarRange();
    sliceM->SetScalarRange(range);

    // Create the Actor

    vtkActor *sliceA = vtkActor::New();
    sliceA->SetMapper(sliceM);

    vtkSmartPointer<vtkCubeAxesActor> cubeAxesActor =
            vtkSmartPointer<vtkCubeAxesActor>::New();
    cubeAxesActor->AddObserver(vtkCommand::ProgressEvent, pObserver );


    ////////////////////////////////////
    /// \brief RenderWindow
    ////

    vtkRenderer *ren1 = vtkRenderer::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();

    renderWindow->AddRenderer(ren1);

///    // An interactor
//      vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
//          vtkSmartPointer<vtkRenderWindowInteractor>::New();
//      renderWindowInteractor->SetRenderWindow(renderWindow);


    // Set the vtkInteractorStyleSwitch for renderWindowInteractor
    vtkSmartPointer<vtkInteractorStyleSwitch> style =
            vtkSmartPointer<vtkInteractorStyleSwitch>::New();

    //renderWindowInteractor->SetInteractorStyle(style);*/

//    //////////////////////////////////
//    / \brief vtkLookupTable
//    //

//    // create a b/w lookup Table
//    vtkLookupTable * bwLut = vtkLookupTable::New();
//    bwLut->SetTableRange( 0,1000);
//    bwLut->SetSaturationRange(.6,.6);
//    bwLut->SetHueRange(0,1);
//    bwLut->SetValueRange(1,1);

//    vtkImageDataGeometryFilter  *crossSection =
//            vtkImageDataGeometryFilter::New();
//    crossSection->SetExtent(50,55,0,1000,0,30);
//    crossSection->SetInput(fitsReader->GetOutput());
//    vtkPolyDataMapper *crossMapper = vtkPolyDataMapper::New();
//    crossMapper->SetInput(crossSection->GetOutput());
//    //crossMapper->ScalarVisibilityOn(); //Used to show whether scalar data is used to color objects
//    crossMapper->SetScalarRange(0,1000);
//    crossMapper->SetLookupTable(bwLut);
//    vtkActor * crossActor = vtkActor::New();
//    crossActor->SetMapper(crossMapper);

    ////////////////////////////////////
    /// \brief Cube Axes Labels
    ////

    cubeAxesActor->SetCamera(ren1->GetActiveCamera());
    cubeAxesActor->SetBounds(sliceA->GetBounds());
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

//    ren1->AddActor(outlineA);
//    ren1->AddActor(shellA);
//    ren1->AddActor(sliceA);
//    //ren1->AddActor(crossActor);
//    ren1->AddActor(cubeAxesActor);
    //

//    this->ui->qvtkWidgetLeft->SetRenderWindow(renderWindow);
//    this->ui->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
//    this->ui->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetRenderWindow(renderWindow);

//    ////////////////////////////////////
//    /// \brief OrientationMarker Widget
//    ////

//    AddOrientationAxes(this->ui->qvtkWidgetLeft);

//    ////////////////////////////////////
//    /// \brief ScalarBar Widget
//    ////

//    AddScalarBar(this->ui->qvtkWidgetLeft, fitsReader);

//    ////////////////////////////////////
//    /// \brief Highlight Slection Widget
//    ////

//    //XYVolumeSelection(this->ui->qvtkWidgetLeft, fitsReader);









////    // global_Points = tempSet;
//     global_Reader = fitsReader;

//     global_Volume = sliceA;


//     ren1->ResetCamera();
}
