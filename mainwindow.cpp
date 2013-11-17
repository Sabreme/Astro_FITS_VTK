#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "main.h"

#include "subVolume.h"
#include "subvolumedialog.h"

#include "sliceDialog.h"

#include "OrientationAxes.h"

#include "ScalarBar.h"

#include "XYHighlightSelection.h"
#include "ZHighlightSelection.h"
#include "vtkBoxWidget.h"

#include "PlaneSelection.h"


#include <QFileDialog>
#include <string>
#include <vtkVersion.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkInteractorStyleSwitch.h>

#include <vtkImageResample.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeRayCastFunction.h>
#include <vtkBoxWidget.h>

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
#include <QThread>
#include <thread>

#include <QTimer>

#include <vtkCallbackCommand.h>


#include "vtkFrameRateWidget.h"

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

void CallbackFunction(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* clientData, void* vtkNotUsed(callData) )
{
  vtkRenderer* renderer = static_cast<vtkRenderer*>(caller);

  double timeInSeconds = renderer->GetLastRenderTimeInSeconds();
  double fps = 1.0/timeInSeconds;
  std::cout << "FPS: " << fps << std::endl;

  std::cout << "Callback" << std::endl;
}


void MainWindow::mySlot() {
    qDebug() << "Hello patrick!";
}


void MainWindow::log(QString msg) {
    ui->plainTextEdit->insertPlainText(msg);
    ui->plainTextEdit->ensureCursorVisible();
}

///
///Constructor
///
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->ui->centralWidget = ui->qvtkWidgetLeft;
    this->vtkWidget = ui->qvtkWidgetLeft;               // Used for FrameRate Calculation

    QTimer* timer = new QTimer(this);
    timer->setInterval(1000/60);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateme()));
    timer->start();

//    this->ProgressDialog = new QProgressDialog();

//    connect (&this->FutureWatcher, SIGNAL(finished()), this, SLOT(slot_Load_Finished()));
//    connect (&this->FutureWatcher, SIGNAL(finished()), this->ProgressDialog, SLOT(cancel()));
//    connect (&this->ProgressDialog, SIGNAL(cancel()), &this->FutureWatcher, SLOT(slot_Load_Finished()));


    //connect(ui->button_Rotate, SIGNAL(clicked()), this, SLOT(mySlot()));
    //ui->button_Open-

    //loadFitsFile("OMC.FITS");
}

void MainWindow::updateme() {
        ui->qvtkWidgetLeft->GetRenderWindow()->Render();
}

void MainWindow::slotExit()
{
    qApp->exit();
}

MainWindow::~MainWindow()
{
    //delete
    delete ui;
}

void MainWindow::button_Exit_clicked()
{
    slotExit();
}

void MainWindow::slot_LoadFile()
{

}

