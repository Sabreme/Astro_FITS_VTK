#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "main.h"
#include "SubVolume/subvolumedialog.h"

#include "Slicing/sliceDialog.h"
#include "System/OrientationAxes.h"
#include "System/ScalarBar.h"

#include "SubVolume/XYHighlightSelection.h"
#include "SubVolume/ZHighlightSelection.h"

//#include "vtkBoxWidget.h"
#include "SubVolume/vtkCustomBoxWidget.h"
#include "vtkCellPicker.h"

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
//#include <vtkBoxWidget.h>

#include <vtkActor2DCollection.h>
#include <vtkTextProperty.h>
#include <vtkActorCollection.h>

#include <vtkTransform.h>
#include <vtkLinearTransform.h>
#include <vtkPerspectiveTransform.h>
#include <math.h>

#include <vtkVolume.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkStructuredPointsToPolyDataFilter.h>

#include <vtkPlaneCollection.h>
#include <vtkStructuredPointsGeometryFilter.h>

#include <QDebug>
#include <QThread>
#include <thread>

#include "vtkResliceCursorLineRepresentation.h"
#include "vtkResliceCursorThickLineRepresentation.h"
#include "vtkResliceCursorWidget.h"
#include "vtkResliceCursorActor.h"
#include "vtkResliceCursorPolyDataAlgorithm.h"

#include "vtkDistanceRepresentation2D.h"
#include "vtkPointHandleRepresentation3D.h"
#include "vtkPointHandleRepresentation2D.h"
#include "vtkDistanceWidget.h"
#include "vtkDistanceRepresentation.h"
#include "vtkHandleRepresentation.h"
#include "vtkResliceImageViewerMeasurements.h"

#include "vtkInteractorStyleImage.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkBoundedPlanePointPlacer.h"
#include "vtkImageSlabReslice.h"
#include "vtkWindowToImageFilter.h"
#include "vtkPNGWriter.h"

#include "vtkCoordinate.h"
#include "vtkWorldPointPicker.h"
#include "vtkLineWidget2.h"
#include "vtkLineRepresentation.h"
#include <vtkPointWidget.h>
#include <vtkViewport.h>
#include <vtkPropPicker.h>

#include <QTimer>

#include <Leap/Sample.h>
#include "System/vtkFrameRateWidget.h"

#include "vtkEventQtSlotConnect.h"

#include "vtkPlaneSource.h"
#include "vtkAbstractTransform.h"
#include "vtkImplicitPlaneWidget2.h"
#include "vtkImplicitPlaneRepresentation.h"

#include "vtkCutter.h"

#include "vtkMath.h"
#include "vtkTransformPolyDataFilter.h"

#include "vtkSliderRepresentation3D.h"

#include "vtkSliderRepresentation2D.h"
#include "vtkSliderWidget.h"
#include "vtkProperty2D.h"

#include "InteractionClasses.h"

#ifdef __WIN32__
#include "windows.h"
#endif

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///                MAIN APPLICATION
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

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

    ///MAIN WINDOW GUI LAYOUT CONFIGURATION
    ///                     |                        |
    /// LEFT Window | QVTKWIDGET    | RIGHT PANEL
    ///                      |      <---->          |
    ///                      |                        |
    QHBoxLayout* guiLayout = new QHBoxLayout;
    this->ui->Frame_LEFT->setFixedWidth(151);
    this->ui->Frame_RIGHT->setFixedWidth(371);
    guiLayout->addWidget(this->ui->Frame_LEFT);
    guiLayout->addWidget(this->ui->qvtkWidgetLeft);
    guiLayout->addWidget(this->ui->Frame_RIGHT);
    centralWidget()->setLayout(guiLayout);

    /// MODE BUTTON GROUPING
    ///
    ///
    QButtonGroup* groupModeButtons = new QButtonGroup(this->ui->TopModeFrame);
    groupModeButtons->addButton(this->ui->buttonModeMouse);
    groupModeButtons->addButton(this->ui->buttonModeLeap);
    groupModeButtons->addButton(this->ui->buttonModeTouch);
    groupModeButtons->setExclusive(true);




    //// Resize Window Handler
    resizeTimer.setSingleShot(true);
    connect (&resizeTimer, SIGNAL(timeout()),SLOT (resizeDone()));

    cameraAzimuth = 0;
    cameraElevation = 0;
    this->controller_ = NULL;

    this->systemMode = Touch;
    this->systemTab =  Information;
    this->systemTransF = Rotation;

    this->infoTabOpen = false;



    QTimer* timer = new QTimer(this);
    timer->setInterval(1000/60);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateme()));
    connect(this->ui->tabLogWidget, SIGNAL(tabCloseRequested(int)),this, SLOT(closeTab(int)));

    timer->start();

    this->ui->tabLogWidget->setVisible(false);
    this->ui->LeftCameraFrame->setVisible(false);
    this->ui->TopMethodFrame->setVisible(false);
    this->ui->TopModeFrame->setVisible(false);

    buttonEnablerTimer.setSingleShot(true);
    buttonEnablerTimer.setInterval(150);
    connect(this->ui->line_Scale, SIGNAL(textChanged(const QString&)),this,SLOT(scaleButtonChanged()));
    connect(&buttonEnablerTimer, SIGNAL(timeout()), this, SLOT(scaleButtonDelay()));

    //this->on_buttonModeMouse_clicked();
    /////
    /// //////////////////////////////////////////////////////////
    /// //////////////////////////////////////////////////////////
    ///// STYLE SHEET CUSTOMIZATION FOR THE APPLICATION
    /// //////////////////////////////////////////////////////////
    ///
    ///


    this->ui->menuBar->setStyleSheet("QMenuBar::item { background: black; color: white}");
    //    this->ProgressDialog = new QProgressDialog();

    //    connect (&this->FutureWatcher, SIGNAL(finished()), this, SLOT(slot_Load_Finished()));
    //    connect (&this->FutureWatcher, SIGNAL(finished()), this->ProgressDialog, SLOT(cancel()));
    //    connect (&this->ProgressDialog, SIGNAL(cancel()), &this->FutureWatcher, SLOT(slot_Load_Finished()));


    //connect(ui->button_Rotate, SIGNAL(clicked()), this, SLOT(mySlot()));
    //ui->button_Open-

    //QString filename = "OMC.FITS";
    //loadFitsFile(filename);


}

void MainWindow::closeTabs (){
    for (int i=0; i < TabCount; i++)
    {
        // QApplication::sendEvent(this->ui->tabLogWidget, this->ui->tabLogWidget->closeEvent(;)
        //this->ui->tabLogWidget->setCurrentIndex(i);
        //        QMetaObject::invokeMethod(this->ui->tabLogWidget->tabCloseRequested(i));
        int currentIndex = this->ui->tabLogWidget->currentIndex();

        //this->ui->tabLogWidget-;
        this->ui->tabLogWidget->removeTab(currentIndex);

        this->ui->tabLogWidget->update();
    }

}

void MainWindow::mySlot() {
    qDebug() << "Hello patrick!";
}


void MainWindow::log(QString msg) {
    ui->plainTextEdit_Leap->insertPlainText(msg);
    ui->plainTextEdit_Leap->ensureCursorVisible();
}

void MainWindow::infoTabCloseSignal()
{
    this->infoTabOpen = false;
    this->ui->actionInfoBarToggle->setChecked(false);
}

bool MainWindow::MessageBoxQuery(QString title, QString question)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(question);
    msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    if (msgBox.exec() == QMessageBox::Yes)
        return true;
    else
        return false;
}

void MainWindow::releaseTabFocus()
{
    switch (this->systemTab)
    {
    case Information:   this->ui->buttonTabInfo->setEnabled(true);
        this->ui->InfoTab->close();
        break;

    case SubVolume:   this->ui->buttonTabSubVol->setEnabled(true);
        this->ui->SubVolTab->close();
        break;

    case SliceAxis:   this->ui->buttonTabSliceAxis->setEnabled(true);
        this->ui->SliceVolTab->close();
        break;

    case SliceArb:   this->ui->buttonTabSliceArb->setEnabled(true);
        this->ui->SliceVolTab2->close();
        break;
    }

    //////////////////////////////////////
    /////////////////////////////////////
    this->on_actionReload_triggered();
}

void MainWindow::resizeDone()
{
    this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();
}

double MainWindow::getDefaultCameraPos(int dimension)
{

}

void MainWindow::updateCameraPosition()
{

//    std::cout <<"UPDATE POSITION" << endl;

    vtkRenderer * renderer = this->defaultRenderer;
    vtkCamera *camera = renderer->GetActiveCamera();

    double* position;

    position = camera->GetPosition();

    double relativePos[3];

    relativePos[0] = position[0] - defaultCameraPosition[0] ;
    relativePos[1] = position[1] - defaultCameraPosition[1] ;
    relativePos[2] = position[2] - defaultCameraPosition[2] ;

    //std::cout << relativePos[1] <<  endl;

//    if (relativePos[0] == -0)
//        relativePos[0] = 0;

//    if (relativePos[1] == -0)
//        relativePos[1] = 0;

//    if (relativePos[2] == -0)
//        relativePos[2] = 0;

//    ui->line_PosX->setText(QString::number(position[0], 'f', 0));
//    ui->line_PosY->setText(QString::number(position[1], 'f', 0));
//    ui->line_PosZ->setText(QString::number(position[2], 'f', 0));

//    ui->line_PosX->setText(QString::number(relativePos[0], 'd', 0));
//    ui->line_PosY->setText(QString::number(relativePos[1], 'd', 0));
//    ui->line_PosZ->setText(QString::number(relativePos[2], 'd', 0));

    ui->line_PosX->setText(QString::number((int) relativePos[0], 10));
    ui->line_PosY->setText(QString::number((int) relativePos[1], 10));
    ui->line_PosZ->setText(QString::number((int) relativePos[2], 10));
}

void MainWindow::startUserTest()
{
    int currentTask = userTest->getCurrentTask();
    QString taskTitle = "";

    switch(currentTask)
    {
    case 4 : taskTitle = "Task 1"; break;
    case 5 : taskTitle = "Task 2"; break;
    case 6 : taskTitle = "Task 3"; break;
    }

    int currentJob = userTest->getCurrentJob();
    QString jobTitle = "";

    switch (currentJob)
    {
    case 0 : jobTitle = "Transformations"; break;
    case 1 : jobTitle = "Sub-Volume Extraction"; break;
    case 2 : jobTitle = "Arbitrary - Slice"; break;
    }

    userTestDlg = new UserTestDialog(this);
    userTestDlg->setAttribute(Qt::WA_DeleteOnClose);
    userTestDlg->setWindowTitle(jobTitle);
    userTestDlg->setTaskLabel(taskTitle);


    userTestDlg->setCurrentJob(currentJob);

    int currentPrototype = userTest->getCurrentPrototype();
    userTestDlg->setCurrentPrototype(currentPrototype);


    QPoint ptRenderer = this->ui->qvtkWidgetLeft->geometry().topRight();
    QPoint ptMainWindow = this->geometry().topLeft();
    QPoint ptTopBar = this->ui->menuBar->geometry().bottomLeft();

    ptRenderer += ptMainWindow;

    int startPosY = ptRenderer.y() + ptTopBar.y() + 5;
    int startPosX = ptRenderer.x() - (userTestDlg->geometry().width() + 5);
    userTestDlg->move(startPosX, startPosY);

    QObject::connect(userTestDlg,SIGNAL(stopTest()),this,SLOT(stopUserTest()));
    QObject::connect(userTestDlg,SIGNAL(saveScreen()),this, SLOT(saveScreenShot()));

    userTestDlg->show();


    userTestActive = true;

    /////////////////////////
    //// FOR QVTKWIDGET TOUCH TEST TRACKING
    ///
    this->ui->qvtkWidgetLeft->setUserTestMode(true);

    switch(currentJob)
    {
    ///////////////////////////////////////////////////
    /////   TRANSFORMATION JOB
    ///
    case 0 :
    {
        this->releaseTabFocus();
        this->systemTab = Information;

        this->ui->buttonTabInfo->setDisabled(true);

        this->infoTab_Triggered();
        /// Refresh the Interaction screen.
        //this->ui->qvtkWidgetLeft->GetRenderWindow()->Render();
        on_actionReset_Camera_triggered();

        switch (systemMode)
        {
        case Mouse:
        {
            setMouseInteractor();
        }
            break;
        case Touch:
        {
            setTouchInteractor();

            this->ui->qvtkWidgetLeft->enableGestures();
            connect(this->ui->qvtkWidgetLeft, SIGNAL(scaleTriggered()), userTestDlg,SLOT(incScaling()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(translateTriggered()), userTestDlg,SLOT(incTranslation()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(rotateTriggered()),userTestDlg,SLOT(incRotation()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(zRotateTriggered()), userTestDlg, SLOT(incZRotation()));

            this->ui->qvtkWidgetLeft->setTransformsOn(true);
            this->ui->qvtkWidgetLeft->SetArbSliceOn(false);

            this->ui->qvtkWidgetLeft->SetSubVolumeOn(false);

            this->ui->buttonTransformActive->setChecked(true);


            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Pressed()), this, SLOT(touchFinger1SubVol()));
            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Pressed()), this, SLOT(touchFinger2SubVol()));
            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(rightClkSubVol()), this, SLOT(touchSubVolTranslate()));
            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Released()), this, SLOT(touchFingerReleased()));

            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Pressed()), this, SLOT(touchFinger1ArbSlicePressed()));
            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Moving()), this, SLOT(touchFinger1ArbSliceMoving()));
            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Released()), this, SLOT(touchFinger1ArbSliceReleased()));

            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Pressed()), this, SLOT(touchFinger2ArbSlicePressed()));
            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Moving()), this, SLOT(touchFinger2ArbSliceMoving()));
            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Released()), this, SLOT(touchFInger2ArbSliceReleased()));

        }
            break;
        case Leap:
        {
           setLeapInteractor();
           this->ui->buttonTransformActive->setChecked(true);
        }
            break;
        }
    }   // CASE 0
        break;
        ///////////////////////////////////////////////////
        /////   SUB VOLUME JOB
        ///
    case 1 :
    {
        this->releaseTabFocus();

        this->systemTab = SubVolume;
        this->ui->buttonTabSubVol->setDisabled(true);
        this->on_actionSubVolSelection_triggered();

        switch (this->systemMode)
        {
        case Mouse:
        {
            this->mouseBeginSubVol();
            this->ui->Frame_SubVolLeapTracking->setVisible(false);
            setMouseInteractor();
        }
            break;

        case Leap:
        {
            this->leapBeginSubVol();
            this->ui->buttonTransformActive->setChecked(false);
            }
            break;

        case Touch:
        {
            this->ui->Frame_SubVolLeapTracking->setVisible(false);
            this->touchBeginSubVol();

            /////////////////////////////////////////////////
            ///  Touch Interaction
            ///
            ///

            setTouchInteractor();

            this->ui->qvtkWidgetLeft->enableGestures();
            connect(this->ui->qvtkWidgetLeft, SIGNAL(scaleTriggered()), userTestDlg,SLOT(incScaling()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(translateTriggered()), userTestDlg,SLOT(incTranslation()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(rotateTriggered()),userTestDlg,SLOT(incRotation()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(zRotateTriggered()), userTestDlg, SLOT(incZRotation()));

            this->ui->qvtkWidgetLeft->setTransformsOn(false);
            this->ui->qvtkWidgetLeft->SetArbSliceOn(false);

            this->ui->qvtkWidgetLeft->SetSubVolumeOn(true);

            this->ui->buttonTransformActive->setChecked(false);


            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Pressed()), this, SLOT(touchFinger1SubVol()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Pressed()), this, SLOT(touchFinger2SubVol()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(rightClkSubVol()), this, SLOT(touchSubVolTranslate()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Released()), this, SLOT(touchFingerReleased()));


        }
            break;
        }
    }   // CASE 1
        this->ui->qvtkWidgetLeft->setFocus();
        break;

        ///////////////////////////////////////////////////
        /////   ARB SLICE JOB
        ///
    case 2 :
    {
        this->releaseTabFocus();

        this->systemTab = SliceArb;
        this->ui->buttonTabSliceArb->setDisabled(true);
        this->on_actionSliceAxisArbitrary_triggered();

        switch (this->systemMode)
        {
        case Mouse:
        {
            this->beginSliceArb();

            setMouseInteractor();
        }

            break;

        case Leap:

            this->leapBeginSliceArb();
            break;

        case Touch:
        {
            this ->beginSliceArb();

            setTouchInteractor();

            this->ui->qvtkWidgetLeft->enableGestures();                        

            this->customArbPlaneWidget->GetInteractor()->RemoveAllObservers();

            connect(this->ui->qvtkWidgetLeft, SIGNAL(scaleTriggered()), userTestDlg,SLOT(incScaling()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(translateTriggered()), userTestDlg,SLOT(incTranslation()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(rotateTriggered()),userTestDlg,SLOT(incRotation()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(zRotateTriggered()), userTestDlg, SLOT(incZRotation()));

            this->ui->buttonTransformActive->setChecked(false);


            this->ui->qvtkWidgetLeft->setTransformsOn(false);

            this->ui->qvtkWidgetLeft->SetSubVolumeOn(false);

            this->ui->qvtkWidgetLeft->SetArbSliceOn(true);

            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Pressed()), this, SLOT(touchFinger1ArbSlicePressed()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Moving()), this, SLOT(touchFinger1ArbSliceMoving()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Released()), this, SLOT(touchFinger1ArbSliceReleased()));

            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Pressed()), this, SLOT(touchFinger2ArbSlicePressed()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Moving()), this, SLOT(touchFinger2ArbSliceMoving()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Released()), this, SLOT(touchFInger2ArbSliceReleased()));


        }
            break;
        }
    }   // CASE 2
        break;
        this->ui->qvtkWidgetLeft->setFocus();
    }
    this->ui->qvtkWidgetLeft->setFocus();
    //this->userTestDlg->res
}

void MainWindow::startUserPractice()
{

    userPractice = new countDialog(this);
    userPractice->setAttribute(Qt::WA_DeleteOnClose);
    userPractice->setWindowTitle("Practice Session");
  ///  userPractice->setWindowTitle(jobTitle);
  ///  userPractice->setTaskLabel(taskTitle);


    QPoint ptRenderer = this->ui->qvtkWidgetLeft->geometry().topRight();
    QPoint ptMainWindow = this->geometry().topLeft();
    QPoint ptTopBar = this->ui->menuBar->geometry().bottomLeft();

    ptRenderer += ptMainWindow;

    int startPosY = ptRenderer.y() + ptTopBar.y() + 5;
    int startPosX = ptRenderer.x() - (userPractice->geometry().width() + 5);
    userPractice->move(startPosX, startPosY);

    QObject::connect(userPractice,SIGNAL(stopTest()),this,SLOT(stopUserPractice()));

    userPractice->show();




//    /////////////////////////
//    //// FOR QVTKWIDGET TOUCH TEST TRACKING
//    ///
//    this->ui->qvtkWidgetLeft->setUserTestMode(true);

//    switch(currentJob)
//    {
//    ///////////////////////////////////////////////////
//    /////   TRANSFORMATION JOB
//    ///
//    case 0 :
//    {
//        this->releaseTabFocus();
//        this->systemTab = Information;

//        this->ui->buttonTabInfo->setDisabled(true);

//        this->infoTab_Triggered();
//        /// Refresh the Interaction screen.
//        this->ui->qvtkWidgetLeft->GetRenderWindow()->Render();

//        switch (systemMode)
//        {
//        case Mouse:
//        {
//            setMouseInteractor();
//        }
//            break;
//        case Touch:
//        {
//            setTouchInteractor();

//            this->ui->qvtkWidgetLeft->enableGestures();
//            connect(this->ui->qvtkWidgetLeft, SIGNAL(scaleTriggered()), userTestDlg,SLOT(incScaling()));
//            connect(this->ui->qvtkWidgetLeft, SIGNAL(translateTriggered()), userTestDlg,SLOT(incTranslation()));
//            connect(this->ui->qvtkWidgetLeft, SIGNAL(rotateTriggered()),userTestDlg,SLOT(incRotation()));
//        }
//            break;
//        case Leap:
//        {
//           setLeapInteractor();
//        }
//            break;
//        }
//    }   // CASE 0
//        break;
//        ///////////////////////////////////////////////////
//        /////   SUB VOLUME JOB
//        ///
//    case 1 :
//    {
//        this->releaseTabFocus();

//        this->systemTab = SubVolume;
//        this->ui->buttonTabSubVol->setDisabled(true);
//        this->on_actionSubVolSelection_triggered();

//        switch (this->systemMode)
//        {
//        case Mouse:
//        {
//            this->mouseBeginSubVol();
//            this->ui->Frame_SubVolLeapTracking->setVisible(false);
//            setMouseInteractor();
//        }
//            break;

//        case Leap:  this->leapBeginSubVol();
//            break;

//        case Touch:
//        {
//            this->ui->Frame_SubVolLeapTracking->setVisible(false);
//            this->touchBeginSubVol();

//            /////////////////////////////////////////////////
//            ///  Touch Interaction
//            ///
//            ///

//            setTouchInteractor();

//            this->ui->qvtkWidgetLeft->enableGestures();
//            connect(this->ui->qvtkWidgetLeft, SIGNAL(scaleTriggered()), userTestDlg,SLOT(incScaling()));
//            connect(this->ui->qvtkWidgetLeft, SIGNAL(translateTriggered()), userTestDlg,SLOT(incTranslation()));
//            connect(this->ui->qvtkWidgetLeft, SIGNAL(rotateTriggered()),userTestDlg,SLOT(incRotation()));
//        }
//            break;
//        }
//    }   // CASE 1
//        this->ui->qvtkWidgetLeft->setFocus();
//        break;

//        ///////////////////////////////////////////////////
//        /////   ARB SLICE JOB
//        ///
//    case 2 :
//    {
//        this->releaseTabFocus();

//        this->systemTab = SliceArb;
//        this->ui->buttonTabSliceArb->setDisabled(true);
//        this->on_actionSliceAxisArbitrary_triggered();

//        switch (this->systemMode)
//        {
//        case Mouse:
//        {
//            this->beginSliceArb();

//            setMouseInteractor();
//        }

//            break;

//        case Leap:

//            this->leapBeginSliceArb();
//            break;

//        case Touch:
//        {
//            this ->beginSliceArb();

//            setTouchInteractor();

//            this->ui->qvtkWidgetLeft->enableGestures();

//            connect(this->ui->qvtkWidgetLeft, SIGNAL(scaleTriggered()), userTestDlg,SLOT(incScaling()));
//            connect(this->ui->qvtkWidgetLeft, SIGNAL(translateTriggered()), userTestDlg,SLOT(incTranslation()));
//            connect(this->ui->qvtkWidgetLeft, SIGNAL(rotateTriggered()),userTestDlg,SLOT(incRotation()));
//        }
//            break;
//        }
//    }   // CASE 0
//        break;
//        this->ui->qvtkWidgetLeft->setFocus();
//    }
    this->ui->qvtkWidgetLeft->setFocus();
}

void MainWindow::stopUserPractice()
{
    //this->getUserTestResults(1);
    userPractice->close();
    //userTest->show();
    //userTest->on_btnStop_clicked();

    /// FOR TOUCH WIDGET Test Flag

    // this->ui->qvtkWidgetLeft->setUserTestMode(false);
}

QString MainWindow::getUserTestResults(int job)
{


       //QString userDetails = QString("UserID_%1.Job_%2.Task_%3.Prototyp_%4.png")
    QString orientation = "[" + this->ui->line_OrientX->text() +
                                " ;" + this->ui->line_OrientY->text() +
                                " ;" + this->ui->line_OrientZ->text() + "] ";

    QString position =    "[" + this->ui->line_PosX->text() +
                                " ;" + this->ui->line_PosY->text() +
                                " ;" + this->ui->line_PosZ->text() + "] ";

    QString zoom =  "[" + this->ui->line_Scale->text() + "] ";

    QString subVolumeX = "[" + this->ui->lineSubVolXMin->text() +
                                   " ;" + this->ui->lineSubVolXMax->text() + "] ";

    QString subVolumeY =  "[" + this->ui->lineSubVolYMin->text() +
                                    " ;"+ this->ui->lineSubVolYMax->text() + "] ";

    QString subVolumeZ =  "[" +  this->ui->lineSubVolZMin->text() +
                                    " ;"+ this->ui->lineSubVolZMax->text() + "] ";

    QString slicePos = "[" + this->ui->lineArbSlicePosX->text() +
                                " ;" + this->ui->lineArbSlicePosY->text() +
                                " ;" + this->ui->lineArbSlicePosZ->text() + "] ";

    QString sliceAngle =  "[" + this->ui->lineArbSliceAngleX->text() +
                                 " ;" + this->ui->lineArbSliceAngleY->text() +
                                 " ;" + this->ui->lineArbSliceAngleZ->text() + "]";


    //////////////////////////////////////////////////////////////////////////
    //// PRINTOUT
    /////////////////////////

    std::cout   << "Rotations: " << orientation.toStdString() << "; "
                  << "Translations: " << position.toStdString()  << "; "
                  << "Scaling: " << zoom.toStdString() << "; "

                  << "SubVolX: " << subVolumeX.toStdString() << "; "
                  << "SubVolY: " << subVolumeY.toStdString() << "; "
                  << "SubVolZ: " << subVolumeZ.toStdString() << "; "

                  << "SlicePos: " << slicePos.toStdString() << "; "
                  << "SliceAngle: " << sliceAngle.toStdString() << "; "


                 << std::endl;
    ////////////////////////////////////////////////////////////
    QString result = "";

      //QString userDetails = QString("UserID_%1.Job_%2.Task_%3.Prototyp_%4.png")


    //// Transformations =0 , SubVol=1, Slicing = 3;

    switch(job)
    {
    case 0 : result = QString("orientation=%1 ,position=%2 ,zoom=%3")
                                    .arg(orientation).arg(position).arg(zoom); break;
    case 1 : result = QString("orientation=%1 ,position=%2 ,zoom=%3 ,subVol=%4%5%6")
                                    .arg(orientation).arg(position).arg(zoom).arg(subVolumeX).arg(subVolumeY).arg(subVolumeZ); break;
    case 2 : result = QString("orientation=%1 ,position=%2 ,zoom=%3 ,slicePos=%4 ,sliceAngle=%5")
                                    .arg(orientation).arg(position).arg(zoom).arg(slicePos).arg(sliceAngle); break;
//    case 6 : result = "orientation=" + orientation  + ", position="+ position + ", zoom=" = zoom +
//                            "slicePos=" + slicePos + ", sliceAngle=" + sliceAngle; break;
    }

    return result;
}

void MainWindow::stopUserTest()
{
    //////////////////////////////////////////////////
    /// \brief saveScreenShot
    ///
    saveScreenShot();


    int currentJob = userTest->getCurrentJob();
    /// We first grab the Results before we close the Dialogbox
    userTest->importUserResults(userTestDlg->getUserResults());

    userTestActive = false;
    userTestDlg->close();

    userTest->on_btnStop_clicked();



    userTest->importSystemResults(getUserTestResults(currentJob));

    /// FOR TOUCH WIDGET Test Flag
    this->ui->qvtkWidgetLeft->setUserTestMode(false);

    //// SUS Dialog Window pops up
    surveyDlg = new SurveyDialog(this);
    surveyDlg->setAttribute(Qt::WA_DeleteOnClose);

    QRect rect = surveyDlg->geometry();
    QPoint ptRenderer = this->ui->qvtkWidgetLeft->geometry().center();
    /// userTest MOve to Center of RenderWidget;
    //userTest->move(ptRenderer.x(), ptRenderer.y());
    QPoint ptMainWindow = this->geometry().topLeft();

    ptRenderer += ptMainWindow;

    rect.moveCenter(ptRenderer);
    surveyDlg->setGeometry(rect);
    surveyDlg->show();

    ///QObject::connect(userTest,SIGNAL(startTest()),this,SLOT(startUserTest()));
    QObject::connect(surveyDlg, SIGNAL(saveSurvey()), this,SLOT(saveSurvey()));




}

void MainWindow::saveSurvey()
{
    /// We first grab the Results before we close the Dialogbox
    userTest->importSurveyResults(surveyDlg->getUserResults());

    std::cout << "Results Saved" << endl;

    surveyDlg->close();
    //// UserTest Window Returns after Survey
    userTest->show();
}

bool MainWindow::userTestRunning()
{
    return this->userTestActive;
}

void MainWindow::countInteraction(int testType)
{
    switch (testType)
    {
    case RotateCount : userTestDlg->incRotation(); break;
    case TranslateCount : userTestDlg->incTranslation(); break;
    case ScaleCount : userTestDlg->incScaling();    break;
    case ZRotateCount : userTestDlg->incZRotation(); break;
    case ResetCount : userTestDlg->incReset(); break;
    case SubVolResetCount : userTestDlg->incSubVolReset(); break;
    case SliceResetCount : userTestDlg->incSliceReset(); break;
    }
}

void MainWindow::saveScreenShot()
{
    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter =
        vtkSmartPointer<vtkWindowToImageFilter>::New();
      windowToImageFilter->SetInput(this->defaultRenWindow);
      windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
      windowToImageFilter->ReadFrontBufferOff(); // read from the back buffer
      windowToImageFilter->Update();

      int userID = userTest->GetCurrentUserID();

      int currentTask = userTest->getCurrentTask();
      int taskNo = 0;

      switch(currentTask)
      {
      case 4 : taskNo = 1; break;
      case 5 : taskNo = 2; break;
      case 6 : taskNo = 3; break;
      }


      /////////////////////////////////////////////////////
      ///////////////////////
      /// SAVE RESULT FOR IMAGE PRINTOUT
      ///
      ///

     ///////NOTE WIN 7 MACHINE DOESNT LIKE C:/
#ifdef __WIN32__
  QDir dir("C:/");
  QString userIDString = QString("UserTesting/UserID_%1").arg(userID);
  dir.mkpath(userIDString);
  if (!dir.exists())
      dir.mkpath(".");
  QString outputDir = dir.absolutePath() + "/" + userIDString + "/";
#elif __linux__
      QDir dir(QDir::homePath());
      QString userIDString = QString("UserTesting/UserID_%1").arg(userID);
      dir.mkpath(userIDString);
      if (!dir.exists())
          dir.mkpath(".");
      QString outputDir = dir.absolutePath() + "/" + userIDString + "/";
#endif
//      QDir dir("C:/");
//      QString userIDString = QString("UserTesting/UserID_%1").arg(userID);
//      dir.mkpath(userIDString);
//      if (!dir.exists())
//          dir.mkpath(".");
//      QString outputDir = dir.absolutePath() + "/" + userIDString + "/";

      QString userDetails = QString("%1/UserID_%2.Job_%3.Task_%4.Prototyp_%5.png")
              .arg(userIDString)
              .arg(userID)
              .arg(taskNo)
              .arg(userTest->GetCurrentTask(userID - 1))
              .arg(userTest->GetCurrentPrototype(userID - 1));

      /// UserID, Job#, Task#, Prototype
//      QString userDetails = userTest->GetCurrentUserID() << ".png" ;

      ///const char* fileName = QString(outputDir + userDetails).toStdString().c_str();
      QString fullFile = outputDir + userDetails;


      std::string stringFilename = userDetails.toLocal8Bit().constData();      
      const char* fileName = QString(outputDir + userDetails).toLocal8Bit().constData();




      vtkSmartPointer<vtkPNGWriter> writer =
        vtkSmartPointer<vtkPNGWriter>::New();      
      writer->SetFileName(userDetails.toLocal8Bit().constData());
      writer->SetInputConnection(windowToImageFilter->GetOutputPort());
      writer->Write();



}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    resizeTimer.start( 500);
    QMainWindow::resizeEvent(event);
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/////////////////MODE SELECTION BUTTONS////////////////////
///////////////////////////////////////////////////////////


