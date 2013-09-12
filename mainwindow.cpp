#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "subVolume.h"
#include "subvolumedialog.h"

#include "OrientationAxes.h"

#include "ScalarBar.h"

#include "XYHighlightSelection.h"


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




#include <QDebug>


void MainWindow::mySlot() {
    qDebug() << "Hello patrick!";
}


void MainWindow::log(QString msg) {
    ui->plainTextEdit->insertPlainText(msg);
    ui->plainTextEdit->ensureCursorVisible();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->ui->centralWidget = ui->qvtkWidgetLeft;

    //connect(ui->button_Rotate, SIGNAL(clicked()), this, SLOT(mySlot()));
    //ui->button_Open-

    //loadFitsFile("OMC.FITS");
}

void MainWindow::slotExit()
{
    qApp->exit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_Exit_clicked()
{
    slotExit();
}

void MainWindow::loadFitsFile(QString filename)
{
    //////////////////////////////////////////////////////////////////
    ///////////VTK FITS READER CODE FROM FITS.CXX ////////////////////
    //////////////////////////////////////////////////////////////////

    // vtk pipeline
    vtkFitsReader *fitsReader = vtkFitsReader::New();
    const char *newFileName = filename.toStdString().c_str();
    fitsReader->SetFileName(newFileName);

    fitsReader->Update();
    fitsReader->Execute();
    //renWin->SetWindowName(fitsReader->GetFileName());

    /////////////////////////////////
    /// \brief Outline Object
    ///

    vtkOutlineFilter *outlineF = vtkOutlineFilter::New();
    outlineF->SetInputConnection(fitsReader->GetOutputPort());

    // Create the Mapper

    vtkPolyDataMapper *outlineM = vtkPolyDataMapper::New();
    outlineM->SetInput(outlineF->GetOutput());
    outlineM->ScalarVisibilityOff();

    // Create the Actor

    vtkActor *outlineA = vtkActor::New();
    outlineA->SetMapper(outlineM);

    ///////////////////////////////
    /// \brief Shell for Objects
    ///

    // isosurface
    vtkMarchingCubes *shellE = vtkMarchingCubes::New();
    shellE->SetInputConnection(fitsReader->GetOutputPort());
    shellE->SetValue(0, 10.0f);

    // decimate
    vtkDecimatePro *shellD = vtkDecimatePro::New();
    shellD->SetInput(shellE->GetOutput());
    shellD->SetTargetReduction(0.7);

    // Create the Mapper

    vtkPolyDataMapper *shellM = vtkPolyDataMapper::New();
    shellM->SetInput(shellE->GetOutput());
    shellM->SetInput(shellD->GetOutput());
    shellM->ScalarVisibilityOff();

    // Create the Actor

    vtkActor *shellA = vtkActor::New();
    shellA->SetMapper(shellM);
    shellA->GetProperty()->SetColor(0.5, 0.5, 1.0);

    /////////////////////////////////////
    /// \brief Slice Object
    ///

    // slice
    vtkImageDataGeometryFilter *sliceE =
            vtkImageDataGeometryFilter::New();
    // values are clamped
    sliceE->SetExtent(0, 1000, 0, 1000, 0, 30);
    sliceE->SetInputConnection(fitsReader->GetOutputPort());

    // Create the Mapper

    vtkPolyDataMapper *sliceM = vtkPolyDataMapper::New();
    sliceM->SetInputConnection(sliceE->GetOutputPort());
    sliceM->ScalarVisibilityOn();
    double *range;

    vtkStructuredPoints *tempSet ;
    tempSet = fitsReader->GetOutput();

    range = fitsReader->GetOutput()->GetScalarRange();
    sliceM->SetScalarRange(range);

    // Create the Actor

    vtkActor *sliceA = vtkActor::New();
    sliceA->SetMapper(sliceM);

    vtkSmartPointer<vtkCubeAxesActor> cubeAxesActor =
            vtkSmartPointer<vtkCubeAxesActor>::New();


    ////////////////////////////////////
    /// \brief RenderWindow
    ////

    vtkRenderer *ren1 = vtkRenderer::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();

    renderWindow->AddRenderer(ren1);    

//    // An interactor
//      vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
//          vtkSmartPointer<vtkRenderWindowInteractor>::New();
//      renderWindowInteractor->SetRenderWindow(renderWindow);


    // Set the vtkInteractorStyleSwitch for renderWindowInteractor
    vtkSmartPointer<vtkInteractorStyleSwitch> style =
            vtkSmartPointer<vtkInteractorStyleSwitch>::New();

    //renderWindowInteractor->SetInteractorStyle(style);

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

    ren1->AddActor(outlineA);
    ren1->AddActor(shellA);
    ren1->AddActor(sliceA);
    ren1->AddActor(cubeAxesActor);
    //

     this->ui->qvtkWidgetLeft->SetRenderWindow(renderWindow);
     this->ui->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
     this->ui->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetRenderWindow(renderWindow);



     ////////////////////////////////////
     /// \brief OrientationMarker Widget
     ////

     AddOrientationAxes(this->ui->qvtkWidgetLeft);

     ////////////////////////////////////
     /// \brief ScalarBar Widget
     ////

     AddScalarBar(this->ui->qvtkWidgetLeft, fitsReader);

     ////////////////////////////////////
     /// \brief Highlight Slection Widget
     ////

     //XYVolumeSelection(this->ui->qvtkWidgetLeft, fitsReader);


     global_Points = tempSet;
     global_Reader = fitsReader;


     ren1->ResetCamera();

}

void MainWindow::loadSubVolume(QVTKWidget *qvtkWidget, vtkFitsReader *source)
{
//    //////////////////////////////////////////////////////////////////
//    ///////////VTK ExtractVOI  CODE FROM EXTRACTVOI.CXX //////////////
//    //////////////////////////////////////////////////////////////////

//    int* inputDims = source->GetOutput()->GetDimensions();
//    qDebug() << "Dims: " << " x: " << inputDims[0]
//                          << " y: " << inputDims[1]
//                          << " z: " << inputDims[2] << endl;
//    qDebug() << "Number of points: " << source->GetOutput()->GetNumberOfPoints() << endl;
//    qDebug() << "Number of cells: " << source->GetOutput()->GetNumberOfCells() << endl;

//    vtkSmartPointer<vtkExtractVOI> extractVOI =
//        vtkSmartPointer<vtkExtractVOI>::New();
//    //extractVOI->SetInputConnection(source->GetOutputPort());
//    extractVOI->SetInputConnection(source->GetOutputPort());
//    extractVOI->SetVOI(inputDims[0]/4.,
//                       10.*inputDims[0]/4.,
//                       inputDims[1]/4.,
//                       10.*inputDims[1]/4.,
//                       inputDims[2]/4.,
//                       10.*inputDims[2]/4.);
//    extractVOI->Update();

//    vtkImageData* extracted = extractVOI->GetOutput();

//    int* extractedDims = extracted->GetDimensions();
//    qDebug() << "Dims: " << " x: " << extractedDims[0]
//                          << " y: " << extractedDims[1]
//                          << " z: " << extractedDims[2] << endl;
//    qDebug() << "Number of points: " << extracted->GetNumberOfPoints() << endl;
//    qDebug() << "Number of cells: " << extracted->GetNumberOfCells() << endl;

//    //////////////////////////////////////////////////////////////////////
//    /// \brief inputCastFilter
//    /////////

//    // outline
//    vtkOutlineFilter *outlineF = vtkOutlineFilter::New();
//    outlineF->SetInputConnection(extractVOI->GetOutputPort());

//    vtkPolyDataMapper *outlineM = vtkPolyDataMapper::New();
//    outlineM->SetInput(outlineF->GetOutput());
//    outlineM->ScalarVisibilityOff();

//    vtkActor *outlineA = vtkActor::New();
//    outlineA->SetMapper(outlineM);

//    // isosurface
//    vtkMarchingCubes *shellE = vtkMarchingCubes::New();
//    shellE->SetInputConnection(extractVOI->GetOutputPort());
//    shellE->SetValue(0, 10.0f);

//    // decimate
//    vtkDecimatePro *shellD = vtkDecimatePro::New();
//    shellD->SetInput(shellE->GetOutput());
//    shellD->SetTargetReduction(2.0);

//    vtkPolyDataMapper *shellM = vtkPolyDataMapper::New();
//    shellM->SetInput(shellE->GetOutput());
//    shellM->SetInput(shellD->GetOutput());
//    shellM->ScalarVisibilityOff();

//    vtkActor *shellA = vtkActor::New();
//    shellA->SetMapper(shellM);
//    shellA->GetProperty()->SetColor(0.5, 0.5, 1.0);

//    // slice
//    vtkImageDataGeometryFilter *sliceE =
//            vtkImageDataGeometryFilter::New();
//    // values are clamped
//    sliceE->SetExtent(0, 1000, 0, 1000, 0, 30);
//    sliceE->SetInputConnection(extractVOI->GetOutputPort());

//    vtkPolyDataMapper *sliceM = vtkPolyDataMapper::New();
//    sliceM->SetInputConnection(sliceE->GetOutputPort());
//    sliceM->ScalarVisibilityOn();
//    double *range;

//    range = extractVOI->GetOutput()->GetScalarRange();
//    sliceM->SetScalarRange(range);

//    vtkActor *sliceA = vtkActor::New();
//    sliceA->SetMapper(sliceM);

//    // Cube Axes Visuazilzer
//    //vtkSmartPointer<vtkCubeA

//    // There will be one render window
//    vtkSmartPointer<vtkRenderWindow> renderWindow =
//      vtkSmartPointer<vtkRenderWindow>::New();
//    renderWindow->SetSize(600, 300);

//    vtkRenderer *ren1 = vtkRenderer::New();

//    renderWindow->AddRenderer(ren1);
//    // add actors to renderer
//    ren1->AddActor(outlineA);
//    ren1->AddActor(shellA);
//    ren1->AddActor(sliceA);

//    // And one interactor
//    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
//      vtkSmartPointer<vtkRenderWindowInteractor>::New();
//    interactor->SetRenderWindow(renderWindow);

//    //this->ui->qvtkWidgetLeft->GetRenderWindow()->(ren1);

//    renderWindow->AddRenderer(ren1);

//    renderWindow->Render();
//    interactor->Start();


//    this->ui->qvtkWidgetLeft->SetRenderWindow(renderWindow);
 //   this->ui->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
 //   this->ui->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetRenderWindow(renderWindow);

    SubVolumeDialog subVol;

    subVol.setModal(true);
    subVol.UpdateQVTKWidget(this->ui->qvtkWidgetLeft, global_Reader );
//    //subVol
//    //subVol->ui->
    subVol.exec();



}


void MainWindow::on_button_Open_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr ("Open File"),"", tr("Files (*.*)"));

    loadFitsFile(fileName);

}

