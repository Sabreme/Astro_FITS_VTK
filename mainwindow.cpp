#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "main.h"

#include "subVolume.h"
#include "subvolumedialog.h"

#include "sliceDialog.h"
//#include "QtVTKRenderWindows.h"

#include "OrientationAxes.h"

#include "ScalarBar.h"
#include "PointerTracker.h"

#include "XYHighlightSelection.h"
#include "ZHighlightSelection.h"
#include "vtkBoxWidget.h"
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
#include <vtkBoxWidget.h>

#include <vtkActor2DCollection.h>
#include <vtkTextProperty.h>
#include <vtkActor.h>
#include <vtkActorCollection.h>

#include <vtkTransform.h>
#include <vtkLinearTransform.h>
#include <vtkPerspectiveTransform.h>
#include <vtkCamera.h>
#include <math.h>

#include <vtkVolume.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkStructuredPointsToPolyDataFilter.h>

#include <vtkPlaneCollection.h>
#include <vtkStructuredPointsGeometryFilter.h>
#include <QDebug>
#include <QThread>
#include <thread>

#include "vtkResliceImageViewer.h"
#include "vtkResliceCursorLineRepresentation.h"
#include "vtkResliceCursorThickLineRepresentation.h"
#include "vtkResliceCursorWidget.h"
#include "vtkResliceCursorActor.h"
#include "vtkResliceCursorPolyDataAlgorithm.h"
#include "vtkResliceCursor.h"
#include "vtkDistanceRepresentation2D.h"
#include "vtkPointHandleRepresentation3D.h"
#include "vtkPointHandleRepresentation2D.h"
#include "vtkDistanceWidget.h"
#include "vtkDistanceRepresentation.h"
#include "vtkHandleRepresentation.h"
#include "vtkResliceImageViewerMeasurements.h"
#include "vtkPlaneSource.h"
#include "vtkInteractorStyleImage.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkBoundedPlanePointPlacer.h"
#include "vtkImageSlabReslice.h"
#include "vtkWindowToImageFilter.h"
#include "vtkPNGWriter.h"
#include "vtkInteractorObserver.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkInteractorStyleJoystickCamera.h"
#include "vtkCoordinate.h"
#include "vtkWorldPointPicker.h"
#include "vtkLineWidget2.h"
#include "vtkLineRepresentation.h"
#include "vtkLineWidget.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkInteractorObserver.h"

#include <vtkPointWidget.h>

#include <QTimer>

#include <Sample.h>

#include <vtkCallbackCommand.h>


#include "vtkFrameRateWidget.h"

#include "vtkEventQtSlotConnect.h"

#include "vtkPlaneSource.h"
#include "vtkAbstractTransform.h"
#include "vtkImplicitPlaneWidget2.h"
#include "vtkImplicitPlaneRepresentation.h"
#include "vtkImplicitPlaneWidget.h"
#include "vtkCutter.h"

#include "vtkMath.h"
#include "vtkTransformPolyDataFilter.h"

#include "vtkSliderRepresentation3D.h"
#include "vtkSliderWidget.h"

#ifdef OS_WINDOWS
    #include "windows.h"
#endif



/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///                OBSERVER CALLBACKS
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


class vtkMySliceCallback : public vtkCommand
{
   public:
    static vtkMySliceCallback * New()
    {
        return new vtkMySliceCallback;
    }
    void Delete()
    {
        delete this;
    }

    virtual void Execute(vtkObject *caller, unsigned long , void *)
    {
        double position[3], normal[3], origin[3];

        ///
        /// The Plane has moved, updatge the samlped data values.
        ///

        vtkImplicitPlaneWidget *impPlaneWidget = reinterpret_cast<vtkImplicitPlaneWidget*>(caller);
        impPlaneWidget->GetPolyData(polyPlane);
        impPlaneWidget->GetPlane(plane);
        cutter->SetCutFunction(plane);
        //std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<"


        ///
        /// Center the Camera so that it follows the 2D window image
        ///

        impPlaneWidget->GetNormal(normal);
        impPlaneWidget->GetOrigin(position);
        impPlaneWidget->GetOrigin(origin);

        camera->SetPosition(position[0] + normal[0], position[1] +normal[1], position[2] + normal[2]);
        camera->SetFocalPoint(position);
        camera->SetDistance(100.0);
        camera->OrthogonalizeViewUp();


        renderWindow->Render();

        //impPlaneWidget->Print(std::cout);
//        std::cout << "Origin = " << origin[0] << "," << origin[1] << "," << origin[2] << " \t";
//        std::cout << "position = " << position[0] << "," << position[1] << "," << position[2] << " \t";
//        std::cout << "normal = " << normal[0] << ", " << normal[1] << ",  " << normal[2] << " \t";
        

        ui->lineArbSlicePosX->setText(QString::number(position[0], 'f', 0));
        ui->lineArbSlicePosY->setText(QString::number(position[1], 'f', 0));
        ui->lineArbSlicePosZ->setText(QString::number(position[2], 'f', 0));

        ui->lineArbSliceAngleX->setText(QString::number( (1 + normal[0]) * 180 , 'f', 0));
        ui->lineArbSliceAngleY->setText(QString::number( (1 + normal[1]) * 180, 'f', 0));
        ui->lineArbSliceAngleZ->setText(QString::number( (1 + normal[2]) * 180, 'f', 0));
        
        std::cout << "X = " << normal[0] << "\tY= " << normal[1] << "\tZ  " << normal[2] << " \t";
        std::cout << endl;

    }

    vtkMySliceCallback (): polyPlane(0), plane(0), cutter(0), camera(0) {}
    vtkPolyData* polyPlane;
    vtkPlane* plane;
    vtkCutter* cutter;
    vtkCamera* camera;
    vtkRenderWindow* renderWindow;
    Ui::MainWindow * ui;

};

// Define interaction style
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{

public:
    static MouseInteractorStyle* New();
    vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonDown()
    {

//      std::cout << "Picking pixel: " << this->Interactor->GetEventPosition()[0] << " " << this->Interactor->GetEventPosition()[1] << std::endl;
//      this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
//                         this->Interactor->GetEventPosition()[1],
//                         0,  // always zero.
//                         this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
//      double picked[3];
//      this->Interactor->GetPicker()->GetPickPosition(picked);
      //std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;



//      double * orientation;
//      orientation = camera->GetOrientation();

//      std::cout << "orientation: " << orientation[0] << " " << orientation[1] << " " << orientation[2] << " " << orientation[3] << std::endl;

      //this->ui->lineInfoNAxis1->setText(QString("%1").arg(orientation[0]));

        //this->
      // Forward events
      vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }    

    virtual void Rotate()
   {


       vtkInteractorStyleTrackballCamera::Rotate();

       double* orientation;

       orientation  = camera->GetOrientation();

       ui->line_OrientX->setText(QString::number(orientation[0], 'f', 0));
       ui->line_OrientY->setText(QString::number(orientation[1], 'f', 0));
       ui->line_OrientZ->setText(QString::number(orientation[2], 'f', 0));


   }


    virtual void Pan()
    {


        vtkInteractorStyleTrackballCamera::Pan();

        double* position;

        position = camera->GetPosition();

        ui->line_PosX->setText(QString::number(position[0], 'f', 0));
        ui->line_PosY->setText(QString::number(position[1], 'f', 0));
        ui->line_PosZ->setText(QString::number(position[2], 'f', 0));
    }

    virtual void Dolly()
    {

        vtkInteractorStyleTrackballCamera::Dolly();

        double value ;

        value = this->defualtDistance /  this->GetCurrentRenderer()->GetActiveCamera()->GetDistance();

        ui->line_Scale->setText(QString::number(value, 'f', 2));
    }


    vtkCamera * camera;
    Ui::MainWindow * ui;
    double defualtDistance;


};

class MouseInteractorStyleShiftAndControlTrackBall : public vtkInteractorStyleTrackballCamera
{
    public:
    static MouseInteractorStyleShiftAndControlTrackBall* New();

    virtual void OnLeftButtonDown()
    {
        if (changeType ==1 )
        {
            this->Interactor->SetShiftKey(1);            
        }
        else
        {
            this->Interactor->SetControlKey(1);            
        }

        // Forward events

        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
    int currentStyle;
    int changeType;
};

class MouseInteractorStyleScalingAndControlTrackBall : public vtkInteractorStyleTrackballCamera
{
    public:
    static MouseInteractorStyleScalingAndControlTrackBall* New();

    virtual void OnLeftButtonDown()
    {

        // Forward events Right Button Mouse Events

        vtkInteractorStyleTrackballCamera::OnRightButtonDown();
    }
};

class MouseInteractorStyleShiftAndControlJoystick : public vtkInteractorStyleJoystickCamera
{
    public:
    static MouseInteractorStyleShiftAndControlJoystick* New();

