#include "subvolumedialog.h"
#include "ui_subvolumedialog.h"
//#include "OrientationAxes.h"

#include "vtkExtractVOI.h"
#include "vtkOutlineFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkMarchingCubes.h"
#include "vtkImageDataGeometryFilter.h"
#include "vtkDecimatePro.h"
#include "vtkProperty.h"


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
    delete ui;
}

void SubVolumeDialog::UpdateQVTKWidget(QVTKWidget *qvtkWidget, vtkFitsReader *source)
{
    //this->ui->qvtkWidgetLeft->SetRenderWindow(updatedVolume);

    //////////////////////////////////////////////////////////////////
    ///////////VTK ExtractVOI  CODE FROM EXTRACTVOI.CXX //////////////
    //////////////////////////////////////////////////////////////////

    int* inputDims = source->GetOutput()->GetDimensions();
    qDebug() << "Dims: " << " x: " << inputDims[0]
                          << " y: " << inputDims[1]
                          << " z: " << inputDims[2] << endl;
    qDebug() << "Number of points: " << source->GetOutput()->GetNumberOfPoints() << endl;
    qDebug() << "Number of cells: " << source->GetOutput()->GetNumberOfCells() << endl;

    vtkSmartPointer<vtkExtractVOI> extractVOI =
        vtkSmartPointer<vtkExtractVOI>::New();
    //extractVOI->SetInputConnection(source->GetOutputPort());
    extractVOI->SetInputConnection(source->GetOutputPort());
    extractVOI->SetVOI(inputDims[0]/4.,
                       10.*inputDims[0]/4.,
                       inputDims[1]/4.,
                       10.*inputDims[1]/4.,
                       inputDims[2]/4.,
                       10.*inputDims[2]/4.);
    extractVOI->Update();

    vtkImageData* extracted = extractVOI->GetOutput();

    int* extractedDims = extracted->GetDimensions();
    qDebug() << "Dims: " << " x: " << extractedDims[0]
                          << " y: " << extractedDims[1]
                          << " z: " << extractedDims[2] << endl;
    qDebug() << "Number of points: " << extracted->GetNumberOfPoints() << endl;
    qDebug() << "Number of cells: " << extracted->GetNumberOfCells() << endl;

    //////////////////////////////////////////////////////////////////////
    /// \brief inputCastFilter
    /////////

    // outline
    vtkOutlineFilter *outlineF = vtkOutlineFilter::New();
    outlineF->SetInputConnection(extractVOI->GetOutputPort());

    vtkPolyDataMapper *outlineM = vtkPolyDataMapper::New();
    outlineM->SetInput(outlineF->GetOutput());
    outlineM->ScalarVisibilityOff();

    vtkActor *outlineA = vtkActor::New();
    outlineA->SetMapper(outlineM);

    // isosurface
    vtkMarchingCubes *shellE = vtkMarchingCubes::New();
    shellE->SetInputConnection(extractVOI->GetOutputPort());
    shellE->SetValue(0, 10.0f);

    // decimate
    vtkDecimatePro *shellD = vtkDecimatePro::New();
    shellD->SetInput(shellE->GetOutput());
    shellD->SetTargetReduction(2.0);

    vtkPolyDataMapper *shellM = vtkPolyDataMapper::New();
    shellM->SetInput(shellE->GetOutput());
    shellM->SetInput(shellD->GetOutput());
    shellM->ScalarVisibilityOff();

    vtkActor *shellA = vtkActor::New();
    shellA->SetMapper(shellM);
    shellA->GetProperty()->SetColor(0.5, 0.5, 1.0);

    // slice
    vtkImageDataGeometryFilter *sliceE =
            vtkImageDataGeometryFilter::New();
    // values are clamped
    sliceE->SetExtent(0, 1000, 0, 1000, 0, 30);
    sliceE->SetInputConnection(extractVOI->GetOutputPort());

    vtkPolyDataMapper *sliceM = vtkPolyDataMapper::New();
    sliceM->SetInputConnection(sliceE->GetOutputPort());
    sliceM->ScalarVisibilityOn();
    double *range;

    range = extractVOI->GetOutput()->GetScalarRange();
    sliceM->SetScalarRange(range);

    vtkActor *sliceA = vtkActor::New();
    sliceA->SetMapper(sliceM);

    // Cube Axes Visuazilzer
    //vtkSmartPointer<vtkCubeA

    // There will be one render window
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetSize(600, 300);

    vtkRenderer *ren1 = vtkRenderer::New();

    renderWindow->AddRenderer(ren1);
    // add actors to renderer
    ren1->AddActor(outlineA);
    ren1->AddActor(shellA);
    ren1->AddActor(sliceA);

    // And one interactor
    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);

    //this->ui->qvtkWidgetLeft->GetRenderWindow()->(ren1);

    //renderWindow->AddRenderer(ren1);

    this->ui->qvtkWidgetRight->GetRenderWindow()->AddRenderer(ren1);

    //renderWindow->Render();
    //interactor->Start();
    //AddScalarBar(this->ui->qvtkWidgetLeft, fitsReader);
    //AddOrientationAxes(this->ui->qvtkWidgetLeft);
}