void MainWindow::loadFitsFile(QString filename)
{
    //////////////////////////////////////////////////////////////////
    ///////////VTK FITS READER CODE FROM FITS.CXX ////////////////////
    //////////////////////////////////////////////////////////////////   

    //moveToThread(QApplication::instance()->thread());
    //qRegisterMetaType<QTextCursor>("QTextCursor");

    vtkCommand* pObserver = vtkProgressCommand::New();

    // vtk pipeline
    vtkFitsReader *fitsReader = vtkFitsReader::New();
    const char *newFileName = filename.toStdString().c_str();
    fitsReader->SetFileName(newFileName);
    fitsReader->AddObserver(vtkCommand::ProgressEvent, pObserver );

     qDebug() << "Obtained Filename" << endl;
    //fitsReader->PrintSelf();
    fitsReader->PrintDetails();
    QApplication::processEvents();

    fitsReader->Update();

    qDebug() << "Gathering Points" << endl;

    fitsReader->Execute();

    qDebug() << "Processing Points" << endl;


    /////////////////////////////////
    /// \brief Outline Object
    ///
    qDebug() << "Adding Outline" << endl;
    vtkOutlineFilter *outlineF = vtkOutlineFilter::New();
    outlineF->AddObserver(vtkCommand::ProgressEvent, pObserver );
    outlineF->SetInputConnection(fitsReader->GetOutputPort());


    ///
    /// \brief resample
    ///
    vtkImageResample *resample = vtkImageResample::New();

    resample->SetInputConnection( fitsReader->GetOutputPort() );
    resample->SetAxisMagnificationFactor(0, 1.0);
    resample->SetAxisMagnificationFactor(1, 1.0);
    resample->SetAxisMagnificationFactor(2, 1.0);

    /// Create the Volume & mapper

    vtkVolume *volume = vtkVolume::New();
    vtkGPUVolumeRayCastMapper *mapper = vtkGPUVolumeRayCastMapper::New();

    //mapper->SetInputConnection(fitsReader->GetOutputPort());
    mapper->SetInputConnection(resample->GetOutputPort());

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
    //property->SetIndependentComponents(1);//

    /// WHITE contrast
//    colorFun->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0 );

//    /// Black and White
//    colorFun->AddRGBSegment(opacityLevel - 0.5*opacityWindow, 0.0, 0.0, 0.0,
//                            opacityLevel + 0.5*opacityWindow, 1.0, 1.0, 1.0 );
//    ///
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

    /// Create the Actor

    vtkSmartPointer<vtkCubeAxesActor> cubeAxesActor =
            vtkSmartPointer<vtkCubeAxesActor>::New();
    //cubeAxesActor->AddObserver(vtkCommand::ProgressEvent, pObserver );


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


    //renderWindowInteractor->SetInteractorStyle(style);*/

    ////////////////////////////////////
    /// \brief Cube Axes Labels
    ////
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

    //cubeAxesActor->DrawXGridpolysOn();
    //cubeAxesActor->



    ren1->AddActor(cubeAxesActor);

    // add actors to renderer

    qDebug() << "Adding Objects to RenderWindow" << endl;

    ren1->AddVolume(volume);


//    vtkSmartPointer<vtkCallbackCommand> callback =
//        vtkSmartPointer<vtkCallbackCommand>::New();

//      callback->SetCallback(CallbackFunction);
//      ren1->AddObserver(vtkCommand::EndEvent, callback);


    this->ui->qvtkWidgetLeft->SetRenderWindow(renderWindow);
    this->ui->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
    this->ui->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetRenderWindow(renderWindow);

    ////////////////////////////////////
    /// \brief OrientationMarker Widget
    ////
    qDebug() << "Adding Orientation Marker" << endl;

    AddOrientationAxes(this->ui->qvtkWidgetLeft);

    ////////////////////////////////////
    /// \brief ScalarBar Widget
    ////
    qDebug() << "Adding ScalarBar" << endl;

    AddScalarBar(this->ui->qvtkWidgetLeft, fitsReader);



////    // global_Points = tempSet;
     global_Reader = fitsReader;

  //   global_Volume = sliceA;

     qDebug() << "Resetting Camera" << endl;
     ren1->ResetCamera();
     qDebug() << "Complete" << endl;


//    this->ProgressDialog->close();

     ////////////////////////////////////
     /// \brief FrameRate Widget
     ////
     qDebug() << "Adding FrameRate" << endl;
     reloadFrameRate();

//     ////////////////////////////////////
//     /// \brief Planeselection Widget
//     ////
//     qDebug() << "Adding FrameRate" << endl;
//     PlaneSelection(this->ui->qvtkWidgetLeft, fitsReader);
}

void MainWindow::loadSubVolume(QVTKWidget *qvtkWidget, vtkFitsReader *source)
{
    SubVolumeDialog subVol;

    subVol.setModal(true);
    subVol.UpdateQVTKWidget(this->ui->qvtkWidgetLeft, global_Reader, global_subVolume );
//    //subVol
//    //subVol->ui->
    subVol.exec();
}


void MainWindow::button_Open_clicked()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr ("Open File"),"", tr("Files (*.*)"));


    //Thread

    //QFuture<void> future = QtConcurrent::run(this->ProcessFileObject, &ProcessFile::loadFitsFile, fileName);
    //QFuture<void> future = QtConcurrent::run(this->loadFitsFile, fileName);
    //this->FutureWatcher.setFuture(future);

    this->ProgressDialog->setMinimum(0);
    this->ProgressDialog->setMaximum(0);
    this->ProgressDialog->setWindowModality(Qt::WindowModal);
    this->ProgressDialog->exec();

    //loadFitsFile(fileName);

}

void MainWindow::Buttom_SubVol_clicked()
{
    loadSubVolume(this->ui->qvtkWidgetLeft, global_Reader );
}


//void MainWindow::LongFunction(int total)
//{
//    for(int count=0; count < total; count++)
//    {
//        sleep(1);
//        std::cout << "Ping Long!" << std::endl;
//    }
//}

//void MainWindow::sleep(time_t delay)
//{
//    time_t timer0, timer1;
//    time(&timer0);
//    do
//    {
//        time(&timer1);
//    }
//    while ((timer1 - timer0) < delay);
//}

void MainWindow::on_actionOpen_triggered()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr ("Open File"),"", tr("Files (*.*)"));

    //Thread

//    connect (&this->FutureWatcher, SIGNAL(finished()), this, SLOT(slot_Load_Finished()));
//    connect (&this->FutureWatcher, SIGNAL(canceled()), this->ProgressDialog, SLOT(cancel()));
//    //connect (&this->ProgressDialog, SIGNAL(cancel()),  FutureWatcher, SLOT(cancel()));

//    //QFuture<void> future = QtConcurrent::run(this->ProcessFileObject, &ProcessFile::loadFitsFile, fileName);
//    QFuture<void> future = QtConcurrent::run(this, &MainWindow::loadFitsFile,fileName);
//    //QFuture<void> future = QtConcurrent::run(this->ProgressDialog, QProgressDialog::exec());
//    this->FutureWatcher.setFuture(future);

