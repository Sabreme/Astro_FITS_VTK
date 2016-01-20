#include "vtkTouchWidget.h"
#include "QVTKWidget.h"
#include "vtkCommand.h"
#include "QPoint"

#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRendererCollection.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkOrientationMarkerWidget.h"

#include "vtkInteractorStyleUser.h"

#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkInteractorObserver.h"
#include "vtkObjectFactory.h"
#include "qgesturerecognizer.h"

#include "vtkAbstractPicker.h"
#include "vtkPropCollection.h"
#include "vtkPropPicker.h"
#include "vtkAssemblyPath.h"


#include "vtkVertexGlyphFilter.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
//#include "private/qgesture_p.h"

#include "qgesture.h"
#include <QEvent>
#include <QMetaEnum>
//#include "qevent.h"
//#include "qwidget.h"
//#include "qabstractscrollarea.h"
//#include <qgraphicssceneevent.h>
//#include "qdebug.h"



class InteractorStyleDefaultTrackBall : public vtkInteractorStyleTrackballCamera
{
public:
    static InteractorStyleDefaultTrackBall* New();

    /// WE REMOVE UNWANTED FUNCTIONALITY SUCH AS SCALING, PANNING TO PREVENT BUGS
    virtual void OnRightButtonDown()
    {
        std::cout << "Right Mouse Button Down" << endl;
    }

    virtual void  OnMiddleButtonDown()
    {

    }

    virtual void OnMouseWheelBackward()
    {}

    virtual void OnMouseWheelForward()
    {}

};

class TouchSubVolInteractorStyle : public vtkInteractorStyleUser
{
    public:
    static TouchSubVolInteractorStyle* New();

    virtual void OnRightButtonDown()
    {
    }

    virtual void OnRightButtonUp()
    {

    }

    virtual void OnMiddleButtonDown()
    {

    }

    virtual void OnMiddleButtonUp()
    {
    }


    virtual void OnLeftButtonUp()
    {
    }

    virtual void OnLeftButtonDown()
    {
    }

    virtual void Rotate()
   {

   }

    virtual void OnMouseMove()
    {

    }

    virtual void Spin()
   {

   }


    virtual void Pan()
    {

    }

    virtual void Dolly()
    {

    }


    // vtkCamera * camera;
    // Ui::MainWindow * ui;
    // MainWindow * mainWindow;

    // double defualtDistance;

};





vtkStandardNewMacro(TouchSubVolInteractorStyle);
vtkStandardNewMacro(InteractorStyleDefaultTrackBall);

QVTKTouchWidget::QVTKTouchWidget(QWidget *parent) :
    QVTKWidget(parent),
    horizonatlOffset(0),
    verticalOffset(0),
    rotationAngle(0),
    scaleFactor(1),
    lastScaleFactor(1),
    lastGesture (None),        /// 1 = Rotation, 2 = Translation, 3 = Scaling, 4 = ZRotate
    touchPointBuffer (0),
    touchTransformBuffer (0),
    touchGestureCounter (0),
    rotationPointBuffer (0),
    touchEventDelay (8),         /// Used as a buffer to simulate delay


    gesturesActive (false),
    userTestRunning (false),
    rightMouseClick (false),
    transformsOn (true),
    subVolumeOn (false),
    arbSliceOn (false)




{
    double colorRotation_Value [] = {0.1, 0.4, 0.5};
    colorRotation = colorRotation_Value;

    double colorTranslate1_Value [] = {0.5, 1, 0.7};
     colorTranslate1 = colorTranslate1_Value;

    double colorTranslate2_Value [] = {0.5, 1, 0.7};
    colorTranslate2 = colorTranslate2_Value;

   double colorScale1_Value [] = {0.7, 0.6, 0.8};
   colorScale1 = colorScale1_Value;

   double colorScale2_Value [] = {0.7, 1, 0.8};
    colorScale2 = colorScale2_Value;

   double colorSpin1_Value [] = {0.8, 0.8, 0.8};
    colorSpin1 = colorSpin1_Value;

    double colorSpin2_Value [] = {0.8, 1, 0.8};
   colorSpin2 = colorSpin2_Value;

    double colorSpin3_Value [] = {0.1, 1, 0.8};
   colorSpin3 = colorSpin3_Value;
}

void QVTKTouchWidget::enableGestures()
{
    //    std::cout << "gestures enabled" << endl;
   /// this->gesturesActive = true;



    this->setAttribute(Qt::WA_AcceptTouchEvents);
    vtkSmartPointer<vtkPoints> points =
            vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(10, 10 ,0);
    //points->InsertNextPoint(100, 100, 0);

    vtkSmartPointer<vtkPolyData> polydata =
            vtkSmartPointer<vtkPolyData>::New();

    polydata->SetPoints(points);

    vtkSmartPointer<vtkVertexGlyphFilter> glyphfilter =
            vtkSmartPointer<vtkVertexGlyphFilter>::New();

    glyphfilter->SetInputConnection(polydata->GetProducerPort());
    glyphfilter->Update();

    vtkSmartPointer<vtkPolyDataMapper2D> mapper =
            vtkSmartPointer<vtkPolyDataMapper2D>::New();

    mapper->SetInputConnection(glyphfilter->GetOutputPort());

    mapper->Update();

    vtkRenderer * renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

    this->GetInteractor()->GetInteractorStyle()->SetCurrentRenderer(renderer);
    this->GetInteractor()->GetInteractorStyle()->SetDefaultRenderer(renderer);

    /// WE CONSTRUCT THE FINGER POINTS
    ///
    fingerActor1 =
            vtkActor2D::New();
    fingerActor1->SetMapper(mapper);
    renderer->AddActor(fingerActor1);

    fingerActor1->GetProperty()->SetColor(0.1,0.2,0.3);
    fingerActor1->GetProperty()->SetPointSize(40);

    fingerActor1->SetVisibility(false);


    /////////////////////////////////////
    fingerActor2 =
            vtkActor2D::New();
    fingerActor2->SetMapper(mapper);
    renderer->AddActor(fingerActor2);

    fingerActor2->GetProperty()->SetColor(0.5,1,0.4);
    fingerActor2->GetProperty()->SetPointSize(40);

    fingerActor2->SetVisibility(false);

    /////////////////////////////////////

    fingerActor3 =
            vtkActor2D::New();
    fingerActor3->SetMapper(mapper);
    renderer->AddActor(fingerActor3);

    fingerActor3->GetProperty()->SetColor(0.6,1,0.5);
    fingerActor3->GetProperty()->SetPointSize(40);

    fingerActor3->SetVisibility(false);

    ////////////////////////////////////

    fingerActor4 =
            vtkActor2D::New();
    fingerActor4->SetMapper(mapper);
    renderer->AddActor(fingerActor4);

    fingerActor4->GetProperty()->SetColor(0.6,1,0.5);
    fingerActor4->GetProperty()->SetPointSize(40);

    fingerActor4->SetVisibility(false);

    ////////////////////////////////////

    fingerActor5 =
            vtkActor2D::New();
    fingerActor5->SetMapper(mapper);
    renderer->AddActor(fingerActor5);

    fingerActor5->GetProperty()->SetColor(0.6,1,0.5);
    fingerActor5->GetProperty()->SetPointSize(40);

    fingerActor5->SetVisibility(false);

    ////////////////////////////////////

    //this->GetRenderWindow()->Print(std::cout);
    //std::cout << endl;

    //this->grabGesture(pan2Finger);
    //    this->grabGesture(Qt::TapGesture);
    //    this->grabGesture(Qt::TapAndHoldGesture);
    //    this->grabGesture(Qt::PinchGesture);
    //    this->grabGesture(Qt::SwipeGesture);
    //    this->grabGesture(Qt::PanGesture);

    //    this->grabGesture(Qt::tra);


    //    TouchInteractorStyleTrackBallCamera * style =
    //         TouchInteractorStyleTrackBallCamera::New();

    //    this->GetInteractor()->SetInteractorStyle(style);

}