void MainWindow::on_buttonModeMouse_clicked()
{
    this->systemMode = Mouse;
    std::cout << "System Mode is: " << this->systemMode << endl;

    ///DISSABLE LEAP AND TOUCH CALLS
    this->ui->qvtkWidgetLeft->dissableGestures();
    this->ui->actionLeapDialogToggle->setChecked(false);
    Leaping_ = false;

    if(this->leapMarkerWidget != NULL)
    {
        this->leapMarkerWidget->SetEnabled(false);
        this->leapMarkerWidget->Delete();
        this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();
    }

    if(this->leapHandWidget != NULL)
    {
        this->leapHandWidget->SetEnabled(false);
        this->leapHandWidget->Delete();
        this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();
    }



    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////

    setMouseInteractor();
}


void MainWindow::on_buttonModeLeap_clicked()
{
    this->systemMode = Leap;
    this->ui->actionLeapDialogToggle->setChecked(true);

    ///DISSABLE MOUSE AND TOUCH CALLS
    this->ui->qvtkWidgetLeft->dissableGestures();

    std::cout << "System Mode is: " << this->systemMode << endl;

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////

    this->controller_= new Controller;
    Leaping_ = true;

    setLeapInteractor();



    this->leapMatrixTotalMotionRotation = Leap::Matrix::identity();
    this->leapVectorTotalMotionalTranslation = Leap::Vector::zero();
    this->leapFloatTotalMotionScale = 1.0f;

    connect(this->ui->checkBoxLeapTracking, SIGNAL(toggled(bool)), this,SLOT(leapTrackingOn(bool)));

    this->ui->qvtkWidgetLeft->setFocus();
    //////
    /// \brief INTRODUCTION OF LEAP WIDGET
    ///
    ///

    this->leapHandWidget = vtkLeapHandWidget::New();
    this->leapHandWidget->SetInteractor(this->ui->qvtkWidgetLeft->GetInteractor());
    this->leapHandWidget->GeneratActors();    
    this->leapHandWidget->SetEnabled(true);
    this->leapHandWidget->InteractiveOff();

  //  this->leapHandWidget->SetEnabled(false);


    this->leapMarkerWidget = vtkLeapMarkerWidget::New();
    this->leapMarkerWidget->SetInteractor(this->ui->qvtkWidgetLeft->GetInteractor());
    this->leapMarkerWidget->GeneratActors();
    this->leapMarkerWidget->SetEnabled(true);
    this->leapMarkerWidget->InteractiveOff();

}

void MainWindow::on_buttonModeTouch_clicked()
{
    this->systemMode = Touch;

    ///DISSABLE MOUSE AND LEAP CALLS
    this->ui->actionLeapDialogToggle->setChecked(false);


    //   this->leapMarkerWidget->Print(std::cout);
    if(this->leapMarkerWidget != NULL)
    {
        this->leapMarkerWidget->SetEnabled(false);
        this->leapMarkerWidget->Delete();
        this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();
    }

    if(this->leapHandWidget != NULL)
    {
        this->leapHandWidget->SetEnabled(false);
        this->leapHandWidget->Delete();
        this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();
    }

    std::cout << "System Mode is: " << this->systemMode << endl;




    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////

    setTouchInteractor();

   // this->ui->qvtkWidgetLeft = new QVTKTouchWidget(centralWidget());

    this->ui->qvtkWidgetLeft->enableGestures();


    connect(this->ui->qvtkWidgetLeft, SIGNAL(translationAction()), this,SLOT(touchInteractionEvent()));
    connect(this->ui->qvtkWidgetLeft, SIGNAL(translationPressed()), this,SLOT(touchTranslatePressed()));
    connect(this->ui->qvtkWidgetLeft, SIGNAL(translationReleased()), this,SLOT(touchTranslateReleased()));

    connect(this->ui->qvtkWidgetLeft, SIGNAL(rotationPressed()), this, SLOT(touchRotationPressed()));
    connect(this->ui->qvtkWidgetLeft, SIGNAL(rotationReleased()), this, SLOT(touchRotationReleased()));
    connect(this->ui->buttonTransformActive,SIGNAL(toggled(bool)),this, SLOT(touchTransformsOn(bool)));
    //connect(this->ui->buttonTransformActive, SIGNAL(pressed()), this, SLOT(touchTransformationToggle()));

    connect(this->ui->qvtkWidgetLeft, SIGNAL(transformationTriggered()),this, SLOT(touchTransformationToggle()));
    this->ui->buttonTransformActive->setChecked(true);



    //        ////////////////////////////////////////////////ui////////////////////////////////
    //        /// \brief cameraObserver for Global Scaling Events for ALl Mouse, Touch and Leap
    //        ///

    //        vtkSmartPointer<vtkCameraTranslationCallback> cameraObserver =
    //                vtkSmartPointer<vtkCameraTranslationCallback>::New();
    //        vtkCamera* thisCamera = this->ui->qvtkWidgetLeft->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();

    //        cameraObserver->ui = this->ui;
    //       // cameraObserver->defualtCameraDistance = this->defaultCameraDistance;


    //        thisCamera->AddObserver(vtkCommand::ModifiedEvent, cameraObserver);
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/////////////////TAB SELECTION BUTTONS ////////////
///////////////////////////////////////////////////////////

void MainWindow::on_buttonTabInfo_pressed()
{
    if(this->systemTab != Information)
    {
       /// if (this->MessageBoxQuery("Switch to Information ?","Are you sure you want to continue?"))
        {
            this->releaseTabFocus();
            this->systemTab = Information;

            this->ui->buttonTabInfo->setDisabled(true);

            this->infoTab_Triggered();

            switch(systemMode)
            {
            case Mouse:
            {
                setMouseInteractor();
            }
                break;

            case Touch:
            {
                setTouchInteractor();

                this->ui->qvtkWidgetLeft->enableGestures();

                this->ui->qvtkWidgetLeft->setTransformsOn(true);
                this->ui->qvtkWidgetLeft->SetArbSliceOn(false);

                this->ui->qvtkWidgetLeft->SetSubVolumeOn(false);

                this->ui->buttonTransformActive->setChecked(true);


                disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Pressed()), this, SLOT(touchFinger1SubVol()));
                disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Pressed()), this, SLOT(touchFinger2SubVol()));
                disconnect(this->ui->qvtkWidgetLeft, SIGNAL(rightClkSubVol()), this, SLOT(touchSubVolTranslate()));
                disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Released()), this, SLOT(touchFingerReleased()));

                disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Pressed()), this, SLOT(touchFinger1ArbSlicePressed()));
                disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Moving()), this, SLOT(touchFinger1ArbSliceMoving()));
                disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Released()), this, SLOT(touchFinger1ArbSliceReleased()));

                disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Pressed()), this, SLOT(touchFinger2ArbSlicePressed()));
                disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Moving()), this, SLOT(touchFinger2ArbSliceMoving()));
                disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Released()), this, SLOT(touchFInger2ArbSliceReleased()));
            }
                break;
            case Leap:
            {
                setTouchInteractor();
                this->ui->buttonTransformActive->setChecked(true);
            }
                break;
            }

            /// Refresh the Interaction screen.
            //this->ui->qvtkWidgetLeft->GetRenderWindow()->Render();
            on_actionReset_Camera_triggered();
        }
    }
    this->ui->qvtkWidgetLeft->setFocus();
}



void MainWindow::on_buttonTabSubVol_pressed()
{
    if(this->systemTab != SubVolume)
    {
       /// if (this->MessageBoxQuery("Switch to SubVolume?","Are you sure you want to continue?"))
        {
            this->releaseTabFocus();

            this->systemTab = SubVolume;
            this->ui->buttonTabSubVol->setDisabled(true);
            this->on_actionSubVolSelection_triggered();

            switch (this->systemMode)
            {
            case Mouse:
            {
                this->mouseBeginSubVol();
                this->ui->Frame_SubVolLeapTracking->setVisible(false);
                setMouseInteractor();
            }
                break;

            case Leap:
            {
                this->leapBeginSubVol();
                this->ui->buttonTransformActive->setChecked(false);
            }

                break;

            case Touch:
            {
                this->ui->Frame_SubVolLeapTracking->setVisible(false);
                this->touchBeginSubVol();

               /// setTouchInteractor();

                this->ui->qvtkWidgetLeft->enableGestures();

                this->ui->qvtkWidgetLeft->setTransformsOn(false);
                this->ui->qvtkWidgetLeft->SetArbSliceOn(false);

                this->ui->qvtkWidgetLeft->SetSubVolumeOn(true);

                this->ui->buttonTransformActive->setChecked(false);


                connect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Pressed()), this, SLOT(touchFinger1SubVol()));
                connect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Pressed()), this, SLOT(touchFinger2SubVol()));
                connect(this->ui->qvtkWidgetLeft, SIGNAL(rightClkSubVol()), this, SLOT(touchSubVolTranslate()));
                connect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Released()), this, SLOT(touchFingerReleased()));


            }
                break;
            }
        }

        this->defaultRenderer->GetActiveCamera()->SetParallelProjection(false);
        this->defaultRenderer->GetActiveCamera()->SetViewAngle(45);
        this->ui->qvtkWidgetLeft->setFocus();
    }
}

void MainWindow::on_buttonTabSliceAxis_pressed()
{
    if(this->systemTab != SliceAxis)
    {
      ///  if (this->MessageBoxQuery("Switch to Axis Slice?","Are you sure you want to continue?"))
        {
            this->releaseTabFocus();

            this->systemTab = SliceAxis;
            this->ui->buttonTabSliceAxis->setDisabled(true);

            this->on_actionSliceAxisAligned_triggered();

            switch (this->systemMode)
            {
            case Mouse:
            {
                this->beginSliceAxis();
                setMouseInteractor();
            }
                break;

            case Leap: this->leapBeginSliceAxis();
                setTouchInteractor();
                break;
            }
        }
        this->ui->qvtkWidgetLeft->setFocus();
    }
}

void MainWindow::on_buttonTabSliceArb_pressed()
{
    if(this->systemTab != SliceArb)
    {
      ///  if (this->MessageBoxQuery("Switch to Arb Slice?","Are you sure you want to continue?"))
        {
            this->releaseTabFocus();

            this->systemTab = SliceArb;
            this->ui->buttonTabSliceArb->setDisabled(true);
            this->on_actionSliceAxisArbitrary_triggered();

            switch (this->systemMode)
            {
            case Mouse:
            {
                this->beginSliceArb();

                setMouseInteractor();
            }

                break;

            case Leap:
            {
                this->leapBeginSliceArb();
                this->ui->buttonTransformActive->setChecked(false);
            }
                break;

            case Touch:
            {
                this ->beginSliceArb();
                setTouchInteractor();


                this->ui->qvtkWidgetLeft->enableGestures();


                this->ui->buttonTransformActive->setChecked(false);


                this->ui->qvtkWidgetLeft->setTransformsOn(false);

                this->ui->qvtkWidgetLeft->SetSubVolumeOn(false);

                this->ui->qvtkWidgetLeft->SetArbSliceOn(true);

                connect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Pressed()), this, SLOT(touchFinger1ArbSlicePressed()));
                connect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Moving()), this, SLOT(touchFinger1ArbSliceMoving()));
                connect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Released()), this, SLOT(touchFinger1ArbSliceReleased()));

                connect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Pressed()), this, SLOT(touchFinger2ArbSlicePressed()));
                connect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Moving()), this, SLOT(touchFinger2ArbSliceMoving()));
                connect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Released()), this, SLOT(touchFInger2ArbSliceReleased()));

                this->customArbPlaneWidget->GetInteractor()->RemoveAllObservers();
            }
                break;
            }
        }
        this->ui->qvtkWidgetLeft->setFocus();
    }
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////     INFORMATION STATUS BAR INFORMATION
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void MainWindow::infoTab_Triggered()
{
    closeTabs();
    this->ui->tabLogWidget->insertTab(0, this->ui->InfoTab,"Information");
    this->ui->tabLogWidget->setCurrentIndex(0);
}

void MainWindow::setMouseInteractor()
{
    /////////////////////////////////////////////////
    ///  Mouse Rotation
    ///
    ///
    MouseInteractorStyle * style = MouseInteractorStyle::New();


    vtkRenderWindowInteractor * interactor = this->ui->qvtkWidgetLeft->GetInteractor();
    interactor->SetInteractorStyle(style);
    style->camera = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();
    style->ui = this->ui;
    style->mainWindow = this;
    style->defualtDistance = this->defaultCameraDistance;
}

void MainWindow::setTouchInteractor()
{
    vtkSmartPointer<TouchInteractorStyle> style =
            vtkSmartPointer<TouchInteractorStyle>::New();

    this->ui->qvtkWidgetLeft->GetInteractor()->SetInteractorStyle(style);
    style->SetCurrentRenderer(this->defaultRenderer);

    style->ui = this->ui;
    style->mainWindow = this;
    style->defualtDistance = this->defaultCameraDistance;
    style->camera = this->ui->qvtkWidgetLeft->GetInteractor()->
            GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();

}

void MainWindow::setLeapInteractor()
{
    vtkSmartPointer<LeapInteractorStyle> style =
            vtkSmartPointer<LeapInteractorStyle>::New();

    this->ui->qvtkWidgetLeft->GetInteractor()->SetInteractorStyle(style);
    style->SetCurrentRenderer(this->defaultRenderer);

    style->rotation = this->ui->checkBox_Rotation;
    style->translation = this->ui->checkBox_Translation;
    style->scaling = this->ui->checkBox_Scaling;
    style->mainWindow = this;


    vtkRenderWindowInteractor * interactor = this->ui->qvtkWidgetLeft->GetInteractor();

    interactor->SetInteractorStyle(style);
    style->camera = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();
    style->ui = this->ui;
    style->defualtDistance = this->defaultCameraDistance;
}


////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
/// \brief MainWindow::updateme
////////////////////////////////////////////////////////////////////


void MainWindow::updateme()
{

    QFont boldFont, normalFont;

    boldFont.setBold(true);
    normalFont.setBold(false);

    this->ui->labelTranslation->setFont(normalFont);
    this->ui->labelRotation->setFont(normalFont);
    this->ui->labelScaling->setFont(normalFont);


    if (Leaping_)
    {
        ui->qvtkWidgetLeft->GetRenderWindow()->Render();


        this->ui->buttonTransfRotation->setEnabled(false);
        this->ui->buttonTransfTranslation->setEnabled(false);
        this->ui->buttonTransfScaling->setEnabled(false);
        LeapMotion();
    }

    if (systemMode == Touch)
    {
        ui->qvtkWidgetLeft->GetRenderWindow()->Render();
    }
}

void MainWindow::slotExit()
{
    if (controller_ != NULL)
    {
        if(controller_->isConnected())
            controller_->~Controller();
    }

    qApp->exit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::button_Exit_clicked()
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
    /// fitsReader->SetFileName(newFileName);
    fitsReader->SetFileName("OMC.FITS");

    fitsReader->Update();

    /////////////////////////////////
    /// \brief Outline Object
    ///
    //qDebug() << "Adding Outline" << endl;
    vtkOutlineFilter *outline = vtkOutlineFilter::New();
    outline->SetInputConnection(fitsReader->GetOutputPort());

    vtkPolyDataMapper *outlineMapper = vtkPolyDataMapper::New();
    outlineMapper->SetInputConnection(outline->GetOutputPort());

    vtkActor *outlineActor = vtkActor::New();
    outlineActor->SetMapper(outlineMapper);
    outlineActor->GetProperty()->SetColor(0.5,0.5,0.5);

    global_Outline = outlineActor;


    ///
    /// \brief resample
    ///
    vtkImageResample *resample = vtkImageResample::New();

    resample->SetInputConnection( fitsReader->GetOutputPort() );
    resample->SetAxisMagnificationFactor(0, 1.0);
    resample->SetAxisMagnificationFactor(1, 1.0);
    resample->SetAxisMagnificationFactor(2, 1.0);

    global_Resample = resample;

    /// Create the Volume & mapper

    vtkVolume *volume = vtkVolume::New();
    vtkGPUVolumeRayCastMapper *mapper = vtkGPUVolumeRayCastMapper::New();
    global_Mapper = mapper;

    //mapper->SetInputConnection(fitsReader->GetOutputPort());
    mapper->SetInputConnection(resample->GetOutputPort());

    /// Create our transfer function
    vtkPiecewiseFunction *opacityFun = vtkPiecewiseFunction::New();
    vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();

    // Create the property and attach the transfer functions

    vtkVolumeProperty * property = vtkVolumeProperty::New();
    property->SetColor(colorFun);
    property->SetScalarOpacity(opacityFun);
    property->SetInterpolationTypeToLinear();
    //property->SetIndependentComponents(1);//


    /// Opacity Function Colour point ( x,  r,  g,  b, midpoint, sharpness, rgb range [0,255])
    /// WHITE contrast
    ///    colorFun->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0 );

    //    colorFun->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 5.0, 1.0, 1.0, 1.0 );
    ///    colorFun->AddRGBPoint(5, 1,1,1);
    //colorFun->AddPoi(0.0, 1.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0 );



    //    /// Black and White
    //    colorFun->AddRGBSegment(opacityLevel - 0.5*opacityWindow, 0.0, 0.0, 0.0,
    //                            opacityLevel + 0.5*opacityWindow, 1.0, 1.0, 1.0 );
    //    ///
    colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
    colorFun->AddRGBPoint( -1000, .62, .36, .18, 0.5, 0.0 );    /// REDISH COLOUR
    colorFun->AddRGBPoint( -500, .88, .60, .29, 0.33, 0.45 );
    colorFun->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );

    /// Add Point x, y, midpoint, sharpness
    //    opacityFun->AddPoint(0,  0.0);

    opacityFun->AddPoint(1,  0.0);
    opacityFun->AddPoint(26,  0.5);
    ///
    ///

    /// Add a Colour Lookup Table
    //    vtkSmartPointer<vtkLookupTable> lut3D =
    //            vtkSmartPointer<vtkLookupTable>::New();
    //    lut3D->SetTableRange(0,1000);
    //    lut3D->SetSaturationRange(0,0);
    //    lut3D->SetHueRange(0,0);
    //    lut3D->SetValueRange(0,1);





    //    double min = this->global_Reader->GetDataMin();
    //    double max = this->global_Reader->GetDataMin();
    //    double mid = (min + max) / 2;

    //    //opacityFun->AddPoint(, 0.0);
    //    opacityFun->AddPoint(min, 0.0);
    //    //opacityFun->AddPoint(mid, 0.1);
    //    //opacityFun->AddPoint(max, 0.55);

    //      colorFun->AddRGBPoint(min, 0.0, 0.0, 0.0);
    //      colorFun->AddRGBPoint(max, 1.0, 1.0, 1.0);

    mapper->SetBlendModeToComposite();
    //mapper->SetBlendModeToMaximumIntensity();
    property->ShadeOn();
    property->SetAmbient(0.4);
    property->SetDiffuse(0.6);

    property->SetSpecular(0.2);
    property->SetSpecularPower(1.0);
    property->SetScalarOpacityUnitDistance(0.8919);

    // connect up the volume to the property and the mapper
    volume->SetProperty( property );
    volume->SetMapper( mapper );

    global_Volume = volume;


    /// Create the Actor

    vtkSmartPointer<vtkCubeAxesActor> cubeAxesActor =
            vtkSmartPointer<vtkCubeAxesActor>::New();
    //cubeAxesActor->AddObserver(vtkCommand::ProgressEvent, pObserver );


    defaultCubeAxes = cubeAxesActor;

     QObject::connect(this->ui->actionCubeAxes_Toggle, SIGNAL(toggled(bool)),this,SLOT(cubeAxesToggled(bool)));
    ////////////////////////////////////
    /// \brief RenderWindow
    ////
    //qDebug() << "Adding RenderWindow" << endl;

    vtkRenderer *ren1 = vtkRenderer::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();

    renderWindow->AddRenderer(ren1);



    //    // Set the vtkInteractorStyleSwitch for renderWindowInteractor
    //    vtkSmartPointer<vtkInteractorStyleSwitch> style =
    //            vtkSmartPointer<vtkInteractorStyleSwitch>::New();

    ////////////////////////////////////
    /// \brief Cube Axes Labels
    ////
    //qDebug() << "Adding Axes Labels" << endl;

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

    ren1->AddActor(cubeAxesActor);
    ren1->AddActor(outlineActor);

    // add actors to renderer

    //qDebug() << "Adding Objects to RenderWindow" << endl;

    ren1->AddVolume(volume);


    this->ui->qvtkWidgetLeft->SetRenderWindow(renderWindow);
    //this->ui->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
    this->ui->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetRenderWindow(renderWindow);


    this->defaultRenWindow = this->ui->qvtkWidgetLeft->GetRenderWindow();
    this->defaultRenderer =  this->ui->qvtkWidgetLeft->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

    ////////////////////////////////////
    /// \brief OrientationMarker Widget
    ////
    //qDebug() << "Adding Orientation Marker" << endl;

    AddOrientationAxes(this->ui->qvtkWidgetLeft);

    ////////////////////////////////////
    /// \brief ScalarBar Widget
    ////
    //qDebug() << "Adding ScalarBar" << endl;

    defaultScalarBar = vtkScalarBarActor::New();
    AddScalarBar(this->ui->qvtkWidgetLeft, fitsReader,defaultScalarBar);
    QObject::connect(this->ui->actionSpectrum_Toggle, SIGNAL(toggled(bool)),this,SLOT(spectrumToggled(bool)));

    ////    // global_Points = tempSet;
    global_Reader = fitsReader;

    //   global_Volume = sliceA;

    //qDebug() << "Resetting Camera" << endl;
    ren1->ResetCamera();
    //qDebug() << "Complete" << endl;


    ////////////////////////////////////
    /// \brief FrameRate Widget
    ////
    //qDebug() << "Adding FrameRate" << endl;
    reloadFrameRate();

    ////////////////////////////////////
    /// Important assignment to get the Camera's Center Focus & clipping Range for Leap manipulation
    ///
    ren1->GetActiveCamera()->GetFocalPoint(cameraFocalPoint);

    ren1->GetActiveCamera()->SetClippingRange(100.00, 900.00);


    ren1->GetActiveCamera()->GetFocalPoint(defaultCameraFocalPnt);
    ren1->GetActiveCamera()->GetPosition(defaultCameraPosition);
    ren1->GetActiveCamera()->GetViewUp(defaultCameraViewUp);
    defaultCameraDistance = ren1->GetActiveCamera()->GetDistance();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief cameraObserver for Global Scaling Events for ALl Mouse, Touch and Leap
    ///

    vtkSmartPointer<vtkCameraScaleCallback> cameraObserver =
            vtkSmartPointer<vtkCameraScaleCallback>::New();
    vtkCamera* thisCamera = this->ui->qvtkWidgetLeft->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();

    cameraObserver->ui = this->ui;
    cameraObserver->defualtCameraDistance = this->defaultCameraDistance;


    thisCamera->AddObserver(vtkCommand::ModifiedEvent, cameraObserver);


    ///////////////////////////////////////////////
    /// \brief SETTING Default Interactor Style to Mouse Interaction
    ///
    ///

    this->on_buttonModeMouse_clicked();

    ////////////////////////////////////////////////
    /// Leapmotion TRACKING Message
    ///
    ///
    ///
    int * screenSize = this->ui->qvtkWidgetLeft->GetRenderWindow()->GetScreenSize();

    this->leapTrackingActor = vtkTextActor::New();
    this->leapTrackingActor->GetTextProperty()->SetFontSize(30);
    this->leapTrackingActor->GetTextProperty()->SetBold(true);


    this->leapTrackingActor->SetInput("TRACKING..");
    this->leapTrackingActor->GetTextProperty()->SetColor(1,1,1);

    this->defaultRenderer->AddActor2D(leapTrackingActor);
    this->leapTrackingActor->SetVisibility(false);
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


void MainWindow::on_actionOpen_triggered()
{

    QFileDialog dlg(NULL,tr("Open File"),"", tr("Files (*.FITS)"));

    //QString fileName = QFileDialog::getOpenFileName(this, tr ("Open File"),"", tr("Files (*.FITS)"));
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    QString fileName;
    if (dlg.exec())
    {
        fileName = dlg.selectedFiles().at(0);

        currentFitsFile = fileName;

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
        //////////////////////////////////////////////////////////////////////
        QString padding = "                                                   ";
        statusBar()->showMessage(padding + fileName);

        this->ui->tabLogWidget->setVisible(true);
        this->ui->LeftCameraFrame->setVisible(true);
        this->ui->TopMethodFrame->setVisible(true);
        this->ui->TopModeFrame->setVisible(true);

        closeTabs();

        this->ui->tabLogWidget->insertTab(0, this->ui->InfoTab, "Information");
        this->ui->tabLogWidget->setCurrentIndex(0);

        this->UpdateFitsFileInfo();

        /// We initialised the default Transformation state
        ///
        //on_buttonTransfRotation_clicked();
        //this->buttonTransRotationPressed();

        /// We Update the Transformation Coordinates from the Camera Details
        ///
        this->resetTransformCoords();

        this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();

        this->ui->qvtkWidgetLeft->update();
    }
}

void MainWindow::on_actionReload_triggered()
{
    vtkVolume *volume = vtkVolume::New();
    vtkGPUVolumeRayCastMapper *mapper = vtkGPUVolumeRayCastMapper::New();
    global_Mapper = mapper;

    //mapper->SetInputConnection(fitsReader->GetOutputPort());
    mapper->SetInputConnection(global_Resample->GetOutputPort());

    /// Create our transfer function
    vtkPiecewiseFunction *opacityFun = vtkPiecewiseFunction::New();
    vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();

    // Create the property and attach the transfer functions

    vtkVolumeProperty * property = vtkVolumeProperty::New();
    property->SetColor(colorFun);
    property->SetScalarOpacity(opacityFun);
    property->SetInterpolationTypeToLinear();
    //property->SetIndependentComponents(1);//

    colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
    colorFun->AddRGBPoint( -1000, .62, .36, .18, 0.5, 0.0 );
    colorFun->AddRGBPoint( -500, .88, .60, .29, 0.33, 0.45 );
    colorFun->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );

    opacityFun->AddPoint(0,  0.0);
    opacityFun->AddPoint(50,  0.5);
    opacityFun->AddPoint(1000,  1.0);

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

    global_Volume = volume;


    /// Create the Actor

    vtkSmartPointer<vtkCubeAxesActor> cubeAxesActor =
            vtkSmartPointer<vtkCubeAxesActor>::New();
    //cubeAxesActor->AddObserver(vtkCommand::ProgressEvent, pObserver );


    defaultCubeAxes = cubeAxesActor;

    defaultCubeAxes->SetVisibility(this->ui->actionCubeAxes_Toggle->isChecked());
    ////////////////////////////////////
    /// \brief RenderWindow
    ////
    //qDebug() << "Adding RenderWindow" << endl;

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
    //qDebug() << "Adding Axes Labels" << endl;

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

    ren1->AddActor(cubeAxesActor);
    ren1->AddActor(global_Outline);

    /// add actors to renderer

    ren1->AddVolume(volume);


    this->ui->qvtkWidgetLeft->SetRenderWindow(renderWindow);
    this->ui->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetRenderWindow(renderWindow);

    this->defaultRenWindow = this->ui->qvtkWidgetLeft->GetRenderWindow();
    this->defaultRenderer =  this->ui->qvtkWidgetLeft->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

    ////////////////////////////////////
    /// \brief OrientationMarker Widget
    ////

    AddOrientationAxes(this->ui->qvtkWidgetLeft);

    ////////////////////////////////////
    /// \brief ScalarBar Widget
    ////

    AddScalarBar(this->ui->qvtkWidgetLeft, global_Reader, defaultScalarBar);

    ren1->ResetCamera();

    ////////////////////////////////////
    /// \brief FrameRate Widget
    ////
    reloadFrameRate();

    ////////////////////////////////////
    /// Important assignment to get the Camera's Center Focus & clipping Range for Leap manipulation
    ///
    ren1->GetActiveCamera()->GetFocalPoint(cameraFocalPoint);

    ren1->GetActiveCamera()->SetClippingRange(100.00, 900.00);


    ren1->GetActiveCamera()->GetFocalPoint(defaultCameraFocalPnt);
    ren1->GetActiveCamera()->GetPosition(defaultCameraPosition);
    ren1->GetActiveCamera()->GetViewUp(defaultCameraViewUp);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief cameraObserver for Global Scaling Events for ALl Mouse, Touch and Leap
    ///

    vtkSmartPointer<vtkCameraScaleCallback> cameraObserver =
            vtkSmartPointer<vtkCameraScaleCallback>::New();
    vtkCamera* thisCamera = this->ui->qvtkWidgetLeft->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();

    cameraObserver->ui = this->ui;
    cameraObserver->defualtCameraDistance = this->defaultCameraDistance;


    thisCamera->AddObserver(vtkCommand::ModifiedEvent, cameraObserver);

    ////////////////////////////////////////////////
    /// Leapmotion TRACKING Message
    ///
    ///

    this->leapTrackingActor = vtkTextActor::New();
    this->leapTrackingActor->GetTextProperty()->SetFontSize(20);
    this->leapTrackingActor->GetTextProperty()->SetBold(true);

    this->leapTrackingActor->SetPosition(20,550);
    this->leapTrackingActor->SetInput("TRACKING..");
    this->leapTrackingActor->GetTextProperty()->SetColor(1,1,1);

    defaultRenderer->AddActor2D(leapTrackingActor);
    this->leapTrackingActor->SetVisibility(false);


    //this->defaultRenderer = ren1;
    /// We initialised the default Transformation state
    ///
    //on_buttonTransfRotation_clicked();
    //this->buttonTransRotationPressed();


    ////////////////////////////////////////////////////
    /////
    /// vtk LeapMarkerWidget should only be active if we are in the leap state
    ///

    if (this->systemMode == Leap )
    {
        ///vtkLeapMarkerWidget
        this->leapMarkerWidget = vtkLeapMarkerWidget::New();
        this->leapMarkerWidget->SetInteractor(this->ui->qvtkWidgetLeft->GetInteractor());

        this->leapMarkerWidget->GeneratActors();

        this->leapMarkerWidget->SetEnabled(true);
        //marker->InteractiveOn();
        this->leapMarkerWidget->InteractiveOff();



        ////vtkLeapHandWidget
        this->leapHandWidget = vtkLeapHandWidget::New();
        this->leapHandWidget->SetInteractor(this->ui->qvtkWidgetLeft->GetInteractor());

        this->leapHandWidget->GeneratActors();

        this->leapHandWidget->SetEnabled(true);
        this->leapHandWidget->InteractiveOff();;
    }
}

