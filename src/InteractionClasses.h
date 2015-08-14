#ifndef INTERACTIONCLASSES_H
#define INTERACTIONCLASSES_H
#include <QtGui>
#include <src/mainwindow.h>
#include "ui_mainwindow.h"

#include "vtkRenderWindow.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkPolyData.h"
#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkLineWidget.h"
#include "vtkTextActor.h"
#include "vtkImplicitPlaneWidget.h"
#include "vtkPlaneSource.h"
#include "vtkSliderRepresentation.h"
#include "vtkSliderWidget.h"
#include "vtkPlane.h"

#include "vtkResliceImageViewer.h"
#include "vtkImagePlaneWidget.h"
#include "vtkResliceCursor.h"
#include "vtkInteractorObserver.h"

#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkInteractorStyleJoystickCamera.h"
#include "vtkInteractorStyle.h"
#include "vtkInteractorStyleSwitch.h"

#include <vtkCallbackCommand.h>


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

class vtkCameraScaleCallback : public vtkCommand
{
    public:

        static vtkCameraScaleCallback * New(){
            return new vtkCameraScaleCallback();};



    virtual void Execute(vtkObject *caller, unsigned long , void *)
    {         
         vtkCamera *cam = reinterpret_cast<vtkCamera*> (caller);

//         // In case you would need this too:
//         vtkMatrix4x4* mat = cam->GetViewTransformMatrix();

//         cout << "View matrix: " << endl;
//         cout << "================================================================================= " << endl;
//         cout << mat->GetElement(0, 0) << "\t" << mat->GetElement(0, 1) << "\t" << mat->GetElement(0, 2) << "\t" << mat->GetElement(0, 3) << "\t" << endl;
//         cout << mat->GetElement(1, 0) << "\t" << mat->GetElement(1, 1) << "\t" << mat->GetElement(1, 2) << "\t" << mat->GetElement(1, 3) << "\t" << endl;
//         cout << mat->GetElement(2, 0) << "\t" << mat->GetElement(2, 1) << "\t" << mat->GetElement(2, 2) << "\t" << mat->GetElement(2, 3) << "\t" << endl;
//         cout << mat->GetElement(3, 0) << "\t" << mat->GetElement(3, 1) << "\t" << mat->GetElement(3, 2) << "\t" << mat->GetElement(3, 3) << "\t" << endl;
//         cout << "================================================================================= " << endl;
//         double* x = cam->GetOrientation();
//         cout << "Orientation : " << x[0] << " " << x[1] << " " << x[2] << endl;


//         /// Camera Rotate
//         ///
//         ///
         double * orientation = cam->GetOrientation();

         ui->line_OrientX->setText(QString::number(orientation[0], 'f', 0));
         ui->line_OrientY->setText(QString::number(orientation[1], 'f', 0));
         ui->line_OrientZ->setText(QString::number(orientation[2], 'f', 0));

         /// Camera Zoom

         double scale ;

         scale = this->defualtCameraDistance /  cam->GetDistance();

         ui->line_Scale->setText(QString::number(scale, 'f', 2));




     }

        Ui::MainWindow * ui;
        double defualtCameraDistance;


};


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

       // std::cout << "X = " << normal[0] << "\tY= " << normal[1] << "\tZ  " << normal[2] << " \t";
       // std::cout << endl;

    }

    vtkMySliceCallback (): polyPlane(0), plane(0), cutter(0), camera(0) {}
    vtkPolyData* polyPlane;
    vtkPlane* plane;
    vtkCutter* cutter;
    vtkCamera* camera;
    vtkRenderWindow* renderWindow;
    Ui::MainWindow * ui;


};

class TouchInteractorStyleTrackBallCamera : public vtkInteractorStyleTrackballCamera
{
    public:
    static TouchInteractorStyleTrackBallCamera* New();

    virtual void OnRightButtonDown()
    {
        vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();

       // std::cout << "Translation triggered" << endl;
    }