void QVTKTouchWidget::dissableGestures()
{
    //        std::cout << "gestures dissabled" << endl;

    this->gesturesActive = false;
    this->setAttribute(Qt::WA_AcceptTouchEvents,false);

    //     this->ungrabGesture(Qt::TapGesture);
    //     this->ungrabGesture(Qt::TapAndHoldGesture);
    //    this->ungrabGesture(Qt::PinchGesture);



    InteractorStyleDefaultTrackBall * style =
            InteractorStyleDefaultTrackBall::New();

    this->GetInteractor()->SetInteractorStyle(style);
}


bool QVTKTouchWidget::event(QEvent *event)
{
    int count = 0;
    ///   lastGesture  ===== 1 = Rotate, 2 = translate, 3 = Scale
    bool gestureDone = false;

    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    /// 5 FINGER TRANSFORMATION TOGGLE OR SUBVOLUME
    if(
            event->type() == QEvent::TouchBegin ||
            event->type() == QEvent::TouchEnd ||
            event->type() == QEvent::TouchUpdate
            )
    {
        QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent*>(event)->touchPoints();

        int eventNumIndex = QEvent::staticMetaObject.indexOfEnumerator("Type");
        string name = QEvent::staticMetaObject.enumerator(eventNumIndex).valueToKey(event->type());

        //touchPoints.
///        std::cout << "TouchPoints: " << touchPoints.count()<< "\t " ; // << endl;
///        std::cout << "eventType(): " << name << "\t" ;
        count = touchPoints.count();

        if (count  == 5 )
        {

            touchTransformBuffer++;

            QTouchEvent::TouchPoint p1 = touchPoints.at(0);
            QTouchEvent::TouchPoint p2 = touchPoints.at(1);
            QTouchEvent::TouchPoint p3 = touchPoints.at(2);
            QTouchEvent::TouchPoint p4 = touchPoints.at(3);
            QTouchEvent::TouchPoint p5 = touchPoints.at(4);


            if (touchTransformBuffer ==(touchEventDelay - 1))
            {
                touchTransformBuffer = touchEventDelay;
                //std::cout << "Rotation Triggered" << endl;

            }

            ////////////////////////////////
            /////
            /// USER TESTING is RUNNING && If LastGesture was Not Translate,
            /// then New Gesture Count
            if ((touchTransformBuffer == touchEventDelay) )
            {
                transformationTriggered();
            ///    std::cout << "Transformation in Micro Event Triggered" << endl;
            }


            {
                ////////////////////////////////
                /////
                ///////////////////////////////////////////////
                /// 2D Finger Position Being Drawn
                /// It Seems Y Coordinate system of Touch Window is Reversed VTK WIndow
                ///  We need to Subtract the position from the Total Height of VTK Window
                ///
                ///
                vtkRenderWindow * renWindow = this->GetRenderWindow();

                int * sizeRW = renWindow->GetSize();
                //                int * screenSize = renWindow->GetScreenSize();

                int maxY = sizeRW[1];



                int x1Pos = p1.pos().toPoint().x();
                int y1Pos = p1.pos().toPoint().y();

                int x2Pos = p2.pos().toPoint().x();
                int y2Pos = p2.pos().toPoint().y();

                int x3Pos = p3.pos().toPoint().x();
                int y3Pos = p3.pos().toPoint().y();

                int x4Pos = p4.pos().toPoint().x();
                int y4Pos = p4.pos().toPoint().y();

                int x5Pos = p5.pos().toPoint().x();
                int y5Pos = p5.pos().toPoint().y();


                fingerActor1->SetPosition(x1Pos, maxY - y1Pos);

                fingerActor1->SetVisibility(true);


                fingerActor2->SetPosition(x2Pos, maxY - y2Pos);

                fingerActor2->SetVisibility(true);


                fingerActor3->SetPosition(x3Pos, maxY - y3Pos);

                fingerActor3->SetVisibility(true);


                fingerActor4->SetPosition(x4Pos, maxY - y4Pos);

                fingerActor4->SetVisibility(true);


                fingerActor5->SetPosition(x5Pos, maxY - y5Pos);

                fingerActor5->SetVisibility(true);

                ////////////////////////////////////////////////////

             ///   std::cout << "Transformation Switch" << "\t";

                fingerActor1->GetProperty()->SetColor(colorSpin1);
                fingerActor2->GetProperty()->SetColor(colorSpin2);
                fingerActor3->GetProperty()->SetColor(colorSpin3);
                fingerActor4->GetProperty()->SetColor(colorSpin3);
                fingerActor5->GetProperty()->SetColor(colorSpin3);


                string name = QEvent::staticMetaObject.enumerator(eventNumIndex).valueToKey(event->type());
///                std::cout << "eventType(): " << name << "\t" ;



            }   /// End Transformation Switch
        }  /// if (count  == 5 )

        if (event->type() == QEvent::TouchEnd)
        {
            touchTransformBuffer = 0;

            fingerActor1->SetVisibility(false);
            fingerActor2->SetVisibility(false);
            fingerActor3->SetVisibility(false);
            fingerActor4->SetVisibility(false);
            fingerActor5->SetVisibility(false);
        }
    }



    if ( transformsOn)
    {
        if(
                event->type() == QEvent::TouchBegin ||
                event->type() == QEvent::TouchEnd ||
                event->type() == QEvent::TouchUpdate
                )
        {
            QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent*>(event)->touchPoints();

///            int eventNumIndex = QEvent::staticMetaObject.indexOfEnumerator("Type");
///            string name = QEvent::staticMetaObject.enumerator(eventNumIndex).valueToKey(event->type());

///            std::cout << "TouchPoints: " << touchPoints.count()<< "\t " ; // << endl;
///            std::cout << "eventType(): " << name << "\t" ;
            count = touchPoints.count();            

            this->GetInteractor()->GetInteractorStyle()->SetEnabled(0);

            if (count == 1)
            {
                QTouchEvent::TouchPoint p1 = touchPoints.at(0);
                QPointF change = p1.pos() - p1.lastPos();

                if (!change.isNull() || lastGesture == Rotate)
                    gestureDone = transformEventRotate(event, touchPoints, gestureDone);
                else
                    gestureDone = true;

            } ///  if (count == 1)

            if (count  == 2)
            {
                QTouchEvent::TouchPoint p1 = touchPoints.at(0);
                QPointF change1 = p1.pos() - p1.lastPos();

                QTouchEvent::TouchPoint p2 = touchPoints.at(1);
                QPointF change2 = p2.pos() - p2.lastPos();

                if (!change1.isNull() || !change2.isNull() || lastGesture == Translate || lastGesture == Scale)
                {
                    transformEventTranslate(event, touchPoints);

                    transformEventScale(event, touchPoints);
                }
            }  /// if (count  == 2 && manual)

            if (count  == 3 )
            {
                transformEventZRotate(event, touchPoints);

            }  /// if (count  == 3 )


            //            if (!userTestRunning)
            //            {
            //                /// If  No Movement =  Decrease the Action Counter
            //                /// && Decrease the Gesture Counter
            //                /// If  Yes Movement = Increase the Gesture Counter
            //                if(!touch1movement)
            //                {
            //                    touchPointBuffer--;
            //                    touchGestureCounter = 0;c
            //                }
            //                else
            //                    touchGestureCounter++;

            //                /// If Gesture Buffer is > 5 = Set Gesture Buffer MAX to 5 (Buffer)
            //                /// && Gesture Counter Reset to ZERO
            //                if (touchPointBuffer > 5)
            //                {
            //                    touchPointBuffer = 5;
            //                    touchGestureCounter = 0;
            //                }

            //                ///If Gesture Counter is < 1  =  Set Gesture Counter MIN to 0 (Buffer)
            //                if(touchPointBuffer < 1)
            //                {
            //                    touchPointBuffer = 0;
            //                    touchGestureCounter = 0;
            //                }

            //                /// If Gesture Counter is 3 = WE have Detected a New Gesture

            //                if (touchPointBuffer > 0 || touch1movement)
            //                {
            //                    if (touchGestureCounter = 3)
            //                        std::cout << "Rotation Gesture Detected" << endl;
            //                }
            //            }


///            std::cout << "LastGesture: " << lastGesture <<  endl;

            if (gestureDone)
            {
              ///  std::cout << "Gesture Done" << endl;

                rotationReleased();
                translationReleased();


                fingerActor1->SetVisibility(false);
                fingerActor2->SetVisibility(false);
                fingerActor3->SetVisibility(false);
                //                switch (lastGesture)
                //                {
                //                case 0: std::cout << "case 0" << endl; break;
                //                case 1: rotationReleased()          ; break;
                //                case 2: translationReleased()          ; break;
                //                case 3: std::cout << "case 3" << endl; break;
                //                default:
                //                    break;
                //                }

                lastGesture = 0;
            }


        }
        return QVTKWidget::event(event);

    }   // IF (transformsOn)

    if ( subVolumeOn)
        ///if (gesturesActive && transformsOn)
    {
        if(
                event->type() == QEvent::TouchBegin ||
                event->type() == QEvent::TouchEnd ||
                event->type() == QEvent::TouchUpdate
                )
        {
            QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent*>(event)->touchPoints();

            int eventNumIndex = QEvent::staticMetaObject.indexOfEnumerator("Type");
            string name = QEvent::staticMetaObject.enumerator(eventNumIndex).valueToKey(event->type());

///            //touchPoints.
///            std::cout << "TouchPoints: " << touchPoints.count()<< "\t " ; // << endl;
///            std::cout << "eventType(): " << name << "\t" ;
            count = touchPoints.count();

            bool manual = true;

            TouchSubVolInteractorStyle * style = TouchSubVolInteractorStyle::New();
            this->GetInteractor()->SetInteractorStyle(style);

            style->SetEnabled(0);

            if (count == 1)
            {
                if (event->type() == QEvent::TouchEnd)
                {
                    gestureDone = true;
                    touchPointBuffer = 0;
                }

                else
                    gestureDone = false;

//                if(!userTestRunning)
//                {
//                    touch1movement = true;
//                    touchPointBuffer ++;
//                    rotateMovement = true;
//                }

                ///////////////////////////////////////////////////////
                /////////////////
                /// 1 Finger Positional Change
                ///

                if (event->type() == QEvent::TouchUpdate)
                {
                    touchPointBuffer++;

                    if (touchPointBuffer ==(touchEventDelay - 1))
                    {
                        touchPointBuffer = touchEventDelay;
                        //std::cout << "Rotation Triggered" << endl;

                    }

//                    ////////////////////////////////
//                    /////
//                    /// USER TESTING is RUNNING && If LastGesture was Not Translate,
//                    /// then New Gesture Count
//                    if ((touchPointBuffer == touchEventDelay) && (userTestRunning))
//                    {
//                        rotateTriggered();
//                        std::cout << "1 Point Tracking Triggered" << endl;
//                    }




                    lastGesture = 1;

                    ///rotationPressed();

                    QTouchEvent::TouchPoint p1 = touchPoints.at(0);


                    ///////////////////////////////////////////////
                    /// 2D Finger Position Being Drawn
                    /// It Seems Y Coordinate system of Touch Window is Reversed VTK WIndow
                    ///  We need to Subtract the position from the Total Height of VTK Window
                    ///
                    ///
                    vtkRenderWindow * renWindow = this->GetRenderWindow();

                    int * sizeRW = renWindow->GetSize();
                    int * screenSize = renWindow->GetScreenSize();

                    int maxY = sizeRW[1];



                    int xPos = p1.pos().toPoint().x();
                    int yPos = p1.pos().toPoint().y();

                    fingerActor1->SetPosition(xPos, maxY - yPos);
                    fingerActor1->GetProperty()->SetColor(colorRotation);

                    fingerActor1->SetVisibility(true);


                    ////////////////////////////////
                    /////
                    /// RIGHT MOUSE BUTTON TOGGLE
                    /// then New Gesture Count
                    if (rightMouseClick)

                    {
                        ///std::cout << "1 Point RIGHT Mouse Tracking Triggered" << endl;

                        //int xPos2 = p1.pos().toPoint().x();
                        int xPos2 = p1.lastPos().toPoint().x();
                        int yPos2 = p1.lastPos().toPoint().y();

                        fingerActor1->SetPosition2(xPos2, maxY - yPos2);

                        rightClkSubVol();
                    }
                    else
                    {
                        ///std::cout << "Single PointWidget Tracking" << endl;

                        ///////////////////////////////////////////////////////////////////////////////////
                        /// \brief finger1Pressed
                        ///
                        ///                        
                        finger1Pressed();


                    }

                } /// if (event->type() == QEvent::TouchUpdate)

            } ///  if (count == 1)

            if (count  == 2 && manual)
            {

                QTouchEvent::TouchPoint p1 = touchPoints.at(0);
                QTouchEvent::TouchPoint p2 = touchPoints.at(1);

                ///////////////////////////////////////////////
                /// 2D Finger Position Being Drawn
                /// It Seems Y Coordinate system of Touch Window is Reversed VTK WIndow
                ///  We need to Subtract the position from the Total Height of VTK Window
                ///
                ///
                vtkRenderWindow * renWindow = this->GetRenderWindow();

                int * sizeRW = renWindow->GetSize();
                //                int * screenSize = renWindow->GetScreenSize();

                int maxY = sizeRW[1];



                int x1Pos = p1.pos().toPoint().x();
                int y1Pos = p1.pos().toPoint().y();

                int x2Pos = p2.pos().toPoint().x();
                int y2Pos = p2.pos().toPoint().y();

                //                std::cout << "[x,y]: {" << x1Pos << ", " << y1Pos << "}" << "\t"
                //                          << "size [x,y]: {" << sizeRW[0] << ", " << sizeRW[1] <<"}" << "\t"
                //                          << "screen [x,y]: {" << screenSize[0] << ", " << screenSize[1] <<"}" << "\t"  <<
                //                             endl;

                fingerActor1->SetPosition(x1Pos, maxY - y1Pos);

                fingerActor1->SetVisibility(true);


                fingerActor2->SetPosition(x2Pos, maxY - y2Pos);

                fingerActor2->SetVisibility(true);

                ///if((distStart < 100) && (distPos < 100))
                //                    {
                ////////////////////////////////
                /////
                /// USER TESTING is RUNNING && If LastGesture was Not Translate,
                /// then New Gesture Count
                //                        if (lastGesture != 2 && userTestRunning)
                //                        {
                //                            translateTriggered();
                //                        }




                lastGesture = 2;

              ///  std::cout << "SubVolume - TRanslation Active" << "\t";

                ///                        if (event->type() == QEvent::TouchUpdate)
                ///                        {
                ///                            translationPressed();
                ///                        }
                ;

                fingerActor1->GetProperty()->SetColor(colorTranslate1);
                fingerActor2->GetProperty()->SetColor(colorTranslate2);

                finger2Pressed();


            }  /// if (count  == 2 && manual)



           /// std::cout << "LastGesture: " << lastGesture <<  endl;

            if (gestureDone)
            {
             ///   std::cout << "Gesture Done" << endl;

                finger1Released();
                /// translationReleased();
                ///



                fingerActor1->SetVisibility(false);
                fingerActor2->SetVisibility(false);
                fingerActor3->SetVisibility(false);

                lastGesture = 0;
            }
        }

        return QVTKWidget::event(event);
    }   /// If(SubVolumeOn)

    if ( arbSliceOn)
        ///if (gesturesActive && transformsOn)
    {
        if(
                event->type() == QEvent::TouchBegin ||
                event->type() == QEvent::TouchEnd ||
                event->type() == QEvent::TouchUpdate
                )
        {
            QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent*>(event)->touchPoints();

            int eventNumIndex = QEvent::staticMetaObject.indexOfEnumerator("Type");
            string name = QEvent::staticMetaObject.enumerator(eventNumIndex).valueToKey(event->type());

            //touchPoints.
            std::cout << "TouchPoints: " << touchPoints.count()<< "\t " ; // << endl;
            std::cout << "eventType(): " << name << "\t" ;
            count = touchPoints.count();

            bool manual = true;

            TouchSubVolInteractorStyle * style = TouchSubVolInteractorStyle::New();
            this->GetInteractor()->SetInteractorStyle(style);

            style->SetEnabled(0);

            if (count == 1)
            {
                if (event->type() == QEvent::TouchBegin)
                {
                    touchPointBuffer = 0;
                    finger1Pressed();
                }
                if (event->type() == QEvent::TouchEnd)
                {
                    gestureDone = true;
                    touchPointBuffer = 0;
                    finger1Released();
                }

                else
                    gestureDone = false;

//                if(!userTestRunning)
//                {
//                    touch1movement = true;
//                    touchPointBuffer ++;
//                    rotateMovement = true;
//                }

                ///////////////////////////////////////////////////////
                /////////////////
                /// 1 Finger Positional Change
                ///

                if (event->type() == QEvent::TouchUpdate)
                {
                    touchPointBuffer++;
                    std::cout << "Buffer " << touchPointBuffer << '\t' ;

                    if (touchPointBuffer ==(touchEventDelay - 1))
                    {
                        touchPointBuffer = touchEventDelay;
                        std::cout << "Rotation Triggered" << endl;

                    }

                    ////////////////////////////////
                    /////
                    /// USER TESTING is RUNNING && If LastGesture was Not Translate,
                    /// then New Gesture Count
//                    if ((touchPointBuffer == touchEventDelay) && (userTestRunning))
//                    {
//                        rotateTriggered();
//                        std::cout << "1 Point Arb Slice - Tracking Triggered" << endl;
//                    }

                    if ((touchPointBuffer == touchEventDelay) )
                    {
                        rotateTriggered();
                        std::cout << "1 Point Arb Slice - Tracking Triggered" << endl;

                    }

                    lastGesture = 1;

                    ///rotationPressed();

                    QTouchEvent::TouchPoint p1 = touchPoints.at(0);


                    ///////////////////////////////////////////////
                    /// 2D Finger Position Being Drawn
                    /// It Seems Y Coordinate system of Touch Window is Reversed VTK WIndow
                    ///  We need to Subtract the position from the Total Height of VTK Window
                    ///
                    ///
                    vtkRenderWindow * renWindow = this->GetRenderWindow();

                    int * sizeRW = renWindow->GetSize();
                    int * screenSize = renWindow->GetScreenSize();

                    int maxY = sizeRW[1];



                    int xPos = p1.pos().toPoint().x();
                    int yPos = p1.pos().toPoint().y();

                    fingerActor1->SetPosition(xPos, maxY - yPos);

                    /// FingerActor1 - SetPosition2 = LastPos of Finger Gestures used in SubVolume

                    int xPos2 = p1.lastPos().toPoint().x();
                    int yPos2 = p1.lastPos().toPoint().y();

                    fingerActor1->SetPosition2(xPos2, maxY - yPos2);


                    fingerActor1->GetProperty()->SetColor(colorRotation);

                    fingerActor1->SetVisibility(true);


                    ////////////////////////////////
                    /////
                    /// RIGHT MOUSE BUTTON TOGGLE
                    /// then New Gesture Count
                    if (rightMouseClick)

                    {
                        std::cout << "1 Point RIGHT Arb Slice Tracking Triggered" << endl;

                        int xPos2 = p1.lastPos().toPoint().x();
                        int yPos2 = p1.lastPos().toPoint().y();

                        fingerActor1->SetPosition2(xPos2, maxY - yPos2);

                       /// rightClkSubVol();
                    }
                    else
                    {
                        std::cout << "Single Arb Slice  Tracking" << endl;

                        finger1Moving();
                    }

                } /// if (event->type() == QEvent::TouchUpdate)

            } ///  if (count == 1)

            if (count  == 2 && manual)
            {

                QTouchEvent::TouchPoint p1 = touchPoints.at(0);
                QTouchEvent::TouchPoint p2 = touchPoints.at(1);

                ///////////////////////////////////////////////
                /// 2D Finger Position Being Drawn
                /// It Seems Y Coordinate system of Touch Window is Reversed VTK WIndow
                ///  We need to Subtract the position from the Total Height of VTK Window
                ///
                ///
                vtkRenderWindow * renWindow = this->GetRenderWindow();

                int * sizeRW = renWindow->GetSize();
                //                int * screenSize = renWindow->GetScreenSize();

                int maxY = sizeRW[1];



                int x1Pos = p1.pos().toPoint().x();
                int y1Pos = p1.pos().toPoint().y();

                int x2Pos = p2.pos().toPoint().x();
                int y2Pos = p2.pos().toPoint().y();

                fingerActor1->SetPosition(x1Pos, maxY - y1Pos);

                //////////////////////////////////////////
                /// \brief xPos2
                ///
                int xPos2 = p1.lastPos().toPoint().x();
                int yPos2 = p1.lastPos().toPoint().y();

                fingerActor1->SetPosition2(xPos2, maxY - yPos2);

                fingerActor1->SetVisibility(true);

                fingerActor2->SetPosition(x2Pos, maxY - y2Pos);

                xPos2 = p2.lastPos().toPoint().x();
                yPos2 = p2.lastPos().toPoint().y();

                fingerActor2->SetPosition2(xPos2, maxY - yPos2);

                fingerActor2->SetVisibility(true);



                ///////////////////////////////////////////////////////////

                QPointF lastPos = (p1.lastPos() + p2.lastPos()) / 2;    /// get the average of the last positions
                QPointF newPos = (p1.pos() + p2.pos())/2;               /// get the aversge of the current positions

                ///                    QPointF lastPos = p1.lastPos();    /// get the average of the last positions
                ///                    QPointF newPos = (p1.pos());               /// get the aversge of the current positions


                QPointF distance =  p1.pos() - p2.pos();
                int distPos = distance.manhattanLength();

                distance = p1.startPos() - p2.startPos();
                int distStart = distance.manhattanLength();

                if((distStart < 100) && (distPos < 100))
                {
                    ////////////////////////////////
                    /////
                    /// USER TESTING is RUNNING && If LastGesture was Not Translate,
                    /// then New Gesture Count
                    //                        if (lastGesture != 2 && userTestRunning)
                    //                        {
                    //                            translateTriggered();
                    //                        }

                    if (lastGesture != 2)
                    {

                        lastGesture = 2;
                        finger2Pressed();
                    }
                    else
                    {

                        std::cout << "Arb Slice - TRanslation Active" << "\t";

                        ///                        if (event->type() == QEvent::TouchUpdate)
                        ///                        {
                        ///                            translationPressed();
                        ///                        }
                        ;

                        fingerActor1->GetProperty()->SetColor(colorTranslate1);
                        fingerActor2->GetProperty()->SetColor(colorTranslate2);

                        finger2Moving();
                    }
                }


            }  /// if (count  == 2 && manual)



            std::cout << "LastGesture: " << lastGesture <<  endl;

            if (gestureDone)
            {
                ///std::cout << "Gesture Done" << endl;

                /// rotationReleased();
                /// translationReleased();


                fingerActor1->SetVisibility(false);
                fingerActor2->SetVisibility(false);               

                lastGesture = 0;
            }
        }

        return QVTKWidget::event(event);
    }   /// If(ArbSliceOn)
    else
        return QVTKWidget::event(event);

}