void MainWindow::printBounds(const char* name, double bounds[6])
{
    std::cout << name << " Bounds (x1-x2,y1-y2,z1-z2) = ( "
              << bounds[0] << " - " << bounds[1] << " , "
              << bounds[2] << " - " << bounds[3] << " , "
              << bounds[4] << " - " << bounds[5] << " )" << std::endl ;
}


void MainWindow::slot_Load_Finished()
{
    qDebug() << "File Loaded " << endl  ;
}

void MainWindow::on_actionExit_triggered()
{
    slotExit();
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
////////////////////////////////////////////////////
void MainWindow::on_actionDefault_triggered()
{
    /// Create out transfer function
    ///

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

    opacityFun->AddPoint(0,  0.0);
    opacityFun->AddPoint(50,  0.5);
    opacityFun->AddPoint(1000,  1.0);

    property->ShadeOn();
    property->SetAmbient(0.4);
    property->SetDiffuse(0.6);
    property->SetSpecular(0.2);
    property->SetSpecularPower(10.0);
    property->SetScalarOpacityUnitDistance(0.8919);

    global_Volume->SetProperty( property );

}

void MainWindow::spectrumToggled(bool status)
{
    //std::cout << "Toggled:" << status << endl;
    this->defaultScalarBar->SetVisibility(status);
    this->defaultRenWindow->Render();
}

void MainWindow::cubeAxesToggled(bool status)
{
    this->defaultCubeAxes->SetVisibility(status);
    this->defaultRenWindow->Render();
}

void MainWindow::on_actionBlack_White_triggered()
{
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

    /// Black and White
    colorFun->AddRGBSegment(opacityLevel - 0.5*opacityWindow, 1.0, 1.0, 1.0,
                            opacityLevel + 0.5*opacityWindow, 1.0, 1.0, 1.0 );

    opacityFun->AddPoint(0,  0.0);
    opacityFun->AddPoint(50,  0.5);
    opacityFun->AddPoint(1000,  1.0);

    property->ShadeOn();
    property->SetAmbient(0.4);
    property->SetDiffuse(0.6);
    property->SetSpecular(0.2);
    property->SetSpecularPower(10.0);
    property->SetScalarOpacityUnitDistance(0.8919);

    global_Volume->SetProperty( property );
}


void MainWindow::on_actionBlue_Red_triggered()
{
    /// Create out transfer function
    ///

    // Create our transfer function
    vtkPiecewiseFunction *opacityFun = vtkPiecewiseFunction::New();
    vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();

    // Create the property and attach the transfer functions

    vtkVolumeProperty * property = vtkVolumeProperty::New();
    property->SetColor(colorFun);
    property->SetScalarOpacity(opacityFun);
    property->SetInterpolationTypeToLinear();

    /// Black and White
    //    colorFun->AddRGBSegment(opacityLevel - 0.5*opacityWindow, 0.0, 0.0, 0.0,
    //                            opacityLevel + 0.5*opacityWindow, 1.0, 1.0, 0.0 );

    //    /// DEFAULT
    //    colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
    colorFun->AddRGBPoint( -1000, .62, .36, .18, 0.5, 0.0 );
    //    colorFun->AddRGBPoint( -500, .88, .60, .29, 0.33, 0.45 );
    colorFun->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );

    opacityFun->AddPoint(0,  0.0);
    opacityFun->AddPoint(50,  0.5);
    opacityFun->AddPoint(1000,  1.0);

    property->ShadeOn();
    property->SetAmbient(0.4);
    property->SetDiffuse(0.6);
    property->SetSpecular(0.2);
    property->SetSpecularPower(10.0);
    property->SetScalarOpacityUnitDistance(0.8919);

    global_Volume->SetProperty( property );
}

void MainWindow::on_actionDemo_triggered()
{
    vtkSmartPointer<vtkRenderer> theRenderer =
            vtkSmartPointer<vtkRenderer>::New();

    theRenderer = this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->
            GetRenderers()->GetFirstRenderer();

    for (int i = 0; i < 360; i ++)
    {
        double  * focalPoint;
        double  * position ;
        double  dist ;

        focalPoint = theRenderer->GetActiveCamera()->GetFocalPoint();
        position = theRenderer->GetActiveCamera()->GetPosition();
        dist =  sqrt(pow((position[0]-focalPoint[0]),2) +
                pow((position[1]-focalPoint[1]),2) +
                pow((position[2]-focalPoint[2]),2));
        theRenderer->GetActiveCamera()->SetPosition(focalPoint[0], focalPoint[1], focalPoint[2] + dist);

        // theRenderer->GetActiveCamera()->SetViewUp(0.0,1.0,0.0);

        theRenderer->GetActiveCamera()->Azimuth(i);  //WORKS
        std::cout << "Roll Angle= " << theRenderer->GetActiveCamera()->GetRoll() << " i = " << i ;
        theRenderer->GetActiveCamera()->Roll(1);

        std::cout << " New Angle= " << theRenderer->GetActiveCamera()->GetRoll() << std::endl;


        //theRenderer->GetActiveCamera()->Pitch(i);
        theRenderer->GetActiveCamera()->Elevation(i); //WORKS

#ifdef __WIN32__
        Sleep(0.1);
#elif __linux__
        sleep(0.1);
#endif

        this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();
    }

}

void MainWindow::UpdateFitsFileInfo()
{
    this->ui->lineInfoObject->setText(QString("%1").arg(this->global_Reader->GetObjectSky()).trimmed());
    this->ui->lineInfoTelescope->setText(QString("%1").arg(this->global_Reader->GetTelescope()).trimmed());
    this->ui->lineInfoEpoch->setText(QString("%1").arg(this->global_Reader->GetEpoch()));

    this->ui->lineInfoNAxis->setText(QString("%1").arg(this->global_Reader->GetNAxis()));
    this->ui->lineInfoNAxis1->setText(QString("%1").arg(this->global_Reader->GetDimensions()[0]));
    this->ui->lineInfoNAxis2->setText(QString("%1").arg(this->global_Reader->GetDimensions()[1]));
    this->ui->lineInfoNAxis3->setText(QString("%1").arg(this->global_Reader->GetDimensions()[2]));

    this->ui->lineInfoPoints->setText(QString("%1").arg(this->global_Reader->GetPoints()));
    this->ui->lineInfoDataMin->setText(QString("%1").arg(this->global_Reader->GetDataMin()));
    this->ui->lineInfoDataMax->setText(QString("%1").arg(this->global_Reader->GetDataMax()));

    this->ui->lineInfoXAxis->setText(QString("%1").arg(this->global_Reader->GetXAxisInfo()).trimmed());
    this->ui->lineInfoYAxis->setText(QString("%1").arg(this->global_Reader->GetYAxisInfo()).trimmed());
    this->ui->lineInfoZAxis->setText(QString("%1").arg(this->global_Reader->GetZAxisInfo()).trimmed());

}

void MainWindow::KeyboardResetCamera()
{
    this->on_buttonCameraReset_clicked();
}

void MainWindow::on_actionInfoBarToggle_toggled(bool arg1)
{
    if (arg1)
    {
        infoTabDlg = new InfoBarDialog(this);
        infoTabDlg->setAttribute(Qt::WA_DeleteOnClose);
        QObject::connect(infoTabDlg, SIGNAL(destroyed()),this,SLOT(infoTabCloseSignal()));
        infoTabDlg->UpdateFitsInfo(global_Reader);
        infoTabDlg->show();

        this->infoTabOpen = true;
    }
    else
    {
        if (this->infoTabOpen)
        {
            infoTabDlg->close();
        }
    }
}

void MainWindow::on_actionFPSToggle_toggled(bool arg1)
{
    frameRateToggle(arg1);
}

void MainWindow::on_actionUserTesting_toggled(bool arg1)
{
    if (arg1)
    {
        userTest = new UserTesting(this);
        userTest->setAttribute(Qt::WA_DeleteOnClose);

        QRect rect = userTest->geometry();
        QPoint ptRenderer = this->ui->qvtkWidgetLeft->geometry().center();
        /// userTest MOve to Center of RenderWidget;
        //userTest->move(ptRenderer.x(), ptRenderer.y());
        QPoint ptMainWindow = this->geometry().topLeft();

        ptRenderer += ptMainWindow;

        rect.moveCenter(ptRenderer);
        userTest->setGeometry(rect);
        userTest->show();
#ifdef  __WIN32__
        userTest->loadCounterBalance("C:/UserTesting/counterbalance.txt");

#elif  __linux__
        userTest->loadCounterBalance("counterbalance.txt");
#endif
        QObject::connect(userTest,SIGNAL(startTest()),this,SLOT(startUserTest()));

    }
    else
    {
        userTest->close();
    }
}

void MainWindow::scaleButtonChanged()
{
    if (systemMode != Leap)
    {
        this->ui->buttonTransfScaling->setEnabled(true);
        //        std::cout << "Text changed" << endl;
        this->buttonEnablerTimer.start();
    }
}

void MainWindow::scaleButtonDelay()
{
    if (systemMode != Leap)
    {
        this->ui->buttonTransfScaling->setEnabled(false);
    }
}




/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///                CAMERA VIEW SECTION
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void MainWindow::on_buttonCameraReset_clicked()
{
    on_actionReset_Camera_triggered();

    if (this->userTestRunning())
        this->countInteraction(ResetCount);
}

void MainWindow::on_actionReset_Camera_triggered()
{

    vtkSmartPointer<vtkRenderer> theRenderer =
            vtkSmartPointer<vtkRenderer>::New();

    theRenderer = this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->
            GetRenderers()->GetFirstRenderer();

    theRenderer->ResetCamera();

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


    vtkRenderWindowInteractor * interactor = this->ui->qvtkWidgetLeft->GetInteractor();
    interactor->ExitCallback();

    ///
    /// \brief Reset the Transformation Coordinates
    ///
    resetTransformCoords();


    //theRenderer->
}



void MainWindow::on_actionFront_Side_View_triggered()
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

    theRenderer->GetActiveCamera()->Azimuth(0);


    this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();
}


void MainWindow::on_actionRear_Side_View_triggered()
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

    theRenderer->GetActiveCamera()->Azimuth(180);


    this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();
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
    theRenderer->GetActiveCamera()->OrthogonalizeViewUp();


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
    theRenderer->GetActiveCamera()->OrthogonalizeViewUp();


    this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();
}



//void MainWindow::actionDefault_triggered()
//{
//    this->ui->qvtkWidgetLeft->update();
//}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///                SUB-VOLUME SECTION
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///

void MainWindow::mouseBeginSubVol()
{

    std::cout << "BeginSubvolume Mouse" << endl;
    longSubVolMode_ = true;

    on_actionReset_Camera_triggered();

    //// CubeSource Generation

    vtkSmartPointer<vtkCubeSource> cubeVolume =
            vtkSmartPointer<vtkCubeSource>::New();

    vtkSmartPointer<vtkPolyDataMapper> cubeMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();

    cubeMapper->SetInputConnection(cubeVolume->GetOutputPort());
    vtkSmartPointer<vtkActor>cubeActor =
            vtkSmartPointer<vtkActor>::New();

    //    cubeVolume->SetInput(global_Reader->GetOutput());

    cubeVolume->SetBounds(global_Volume->GetBounds());

    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(1,1,1);

    /// MADE THE Cube Prop3D Invisible to maintain the cordinates of the SUbVolume Selected.
    cubeActor->GetProperty()->SetOpacity(0.0);

    this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->
            GetRenderers()->GetFirstRenderer()->AddActor(cubeActor);

    // global_subVolume = cubeVolume;

    ////
    /// \brief vtkBoxWidget
    ///
    vtkEventQtSlotConnect* vtkEventConnector =
            vtkEventQtSlotConnect::New();

    boxWidget_ = vtkCustomBoxWidget::New();
    boxWidget_->SetInteractor(this->ui->qvtkWidgetLeft->GetInteractor());
    boxWidget_->SetPlaceFactor(1.0);
    boxWidget_->KeyPressActivationOff();
    boxWidget_->InsideOutOn();

    //boxWidget_->SetInput(imageResample_->GetOutput());
    boxWidget_->RotationEnabledOff();

    boxWidget_->SetProp3D(cubeActor);
    boxWidget_->PlaceWidget();
    boxWidget_->EnabledOff();
    vtkEventConnector->Connect(boxWidget_, vtkCommand::InteractionEvent, this, SLOT(boxWidgetCallback()));




    if (this->userTestRunning())
        vtkEventConnector->Connect(boxWidget_,vtkCommand::StartInteractionEvent, userTestDlg, SLOT(incSubVolResize()));

    //    if (this->userTestRunning())
    //    {vtkEventQtSlotConnect* vtkUserTestEventConnector =
    //                vtkEventQtSlotConnect::New();

    //        vtkUserTestEventConnector->Connect(boxWidget_,vtkCommand::StartInteractionEvent, this, SLOT(countInteraction(SubVolResizeCount)));
    //    }

    boxWidget_->SetHandleSize(0.008);

    boxWidget_->EnabledOn();

    boxWidget_->InvokeEvent(vtkCommand::InteractionEvent);
}

void MainWindow::on_buttonSubVolReset_clicked()
{
    //on_actionReset_Camera_triggered();
    if(this->userTestRunning())
        this->countInteraction(SliceResetCount);

    switch (this->systemMode)
    {
    case Mouse:
    {
        if(this->userTestRunning())
            this->countInteraction(SubVolResetCount);

        boxWidget_->PlaceWidget(global_Volume->GetBounds());
        boxWidget_->InvokeEvent(vtkCommand::InteractionEvent);
    }
        break;

    case Leap:
    {
        //// Add the Placement of the 2 Widget Points

        double bounds[6];
        global_Reader->GetOutput()->GetBounds(bounds);

        double offset = 0.6;
        double p1XOffset = bounds[1] * offset;
        double p1YOffset = bounds[3] * offset;
        double p1ZOffset = bounds[5] * offset;

        offset = 0.3;
        double p2xOffset = bounds[1] * offset;
        double p2yOffset = bounds[3] * offset;
        double p2zOffset = bounds[5] * offset;


        double pt1[3] = {p1XOffset, p1YOffset, p1ZOffset};
        double pt2[3] = {p2xOffset, p2yOffset, p2zOffset};

        this->pointWidget1_->SetPosition(pt2);
        this->pointWidget2_->SetPosition(pt1);

        trackSubVolume(this->pointWidget2_->GetPosition(), this->pointWidget1_->GetPosition());

        this->pointWidget1_->InvokeEvent(vtkCommand::InteractionEvent);
        this->pointWidget2_->InvokeEvent(vtkCommand::InteractionEvent);


    }
        break;

    case Touch:
    {
        //// Add the Placement of the 2 Widget Points

        double bounds[6];
        global_Reader->GetOutput()->GetBounds(bounds);

        double offset = 0.3;
        double p1XOffset = bounds[1] * offset;
        double p1YOffset = bounds[3] * offset;
        double p1ZOffset = bounds[5] * offset;

        offset = 0.6;
        double p2xOffset = bounds[1] * offset;
        double p2yOffset = bounds[3] * offset;
        double p2zOffset = bounds[5] * offset;


        double pt1[3] = {p1XOffset, p1YOffset, p1ZOffset};
        double pt2[3] = {p2xOffset, p2yOffset, p2zOffset};

        this->pointWidget1_->SetPosition(pt1);
        this->pointWidget2_->SetPosition(pt2);

        this->pointWidget1_->InvokeEvent(vtkCommand::InteractionEvent);
        this->pointWidget2_->InvokeEvent(vtkCommand::InteractionEvent);
    }
        break;
    }

    /// Refresh the Interaction screen.
    this->ui->qvtkWidgetLeft->GetRenderWindow()->Render();
}

//void MainWindow::loadSubVolume(QVTKWidget *qvtkWidget, vtkFitsReader *source)
//{
//    double subVolBounds[6];
//    global_subVolume->GetOutput()->GetBounds(subVolBounds);

//    SubVolumeDialog subVol;

//    subVol.setModal(true);
//    subVol.UpdateQVTKWidget(this->ui->qvtkWidgetLeft, global_Reader, subVolBounds);
//    connect(&subVol,SIGNAL(windowClosing(bool)),this,SLOT(infoTabCloseSignal(bool)));
////    //subVol
////    //subVol->ui->
//    subVol.exec();
//}

void MainWindow::on_checkSubVolVolumeEnable_stateChanged(int arg1)
{
    global_Volume->SetVisibility(arg1);
}



//void MainWindow::Buttom_SubVol_clicked()
//{
//    loadSubVolume(this->ui->qvtkWidgetLeft, global_Reader );
//}


void MainWindow::on_actionSubVolumeXY_triggered()
{
    longSubVolMode_ = false;

    on_actionReset_Camera_triggered();

    XYVolumeSelection(this->ui->qvtkWidgetLeft, global_Reader);

}

void MainWindow::on_action_SubVolume_Z_triggered()
{
    on_actionLeft_Side_View_triggered();

    ZVolumeSelection(this->ui->qvtkWidgetLeft, global_Reader);

}

void MainWindow::on_buttonSubVolExport_clicked()
{
    std::cout << "on_buttonSubVolExport_clicked()" << endl;

    //double minX, maxX, minY, maxY, minZ, maxZ;
    double subVolBounds[6] ;


    if (this->systemMode == Mouse)
        ////    if (longSubVolMode_)
    {
        subVolBounds[0] = global_subVolBounds_[0];
        subVolBounds[1] = global_subVolBounds_[1];
        subVolBounds[2] = global_subVolBounds_[2];
        subVolBounds[3] = global_subVolBounds_[3];
        subVolBounds[4] = global_subVolBounds_[4];
        subVolBounds[5] = global_subVolBounds_[5];

        //subVolBounds[6] = {minX, maxX, minY, maxY, minZ, maxZ};

    }
    if (this->systemMode == Leap)
    {
        global_subVolume->GetOutput()->GetBounds(subVolBounds);
    }

    if (this->systemMode == Touch)
    {
        global_subVolume->GetOutput()->GetBounds(subVolBounds);
    }

    SubVolumeDialog subVol;
    subVol.setModal(true);

    subVol.UpdateQVTKWidget(this->ui->qvtkWidgetLeft, global_Reader, subVolBounds);
    subVol.exec();
}

void MainWindow::on_actionSubVol_Export_triggered()
{
    std::cout << "on_actionSubVol_Export_Triggered()" << endl;
    double subVolBounds[6];

    global_subVolume->GetOutput()->GetBounds(subVolBounds);

    SubVolumeDialog subVol;
    subVol.setModal(true);
    subVol.UpdateQVTKWidget(this->ui->qvtkWidgetLeft, global_Reader, subVolBounds );
    subVol.exec();
}