    virtual void OnLeftButtonDown()
    {
        if (changeType == 1)
        {
            this->Interactor->SetShiftKey(1);
        }
        else
        {
            this->Interactor->SetControlKey(1);
        }

        vtkInteractorStyleJoystickCamera::OnLeftButtonDown();
        // Forward events

//        if (currentStyle==1)
//        {

//            vtkInteractorStyleJoystickActor::OnLeftButtonDown();
//        }
//        else
//        if
//           (currentStyle==2)
//        {
//            vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
//        }
//        vtkInteractorStyle::OnLeftButtonDown();

    }
    int currentStyle;
    int changeType;

};

class vtkSliderCallback : public vtkCommand
{
public:
  static vtkSliderCallback *New()
    {
    return new vtkSliderCallback;
    }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      vtkSliderWidget *sliderWidget =
              reinterpret_cast<vtkSliderWidget*>(caller);

      int slice = static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue();
      this->riw[axis]->SetSlice(slice);

      this->label->setText(QString("%1").arg(slice));

      vtkPlaneSource *ps = static_cast< vtkPlaneSource * >(
                  this->ipw[axis]->GetPolyDataAlgorithm());
      ps->SetNormal(riw[0]->GetResliceCursor()->GetPlane(axis)->GetNormal());
      ps->SetCenter(riw[0]->GetResliceCursor()->GetPlane(axis)->GetOrigin());
      this->ipw[axis]->SetSliceIndex(slice);

      this->ipw[axis]->UpdatePlacement();

    ///MainWindow->Render();

    }
  vtkSliderCallback() {}
  vtkResliceImageViewer *riw[3];
  QLabel * label;
  vtkImagePlaneWidget * ipw[3];
  int axis;
};

class vtkLineCallback : public vtkCommand
{
  public:
    static vtkLineCallback *New()
    {
      return new vtkLineCallback;
    }

    virtual void Execute(vtkObject *caller, unsigned long, void*)
    {

      vtkLineWidget *lineWidget =
          reinterpret_cast<vtkLineWidget*>(caller);


//      // Get the actual box coordinates of the line
//      vtkSmartPointer<vtkPolyData> polydata =
//          vtkSmartPointer<vtkPolyData>::New();
//      static_cast<vtkLineRepresentation*>(lineWidget->GetRepresentation())->GetPolyData (polydata);

//      vtkSmartPointer<vtkLineRepresentation> lineRep =
//              lineWidget->GetLineRepresentation();
      // Display one of the points, just so we know it's working
      double displayPos1[3];
      double displayPos2[3];
      double worldPos1[3];
      double worldPos2[3];



//      lineRep->GetPoint1DisplayPosition(displayPos1);
//      lineRep->GetPoint2DisplayPosition(displayPos2);
//      lineRep->GetPoint1WorldPosition(worldPos1);
//      lineRep->GetPoint2WorldPosition(worldPos2);

      lineWidget->GetPoint1(displayPos1);
      lineWidget->GetPoint2(displayPos2);

      std::cout << "Display1: " << displayPos1[0] << ", " << displayPos1[1] << ", " << displayPos1[2] << std::endl;
      std::cout << "Display2: " << displayPos2[0] << ", " << displayPos2[1] << ", " << displayPos2[2] << std::endl;

      lineWidget->ComputeDisplayToWorld(lineWidget->GetCurrentRenderer(),
                                        displayPos1[0], displayPos1[1], displayPos1[2],
                                        worldPos1);

      lineWidget->ComputeDisplayToWorld(lineWidget->GetCurrentRenderer(),
                                        displayPos2[0], displayPos2[1], displayPos2[2],
                                        worldPos2);

      std::cout << "World1: " << worldPos1[0] << ", " << worldPos1[1] << ", " << worldPos1[2] << std::endl;
      std::cout << "World2: " << worldPos2[0] << ", " << worldPos2[1] << ", " << worldPos2[2] << std::endl;


      vtkRenderWindowInteractor * vtkInteractor = lineWidget->GetInteractor();



      int * lastPos = vtkInteractor->GetLastEventPosition();

      std::cout << "LastPos: " << lastPos[0] << " " << lastPos[1] << endl;

      riw->GetInteractor()->LeftButtonPressEvent();
      riw->GetInteractor()->SetEventPosition(lastPos[0] + 10, lastPos[1] + 10);
      riw->GetInteractor()->MouseMoveEvent();
      riw->GetInteractor()->LeftButtonReleaseEvent();


      //std::cout << "WorldPt1: " << worldPos1[0] << ", " << worldPos1[1] << ", " << worldPos1[2] << std::endl;
      //std::cout << "WorldPt2: " << worldPos2[0] << ", " << worldPos2[1] << ", " << worldPos2[2] << std::endl;

      std::cout << "---------------------------------------------" << endl;


    }
    vtkLineCallback(){}
    vtkResliceImageViewer * riw;


};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Define interaction style
class KeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera
{
  public:
    static KeyPressInteractorStyle* New();
    vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnKeyPress()
    {
      // Get the keypress
      vtkRenderWindowInteractor *rwi = this->Interactor;
      std::string key = rwi->GetKeySym();

      // Output the key that was pressed
//      std::cout << "Pressed " << key << std::endl;

//      // Handle an arrow key
//      if(key == "Up")
//        {
//        std::cout << "The up arrow was pressed." << std::endl;
//        }
      // Handle r "normal" key
      if(key == "r")
      {

          if (this->rotation->isChecked())
          {
              rotation->setChecked(false);
              this->mainWindow->leapTransfNoneClicked();
          }
          else
          {
              rotation->setChecked(true);
              this->mainWindow->leapTransfRotationClicked();
          }

      }


      if(key == "t")
      {
          if (this->translation->isChecked())
          {
              translation->setChecked(false);
              this->mainWindow->leapTransfNoneClicked();
          }
          else
          {
              translation->setChecked(true);
              this->mainWindow->leapTransfTranslationClicked();
          }

      }

      if(key == "s")
      {
          if (this->scaling->isChecked())
          {
              scaling->setChecked(false);
              this->mainWindow->leapTransfNoneClicked();
          }
          else
          {
              scaling->setChecked(true);
              this->mainWindow->leapTransfScalingClicked();
          }

      }

      if(key == "c")
      {
          mainWindow->KeyboardResetCamera();
      }


      // Forward events
      vtkInteractorStyleTrackballCamera::OnKeyPress();
    }
    QCheckBox * rotation;
    QCheckBox * translation;
    QCheckBox * scaling;
    MainWindow * mainWindow;

//    char * rotation;
//    char * translation;
//    char * zooming;


};
vtkStandardNewMacro(KeyPressInteractorStyle);
vtkStandardNewMacro(MouseInteractorStyle);
vtkStandardNewMacro(MouseInteractorStyleShiftAndControlTrackBall);
vtkStandardNewMacro(MouseInteractorStyleShiftAndControlJoystick);
vtkStandardNewMacro(MouseInteractorStyleScalingAndControlTrackBall);

void CallbackFunction(vtkObject* caller, long unsigned int vtkNotUsed(eventId),void* vtkNotUsed(clientData), void* vtkNotUsed(callData) )
{
  vtkRenderer* renderer = static_cast<vtkRenderer*>(caller);

  double timeInSeconds = renderer->GetLastRenderTimeInSeconds();
  double fps = 1.0/timeInSeconds;
  std::cout << "FPS: " << fps << std::endl;

  std::cout << "Callback" << std::endl;
}


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
//    connect(this->ui->tabLogWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(removeTab(int)) )

    timer->start();

    this->ui->tabLogWidget->setVisible(false);
    this->ui->LeftCameraFrame->setVisible(false);
    this->ui->TopMethodFrame->setVisible(false);
    this->ui->TopModeFrame->setVisible(false);

//    this->ui->TopMethodFrame->setStyleSheet(QString::fromUtf8("QPushButton:disabled {"
//                                                                            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #dadbde);"
//                                                                            "border: 2px solid #8f8f91; "
//                                                                            "border-radius: 6px;"
//                                                                            "color: rgb(81, 81, 81) "
//                                                                            "}"));

//    this->ui->TopMethodFrame->setStyleSheet(QString::fromUtf8("QPushButton:disabled {"
//                                                                            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #dadbde);"
//                                                                            "border: 2px solid #8f8f91; "
//                                                                            "border-radius: 6px;"
//                                                                            "color: rgb(81, 81, 81) "
//                                                                            "}"));

//    this->ui->LeftCameraFrame->setStyleSheet(QString::fromUtf8("QPushButton:disabled {"
//                                                                            "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));"
//                                                                            "border: 2px solid #8f8f91; "
//                                                                            "border-radius: 6px;"
//                                                                            "color: rgb(81, 81, 81) "
//                                                                            "}"));

    this->on_buttonModeMouse_clicked();
/////
/// //////////////////////////////////////////////////////////
/// //////////////////////////////////////////////////////////
///// STYLE SHEET CUSTOMIZATION FOR THE APPLICATION
/// //////////////////////////////////////////////////////////
///
///


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