//    this->ProgressDialog->setMinimum(0);
//    this->ProgressDialog->setMaximum(0);
//    this->ProgressDialog->setWindowModality(Qt::WindowModal);

//    this->ProgressDialog->exec();

//    this->ProgressDialog = new QProgressDialog();
//    this->ProgressDialog->setMinimum(0);
//    this->ProgressDialog->setMaximum(0);
//    this->ProgressDialog->setWindowModality(Qt::WindowModal);





   // std::thread(&QProgressDialog::exec, this->ProgressDialog).detach();

   // std::thread(&MainWindow::loadFitsFile, this, fileName);

    //this->ProgressDialog->show();


    //QFuture<int> future = QtConcurrent::run(ProgressDialog, &QProgressDialog::exec);
    //this->FutureWatcher.setFuture(future);
    loadFitsFile(fileName);


    //this->FutureWatcher.waitForFinished();
}

void MainWindow::slot_Load_Finished()
{
    qDebug() << "File Loaded " << endl  ;
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

void MainWindow::on_actionBottom_Side_View_triggered()
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

    theRenderer->GetActiveCamera()->Elevation(270);


    this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();
}

void MainWindow::on_actionSliceSelection_triggered()
{
    PlaneSelection(this->ui->qvtkWidgetLeft, global_Reader);
     //AddScalarBar(this->ui->qvtkWidgetLeft, fitsReader);
}

void MainWindow::on_actionBlack_White_triggered()
{
    // create a b/w lookup Table
    vtkLookupTable * bwLut = vtkLookupTable::New();
    bwLut->SetTableRange( 0,1000);
    bwLut->SetSaturationRange(0,0);
    bwLut->SetHueRange(0,0);
    bwLut->SetValueRange(0,1);

    //global_Volume->GetMapper()->SetLookupTable(bwLut);
    this->ui->qvtkWidgetLeft->update();
}

void MainWindow::actionDefault_triggered()
{

    //vtkPolyDataMapper *shellM = vtkPolyDataMapper::New();
    //shellM->SetInputConnection(global_Volume->GetMapper()->getin());
    //global_Volume->GetMapper()->SetLookupTable(default_Volume_Colours);
    this->ui->qvtkWidgetLeft->update();
}

void MainWindow::actionBlue_Red_triggered()
{
    vtkLookupTable * bwLut = vtkLookupTable::New();
    bwLut->SetTableRange( 0,1000);
    bwLut->SetSaturationRange(1,1);
    bwLut->SetHueRange(0,1);
    bwLut->SetValueRange(1,1);

    this->ui->qvtkWidgetLeft->update();
}

void MainWindow::on_actionSubVolumeXY_triggered()
{

    on_actionReset_Camera_triggered();

    XYVolumeSelection(this->ui->qvtkWidgetLeft, global_Reader);

    //loadSubVolume(this->ui->qvtkWidgetLeft, global_Reader );
}

void MainWindow::on_action_SubVolume_Z_triggered()
{
    on_actionLeft_Side_View_triggered();

    ZVolumeSelection(this->ui->qvtkWidgetLeft, global_Reader);
}

void MainWindow::on_actionPreview_triggered()
{
    double minX, maxX, minY, maxY, minZ, maxZ;

    minX = GetMinXBound();
    maxX = GetMaxXBound();
    minY = GetMinYBound();
    maxY = GetMaxYBound();
    minZ = GetMinZBound();
    maxZ = GetMaxZBound();

    vtkSmartPointer<vtkCubeSource> subVolume =
            vtkSmartPointer<vtkCubeSource>::New();

    //#if VTK_MAJOR_VERSION <= 5
    subVolume->SetInput(global_Reader->GetOutput());

    vtkSmartPointer<vtkPolyDataMapper> subVolMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();

    subVolMapper->SetInputConnection(subVolume->GetOutputPort());
    vtkSmartPointer<vtkActor>subVolActor =
            vtkSmartPointer<vtkActor>::New();

    subVolume->SetBounds(minX, maxX, minY, maxY, minZ, maxZ);

    subVolActor->SetMapper(subVolMapper);
    subVolActor->GetProperty()->SetColor(1,1,1);
    subVolActor->GetProperty()->SetOpacity(0.5);


    this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->
                GetRenderers()->GetFirstRenderer()->AddActor(subVolActor);

    this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();

    global_subVolume = subVolume;


    std::cout << "minX: " << minX << std::endl;

}

void MainWindow::on_actionSubVol_Export_triggered()
{
    SubVolumeDialog subVol;

    subVol.setModal(true);
    subVol.UpdateQVTKWidget(this->ui->qvtkWidgetLeft, global_Reader, global_subVolume );
    //    //subVol
    //    //subVol->ui->
    subVol.exec();
}

void MainWindow::on_actionSliceAxisAligned_triggered()
{
    SliceDialog slicer;

    slicer.setModal(true);
    slicer.UpdateQVTKWidget(this->ui->qvtkWidgetLeft, global_Reader);
    slicer.exec();
}