void MainWindow::on_Buttom_SubVol_clicked()
{
    loadSubVolume(this->ui->qvtkWidgetLeft, global_Reader );
}

void MainWindow::on_actionSubVolume_triggered()
{
//    vtkSmartPointer<vtkRenderWindow> renderWindow =
//      vtkSmartPointer<vtkRenderWindow>::New();

    loadSubVolume(this->ui->qvtkWidgetLeft, global_Reader );

  //  SubVolumeDialog subVol;
  //  subVol.setModal(true);
  //  subVol.exec();

    //loadSubVolume(this->ui->qvtkWidgetLeft, global_Reader );
    //subVolDialog = new subVolDialog(this);
    //subVolDialog->show();

}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr ("Open File"),"", tr("Files (*.*)"));

    loadFitsFile(fileName);
}

void MainWindow::on_actionExit_triggered()
{
    slotExit();
}

void MainWindow::on_actionReset_Camera_triggered()
{
    vtkSmartPointer<vtkRenderer> theRenderer =
            vtkSmartPointer<vtkRenderer>::New();

    theRenderer = this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->
            GetRenderers()->GetFirstRenderer();
    

    double  * focalPoint;
    double  * position ;
    double  dist ;

    focalPoint = theRenderer->GetActiveCamera()->GetFocalPoint();
    position = theRenderer->GetActiveCamera()->GetPosition();
    dist =  sqrt(pow((position[0]-focalPoint[0]),2) +
                 pow((position[1]-focalPoint[1]),2) +
                 pow((position[2]-focalPoint[2]),2));
    theRenderer->GetActiveCamera()->SetPosition(focalPoint[0], focalPoint[1], focalPoint[2] + dist);


    theRenderer->GetActiveCamera()->SetViewUp(0.0,1.0,0.0);


    this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();
    //theRenderer->
}