void MainWindow::on_actionSubVolSelection_triggered()
{
    this->closeTabs();
    //    this->ui->SubVolTab = new QWidget();
    this->ui->tabLogWidget->insertTab(0, this->ui->SubVolTab,"Sub-Volume Extraction");
    this->ui->tabLogWidget->setCurrentIndex(0);
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///                AXIS SLICE SECTION
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void MainWindow::beginSliceAxis()
{
    //this->ui->buttonAxisSliceLoad->setDefault(true);

    double reductionFactor = 2.0;

    global_Reader->GetOutput()->GetDimensions(imageDims);

    LoadSliderWidgets();


    this->xAxisSlice = imageDims[0]/2;
    this->yAxisSlice = imageDims[1]/2;
    this->zAxisSlice = imageDims[2]/2;

    this->ui->lbl_XSlice->setText(QString("%1").arg(xAxisSlice));
    this->ui->lbl_YSlice->setText(QString("%1").arg(yAxisSlice));
    this->ui->lbl_ZSlice->setText(QString("%1").arg(zAxisSlice));

    ///
    /// \brief resample
    ///
    ///
    vtkImageResample *resample = vtkImageResample::New();

    resample->SetInputConnection( global_Reader->GetOutputPort() );
    resample->SetAxisMagnificationFactor(0, reductionFactor);
    resample->SetAxisMagnificationFactor(1, reductionFactor);
    resample->SetAxisMagnificationFactor(2, reductionFactor);

    for (int i = 0; i < 3; i++)
    {
        riw[i] = vtkSmartPointer< vtkResliceImageViewer >::New();
    }

    this->ui->sliceView1->SetRenderWindow(riw[0]->GetRenderWindow());
    riw[0]->SetupInteractor(
                this->ui->sliceView1->GetRenderWindow()->GetInteractor());

    this->ui->sliceView2->SetRenderWindow(riw[1]->GetRenderWindow());
    riw[1]->SetupInteractor(
                this->ui->sliceView2->GetRenderWindow()->GetInteractor());

    this->ui->sliceView3->SetRenderWindow(riw[2]->GetRenderWindow());
    riw[2]->SetupInteractor(
                this->ui->sliceView3->GetRenderWindow()->GetInteractor());


    for (int i = 0; i < 3; i++)
    {
        // make them all share the same reslice cursor object.
        vtkResliceCursorLineRepresentation *rep =
                vtkResliceCursorLineRepresentation::SafeDownCast(
                    riw[i]->GetResliceCursorWidget()->GetRepresentation());
        riw[i]->SetResliceCursor(riw[0]->GetResliceCursor());

        rep->GetResliceCursorActor()->
                GetCursorAlgorithm()->SetReslicePlaneNormal(i);

        riw[i]->SetInput(global_Reader->GetOutput());
        riw[i]->SetSliceOrientation(i);
        riw[i]->SetResliceModeToAxisAligned();
    }

    vtkSmartPointer<vtkCellPicker> picker =
            vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance(0.005);

    vtkSmartPointer<vtkProperty> ipwProp =
            vtkSmartPointer<vtkProperty>::New();

    vtkSmartPointer< vtkRenderer > ren =
            this->defaultRenderer;
    vtkRenderWindowInteractor *iren = this->ui->qvtkWidgetLeft->GetInteractor();

    for (int i = 0; i < 3; i++)
    {
        planeWidget[i] = vtkSmartPointer<vtkImagePlaneWidget>::New();
        planeWidget[i]->SetInteractor( iren );
        planeWidget[i]->SetPicker(picker);
        planeWidget[i]->RestrictPlaneToVolumeOn();
        double color[3] = {0, 0, 0};
        color[i] = 1;
        planeWidget[i]->GetPlaneProperty()->SetColor(color);

        color[0] /= 3.0;
        color[1] /= 3.0;
        color[2] /= 3.0;
        riw[i]->GetRenderer()->SetBackground( color );

        planeWidget[i]->SetTexturePlaneProperty(ipwProp);
        planeWidget[i]->TextureInterpolateOff();
        planeWidget[i]->SetResliceInterpolateToLinear();
        planeWidget[i]->SetInput(global_Reader->GetOutput());
        planeWidget[i]->SetPlaneOrientation(i);
        planeWidget[i]->SetSliceIndex(imageDims[i]/2);
        planeWidget[i]->DisplayTextOn();
        planeWidget[i]->SetDefaultRenderer(ren);
        planeWidget[i]->On();
        planeWidget[i]->InteractionOn();

    }

    this->ui->sliceView1->show();
    this->ui->sliceView2->show();
    this->ui->sliceView3->show();

    vtkEventQtSlotConnect* vtkEventConnector =
            vtkEventQtSlotConnect::New();

    connect(this->ui->buttonAxisSliceReset, SIGNAL(clicked()),this,SLOT(ResetViews()));
    connect(this->ui->resetButtonX, SIGNAL(clicked()),this, SLOT(ResetViewX()));
    connect(this->ui->resetButton_Y, SIGNAL(clicked()),this, SLOT(ResetViewY()));
    connect(this->ui->resetButton_Z, SIGNAL(clicked()),this, SLOT(ResetViewZ()));


    vtkEventConnector->Connect(sliderWidgetX, vtkCommand::InteractionEvent, this, SLOT(UpdateSliceX()));
    vtkEventConnector->Connect(sliderWidgetY, vtkCommand::InteractionEvent, this, SLOT(UpdateSliceY()));
    vtkEventConnector->Connect(sliderWidgetZ, vtkCommand::InteractionEvent, this, SLOT(UpdateSliceZ()));

    connect(this->ui->checkBox_SliceX_Enable, SIGNAL(stateChanged(int)), this, SLOT(sliceXEnable(int)));
    connect(this->ui->checkBox_SliceY_Enable, SIGNAL(stateChanged(int)), this, SLOT(sliceYEnable(int)));
    connect(this->ui->checkBox_SliceZ_Enable, SIGNAL(stateChanged(int)), this, SLOT(sliceZEnable(int)));

    ///////////////////////////////////////
    // This is a quick fix to AutoAlign the camera to maximise view of Each Side render panel
    // I have to do it twice for Z Windows because I have 2 (ie: I have no idea )
    for (int i = 0; i < 3; i++)
    {
        riw[i]->GetRenderer()->ResetCamera();
        riw[i]->Render();
    }

    for (int i = 0; i < 3; i++)
    {
        riw[i]->GetRenderer()->ResetCamera();
        riw[i]->Render();
    }
}

void MainWindow::on_actionSliceAxisAligned_triggered()
{
    closeTabs();
    this->ui->tabLogWidget->insertTab(0,this->ui->SliceVolTab,"Axis-Alligned Slice");
    this->ui->tabLogWidget->setCurrentIndex(0);
}



void MainWindow::leapBeginSubVol()
{
    on_actionTracking_triggered();

    addSecondPointer();

    //// Add the Placement of the 2 Widget Points

    double bounds[6];
    global_Reader->GetOutput()->GetBounds(bounds);

    double offset = 0.6;
    double p1XOffset = bounds[1] * offset;
    double p1YOffset = bounds[3] * offset;
    double p1ZOffset = bounds[5] * offset;

    offset = 0.3;
    double p2xOffset = bounds[1] * offset;
    double p2yOffset = bounds[3] * offset;
    double p2zOffset = bounds[5] * offset;


    double pt1[3] = {p1XOffset, p1YOffset, p1ZOffset};
    double pt2[3] = {p2xOffset, p2yOffset, p2zOffset};

    this->pointWidget1_->SetPosition(pt2);
    this->pointWidget2_->SetPosition(pt1);

    trackSubVolume(this->pointWidget2_->GetPosition(), this->pointWidget1_->GetPosition());

    this->pointWidget1_->InvokeEvent(vtkCommand::InteractionEvent);
    this->pointWidget2_->InvokeEvent(vtkCommand::InteractionEvent);

    // Create a sample listener and Controller
    this->controller_= new Controller;
    Leaping_ = true;
    this->leapSubVolMode = true;

    this->ui->checkBox_SubVolume->setChecked(true);



    //    this->ui->tabLogWidget->insertTab(2, this->ui->LeapTab,"Leap Information");
    //    this->ui->tabLogWidget->setCurrentIndex(2);

    this->ui->tabLogWidget->insertTab(0, this->ui->SubVolTab,"Sub-Volume Extraction");
    this->ui->tabLogWidget->setCurrentIndex(0);

    if (userTestRunning())
    {
        vtkEventQtSlotConnect * ptWidget1Connector = vtkEventQtSlotConnect::New();
        ptWidget1Connector->Connect(this->pointWidget1_,vtkCommand::StartInteractionEvent,userTestDlg,SLOT(incSubVolResize()));

        vtkEventQtSlotConnect * ptWidget2Connector = vtkEventQtSlotConnect::New();
        ptWidget2Connector->Connect(this->pointWidget2_,vtkCommand::StartInteractionEvent,userTestDlg,SLOT(incSubVolResize()));

    }
}

void MainWindow::leapBeginSliceAxis()
{
    beginSliceAxis();

    std::cout << "Loaded beginSliceAxis () in Leap Call" << endl;

    this->controller_= new Controller;
    Leaping_ = true;
    this->leapAxisSliceMode = true;

    this->ui->checkBox_S->setChecked(true);



    this->ui->tabLogWidget->insertTab(2, this->ui->LeapTab,"Leap Info");
    this->ui->tabLogWidget->setCurrentIndex(2);

    this->ui->tabLogWidget->insertTab(0, this->ui->SliceVolTab,"Axis-Alligned Slice");
    this->ui->tabLogWidget->setCurrentIndex(0);
}

void MainWindow::leapBeginSliceArb()
{
    beginSliceArb();

    std::cout << "Loaded beginSliceArb () in Leap Call" << endl;

    this->controller_= new Controller;
    Leaping_ = true;
    this->leapArbSliceMode = true;

    this->ui->checkBox_S->setChecked(true);


    this->ui->tabLogWidget->insertTab(2, this->ui->LeapTab,"Leap Info");
    this->ui->tabLogWidget->setCurrentIndex(2);

    this->ui->tabLogWidget->insertTab(0, this->ui->SliceVolTab2,"Arbitrary Slice");
    this->ui->tabLogWidget->setCurrentIndex(0);
}

void MainWindow::leapArbSliceUpdate(Frame frame, Hand hand, bool &sliceMovement)
{
    const FingerList rightFingers = hand.fingers();
    const FingerList extendedRight = hand.fingers().extended();

    //Finger rightThumb = hand.fingers().fingerType(Finger::TYPE_THUMB))[0];
    //Finger rightIndex = hand.fingers().fingerType(Finger::TYPE_INDEX))[0];

    //leapMovement = false;

    //  std::cout << " 1. Movement = " << leapMovement << "\t";

    //std::cout << "Extended Count: " << extendedRight.count() << endl;
    if(extendedRight.count() ==5)
    {

        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////
        /// \brief ROTATE STATE
        ///
        Vector newNormal = hand.palmNormal();
        double newNormalD[3] = {newNormal.x, newNormal.y, newNormal.z};


        //                    vtkPlane * plane = vtkPlane::New();
        //                    this->customArbPlaneWidget->GetPlane(plane);
        //                    vtkMath::Normalize(newNormalD);

        // plane->SetNormal(newNormalD);
        this->customArbPlaneWidget->SetNormal(newNormalD);
        this->customArbPlaneWidget->UpdatePlacement();
        this->customArbPlaneWidget->leapMovingNormal(true);

        //this->customArbPlaneWidget->InvokeEvent(vtkCommand::EnableEvent);
        //this->customArbPlaneWidget->InvokeEvent(vtkCommand::StartInteractionEvent);
        this->customArbPlaneWidget->InvokeEvent(vtkCommand::InteractionEvent);

        if(this->userTestRunning())
        {
            leapHand1Move = true;
            leapHand1FrameBuffer ++;
            sliceMovement = true;
        }
    }

    if(extendedRight.count() == 0)
    {
        //////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////
        /// \brief PUSH STATE
        ///
        Vector newMotion = frame.translation(controller_->frame(1));
        double newMotionD[3] = {newMotion.x, newMotion.y, newMotion.z};

        vtkPlane * plane = vtkPlane::New();
        this->customArbPlaneWidget->GetPlane(plane);

        plane->Push(vtkMath::Dot(newMotionD, plane->GetNormal()));
        this->customArbPlaneWidget->SetOrigin(plane->GetOrigin());

        this->customArbPlaneWidget->UpdatePlacement();
        this->customArbPlaneWidget->leapMovingPlane(true);

        ///////////////////////////////////////////////////////////

        //   this->customArbPlaneWidget->InvokeEvent(vtkCommand::EnableEvent);
        //  this->customArbPlaneWidget->InvokeEvent(vtkCommand::StartInteractionEvent);
        this->customArbPlaneWidget->InvokeEvent(vtkCommand::InteractionEvent);

        if(this->userTestRunning())
        {
            leapHand1Move = true;
            leapHand1FrameBuffer ++;
            sliceMovement = true;
        }
    }
}

void MainWindow::leapSubVolumeUpdate(Frame frame, Hand hand, bool &subVolRightHand, bool &subVolLeftHand)
{
    /// Assume the first hand is the Right Hand. If it is the right hand then 2nd Hand is left.

    Hand rightHand = frame.hands().rightmost();
    Hand leftHand = frame.hands().leftmost();
    bool leftHandActive = true;
    bool rightHandActive = true;

    bool leftPinch = false;
    bool rightPinch = false;

    bool leftGrab = false;
    bool rightGrab = false;


    /// If Not the Right Hand then reverse outcome
    /// If RightHand is Active then
    if (frame.hands().count() == 1)
    {
        if (frame.hands().frontmost().isRight())
            leftHandActive = false;
        else
            rightHandActive = false;
    }

    vtkRenderer * renderer = this->defaultRenderer;
    vtkCamera *camera = renderer->GetActiveCamera();

    //                if (leftHand.isLeft() && frame.hands.count() == 2)
    //                {
    //                    firstHand
    //                }


    //                if (firstHand.isRight())
    //                    Hand rightHand = firstHand;
    //                else
    //                    Hand leftHand = firstHand;

    //                if (frame.hands().count() > 1)
    //                    if



    //                const FingerList leftFingers = frame.hands().leftmost().fingers();
    //                const FingerList rightFingers = frame.hands().rightmost().fingers();

    const FingerList extendedLeft = leftHand.fingers().extended();
    const FingerList extendedRight = rightHand.fingers().extended();

    Finger leftThumb = (leftHand.fingers().fingerType(Finger::TYPE_THUMB))[0];
    Finger rightThumb = (rightHand.fingers().fingerType(Finger::TYPE_THUMB))[0];

    Finger leftIndex = (leftHand.fingers().fingerType(Finger::TYPE_INDEX))[0];
    Finger rightIndex = (rightHand.fingers().fingerType(Finger::TYPE_INDEX))[0];

    /// PRINTOUT OF STATUSSES
    //                std::cout << "Right Thumb out: " << rightThumb.isExtended() << " \t" ;
    //                std::cout << "Left  Thumb out: " << leftThumb.isExtended() << " \t" ;

    //                std::cout << "Right Index out: " << rightIndex.isExtended() << " \t" ;
    //                std::cout << "Left  Index out: " << leftIndex.isExtended() << " \t" ;

    //                std::cout << "left: #" << extendedLeft.count() << "\t";
    //                std::cout << "Right #: " << extendedRight.count() << "\t";

    //                std::cout << "front Left Finger: " << extendedLeft.frontmost() << "\t";
    //                std::cout << "front Right Finger: " << extendedRight.frontmost() << "\t";

    /////////////////////////////////////////////////////////////
    //                std::cout << "left Pinch: " << frame.hands().leftmost().pinchStrength() << "\t";

    //                std::cout << "hand Pinch: " << frame.hands().rightmost().pinchStrength() << "\t";

    //

    //                float leftPinch = frame.hands().leftmost().pinchStrength();

    //               float rightPinch = frame.hands().rightmost().pinchStrength();


    //Finger leftFinger =


    //                if( leftPinch > 0.5 && rightPinch > 0.5)
    //                {

    /////////////////////////////////////////////////////////////
    ///////////////////////////////////
    /// Code for UI Checkboxes
    ///

    if(leftHandActive)
    {
        if (leftHand.isLeft())
        {
            this->ui->checkbx_SubVolLeapLeftHand->setChecked(true);
        }

/////       if (leftHand.translationProbability(controller_->frame(1)) > 0.7)
       if (leftHand.grabStrength() == 1)
       {
            leftGrab = true;
            this->ui->checkbx_SubVolLeapLeftGrab->setChecked(true);
       }


        /// SOURCE CODE TAKE FROM
        /// http://blog.leapmotion.com/grabbing-and-throwing-small-objects-ragdoll-style/
        ///

        /// Thumb TIp is the pinch Position
       {
           Vector thumb_tip = leftHand.fingers()[0].tipPosition();

           Bone bone;
           Bone::Type boneType;

           boneType = static_cast<Bone::Type>(3);
           bone = leftHand.fingers()[1].bone(boneType);
           Vector bone_Pos = bone.center();
           Vector distance = thumb_tip - bone_Pos;

           if (distance.magnitude() < 30)
           {
               leftPinch = true;
               this->ui->checkbx_SubVolLeapLeftPinch->setChecked(true);
           }
       }
    }


    if (rightHandActive)
    {
        if (rightHand.isRight())
        {
            this->ui->checkbx_SubVolLeapRightHand->setChecked(true);            
        }

        if(rightHand.grabStrength() == 1)
        {
            rightGrab = true;
            this->ui->checkbx_SubVolLeapRightGrab->setChecked(true);
        }

//        if(rightThumb.isExtended())
//        {
//            this->ui->checkbx_SubVolLeapRightThumb->setChecked(true);
//        }


        /// Thumb TIp is the pinch Position
       {
           Vector thumb_tip = rightHand.fingers()[0].tipPosition();

           Bone bone;
           Bone::Type boneType;

           boneType = static_cast<Bone::Type>(3);
           bone = rightHand.fingers()[1].bone(boneType);
           Vector bone_Pos = bone.center();
           Vector distance = thumb_tip - bone_Pos;

           if (distance.magnitude() < 30)
           {
               rightPinch = true;
               this->ui->checkbx_SubVolLeapRightPinch->setChecked(true);
           }
       }
    }

//    if(leftHandActive)
//    {
//        if (leftHand.isLeft())
//        {
//            translateLeft++;
//            this->ui->checkbx_SubVolLeapLeftHand->setChecked(true);
//        }

/////       if (leftHand.translationProbability(controller_->frame(1)) > 0.7)
//       if (leftIndex.isExtended())
//       {
//           translateLeft++;
//            this->ui->checkbx_SubVolLeapLeftIndex->setChecked(true);
//       }

//        if(leftThumb.isExtended())
//        {
//            translateLeft++;
//            this->ui->checkbx_SubVolLeapLeftThumb->setChecked(true);
//        }
//    }

//    if (rightHandActive)
//    {
//        if (rightHand.isRight())
//        {
//            translateRight++;
//            this->ui->checkbx_SubVolLeapRightHand->setChecked(true);
//        }

//        if(rightIndex.isExtended())
//        {
//            translateRight++;
//            this->ui->checkbx_SubVolLeapRightIndex->setChecked(true);
//        }

//        if(rightThumb.isExtended())
//        {
//            translateRight ++ ;
//            this->ui->checkbx_SubVolLeapRightThumb->setChecked(true);
//        }
//    }


    //// WE TRACK THE LEFT HAND for SCALE

    if(
            (frame.hands().leftmost().isLeft())
            //&& (leftHand.grabStrength() == 1)
            && leftPinch
            //&& (extendedLeft.count() == 0)
            //&& (leftHandActive  )
            && !leftGrab
            )
//          &&   leftHand.translationProbability(controller_->frame(1)) > 0.7)
    {

        ////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////    FINGER ACCURACY BUT POOR DEPTH CAPTURE    ///////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////

        /// Vector newPosition = hand.translation(controller_->frame(1));
        ///Vector newPosition = hand.translation(controller_->frame(1))..fingers().frontmost().);

        /// Get the Leap Motion Vector
        Vector hand1OldPos = controller_->frame(1).hands().leftmost().stabilizedPalmPosition();
        Vector hand1NewPos = controller_->frame().hands().leftmost().stabilizedPalmPosition();

        //controller_->frame().hands().leftmost().

        Vector leapLeftMove = hand1NewPos-hand1OldPos;  /// Vector of the Finger Movement in Leap SPACE
        ///
        ///Vector leapLeftMove = leftHand.translation(controller_->frame(1));

        /// Get the Camera Orientation and angle
        double * camOrientation = camera->GetOrientationWXYZ();
        double angle = camOrientation[0];
        Vector cameraAngle = Vector(camOrientation[1], camOrientation[2], camOrientation[3]);

        const double PI = 3.141592653589793;
        /// Generate the Matrix and Transforms
        Matrix transformMatrix = Matrix();
        transformMatrix.identity();
        transformMatrix.setRotation(cameraAngle,angle * (PI / 180));
        Vector rotatedLeftVector = transformMatrix.transformDirection(leapLeftMove);

        /// Get the pointWidget Position
        double * newPosition;
        double oldPosition[3];

        pointWidget1_->GetPosition(oldPosition);

        /// Generate the Transform and Apply it
        vtkTransform * AggregateTransform =  vtkTransform::New();
        AggregateTransform->Identity();
        AggregateTransform->Translate(rotatedLeftVector.x, rotatedLeftVector.y, rotatedLeftVector.z);
        newPosition = AggregateTransform->TransformPoint(oldPosition);

        /// Apply the New Position to the widget
        pointWidget1_->SetPosition(newPosition[0],newPosition[1],newPosition[2]);

        pointWidget1_->InvokeEvent(vtkCommand::InteractionEvent);

        vtkProperty * pointerProperty = pointWidget1_->GetProperty();


        pointerProperty->SetColor(0.8900, 0.8100, 0.3400);
        pointWidget1_->InvokeEvent(vtkCommand::StartInteractionEvent);

        if(this->userTestRunning())
        {
            leapHand2Move = true;
            leapHand2FrameBuffer ++;
            subVolLeftHand = true;
        }
    }

    //// WE TRACK THE RIGHT HAND for Scale
    if(
            (frame.hands().rightmost().isRight())
            //&& (leftHand.grabStrength() == 1)
            //&& (extendedRight.count() == 0)
            && rightPinch
            //&& (rightHandActive  )
            && !rightGrab
            )
    {
        ////////////////////////////////////////////////////////
        /// \brief Second Hand capture
        ///


        ///Vector newPosition2 = hand2.translation(controller_->frame(1));
        ///Vector newPosition2 = hand2.translation(controller_->frame(1).fingers().frontmost().tipPosition());
        ///


        //                    Vector hand2OldPos = controller_->frame(2).hands().leftmost().stabilizedPalmPosition();
        //                    Vector hand2NewPos = controller_->frame(1).hands().leftmost().stabilizedPalmPosition();
        /// Get the Leap Motion Vector
        Vector hand2OldPos = controller_->frame(1).hands().rightmost().stabilizedPalmPosition();
        Vector hand2NewPos = controller_->frame().hands().rightmost().stabilizedPalmPosition();

        Vector leapRightMove = hand2NewPos-hand2OldPos;  /// Vector of the Finger Movement in Leap SPACE

        /// Get the Camera Orientation and angle
        double * camOrientation = camera->GetOrientationWXYZ();
        double angle = camOrientation[0];
        Vector cameraAngle = Vector(camOrientation[1], camOrientation[2], camOrientation[3]);

        const double PI = 3.141592653589793;
        /// Generate the Matrix and Transforms
        Matrix transformMatrix = Matrix();
        transformMatrix.identity();
        transformMatrix.setRotation(cameraAngle,angle * (PI / 180));
        Vector rotatedRightVector = transformMatrix.transformDirection(leapRightMove);

        /// Get the pointWidget Position
        /// USE the Same Position Indicators
        double * newPosition;
        double oldPosition[3];

        pointWidget2_->GetPosition(oldPosition);

        /// Generate the Transform and Apply it
        vtkTransform * AggregateTransform =  vtkTransform::New();
        AggregateTransform->Identity();
        AggregateTransform->Translate(rotatedRightVector.x, rotatedRightVector.y, rotatedRightVector.z);
        newPosition = AggregateTransform->TransformPoint(oldPosition);

        /// Apply the New Position to the widget
        pointWidget2_->SetPosition(newPosition[0],newPosition[1],newPosition[2]);

        pointWidget2_->InvokeEvent(vtkCommand::InteractionEvent);

        vtkProperty * pointerProperty =
                pointWidget2_->GetProperty();

        pointerProperty->SetColor(0.3400, 0.8100, 0.8900);
        //pointerProperty->SetColor(0.8900, 0.8100, 0.3400);

        if(this->userTestRunning())
        {
            leapHand1Move = true;
            leapHand1FrameBuffer ++;
            subVolRightHand = true;
        }

    }



    //////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////
    ///         BOX TRANSLATION     ///
    /// ///////////////////////////////////////////////////////////
    ///
    ///
    ///
    if (leftGrab && rightGrab)
    {


        bool widget1Valid = false;
        bool widget2Valid = false;

        double pickPoint1[4], pickPoint2[4];

        {
            ///std::cout << "VALID TRANSLATE LOCATION \t[x,y]: " << finger.x() << ", " << finger.y() << endl;


            //    int threshold = 50;

            /// 1. Compute the new Position by Getting the PointWidget Focal Point
            /// 2. Getting the Finger Actor 2D position into 3D World Position using FocalPoint Z axis
            /// 3. Create the Translation transformation from finger by lookint at last position
            /// 3. Apply the translation to the widget position
            /// 4. PointWidget Update

            //// --------MOVE PointWidget 1 with HAND Left TRANSFORMATION--------"

            {

    //             std::cout << "Finger Translation: New[x,y]: " << actorPos2D[0] << ", " << actorPos2D[1]
    //                                        << "\t Old[x,y]: " << actorPos2D2[0] << ", " << actorPos2D2[1] << endl;

                /// If the NEW pointWidget 1 IS NOT ON the Volume Bounds, Set pointWidget Change to true
                ///



                 /// Get the Motion Vector of the Hand
                 ///
                Vector hand1OldPos = controller_->frame(1).hands().rightmost().stabilizedPalmPosition();
                Vector hand1NewPos = controller_->frame().hands().rightmost().stabilizedPalmPosition();

                Vector leapRightMove = hand1NewPos-hand1OldPos;  /// Vector of the Finger Movement in Leap SPACE

                /// Get the Camera Orientation and angle
                double * camOrientation = camera->GetOrientationWXYZ();
                double angle = camOrientation[0];
                Vector cameraAngle = Vector(camOrientation[1], camOrientation[2], camOrientation[3]);

                const double PI = 3.141592653589793;
                /// Generate the Matrix and Transforms
                Matrix transformMatrix = Matrix();
                transformMatrix.identity();
                transformMatrix.setRotation(cameraAngle,angle * (PI / 180));
                Vector rotatedRightVector = transformMatrix.transformDirection(leapRightMove);

                /// Get the pointWidget Position
                double * newPosition;
                double oldPosition[3];

                pointWidget1_->GetPosition(oldPosition);

                /// Generate the Transform and Apply it
                vtkTransform * AggregateTransform =  vtkTransform::New();
                AggregateTransform->Identity();
                AggregateTransform->Translate(rotatedRightVector.x, rotatedRightVector.y, rotatedRightVector.z);
                newPosition = AggregateTransform->TransformPoint(oldPosition);

                /// Send New 3D Point to World

                ///pointWidget1_->ComputeDisplayToWorld(defaultRenderer, (newPosition[0]), newPosition[1], newPosition, pickPoint1);


                /// If the NEW pointWidget 1 IS NOT ON the Volume Bounds, Set pointWidget Change to true
                ///

                bool xRangeValid = (newPosition[0] >= global_Volume->GetBounds()[0]) && (newPosition[0] <= global_Volume->GetBounds()[1]);
                bool yRangeValid = (newPosition[1] >= global_Volume->GetBounds()[2]) && (newPosition[1] <= global_Volume->GetBounds()[3]);
                bool zRangeValid = (newPosition[2] >= global_Volume->GetBounds()[4]) && (newPosition[2] <= global_Volume->GetBounds()[5]);

                if (xRangeValid && yRangeValid && zRangeValid)
                {
                      widget1Valid = true;
                      pickPoint1[0] = newPosition[0];
                      pickPoint1[1] = newPosition[1];
                      pickPoint1[2] = newPosition[2];
                }




            }

            //// --------MOVE PointWidget 2 with HAND 1 TRANSFORMATION--------"

            {

    //             std::cout << "Finger Translation: New[x,y]: " << actorPos2D[0] << ", " << actorPos2D[1]
    //                                        << "\t Old[x,y]: " << actorPos2D2[0] << ", " << actorPos2D2[1] << endl;


                Vector hand1OldPos = controller_->frame(1).hands().rightmost().stabilizedPalmPosition();
                Vector hand1NewPos = controller_->frame().hands().rightmost().stabilizedPalmPosition();

                Vector rightLeftMove = hand1NewPos-hand1OldPos;  /// Vector of the Finger Movement in Leap SPACE

                /// Get the Camera Orientation and angle
                double * camOrientation = camera->GetOrientationWXYZ();
                double angle = camOrientation[0];
                Vector cameraAngle = Vector(camOrientation[1], camOrientation[2], camOrientation[3]);

                const double PI = 3.141592653589793;
                /// Generate the Matrix and Transforms
                Matrix transformMatrix = Matrix();
                transformMatrix.identity();
                transformMatrix.setRotation(cameraAngle,angle * (PI / 180));
                Vector rotatedLeftVector = transformMatrix.transformDirection(rightLeftMove);

                /// Get the pointWidget Position
                double * newPosition;
                double oldPosition[3];

                pointWidget2_->GetPosition(oldPosition);

                /// Generate the Transform and Apply it
                vtkTransform * AggregateTransform =  vtkTransform::New();
                AggregateTransform->Identity();
                AggregateTransform->Translate(rotatedLeftVector.x, rotatedLeftVector.y, rotatedLeftVector.z);
                newPosition = AggregateTransform->TransformPoint(oldPosition);

                /// Send New 3D Point to World

                ///pointWidget1_->ComputeDisplayToWorld(defaultRenderer, (newPosition[0]), newPosition[1], newPosition, pickPoint1);


                /// If the NEW pointWidget 1 IS NOT ON the Volume Bounds, Set pointWidget Change to true
                ///

                bool xRangeValid = (newPosition[0] >= global_Volume->GetBounds()[0]) && (newPosition[0] <= global_Volume->GetBounds()[1]);
                bool yRangeValid = (newPosition[1] >= global_Volume->GetBounds()[2]) && (newPosition[1] <= global_Volume->GetBounds()[3]);
                bool zRangeValid = (newPosition[2] >= global_Volume->GetBounds()[4]) && (newPosition[2] <= global_Volume->GetBounds()[5]);

                 if (xRangeValid && yRangeValid && zRangeValid)
                 {
                       widget2Valid = true;
                       pickPoint2[0] = newPosition[0];
                       pickPoint2[1] = newPosition[1];
                       pickPoint2[2] = newPosition[2];
                 }
            }

            /// If the widgets are Both Valid Translations, apply the New Point Positions
            if (widget1Valid && widget2Valid)
            {
                if (userTestRunning() && !pointWidget1Active && !pointWidget2Active)
                {
                    userTestDlg->incSubVolPoint1();
                    userTestDlg->incSubVolPoint2();
                }

                if (!pointWidget2Active && !pointWidget1Active)
                {
                    std::cout << "SubVolume TRIGGERED" << endl ;
                    pointWidget1Active = true;
                    pointWidget2Active = true;
                }

                pointWidget1_->SetPosition(pickPoint1);

                /// Interact, if desired



                vtkProperty * pointerProperty1 = pointWidget1_->GetProperty();
                pointerProperty1->SetColor(0.8900, 0.8100, 0.3400);

                pointWidget1_->InvokeEvent(vtkCommand::InteractionEvent, NULL);



                pointWidget2_->SetPosition(pickPoint2);

                vtkProperty * pointerProperty2 =
                        pointWidget2_->GetProperty();

                pointerProperty2->SetColor(0.3400, 0.8100, 0.8900);

                /// Interact, if desired

                pointWidget2_->InvokeEvent(vtkCommand::InteractionEvent, NULL);
            } /// if (widget1Valid && widget2Valid)
        } ///(validXmax && validXmin && validYmin && validYmax &&
    }

//    std::cout << "Left Pinch: " << leftHand.pinchStrength();
//    std::cout << "Left Fingers: \t" ;
//    for(Leap::FingerList::const_iterator finger = extendedLeft.begin(); finger != extendedLeft.end(); finger++)
//    {
//        std::cout << (*finger).toString() << "\t" ;
//    }

//    std::cout << "\t\t";

//    std::cout << "Right Pinch: " << rightHand.pinchStrength();
//    std::cout << "Right Fingers: \t" ;
//    for(Leap::FingerList::const_iterator finger = extendedRight.begin(); finger != extendedRight.end(); finger++)
//    {
//        std::cout << (*finger).toString() << "\t" ;
//    }

//    std::cout << endl;


    ///////////////////////////////

//    if (translateLeft ==3 && translateRight == 3)
//    {
//        std::cout << "TRANSLATION REQUIRED" << endl;
//    }
//    else
//    {
//        translateLeft = 0;
//        translateRight = 0;

//    }


    double* finalPos1 = pointWidget1_->GetPosition();
    double* finalPos2 = pointWidget2_->GetPosition();

    trackSubVolume(finalPos1, finalPos2);       

}

void MainWindow::leapTranslateUpdate(Frame frame, bool &translateMovement , Hand hand)
{
    leapHand1Move = true;
    leapHand1FrameBuffer ++;
    translateMovement = true;

//    this->ui->labelTranslation->setFont(boldFont);
    this->ui->buttonTransfTranslation->setEnabled(true);

    //////////////////////////////////////////////////////////////////////////////////////
    /// \brief newPosition - CURRENT REPRESENTATION
    /////////////////////////////////////////////////////////////////////////////////////

    vtkRenderer * renderer = this->defaultRenderer;
    vtkCamera *camera = renderer->GetActiveCamera();

    Vector newPosition = hand.translation(controller_->frame(1));
    Vector newTranslation =hand.translation(controller_->frame(1));


    vtkMatrix4x4 * cameraMatrix = camera->GetViewTransformMatrix();


    //// FOR BEGINNERES
    /// Now since we are grabbing the CameraMatrix.. we have to cater for the fact that the Camera may have been
    /// rotated. So we have to Multply the Leap Translation Vector by the rotated camera to get the New Translaiton Vector
    /// to apply to the CameraMatrix. By ysing the old fashioned Xt, Yt, Zt * Matrix COLUMNS

    newTranslation.x =   (cameraMatrix->GetElement(0,0) * newPosition.x) +
            (cameraMatrix->GetElement(1,0) * newPosition.y) +
            (cameraMatrix->GetElement(2,0) * newPosition.z)  ;

    newTranslation.y =   (cameraMatrix->GetElement(0,1) * newPosition.x) +
            (cameraMatrix->GetElement(1,1) * newPosition.y) +
            (cameraMatrix->GetElement(2,1) * newPosition.z)    ;

    newTranslation.z =   (cameraMatrix->GetElement(0,2) * newPosition.x) +
            (cameraMatrix->GetElement(1,2) * newPosition.y) +
            (cameraMatrix->GetElement(2,2) * newPosition.z)    ;


    vtkTransform* handMove = vtkTransform::New();

    handMove->Identity();

    handMove->Translate(-newTranslation.x, - newTranslation.y, -newTranslation.z);

    camera->ApplyTransform(handMove);

    /// ADDED INTERACTION CAPTURE
    ///
//                double* position;

//                position = camera->GetPosition();

//                ui->line_PosX->setText(QString::number(position[0], 'f', 0));
//                ui->line_PosY->setText(QString::number(position[1], 'f', 0));
//                ui->line_PosZ->setText(QString::number(position[2], 'f', 0));

    updateCameraPosition();
}

void MainWindow::leapRotateUpdate(Frame frame, bool &rotateMovement, Hand hand)
{
    Matrix newRotation = hand.rotationMatrix(controller_->frame(3));



    //std::cout << newRotation.toString() << endl;

    vtkRenderer * renderer = this->defaultRenderer;
    vtkCamera *camera = renderer->GetActiveCamera();

    double * oldPosition = camera->GetPosition();
    double * oldViewUp = camera->GetViewUp();
    double * oldFocalPoint = camera->GetFocalPoint();
    double * oldOrientation = camera->GetOrientation();


    double position[3] = {oldPosition[0] , oldPosition[1], oldPosition[2] };
    double viewUp[3] = {oldViewUp[0] , oldViewUp[1], oldViewUp[2] };
    double focalPoint[3] = {oldFocalPoint[0] , oldFocalPoint[1], oldFocalPoint[2] };
    double oriental[3] = {oldOrientation[0] , oldOrientation[1], oldOrientation[2] };

    ///camera->SetFocalPoint(cameraFocalPoint);
    ///


    ///vtkMatrix4x4* cameraMatrix = camera->GetModelTransformMatrix();

    vtkMatrix4x4* cameraMatrix = camera->GetModelTransformMatrix();

    vtkMatrix4x4* rotationMatrix = vtkMatrix4x4::New();

    rotationMatrix->Identity();
    int index = 0;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            rotationMatrix->SetElement(i,j,newRotation.toArray4x4().m_array[index] );
            index++;
        }
    }

//    std::cout << "----------------------------------" << endl;
//    for (int i = 0; i < 4; i++)
//    {
//        std::cout << "[ " ;
//        for (int j = 0; j < 4; j++)
//        {
//            rotationMatrix->SetElement(i,j,newRotation.toArray4x4().m_array[index]);
//            std::cout << newRotation.toArray4x4().m_array[index] << ", " ;
//            index++;
//        }
//        std::cout << "]" << endl;
//    }


    vtkTransform* transform = vtkTransform::New();

    transform ->Identity();

    transform->SetMatrix(cameraMatrix);

    transform->Translate( cameraFocalPoint);

    transform->Concatenate(rotationMatrix);

    transform->Translate(-cameraFocalPoint[0], -cameraFocalPoint[1], -cameraFocalPoint[2]);

    camera->ApplyTransform(transform);





//                double * newFP = transform->TransformDoublePoint(cameraFocalPoint);

//                double newTFP[3] = {newFP[0] , newFP[1], newFP[2] };

//                oldPosition = camera->GetPosition();
//                oldViewUp = camera->GetViewUp();
//                oldFocalPoint = camera->GetFocalPoint();
//                oldOrientation = camera->GetOrientation();



//                std::cout << std::fixed << std::setprecision(2) ;
//                std::cout <<
//                             "P:" << oldPosition[0] << ", " << oldPosition[1] << "," << oldPosition[2] << "\t" <<
//                             "V:" << oldViewUp[0] << ", " << oldViewUp[1] << "," << oldViewUp[2] << "\t" <<
//                             "FP:" << oldFocalPoint[0] << ", " << oldFocalPoint[1] << "," << oldFocalPoint[2] << "\t" <<



//                             endl;



    ///camera->SetFocalPoint(cameraFocalPoint);
    ///

    //camera->SetFocalPoint(newFP);
    //camera->SetPosition(transform->TransformDoublePoint(oldPosition));
    //camera->SetViewUp(transform->TransformDoublePoint(oldViewUp));
    //camera->SetViewUp(viewUp);

    /// ADDED INTERACTION CAPTURE
    double* orientation;

    orientation  = camera->GetOrientation();

    ui->line_OrientX->setText(QString::number(orientation[0], 'f', 0));
    ui->line_OrientY->setText(QString::number(orientation[1], 'f', 0));
    ui->line_OrientZ->setText(QString::number(orientation[2], 'f', 0));

    this->ui->buttonTransfRotation->setEnabled(true);

    //renderer->ResetCameraClippingRange();
    if(this->userTestRunning())
    {
        leapHand1Move = true;
        leapHand1FrameBuffer ++;
        rotateMovement = true;
    }
}

void MainWindow::leapScaleUpdate(Frame frame, bool &scaleMovement, Hand hand)
{

    vtkRenderer * renderer = this->defaultRenderer;
    vtkCamera *camera = renderer->GetActiveCamera();

    //float adjustmentFactor = 0.5;
    //float scaleFactor = frame.scaleFactor(controller_->frame(1));
    float scaleFactor = hand.scaleFactor(controller_->frame(2));



    if (camera->GetParallelProjection())
    {
        camera->SetParallelProjection(camera->GetParallelScale() / scaleFactor);
        //     qDebug() << "ScaleFactor Parallel = " << scaleFactor << endl;
    }
    else
    {
        camera->Dolly(scaleFactor);
        renderer->ResetCameraClippingRange();
        //     qDebug() << "ScaleFactor Normal = " << scaleFactor << endl;
    }

    double value ;

    value = this->defaultCameraDistance /  camera->GetDistance();

    ui->line_Scale->setText(QString::number(value, 'f', 2));
    ui->buttonTransfScaling->setEnabled(true);

    if(this->userTestRunning())
    {
        leapHand1Move = true;
        leapHand1FrameBuffer ++;
        scaleMovement = true;
    }
}