    virtual void OnRightButtonUp()
    {
        vtkInteractorStyleTrackballCamera::OnMiddleButtonUp();
    }

    virtual void OnMouseMove()
    {
        vtkInteractorStyleTrackballCamera::OnMouseMove();

        //this->GetInteractor()->GetRenderWindow()->Render();

//        switch (this->GetState())
//        {
//            case 0: std::cout << "Start/Stop" << endl;
//                break;

//            case 1: std::cout << "Rotate" << endl;
//                break;

//            case 2: std::cout << "Pan" << endl;
//                break;

//            case 3: std::cout << "Spin" << endl;
//                break;

//            case 4: std::cout << "Dolly" << endl;
//                break;

//            case 5: std::cout << "Zoom" << endl;
//                break;
//        default : std::cout << "Defualt" << endl;

//        }
    }

    virtual void OnMiddleButtonDown()
    {
       // vtkInteractorStyleTrackballCamera::OnMiddleButtonUp();

        //std::cout << "Translation Finished" << endl;

    }

    virtual void OnLeftButtonDown()
    {
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

//       std::cout << "Touch "  << orientation[0] << ", "
//                                << orientation[1] << ", "
//                                << orientation[2] << endl;


   }

    virtual void Spin()
   {


       vtkInteractorStyleTrackballCamera::Spin();

       std::cout << "We Spinning !!!" << endl;

       double* orientation;

       //camera->Print(std::cout);

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

//        std::cout << "Touch "  << position[0] << ", "
//                                 << position[1] << ", "
//                                 << position[2] << endl;
    }

    virtual void Dolly()
    {
        vtkInteractorStyleTrackballCamera::Dolly();

        ui->buttonTransfScaling->setEnabled(true);
    }


    vtkCamera * camera;
    Ui::MainWindow * ui;
    double defualtDistance;

};