void QVTKTouchWidget::mouseDoubleClickEvent(QMouseEvent *event)
{

    this->mRenWin->GetRenderers()->GetFirstRenderer()->ResetCamera();
    this->mRenWin->GetRenderers()->GetFirstRenderer()->Render();

    this->rotationAngle = 0;
    this->scaleFactor = 1;
    this->lastScaleFactor = 1;
    this->verticalOffset = 0;
    this->horizonatlOffset = 0;
    this->update();    

}

///
/// We keep this flag for captring Right Mouse Button Actions
///
void QVTKTouchWidget::mousePressEvent(QMouseEvent *event)
{
    if (subVolumeOn && event->button()==Qt::RightButton)
    {
        rightMouseClick = true;
    }
    else
        QVTKWidget::mousePressEvent(event);
}

void QVTKTouchWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (subVolumeOn && event->button()== Qt::RightButton)
    {
        rightMouseClick = false;
    }
    else
        QVTKWidget::mouseReleaseEvent(event);

}

bool QVTKTouchWidget::transformEventRotate(QEvent *event, QList<QTouchEvent::TouchPoint> touchPoints, bool gestureDone )
{
    if (event->type() == QEvent::TouchEnd)
    {
        gestureDone = true;
        touchPointBuffer = 0;
        touchTransformBuffer = 0;
    }

    else
        gestureDone = false;

    //                if(!userTestRunning)
    //                {
    //                    touch1movement = true;
    //                    touchPointBuffer ++;
    //                    rotateMovement = true;
    //                }

    /////////////////////////////////////////////////////////
    ///////////////////
    /// 1 Finger Rotation
    ///

    if (event->type() == QEvent::TouchUpdate)
    {
///        std::cout << "Buffer " << touchPointBuffer << '\t' ;
        touchPointBuffer++;

        QTouchEvent::TouchPoint p1 = touchPoints.at(0);


        if (touchPointBuffer ==(touchEventDelay - 1))
        {
            touchPointBuffer = touchEventDelay;
            //std::cout << "Rotation Triggered" << endl;

        }

        ////////////////////////////////
        /////
        /// USER TESTING is RUNNING && If LastGesture was Not Translate,
        /// then New Gesture Count
//        if ((touchPointBuffer == touchEventDelay) && (userTestRunning))
//        {
//            rotateTriggered();
//            std::cout << "Rotation Triggered" << endl;
//        }

        if (lastGesture != Rotate && userTestRunning)
        {
            rotateTriggered();
        }

        if (lastGesture != Rotate )
        {
            //rotateTriggered();
            std::cout << "Rotation Triggered" << endl;
            lastGesture = Rotate;
        }

        //lastGesture = 1;

        rotationPressed();




        ///////////////////////////////////////////////
        /// 2D Finger Position Being Drawn
        /// It Seems Y Coordinate system of Touch Window is Reversed VTK WIndow
        ///  We need to Subtract the position from the Total Height of VTK Window
        ///
        ///
        vtkRenderWindow * renWindow = this->GetRenderWindow();

        int * sizeRW = renWindow->GetSize();
        int * screenSize = renWindow->GetScreenSize();

        int maxY = sizeRW[1];



        int xPos = p1.pos().toPoint().x();
        int yPos = p1.pos().toPoint().y();

        fingerActor1->SetPosition(xPos, maxY - yPos);
        fingerActor1->GetProperty()->SetColor(colorRotation);

        fingerActor1->SetVisibility(true);

        ///////////////////////////////////////////////////////////

        QPointF lastPos = p1.lastPos();    /// get the average of the last positions
        QPointF newPos = (p1.pos());               /// get the aversge of the current positions

        vtkRenderer * renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

        vtkRenderWindowInteractor *rwi = this->GetInteractor();

        vtkCamera *camera = renderer->GetActiveCamera();

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Camera Movement
        ///

        double MotionFactor = 10;

        int dx = newPos.x() - lastPos.x();
        int dy = lastPos.y() - newPos.y();     /// THIS IS REVERSED ON TOUCH FOR SOME REASON

        int *size = renderer->GetRenderWindow()->GetSize();

        double delta_elevation = -20.0 / size[1];
        double delta_azimuth = -20.0 / size[0];

        double rxf = dx * delta_azimuth * MotionFactor;
        double ryf = dy * delta_elevation * MotionFactor;

        camera->Azimuth(rxf);
        camera->Elevation(ryf);
        camera->OrthogonalizeViewUp();

        renderer->ResetCameraClippingRange();

        renderer->UpdateLightsGeometryToFollowCamera();

        ///////////////////////////////////////////////////////////////////////////////////

    } /// if (event->type() == QEvent::TouchUpdate)

    return gestureDone;
}