void MainWindow::releaseTransfFocus()
{
    switch (this->systemTransF)
    {
        case Rotation:   this->ui->buttonTransfRotation->setEnabled(true);
        break;

        case Translation:   this->ui->buttonTransfTranslation->setEnabled(true);
        break;

        case Scaling:   this->ui->buttonTransfScaling->setEnabled(true);
        break;

        case None:   ;
        break;
    }
}

void MainWindow::releaseModeSlots()
{

    switch (this->systemMode)
    {
        case Mouse:
        disconnect(this->ui->buttonTransfRotation,          SIGNAL(pressed()),  this,SLOT(buttonTransRotationPressed()));
        disconnect(this->ui->buttonTransfTranslation,       SIGNAL(pressed()),  this,SLOT(buttonTransTranslationPressed()));
        disconnect(this->ui->buttonTransfScaling,           SIGNAL(pressed()),  this,SLOT(buttonTransScalingPressed()));

            //disconnect(this->ui->buttonTabSubVol,           SIGNAL(pressed()), this, SLOT(on_buttonTabSubVol_pressed()));


        break;

        case Leap:
        disconnect(this->ui->buttonTransfRotation,          SIGNAL(pressed()), this,SLOT(leapTransRotationPressed()));
        disconnect(this->ui->buttonTransfTranslation,       SIGNAL(pressed()), this,SLOT(leapTransTranslationPressed()));
        disconnect(this->ui->buttonTransfScaling,           SIGNAL(pressed()), this,SLOT(leapTransScalingPressed()));

            // disconnect(this->ui->buttonTabSubVol,           SIGNAL(clicked()), this, SLOT(leapBeginSubVol()));

        break;

        case Touch:
        break;

    }
}



void MainWindow::leapTransfRotationClicked()
{    

    std::cout << "Leap ROtaion Code" << endl;
    /// If WE ARE LEAP MODE.. ACTIVATE INTERACTOR TO DEFAULT and BUTTON DEAULT TO TRUE
    if(this->ui->checkBox_Rotation->isChecked())
    {
        this->systemTransF = Rotation;
        this->ui->buttonTransfRotation->setDefault(true);

        this->ui->buttonTransfScaling->setDefault(false);
        this->ui->buttonTransfTranslation->setDefault(false);

        ///////////////////////////////////////////////
        /// Leap Interaction
        ///

        vtkRenderWindowInteractor * interactor = this->ui->qvtkWidgetLeft->GetInteractor();
        KeyPressInteractorStyle * style = KeyPressInteractorStyle::New();
        interactor->SetInteractorStyle(style);

        //style->SetCurrentRenderer(this->defaultRenderer);

        //this->rotationPress = this->ui->checkBox_Rotation;
        style->rotation = this->ui->checkBox_Rotation;
        style->translation = this->ui->checkBox_Translation;
        style->scaling = this->ui->checkBox_Scaling;
        style->mainWindow = this;

        this->leapTrackingActor->SetVisibility(true);
    }
}

void MainWindow::leapTransfTranslationClicked()
{

    std::cout << "Leap Translation Code" << endl;

    /// If WE ARE LEAP MODE.. ACTIVATE INTERACTOR TO DEFAULT and BUTTON DEAULT TO TRUE
    if(this->ui->checkBox_Translation->isChecked())
    {
        this->systemTransF = Translation;
        this->ui->buttonTransfTranslation->setDefault(true);

        this->ui->buttonTransfScaling->setDefault(false);
        this->ui->buttonTransfRotation->setDefault(false);

        ///////////////////////////////////////////////
        /// Leap Interaction
        ///

        vtkRenderWindowInteractor * interactor = this->ui->qvtkWidgetLeft->GetInteractor();
        KeyPressInteractorStyle * style = KeyPressInteractorStyle::New();
        interactor->SetInteractorStyle(style);

        //style->SetCurrentRenderer(this->defaultRenderer);

        //this->rotationPress = this->ui->checkBox_Rotation;
        style->rotation = this->ui->checkBox_Rotation;
        style->translation = this->ui->checkBox_Translation;
        style->scaling = this->ui->checkBox_Scaling;
        style->mainWindow = this;

        this->leapTrackingActor->SetVisibility(true);
    }
}


void MainWindow::leapTransfScalingClicked()
{

    std::cout << "Leap Scaling Code" << endl;
    /// If WE ARE LEAP MODE.. ACTIVATE INTERACTOR TO DEFAULT and BUTTON DEAULT TO TRUE
    if(this->ui->checkBox_Scaling->isChecked())
    {
        this->systemTransF = Scaling;
        this->ui->buttonTransfScaling->setDefault(true);

        this->ui->buttonTransfTranslation->setDefault(false);
        this->ui->buttonTransfRotation->setDefault(false);

        ///////////////////////////////////////////////
        /// Leap Interaction
        ///

        vtkRenderWindowInteractor * interactor = this->ui->qvtkWidgetLeft->GetInteractor();
        KeyPressInteractorStyle * style = KeyPressInteractorStyle::New();
        interactor->SetInteractorStyle(style);

        //style->SetCurrentRenderer(this->defaultRenderer);

        //this->rotationPress = this->ui->checkBox_Rotation;
        style->rotation = this->ui->checkBox_Rotation;
        style->translation = this->ui->checkBox_Translation;
        style->scaling = this->ui->checkBox_Scaling;
        style->mainWindow = this;

        this->leapTrackingActor->SetVisibility(true);
    }
}

void MainWindow::leapTransfNoneClicked()
{
     std::cout << "Leap NONE Code" << endl;

     this->systemTransF = None;
     this->ui->buttonTransfRotation->setDefault(false);

     this->ui->buttonTransfScaling->setDefault(false);
     this->ui->buttonTransfTranslation->setDefault(false);

     ///////////////////////////////////////////////
     /// Leap Interaction
     ///

     vtkRenderWindowInteractor * interactor = this->ui->qvtkWidgetLeft->GetInteractor();
     KeyPressInteractorStyle * style = KeyPressInteractorStyle::New();
     interactor->SetInteractorStyle(style);

     //style->SetCurrentRenderer(this->defaultRenderer);

     //this->rotationPress = this->ui->checkBox_Rotation;
     style->rotation = this->ui->checkBox_Rotation;
     style->translation = this->ui->checkBox_Translation;
     style->scaling = this->ui->checkBox_Scaling;
     style->mainWindow = this;

     this->leapTrackingActor->SetVisibility(false);
}



///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/////////////////MODE SELECTION BUTTONS////////////////////
///////////////////////////////////////////////////////////


void MainWindow::on_buttonModeMouse_clicked()
{
    if (this->systemMode != Mouse )
    {
        releaseModeSlots();

        this->ui->buttonModeMouse->setDefault(true);
        this->systemMode = Mouse;

        this->ui->buttonModeLeap->setDefault(false);
        this->ui->buttonModeTouch->setDefault(false);

        std::cout << "System Mode is: " << this->systemMode << endl;        

        connect(this->ui->buttonTransfRotation, SIGNAL(pressed()), this,SLOT(buttonTransRotationPressed()));
        connect(this->ui->buttonTransfTranslation, SIGNAL(pressed()), this,SLOT(buttonTransTranslationPressed()));
        connect(this->ui->buttonTransfScaling, SIGNAL(pressed()), this,SLOT(buttonTransScalingPressed()));

        //connect(this->ui->buttonTabSubVol,           SIGNAL(clicked()), this, SLOT(beginSubVolume()));

        //this

        //this->on_buttonTransfRotation_clicked();
        this->buttonTransRotationPressed();

    }
}


void MainWindow::on_buttonModeLeap_clicked()
{
    if (this->systemMode != Leap )
    {
        releaseModeSlots();

        this->ui->buttonModeLeap->setDefault(true);

        this->systemMode = Leap;

        this->ui->buttonModeMouse->setDefault(false);
        this->ui->buttonModeTouch->setDefault(false);

        std::cout << "System Mode is: " << this->systemMode << endl;

        ///////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////

        this->controller_= new Controller;
        Leaping_ = true;

        this->ui->tabLogWidget->insertTab(0, this->ui->LeapTab, "Leap Data Tab");
        this->ui->tabLogWidget->setCurrentIndex(0);

        vtkSmartPointer<KeyPressInteractorStyle> style =
                vtkSmartPointer<KeyPressInteractorStyle>::New();

        this->ui->qvtkWidgetLeft->GetInteractor()->SetInteractorStyle(style);
        style->SetCurrentRenderer(this->defaultRenderer);

        //this->rotationPress = this->ui->checkBox_Rotation;
        style->rotation = this->ui->checkBox_Rotation;
        style->translation = this->ui->checkBox_Translation;
        style->scaling = this->ui->checkBox_Scaling;
        style->mainWindow = this;



        this->leapMatrixTotalMotionRotation = Leap::Matrix::identity();
        this->leapVectorTotalMotionalTranslation = Leap::Vector::zero();
        this->leapFloatTotalMotionScale = 1.0f;

        LeapDiagnostic();

//        connect(this->ui->buttonTransfRotation, SIGNAL(clicked()), this,SLOT(leapTransfRotationClicked()));
//        connect(this->ui->buttonTransfTranslation, SIGNAL(clicked()), this,SLOT(leapTransfTranslationClicked()));
//        connect(this->ui->buttonTransfScaling, SIGNAL(clicked()), this,SLOT(leapTransfScalingClicked()));

        connect(this->ui->buttonTransfRotation, SIGNAL(pressed()), this,SLOT(leapTransRotationPressed()));
        connect(this->ui->buttonTransfTranslation, SIGNAL(pressed()), this,SLOT(leapTransTranslationPressed()));
        connect(this->ui->buttonTransfScaling, SIGNAL(pressed()), this,SLOT(leapTransScalingPressed()));

        connect(this->ui->checkBoxLeapTracking, SIGNAL(toggled(bool)), this,SLOT(leapTrackingOn(bool)));

        //connect(this->ui->buttonTabSubVol,           SIGNAL(clicked()), this, SLOT(leapBeginSubVol()));

        this->leapTransfNoneClicked();

        this->ui->qvtkWidgetLeft->setFocus();

         this->leapTransRotationPressed();
    }
}