void MainWindow::leapDiagnosticUpdate(Frame frame, Hand hand)
{
    if (hand.translationProbability(controller_->frame(1)) > 0.6)
    {
        Vector handPos = hand.palmPosition();
        //Pointable frontFinger = controller_->frame(1).fingers().frontmost();

        double sensitivity = 0.01;
        double offSetX = 0;
        double offSetY = 2.5;
        double offSetZ = 1;

        double handPosPoint[3] = {handPos.x * sensitivity - offSetX,
                                  handPos.y * sensitivity - offSetY,
                                  handPos.z * sensitivity - offSetZ
                                 };

        this->leapMarkerWidget->leapDbgPointWidget->SetPosition(handPosPoint);//handPos.

        this->leapMarkerWidget->leapDbgPointWidget->GetProperty()->SetColor(1.0, 1.0, 0.0);
    }


    /////////////////////////////////////////////////////////////////////////////
    /// \brief newNormal
    ///

    if (hand.rotationProbability(controller_->frame(1)) > 0.6)
    {

        Vector newNormal = hand.palmNormal();
        double scale = 1.1;

        double oldNormal[3] ;
        this->leapMarkerWidget->leapDbgPlaneWidget->GetNormal(oldNormal);
        double newNormalD [3];
        double theta, rotVector[3];

        double *point1 = this->leapMarkerWidget->leapDbgPlaneWidget->GetPoint1();
        double *origin = this->leapMarkerWidget->leapDbgPlaneWidget->GetOrigin();
        //double *center = leapDbgPlaneWidget->GetCenter();


        newNormalD[0] = newNormal.x;
        newNormalD[1] = newNormal.y;
        newNormalD[2] = newNormal.z;

        this->leapMarkerWidget->leapDbgPlaneWidget->SetNormal(newNormal.x, newNormal.y, newNormal.z);
        this->leapMarkerWidget->leapDbgPlaneWidget->UpdatePlacement();


        ///Compute the rotation vector using a transformation matrix
        ///Note that is fnromals are aparelle then te rotation is either 0 or 180 Degrees

        double dp = vtkMath::Dot(oldNormal, newNormalD);
        if (dp >= 1.0)
        {
            return;    ///zero rotation
        }

        else if (dp <= -1.0)
        {
            theta = 180.0;
            rotVector[0] = point1[0] - origin[0];
            rotVector[1] = point1[1] - origin[1];
            rotVector[2] = point1[2] - origin[2];
        }
        else
        {
            vtkMath::Cross(oldNormal, newNormalD,rotVector);
            theta = vtkMath::DegreesFromRadians(acos(dp));


        }

        this->leapMarkerWidget->leapDbgArrowActor->RotateWXYZ(theta, rotVector[0], rotVector[1], rotVector[2]);
        this->leapMarkerWidget->leapDbgSphereActor->RotateWXYZ(theta, rotVector[0], rotVector[1], rotVector[2]);
        this->leapMarkerWidget->leapDbgSphereActor->GetProperty()->SetColor(0.0, 1.0, 0.0 );
    }

    //////////////////////////////////////////////////////////////////////////
    /// \brief Pinch Scaling
    //////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    ///
    ///
    /// Compute the Scale Factor using the leap motion factor

    /// The following code checks to see if the sensor has regained focus.
    /// if so, we set the global_CameraPosition to the default value
    /// Effectively functioning as a reset value.
    /// We also have a skip value to true to not invert the slider.

    if (hand.scaleProbability(controller_->frame(1)) > 0.6)
    {
        if (abs(controller_->frame(1).id() - this->leapMarkerWidget->global_ScaleFactorID) > 15 )
        {
            this->leapMarkerWidget->global_CameraPosition = static_cast<vtkSliderRepresentation3D*>
                    (this->leapMarkerWidget->leapDbgSliderWidget->GetRepresentation())->GetValue();
            //                    std::cout << "Return focus" << endl;
        }

        this->leapMarkerWidget->global_ScaleFactorID = frame.id();       //Current Frame

        float scaleFactor = frame.hands()[0].scaleFactor(controller_->frame(2));

        double oldPosition = this->leapMarkerWidget->global_CameraPosition;

        this->leapMarkerWidget->global_CameraPosition = oldPosition * (scaleFactor) ;

        double newPosition = this->leapMarkerWidget->global_CameraPosition;


        /// We add color chromatic scale to the Slider Widget Propoert to highligh strength

        //                    double colourRange = (newPosition /  this->leapMarkerWidget->scaling_Max) ;

        //                    if (colourRange < 0) colourRange = 0;
        //                    else
        //                        if(colourRange > 1) colourRange = 1;

        //                    std::cout << "Scale Factor: " << scaleFactor
        //                              << "\tPosition: " << newPosition
        //                              << "\tColourRange: " << colourRange << endl;

        if (scaleFactor > 1.0000001)            /// EXPANDING .... ColourRange Getting BIGGER - Blue Adjustment
        {
            /// We add color chromatic scale to the Slider Widget Propoert to highligh strength
            ///
            double colourRange = (newPosition /  this->leapMarkerWidget->scaling_Max) ;
            if(colourRange > 1) colourRange = 1;

            static_cast<vtkSliderRepresentation3D*>
                    (this->leapMarkerWidget->leapDbgSliderWidget->GetRepresentation())->SetValue(newPosition );
            static_cast<vtkSliderRepresentation3D*>
                    (this->leapMarkerWidget->leapDbgSliderWidget->GetRepresentation())->
                    //GetTubeProperty()->SetColor(colourRange,colourRange,1);
                    GetTubeProperty()->SetColor(1.2-colourRange,1.2-colourRange,1);
        }
        else                                           /// SCHINKING.... ColourRange Getting SMALLER -- Red Adjustment
        {
            /// We add color chromatic scale to the Slider Widget Propoert to highligh strength
            ///
            double colourRange = (newPosition /  this->leapMarkerWidget->scaling_Start) ;
            if(colourRange < 0) colourRange = 0;

            static_cast<vtkSliderRepresentation3D*>
                    (this->leapMarkerWidget->leapDbgSliderWidget->GetRepresentation())->SetValue(newPosition );
            static_cast<vtkSliderRepresentation3D*>
                    (this->leapMarkerWidget->leapDbgSliderWidget->GetRepresentation())->
                    //GetTubeProperty()->SetColor(1,1-colourRange,1-colourRange);
                    GetTubeProperty()->SetColor(1, colourRange * 0.9, colourRange * 0.9);

            //                                std::cout << "Scale Factor: " << scaleFactor
            //                                          << "\tPosition: " << newPosition
            //                                          << "\tRGB(1,: " << colourRange * 0.8
            //                                          << "," << colourRange * 0.8 << endl;
        }
    }
}

void MainWindow::leapHandWidgetUpdate(Frame frame, Hand hand)
{
    if (!frame.hands().isEmpty() && !frame.hands()[0].fingers().isEmpty())
    {
        //                    Get the first hand
        const Hand rightHandMoving = frame.hands().rightmost();
        //                    Get the 2nd hand
        const Hand leftHandMoving = frame.hands().leftmost();

        bool leftHandActive = true;
        bool rightHandActive = true;

        /// If we have a single hand, we must determine which 1 it is
        /// If not the Right hand, then reverse outcome
        /// Otherwise we swap hands accordingly
        if(frame.hands().count() == 1)
        {
            if (frame.hands().frontmost().isRight())
                leftHandActive = false;
            else
                rightHandActive = false;
        }

        /// The Leap MOTION Interaction Box
        InteractionBox leapBox = frame.interactionBox();


        if (rightHandActive)
        {
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    Right Finger Joints  TRACKING  /////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            Vector normalPos = leapBox.normalizePoint(rightHandMoving.palmPosition(),true);


            bool outsideBounds = ( (normalPos.x  == 0) || (normalPos.x == 1)) ||
                    ( (normalPos.y  == 0)  || (normalPos.y == 1)) ||
                    ( (normalPos.z  == 0)  || (normalPos.z == 1)) ;


            leapHandWidget->translateHand(rightHand,rightHandMoving,outsideBounds, 0.01);
        }/// if(hand.isLeft())

        if (leftHandActive)
        {
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    Right Finger Joints  TRACKING  /////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            Vector normalPos = leapBox.normalizePoint(leftHandMoving.palmPosition(),true);


            bool outsideBounds = ( (normalPos.x  == 0) || (normalPos.x == 1)) ||
                    ( (normalPos.y  == 0)  || (normalPos.y == 1)) ||
                    ( (normalPos.z  == 0)  || (normalPos.z == 1)) ;


            leapHandWidget->translateHand(leftHand,leftHandMoving,outsideBounds, 0.01);
        }   /// if(hand.isLeft())
    }   ///    if (!frame.hands().isEmpty()
}

void MainWindow::leapHandModelUpdate(Frame frame)
{
    if (!frame.hands().isEmpty() && !frame.hands()[0].fingers().isEmpty())
    {
        //                    Get the first hand
        const Hand rightHandMoving = frame.hands().rightmost();
        //                    Get the 2nd hand
        const Hand leftHandMoving = frame.hands().leftmost();

        bool leftHandActive = true;
        bool rightHandActive = true;

        /// If we have a single hand, we must determine which 1 it is
        /// If not the Right hand, then reverse outcome
        /// Otherwise we swap hands accordingly
        if(frame.hands().count() == 1)
        {
            if (frame.hands().frontmost().isRight())
                leftHandActive = false;
            else
                rightHandActive = false;
        }

        /// The Leap MOTION Interaction Box
        InteractionBox leapBox = frame.interactionBox();


        if (rightHandActive)
        {
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    Right Finger Joints  TRACKING  /////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            Vector normalPos = leapBox.normalizePoint(rightHandMoving.palmPosition(),true);


            bool outsideBounds = ( (normalPos.x  == 0) || (normalPos.x == 1)) ||
                    ( (normalPos.y  == 0)  || (normalPos.y == 1)) ||
                    ( (normalPos.z  == 0)  || (normalPos.z == 1)) ;


            handRenderer->translateHand(rightHand,rightHandMoving,outsideBounds, 0.1);
        }/// if(hand.isLeft())

        if (leftHandActive)
        {
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    Right Finger Joints  TRACKING  /////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            Vector normalPos = leapBox.normalizePoint(leftHandMoving.palmPosition(),true);


            bool outsideBounds = ( (normalPos.x  == 0) || (normalPos.x == 1)) ||
                    ( (normalPos.y  == 0)  || (normalPos.y == 1)) ||
                    ( (normalPos.z  == 0)  || (normalPos.z == 1)) ;


            handRenderer->translateHand(leftHand,leftHandMoving,outsideBounds, 0.1);
        }   /// if(hand.isLeft())
    }   ///    if (!frame.hands().isEmpty()
}


void MainWindow::boxWidgetCallback()
{
    vector<double> cubeVector;
    vector<double> subVolVector;

    ///
    /// This Section Validates transform
    ///
    double oldBounds[6];
    double newBounds[6];
    double volBounds[6];
    bool transformPassed = true;

    global_Volume->GetBounds(volBounds);

    vtkLinearTransform* oldCubeTransform = boxWidget_->GetProp3D()->GetUserTransform();
    boxWidget_->GetProp3D()->GetBounds(oldBounds);
    //boxWidget_->Get


    vtkTransform* newCubeTransform = vtkTransform::New();
    boxWidget_->GetTransform(newCubeTransform);

    boxWidget_->GetProp3D()->SetUserTransform(newCubeTransform);

    //get the cube bounds


    boxWidget_->GetProp3D()->GetBounds(newBounds);

    /// This is where we validate the Bounds of the new cube vs the Global Volume
    ///We use a switch statement to check whether the new cube is Greater than the Vol lOW Limits
    ///the other switch statement is used to show the new cube is Smaller than the Vol MAX limits
    for (unsigned int i=0; i < 6; i++)
    {
        switch(i)
        {
        case 0:
        case 2:
        case 4:
            if (newBounds[i] < volBounds[i])
                transformPassed = false;
            break;
        case 1:
        case 3:
        case 5:
            if (newBounds[i] > volBounds[i])
                transformPassed = false;
            break;
        }
    }

    if (!transformPassed)
    {
        boxWidget_->GetProp3D()->SetUserTransform(oldCubeTransform);
        boxWidget_->GetOutlineProperty()->SetColor(1,0,0);
        for (unsigned int i=0; i < 6; i++)
        {
            global_subVolBounds_[i] = oldBounds[i];
        }
    }
    else
    {
        for (unsigned int i=0; i < 6; i++)
        {
            //cubeVector.push_back(newBounds[i]);
            global_subVolBounds_[i] = newBounds[i];
            boxWidget_->GetOutlineProperty()->SetColor(1,1,1);
        }
    }

    //    PrintBounds("subVol", global_subVolBounds_);

    /// We set the SubVolume Extract Button Enabled to the Passing of the Transform
    this->ui->buttonSubVolExport->setEnabled(transformPassed);


    this->ui->lineSubVolXMin->setText(QString::number(global_subVolBounds_[0], 'f', 2));
    this->ui->lineSubVolXMax->setText(QString::number(global_subVolBounds_[1], 'f', 2));
    this->ui->lineSubVolYMin->setText(QString::number(global_subVolBounds_[2], 'f', 2));
    this->ui->lineSubVolYMax->setText(QString::number(global_subVolBounds_[3], 'f', 2));
    this->ui->lineSubVolZMin->setText(QString::number(global_subVolBounds_[4], 'f', 2));
    this->ui->lineSubVolZMax->setText(QString::number(global_subVolBounds_[5], 'f', 2));



}


//void MainWindow::on_LogButtonClear_clicked()
//{
////    int tab = this->ui->tabLogWidget->currentIndex();
////    switch (tab)
////    {
////        case 0: this->ui->plainTextEdit_Info->clear();
////        break;

////        case 1: this->ui->plainTextEdit_Leap->clear();
////        break;
////    }
//}

void MainWindow::on_actionStats_triggered()
{

    vtkRenderer* renderer = this->defaultRenderer;
    vtkCamera * camera = renderer->GetActiveCamera();

    double * pointDouble;
    QString message = "";

    this->ui->plainTextEdit_Leap->insertPlainText("--------------------------------------\n");

    pointDouble = camera->GetFocalPoint();
    message += QString("camera focalPoint: (%1, %2, %3)\n").arg(pointDouble[0]).arg(pointDouble[1]).arg(pointDouble[2]);

    pointDouble = camera->GetViewUp();
    message += QString("Camera ViewUp: (%1, %2, %3)\n").arg(pointDouble[0]).arg(pointDouble[1]).arg(pointDouble[2]);

    pointDouble = camera->GetPosition();
    message += QString("Camera Position: (%1, %2, %3)\n").arg(pointDouble[0]).arg(pointDouble[1]).arg(pointDouble[2]);

    pointDouble = camera->GetDirectionOfProjection();
    message += QString("Camera DirOfProj: (%1, %2, %3)\n").arg(pointDouble[0]).arg(pointDouble[1]).arg(pointDouble[2]);

    //    message += QString("Camera ViewAngle: (%1)\n").arg(camera->GetViewAngle());

    //    pointInt = renderer->GetOrigin();
    //    message += QString("Renderer Origin: (%1, %2, %3)\n").arg(pointInt[0]).arg(pointInt[1]).arg(pointInt[2]);


    this->ui->plainTextEdit_Leap->insertPlainText(message);

    this->ui->plainTextEdit_Leap->insertPlainText("--------------------------------------\n\n");
}

void MainWindow::on_actionLeapHandFullWindow_triggered()
{

}

void MainWindow::touchInteractionEvent()
{

//    vtkRenderer* renderer = this->defaultRenderer;
//    vtkCamera * camera = renderer->GetActiveCamera();

//    double * position = camera->GetPosition();

//    ui->line_PosX->setText(QString::number(position[0], 'f', 0));
//    ui->line_PosY->setText(QString::number(position[1], 'f', 0));
//    ui->line_PosZ->setText(QString::number(position[2], 'f', 0));
    updateCameraPosition();




    //    double value ;

    //    value = this->defaultCameraDistance /  this->ui->qvtkWidgetLeft->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera()->GetDistance();

    //this->ui->line_Scale->setText(QString::number(value, 'f', 2));
}

void MainWindow::touchBeginSubVol()
{
    on_actionReset_Camera_triggered();

    //this->ui->qvtkWidgetLeft->setGesturesActive(false);


    double bounds[6];

    global_Reader->GetOutput()->GetBounds(bounds);


    double offset = 0.3;
    double p1XOffset = bounds[1] * offset;
    double p1YOffset = bounds[3] * offset;
    double p1ZOffset = bounds[5] * offset;

    offset = 0.6;
    double p2xOffset = bounds[1] * offset;
    double p2yOffset = bounds[3] * offset;
    double p2zOffset = bounds[5] * offset;


    double pt1[3] = {p1XOffset, p1YOffset, p1ZOffset};
    double pt2[3] = {p2xOffset, p2yOffset, p2zOffset};


    ////////////////////////////////////////////
    ////////////////////////////////////////////
    /// POINT 1
    {
        /// The plane widget is used probe the dataset.
        vtkSmartPointer<vtkPolyData> point =
                vtkSmartPointer<vtkPolyData>::New();

        vtkSmartPointer<vtkProbeFilter> probe =
                vtkSmartPointer<vtkProbeFilter>::New();
        probe->SetInput(point);
        probe->SetSource(global_Reader->GetOutput());

        /// create glyph
        ///
        ///
        vtkSmartPointer<vtkSphereSource> pointMarker =
                vtkSmartPointer<vtkSphereSource>::New();
        pointMarker->SetRadius(0.15);

        vtkSmartPointer<vtkGlyph3D> glyph =
                vtkSmartPointer<vtkGlyph3D>::New();
        glyph->SetInputConnection(probe->GetOutputPort());
        glyph->SetSourceConnection(pointMarker->GetOutputPort());
        glyph->SetVectorModeToUseVector();
        glyph->SetScaleModeToDataScalingOff();
        glyph->SetScaleFactor(global_Volume->GetLength() * 0.1);

        vtkSmartPointer<vtkPolyDataMapper> glyphMapper =
                vtkSmartPointer<vtkPolyDataMapper>::New();
        glyphMapper->SetInputConnection(glyph->GetOutputPort());

        vtkSmartPointer<vtkActor> glyphActor =
                vtkSmartPointer<vtkActor>::New();
        glyphActor->SetMapper(glyphMapper);
        glyphActor->VisibilityOn();

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
                this->defaultRenderer;

        // The SetInteractor method is how 3D widgets are associated with the render
        // window interactor. Internally, SetInteractor sets up a bunch of callbacks
        // using the Command/Observer mechanism (AddObserver()).
        vtkSmartPointer<vtkmyPWCallback> myCallback =
                vtkSmartPointer<vtkmyPWCallback>::New();
        myCallback->PolyData = point;
        myCallback->Actor = glyphActor;
        myCallback->TextActor = textActor;

        pointWidget1_ = vtkPointWidget::New();
        pointWidget1_->SetInteractor(this->ui->qvtkWidgetLeft->GetInteractor());
        pointWidget1_->SetInput(global_Reader->GetOutput());
        pointWidget1_->AllOff();
        pointWidget1_->PlaceWidget();
        pointWidget1_->GetPolyData(point);
        pointWidget1_->EnabledOff();
        //vtkEventConnector->Connect(pointWidget_, vtkCommand::InteractionEvent, this, SLOT(pointWidgetCallBack()));
        pointWidget1_->AddObserver(vtkCommand::InteractionEvent  ,myCallback);

        renderer->AddActor(glyphActor);
        renderer->AddActor2D(textActor);

        pointWidget1_->EnabledOn();
        pointWidget1_->GetInteractor()->RemoveAllObservers();


    }
    pointWidget1_->SetPosition(pt1);
    //pointWidget1_->GetProperty()->SetColor(0.8900, 0.8100, 0.3400);
    pointWidget1_->GetProperty()->SetColor(0.8900, 0.8100, 0.3400);

    pointWidget1_->InvokeEvent(vtkCommand::InteractionEvent);


    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    /// THIS IS THE 2nd POINTER

    {
        /// The plane widget is used probe the dataset.
        vtkSmartPointer<vtkPolyData> point =
                vtkSmartPointer<vtkPolyData>::New();

        vtkSmartPointer<vtkProbeFilter> probe =
                vtkSmartPointer<vtkProbeFilter>::New();
        probe->SetInput(point);
        probe->SetSource(global_Reader->GetOutput());

        /// create glyph
        ///
        ///
        vtkSmartPointer<vtkSphereSource> pointMarker =
                vtkSmartPointer<vtkSphereSource>::New();
        pointMarker->SetRadius(0.15);

        vtkSmartPointer<vtkGlyph3D> glyph =
                vtkSmartPointer<vtkGlyph3D>::New();
        glyph->SetInputConnection(probe->GetOutputPort());
        glyph->SetSourceConnection(pointMarker->GetOutputPort());
        glyph->SetVectorModeToUseVector();
        glyph->SetScaleModeToDataScalingOff();
        glyph->SetScaleFactor(global_Volume->GetLength() * 0.1);

        vtkSmartPointer<vtkPolyDataMapper> glyphMapper =
                vtkSmartPointer<vtkPolyDataMapper>::New();
        glyphMapper->SetInputConnection(glyph->GetOutputPort());

        vtkSmartPointer<vtkActor> glyphActor =
                vtkSmartPointer<vtkActor>::New();
        glyphActor->SetMapper(glyphMapper);
        glyphActor->VisibilityOn();

        /// A Text Widget showing the point location
        ///
        ///
        vtkSmartPointer<vtkTextActor> textActor =
                vtkSmartPointer<vtkTextActor>::New();
        textActor->GetTextProperty()->SetFontSize(16);
        textActor->SetPosition(400, 20);
        textActor->SetInput("cursor:");
        textActor->GetTextProperty()->SetColor(0.3400, 0.8100, 0.8900);

        /// Extract the RenderWindow, Renderer and  add both Actors
        ///
        ///

        vtkSmartPointer<vtkRenderer> renderer =
                this->defaultRenderer;
        // The SetInteractor method is how 3D widgets are associated with the render
        // window interactor. Internally, SetInteractor sets up a bunch of callbacks
        // using the Command/Observer mechanism (AddObserver()).
        vtkSmartPointer<vtkmyPWCallback> myCallback =
                vtkSmartPointer<vtkmyPWCallback>::New();
        myCallback->PolyData = point;
        myCallback->Actor = glyphActor;
        myCallback->TextActor = textActor;

        pointWidget2_ = vtkPointWidget::New();
        pointWidget2_->SetInteractor(this->ui->qvtkWidgetLeft->GetInteractor());
        pointWidget2_->SetInput(global_Reader->GetOutput());
        pointWidget2_->AllOff();
        pointWidget2_->PlaceWidget();
        pointWidget2_->GetPolyData(point);
        pointWidget2_->EnabledOff();
        //vtkEventConnector->Connect(pointWidget_, vtkCommand::InteractionEvent, this, SLOT(pointWidgetCallBack()));
        pointWidget2_->AddObserver(vtkCommand::InteractionEvent  ,myCallback);

        renderer->AddActor(glyphActor);
        renderer->AddActor2D(textActor);

        pointWidget2_->EnabledOn();
        pointWidget2_->GetInteractor()->RemoveAllObservers();

        pointWidget2_->GetProperty()->SetColor(0.3400, 0.8100, 0.8900);
    }

    loadSubVolume();

    pointWidget2_->SetPosition(pt2);

    vtkEventQtSlotConnect* vtkEventConnector =
            vtkEventQtSlotConnect::New();

    vtkEventConnector->Connect(pointWidget1_, vtkCommand::InteractionEvent, this, SLOT(touchUpdateSubVol()));
    vtkEventConnector->Connect(pointWidget2_, vtkCommand::InteractionEvent, this, SLOT(touchUpdateSubVol()));

    pointWidget1_->InvokeEvent(vtkCommand::InteractionEvent);
    pointWidget2_->InvokeEvent(vtkCommand::InteractionEvent);

    if (userTestRunning())              ///USERTEST
    {
        vtkEventQtSlotConnect * ptWidget1Connector = vtkEventQtSlotConnect::New();
        ptWidget1Connector->Connect(this->pointWidget1_,vtkCommand::StartInteractionEvent,userTestDlg,SLOT(incSubVolPointRight()));

        vtkEventQtSlotConnect * ptWidget2Connector = vtkEventQtSlotConnect::New();
        ptWidget2Connector->Connect(this->pointWidget2_,vtkCommand::StartInteractionEvent,userTestDlg,SLOT(incSubVolPointLeft()));
    }
}

void MainWindow::touchUpdateSubVol()
{
    double* point1;
    double* point2;

    point1 = this->pointWidget1_->GetPosition();
    point2 = this->pointWidget2_->GetPosition();

    ///
    /// 1. Determine the MIN and MAX values for each axis
    /// 2. Create a subVolume bounding box using the two points
    /// 3. Update the current volume with the new specs
    ///

    double minX;
    double maxX;
    double minY;
    double maxY;
    double minZ;
    double maxZ;


    //determine X Min/Max
    if (point1[0] < point2[0])
    {
        minX = point1[0];
        maxX = point2[0];
    }
    else
    {
        minX = point2[0];
        maxX = point1[0];
    }
    ////////////////////////////////////////////////////
    //determine Y Min/Max
    if (point1[1] < point2[1])
    {
        minY = point1[1];
        maxY = point2[1];
    }
    else
    {
        minY = point2[1];
        maxY = point1[1];
    }
    ///////////////////////////////////////////////////
    //determine Z Min/Max
    if (point1[2] < point2[2])
    {
        minZ = point1[2];
        maxZ = point2[2];
    }
    else
    {
        minZ = point2[2];
        maxZ = point1[2];
    }

    global_subVolume->SetBounds(minX, maxX, minY,maxY, minZ, maxZ);

    this->ui->lineSubVolXMin->setText(QString::number(minX, 'f', 2));
    this->ui->lineSubVolXMax->setText(QString::number(maxX, 'f', 2));
    this->ui->lineSubVolYMin->setText(QString::number(minY, 'f', 2));
    this->ui->lineSubVolYMax->setText(QString::number(maxY, 'f', 2));
    this->ui->lineSubVolZMin->setText(QString::number(minZ, 'f', 2));
    this->ui->lineSubVolZMax->setText(QString::number(maxZ, 'f', 2));
}

void MainWindow::touchTranslatePressed()
{
    this->ui->buttonTransfTranslation->setEnabled(true);
}

void MainWindow::touchTranslateReleased()
{
    this->ui->buttonTransfTranslation->setEnabled(false);
}

void MainWindow::touchRotationPressed()
{
    this->ui->buttonTransfRotation->setEnabled(true);
}

void MainWindow::touchRotationReleased()
{
    this->ui->buttonTransfRotation->setEnabled(false);
}

void MainWindow::touchTransformsOn(bool status)
{    
    this->ui->qvtkWidgetLeft->setTransformsOn(status);    
}

void MainWindow::touchTransformationToggle()
{
    this->ui->buttonTransformActive->toggle();

    //std::cout << "\t Signal Recied \t" << endl;



    if (this->systemTab == SubVolume)
    {
        bool status  = this->ui->qvtkWidgetLeft->GetTransformsOn();

        this->ui->qvtkWidgetLeft->SetSubVolumeOn(!status);

        if (!status)
        {
            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Pressed()), this, SLOT(touchFinger1SubVol()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Pressed()), this, SLOT(touchFinger2SubVol()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(rightClkSubVol()), this, SLOT(touchSubVolTranslate()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Released()), this, SLOT(touchFingerReleased()));
        }
        if (status)
        {
            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Pressed()), this, SLOT(touchFinger1SubVol()));
            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Pressed()), this, SLOT(touchFinger2SubVol()));
            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(rightClkSubVol()), this, SLOT(touchSubVolTranslate()));
            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Released()), this, SLOT(touchFingerReleased()));
        }
    }

    if (this->systemTab == SliceArb)
    {
        bool status  = this->ui->qvtkWidgetLeft->GetTransformsOn();

        this->ui->qvtkWidgetLeft->SetArbSliceOn(!status);



        //customArbPlaneWidget->Print(std::cout);
        ///customArbPlaneWidget->GetInteractor()->Print(std::cout);

        if (!status)
        {
          //  customArbPlaneWidget->GetInteractor()->RemoveAllObservers();

            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Pressed()), this, SLOT(touchFinger1ArbSlicePressed()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Moving()), this, SLOT(touchFinger1ArbSliceMoving()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Released()), this, SLOT(touchFinger1ArbSliceReleased()));

            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Pressed()), this, SLOT(touchFinger2ArbSlicePressed()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Moving()), this, SLOT(touchFinger2ArbSliceMoving()));
            connect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Released()), this, SLOT(touchFInger2ArbSliceReleased()));
        }
        if (status)
        {
            //customArbPlaneWidget->GetInteractor()->R
            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Pressed()), this, SLOT(touchFinger1ArbSlicePressed()));
            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Moving()), this, SLOT(touchFinger1ArbSliceMoving()));
            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger1Released()), this, SLOT(touchFinger1ArbSliceReleased()));

            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Pressed()), this, SLOT(touchFinger2ArbSlicePressed()));
            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Moving()), this, SLOT(touchFinger2ArbSliceMoving()));
            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Released()), this, SLOT(touchFInger2ArbSliceReleased()));
            //            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(finger2Pressed()), this, SLOT(touchFinger2SubVol()));

//            disconnect(this->ui->qvtkWidgetLeft, SIGNAL(rightClkSubVol()), this, SLOT(touchSubVolTranslate()));


//            vtkRenderWindowInteractor *i = customArbPlaneWidget->GetInteractor();
//                i->AddObserver(vtkCommand::MouseMoveEvent, customArbPlaneWidget->,
//                               0.5);
//                i->AddObserver(vtkCommand::LeftButtonPressEvent,
//                               this->EventCallbackCommand, this->Priority);
//                i->AddObserver(vtkCommand::LeftButtonReleaseEvent,
//                               this->EventCallbackCommand, this->Priority);
//                i->AddObserver(vtkCommand::MiddleButtonPressEvent,
//                               this->EventCallbackCommand, this->Priority);
//                i->AddObserver(vtkCommand::MiddleButtonReleaseEvent,
//                               this->EventCallbackCommand, this->Priority);
//                i->AddObserver(vtkCommand::RightButtonPressEvent,
//                               this->EventCallbackCommand, this->Priority);
//                i->AddObserver(vtkCommand::RightButtonReleaseEvent,
//                               this->EventCallbackCommand, this->Priority);
        }
    }
}