void QVTKTouchWidget::transformEventZRotate(QEvent *event, QList<QTouchEvent::TouchPoint> touchPoints)
{
    QTouchEvent::TouchPoint p1 = touchPoints.at(0);
    QTouchEvent::TouchPoint p2 = touchPoints.at(1);
    QTouchEvent::TouchPoint p3 = touchPoints.at(2);


    ///////////////////////////////////////////////
    /// 2D Finger Position Being Drawn
    /// It Seems Y Coordinate system of Touch Window is Reversed VTK WIndow
    ///  We need to Subtract the position from the Total Height of VTK Window
    ///
    ///
    vtkRenderWindow * renWindow = this->GetRenderWindow();

    int * sizeRW = renWindow->GetSize();
    //                int * screenSize = renWindow->GetScreenSize();

    int maxY = sizeRW[1];



    int x1Pos = p1.pos().toPoint().x();
    int y1Pos = p1.pos().toPoint().y();

    int x2Pos = p2.pos().toPoint().x();
    int y2Pos = p2.pos().toPoint().y();

    int x3Pos = p3.pos().toPoint().x();
    int y3Pos = p3.pos().toPoint().y();

    fingerActor1->SetPosition(x1Pos, maxY - y1Pos);

    fingerActor1->SetVisibility(true);


    fingerActor2->SetPosition(x2Pos, maxY - y2Pos);

    fingerActor2->SetVisibility(true);


    fingerActor3->SetPosition(x3Pos, maxY - y3Pos);

    fingerActor3->SetVisibility(true);

    ///////////////////////////////////////////////////////////


    QPointF lastPos = (p2.lastPos() + p3.lastPos()) / 2;    /// get the average of the last positions
    QPointF newPos = (p2.pos() + p3.pos())/2;               /// get the aversge of the current positions

    ///                    QPointF lastPos = p1.lastPos();    /// get the average of the last positions
    ///                    QPointF newPos = (p1.pos());               /// get the aversge of the current positions


    QPointF distance =  p2.pos() - p3.pos();
    int distPos = distance.manhattanLength();

    distance = p2.startPos() - p3.startPos();
    int distStart = distance.manhattanLength();


    //                    std::cout << "Start Difference: " << distStart << "\t"
    //                              << "Pos Difference: " << distPos << "\t";
    ///   We implement the Z Rotation Code Here to Zoom code here SCALING
    if((distStart < 150) && (distPos < 150))
    {
        ////////////////////////////////
        /////
        /// USER TESTING is RUNNING && If LastGesture was Not SCALING,
        /// then New Gesture Count
        if (lastGesture != ZRotate && userTestRunning)
        {
            zRotateTriggered();
        }

        rotationPressed();

        if (lastGesture != ZRotate)
        {
            std::cout << "ZRotation Triggered" << endl;
            lastGesture = ZRotate;
        }

///        std::cout << "Z Axis Rotation Active" << "\t";

        fingerActor1->GetProperty()->SetColor(colorSpin1);
        fingerActor2->GetProperty()->SetColor(colorSpin2);
        fingerActor3->GetProperty()->SetColor(colorSpin3);

        vtkRenderWindowInteractor *iren = this->mRenWin->GetInteractor();

        vtkRenderer * renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

        // Calculate the focal depth since we'll be using it a lot

        vtkCamera *camera = renderer->GetActiveCamera();


        QPointF fingersPos = ((p2.screenPos() + p3.screenPos()) / 2);
        QPointF fingersLastPos = ((p2.lastScreenPos() + p3.lastScreenPos()) / 2);

        qreal angle = QLineF(p1.screenPos(), fingersPos).angle();

        qreal lastAngle = QLineF(p1.lastScreenPos(), fingersLastPos).angle();


        const qreal rotationAngle = angle - lastAngle;


        camera->Roll( rotationAngle);
        camera->OrthogonalizeViewUp();

///        std::cout << "Rotation Angle:" << rotationAngle << "\t"
///                  <<endl;

    }   /// End Pinch Zoon
}