void MainWindow::on_buttonModeTouch_clicked()
{
    if (this->systemMode != Touch )
    {
        this->ui->buttonModeTouch->setDefault(true);
        this->systemMode = Touch;

        this->ui->buttonModeMouse->setDefault(false);
        this->ui->buttonModeLeap->setDefault(false);

        std::cout << "System Mode is: " << this->systemMode << endl;

    }
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/////////////////TAB SELECTION BUTTONS - MOUSE ////////////
///////////////////////////////////////////////////////////

void MainWindow::on_buttonTabInfo_pressed()
{
    if(this->systemTab != Information)
    {
        if (this->MessageBoxQuery("Switch to Information ?","Are you sure you want to continue?"))
        {
            this->releaseTabFocus();
            this->systemTab = Information;

            this->ui->buttonTabInfo->setDisabled(true);

            this->infoTab_Triggered();

            switch (this->systemMode)
            {
                case Mouse:
                {
                    /////////////////////////////////////////////////
                    ///  Mouse Interaction
                    ///
                    ///
                    MouseInteractorStyle * style = MouseInteractorStyle::New();
                    vtkRenderWindowInteractor * interactor = this->ui->qvtkWidgetLeft->GetInteractor();

                    interactor->SetInteractorStyle(style);
                    style->camera = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();
                    style->ui = this->ui;
                    style->defualtDistance = this->defaultCameraDistance;
                }
                break;

                case Leap:  ;
                break;

                //case Touch: this->touchBeginSubVol();
            }
        }

    }
    this->ui->qvtkWidgetLeft->setFocus();
}



void MainWindow::on_buttonTabSubVol_pressed()
{
    if(this->systemTab != SubVolume)
    {
        if (this->MessageBoxQuery("Switch to SubVolume?","Are you sure you want to continue?"))
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
                    /////////////////////////////////////////////////
                    ///  Mouse Interaction
                    ///
                    ///
                    MouseInteractorStyle * style = MouseInteractorStyle::New();
                    vtkRenderWindowInteractor * interactor = this->ui->qvtkWidgetLeft->GetInteractor();

                    interactor->SetInteractorStyle(style);
                    style->camera = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();
                    style->ui = this->ui;
                    style->defualtDistance = this->defaultCameraDistance;
                }
                break;

                case Leap:  this->leapBeginSubVol();
                break;

                //case Touch: this->touchBeginSubVol();
            }
        }
        this->ui->qvtkWidgetLeft->setFocus();
    }
}

void MainWindow::on_buttonTabSliceAxis_pressed()
{
    if(this->systemTab != SliceAxis)
    {
        if (this->MessageBoxQuery("Switch to Axis Slice?","Are you sure you want to continue?"))
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
                    /////////////////////////////////////////////////
                    ///  Mouse Interactor
                    ///
                    ///
                    MouseInteractorStyle * style = MouseInteractorStyle::New();
                    vtkRenderWindowInteractor * interactor = this->ui->qvtkWidgetLeft->GetInteractor();
                    interactor->SetInteractorStyle(style);
                    style->camera = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();
                    style->ui = this->ui;
                    style->defualtDistance = this->defaultCameraDistance;
                }
                break;

                case Leap: this->leapBeginSliceAxis();
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
        if (this->MessageBoxQuery("Switch to Arb Slice?","Are you sure you want to continue?"))
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

                /////////////////////////////////////////////////
                ///  Mouse Rotation
                ///
                ///
                MouseInteractorStyle * style = MouseInteractorStyle::New();


                vtkRenderWindowInteractor * interactor = this->ui->qvtkWidgetLeft->GetInteractor();
                interactor->SetInteractorStyle(style);
                style->camera = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();
                style->ui = this->ui;
                style->defualtDistance = this->defaultCameraDistance;
            }
                break;

                case Leap: this->leapBeginSliceArb();
                break;
            }
        }
        this->ui->qvtkWidgetLeft->setFocus();
    }

}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/////////////////TRANSFORM SELECTION BUTTONS///////////////
///////////////////////////////////////////////////////////

void MainWindow::buttonTransRotationPressed()
{
    if (this->systemTransF != Rotation)
    {
        this->releaseTransfFocus();
        this->systemTransF = Rotation;

        this->ui->buttonTransfRotation->setDisabled(true);

        /////////////////////////////////////////////////
        ///  Mouse Rotation
        ///
        ///
        MouseInteractorStyle * style = MouseInteractorStyle::New();


        vtkRenderWindowInteractor * interactor = this->ui->qvtkWidgetLeft->GetInteractor();
        interactor->SetInteractorStyle(style);
        style->camera = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();
        style->ui = this->ui;
        style->defualtDistance = this->defaultCameraDistance;

        //vtkInteractorStyleSwitch * camSwitch =  vtkInteractorStyleSwitch::New();
        //interactor->SetInteractorStyle(camSwitch);


        //camSwitch->SetCurrentStyleToTrackballCamera();
    }
    this->ui->qvtkWidgetLeft->setFocus();
}

void MainWindow::buttonTransTranslationPressed()
{
    if (this->systemTransF != Translation)
    {
        this->releaseTransfFocus();
        this->systemTransF = Translation;

        this->ui->buttonTransfTranslation->setDisabled(true);

        /////////////////////////////////////////////////
        ///  Mouse Translation
        ///
        vtkRenderWindowInteractor * interactor = this->ui->qvtkWidgetLeft->GetInteractor();

        MouseInteractorStyleShiftAndControlTrackBall * style =
            MouseInteractorStyleShiftAndControlTrackBall::New();

        style->changeType = 1;
        interactor->SetInteractorStyle(style);

    }
    this->ui->qvtkWidgetLeft->setFocus();
}

void MainWindow::buttonTransScalingPressed()
{
    if (this->systemTransF != Scaling)
    {
        this->releaseTransfFocus();
        this->systemTransF = Scaling;

        this->ui->buttonTransfScaling->setDisabled(true);

        /////////////////////////////////////////////////
        ///  Mouse Scaling
        ///
        vtkRenderWindowInteractor * interactor = this->ui->qvtkWidgetLeft->GetInteractor();

        MouseInteractorStyleScalingAndControlTrackBall * style =
            MouseInteractorStyleScalingAndControlTrackBall::New();
        interactor->SetInteractorStyle(style);

    }
    this->ui->qvtkWidgetLeft->setFocus();
}

void MainWindow::leapTransRotationPressed()
{
    std::cout << "leapTransRotationPressed" << endl;

    if (this->systemTransF != Rotation)
    {
        this->releaseTransfFocus();
        this->systemTransF = Rotation;

        this->ui->buttonTransfRotation->setDisabled(true);
    }
    this->ui->qvtkWidgetLeft->setFocus();

}

void MainWindow::leapTransTranslationPressed()
{

    std::cout << "leapTransTranslationPressed" << endl;


    if (this->systemTransF != Translation)
    {
        this->releaseTransfFocus();
        this->systemTransF = Translation;

        this->ui->buttonTransfTranslation->setDisabled(true);
    }
    this->ui->qvtkWidgetLeft->setFocus();
}