void MainWindow::touchFinger1SubVol()
{

    /// FingerActor Position Information
    double * actorPos2D = this->ui->qvtkWidgetLeft->fingerActor1->GetPositionCoordinate()->GetValue();
    vtkCoordinate *coordinate = this->ui->qvtkWidgetLeft->fingerActor1->GetActualPositionCoordinate();
    double * actorPosWorld =  coordinate->GetComputedWorldValue(this->ui->qvtkWidgetLeft->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

    /// Get PointWidget 1 2D Position
    double * pointW1 = pointWidget1_->GetPosition();
    double pntW1P2D[3] = {0,0,0};
    this->pointWidget1_->ComputeWorldToDisplay(defaultRenderer, pointW1[0], pointW1[1], pointW1[2], pntW1P2D);

    /// Get PointWidget 2 2D Position
    double * pointW2 = pointWidget2_->GetPosition();
    double pntW2P2D[3] = {0,0,0};
    this->pointWidget2_->ComputeWorldToDisplay(defaultRenderer, pointW2[0], pointW2[1], pointW2[2], pntW2P2D);

    QPointF finger(actorPos2D[0], actorPos2D[1]);

    QPointF widget1(pntW1P2D[0], pntW1P2D[1]);
    QPointF distance1 =  finger - widget1;

    QPointF widget2(pntW2P2D[0], pntW2P2D[1]);
    QPointF distance2 =  finger - widget2;

    int distPos1 = distance1.manhattanLength();
    int distPos2 = distance2.manhattanLength();

    int threshold = touchWidgetRange_;

    /// 1. Compute the new Position by Getting the PointWidget Focal Point
    /// 2. Getting the Finger Actor 2D position into 3D World Position using FocalPoint Z axis
    /// 3. Set the Position of the PointWidget to the Finger Actor 3D World Point Position
    /// 4. PointWidget Update

    //// --------MOVE PointWidget 1 with Finger 1--------"
    if ((distPos1 <= distPos2) && (distPos1 < threshold))
    {

        if (userTestRunning() && !pointWidget1Active && !pointWidget2Active)
        {
            userTestDlg->incSubVolPoint1();
        }

        if (!pointWidget1Active && !pointWidget2Active )
        {
            std::cout << "SubVolume TRIGGERED" << endl;
            pointWidget1Active = true;

        }


        double focalPoint[4], pickPoint[4];
        double z;

          pointWidget1_->ComputeWorldToDisplay(defaultRenderer, pointW1[0], pointW1[1], pointW1[2], focalPoint);
          z = focalPoint[2];

          pointWidget1_->ComputeDisplayToWorld(defaultRenderer, (actorPos2D[0]), actorPos2D[1], z,pickPoint);

          pointWidget1_->SetPosition(pickPoint[0], pickPoint[1], pickPoint[2]);

        /// Interact, if desired

        pointWidget1_->InvokeEvent(vtkCommand::InteractionEvent,NULL);
    }

    //// --------MOVE PointWidget 2 with Finger 1--------"
    if ((distPos2 <= distPos1) && (distPos2 < threshold))
    {     

        if (userTestRunning() && !pointWidget1Active && !pointWidget2Active)
        {
            userTestDlg->incSubVolPoint2();
        }


        if (!pointWidget2Active && !pointWidget1Active)
        {
            std::cout << "SubVolume TRIGGERED" << endl ;
            pointWidget2Active = true;
        }

        double focalPoint[4], pickPoint[4];
        double z;

          pointWidget2_->ComputeWorldToDisplay(defaultRenderer, pointW2[0], pointW2[1], pointW2[2], focalPoint);
          z = focalPoint[2];

          pointWidget2_->ComputeDisplayToWorld(defaultRenderer, (actorPos2D[0]), actorPos2D[1], z,pickPoint);

          pointWidget2_->SetPosition(pickPoint[0], pickPoint[1], pickPoint[2]);

        /// Interact, if desired

        pointWidget2_->InvokeEvent(vtkCommand::InteractionEvent,NULL);
    }
}

void MainWindow::touchFinger2SubVol()
{    

    /// FingerActor1 Position Information
    double * actor1Pos2D = this->ui->qvtkWidgetLeft->fingerActor1->GetPositionCoordinate()->GetValue();
    vtkCoordinate *coordinate1 = this->ui->qvtkWidgetLeft->fingerActor1->GetActualPositionCoordinate();
    double * actor1PosWorld =  coordinate1->GetComputedWorldValue(this->ui->qvtkWidgetLeft->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

    /// FingerActor2 Position Information
    double * actor2Pos2D = this->ui->qvtkWidgetLeft->fingerActor2->GetPositionCoordinate()->GetValue();
    vtkCoordinate *coordinate2 = this->ui->qvtkWidgetLeft->fingerActor2->GetActualPositionCoordinate();
    double * actor2PosWorld =  coordinate2->GetComputedWorldValue(this->ui->qvtkWidgetLeft->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

    /// Get PointWidget 1 2D Position
    double * pointW1 = pointWidget1_->GetPosition();
    double pntW1P2D[3] = {0,0,0};
    this->pointWidget1_->ComputeWorldToDisplay(defaultRenderer, pointW1[0], pointW1[1], pointW1[2], pntW1P2D);

    /// Get PointWidget 2 2D Position
    double * pointW2 = pointWidget2_->GetPosition();
    double pntW2P2D[3] = {0,0,0};
    this->pointWidget2_->ComputeWorldToDisplay(defaultRenderer, pointW2[0], pointW2[1], pointW2[2], pntW2P2D);

    QPointF finger1(actor1Pos2D[0], actor1Pos2D[1]);
    QPointF widget1(pntW1P2D[0], pntW1P2D[1]);

    QPointF finger2(actor2Pos2D[0], actor2Pos2D[1]);
    QPointF widget2(pntW2P2D[0], pntW2P2D[1]);

    /// Get Distance of Finger 1 to Widget1 && Widget 2
    QPointF distF1W1 =  finger1 - widget1;
    int dist1Pos1 = distF1W1.manhattanLength();
    QPointF distF1W2 =  finger1 - widget2;
    int dist1Pos2 = distF1W2.manhattanLength();


    QPointF distF2W1 =  finger2 - widget1;
    int dist2Pos1 = distF2W1.manhattanLength();
    QPointF distF2W2 =  finger2 - widget2;
    int dist2Pos2 = distF2W2.manhattanLength();

    int threshold = touchWidgetRange_;

    /////
    ////
    /// We check if point 1 is close to Any of the widgets...

    //// "--------MOVE PointWidget 1 with Finger 1--------"
    if ((dist1Pos1 <= dist1Pos2) && (dist1Pos1 < threshold))
    {        

        double focalPoint[4], pickPoint[4];
        double z;

          pointWidget1_->ComputeWorldToDisplay(defaultRenderer, pointW1[0], pointW1[1], pointW1[2], focalPoint);
          z = focalPoint[2];

          pointWidget1_->ComputeDisplayToWorld(defaultRenderer, (actor1Pos2D[0]), actor1Pos2D[1], z,pickPoint);

          pointWidget1_->SetPosition(pickPoint[0], pickPoint[1], pickPoint[2]);

        /// Interact, if desired

        pointWidget1_->InvokeEvent(vtkCommand::InteractionEvent,NULL);
    }

    //// "--------MOVE PointWidget 1 with Finger 2--------"
    if ((dist1Pos2 <= dist1Pos1) && (dist1Pos2 < threshold))
    {       

        double focalPoint[4], pickPoint[4];
        double z;

          pointWidget1_->ComputeWorldToDisplay(defaultRenderer, pointW1[0], pointW1[1], pointW1[2], focalPoint);
          z = focalPoint[2];

          pointWidget1_->ComputeDisplayToWorld(defaultRenderer, (actor2Pos2D[0]), actor2Pos2D[1], z,pickPoint);

          pointWidget1_->SetPosition(pickPoint[0], pickPoint[1], pickPoint[2]);

        /// Interact, if desired

        pointWidget1_->InvokeEvent(vtkCommand::InteractionEvent,NULL);
    }    

    //// "--------MOVE PointWidget 2 with Finger 1--------"
    if ((dist2Pos1 <= dist2Pos2) && (dist2Pos1 < threshold))
    {        

        double focalPoint[4], pickPoint[4];
        double z;

          pointWidget2_->ComputeWorldToDisplay(defaultRenderer, pointW2[0], pointW2[1], pointW2[2], focalPoint);
          z = focalPoint[2];

          pointWidget2_->ComputeDisplayToWorld(defaultRenderer, (actor1Pos2D[0]), actor1Pos2D[1], z,pickPoint);

          pointWidget2_->SetPosition(pickPoint[0], pickPoint[1], pickPoint[2]);

        /// Interact, if desired

        pointWidget2_->InvokeEvent(vtkCommand::InteractionEvent,NULL);
    }

    //// "--------MOVE PointWidget 2 with Finger 2--------"
    if ((dist2Pos2 <= dist2Pos1) && (dist2Pos2 < threshold))
    {        

        double focalPoint[4], pickPoint[4];
        double z;

          pointWidget2_->ComputeWorldToDisplay(defaultRenderer, pointW2[0], pointW2[1], pointW2[2], focalPoint);
          z = focalPoint[2];

          pointWidget2_->ComputeDisplayToWorld(defaultRenderer, (actor2Pos2D[0]), actor2Pos2D[1], z,pickPoint);

          pointWidget2_->SetPosition(pickPoint[0], pickPoint[1], pickPoint[2]);

        /// Interact, if desired

        pointWidget2_->InvokeEvent(vtkCommand::InteractionEvent,NULL);
    }
}

void MainWindow::touchFingerReleased()
{
    this->pointWidget1Active = false;
    this->pointWidget2Active = false;
}

void MainWindow::touchSubVolTranslate()
{

    /// FingerActor Position Information
    double * actorPos2D = this->ui->qvtkWidgetLeft->fingerActor1->GetPositionCoordinate()->GetValue();
    double * actorPos2D2 = this->ui->qvtkWidgetLeft->fingerActor1->GetPosition2Coordinate()->GetValue();

    vtkCoordinate *coordinate = this->ui->qvtkWidgetLeft->fingerActor1->GetActualPositionCoordinate();
    double *  actor1PosWorld =  coordinate->GetComputedWorldValue(this->ui->qvtkWidgetLeft->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

    /// Get PointWidget 1 2D Position
    double * pointW1 = pointWidget1_->GetPosition();
    double pntW1P2D[3] = {0,0,0};
    this->pointWidget1_->ComputeWorldToDisplay(defaultRenderer, pointW1[0], pointW1[1], pointW1[2], pntW1P2D);

    /// Get PointWidget 2 2D Position
    double * pointW2 = pointWidget2_->GetPosition();
    double pntW2P2D[3] = {0,0,0};
    this->pointWidget2_->ComputeWorldToDisplay(defaultRenderer, pointW2[0], pointW2[1], pointW2[2], pntW2P2D);

    QPointF finger(actorPos2D[0], actorPos2D[1]);

    QPointF widget1(pntW1P2D[0], pntW1P2D[1]);
    QPointF distance1 =  finger - widget1;

    QPointF widget2(pntW2P2D[0], pntW2P2D[1]);
    QPointF distance2 =  finger - widget2;

    int distPos1 = distance1.manhattanLength();
    int distPos2 = distance2.manhattanLength();

    bool validXmin = (finger.x() > min(widget1.x(),widget2.x()));
    bool validXmax = (finger.x() < max(widget1.x(),widget2.x()));
    bool validYmin = (finger.y() > min(widget1.y(),widget2.y()));
    bool validYmax = (finger.y() < max(widget1.y(),widget2.y()));


//    std::cout << "XMin: " << validXmin << "\t XMax: " << validXmax
//              << "YMin: " << validYmin << "\t YMax: " << validYmax
//              << endl;

//    if (validXmax && validXmin && validYmin && validYmax)
//    {
//        std::cout << "VALID TRANSLATE LOCATION \t[x,y]: " << finger.x() << ", " << finger.y() << endl;
//    }

    bool widget1Valid = false;
    bool widget2Valid = false;

    double pickPoint1[4], pickPoint2[4];

    if (validXmax && validXmin && validYmin && validYmax &&
            (distPos1 >touchWidgetRange_) && (distPos2 > touchWidgetRange_))
    {
        ///std::cout << "VALID TRANSLATE LOCATION \t[x,y]: " << finger.x() << ", " << finger.y() << endl;


        //    int threshold = 50;

        /// 1. Compute the new Position by Getting the PointWidget Focal Point
        /// 2. Getting the Finger Actor 2D position into 3D World Position using FocalPoint Z axis
        /// 3. Create the Translation transformation from finger by lookint at last position
        /// 3. Apply the translation to the widget position
        /// 4. PointWidget Update

        //// --------MOVE PointWidget 1 with Finger 1 TRANSFORMATION--------"

        {

//             std::cout << "Finger Translation: New[x,y]: " << actorPos2D[0] << ", " << actorPos2D[1]
//                                        << "\t Old[x,y]: " << actorPos2D2[0] << ", " << actorPos2D2[1] << endl;


             double focalPoint1[4];
             double z1;

             pointWidget1_->ComputeWorldToDisplay(defaultRenderer, pointW1[0], pointW1[1], pointW1[2], focalPoint1);
             z1 = focalPoint1[2];

             /// Get the Motion Vector of the Finger

             double fingerVector2D[2] ;
             fingerVector2D[0] =  actorPos2D[0] - actorPos2D2[0];
             fingerVector2D[1] =  actorPos2D[1] - actorPos2D2[1];

             /// Translate the 2D PointWidget

             double point2DW1New[2];
             point2DW1New[0] = focalPoint1[0] + fingerVector2D[0];
             point2DW1New[1] = focalPoint1[1] + fingerVector2D[1];

             /// Send New 2D Point to World

             pointWidget1_->ComputeDisplayToWorld(defaultRenderer, (point2DW1New[0]), point2DW1New[1], z1, pickPoint1);


            /// If the NEW pointWidget 1 IS NOT ON the Volume Bounds, Set pointWidget Change to true
            ///

            bool xRangeValid = (pickPoint1[0] >= global_Volume->GetBounds()[0]) && (pickPoint1[0] <= global_Volume->GetBounds()[1]);
            bool yRangeValid = (pickPoint1[1] >= global_Volume->GetBounds()[2]) && (pickPoint1[1] <= global_Volume->GetBounds()[3]);
            bool zRangeValid = (pickPoint1[2] >= global_Volume->GetBounds()[4]) && (pickPoint1[2] <= global_Volume->GetBounds()[5]);

            if (xRangeValid && yRangeValid && zRangeValid)
                  widget1Valid = true;

//            if (!xRangeValid || !yRangeValid || !zRangeValid)
//                  std::cout << "INVALID POSITION" << endl;
            // if (pickPoint1[0])


             std::cout << std::fixed << std::setprecision(2) ;


//             std::cout << "Finger 1: New[x,y,z]: " << pickPoint1[0] << ", " << pickPoint1[1] << ", " << pickPoint1[2]
//                       << "\t Old[x,y,z]: " << pointW1[0] << ", " << pointW1[1]  << ", " << pointW1[2] << endl;

        }

        //// --------MOVE PointWidget 2 with Finger 1 TRANSFORMATION--------"

        {

//             std::cout << "Finger Translation: New[x,y]: " << actorPos2D[0] << ", " << actorPos2D[1]
//                                        << "\t Old[x,y]: " << actorPos2D2[0] << ", " << actorPos2D2[1] << endl;


             double focalPoint2[4];
             double z2;

             pointWidget2_->ComputeWorldToDisplay(defaultRenderer, pointW2[0], pointW2[1], pointW2[2], focalPoint2);
             z2 = focalPoint2[2];


             /// Get the Motion Vector of the Finger

             double fingerVector2D[2] ;
             fingerVector2D[0] =  actorPos2D[0] - actorPos2D2[0];
             fingerVector2D[1] =  actorPos2D[1] - actorPos2D2[1];

             /// Translate the 2D PointWidget

             double point2DW2New[2];
             point2DW2New[0] = focalPoint2[0] + fingerVector2D[0];
             point2DW2New[1] = focalPoint2[1] + fingerVector2D[1];

             /// Send New 2D Point to World

             pointWidget2_->ComputeDisplayToWorld(defaultRenderer, (point2DW2New[0]), point2DW2New[1], z2, pickPoint2);

             /// If the NEW pointWidget 2 IS NOT ON the Volume Bounds, Set pointWidget Change to true
             ///

             bool xRangeValid = (pickPoint2[0] >= global_Volume->GetBounds()[0]) && (pickPoint2[0] <= global_Volume->GetBounds()[1]);
             bool yRangeValid = (pickPoint2[1] >= global_Volume->GetBounds()[2]) && (pickPoint2[1] <= global_Volume->GetBounds()[3]);
             bool zRangeValid = (pickPoint2[2] >= global_Volume->GetBounds()[4]) && (pickPoint2[2] <= global_Volume->GetBounds()[5]);

             if (xRangeValid && yRangeValid && zRangeValid)
                   widget2Valid = true;



        }

        /// If the widgets are Both Valid Translations, apply the New Point Positions
        if (widget1Valid && widget2Valid)
        {
            if (userTestRunning() && !pointWidget1Active && !pointWidget2Active)
            {
                userTestDlg->incSubVolPoint1();
                userTestDlg->incSubVolPoint2();
            }

            if (!pointWidget2Active && !pointWidget1Active)
            {
                std::cout << "SubVolume TRIGGERED" << endl ;
                pointWidget1Active = true;
                pointWidget2Active = true;
            }

            pointWidget1_->SetPosition(pickPoint1);

            /// Interact, if desired

            pointWidget1_->InvokeEvent(vtkCommand::InteractionEvent, NULL);



            pointWidget2_->SetPosition(pickPoint2);

            /// Interact, if desired

            pointWidget2_->InvokeEvent(vtkCommand::InteractionEvent, NULL);
        } /// if (widget1Valid && widget2Valid)
    } ///(validXmax && validXmin && validYmin && validYmax &&
}

void MainWindow::touchFinger1ArbSlicePressed()
{

    /// FingerActor Position Information
    double * actorPos2D = this->ui->qvtkWidgetLeft->fingerActor1->GetPositionCoordinate()->GetValue();

    customArbPlaneWidget->finger1Pressed(actorPos2D[0], actorPos2D[1]);

}


void MainWindow::touchFinger1ArbSliceMoving()
{
    /// FingerActor Position Information
    double * actorPos2D = this->ui->qvtkWidgetLeft->fingerActor1->GetPositionCoordinate()->GetValue();

    double * actorPos2D2 = this->ui->qvtkWidgetLeft->fingerActor1->GetPosition2Coordinate()->GetValue();

    vtkCoordinate *coordinate = this->ui->qvtkWidgetLeft->fingerActor1->GetActualPositionCoordinate();
    double * actorPosWorld =  coordinate->GetComputedWorldValue(this->ui->qvtkWidgetLeft->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

    customArbPlaneWidget->finger1Moving(actorPos2D[0], actorPos2D[1], actorPos2D2[0], actorPos2D2[1]);
}

void MainWindow::touchFinger1ArbSliceReleased()
{
    customArbPlaneWidget->finger1Released();
}

void MainWindow::touchFinger2ArbSlicePressed()
{
    double * actor1Pos2D = this->ui->qvtkWidgetLeft->fingerActor1->GetPositionCoordinate()->GetValue();

    double * actor2Pos2D = this->ui->qvtkWidgetLeft->fingerActor2->GetPositionCoordinate()->GetValue();

    double combined2D[2] = {(actor1Pos2D[0] + actor2Pos2D[0]) / 2 ,
                           (actor1Pos2D[1] + actor2Pos2D[2]) / 2 };

    customArbPlaneWidget->finger2Pressed(combined2D[0], combined2D[1]);
}

void MainWindow::touchFinger2ArbSliceMoving()
{
    /// FingerActor Position Information
    double *actor1APos2D = this->ui->qvtkWidgetLeft->fingerActor1->GetPositionCoordinate()->GetValue();

    double *actor1BPos2D = this->ui->qvtkWidgetLeft->fingerActor1->GetPosition2Coordinate()->GetValue();

    /// FingerActor Position Information
    double *actor2APos2D = this->ui->qvtkWidgetLeft->fingerActor2->GetPositionCoordinate()->GetValue();

    double * actor2BPos2D = this->ui->qvtkWidgetLeft->fingerActor2->GetPosition2Coordinate()->GetValue();

  //  std::cout << "--------MOVING PLANE ----------------" << endl;

    //std::cout << "start [x,y]: " << actor

    double combinedA2D[2] = {(actor1APos2D[0] + actor2APos2D[0]) / 2 ,
                             (actor1APos2D[1] + actor2APos2D[2]) / 2 };

    double combinedB2D[2] = {(actor1BPos2D[0] + actor2BPos2D[0]) / 2 ,
                             (actor1BPos2D[1] + actor2BPos2D[2]) / 2 };

    customArbPlaneWidget->finger2Moving(combinedA2D[0], combinedA2D[1], combinedB2D[0], combinedB2D[1]);
}

void MainWindow::touchFInger2ArbSliceReleased()
{
    customArbPlaneWidget->finger1Released();
}


void MainWindow::on_actionTracking_triggered()
{
    on_actionReset_Camera_triggered();

    /// The plane widget is used probe the dataset.
    vtkSmartPointer<vtkPolyData> point =
            vtkSmartPointer<vtkPolyData>::New();

    vtkSmartPointer<vtkProbeFilter> probe =
            vtkSmartPointer<vtkProbeFilter>::New();
    probe->SetInput(point);
    probe->SetSource(global_Reader->GetOutput());

    /// create glyph
    ///
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
    glyph->SetScaleFactor(global_Volume->GetLength() * 0.1);

    vtkSmartPointer<vtkPolyDataMapper> glyphMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    glyphMapper->SetInputConnection(glyph->GetOutputPort());

    vtkSmartPointer<vtkActor> glyphActor =
            vtkSmartPointer<vtkActor>::New();
    glyphActor->SetMapper(glyphMapper);
    glyphActor->VisibilityOn();

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
            this->defaultRenderer;

    // The SetInteractor method is how 3D widgets are associated with the render
    // window interactor. Internally, SetInteractor sets up a bunch of callbacks
    // using the Command/Observer mechanism (AddObserver()).
    vtkSmartPointer<vtkmyPWCallback> myCallback =
            vtkSmartPointer<vtkmyPWCallback>::New();
    myCallback->PolyData = point;
    myCallback->Actor = glyphActor;
    myCallback->TextActor = textActor;

    pointWidget1_ = vtkPointWidget::New();
    pointWidget1_->SetInteractor(this->ui->qvtkWidgetLeft->GetInteractor());
    pointWidget1_->SetInput(global_Reader->GetOutput());
    pointWidget1_->AllOff();
    pointWidget1_->PlaceWidget();
    pointWidget1_->GetPolyData(point);
    pointWidget1_->EnabledOff();
    //vtkEventConnector->Connect(pointWidget_, vtkCommand::InteractionEvent, this, SLOT(pointWidgetCallBack()));
    pointWidget1_->AddObserver(vtkCommand::InteractionEvent  ,myCallback);

    renderer->AddActor(glyphActor);
    renderer->AddActor2D(textActor);

    pointWidget1_->EnabledOn();
    //pointWidget1_->On();


}

void MainWindow::addSecondPointer()
{
    on_actionReset_Camera_triggered();

    /// The plane widget is used probe the dataset.
    vtkSmartPointer<vtkPolyData> point =
            vtkSmartPointer<vtkPolyData>::New();

    vtkSmartPointer<vtkProbeFilter> probe =
            vtkSmartPointer<vtkProbeFilter>::New();
    probe->SetInput(point);
    probe->SetSource(global_Reader->GetOutput());

    /// create glyph
    ///
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
    glyph->SetScaleFactor(global_Volume->GetLength() * 0.1);

    vtkSmartPointer<vtkPolyDataMapper> glyphMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    glyphMapper->SetInputConnection(glyph->GetOutputPort());

    vtkSmartPointer<vtkActor> glyphActor =
            vtkSmartPointer<vtkActor>::New();
    glyphActor->SetMapper(glyphMapper);
    glyphActor->VisibilityOn();

    /// A Text Widget showing the point location
    ///
    ///
    vtkSmartPointer<vtkTextActor> textActor =
            vtkSmartPointer<vtkTextActor>::New();
    textActor->GetTextProperty()->SetFontSize(16);
    textActor->SetPosition(400, 20);
    textActor->SetInput("cursor:");
    textActor->GetTextProperty()->SetColor(0.3400, 0.8100, 0.8900);

    /// Extract the RenderWindow, Renderer and  add both Actors
    ///
    ///

    vtkSmartPointer<vtkRenderer> renderer =
            this->defaultRenderer;
    // The SetInteractor method is how 3D widgets are associated with the render
    // window interactor. Internally, SetInteractor sets up a bunch of callbacks
    // using the Command/Observer mechanism (AddObserver()).
    vtkSmartPointer<vtkmyPWCallback> myCallback =
            vtkSmartPointer<vtkmyPWCallback>::New();
    myCallback->PolyData = point;
    myCallback->Actor = glyphActor;
    myCallback->TextActor = textActor;

    pointWidget2_ = vtkPointWidget::New();
    pointWidget2_->SetInteractor(this->ui->qvtkWidgetLeft->GetInteractor());
    pointWidget2_->SetInput(global_Reader->GetOutput());
    pointWidget2_->AllOff();
    pointWidget2_->PlaceWidget();
    pointWidget2_->GetPolyData(point);
    pointWidget2_->EnabledOff();
    //vtkEventConnector->Connect(pointWidget_, vtkCommand::InteractionEvent, this, SLOT(pointWidgetCallBack()));
    pointWidget2_->AddObserver(vtkCommand::InteractionEvent  ,myCallback);

    renderer->AddActor(glyphActor);
    renderer->AddActor2D(textActor);

    pointWidget2_->EnabledOn();

    //    if((vtkPointWidget::WidgetState)::Moving == pointWidget_->Moving)
    //    {
    //        std::cout << "Moving... " << std::endl;
    //    }
    loadSubVolume();
}

void MainWindow::addThirdPointer()
{
    //on_actionReset_Camera_triggered();

    /// The plane widget is used probe the dataset.
    vtkSmartPointer<vtkPolyData> point =
            vtkSmartPointer<vtkPolyData>::New();

    vtkSmartPointer<vtkProbeFilter> probe =
            vtkSmartPointer<vtkProbeFilter>::New();
    probe->SetInput(point);
    probe->SetSource(global_Reader->GetOutput());

    /// create glyph
    ///
    ///
    vtkSmartPointer<vtkSphereSource> pointMarker =
            vtkSmartPointer<vtkSphereSource>::New();
    pointMarker->SetRadius(0.5);

    vtkSmartPointer<vtkGlyph3D> glyph =
            vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetInputConnection(probe->GetOutputPort());
    glyph->SetSourceConnection(pointMarker->GetOutputPort());
    glyph->SetVectorModeToUseVector();
    glyph->SetScaleModeToDataScalingOff();
    glyph->SetScaleFactor(global_Volume->GetLength() * 0.1);

    vtkSmartPointer<vtkPolyDataMapper> glyphMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    glyphMapper->SetInputConnection(glyph->GetOutputPort());

    vtkSmartPointer<vtkActor> glyphActor =
            vtkSmartPointer<vtkActor>::New();
    glyphActor->SetMapper(glyphMapper);
    glyphActor->VisibilityOn();

    /// A Text Widget showing the point location
    ///
    ///
    vtkSmartPointer<vtkTextActor> textActor =
            vtkSmartPointer<vtkTextActor>::New();
    textActor->GetTextProperty()->SetFontSize(16);
    textActor->SetPosition(400, 20);
    textActor->SetInput("cursor:");
    textActor->GetTextProperty()->SetColor(0.3400, 0.8100, 0.8900);

    /// Extract the RenderWindow, Renderer and  add both Actors
    ///
    ///

    vtkSmartPointer<vtkRenderer> renderer =
            riw[0]->GetRenderer();


    renderer->AddActor(glyphActor);
    renderer->AddActor2D(textActor);

}

void MainWindow::trackSubVolume(double* point1, double* point2)
{
    ///
    /// 1. Determine the MIN and MAX values for each axis
    /// 2. Create a subVolume bounding box using the two points
    /// 3. Update the current volume with the new specs
    ///

    double minX;
    double maxX;
    double minY;
    double maxY;
    double minZ;
    double maxZ;


    //determine X Min/Max
    if (point1[0] < point2[0])
    {
        minX = point1[0];
        maxX = point2[0];
    }
    else
    {
        minX = point2[0];
        maxX = point1[0];
    }
    ////////////////////////////////////////////////////
    //determine Y Min/Max
    if (point1[1] < point2[1])
    {
        minY = point1[1];
        maxY = point2[1];
    }
    else
    {
        minY = point2[1];
        maxY = point1[1];
    }
    ///////////////////////////////////////////////////
    //determine Z Min/Max
    if (point1[2] < point2[2])
    {
        minZ = point1[2];
        maxZ = point2[2];
    }
    else
    {
        minZ = point2[2];
        maxZ = point1[2];
    }

    global_subVolume->SetBounds(minX, maxX, minY,maxY, minZ, maxZ);

    this->ui->lineSubVolXMin->setText(QString::number(minX, 'f', 2));
    this->ui->lineSubVolXMax->setText(QString::number(maxX, 'f', 2));
    this->ui->lineSubVolYMin->setText(QString::number(minY, 'f', 2));
    this->ui->lineSubVolYMax->setText(QString::number(maxY, 'f', 2));
    this->ui->lineSubVolZMin->setText(QString::number(minZ, 'f', 2));
    this->ui->lineSubVolZMax->setText(QString::number(maxZ, 'f', 2));
}

double MainWindow::Distance2Point(double *point1, double *point2)
{
    double dist =  sqrt(pow((point1[0]-point2[0]),2) +
            pow((point1[1]-point2[1]),2) +
            pow((point1[2]-point2[2]),2));

    return dist;
}

void MainWindow::loadSubVolume()
{
    vtkSmartPointer<vtkCubeSource> subVolume =
            vtkSmartPointer<vtkCubeSource>::New();

    //#if VTK_MAJOR_VERSION <= 5
    subVolume->SetBounds(global_Volume->GetBounds());

    vtkSmartPointer<vtkPolyDataMapper> subVolMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();

    subVolMapper->SetInputConnection(subVolume->GetOutputPort());
    vtkSmartPointer<vtkActor>subVolActor =
            vtkSmartPointer<vtkActor>::New();

    subVolActor->SetMapper(subVolMapper);
    subVolActor->GetProperty()->SetColor(1,1,1);
    subVolActor->GetProperty()->SetOpacity(0.5);

    this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->
            GetRenderers()->GetFirstRenderer()->AddActor(subVolActor);

    this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();

    global_subVolume = subVolume;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
///

void MainWindow::on_checkAxisSliceVolumeEnable_stateChanged(int arg1)
{
    global_Volume->SetVisibility(arg1);
}

void MainWindow::sliceXEnable(int mode)
{
    planeWidget[0]->SetEnabled(mode);
    this->sliderWidgetX->SetEnabled(mode);
}

void MainWindow::sliceYEnable(int mode)
{
    planeWidget[1]->SetEnabled(mode);
    this->sliderWidgetY->SetEnabled(mode);
}

void MainWindow::sliceZEnable(int mode)
{
    planeWidget[2]->SetEnabled(mode);
    this->sliderWidgetZ->SetEnabled(mode);
}


void MainWindow::ResetViews()
{
    for (int i = 0; i < 3; i++)
    {
        riw[i]->SetSlice(imageDims[i]/2);
    }

    //this->sliderWidgetX->SetV
    static_cast<vtkSliderRepresentation3D*>(this->sliderWidgetX->GetRepresentation())->SetValue(xAxisSlice);
    static_cast<vtkSliderRepresentation3D*>(this->sliderWidgetY->GetRepresentation())->SetValue(yAxisSlice);
    static_cast<vtkSliderRepresentation3D*>(this->sliderWidgetZ->GetRepresentation())->SetValue(zAxisSlice);

    this->ui->lbl_XSlice->setText(QString("%1").arg(xAxisSlice));
    this->ui->lbl_YSlice->setText(QString("%1").arg(yAxisSlice));
    this->ui->lbl_ZSlice->setText(QString("%1").arg(zAxisSlice));

    // Reset the reslice image views
    for (int i = 0; i < 3; i++)
    {
        riw[i]->Reset();
    }

    // Also sync the Image plane widget on the 3D top right view with any
    // changes to the reslice cursor.
    for (int i = 0; i < 3; i++)
    {
        vtkPlaneSource *ps = static_cast< vtkPlaneSource * >(
                    planeWidget[i]->GetPolyDataAlgorithm());
        ps->SetNormal(riw[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
        ps->SetCenter(riw[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());

        // If the reslice plane has modified, update it on the 3D widget
        this->planeWidget[i]->UpdatePlacement();
    }

    // Render in response to changes.
    this->Render();
}

void MainWindow::ResetViewX()
{
    std::cout << "ResetViewX" << std::endl;

    int i = 0;
    // Reset the reslice image views
    //for (int i = 0; i < 3; i++)
    //{
    riw[i]->Reset();
    static_cast<vtkSliderRepresentation3D*>(this->sliderWidgetX->GetRepresentation())->SetValue(imageDims[i]/2);
    this->ui->lbl_XSlice->setText(QString("%1").arg(xAxisSlice));
    //riw[i]->SetSlice();

    //}

    // Also sync the Image plane widget on the 3D top right view with any
    // changes to the reslice cursor.
    // for (int i = 0; i < 3; i++)
    //{
    vtkPlaneSource *ps = static_cast< vtkPlaneSource * >(
                planeWidget[i]->GetPolyDataAlgorithm());
    ps->SetNormal(riw[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
    ps->SetCenter(riw[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());

    // If the reslice plane has modified, update it on the 3D widget
    this->planeWidget[i]->UpdatePlacement();
    // }

    // Render in response to changes.
    this->Render();
}

void MainWindow::ResetViewY()
{
    int i = 1;
    // Reset the reslice image views
    //for (int i = 0; i < 3; i++)
    //{
    riw[i]->Reset();
    static_cast<vtkSliderRepresentation3D*>(this->sliderWidgetY->GetRepresentation())->SetValue(imageDims[i]/2);
    this->ui->lbl_YSlice->setText(QString("%1").arg(yAxisSlice));
    //}

    // Also sync the Image plane widget on the 3D top right view with any
    // changes to the reslice cursor.
    // for (int i = 0; i < 3; i++)
    //{
    vtkPlaneSource *ps = static_cast< vtkPlaneSource * >(
                planeWidget[i]->GetPolyDataAlgorithm());
    ps->SetNormal(riw[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
    ps->SetCenter(riw[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());

    // If the reslice plane has modified, update it on the 3D widget
    this->planeWidget[i]->UpdatePlacement();
    // }

    // Render in response to changes.
    this->Render();
}

void MainWindow::ResetViewZ()
{
    int i = 2;
    // Reset the reslice image views
    //for (int i = 0; i < 3; i++)
    //{
    riw[i]->Reset();
    static_cast<vtkSliderRepresentation3D*>(this->sliderWidgetZ->GetRepresentation())->SetValue(imageDims[i]/2);
    this->ui->lbl_ZSlice->setText(QString("%1").arg(zAxisSlice));
    //}

    // Also sync the Image plane widget on the 3D top right view with any
    // changes to the reslice cursor.
    // for (int i = 0; i < 3; i++)
    //{
    vtkPlaneSource *ps = static_cast< vtkPlaneSource * >(
                planeWidget[i]->GetPolyDataAlgorithm());
    ps->SetNormal(riw[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
    ps->SetCenter(riw[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());

    // If the reslice plane has modified, update it on the 3D widget
    this->planeWidget[i]->UpdatePlacement();
    // }

    // Render in response to changes.
    this->Render();
}

void MainWindow::UpdateSliceX(int slice)
{
    int i = 0;
    // Reset the reslice image views
    //for (int i = 0; i < 3; i++)
    //{
    riw[i]->SetSlice(slice);

    //}

    this->ui->lbl_XSlice->setText(QString("%1").arg(slice));
    // Also sync the Image plane widget on the 3D top right view with any
    // changes to the reslice cursor.
    // for (int i = 0; i < 3; i++)
    //{
    vtkPlaneSource *ps = static_cast< vtkPlaneSource * >(
                planeWidget[i]->GetPolyDataAlgorithm());
    ps->SetNormal(riw[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
    ps->SetCenter(riw[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());
    planeWidget[i]->SetSliceIndex(slice);

    // If the reslice plane has modified, update it on the 3D widget
    this->planeWidget[i]->UpdatePlacement();
    // }

    // Render in response to changes.
    this->Render();
}

void MainWindow::UpdateSliceX()
{
    int slice = (static_cast<vtkSliderRepresentation *>(sliderWidgetX->GetRepresentation()))->GetValue();
    int i = 0;
    // Reset the reslice image views
    //for (int i = 0; i < 3; i++)
    //{
    riw[i]->SetSlice(slice);

    //}

    this->ui->lbl_XSlice->setText(QString("%1").arg(slice));
    // Also sync the Image plane widget on the 3D top right view with any
    // changes to the reslice cursor.
    // for (int i = 0; i < 3; i++)
    //{
    vtkPlaneSource *ps = static_cast< vtkPlaneSource * >(
                planeWidget[i]->GetPolyDataAlgorithm());
    ps->SetNormal(riw[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
    ps->SetCenter(riw[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());
    planeWidget[i]->SetSliceIndex(slice);

    // If the reslice plane has modified, update it on the 3D widget
    this->planeWidget[i]->UpdatePlacement();
    // }

    // Render in response to changes.
    this->Render();
}

void MainWindow::UpdateSliceY(int slice)
{
    int i = 1;
    // Reset the reslice image views
    //for (int i = 0; i < 3; i++)
    //{
    riw[i]->SetSlice(slice);

    //}

    this->ui->lbl_YSlice->setText(QString("%1").arg(slice));
    // Also sync the Image plane widget on the 3D top right view with any
    // changes to the reslice cursor.
    // for (int i = 0; i < 3; i++)
    //{
    vtkPlaneSource *ps = static_cast< vtkPlaneSource * >(
                planeWidget[i]->GetPolyDataAlgorithm());
    ps->SetNormal(riw[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
    ps->SetCenter(riw[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());
    planeWidget[i]->SetSliceIndex(slice);

    // If the reslice plane has modified, update it on the 3D widget
    this->planeWidget[i]->UpdatePlacement();
    // }

    // Render in response to changes.
    this->Render();
}

void MainWindow::UpdateSliceY()
{
    int i = 1;
    int slice = (static_cast<vtkSliderRepresentation *>(sliderWidgetY->GetRepresentation()))->GetValue();
    // Reset the reslice image views
    //for (int i = 0; i < 3; i++)
    //{
    riw[i]->SetSlice(slice);

    //}

    this->ui->lbl_YSlice->setText(QString("%1").arg(slice));
    // Also sync the Image plane widget on the 3D top right view with any
    // changes to the reslice cursor.
    // for (int i = 0; i < 3; i++)
    //{
    vtkPlaneSource *ps = static_cast< vtkPlaneSource * >(
                planeWidget[i]->GetPolyDataAlgorithm());
    ps->SetNormal(riw[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
    ps->SetCenter(riw[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());
    planeWidget[i]->SetSliceIndex(slice);

    // If the reslice plane has modified, update it on the 3D widget
    this->planeWidget[i]->UpdatePlacement();
    // }

    // Render in response to changes.
    this->Render();
}

void MainWindow::UpdateSliceZ(int slice)
{
    int i = 2;
    // Reset the reslice image views
    //for (int i = 0; i < 3; i++)
    //{
    riw[i]->SetSlice(slice);

    //}
    this->ui->lbl_ZSlice->setText(QString("%1").arg(slice));

    // Also sync the Image plane widget on the 3D top right view with any
    // changes to the reslice cursor.
    // for (int i = 0; i < 3; i++)
    //{
    vtkPlaneSource *ps = static_cast< vtkPlaneSource * >(
                planeWidget[i]->GetPolyDataAlgorithm());
    ps->SetNormal(riw[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
    ps->SetCenter(riw[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());
    planeWidget[i]->SetSliceIndex(slice);

    // If the reslice plane has modified, update it on the 3D widget
    this->planeWidget[i]->UpdatePlacement();
    // }

    // Render in response to changes.
    this->Render();
}

void MainWindow::UpdateSliceZ()
{
    int slice = (static_cast<vtkSliderRepresentation *>(sliderWidgetZ->GetRepresentation()))->GetValue();
    int i = 2;
    // Reset the reslice image views
    //for (int i = 0; i < 3; i++)
    //{
    riw[i]->SetSlice(slice);

    //}
    this->ui->lbl_ZSlice->setText(QString("%1").arg(slice));

    // Also sync the Image plane widget on the 3D top right view with any
    // changes to the reslice cursor.
    // for (int i = 0; i < 3; i++)
    //{
    vtkPlaneSource *ps = static_cast< vtkPlaneSource * >(
                planeWidget[i]->GetPolyDataAlgorithm());
    ps->SetNormal(riw[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
    ps->SetCenter(riw[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());
    planeWidget[i]->SetSliceIndex(slice);

    // If the reslice plane has modified, update it on the 3D widget
    this->planeWidget[i]->UpdatePlacement();
    // }

    // Render in response to changes.
    this->Render();
}

void MainWindow::Render()
{
    for (int i = 0; i < 3; i++)
    {
        riw[i]->Render();
    }
    this->ui->qvtkWidgetLeft->GetRenderWindow()->Render();
}

void MainWindow::AddDistanceMeasurementToView1()
{
    this->AddDistanceMeasurementToView(1);
}

void MainWindow::AddDistanceMeasurementToView(int i)
{
    // remove existing widgets.
    if (this->DistanceWidget[i])
    {
        this->DistanceWidget[i]->SetEnabled(0);
        this->DistanceWidget[i] = NULL;
    }

    // add new widget
    this->DistanceWidget[i] = vtkSmartPointer< vtkDistanceWidget >::New();
    this->DistanceWidget[i]->SetInteractor(
                this->riw[i]->GetResliceCursorWidget()->GetInteractor());

    // Set a priority higher than our reslice cursor widget
    this->DistanceWidget[i]->SetPriority(
                this->riw[i]->GetResliceCursorWidget()->GetPriority() + 0.01);

    //vtkSmartPointer< vtkPointHandleRepresentation2D > handleRep =
    //      vtkSmartPointer< vtkPointHandleRepresentation2D >::New();
    vtkSmartPointer< vtkDistanceRepresentation2D > distanceRep =
            vtkSmartPointer< vtkDistanceRepresentation2D >::New();
    this->DistanceWidget[i]->SetRepresentation(distanceRep);
    distanceRep->InstantiateHandleRepresentation();
    distanceRep->GetPoint1Representation()->SetPointPlacer(riw[i]->GetPointPlacer());
    distanceRep->GetPoint2Representation()->SetPointPlacer(riw[i]->GetPointPlacer());

    // Add the distance to the list of widgets whose visibility is managed based
    // on the reslice plane by the ResliceImageViewerMeasurements class
    this->riw[i]->GetMeasurements()->AddItem(this->DistanceWidget[i]);

    this->DistanceWidget[i]->CreateDefaultRepresentation();
    this->DistanceWidget[i]->EnabledOn();

    vtkSmartPointer<vtkPointPlacer> pointPlacer =
            vtkSmartPointer<vtkPointPlacer>::New();

    pointPlacer = riw[i]->GetPointPlacer();

    //distanceRep->Print(std::cout);

    distanceRep->GetPoint1Representation()->SetWorldPosition(riw[0]->GetResliceCursor()->GetCenter());
    //distanceRep->GetPoint1Representation()->SetWorldPosition(riw[0]->GetResliceCursor()->);
    this->DistanceWidget[i]->SetRepresentation(distanceRep);

}


void MainWindow::captureSlice(int axis)
{
    SliceDialog sliceSaver;

    sliceSaver.setModal(true);
    //planeWidget[axis]->GetPolyDataAlgorithm()->Print(std::cout);
    sliceSaver.UpdateQVTKWidget(riw[axis], axis);
    sliceSaver.exec();
}

void MainWindow::on_buttonExportXSlice_clicked()
{
    captureSlice(0);
}

void MainWindow::on_buttonExportYSlice_clicked()
{
    captureSlice(1);
}

void MainWindow::on_buttonExportZSlice_clicked()
{
    captureSlice(2);
}

void MainWindow::on_actionSliceAxisArbitrary_triggered()
{
    closeTabs();
    this->ui->tabLogWidget->insertTab(0, this->ui->SliceVolTab2, "Arbitrary Slice");
    //this->ui->tabLogWidget->insertTab(1, this->ui->InfoTab, "Info Tab");
    this->ui->tabLogWidget->setCurrentIndex(0);
}



void MainWindow::LoadSliderWidgets()
{
    ///
    /// \brief bounds
    /// [0] x min
    /// [1] x Max
    /// [2] y Min
    /// [3] y Max
    /// [4] z Min
    /// [5] z Max

    double bounds[6];
    double xMin, xMax, yMin, yMax, zMin, zMax;
    double offset = 5;
    double onset = 3;



    global_Volume->GetBounds(bounds);

    xMin = bounds[0];
    xMax = bounds[1];
    yMin = bounds[2];
    yMax = bounds[3];
    zMin = bounds[4];
    zMax = bounds[5];

    /// Provide the Points for the EndPoints for the Sliders to be placed outside the bounding box of the volume

    double xPoint1[3] = {xMin + onset, yMax + offset, zMax /2};
    double xPoint2[3] = {xMax - onset, yMax + offset, zMax /2};

    double yPoint1[3] = {xMax + offset, yMax - onset, zMax /2};
    double yPoint2[3] = {xMax + offset, yMin + onset, zMax /2};

    double zPoint1[3] = {xMax + offset , yMax, zMax - onset};
    double zPoint2[3] = {xMax + offset , yMax, zMin + onset};

    double sliderXFactor, sliderYFactor, sliderZFactor = 1;
    double distanceMax = 0;

    ///
    /// Upon construction of the Sliders.. the thickness is determined by distance between the 2 points.
    /// Therefore in order to ensure the thickness matches across all sliders, we need to determine the
    /// constant size to use across all WIDTHS, ENDCAPS & SPHERES

    double distanceX = Distance2Point(xPoint1, xPoint2);
    double distanceY = Distance2Point(yPoint1, yPoint2);
    double distanceZ = Distance2Point(zPoint1, zPoint2);


    /// Once we have the distances, we now get a Constant to Multiply the differences in sizes
    /// We can't use a single Factor because each particular slider may be different requiring a different
    /// multiplication factor.

    /// We first determine which is the longest distance and then we use it to compare each axis distance
    /// to the maximum and Formulate a factor to which we multiply the sliders to get them all to a uniform length

    if (max(distanceX, distanceY) < distanceZ)
        distanceMax = distanceZ;
    else
        distanceMax = max(distanceX, distanceY);

    sliderXFactor = distanceMax / distanceX;
    sliderYFactor = distanceMax / distanceY;
    sliderZFactor = distanceMax / distanceZ;


    /////
    ///  X SLIDER WIDGET
    ///
    vtkSmartPointer<vtkSliderRepresentation3D> sliderRepX =
            vtkSmartPointer<vtkSliderRepresentation3D>::New();
    sliderRepX->SetMinimumValue(xMin);
    sliderRepX->SetMaximumValue(xMax);
    sliderRepX->SetValue(xMax /2);
    sliderRepX->SetLabelFormat("%4.0lf");
    sliderRepX->GetTubeProperty()->SetColor(1.0, 0.0, 0.0);

    sliderRepX->GetPoint1Coordinate()->SetCoordinateSystemToWorld();
    sliderRepX->GetPoint1Coordinate()->SetValue(xPoint1);
    sliderRepX->GetPoint2Coordinate()->SetCoordinateSystemToWorld();
    sliderRepX->GetPoint2Coordinate()->SetValue(xPoint2);

    sliderRepX->SetSliderLength(0.055 * sliderXFactor);
    sliderRepX->SetSliderWidth(0.05 * sliderXFactor);
    sliderRepX->SetEndCapLength(0.05 * sliderXFactor);
    sliderRepX->SetEndCapWidth(0.05 * sliderXFactor);
    sliderRepX->SetTubeWidth(0.025 * sliderXFactor);
    sliderRepX->SetLabelHeight(0.05 * sliderXFactor);

    this->sliderWidgetX =
            vtkSmartPointer<vtkSliderWidget>::New();
    sliderWidgetX->SetInteractor(this->ui->qvtkWidgetLeft->GetInteractor());
    sliderWidgetX->SetRepresentation(sliderRepX);
    //sliderWidgetX->SetAnimationModeToAnimate();
    sliderWidgetX->EnabledOn();

    //////////////////////////////////////////////////////////
    ///  Y SLIDER WIDGET
    ////

    vtkSmartPointer<vtkSliderRepresentation3D> sliderRepY =
            vtkSmartPointer<vtkSliderRepresentation3D>::New();
    sliderRepY->SetMinimumValue(yMin);
    sliderRepY->SetMaximumValue(yMax);
    sliderRepY->SetValue(yMax/2);
    sliderRepY->SetLabelFormat("%4.0lf");
    sliderRepY->GetTubeProperty()->SetColor(0.0, 1.0, 0.0);

    sliderRepY->GetPoint1Coordinate()->SetCoordinateSystemToWorld();
    sliderRepY->GetPoint1Coordinate()->SetValue(yPoint1);
    sliderRepY->GetPoint2Coordinate()->SetCoordinateSystemToWorld();
    sliderRepY->GetPoint2Coordinate()->SetValue(yPoint2);

    sliderRepY->SetSliderLength(0.055 * sliderYFactor);
    sliderRepY->SetSliderWidth(0.05 * sliderYFactor);
    sliderRepY->SetEndCapLength(0.05 * sliderYFactor);
    sliderRepY->SetEndCapWidth(0.05 * sliderYFactor);
    sliderRepY->SetTubeWidth(0.025 * sliderYFactor);
    sliderRepY->SetLabelHeight(0.05 * sliderYFactor);

    this->sliderWidgetY =
            vtkSmartPointer<vtkSliderWidget>::New();
    sliderWidgetY->SetInteractor(this->ui->qvtkWidgetLeft->GetInteractor());
    sliderWidgetY->SetRepresentation(sliderRepY);
    sliderWidgetY->SetAnimationModeToAnimate();
    sliderWidgetY->EnabledOn();


    //////////////////////////////////////////////////////////
    ///  Z SLIDER WIDGET
    ////

    vtkSmartPointer<vtkSliderRepresentation3D> sliderRepZ =
            vtkSmartPointer<vtkSliderRepresentation3D>::New();
    sliderRepZ->SetMinimumValue(zMin);
    sliderRepZ->SetMaximumValue(zMax);
    sliderRepZ->SetValue(zMax/2);
    sliderRepZ->SetLabelFormat("%4.0lf");
    sliderRepZ->GetTubeProperty()->SetColor(0.0, 0.0, 1.0);

    sliderRepZ->GetPoint1Coordinate()->SetCoordinateSystemToWorld();
    sliderRepZ->GetPoint1Coordinate()->SetValue(zPoint1);
    sliderRepZ->GetPoint2Coordinate()->SetCoordinateSystemToWorld();
    sliderRepZ->GetPoint2Coordinate()->SetValue(zPoint2);

    sliderRepZ->SetSliderLength(0.055 * sliderZFactor);
    sliderRepZ->SetSliderWidth(0.05 * sliderZFactor);
    sliderRepZ->SetEndCapLength(0.05 * sliderZFactor);
    sliderRepZ->SetEndCapWidth(0.05 * sliderZFactor);
    sliderRepZ->SetTubeWidth(0.025 * sliderZFactor);
    sliderRepZ->SetLabelHeight(0.05 * sliderZFactor);

    this->sliderWidgetZ =
            vtkSmartPointer<vtkSliderWidget>::New();
    sliderWidgetZ->SetInteractor(this->ui->qvtkWidgetLeft->GetInteractor());
    sliderWidgetZ->SetRepresentation(sliderRepZ);
    sliderWidgetZ->SetAnimationModeToAnimate();
    sliderWidgetZ->EnabledOn();

}






/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///                ARBITRARY SLICE
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void MainWindow::beginSliceArb()
{
    on_actionReset_Camera_triggered();

    vtkPolyData * polyPlane = vtkPolyData::New();

    double range[2] ;
    global_Reader->Update();
    global_Reader->GetOutput()->GetScalarRange(range);


    //QString("%1").arg(slice)
    //this->ui->lineArbSliceRangeMin->setText(QString("%1").arg(range[0]));
    //this->ui->lineArbSliceRangeMax->setText(QString("%1").arg(range[1]));

    ///
    /// \brief vtkPlaneWidget
    ///
    customArbPlaneWidget = vtkImplicitCustomPlaneWidget::New();
    //customPlaneWidget = vtkSmartPointer<vtkImplicitCustomPlaneWidget>::New();
    customArbPlaneWidget->SetInput(global_Reader->GetOutput());
    customArbPlaneWidget->NormalToZAxisOn();
    customArbPlaneWidget->SetPlaceFactor(1.0);
    customArbPlaneWidget->GetPolyData(polyPlane);
    customArbPlaneWidget->SetOrigin(1, 1, 1);
    customArbPlaneWidget->TubingOn();
    customArbPlaneWidget->PlaceWidget();
    customArbPlaneWidget->DrawPlaneOff();
    customArbPlaneWidget->SetScaleEnabled(false);
    customArbPlaneWidget->SetOriginTranslation(false);

//    customArbPlaneWidget->GetSelectedNormalProperty()->SetColor(0,1,0);
//    customArbPlaneWidget->GetEdgesProperty()->SetAmbient(0);
//    customArbPlaneWidget->GetPlaneProperty()->SetAmbient(0);

    //customArbPlaneWidget->
    //customArbPlaneWidget->

    customArbPlaneWidget->SetInteractor(this->ui->qvtkWidgetLeft->GetInteractor());

    vtkCutter * cutter = vtkCutter::New();
    cutter->SetInput(global_Reader->GetOutput());

    vtkPlane * plane = vtkPlane::New();
    customArbPlaneWidget->GetPlane(plane);
    cutter->SetCutFunction(plane);

    /// Map the slice from the plane and create the geometry to be rendered
    ///

    vtkPolyDataMapper * slice = vtkPolyDataMapper::New();
    slice->SetInput(cutter->GetOutput());
    slice->Update();
    slice->SetScalarRange(global_Reader->GetOutput()->GetScalarRange());

    /// Add a Colour Lookup Table
    vtkSmartPointer<vtkLookupTable> lut3D =
            vtkSmartPointer<vtkLookupTable>::New();
    //lut3D->SetTableRange(0,1000);
    lut3D->SetTableRange(global_Reader->GetDataMin(), global_Reader->GetDataMax());
    lut3D->SetSaturationRange(0,1);
    lut3D->SetHueRange(0,1);
    lut3D->SetValueRange(0,1);
    lut3D->SetAlphaRange(0,1);
    lut3D->SetVectorComponent(3);
    lut3D->Build();


    slice->SetLookupTable(lut3D);

    /// Assign the LookupTable to the Global Pointer
    lookupTable3D = lut3D;

    //lut3D->Print(std::cout);

    /// Actor representing Volume Slice
    vtkActor * sliceActor = vtkActor::New();
    sliceActor->SetMapper(slice);


    vtkRenderer * renderer = this->defaultRenderer;

    renderer->AddActor(sliceActor);

    /////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    ////
    /// \brief Code for the 2D Side Window Contour Viewer
    ///


    ///
    /// Create Camera for the Side Window Panel
    ///

    double *bounds = global_Volume->GetBounds();
    double zoomScale = (bounds[1]-bounds[2]) / 1.5;

    vtkSmartPointer<vtkCamera> camera2D = vtkSmartPointer<vtkCamera>::New();
    camera2D->ParallelProjectionOn();

    camera2D->SetParallelScale(zoomScale);


    vtkSmartPointer<vtkContourFilter> contours2D = vtkSmartPointer<vtkContourFilter>::New();
    contours2D->SetInput(cutter->GetOutput());
    contours2D->GenerateValues(10, global_Reader->GetOutput()->GetScalarRange());
    global_Contours2D = contours2D;
    //contours2D->SetNumberOfContours(10);



    /// Add a Colour Lookup Table
    vtkSmartPointer<vtkLookupTable> lut2D =
            vtkSmartPointer<vtkLookupTable>::New();
    lut2D = BuildHueIntensityBaseMap(range[0], range[1]);


    vtkSmartPointer<vtkPolyDataMapper> contour2DMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    contour2DMapper->SetInput(contours2D->GetOutput());
    contour2DMapper->SetLookupTable(lut2D);
    ///contourMapper->SetInputConnection(probe->GetOutputPort());
    contour2DMapper->SetScalarRange(global_Reader->GetOutput()->GetScalarRange());

    lookupTable2D = lut2D;

    /// Will give the iso-Lines a little thickness for better viewing

    vtkProperty * line2DWidth = vtkProperty::New();
    line2DWidth->SetLineWidth(1.5);
    //line2DWidth->SetEdgeVisibility(1);

    vtkSmartPointer<vtkActor> contour2DActor =
            vtkSmartPointer<vtkActor>::New();
    contour2DActor->SetMapper(contour2DMapper);
    contour2DActor->SetProperty(line2DWidth);

    /// Create seperate Renderer for the 2D Side Window
    vtkSmartPointer<vtkRenderer> ren2D =
            vtkSmartPointer<vtkRenderer>::New();
    //ren2D->SetBackground(0.3, 0.3, 0.2);
    ren2D->SetViewport(0.0, 0.0, 1.0, 1.0);
    ren2D->SetActiveCamera(camera2D);
    //ren2D->InteractiveOff();

    /// Create Seperate RenderWindow for the 2D side window
    vtkSmartPointer<vtkRenderWindow> renWindow2D =
            vtkSmartPointer<vtkRenderWindow>::New();

    /// Add the 2nd Renderer to the 2D RenderWindow
    renWindow2D->AddRenderer(ren2D);

    /// Add the 2nd RenderWindow to the 2D sidewindow
    this->ui->widget_ArbPlanePreview->SetRenderWindow(renWindow2D);

    /// Add the 2D actor to the 2D Renderer
    ren2D->AddActor(contour2DActor);

    /// Handle the events

    vtkSmartPointer<vtkArbSliceCallback> ProbeData =
            vtkSmartPointer<vtkArbSliceCallback>::New();
    ProbeData->polyPlane = polyPlane;
    ProbeData->plane = plane;
    ProbeData->cutter = cutter;
    ProbeData->camera = camera2D;
    ProbeData->renderWindow = this->ui->widget_ArbPlanePreview->GetRenderWindow();
    ProbeData->ui = this->ui;


    customArbPlaneWidget->AddObserver(vtkCommand::EnableEvent, ProbeData);
    customArbPlaneWidget->AddObserver(vtkCommand::StartInteractionEvent, ProbeData);
    customArbPlaneWidget->AddObserver(vtkCommand::InteractionEvent, ProbeData);

    customArbPlaneWidget->SetOrigin(global_Volume->GetCenter());
    customArbPlaneWidget->PlaceWidget(global_Volume->GetBounds());

    customArbPlaneWidget->OutlineTranslationOff();

    customArbPlaneWidget->EnabledOn();

    this->ui->widget_ArbPlanePreview->setDisabled(true);

    if(this->userTestRunning() )
    {
        vtkEventQtSlotConnect * vtkEventConnector = vtkEventQtSlotConnect::New();
        vtkEventConnector->Connect(customArbPlaneWidget,vtkCommand::StartInteractionEvent, userTestDlg, SLOT(incSliceReSize()));
    }

}

void MainWindow::on_buttonArbReset_clicked()
{
    if(this->userTestRunning())
        this->countInteraction(SliceResetCount);

    customArbPlaneWidget->PlaceWidget(global_Volume->GetBounds());
    customArbPlaneWidget->SetOrigin(global_Volume->GetCenter());

    customArbPlaneWidget->InvokeEvent(vtkCommand::EnableEvent);
    customArbPlaneWidget->InvokeEvent(vtkCommand::StartInteractionEvent);
    customArbPlaneWidget->InvokeEvent(vtkCommand::InteractionEvent);



    this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->Render();



}


vtkLookupTable* MainWindow::BuildHueIntensityBaseMap(double min, double max)
{
    double weight, del_weight;
    double min_weight = min;
    double max_weight = max;
    int n_steps = 16;
    weight = min_weight;
    del_weight = (max_weight-min_weight) / n_steps;

    vtkLookupTable *lut = vtkLookupTable::New();
    lut->SetNumberOfTableValues( 256 );

    int i;
    int it = 0;
    double color[4];
    color[3] = 1.0;  //  Opacity
    for( i = 0; i < 64; i++ )
    {
        color[0] = 0.0;                        /* red = 0 */
        color[1] = weight * (float) i / 63.0;  /* green ramps up */
        color[2] = weight;                     /* blue = 1 */
        lut->SetTableValue( it++, color );
        if( i % n_steps == 0 )
            weight += del_weight;
    }
    for( ; i < 128; i++ )
    {
        color[0] = 0.0;                                      /* red = 0 */
        color[1] = weight;                                   /* green = 1 */
        color[2] = weight * (1.0 - (float) (i-64) / 63.0 );  /* blue ramps down */
        lut->SetTableValue( it++, color );
        if( i % n_steps == 0 )
            weight += del_weight;
    }
    for( ; i < 192; i++ )
    {
        color[0] = weight * (float) (i-128) / 63.0;  /* red ramps up */
        color[1] = weight;                           /* green = 1 */
        color[2] = 0.0;                              /* blue = 0 */
        lut->SetTableValue( it++, color );
        if( i % n_steps == 0 )
            weight += del_weight;
    }
    for( ; i < 255; i++ )
    {
        color[0] = weight;                                   /* red = 1 */
        color[1] = weight * (1.0 - (float) (i-192) / 63.0);  /* green ramps down */
        color[2] = 0.0;                                      /* blue = 0 */
        lut->SetTableValue( it++, color );
        if( i % n_steps == 0 )
            weight += del_weight;
    }
    color[0] = 1.0;  /* red = 1 */
    color[1] = 0.0;  /* green = 0 */
    color[2] = 0.0;  /* blue = 0 */
    lut->SetTableValue( it, color );

    return lut;
}

void MainWindow::on_checkArbSliceVolumeEnable_stateChanged(int arg1)
{
    global_Volume->SetVisibility(arg1);
}

void MainWindow::on_slider_CntourDisplay_valueChanged(int value)
{
    global_Contours2D->SetNumberOfContours(value);
    global_Contours2D->GenerateValues(value, global_Reader->GetOutput()->GetScalarRange());
    this->ui->widget_ArbPlanePreview->GetRenderWindow()->Render();

    this->ui->lineArbSliceContours->setText(QString("%1").arg(value));
}

void MainWindow::on_buttonArbSliceContourBackground_clicked()
{
    QPalette palette = this->ui->lineArbSLiceContourColour->palette();

    QColor selected  = QColorDialog::getColor(palette.color(this->ui->lineArbSLiceContourColour->backgroundRole()), this);

    palette.setColor(this->ui->lineArbSLiceContourColour->backgroundRole(),selected);
    this->ui->lineArbSLiceContourColour->setPalette(palette);
    this->ui->lineArbSLiceContourColour->setAutoFillBackground(true);

    double renderBackGround[3] ;

    renderBackGround[0] = double(selected.red())/256;
    renderBackGround[1] = double(selected.green())/256;
    renderBackGround[2] = double(selected.blue())/256;


    this->ui->widget_ArbPlanePreview->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->SetBackground(renderBackGround);

    this->ui->widget_ArbPlanePreview->GetRenderWindow()->Render();
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///                LEAP MOTION
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void MainWindow::leapTrackingOn(bool arg1)
{
    if (this->systemMode == Leap)
    {
        int * screenSize = this->ui->qvtkWidgetLeft->GetRenderWindow()->GetSize();

//        std::cout << "screen size [x,y]: " << screenSize[0] << ", " << screenSize[1] << endl;

        this->leapTrackingActor->SetPosition(screenSize[0] / 2 - 100, screenSize[1] - 50);

        this->leapTrackingActor->SetVisibility(arg1);
    }
}

void MainWindow::on_actionLeapBasic_triggered()
{
    this->controller_= new Controller;
    Leaping_ = true;


    setLeapInteractor();

    this->leapMatrixTotalMotionRotation = Leap::Matrix::identity();
    this->leapVectorTotalMotionalTranslation = Leap::Vector::zero();
    this->leapFloatTotalMotionScale = 1.0f;

    this->ui->qvtkWidgetLeft->setFocus();
}


void MainWindow::LeapMotion()
{
    QFont boldFont, normalFont;

    boldFont.setBold(true);
    normalFont.setBold(false);

    /// IMPORTANT: Visualizer Window must be RENDERED B4 colour notification otherwise it will only stick 2 1 colour
    ///
    if(this->ui->actionLeapDialogToggle->isChecked())
    {

        this->leapMarkerWidget->leapDbgSphereActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
        this->leapMarkerWidget->leapDbgPointWidget->GetProperty()->SetColor(1.0, 1.0, 1.0);
        static_cast<vtkSliderRepresentation3D*>( this->leapMarkerWidget->leapDbgSliderWidget->GetRepresentation())->SetValue(this->leapMarkerWidget->scaling_Start);
        static_cast<vtkSliderRepresentation3D*>( this->leapMarkerWidget->leapDbgSliderWidget->GetRepresentation())->GetTubeProperty()->SetColor(1,1,1);

        this->leapMarkerWidget->On();
    }
    else
    {
        this->leapMarkerWidget->Off();
    }

    if(controller_->isConnected())  // controller is a controller object
    {
        // Get the most recent frame and report some basic information
        const Frame frame = controller_->frame();
        //const FingerList fingers = controller_->frame().fingers();

        //controller_->enableGesture(Gesture::TYPE_SCREEN_TAP, true);
        //controller_->enableGesture(Gesture::TYPE_KEY_TAP, true);
        controller_->enableGesture(Gesture::TYPE_CIRCLE, true);

        controller_->config().setFloat("Gesture.Circle.MinRadius", 15.0);
        controller_->config().setFloat("Gesture.Circle.MinArc", .5);
        controller_->config().save();

        controller_->config().setFloat("Gesture.KeyTap.MinDownVelocity", 40.0);
        controller_->config().setFloat("Gesture.KeyTap.HistorySeconds", .2);
        controller_->config().setFloat("Gesture.KeyTap.MinDistance", 10.0);
        controller_->config().save();

        bool shouldSubVol = this->leapSubVolMode;
        bool shouldAxisSlice =  this->leapAxisSliceMode;
        bool shouldArbSlice = this->leapArbSliceMode;

        int activeFrameCount = 0;

        int gestureState ;

        bool rotateMovement = false;
        bool translateMovement = false;
        bool scaleMovement = false;
        bool subVolLeftHand = false;
        bool subVolRightHand = false;
        bool sliceMovement = false;


        //// --  MOVED THE CODE TO INSDE HAND LOOP TO SELECT PROBABILITIES BASED ON HAND MOVEMENT
        ///bool chkTranslate = this->ui->checkBox_Translation->isChecked();
        ///bool chkRotate = this->ui->checkBox_Rotation->isChecked();
        ///bool chkScale = this->ui->checkBox_Scaling->isChecked();
        ///
        bool chkTranslate =false;
        bool chkRotate =false;
        bool chkScale = false;

        //        bool chkTranslate = this->ui->checkBoxLeapTracking->isChecked() && this->ui->checkBox_Translation_2->isChecked();
        //        bool chkRotate = this->ui->checkBoxLeapTracking->isChecked() && this->ui->checkBox_Rotation_2->isChecked();
        //        bool chkScale = this->ui->checkBoxLeapTracking->isChecked() && this->ui->checkBox_Scaling_2->isChecked();




        this->leapTrackingActor->SetVisibility(this->ui->checkBoxLeapTracking->isChecked());

        vtkRenderer * renderer = this->defaultRenderer;
        vtkCamera *camera = renderer->GetActiveCamera();


        if (shouldAxisSlice)
        {

        }

        /// IF Rotation Gesture is detected, we set the leapMovement Flag to True
        /// If the LeapMovement is already True, then  NO new gesture was detected
        ///
//        if(chkRotate)
//        {

//        }

        if (shouldArbSlice)
        {
            this->customArbPlaneWidget->leapMovingNormal(false);
            this->customArbPlaneWidget->leapMovingPlane(false);
        }

        if (shouldSubVol)
        {
            vtkProperty * pointerProperty =
                    pointWidget1_->GetProperty();

            //if pointerProperty->GetColor()

            pointerProperty->SetColor(1, 1,1 );

            pointerProperty = pointWidget2_->GetProperty();

            pointerProperty->SetColor(1,1,1);

            this->ui->Frame_SubVolLeapTracking->setVisible(true);

            this->ui->checkbx_SubVolLeapLeftHand->setChecked(false);
            this->ui->checkbx_SubVolLeapLeftGrab->setChecked(false);
            this->ui->checkbx_SubVolLeapLeftPinch->setChecked(false);
            this->ui->checkbx_SubVolLeapRightHand->setChecked(false);
            this->ui->checkbx_SubVolLeapRightGrab->setChecked(false);
            this->ui->checkbx_SubVolLeapRightPinch->setChecked(false);

        }


//        if (!frame.hands().isEmpty() && !frame.hands()[0].fingers().isEmpty())
         if (!frame.hands().isEmpty())
        {
            Hand rightHand, leftHand;

            if (frame.hands().count() ==2)
            {
                if (frame.hands()[0].isRight())
                {
                    rightHand = frame.hands()[0];
                    leftHand = frame.hands()[1];
                }
                else
                {
                    leftHand = frame.hands()[0];
                    rightHand = frame.hands()[1];
                }
            }
            else
            {
                if (frame.hands()[0].isRight())
                    rightHand = frame.hands()[0];
                 else
                    leftHand = frame.hands()[0];
            }



///            std::cout << std::fixed << std::setprecision(2) ;
///            std::cout << "Hands.Count(): " << frame.hands().count()
///                                //<< "\tRight hand Valid: " << rightHand.isValid()
///                                 << "\tRight hand isRight: " << rightHand.isRight()
///                                  << "\t";

//            if (frame.gestures().count() > 0)
//            {
//                if (frame.gestures()[0].type() == Gesture::TYPE_SCREEN_TAP)
//                    std::cout << "SCREEN TAP: " << frame.gestures()[0].id() << endl ;

//                if (frame.gestures()[0].type() == Gesture::TYPE_KEY_TAP)
//                {
//                    std::cout << "KEY TAP: " << frame.gestures()[0].id() << endl ;
//                     (this->ui->buttonTransformActive->setChecked
//                            (!this->ui->buttonTransformActive->isChecked()));
//                }

//                if (frame.gestures()[0].type() == Gesture::TYPE_CIRCLE)
//                {
//                    std::cout << "CIRCLE: " << frame.gestures()[0].id() << endl ;

//                }

//            }
//            if (frame.gestures().count() > 0)
//                std::cout << "Gestures Found: " << frame.gestures().count()            << endl;

            std::cout << "leftID: " << leftHand.id() << "\trightID: " << rightHand.id() << endl;

          GestureList gestures = leftHand.frame().gestures();
            //GestureList gestures = rightHand.frame().gestures();

            for (GestureList::const_iterator gesture = gestures.begin(); gesture != leftHand.frame().gestures().end(); gesture++)
            {
                    switch ((*gesture).type())
                    {
                        case Gesture::TYPE_CIRCLE:
                    {
                        CircleGesture circle = CircleGesture(*gesture);
                        std::cout << "radius: " << circle.radius() << "\t HandIDL: " << leftHand.id() << endl;

                        if (circle.radius() > 40)
                        {

                            if((*gesture).state() == Gesture::STATE_STOP)
                            {
                                std::cout << "CIRCLE : STOP " << (*gesture).id() << endl ;

                            }
                            if((*gesture).state() == Gesture::STATE_START)
                            {
                                if (systemTab == SubVolume)
                                    this->on_buttonSubVolReset_clicked();

                                if(systemTab == SliceArb)
                                    this->on_buttonArbReset_clicked();

                                if (systemTab == Information)
                                    this->on_buttonCameraReset_clicked();

                                std::cout << "CIRCLE : BEGIN" << (*gesture).id() << endl ;

                            }   /// if(*gesture).state)
                        }
                            else
                            {
                                    //std::cout << "CIRCLE : SOMETHING " << (*gesture).id() << endl ;
                            }
                           break;
                        }
                    case Gesture::TYPE_KEY_TAP:
                        {
                            std::cout << "KEY TAP: " << (*gesture).id() << endl ;
                                (this->ui->buttonTransformActive->setChecked
                                (!this->ui->buttonTransformActive->isChecked()));
                            break;
                        }
                    case Gesture::TYPE_SCREEN_TAP:
                    {
                        std::cout  << "SCREEN TAP: " << (*gesture).id()
                                            << "\t State: " << (*gesture).state() << endl ;
                        (this->ui->buttonTransformActive->setChecked
                        (!this->ui->buttonTransformActive->isChecked()));
                        break;
                    }
                    default:
                        std::cout << "INVALID GESTURE" << endl;
                            break;
                    }
            }


//            if (frame.gestures(controller_->frame(1)).count() > 0)
//            {
//                if (frame.gestures()[0].type() == Gesture::TYPE_SCREEN_TAP)
//                    std::cout << "SCREEN TAP: " << frame.gestures()[0].id() << endl ;

//                if (frame.gestures()[0].type() == Gesture::TYPE_KEY_TAP)
//                {
//                     Gesture keyTap = frame.gestures()[0];

//                    // if (keyTap.state() == Gesture::STATE_START)
//                  //   {
//                        std::cout << "KEY TAP: " << frame.gestures()[0].id() << endl ;
//                        (this->ui->buttonTransformActive->setChecked
//                            (!this->ui->buttonTransformActive->isChecked()));
//                  //   }
//                }

//                if (frame.gestures()[0].type() == Gesture::TYPE_CIRCLE)
//                {
//                    Gesture circle = frame.gestures()[0];

//                    if (circle.state() == Gesture::STATE_START)
//                        std::cout << "CIRCLE: " << frame.gestures()[0].id() << endl ;

//                }

//            }

            bool checkForBigTap = true;



            if(checkForBigTap)
            {
                //Vector motionSinceLastFrame = frame.translation(controller_->frame(1));

                Vector motionSinceLastFrame = leftHand.translation(controller_->frame(1));

//                if(motionSinceLastFrame.x > gestureRange)
//                {
//                    framesRightMoving++;
//                }
//                else
//                {
//                    framesRightMoving = 0;
//                }


//                 if(motionSinceLastFrame.x  < - gestureRange)
//                 {
//                     framesLeftMoving++;
//                 }
//                 else
//                 {
//                     framesLeftMoving = 0;
//                 }

//                 if(motionSinceLastFrame.y  > gestureRange)
//                 {
//                     framesUpMoving++;
//                 }
//                 else
//                 {
//                     framesUpMoving = 0;
//                 }

                 if(motionSinceLastFrame.y < - gestureRange)
                 {
                     framesDownMoving++;
                 }
                 else
                 {
                     framesDownMoving = 0;
                 }

//                 std::cout << "right: " << framesRightMoving << "\t left:" << framesLeftMoving << endl;

//                 if (framesRightMoving >= frameActionCount)
//                 {
//                     std::cout << "Right hand Swipe Detected" << endl;
//                 }

//                 if(framesLeftMoving >= frameActionCount)
//                 {
//                     std::cout << "Left hand Swipe Detected" << endl;
//                 }

//                 if (framesUpMoving >= frameActionCount)
//                 {
//                     std::cout << "Up hand Swipe Detected" << endl;
//                 }

                 if(framesDownMoving >= frameActionCount)
                 {
                     std::cout << "Tap Detected" << endl;
                     this->ui->buttonTransformActive->setChecked
                                                 (!this->ui->buttonTransformActive->isChecked());
                     framesDownMoving=0;

                 }

            }



            if (leftHand.grabStrength() == 1 )
            {
                leapHandTriggerBuffer++;
                if (leapHandTriggerBuffer == 50)
                {
                    std::cout << "leapHandTrigger" << endl;
                    leapHandTriggerBuffer = 0;
                    //this->ui->checkBoxLeapTracking->toggle();

                }
            }

///            std::cout << "Grab Strength:" << leftHand.grabStrength()
///                               << "\t Pinch Strength: " << leftHand.pinchStrength()
///                               <<  endl;
///                std::cout << endl;

            bool grabLeft = false;


//            ///   USE GRABSTRENGTH TOGGLE
//            if (leftHand.grabStrength() == 1)
//                 grabLeft = true;

//             /// USE PINCHSTRENGTH TOGGLE
//            if (leftHand.pinchStrength() == 1)
//                 grabLeft = true;

            /// USE FINGERS TOGGLE
           if (leftHand.fingers().extended().count() == 0 && leftHand.isLeft())
           {
               this->leapTrackingOn(true);
                grabLeft = true;
           }
           else
           {
                 if (!this->ui->checkBoxLeapTracking->isChecked())
                     this->leapTrackingOn(false);
           }




            ///bool useLeftHand = true  ;
            bool useLeftHand = grabLeft;

            if (rightHand.isValid())
            {
                chkRotate = (rightHand.rotationProbability(controller_->frame(1)) > 0.6);
                chkTranslate = (rightHand.translationProbability(controller_->frame(1)) > 0.6);
                chkScale = (rightHand.scaleProbability(controller_->frame(1)) > 0.6);
            }


//                std::cout  << "Pitch: " <<  vtkMath::DegreesFromRadians(rightHand.direction().pitch()) << "\t"
//                                    << "Yaw: " <<    vtkMath::DegreesFromRadians(rightHand.direction().yaw()) << "\t"
//                                     << "Roll: " <<    vtkMath::DegreesFromRadians(rightHand.direction().roll()) << "\t"
//                                      << endl;
 //           if (vtkMath::DegreesFromRadians(rightHand.direction().roll())  > 150 )
//                    &&
//                    vtkMath::DegreesFromRadians(rightHand.direction().picht())  > -150
//            {
//                std::cout << "calculating " << endl;
//            }


            Hand hand = frame.hands().rightmost();                

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////  AXIS  SLICE TRACKING  /// //////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            if((shouldAxisSlice) && (frame.hands().count() ==1))
            {

                Finger rightThumb = (frame.hands().rightmost().fingers().fingerType(Finger::TYPE_THUMB))[0];
                Finger rightIndex = (frame.hands().rightmost().fingers().fingerType(Finger::TYPE_INDEX))[0];

                //leapMovement = false;

                std::cout << " 1. Movement = " << leapHand1Move << "\t";

                if(frame.hands().rightmost().fingers().frontmost().id() == rightIndex.id() &&
                        rightThumb.isExtended())
                {
                    double moveFactor = 2.0;

                    Vector newPosition = frame.translation(controller_->frame(1));
                    ///
                    ///Translate the X Axis Left and Right  (RED)
                    ///

                    if (this->ui->checkBox_SliceX_Enable->isChecked())
                    {
                        leapHand1Move = true;

                        int changeX = (int) (newPosition.x * moveFactor);

                        int currentPosX = (static_cast<vtkSliderRepresentation *>
                                           (sliderWidgetX->GetRepresentation()))->GetValue();

                        currentPosX += changeX;

                        (static_cast<vtkSliderRepresentation *>
                                (sliderWidgetX->GetRepresentation()))->SetValue(currentPosX);

                        std::cout << " 2. Movement = " << leapHand1Move << "\t";
                    }

                    ///
                    ///Translate the Y Axis Up and Down     (GREEN)
                    ///

                    if (this->ui->checkBox_SliceY_Enable->isChecked())
                    {
                        leapHand1Move = true;

                        int changeY = (int) (newPosition.y * moveFactor);

                        int currentPosY = (static_cast<vtkSliderRepresentation *>
                                           (sliderWidgetY->GetRepresentation()))->GetValue();

                        currentPosY += changeY;

                        (static_cast<vtkSliderRepresentation *>
                                (sliderWidgetY->GetRepresentation()))->SetValue(currentPosY);

                        std::cout << " 3. Movement = " << leapHand1Move << "\t";
                    }


                    ///
                    ///Translate the Z Axis Forward and Backward    (BLUE)
                    ///

                    if (this->ui->checkBox_SliceZ_Enable->isChecked())
                    {
                        leapHand1Move = true;


                        int changeZ = (int) (newPosition.z * moveFactor);

                        int currentPosZ = (static_cast<vtkSliderRepresentation *>
                                           (sliderWidgetZ->GetRepresentation()))->GetValue();

                        currentPosZ += changeZ;

                        (static_cast<vtkSliderRepresentation *>
                                (sliderWidgetZ->GetRepresentation()))->SetValue(currentPosZ);

                        std::cout << " 4. Movement = " << leapHand1Move << "\t";

                    }

                    std::cout << " 5. Movement = " << leapHand1Move << "\t";
                }

                std::cout << " 6. Movement = " << leapHand1Move << "\t";

                if(frame.hands().rightmost().fingers().frontmost().id() == rightIndex.id() &&
                        !rightThumb.isExtended() && leapHand1Move)
                {

                    std::cout << " 7. Movement = " << leapHand1Move << "\t";

                    if (this->ui->checkBox_SliceX_Enable->isChecked())
                        this->UpdateSliceX();

                    if (this->ui->checkBox_SliceY_Enable->isChecked())
                        this->UpdateSliceY();

                    if (this->ui->checkBox_SliceZ_Enable->isChecked())
                        this->UpdateSliceZ();

                    leapHand1Move = false;
                }

                std::cout << " 8. Movement = " << leapHand1Move << "\t";

                std::cout << endl;

            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////  ARB  SLICE TRACKING  /// //////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            if((shouldArbSlice) && (rightHand.isValid()) && !this->ui->buttonTransformActive->isChecked())
            {
                leapArbSliceUpdate( frame, rightHand, sliceMovement);
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    SUB-VOL TRACKING  /// //////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            if((shouldSubVol) &&  frame.hands().count() >= 1 && !this->ui->buttonTransformActive->isChecked())
            {
               leapSubVolumeUpdate(frame, hand, subVolRightHand, subVolLeftHand);
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    TRANSLATION   //////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            if(chkTranslate && (this->ui->checkBoxLeapTracking->isChecked()  || useLeftHand ) && rightHand.isValid() && this->ui->buttonTransformActive->isChecked())
            {                
                leapTranslateUpdate(frame, translateMovement, rightHand);
            }


            //////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    ROTATION   /////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////
            if(chkRotate && (this->ui->checkBoxLeapTracking->isChecked()  || useLeftHand ) && rightHand.isValid() && this->ui->buttonTransformActive->isChecked() )
            {
                leapRotateUpdate(frame, rotateMovement,rightHand);

            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    SCALING       //////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            if(chkScale && (this->ui->checkBoxLeapTracking->isChecked()  || useLeftHand ) && rightHand.isValid()  && this->ui->buttonTransformActive->isChecked())
            {
                leapScaleUpdate(frame, scaleMovement, rightHand);
            }


            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    HANDMODELLER       //////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            if (this->handModelActive)
            {
                leapHandModelUpdate(frame);
            }



            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    VTKLEAPHANDMODELLER       //////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            if (this->ui->actionLeapDialogToggle->isChecked() && leapHandWidget->GetEnabled())
            {
                leapHandWidgetUpdate(frame, hand);
            }


            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    DIAGNOSTIC TRACKING  /// //////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////
            ///

            if (this->ui->actionLeapDialogToggle->isChecked() && rightHand.isValid())
            {

                leapDiagnosticUpdate(frame, rightHand);
            }
        } // (!frame.hands().isEmpty() && !frame.hands()[0].fingers().isEmpty())

        //////////////////////////////////////////////////
        ///// USER TEST PRINTOUT
        if(this->userTestRunning())
        {

            /////// SECOND HAND OPERATION

            /// If  No Movement =  Decrease the Action Counter
            /// && Decrease the Gesture Counter
            /// If  Yes Movement = Increase the Gesture Counter
            if(!leapHand2Move)
            {
                leapHand2FrameBuffer--;
                leapHand2GestureCounter = 0;
            }
            else
                leapHand2GestureCounter++;

            /// If Action Counter is > 5 = Set Action Counter MAX to 4 (Buffer)
            /// && Gesture Counter Reset to ZERO
            if (leapHand2FrameBuffer > 5)
            {
                leapHand2FrameBuffer = 5;
                leapHand2GestureCounter = 0;
            }

            ///If Action Counter is < 1  =  Set Action Counter MIN to 0 (Buffer)
            if(leapHand2FrameBuffer < 1)
            {
                leapHand2FrameBuffer = 0;
                leapHand2GestureCounter = 0;
            }

            /// If Gesture Counter is 3 = WE have Detected a New Gesture

            if (leapHand2FrameBuffer > 0 || leapHand2Move)
            {
                //            std::cout   << "Frame ID2: " << frame.id()  << ", "
                //                        << "Action2: " << leapHand2FrameBuffer <<", "
                //                        << "Bool2: " << leapHand2Move << ","
                //                        << "Gestures2: " << leapHand2GestureCounter << ","
                //                           ;

                if (leapHand2GestureCounter == 3)
                {
                    if(rotateMovement)
                        //std::cout << "\t Rotated" ;
                        userTestDlg->incRotation();
                    if(translateMovement)
                        //std::cout << "\t Translated" ;
                        userTestDlg->incTranslation();
                    if(scaleMovement)
                        //std::cout << "\t Scaled" ;
                        userTestDlg->incScaling();
                    if(subVolLeftHand)
                        //std::cout << "\t SubVolLeft" ;
                        userTestDlg->incSubVolPoint1();
                }
                //std::cout << endl;
            }

            /// Reset Movement Flag to False after All Possible Actions are DONE
            leapHand2Move = false;

            /// If  No Movement =  Decrease the Action Counter
            /// && Decrease the Gesture Counter
            /// If  Yes Movement = Increase the Gesture Counter
            if (!leapHand1Move)
            {
                leapHand1FrameBuffer--;
                leapHand1GestureCounter = 0;
            }
            else
                leapHand1GestureCounter++;

            /// If Action Counter is > 5 = Set Action Counter MAX to 4 (Buffer)
            /// && Gesture Counter Reset to ZERO
            if (leapHand1FrameBuffer > 5)
            {
                leapHand1FrameBuffer = 5;
                leapHand1GestureCounter = 0;
            }

            ///If Action Counter is < 1  =  Set Action Counter MIN to 0 (Buffer)
            if(leapHand1FrameBuffer < 1)
            {
                leapHand1FrameBuffer = 0;
                leapHand1GestureCounter = 0;
            }

            /// If Gesture Counter is 3 = WE have Detected a New Gesture

            if (leapHand1FrameBuffer > 0 || leapHand1Move)
            {

                //                            std::cout   << "Frame ID: " << frame.id()  << ", "
                //                                        << "Action: " << leapHand1FrameBuffer <<", "
                //                                        << "Bool: " << leapHand1Move << ","
                //                                        << "Gestures: " << leapHand1GestureCounter << ","
                ;

                if (leapHand1GestureCounter == 3)
                {
                    if(rotateMovement)
                        //std::cout << "\t Rotated" ;
                        userTestDlg->incRotation();
                    if(translateMovement)
                        //std::cout << "\t Translated" ;
                        userTestDlg->incTranslation();
                    if(scaleMovement)
                        //std::cout << "\t Scaled" ;
                        userTestDlg->incScaling();
                    if(subVolRightHand)
                        //std::cout << "\t SubVolRight" ;
                        userTestDlg->incSubVolPoint2();
                    if(sliceMovement)
                        //std::cout << "\t SliceMovement";
                        userTestDlg->incSliceReSize();
                }
                std::cout << endl;
            }

            /// Reset Movement Flag to False after All Possible Actions are DONE
            leapHand1Move = false;
        }//     if(userTestRunning)

    } //    if(controller_->isConnected())  // controller is a controller object
}   //void MainWindow::LeapMotion()

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///                TEST SECTION
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void MainWindow::on_actionWorldCoords_triggered()
{

    //    AddDistanceMeasurementToView1();
    //    // remove existing widgets.
    //    if (this->lineWidget[0])
    //    {
    //        this->lineWidget[0]->SetEnabled(0);
    //        this->lineWidget[0] = NULL;
    //    }

    //    // add new widget
    //    this->lineWidget[0] = vtkSmartPointer<vtkLineWidget>::New();
    //    this->lineWidget[0]->SetInteractor(
    //                this->riw[0]->GetResliceCursorWidget()->GetInteractor());

    //    // Set a priority higher than our reslice cursor widget
    //    this->lineWidget[0]->SetPriority(
    //                this->riw[0]->GetResliceCursorWidget()->GetPriority() + 0.01);

    //    vtkSmartPointer<vtkLineCallback> lineCallback =
    //            vtkSmartPointer<vtkLineCallback>::New();
    //    lineCallback->riw = riw[0];

    //    lineWidget[0]->AddObserver(vtkCommand::InteractionEvent, lineCallback);


    //    vtkSmartPointer<vtkLineRepresentation> lineRepresentation =
    //          vtkSmartPointer<vtkLineRepresentation>::New();


    //    double pos1 [3] = {115, 31.87, 13.05};
    //    double pos2 [3] = {115, 31.87, 40.55};
    //    lineRepresentation->SetPoint1DisplayPosition(pos1);
    //    lineRepresentation->SetPoint2DisplayPosition(pos2);

    //    lineWidget[0]->SetPoint1(pos1);
    //    lineWidget[0]->SetPoint2(pos2);

    //    lineWidget[0]->On();


    //    riw[0]->Render();


    //    vtkSmartPointer<MouseInteractorStyle> style =
    //            vtkSmartPointer<MouseInteractorStyle>::New();
    //    style->SetDefaultRenderer(this->riw[0]->GetResliceCursorWidget()->GetCurrentRenderer());
    //    this->riw[0]->GetInteractor()->SetInteractorStyle(style);
    //    this->riw[0]->GetResliceCursorWidget()->GetInteractor()->SetInteractorStyle(style);
    ////    //this->ui->sliceView1->update();
    ////    riw[0]->GetInteractor()->SetInteractorStyle(style);
    ///
    ///
    ///

    vtkSmartPointer<vtkCameraScaleCallback> cameraObserver =
            vtkSmartPointer<vtkCameraScaleCallback>::New();
    vtkCamera* thisCamera = this->ui->qvtkWidgetLeft->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();

    cameraObserver->ui = this->ui;
    cameraObserver->defualtCameraDistance = this->defaultCameraDistance;


    thisCamera->AddObserver(vtkCommand::ModifiedEvent, cameraObserver);

}



void MainWindow::closeTab(int index)
{
    this->ui->tabLogWidget->removeTab(index);
}

void MainWindow::resetTransformCoords()
{
    vtkCamera* camera = this->ui->qvtkWidgetLeft->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();
    ///
    /// \brief Rotation
    ///
    double* orientation;

    orientation  = camera->GetOrientation();

    ui->line_OrientX->setText(QString::number(orientation[0], 'f', 0));
    ui->line_OrientY->setText(QString::number(orientation[1], 'f', 0));
    ui->line_OrientZ->setText(QString::number(orientation[2], 'f', 0));

    ///
    /// \brief Translation
    ///
//    double* position;

//    position = camera->GetPosition();

//    ui->line_PosX->setText(QString::number(position[0], 'f', 0));
//    ui->line_PosY->setText(QString::number(position[1], 'f', 0));
//    ui->line_PosZ->setText(QString::number(position[2], 'f', 0));

    updateCameraPosition();

    ///
    /// \brief Scaling
    ///
    ///
    double value ;

    value = this->defaultCameraDistance /  camera->GetDistance();

    ui->line_Scale->setText(QString::number(value, 'f', 2));
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_Tab) {
            // special tab handling here
            std::cout << "tab button pressed" << endl;
            return true;
        }
    }
    return QWidget::event(event);
}

void MainWindow::on_actionTestButton_triggered()
{
        ///
        vtkSmartPointer<vtkCubeSource> centerBox =
                vtkSmartPointer<vtkCubeSource>::New();

        double * bounds = global_Volume->GetBounds();
        double * cubeCenter = global_Volume->GetCenter();

        double leftHandOffset[3] = {
            cubeCenter[0]  / 1.0 ,
            cubeCenter[1] * 0.0 ,
            cubeCenter[2] * 0.6
        };

        double rightHandOffset[3] = {
            cubeCenter[0] * 0.9 ,
            cubeCenter[1] * 0.0 ,
            cubeCenter[2] * 0.6
        };

        centerBox->SetCenter(global_Volume->GetCenter());

        bounds[0] = bounds[0] + (bounds[1] * 0.2) ;
        bounds[1] = bounds[1]  -  (bounds[1] * 0.2) ;
        bounds[2] = bounds[2] + (bounds[3] * 0.2) ;
        bounds[3] = bounds[3]  -  (bounds[3] * 0.2) ;
        bounds[4] = bounds[4] + (bounds[5] * 0.0) ;
        bounds[5] = bounds[5] +  (bounds[5] * 0.1) ;

        centerBox->SetBounds(bounds);

        vtkSmartPointer<vtkPolyDataMapper> centerMapper =
              vtkSmartPointer<vtkPolyDataMapper>::New();
      centerMapper->SetInputConnection(centerBox->GetOutputPort());

      vtkSmartPointer<vtkActor>  boxActor =
              vtkSmartPointer<vtkActor>::New();
      boxActor->GetProperty()->SetOpacity(0.2);
       boxActor->SetMapper(centerMapper);

       this->defaultRenderer->AddActor(boxActor);

       handRenderer = new HandRenderer();
       handRenderer->setScale(1);
       handRenderer->setJoinSize(1.5);
       handRenderer->setFingerSize(15);

       handRenderer->drawJoints(rightHand,defaultRenderer);
       handRenderer->drawBones(rightHand, defaultRenderer);

        handRenderer->drawJoints(leftHand, defaultRenderer);
        handRenderer->drawBones(leftHand, defaultRenderer);

       handRenderer->setStartLocation(leftHand,leftHandOffset);
       handRenderer->setStartLocation(rightHand,rightHandOffset);





        handModelActive = true;




    //this->customArbPlaneWidget->GetNormal()->
}

void MainWindow::on_actionTestButton2_triggered()
{
    for (int f = 0; f < 5;  f++)
    {
        std::cout << "Left Hand: Joints Finger : " << f+1 << endl;
         handRenderer->printFingerJoints(leftHand,f);
        std::cout << endl;
    }
     std::cout << "----------------------------" << endl;
    for (int f = 0; f < 5;  f++)
    {
        std::cout << "Right Hand: Joints Finger: " << f +1 << endl;
         handRenderer->printFingerJoints(rightHand,f);
         std::cout << endl;
    }

    for (int f = 0; f < 5;  f++)
    {
        std::cout << "Left Hand: Bones Finger: " << f+1 << endl;
         handRenderer->printFingerBones(leftHand,f);
        std::cout << endl;
    }
     std::cout << "----------------------------" << endl;
    for (int f = 0; f < 5;  f++)
    {
        std::cout << "Right Hand: Bones Finger: " << f +1 << endl;
         handRenderer->printFingerBones(rightHand,f);
         std::cout << endl;
    }

}