void QVTKTouchWidget::transformEventScale(QEvent *event, QList<QTouchEvent::TouchPoint> touchPoints)
{
    QTouchEvent::TouchPoint p1 = touchPoints.at(0);
    QTouchEvent::TouchPoint p2 = touchPoints.at(1);

    ///////////////////////////////////////////////
    /// 2D Finger Position Being Drawn
    /// It Seems Y Coordinate system of Touch Window is Reversed VTK WIndow
    ///  We need to Subtract the position from the Total Height of VTK Window
    ///
    ///
    vtkRenderWindow * renWindow = this->GetRenderWindow();

    int * sizeRW = renWindow->GetSize();

    int maxY = sizeRW[1];



    int x1Pos = p1.pos().toPoint().x();
    int y1Pos = p1.pos().toPoint().y();

    int x2Pos = p2.pos().toPoint().x();
    int y2Pos = p2.pos().toPoint().y();

    fingerActor1->SetPosition(x1Pos, maxY - y1Pos);

    fingerActor1->SetVisibility(true);


    fingerActor2->SetPosition(x2Pos, maxY - y2Pos);

    fingerActor2->SetVisibility(true);

    ///////////////////////////////////////////////////////////

    QPointF lastPos = (p1.lastPos() + p2.lastPos()) / 2;    /// get the average of the last positions
    QPointF newPos = (p1.pos() + p2.pos())/2;               /// get the aversge of the current positions

    ///                    QPointF lastPos = p1.lastPos();    /// get the average of the last positions
    ///                    QPointF newPos = (p1.pos());               /// get the aversge of the current positions


    QPointF distance =  p1.pos() - p2.pos();
    int distPos = distance.manhattanLength();

    distance = p1.startPos() - p2.startPos();
    int distStart = distance.manhattanLength();

    if((distStart > 150) || (distPos > 150))
    {
        ////////////////////////////////
        /////
        /// USER TESTING is RUNNING && If LastGesture was Not SCALING,
        /// then New Gesture Count
        if (lastGesture != Scale && userTestRunning)
        {
            scaleTriggered();
        }

        if (lastGesture != Scale )
        {
            lastGesture = Scale;
            std::cout << "Scaling TRIGGERED" << endl;
        }

///        std::cout << "Scaling Active" << "\t";

        rotationReleased();



        fingerActor1->GetProperty()->SetColor(colorScale1);
        fingerActor2->GetProperty()->SetColor(colorScale2);

        vtkRenderWindowInteractor *iren = this->mRenWin->GetInteractor();

        vtkRenderer * renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

        /// Calculate the focal depth since we'll be using it a lot

        vtkCamera *camera = renderer->GetActiveCamera();


//        string name = QEvent::staticMetaObject.enumerator(eventNumIndex).valueToKey(event->type());
//        std::cout << "eventType(): " << name << "\t" ;

        this->lastScaleFactor = this->scaleFactor;
        QLineF line(p1.screenPos(), p2.screenPos());
        QLineF lastLine(p1.lastScreenPos(),  p2.lastScreenPos());
        this->scaleFactor = line.length() / lastLine.length();

///        std::cout << "scaleFactor:" << scaleFactor << "\t"
///                  <<endl;

        camera->Dolly(this->scaleFactor);
        renderer->ResetCameraClippingRange();
    }
}