// Define interaction style
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{

public:
    static MouseInteractorStyle* New();
    vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonDown()
    {

      vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }

    virtual void OnLeftButtonUp()
    {
        ui->buttonTransfRotation->setEnabled(false);

        ui->buttonTransfScaling->setEnabled(false);
        vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
    }

    virtual void OnRightButtonDown()
    {
        vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
    }

    virtual void OnRightButtonUp()
    {
        vtkInteractorStyleTrackballCamera::OnMiddleButtonUp();

        ui->buttonTransfTranslation->setEnabled(false);
    }

    virtual void OnMiddleButtonDown()
    {
        vtkInteractorStyleTrackballCamera::OnRightButtonDown();

        ui->buttonTransfScaling->setEnabled(true);

    }

    virtual void OnMiddleButtonUp()
    {
        vtkInteractorStyleTrackballCamera::OnRightButtonUp();

        ui->buttonTransfScaling->setEnabled(false);

    }

    virtual void OnMouseWheelBackward()
    {
        this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                                this->Interactor->GetEventPosition()[1]);
        if (this->CurrentRenderer == NULL)
          {
          return;
          }

        this->GrabFocus(this->EventCallbackCommand);
        this->StartDolly();
        ui->buttonTransfScaling->setEnabled(true);
        double factor = this->MotionFactor * 0.2 * this->MouseWheelMotionFactor;
        this->Dolly(pow(1.1, factor));
        this->EndDolly();
        ui->buttonTransfScaling->setEnabled(false);
        this->ReleaseFocus();
    }

    virtual void OnMouseWheelForward()
    {
        ui->buttonTransfScaling->setEnabled(false);
        vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
    }

    virtual void Zoom()
    {
        vtkInteractorStyleTrackballCamera::Zoom();

        std::cout << "Zoom" << endl;
        ui->buttonTransfScaling->setEnabled(true);
    }



    virtual void Rotate()
   {


       vtkInteractorStyleTrackballCamera::Rotate();

       double* orientation;

       orientation  = camera->GetOrientation();

       ui->line_OrientX->setText(QString::number(orientation[0], 'f', 0));
       ui->line_OrientY->setText(QString::number(orientation[1], 'f', 0));
       ui->line_OrientZ->setText(QString::number(orientation[2], 'f', 0));

     ui->buttonTransfRotation->setEnabled(true);

   }


    virtual void Pan()
    {


        vtkInteractorStyleTrackballCamera::Pan();

        double* position;

        position = camera->GetPosition();

        ui->line_PosX->setText(QString::number(position[0], 'f', 0));
        ui->line_PosY->setText(QString::number(position[1], 'f', 0));
        ui->line_PosZ->setText(QString::number(position[2], 'f', 0));

        ui->buttonTransfTranslation->setEnabled(true);
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
      std::cout << "Pressed " << key << std::endl;

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
              //this->mainWindow->leapTransfNoneClicked();
          }
          else
          {
              rotation->setChecked(true);
              //this->mainWindow->leapTransfRotationClicked();
          }

      }


      if(key == "t")
      {
          if (this->translation->isChecked())
          {
              translation->setChecked(false);
              //this->mainWindow->leapTransfNoneClicked();
          }
          else
          {
              translation->setChecked(true);
            //  this->mainWindow->leapTransfTranslationClicked();
          }

      }

      if(key == "s")
      {
          if (this->scaling->isChecked())
          {
              scaling->setChecked(false);
            //  this->mainWindow->leapTransfNoneClicked();
          }
          else
          {
              scaling->setChecked(true);
           //   this->mainWindow->leapTransfScalingClicked();
          }

      }

      if(key == "c")
      {
          mainWindow->KeyboardResetCamera();
      }


      // Forward events
      vtkInteractorStyleTrackballCamera::OnKeyPress();


    }

    virtual void Rotate()
   {


       vtkInteractorStyleTrackballCamera::Rotate();

       double* orientation;

       //camera->Print(std::cout);

       orientation  = camera->GetOrientation();

       ui->line_OrientX->setText(QString::number(orientation[0], 'f', 0));
       ui->line_OrientY->setText(QString::number(orientation[1], 'f', 0));
       ui->line_OrientZ->setText(QString::number(orientation[2], 'f', 0));


   }




//    virtual void Pan()
//    {


//        vtkInteractorStyleTrackballCamera::Pan();

//        double* position;

//        position = camera->GetPosition();

//        ui->line_PosX->setText(QString::number(position[0], 'f', 0));
//        ui->line_PosY->setText(QString::number(position[1], 'f', 0));
//        ui->line_PosZ->setText(QString::number(position[2], 'f', 0));
//    }

//    virtual void Dolly()
//    {

//        vtkInteractorStyleTrackballCamera::Dolly();

//        double value ;

//        value = this->defualtDistance /  this->GetCurrentRenderer()->GetActiveCamera()->GetDistance();

//        ui->line_Scale->setText(QString::number(value, 'f', 2));
//    }


    vtkCamera * camera;
    Ui::MainWindow * ui;
    double defualtDistance;


    QCheckBox * rotation;
    QCheckBox * translation;
    QCheckBox * scaling;
    MainWindow * mainWindow;

//    char * rotation;
//    char * translation;
//    char * zooming;


};

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

vtkStandardNewMacro(KeyPressInteractorStyle);
vtkStandardNewMacro(MouseInteractorStyle);
vtkStandardNewMacro(MouseInteractorStyleShiftAndControlTrackBall);
vtkStandardNewMacro(MouseInteractorStyleShiftAndControlJoystick);
vtkStandardNewMacro(MouseInteractorStyleScalingAndControlTrackBall);
vtkStandardNewMacro(TouchInteractorStyleTrackBallCamera);
//vtkStandardNewMacro(vtkCameraCallback);

#endif // INTERACTIONCLASSES_H