void MainWindow::leapTransScalingPressed()
{

    std::cout << "leapTransScalingPressed" << endl;

    if (this->systemTransF != Scaling)
    {
        this->releaseTransfFocus();
        this->systemTransF = Scaling;

        this->ui->buttonTransfScaling->setDisabled(true);

    }
    this->ui->qvtkWidgetLeft->setFocus();
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


////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
/// \brief MainWindow::updateme
////////////////////////////////////////////////////////////////////


void MainWindow::updateme()
{
    ///ui->qvtkWidgetLeft->GetRenderWindow()->Render();

    //        if (this->ui->checkCameraUpdate->isChecked())
    //        {
    //            updateCameraDetails();
    //        }


    QFont boldFont, normalFont;

    boldFont.setBold(true);
    normalFont.setBold(false);

    this->ui->labelTranslation->setFont(normalFont);
    this->ui->labelRotation->setFont(normalFont);
    this->ui->labelScaling->setFont(normalFont);


    //this->ui->labelRotation->font().bold() = false;
    //this->ui->labelRotation->font().bold() = false;

    if (Leaping_)
    {
        ui->qvtkWidgetLeft->GetRenderWindow()->Render();
       LeapMotion();
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
    fitsReader->SetFileName(newFileName);

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


    global_CubeAxes = cubeAxesActor;
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

    AddScalarBar(this->ui->qvtkWidgetLeft, fitsReader);

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


     ///////////////////////////////////////////////
     /// \brief SETTING Default Interactor Style to Mouse Interaction
     ///
     ///
     MouseInteractorStyle * style = MouseInteractorStyle::New();
     vtkRenderWindowInteractor * interactor = this->ui->qvtkWidgetLeft->GetInteractor();
     interactor->SetInteractorStyle(style);
     style->camera = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();
     style->ui = this->ui;
     style->defualtDistance = this->defaultCameraDistance;

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
    this->buttonTransRotationPressed();

    /// We Update the Transformation Coordinates from the Camera Details
    ///
    this->updateTransformCoords();
    }
}

void MainWindow::on_actionReload_triggered()
{


//    //////////////////////////////////////////////////////////////////
//    ///////////VTK FITS READER CODE FROM FITS.CXX ////////////////////
//    //////////////////////////////////////////////////////////////////

//    //moveToThread(QApplication::instance()->thread());
//    //qRegisterMetaType<QTextCursor>("QTextCursor");

//    vtkCommand* pObserver = vtkProgressCommand::New();

//    // vtk pipeline
//    vtkFitsReader *fitsReader = vtkFitsReader::New();
//    const char *newFileName = filename.toStdString().c_str();
//    fitsReader->SetFileName(newFileName);
//    fitsReader->AddObserver(vtkCommand::ProgressEvent, pObserver );

//    // qDebug() << "Obtained Filename" << endl;
//    //fitsReader->PrintSelf();
//    //fitsReader->PrintDetails();
//    QApplication::processEvents();

    //qDebug() << "Gathering Points" << endl;


    //qDebug() << "Processing Points" << endl;


//    /////////////////////////////////
//    /// \brief Outline Object
//    ///
//    //qDebug() << "Adding Outline" << endl;
//    vtkOutlineFilter *outline = vtkOutlineFilter::New();
//    outline->AddObserver(vtkCommand::ProgressEvent, pObserver );
//    outline->SetInputConnection(fitsReader->GetOutputPort());

//    vtkPolyDataMapper *outlineMapper = vtkPolyDataMapper::New();
//    outlineMapper->SetInputConnection(outline->GetOutputPort());

//    vtkActor *outlineActor = vtkActor::New();
//    outlineActor->SetMapper(outlineMapper);
//    outlineActor->GetProperty()->SetColor(0.5,0.5,0.5);

//    global_Outline = outlineActor;


//    ///
//    /// \brief resample
//    ///
//    vtkImageResample *resample = vtkImageResample::New();

//    resample->SetInputConnection( fitsReader->GetOutputPort() );
//    resample->SetAxisMagnificationFactor(0, 1.0);
//    resample->SetAxisMagnificationFactor(1, 1.0);
//    resample->SetAxisMagnificationFactor(2, 1.0);

//    imageResample_ = resample;

//    /// Create the Volume & mapper

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

   // mapper->SetBlendModeToComposite();
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


    global_CubeAxes = cubeAxesActor;
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

    // add actors to renderer

    //qDebug() << "Adding Objects to RenderWindow" << endl;

    ren1->AddVolume(volume);


    this->ui->qvtkWidgetLeft->SetRenderWindow(renderWindow);
//    this->ui->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
//    style->SetCurrentStyleToTrackballCamera();
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

    AddScalarBar(this->ui->qvtkWidgetLeft, global_Reader);

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
     this->buttonTransRotationPressed();

//    this->ui->menuBar->setVisible(false);
//    this->ui->menuViews->setDisabled(true);
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

     this->ui->qvtkWidgetLeft->update();

    ///
    /// \brief Reset the Transformation Coordinates
    ///
    updateTransformCoords();;


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



void MainWindow::actionDefault_triggered()
{
    this->ui->qvtkWidgetLeft->update();
}



///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///     CAMERA BUTTON FOCUS VIEWS       ///////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

//void MainWindow::on_buttonCameraFront_clicked()
//{
//    on_actionFront_Side_View_triggered();
//}

//void MainWindow::on_buttonCameraRear_clicked()
//{
//    on_actionRear_Side_View_triggered();
//}

//void MainWindow::on_buttonCameraLeft_clicked()
//{
//    on_actionLeft_Side_View_triggered();
//}

//void MainWindow::on_buttonCameraRight_clicked()
//{
//    on_actionRight_Side_View_triggered();
//}

//void MainWindow::on_buttonCameraTop_clicked()
//{
//    on_actionTop_Side_View_triggered();
//}

//void MainWindow::on_buttonCameraBottom_clicked()
//{
//    on_actionBottom_Side_View_triggered();
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
    cubeActor->GetProperty()->SetOpacity(0.2);

    this->ui->qvtkWidgetLeft->GetInteractor()->GetRenderWindow()->
                GetRenderers()->GetFirstRenderer()->AddActor(cubeActor);

   // global_subVolume = cubeVolume;

    ////
    /// \brief vtkBoxWidget
    ///
    vtkEventQtSlotConnect* vtkEventConnector =
            vtkEventQtSlotConnect::New();

    boxWidget_ = vtkBoxWidget::New();
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

    boxWidget_->SetHandleSize(0.008);

    boxWidget_->EnabledOn();

    boxWidget_->InvokeEvent(vtkCommand::InteractionEvent);
}

void MainWindow::on_buttonSubVolReset_clicked()
{
    //on_actionReset_Camera_triggered();

    boxWidget_->PlaceWidget(global_Volume->GetBounds());
    boxWidget_->InvokeEvent(vtkCommand::InteractionEvent);

    /// Refresh the Interaction screen.
    this->ui->qvtkWidgetLeft->update();
}

void MainWindow::loadSubVolume(QVTKWidget *qvtkWidget, vtkFitsReader *source)
{
    double subVolBounds[6];
    global_subVolume->GetOutput()->GetBounds(subVolBounds);

    SubVolumeDialog subVol;

    subVol.setModal(true);
    subVol.UpdateQVTKWidget(this->ui->qvtkWidgetLeft, global_Reader, subVolBounds);
    connect(&subVol,SIGNAL(windowClosing(bool)),this,SLOT(infoTabCloseSignal(bool)));
//    //subVol
//    //subVol->ui->
    subVol.exec();
}

void MainWindow::on_checkSubVolVolumeEnable_stateChanged(int arg1)
{
    global_Volume->SetVisibility(arg1);
}



void MainWindow::Buttom_SubVol_clicked()
{
    loadSubVolume(this->ui->qvtkWidgetLeft, global_Reader );
}


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
    // Create a sample listener and Controller
    this->controller_= new Controller;
    Leaping_ = true;
    this->leapSubVolMode = true;

    this->ui->checkBox_SubVolume->setChecked(true);



    this->ui->tabLogWidget->insertTab(2, this->ui->LeapTab,"Leap Information");
    this->ui->tabLogWidget->setCurrentIndex(2);

    this->ui->tabLogWidget->insertTab(0, this->ui->SubVolTab,"Sub-Volume Extraction");
    this->ui->tabLogWidget->setCurrentIndex(0);

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
        }
    }

//    PrintBounds("subVol", global_subVolBounds_);

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
    subVolume->SetInput(global_Reader->GetOutput());

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

    distanceRep->Print(std::cout);

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

void MainWindow::on_actionLeap_Slice_triggered()
{
    this->controller_= new Controller;
   Leaping_ = true;
   this->leapAxisSliceMode = true;


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
    lut3D->SetTableRange(0,1000);
    lut3D->SetSaturationRange(0,0);
    lut3D->SetHueRange(0,0);
    lut3D->SetValueRange(0,1);

    slice->SetLookupTable(lut3D);

    /// Assign the LookupTable to the Global Pointer
    lookupTable3D = lut3D;

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

    vtkSmartPointer<vtkMySliceCallback> ProbeData =
            vtkSmartPointer<vtkMySliceCallback>::New();
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

//    if (status)
//    /////////////////////////////////////////////////
//    ///  Mouse Rotation
//    ///
//    ///
//    MouseInteractorStyle * style = MouseInteractorStyle::New();


//    vtkRenderWindowInteractor * interactor = this->ui->qvtkWidgetLeft->GetInteractor();
//    interactor->SetInteractorStyle(style);
//    style->camera = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();
//    style->ui = this->ui;
//    style->defualtDistance = this->defaultCameraDistance;

//    ///
//    /// \brief Set the Position
//    ///
//    double * position = customArbPlaneWidget->GetOrigin();
//    double * angle   = customArbPlaneWidget->GetNormal();


//    this->ui->lineArbSlicePosX->setText(QString::number(position[0], 'f', 0));
//    this->ui->lineArbSlicePosY->setText(QString::number(position[1], 'f', 0));
//    this->ui->lineArbSlicePosZ->setText(QString::number(position[2], 'f', 0));

//    this->ui->lineArbSliceAngleX->setText(QString::number(angle[0], 'f', 0));
//    this->ui->lineArbSliceAngleY->setText(QString::number(angle[1], 'f', 0));
//    this->ui->lineArbSliceAngleZ->setText(QString::number(angle[2], 'f', 0));
}