void MainWindow::on_actionLeft_Side_View_triggered()
{
    vtkSmartPointer<vtkRenderer> theRenderer =
            vtkSmartPointer<vtkRenderer>::New();

    theRenderer = this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->
            GetRenderers()->GetFirstRenderer();


    double  * focalPoint;
    double  * position ;
    double  dist ;

    focalPoint = theRenderer->GetActiveCamera()->GetFocalPoint();
    position = theRenderer->GetActiveCamera()->GetPosition();
    dist =  sqrt(pow((position[0]-focalPoint[0]),2) +
                 pow((position[1]-focalPoint[1]),2) +
                 pow((position[2]-focalPoint[2]),2));
    theRenderer->GetActiveCamera()->SetPosition(focalPoint[0], focalPoint[1], focalPoint[2] + dist);

    theRenderer->GetActiveCamera()->SetViewUp(0.0,1.0,0.0);

    theRenderer->GetActiveCamera()->Azimuth(90);


    this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();

}

void MainWindow::on_actionRight_Side_View_triggered()
{
    vtkSmartPointer<vtkRenderer> theRenderer =
            vtkSmartPointer<vtkRenderer>::New();

    theRenderer = this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->
            GetRenderers()->GetFirstRenderer();


    double  * focalPoint;
    double  * position ;
    double  dist ;

    focalPoint = theRenderer->GetActiveCamera()->GetFocalPoint();
    position = theRenderer->GetActiveCamera()->GetPosition();
    dist =  sqrt(pow((position[0]-focalPoint[0]),2) +
                 pow((position[1]-focalPoint[1]),2) +
                 pow((position[2]-focalPoint[2]),2));
    theRenderer->GetActiveCamera()->SetPosition(focalPoint[0], focalPoint[1], focalPoint[2] + dist);

    theRenderer->GetActiveCamera()->SetViewUp(0.0,1.0,0.0);

    theRenderer->GetActiveCamera()->Azimuth(270);


    this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();
}

void MainWindow::on_actionTop_Side_View_triggered()
{
    vtkSmartPointer<vtkRenderer> theRenderer =
            vtkSmartPointer<vtkRenderer>::New();

    theRenderer = this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->
            GetRenderers()->GetFirstRenderer();


    double  * focalPoint;
    double  * position ;
    double  dist ;

    focalPoint = theRenderer->GetActiveCamera()->GetFocalPoint();
    position = theRenderer->GetActiveCamera()->GetPosition();
    dist =  sqrt(pow((position[0]-focalPoint[0]),2) +
                 pow((position[1]-focalPoint[1]),2) +
                 pow((position[2]-focalPoint[2]),2));
    theRenderer->GetActiveCamera()->SetPosition(focalPoint[0], focalPoint[1], focalPoint[2] + dist);

    theRenderer->GetActiveCamera()->SetViewUp(0.0,1.0,0.0);

    theRenderer->GetActiveCamera()->Elevation(90);


    this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();
}
