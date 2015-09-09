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

#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkInteractorObserver.h"
#include "vtkObjectFactory.h"
#include "qgesturerecognizer.h"
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
    {}

    virtual void  OnMiddleButtonDown()
    {}

    virtual void OnMouseWheelBackward()
    {}

    virtual void OnMouseWheelForward()
    {}

};





//vtkStandardNewMacro(TouchInteractorStyleTrackBallCamera);
vtkStandardNewMacro(InteractorStyleDefaultTrackBall);

QVTKTouchWidget::QVTKTouchWidget(QWidget *parent) :
    QVTKWidget(parent),
    horizonatlOffset(0),
    verticalOffset(0),
    rotationAngle(0),
    scaleFactor(1),   
    lastScaleFactor(1)
{
    translateGesture = new TranslateRecognizer();
   pan2Finger = QGestureRecognizer::registerRecognizer(translateGesture);
   scaleSequenceNew = true;


}

void QVTKTouchWidget::enableGestures()
{
//    std::cout << "gestures enabled" << endl;
    this->gesturesActive = true;
    this->setAttribute(Qt::WA_AcceptTouchEvents);

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

        if (gesturesActive)
        {
            if(
                    event->type() == QEvent::TouchBegin ||
                    event->type() == QEvent::TouchEnd ||
                    event->type() == QEvent::TouchUpdate
                    //event->type() == QEvent::Gesture
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

                 if (count == 1)
                 {
                     if (event->type() == QEvent::TouchEnd)
                            gestureDone = true;
                     else
                         gestureDone = false;

                     /////////////////////////////////////////////////////////
                     ///////////////////
                     /// 1 Finger Rotation
                     ///

                     if (event->type() == QEvent::TouchUpdate)
                     {
                         lastGesture = 1;

                         rotationPressed();

                         QTouchEvent::TouchPoint p1 = touchPoints.at(0);

                         QPointF lastPos = p1.lastPos();    /// get the average of the last positions
                         QPointF newPos = (p1.pos());               /// get the aversge of the current positions

                         vtkRenderer * renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

                         vtkRenderWindowInteractor *rwi = this->GetInteractor();

                         double MotionFactor = 10;

                         int dx = newPos.x() - lastPos.x();
                         int dy = lastPos.y() - newPos.y();     /// THIS IS REVERSED ON TOUCH FOR SOME REASON

                         int *size = renderer->GetRenderWindow()->GetSize();

                         double delta_elevation = -20.0 / size[1];
                         double delta_azimuth = -20.0 / size[0];

                         double rxf = dx * delta_azimuth * MotionFactor;
                         double ryf = dy * delta_elevation * MotionFactor;

                         vtkCamera *camera = renderer->GetActiveCamera();
                         camera->Azimuth(rxf);
                         camera->Elevation(ryf);
                         camera->OrthogonalizeViewUp();

                         renderer->ResetCameraClippingRange();

                         renderer->UpdateLightsGeometryToFollowCamera();

                     } /// if (event->type() == QEvent::TouchUpdate)

                 } ///  if (count == 1)

                 if (count  == 2 && manual)
                 {

                    QTouchEvent::TouchPoint p1 = touchPoints.at(0);
                    QTouchEvent::TouchPoint p2 = touchPoints.at(1);

                    QPointF lastPos = (p1.lastPos() + p2.lastPos()) / 2;    /// get the average of the last positions
                    QPointF newPos = (p1.pos() + p2.pos())/2;               /// get the aversge of the current positions

///                    QPointF lastPos = p1.lastPos();    /// get the average of the last positions
///                    QPointF newPos = (p1.pos());               /// get the aversge of the current positions


                    QPointF distance =  p1.pos() - p2.pos();
                    int distPos = distance.manhattanLength();

                    distance = p1.startPos() - p2.startPos();
                    int distStart = distance.manhattanLength();


//                    std::cout << "Start Difference: " << distStart << "\t"
//                              << "Pos Difference: " << distPos << "\t";

                    if((distStart < 100) && (distPos < 100))
                    {
                        lastGesture = 2;

                       std::cout << "Translation Active" << "\t";

                       if (event->type() == QEvent::TouchUpdate)
                       {
                           translationPressed();
                       }


//                    }

//                    << "distance:" << distance.x() << "," << distance.y() << "\t"
//                                                   << "length: " << dist << "\t"

//                                                      ;

                    /////////////////////////////////////////////////////////////////
                    /////////////////////////////////////////////////////////////////
                    vtkRenderWindowInteractor *iren = this->mRenWin->GetInteractor();

                    vtkRenderer * renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

                    // Calculate the focal depth since we'll be using it a lot

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
                                                  newPos.y(),
                                                  focalDepth,
                                                  newPickPoint);

                      // Has to recalc old mouse point since the viewport has moved,
                        // so can't move it outside the loop

                        irenObserver->ComputeDisplayToWorld(renderer,
                                                            lastPos.x(),
                                                    lastPos.y(),
                                                    focalDepth,
                                                    oldPickPoint);

                        // Camera motion is reversed

                        motionVector[0] = oldPickPoint[0] - newPickPoint[0];
                        motionVector[1] = newPickPoint[1] - oldPickPoint[1];
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

                        //static_cast<MainWindow*>(this->parent())->updateCameraPosition();

                        /// ADDED INTERACTION CAPTURE
                        ///
                        ///
                        ///
//                        double* position;

//                        position = camera->GetPosition();

//                        ui->line_PosX->setText(QString::number(position[0], 'f', 0));
//                        ui->line_PosY->setText(QString::number(position[1], 'f', 0));
//                        ui->line_PosZ->setText(QString::number(position[2], 'f', 0));



                    } ///  if((distStart < 100) && (distPos < 100))
                    else
                     ///   We implement the pinch to Zoom code here
                    {

                        lastGesture = 3;

                        std::cout << "Scaling Active" << "\t";

                        vtkRenderWindowInteractor *iren = this->mRenWin->GetInteractor();

                        vtkRenderer * renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

                        // Calculate the focal depth since we'll be using it a lot

                        vtkCamera *camera = renderer->GetActiveCamera();

                        ///QPinchGesture *ppinch = static_cast<QPinchGesture*> (pinch);
                        ///
                        /// INSERT PINCH CODE HERE FROM GestureRecognizer
                        ///
                        string name = QEvent::staticMetaObject.enumerator(eventNumIndex).valueToKey(event->type());
                        std::cout << "eventType(): " << name << "\t" ;



//                        QPointF centerPoint = (p1.screenPos() + p2.screenPos()) / 2.0;
//                        if (d->isNewSequence)
//                        {
//                            d->startPosition[0] = p1.screenPos();
//                            d->startPosition[1] = p2.screenPos();
//                            d->lastCenterPoint = centerPoint;
//                        } else {
//                            d->lastCenterPoint = d->centerPoint;
//                        }
//                        d->centerPoint = centerPoint;

//                        d->changeFlags |= QPinchGesture::CenterPointChanged;

//                        if (this->scaleSequenceNew )
//                        {
//                            this->scaleFactor = 1.0;
//                            this->lastScaleFactor = 1.0;
//                        }
//                        else
//                        {
                            this->lastScaleFactor = this->scaleFactor;
                            QLineF line(p1.screenPos(), p2.screenPos());
                            QLineF lastLine(p1.lastScreenPos(),  p2.lastScreenPos());
                            this->scaleFactor = line.length() / lastLine.length();

                            std::cout << "scaleFactor:" << scaleFactor << "\t"



                                      <<endl;
//                            scaleSequenceNew = false;
//                        }
                        //d->totalScaleFactor = d->totalScaleFactor * d->scaleFactor;
                        //d->changeFlags |= QPinchGesture::ScaleFactorChanged;



//                        qreal angle = QLineF(p1.screenPos(), p2.screenPos()).angle();
//                        if (angle > 180)
//                            angle -= 360;
//                        qreal startAngle = QLineF(p1.startScreenPos(), p2.startScreenPos()).angle();
//                        if (startAngle > 180)
//                            startAngle -= 360;
//                        const qreal rotationAngle = startAngle - angle;
//                        if (d->isNewSequence)
//                            d->lastRotationAngle = 0.0;
//                        else
//                            d->lastRotationAngle = d->rotationAngle;
//                        d->rotationAngle = rotationAngle;
//                        d->totalRotationAngle += d->rotationAngle - d->lastRotationAngle;
//                        d->changeFlags |= QPinchGesture::RotationAngleChanged;

//                        d->totalChangeFlags |= d->changeFlags;
//                        d->isNewSequence = false;



                        camera->Dolly(this->scaleFactor);
                        renderer->ResetCameraClippingRange();
                    }   /// End Pinch Zoon
                 }  /// if (count  == 2 && manual)

                 std::cout << "LastGesture: " << lastGesture <<  endl;

                 if (gestureDone)
                 {
                     std::cout << "Gesture Done" << "\t";
                     switch (lastGesture)
                     {
                         case 0: std::cout << "case 0" << endl; break;
                         case 1: rotationReleased()          ; break;
                         case 2: translationReleased()          ; break;
                         case 3: std::cout << "case 3" << endl; break;
                     default:
                         break;
                     }
                 }


               //  if (touchPoints.count() == 2)

                //                std::cout << "TouchPoints: " << touchPoints.count() << '\t'
                //                          << "even Type: " << static_cast<QTouchEvent*>(event)->type() << endl;
                //                count = touchPoints.count();
            }

                if (event->type() == QEvent::Gesture)
                {
                                //    std::cout << "gesture Event" << endl;
                                    return gestureEvent(static_cast<QGestureEvent*> (event));
                }
                    ///
///////////

//                    //std::cout << "Gesture Type:" << event->gestures()[0]->gestureType() << endl;
//                    //            QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent*>(event)->touchPoints();

//                    //            std::cout << touchPoints.count() << endl;

//                    if (count == 1)
//                    {

//                        std::cout << " Inside 1 finger touch:" << endl;
//                        if (QGesture *pinch = static_cast<QGestureEvent*>(event)->gesture(Qt::PinchGesture))
//                        {
//                            /// pinchTriggered(static_cast<QPinchGesture *>(pinch));
//                            std::cout << "Pinch Gesture" << endl;
//                            return true;
//                        }

//                    }

////                    if (touchPoints.count() == 2 && )
////                    {
////                        if (QGesture *translate = static_cast<QGestureEvent*>(event)->gesture(Qt::PanGesture))
////                    {
////                        QPanGesture *ppan = static_cast<QPanGesture*> (translate);

////                        /// pinchTriggered(static_cast<QPinchGesture *>(pinch));
////                        std::cout << "Pan Gesture" << endl;
////                        return true;
////                    }

////                    if (QGesture *rotate = event->gesture(Qt::TapGesture))
////                    {
////                        ///  pinchTriggered(static_cast<QPinchGesture *>(pinch));
////                        std::cout << "Rotate Gesture" << endl;
////                        return true;
////                    }
//                }
                else
                    return QVTKWidget::event(event);


//            else
//                return QVTKWidget::event(event);
        }   // IF (gesturesActive)
        else

            return QVTKWidget::event(event);

    }

    void QVTKTouchWidget::paintEvent(QPaintEvent *event)
    {

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

//        std::cout << "Position Reset" << endl;

    }

    void QVTKTouchWidget::setGesturesActive(bool status)
    {
        this->gesturesActive = status;
    }


    bool QVTKTouchWidget::gestureEvent(QGestureEvent *event)
    {
        if (gesturesActive)
        {
            //std::cout << "Gesture Type:" << event->gestures()[0]->gestureType() << endl;
//            QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent*>(event)->touchPoints();

//            std::cout << touchPoints.count() << endl;


//            if (QGesture * translate = event->gesture(panGesture5))
//            {
//                std::cout << "Translate Gesture" ;

//                QPanGesture * gesture = static_cast<QPanGesture*>(translate);
//                std::cout << "offset (x,y): " << gesture->offset().x() << "," << gesture->offset().y() << "\t"
//                          << "LastOf (x,y): " << gesture->lastOffset().x() << "," << gesture->lastOffset().y() << "\t"
//                          << endl;

//                panTriggered(static_cast<QPanGesture*> (translate));

//                return true;
//            }


            if (QGesture *pinch = event->gesture(Qt::PinchGesture))
            {
               pinchTriggered(static_cast<QPinchGesture *>(pinch));
               std::cout << "Pinch Gesture" << endl;
            return true;
            }

            if (QGesture *rotate = event->gesture(Qt::TapGesture))
            {
              ///  pinchTriggered(static_cast<QPinchGesture *>(pinch));
              std::cout << "Rotate Gesture" << endl;
            return true;
            }

            if (QGesture *translate = event->gesture(pan2Finger))
            {
                QPanGesture * gesture = static_cast<QPanGesture*>(translate);
                std::cout << "Offset (x,y): " << gesture->offset().x() << "," << gesture->offset().y() << "\t"
                          << "LastOf (x,y): " << gesture->lastOffset().x() << "," << gesture->lastOffset().y() << "\t"
                          << endl;

               panTriggered(static_cast<QPanGesture*> (translate));

              std::cout << "2 Finger Pan" << endl;
            return true;
            }




        }
    }    

    void QVTKTouchWidget::pinchTriggered(QPinchGesture *pinch)
    {
        //std::cout << "SCALING" << "\t" << "Pinch: " << pinch->state() << endl;

        QPinchGesture::ChangeFlags changeFlags = pinch->changeFlags();


        /// IF We are starting the Pinch Gesture we EMIT a MiddleMouseButton PRESS Signal
        if (pinch->state() == Qt::GestureStarted)
        {
            ///std::cout << "SCALING Started" << "\t" << endl;
          //  this->GetInteractor()->MiddleButtonPressEvent();
            scaleTriggered();
        }

        /// IF We are ending the Pinch Gesture we EMIT a MiddleMouseButton RELEASE Signal
        if (pinch->state() == Qt::GestureFinished)
        {
           // this->GetInteractor()->MiddleButtonReleaseEvent();
        }



   // std::cout << "center: (x,y) - ("  << pinch->centerPoint().x() << pinch->centerPoint().y() << endl;

        if (changeFlags & QPinchGesture::ScaleFactorChanged ) {

        //    std::cout << "SCALING" << endl;
//            changeFlags.

            vtkRenderWindowInteractor *iren = this->mRenWin->GetInteractor();

            vtkRenderer * renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

            // Calculate the focal depth since we'll be using it a lot

            vtkCamera *camera = renderer->GetActiveCamera();

            QPinchGesture *ppinch = static_cast<QPinchGesture*> (pinch);




            camera->Dolly(ppinch->scaleFactor());
            renderer->ResetCameraClippingRange();
//            TouchInteractorStyleTrackBallCamera * style = static_cast<TouchInteractorStyleTrackBallCamera*>
//                    (iren->GetInteractorStyle());
//            style->UpdateScale();





        }

        if (changeFlags & !QPinchGesture::CenterPointChanged ) {

            std::cout << "TRANSLATING" << endl;
//            changeFlags.

//            vtkRenderWindowInteractor *iren = this->mRenWin->GetInteractor();

//            vtkRenderer * renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

//            // Calculate the focal depth since we'll be using it a lot

//            vtkCamera *camera = renderer->GetActiveCamera();

//            QPinchGesture *ppinch = static_cast<QPinchGesture*> (pinch);


//            camera->Dolly(ppinch->scaleFactor());
//            renderer->ResetCameraClippingRange();
//            TouchInteractorStyleTrackBallCamera * style = static_cast<TouchInteractorStyleTrackBallCamera*>
//                    (iren->GetInteractorStyle());
//            style->UpdateScale();





        }

        ////
        //// OPTIONAL ROTATIONAL 2-Finger ROLL
        ///
        if (changeFlags & QPinchGesture::RotationAngleChanged) {

            //std::cout << "Z-Axis Rotation" << endl;

            vtkRenderWindowInteractor *iren = this->mRenWin->GetInteractor();

            vtkRenderer * renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

            // Calculate the focal depth since we'll be using it a lot

            vtkCamera *camera = renderer->GetActiveCamera();

            QPinchGesture *ppinch = static_cast<QPinchGesture*> (pinch);


            double rotationDelta = ppinch->rotationAngle() - ppinch->lastRotationAngle();
            camera->Roll(-rotationDelta);
            renderer->ResetCameraClippingRange();
            //            TouchInteractorStyleTrackBallCamera * style = static_cast<TouchInteractorStyleTrackBallCamera*>
            //                    (iren->GetInteractorStyle());
            //            style->UpdateScale();


        }



//        if (pinch->state() == Qt::GestureFinished) {
//            scaleFactor *= currentStepScaleFactor;
//            currentStepScaleFactor = 1;
//        }

    }

    void QVTKTouchWidget::panTriggered(QPanGesture *translate)
    {

       // std::cout <<" INSIDE PAN TRIGGERED CODE" << endl;


            vtkRenderWindowInteractor *iren = this->mRenWin->GetInteractor();

            vtkRenderer * renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

            // Calculate the focal depth since we'll be using it a lot

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
                                                 translate->offset().x(),
                                          translate->offset().y(),
                                          focalDepth,
                                          newPickPoint);

              // Has to recalc old mouse point since the viewport has moved,
                // so can't move it outside the loop

                irenObserver->ComputeDisplayToWorld(renderer,
                                                    translate->lastOffset().x(),
                                            translate->lastOffset().y(),
                                            focalDepth,
                                            oldPickPoint);

                // Camera motion is reversed