void MainWindow::on_buttonArbReset_clicked()
{
    customArbPlaneWidget->PlaceWidget(global_Volume->GetBounds());
    customArbPlaneWidget->InvokeEvent(vtkCommand::EnableEvent);
    customArbPlaneWidget->InvokeEvent(vtkCommand::StartInteractionEvent);
    customArbPlaneWidget->InvokeEvent(vtkCommand::InteractionEvent);
    this->ui->qvtkWidgetLeft->update();

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

void MainWindow::on_radioArbSliceContourCount10_clicked()
{
    global_Contours2D->SetNumberOfContours(10);
    global_Contours2D->GenerateValues(10, global_Reader->GetOutput()->GetScalarRange());
    this->ui->widget_ArbPlanePreview->GetRenderWindow()->Render();
}

void MainWindow::on_radioArbSliceContourCount20_clicked()
{
    global_Contours2D->SetNumberOfContours(20);
    global_Contours2D->GenerateValues(20, global_Reader->GetOutput()->GetScalarRange());
    this->ui->widget_ArbPlanePreview->GetRenderWindow()->Render();


}

void MainWindow::on_radioArbSliceContourCount30_clicked()
{
    global_Contours2D->SetNumberOfContours(30);
    global_Contours2D->GenerateValues(30, global_Reader->GetOutput()->GetScalarRange());
    this->ui->widget_ArbPlanePreview->GetRenderWindow()->Render();
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
        this->leapTrackingActor->SetVisibility(arg1);
}

void MainWindow::on_actionLeapBasic_triggered()
{
    this->controller_= new Controller;
   Leaping_ = true;
   //this->leapSliceMode = true;
   //std::cout << "Leaping " << endl;
   //CloseTabs();

   this->ui->tabLogWidget->insertTab(0, this->ui->LeapTab, "Leap Data Tab");
   this->ui->tabLogWidget->setCurrentIndex(0);

   vtkSmartPointer<KeyPressInteractorStyle> style =
           vtkSmartPointer<KeyPressInteractorStyle>::New();

   this->ui->qvtkWidgetLeft->GetInteractor()->SetInteractorStyle(style);
   style->SetCurrentRenderer(this->defaultRenderer);

   //this->rotationPress = this->ui->checkBox_Rotation;
   style->rotation = this->ui->checkBox_Rotation;
   style->translation = this->ui->checkBox_Translation;
   style->scaling = this->ui->checkBox_Scaling;
   style->mainWindow = this;

   this->leapMatrixTotalMotionRotation = Leap::Matrix::identity();
   this->leapVectorTotalMotionalTranslation = Leap::Vector::zero();
   this->leapFloatTotalMotionScale = 1.0f;

   LeapDiagnostic();

   this->ui->qvtkWidgetLeft->setFocus();

}


void MainWindow::LeapMotion()
{
    QFont boldFont, normalFont;

    boldFont.setBold(true);
    normalFont.setBold(false);       
    int currentFreq = 1;


    /// IMPORTANT: Visualizer Window must be RENDERED B4 colour notification otherwise it will only stick 2 1 colour
    ///
    ///if(this->ui->checkBox_Diagnostic->isChecked() && leapFrameFreqCount == currentFreq)
    if(this->ui->checkBox_Diagnostic->isChecked())
    {
        this->ui->widget_LeapVisualizer->GetRenderWindow()->Render();
        leapDbgSphereActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
        leapDbgPointWidget->GetProperty()->SetColor(1.0, 1.0, 1.0);
    }    






    if(controller_->isConnected())  // controller is a controller object
    {
        // Get the most recent frame and report some basic information
        const Frame frame = controller_->frame();
        //const FingerList fingers = controller_->frame().fingers();



        //controller_->enableGesture(Gesture::TYPE_SCREEN_TAP, true);
        controller_->enableGesture(Gesture::TYPE_SCREEN_TAP, true);                

        bool shouldTranslate = true;
        bool shouldRotate = true;
        bool shouldScale = true;
        bool shouldSubVol = this->leapSubVolMode;
        bool shouldAxisSlice =  this->leapAxisSliceMode;
        bool shouldArbSlice = this->leapArbSliceMode;

        //bool shouldCheck  = false;

        //bool chkTranslate = this->ui->checkBox_Translation->isChecked();
        //bool chkRotate = this->ui->checkBox_Rotation->isChecked();
        //bool chkScale = this->ui->checkBox_Scaling->isChecked();

        bool chkTranslate = !this->ui->buttonTransfTranslation->isEnabled() && this->ui->checkBoxLeapTracking->isChecked();
        bool chkRotate = !this->ui->buttonTransfRotation->isEnabled() && this->ui->checkBoxLeapTracking->isChecked();
        bool chkScale = !this->ui->buttonTransfScaling->isEnabled() && this->ui->checkBoxLeapTracking->isChecked();

        this->leapTrackingActor->SetVisibility(this->ui->checkBoxLeapTracking->isChecked());

        vtkRenderer * renderer = this->defaultRenderer;
        vtkCamera *camera = renderer->GetActiveCamera();


        // Get the most recent frame and report some basic information
        //                std::cout   << "Frame id: " << frame.id()
        //                            << ", timestamp: " << frame.timestamp()
        //                            << ", hands: " << frame.hands().count()
        //                            << ", fingers: " << frame.fingers().count()
                                    //<< ", fingers: " << fingers.count()
        //                            //<< ", Rotation (Y): " << shouldRotate
        //                            //<< ", Translate (Y): " << shouldTranslate
        //                            //<< ", Scale (Y): " << shouldScale
        //                            //<< ", tools: " << frame.tools().count()
        //                            //<< ", gestures: " << frame.gestures().count()
        //                            //<< ", PalmNormal: " << frame.hands()[0].palmNormal()
        //                            //<< ", PalmDirect: " << frame.hands()[0].direction()
        //                            << ", yaw: " << frame.hands()[0].direction().yaw()
        //                            //<< ", Angle: " << frame.rotationAngle(controller_->frame(1))
        //                            << std::endl;

//        const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};

//        // Get fingers
//            const FingerList fingers = frame.hands().rightmost().fingers();
//            const FingerList openFingers = frame.hands()[0].fingers().extended();
//            std::cout   << "Fingers" <<  openFingers.count() << "------------------------" << endl;
//            for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
//              const Finger finger = *fl;
//              std::cout << std::string(4, ' ') <<  fingerNames[finger.type()]
//                        << " finger, id: " << finger.id()
//                        << ", length: " << finger.length()
//                        << ",direction:"<< finger.direction()
//                        << "mm, width: " << finger.width() << std::endl;
//            }

//        if (shouldSubVol )
//        {
//            const GestureList gestures = frame.gestures();
//            for (int g = 0; g < gestures.count(); ++g)
//            {
//                Gesture gesture = gestures[g];

//                if (gesture.type() == Gesture::TYPE_SCREEN_TAP)
//                {
//                    std::cout << "KeyTapp Pressed" << std::endl;
//                    if (!this->ui->checkBox_Rotation->isChecked())
//                    {
//                        std::cout << "Should Check" << std::endl;
//                        this->ui->checkBox_Rotation->setChecked(true);
//                    }
//                    else
//                    {
//                        std::cout << "Should Export" << std::endl;
//                        on_actionSubVol_Export_triggered();
//                    }
//                }
//            }
//        }

        if (shouldAxisSlice)
        {

        }

        if (shouldSubVol)
        {
            vtkProperty * pointerProperty =
                    pointWidget1_->GetProperty();

            pointerProperty->SetColor(1, 1,1 );

            pointerProperty = pointWidget2_->GetProperty();

            pointerProperty->SetColor(1,1,1);
        }


        if (!frame.hands().isEmpty() && !frame.hands()[0].fingers().isEmpty())
        {





//            int handCount = 0;
            Hand hand = frame.hands().rightmost();
            std::cout << hand.toString() << endl;


            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////  AXIS  SLICE TRACKING  /// //////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            if((shouldAxisSlice) && (frame.hands().count() ==1))
            {                                
                const FingerList rightFingers = frame.hands().rightmost().fingers();
                const FingerList extendedRight = frame.hands().rightmost().fingers().extended();

                Finger rightThumb = (frame.hands().rightmost().fingers().fingerType(Finger::TYPE_THUMB))[0];
                Finger rightIndex = (frame.hands().rightmost().fingers().fingerType(Finger::TYPE_INDEX))[0];

                 //leapMovement = false;

                std::cout << " 1. Movement = " << leapMovement << "\t";

                if(frame.hands().rightmost().fingers().frontmost().id() == rightIndex.id() &&
                  rightThumb.isExtended())
                {


                    //std::cout << "Slice Mode" << endl;
                    double moveFactor = 2.0;

                    Vector newPosition = frame.translation(controller_->frame(1));



                    ///
                    ///Translate the X Axis Left and Right  (RED)
                    ///

                    if (this->ui->checkBox_SliceX_Enable->isChecked())
                    {
                        leapMovement = true;

                        int changeX = (int) (newPosition.x * moveFactor);

                        int currentPosX = (static_cast<vtkSliderRepresentation *>
                                           (sliderWidgetX->GetRepresentation()))->GetValue();

                        currentPosX += changeX;

                        (static_cast<vtkSliderRepresentation *>
                                (sliderWidgetX->GetRepresentation()))->SetValue(currentPosX);

                        std::cout << " 2. Movement = " << leapMovement << "\t";
                    }

                    ///
                    ///Translate the Y Axis Up and Down     (GREEN)
                    ///

                    if (this->ui->checkBox_SliceY_Enable->isChecked())
                    {
                        leapMovement = true;

                        int changeY = (int) (newPosition.y * moveFactor);

                        int currentPosY = (static_cast<vtkSliderRepresentation *>
                                           (sliderWidgetY->GetRepresentation()))->GetValue();

                        currentPosY += changeY;

                        (static_cast<vtkSliderRepresentation *>
                                (sliderWidgetY->GetRepresentation()))->SetValue(currentPosY);

                        std::cout << " 3. Movement = " << leapMovement << "\t";
                    }


                    ///
                    ///Translate the Z Axis Forward and Backward    (BLUE)
                    ///

                    if (this->ui->checkBox_SliceZ_Enable->isChecked())
                    {
                        leapMovement = true;


                        int changeZ = (int) (newPosition.z * moveFactor);

                        int currentPosZ = (static_cast<vtkSliderRepresentation *>
                                           (sliderWidgetZ->GetRepresentation()))->GetValue();

                        currentPosZ += changeZ;

                        (static_cast<vtkSliderRepresentation *>
                                (sliderWidgetZ->GetRepresentation()))->SetValue(currentPosZ);

                        std::cout << " 4. Movement = " << leapMovement << "\t";

                    }

                    //sliderWidgetX->InvokeEvent(vtkCommand::InteractionEvent);
                    //sliderWidgetY->InvokeEvent(vtkCommand::InteractionEvent);
                    //sliderWidgetZ->InvokeEvent(vtkCommand::InteractionEvent);



//                    std::cout << "X: " << currentPosX << "\t = " << changeX << "\t" ;
//                    std::cout << "Y: " << currentPosY << "\t = " << changeY << "\t" ;
//                    std::cout << "Z: " << currentPosZ << "\t = " << changeZ << "\t" ;
//                    std::cout << endl;

                    std::cout << " 5. Movement = " << leapMovement << "\t";
                }

                std::cout << " 6. Movement = " << leapMovement << "\t";

                if(frame.hands().rightmost().fingers().frontmost().id() == rightIndex.id() &&
                        !rightThumb.isExtended() && leapMovement)
                {

                    std::cout << " 7. Movement = " << leapMovement << "\t";

                    if (this->ui->checkBox_SliceX_Enable->isChecked())
                        this->UpdateSliceX();

                    if (this->ui->checkBox_SliceY_Enable->isChecked())
                        this->UpdateSliceY();

                    if (this->ui->checkBox_SliceZ_Enable->isChecked())
                        this->UpdateSliceZ();

                    leapMovement = false;
                }

                std::cout << " 8. Movement = " << leapMovement << "\t";

                std::cout << endl;

            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////  ARB  SLICE TRACKING  /// //////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            if((shouldArbSlice) && (frame.hands().count() ==1))
            {
                const FingerList rightFingers = frame.hands().rightmost().fingers();
                const FingerList extendedRight = frame.hands().rightmost().fingers().extended();

                Finger rightThumb = (frame.hands().rightmost().fingers().fingerType(Finger::TYPE_THUMB))[0];
                Finger rightIndex = (frame.hands().rightmost().fingers().fingerType(Finger::TYPE_INDEX))[0];

                 //leapMovement = false;

              //  std::cout << " 1. Movement = " << leapMovement << "\t";

                if(extendedRight.count() >=4)
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

                    this->customArbPlaneWidget->InvokeEvent(vtkCommand::EnableEvent);
                    this->customArbPlaneWidget->InvokeEvent(vtkCommand::StartInteractionEvent);
                    this->customArbPlaneWidget->InvokeEvent(vtkCommand::InteractionEvent);
                }

                if(extendedRight.count() <=2)
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

                    ///////////////////////////////////////////////////////////

                    this->customArbPlaneWidget->InvokeEvent(vtkCommand::EnableEvent);
                    this->customArbPlaneWidget->InvokeEvent(vtkCommand::StartInteractionEvent);
                    this->customArbPlaneWidget->InvokeEvent(vtkCommand::InteractionEvent);
                }       
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    SUB-VOL TRACKING  /// //////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            if((shouldSubVol) &&  frame.hands().count() > 1)
            {
               const FingerList leftFingers = frame.hands().leftmost().fingers();
               const FingerList rightFingers = frame.hands().rightmost().fingers();
               const FingerList extendedLeft = frame.hands().leftmost().fingers().extended();
               const FingerList extendedRight = frame.hands().rightmost().fingers().extended();

//               const FingerList indexFingerLeftList = frame.hands().leftmost().fingers().fingerType(Finger::TYPE_INDEX);
//               const FingerList indexFingerRightList = frame.hands().rightmost().fingers().fingerType(Finger::TYPE_INDEX);

//               const FingerList thumbFingerLeftList = frame.hands().leftmost().fingers().fingerType(Finger::TYPE_THUMB);
//               const FingerList thumbFingerRightList = frame.hands().rightmost().fingers().fingerType(Finger::TYPE_THUMB);


                Finger leftThumb = (frame.hands().leftmost().fingers().fingerType(Finger::TYPE_THUMB))[0];
                Finger rightThumb = (frame.hands().rightmost().fingers().fingerType(Finger::TYPE_THUMB))[0];

                Finger leftIndex = (frame.hands().leftmost().fingers().fingerType(Finger::TYPE_INDEX))[0];
                Finger rightIndex = (frame.hands().rightmost().fingers().fingerType(Finger::TYPE_INDEX))[0];


                std::cout << "Right Thumb out: " << rightThumb.isExtended() << " \t" ;
                std::cout << "Left  Thumb out: " << leftThumb.isExtended() << " \t" ;

                std::cout << "Right Index out: " << rightIndex.isExtended() << " \t" ;
                std::cout << "Left  Index out: " << leftIndex.isExtended() << " \t" ;

                std::cout << "left: #" << extendedLeft.count() << "\t";
                std::cout << "Right #: " << extendedRight.count() << "\ t";
                std::cout << endl;



                if(frame.hands().leftmost().fingers().frontmost().id() == leftIndex.id()  &&
                   frame.hands().rightmost().fingers().frontmost().id() == rightIndex.id() &&
                   leftThumb.isExtended() && rightThumb.isExtended() )
                {

                    ////////////////////////////////////////////////////////////////////////////////////////////
                    ///////////////    FINGER ACCURACY BUT POOR DEPTH CAPTURE    ///////////////////////////////
                    ////////////////////////////////////////////////////////////////////////////////////////////

                    /// Vector newPosition = hand.translation(controller_->frame(1));
                    ///Vector newPosition = hand.translation(controller_->frame(1))..fingers().frontmost().);

                    Vector hand1OldPos = controller_->frame(2).hands().rightmost().fingers().frontmost().stabilizedTipPosition();
                    Vector hand1NewPos = controller_->frame(1).hands().rightmost().fingers().frontmost().stabilizedTipPosition();

                    double change1[3] = {
                        hand1NewPos.x - hand1OldPos.x,
                        hand1NewPos.y - hand1OldPos.y,
                        hand1NewPos.z - hand1OldPos.z
                    };
                    double position[3];


                    pointWidget1_->GetPosition(position);

                    pointWidget1_->SetPosition(
                                position[0] + change1[0],
                            position[1] + change1[1],
                            position[2] + change1[2]);

                    pointWidget1_->InvokeEvent(vtkCommand::InteractionEvent);

                    vtkProperty * pointerProperty =
                            pointWidget1_->GetProperty();

                    pointerProperty->SetColor(0.3400, 0.8100, 0.8900);


                    ////////////////////////////////////////////////////////
                    /// \brief Second Hand capture
                    ///


                    ///Vector newPosition2 = hand2.translation(controller_->frame(1));
                    ///Vector newPosition2 = hand2.translation(controller_->frame(1).fingers().frontmost().tipPosition());
                    ///

                    Vector hand2OldPos = controller_->frame(2).hands().leftmost().fingers().frontmost().stabilizedTipPosition();
                    Vector hand2NewPos = controller_->frame(1).hands().leftmost().fingers().frontmost().stabilizedTipPosition();

                    double change2[3] = {
                        hand2NewPos.x - hand2OldPos.x,
                        hand2NewPos.y - hand2OldPos.y,
                        hand2NewPos.z - hand2OldPos.z
                    };

                    double position2[3];

                    pointWidget2_->GetPosition(position2);

                    pointWidget2_->SetPosition(
                                position2[0] + change2[0] ,
                            position2[1] + change2[1] ,
                            position2[2] + change2[2] );

                    pointWidget2_->InvokeEvent(vtkCommand::InteractionEvent);

                    pointerProperty =
                            pointWidget2_->GetProperty();

                    pointerProperty->SetColor(0.8900, 0.8100, 0.3400);

                    double* finalPos1 = pointWidget1_->GetPosition();
                    double* finalPos2 = pointWidget2_->GetPosition();

                    trackSubVolume(finalPos1, finalPos2);

                }
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    TRANSLATION   //////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            if(chkTranslate)
            {                
                this->ui->labelTranslation->setFont(boldFont);

                //////////////////////////////////////////////////////////////////////////////////////
                /// \brief newPosition - CURRENT REPRESENTATION
                /////////////////////////////////////////////////////////////////////////////////////

                Vector newPosition = frame.translation(controller_->frame(1));
                Vector newTranslation = frame.translation(controller_->frame(1));


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

            }


            //////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    ROTATION   /////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////            
            if(chkRotate)
            {
                this->ui->labelRotation->setFont(boldFont);
                
                Matrix newRotation = frame.rotationMatrix(controller_->frame(2));
                //std::cout << newRotation.toString() << endl;

                vtkMatrix4x4* cameraMatrix = camera->GetModelTransformMatrix();

                vtkMatrix4x4* rotationMatrix = vtkMatrix4x4::New();

                rotationMatrix->Identity();
                int index = 0;                

                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < 4; j++)
                    {                 
                        rotationMatrix->SetElement(i,j,newRotation.toArray4x4().m_array[index]);                        
                        index++;
                    }                    
                }

                vtkTransform* transform = vtkTransform::New();

                transform ->Identity();

                transform->SetMatrix(cameraMatrix);

                transform->Concatenate(rotationMatrix);

                camera->ApplyTransform(transform);

                camera->SetFocalPoint(cameraFocalPoint);

                //renderer->ResetCameraClippingRange();

            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    SCALING       //////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            if(chkScale)
            {

                this->ui->labelScaling->setFont(boldFont);

                //float adjustmentFactor = 0.5;
                //float scaleFactor = frame.scaleFactor(controller_->frame(1));
                float scaleFactor = frame.hands()[0].scaleFactor(controller_->frame(2));


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
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    DIAGNOSTIC TRACKING  /// //////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////
            ///
            //qDebug() << "Hand Tracking " << endl ;

            ///if (this->ui->checkBox_Diagnostic->isChecked() && leapFrameFreqCount == currentFreq)
            if (this->ui->checkBox_Diagnostic->isChecked())
            {

                std::cout << "Frame ID:" << frame.id() << "\t FPS:" << frame.currentFramesPerSecond() << endl;
                //////////////////////////////////////////////////////////////////////////
                /// \brief handPos
                //////////////////////////////////////////////////////////////////////////
                ////////////////////////////////////////////////////////////////////////

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

                leapDbgPointWidget->SetPosition(handPosPoint);//handPos.

                leapDbgPointWidget->GetProperty()->SetColor(1.0, 0.0, 0.0);


                /////////////////////////////////////////////////////////////////////////////
                /// \brief newNormal
                ///


                Vector newNormal = hand.palmNormal();

                double oldNormal[3] ;
                leapDbgPlaneWidget->GetNormal(oldNormal);
                double newNormalD [3];
                double theta, rotVector[3];

                double *point1 = leapDbgPlaneWidget->GetPoint1();
                double *origin = leapDbgPlaneWidget->GetOrigin();
                //double *center = leapDbgPlaneWidget->GetCenter();


                newNormalD[0] = newNormal.x;
                newNormalD[1] = newNormal.y;
                newNormalD[2] = newNormal.z;

                leapDbgPlaneWidget->SetNormal(newNormal.x, newNormal.y, newNormal.z);
                leapDbgPlaneWidget->UpdatePlacement();


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

                leapDbgArrowActor->RotateWXYZ(theta, rotVector[0], rotVector[1], rotVector[2]);
                leapDbgSphereActor->RotateWXYZ(theta, rotVector[0], rotVector[1], rotVector[2]);
                leapDbgSphereActor->GetProperty()->SetColor(0.0, 1.0, 0.0 );

                //this->ui->widget_LeapVisualizer->update();
                leapFrameFreqCount = 0;

            }

            leapFrameFreqCount++;
            ////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////
            ///KEYBOARD FOCUS SET TO WIDGET TO MAINTAIN KEYBOARD INTERACTION

            this->ui->qvtkWidgetLeft->setFocus();

        }
    }
}

void MainWindow::LeapDiagnostic()
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

    renWin->SetSize(50,10);
    renWin->AddRenderer(renderer);    

    renderer->SetBackground(0.1, 0.2, 0.4);

    this->ui->widget_LeapVisualizer->SetRenderWindow(renWin);    

    renderer->AddActor(sphereActor);

    sphereSource->GetCenter(sphereCenter);
    sphereRadius =  sphereSource->GetRadius();    

    leapDbgSphere = sphereSource;
    leapDbgSphereActor = sphereActor;
    sphereActor->RotateY(-90);

    /// Get the Interactor
    vtkRenderWindowInteractor * interactor = this->ui->widget_LeapVisualizer->GetInteractor();

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

    leapDbgPlaneWidget->SetInteractor(this->ui->widget_LeapVisualizer->GetInteractor());
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
    leapDbgPointWidget->SetInteractor(this->ui->widget_LeapVisualizer->GetInteractor());
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

    AddDistanceMeasurementToView1();
    // remove existing widgets.
    if (this->lineWidget[0])
    {
        this->lineWidget[0]->SetEnabled(0);
        this->lineWidget[0] = NULL;
    }

    // add new widget
    this->lineWidget[0] = vtkSmartPointer<vtkLineWidget>::New();
    this->lineWidget[0]->SetInteractor(
                this->riw[0]->GetResliceCursorWidget()->GetInteractor());

    // Set a priority higher than our reslice cursor widget
    this->lineWidget[0]->SetPriority(
                this->riw[0]->GetResliceCursorWidget()->GetPriority() + 0.01);

    vtkSmartPointer<vtkLineCallback> lineCallback =
            vtkSmartPointer<vtkLineCallback>::New();
    lineCallback->riw = riw[0];

    lineWidget[0]->AddObserver(vtkCommand::InteractionEvent, lineCallback);


    vtkSmartPointer<vtkLineRepresentation> lineRepresentation =
          vtkSmartPointer<vtkLineRepresentation>::New();


    double pos1 [3] = {115, 31.87, 13.05};
    double pos2 [3] = {115, 31.87, 40.55};
    lineRepresentation->SetPoint1DisplayPosition(pos1);
    lineRepresentation->SetPoint2DisplayPosition(pos2);

    lineWidget[0]->SetPoint1(pos1);
    lineWidget[0]->SetPoint2(pos2);

    lineWidget[0]->On();


    riw[0]->Render();


    vtkSmartPointer<MouseInteractorStyle> style =
            vtkSmartPointer<MouseInteractorStyle>::New();
    style->SetDefaultRenderer(this->riw[0]->GetResliceCursorWidget()->GetCurrentRenderer());
    this->riw[0]->GetInteractor()->SetInteractorStyle(style);
    this->riw[0]->GetResliceCursorWidget()->GetInteractor()->SetInteractorStyle(style);
//    //this->ui->sliceView1->update();
//    riw[0]->GetInteractor()->SetInteractorStyle(style);



}



void MainWindow::closeTab(int index)
{
    this->ui->tabLogWidget->removeTab(index);
    //delete this->ui->tabLogWidget->widget(index);
}

void MainWindow::updateTransformCoords()
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
     double* position;

     position = camera->GetPosition();

     ui->line_PosX->setText(QString::number(position[0], 'f', 0));
     ui->line_PosY->setText(QString::number(position[1], 'f', 0));
     ui->line_PosZ->setText(QString::number(position[2], 'f', 0));

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
//    } else if (event->type() == MyCustomEventType) {
//    MyCustomEvent *myEvent = static_cast<MyCustomEvent *>(event);
//    // custom event handling here
//    return true;
//    }

    }
   return QWidget::event(event);
}