void QVTKTouchWidget::transformEventTranslate(QEvent * event, QList<QTouchEvent::TouchPoint> touchPoints)
{
    QTouchEvent::TouchPoint p1 = touchPoints.at(0);
    QTouchEvent::TouchPoint p2 = touchPoints.at(1);

    ///////////////////////////////////////////////
    /// 2D Finger Position Being Drawn
    /// It Seems Y Coordinate system of Touch Window is Reversed VTK WIndow
    ///  We need to Subtract the position from the Total Height of VTK Window
    ///
    ///
    vtkRenderWindow * renWindow = this->GetRenderWindow();

    int * sizeRW = renWindow->GetSize();

    int maxY = sizeRW[1];



    int x1Pos = p1.pos().toPoint().x();
    int y1Pos = p1.pos().toPoint().y();

    int x2Pos = p2.pos().toPoint().x();
    int y2Pos = p2.pos().toPoint().y();

    fingerActor1->SetPosition(x1Pos, maxY - y1Pos);

    fingerActor1->SetVisibility(true);


    fingerActor2->SetPosition(x2Pos, maxY - y2Pos);

    fingerActor2->SetVisibility(true);

    ///////////////////////////////////////////////////////////

    QPointF lastPos = (p1.lastPos() + p2.lastPos()) / 2;    /// get the average of the last positions
    QPointF newPos = (p1.pos() + p2.pos())/2;               /// get the aversge of the current positions

    ///                    QPointF lastPos = p1.lastPos();    /// get the average of the last positions
    ///                    QPointF newPos = (p1.pos());               /// get the aversge of the current positions


    QPointF distance =  p1.pos() - p2.pos();
    int distPos = distance.manhattanLength();

    distance = p1.startPos() - p2.startPos();
    int distStart = distance.manhattanLength();

    if((distStart < 150) && (distPos < 150))
    {
        ////////////////////////////////
        /////
        /// USER TESTING is RUNNING && If LastGesture was Not Translate,
        /// then New Gesture Count
        if (lastGesture != Translate && userTestRunning)
        {
            translateTriggered();
        }

        if (lastGesture != Translate)
        {
            std::cout << "Translate TRIGGERED" << endl;
            lastGesture = Translate;
        }

///     std::cout << "Translation Active" << "\t";

        if (event->type() == QEvent::TouchUpdate)
        {
            translationPressed();
        }

        fingerActor1->GetProperty()->SetColor(colorTranslate1);
        fingerActor2->GetProperty()->SetColor(colorTranslate2);

        /////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////

        ///
        vtkRenderWindow * renWindow = this->GetRenderWindow();

        int * sizeRW = renWindow->GetSize();

        int maxY = sizeRW[1];


        vtkRenderWindowInteractor *iren = this->mRenWin->GetInteractor();

        vtkRenderer * renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

        /// Calculate the focal depth since we'll be using it a lot

        vtkCamera *camera = renderer->GetActiveCamera();


        double viewFocus[4], focalDepth, viewPoint[3];
        double newPickPoint[4], oldPickPoint[4], motionVector[3];

        vtkInteractorObserver * irenObserver =
                static_cast<vtkInteractorObserver*> (this->GetInteractor()->GetInteractorStyle());


        camera->GetFocalPoint(viewFocus);
        irenObserver->ComputeWorldToDisplay(renderer, viewFocus[0], viewFocus[1], viewFocus[2],
                viewFocus);
        focalDepth = viewFocus[2];

        irenObserver->ComputeDisplayToWorld(renderer,
                                            newPos.x(),
                                            maxY - newPos.y(),
                                            focalDepth,
                                            newPickPoint);

        irenObserver->ComputeDisplayToWorld(renderer,
                                            lastPos.x(),
                                            maxY - lastPos.y(),
                                            focalDepth,
                                            oldPickPoint);

        //////  NOTE
        //////
        /// //motionVector[1] = newPickPoint[1] - oldPickPoint[1];        /// REVERSED FOR SOME reason
        /// For a long time, it was reversed until we realise the Screen Coordinate System Y is the Reverse
        /// of the VTK Rendering Window Coordinate System
        /// So we Compute DisplayToWorld of maxY - lastPos.y && maxY - newPos.y

        motionVector[0] = oldPickPoint[0] - newPickPoint[0];
        motionVector[1] = oldPickPoint[1] - newPickPoint[1];        /// REVERSED FOR SOME reason
        motionVector[2] = oldPickPoint[2] - newPickPoint[2];

        camera->GetFocalPoint(viewFocus);
        camera->GetPosition(viewPoint);
        camera->SetFocalPoint(motionVector[0] + viewFocus[0],
                motionVector[1] + viewFocus[1],
                motionVector[2] + viewFocus[2]);

        camera->SetPosition(motionVector[0] + viewPoint[0],
                motionVector[1] + viewPoint[1],
                motionVector[2] + viewPoint[2]);

        translationAction();
    }
}

void QVTKTouchWidget::setGesturesActive(bool status)
{
    this->gesturesActive = status;
}

void QVTKTouchWidget::setUserTestMode(bool status)
{
    this->userTestRunning = status;
}

void QVTKTouchWidget::setTransformsOn(bool status)
{
    this->transformsOn = status;        
}

bool QVTKTouchWidget::GetTransformsOn()
{
    return this->transformsOn;
}

void QVTKTouchWidget::SetSubVolumeOn(bool status)
{
    this->subVolumeOn = status;
}

bool QVTKTouchWidget::GetSubVolumeOn()
{
    return this->subVolumeOn;
}

void QVTKTouchWidget::SetArbSliceOn(bool status)
{
    this->arbSliceOn = status;
}

bool QVTKTouchWidget::GetArbSliceOn()
{
    return this->arbSliceOn;
}