//                motionVector[0] = oldPickPoint[0] - newPickPoint[0];
//                motionVector[1] = oldPickPoint[1] - newPickPoint[1];
//                motionVector[2] = oldPickPoint[2] - newPickPoint[2];

                motionVector[0] = oldPickPoint[0] - newPickPoint[0];
                motionVector[1] = newPickPoint[1] - oldPickPoint[1];
                motionVector[2] = newPickPoint[2] - oldPickPoint[2];


                camera->GetFocalPoint(viewFocus);
                camera->GetPosition(viewPoint);
                camera->SetFocalPoint(motionVector[0] + viewFocus[0],
                                      motionVector[1] + viewFocus[1],
                                      motionVector[2] + viewFocus[2]);

                camera->SetPosition(motionVector[0] + viewPoint[0],
                                    motionVector[1] + viewPoint[1],
                                    motionVector[2] + viewPoint[2]);




    }




//    void QVTKTouchWidget::printGestureInfo(QPanGesture *gesture)
//    {
//        std::cout() << "Delta: " << gesture->delta().x() << "," <<  gesture->delta().y() << endl;
//    //    qDebug() << "Delta: " << gesture->delta().x() << "," <<  gesture->delta().y() << endl;
//    }

    //void TouchWidget::printGestureInfo(QGestureEvent *event)
    //{
    //    if(QGesture *swipe = event->gesture(Qt::SwipeGesture))
    //        swipeTriggered(static_cast<QSwipeGesture*> (swipe));
    //else
    //    if(QGesture *pan = event->gesture(Qt::PanGesture))
    //        panTriggered(static_cast<QPanGesture*> (pan));
    //else
    //   if (QGesture *pinch = event->gesture(Qt::PinchGesture))
    //        pinchTriggered(static_cast<QPinchGesture*> (pinch));
    //}
