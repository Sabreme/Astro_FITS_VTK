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
//#include "qevent.h"
//#include "qwidget.h"
//#include "qabstractscrollarea.h"
//#include <qgraphicssceneevent.h>
//#include "qdebug.h"

//class TouchInteractorStyleTrackBallCamera : public vtkInteractorStyleTrackballCamera
//{
//    public:
//    static TouchInteractorStyleTrackBallCamera* New();

//    virtual void OnRightButtonDown()
//    {
//        vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();

//        std::cout << "Translation triggered" << endl;
//    }

//    virtual void OnMouseMove()
//    {
//        vtkInteractorStyleTrackballCamera::OnMouseMove();

//        //this->GetInteractor()->GetRenderWindow()->Render();

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
//    }

//    virtual void OnRightButtonUp()
//    {
//        vtkInteractorStyleTrackballCamera::OnMiddleButtonUp();

//        std::cout << "Translation Finished" << endl;

//    }


//    virtual void OnLeftButtonDown()
//    {
//        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
//    }

//    virtual void Rotate()
//   {


//       vtkInteractorStyleTrackballCamera::Rotate();

//       double* orientation;

//       orientation  = camera->GetOrientation();

//       ui->line_OrientX->setText(QString::number(orientation[0], 'f', 0));
//       ui->line_OrientY->setText(QString::number(orientation[1], 'f', 0));
//       ui->line_OrientZ->setText(QString::number(orientation[2], 'f', 0));


//   }


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

//    Ui::MainWindow * ui;

//};

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
    currentStepScaleFactor(1)
{
    translateGesture = new TranslateRecognizer();
   panGesture5 = QGestureRecognizer::registerRecognizer(translateGesture);


}

void QVTKTouchWidget::enableGestures()
{
//    std::cout << "gestures enabled" << endl;
    this->gesturesActive = true;
    this->setAttribute(Qt::WA_AcceptTouchEvents);

    this->grabGesture(Qt::TapGesture);
    this->grabGesture(Qt::TapAndHoldGesture);
    this->grabGesture(Qt::PinchGesture);
    this->grabGesture(Qt::SwipeGesture);
//    this->grabGesture(Qt::PanGesture);
    this->grabGesture(panGesture5);
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

        this->ungrabGesture(Qt::TapGesture);
        this->ungrabGesture(Qt::TapAndHoldGesture);
        this->ungrabGesture(Qt::PinchGesture);



        InteractorStyleDefaultTrackBall * style =
                InteractorStyleDefaultTrackBall::New();

        this->GetInteractor()->SetInteractorStyle(style);       
    }


    bool QVTKTouchWidget::event(QEvent *event)
    {
        int count = 0;

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


                //touchPoints.
                 std::cout << "TouchPoints: " << touchPoints.count()<< "\t " ; // << endl;
                 count = touchPoints.count();

                 if (count  >= 2)
                 {
                    QTouchEvent::TouchPoint p1 = touchPoints.at(0);
                    QTouchEvent::TouchPoint p2 = touchPoints.at(1);

                    QPointF distance =  p1.pos() - p2.pos();
                    int dist = distance.manhattanLength();


                    std::cout << "p1 Start (x,y): " << p1.startPos().x() << "," << p1.startPos().y() << "\t"
                              << "p2 Start (x,y): " << p2.startPos().x() << "," << p2.startPos().y() << "\t"
                              << "p1 track (x,y): " << p1.pos().x() << "," << p1.pos().y() << "\t"
                              << "p2 track (x,y): " << p2.pos().x() << "," << p2.pos().y() << "\t"
                              << "distance:" << distance.x() << "," << distance.y() << "\t"
                              << "length: " << dist << "\t"
                                ;
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
        this->currentStepScaleFactor = 1;
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



            if (QGesture *pinch = event->gesture(Qt::PinchGesture))
            {
               pinchTriggered(static_cast<QPinchGesture *>(pinch));
               std::cout << "Pinch Gesture" << endl;
            return true;
            }

//            if (QGesture *translate = event->gesture(Qt::PanGesture))
//            {
//                QPanGesture *ppan = static_cast<QPanGesture*> (translate);

//               /// pinchTriggered(static_cast<QPinchGesture *>(pinch));
//               std::cout << "Pan Gesture" << endl;
//            return true;
//            }

            if (QGesture *rotate = event->gesture(Qt::TapGesture))
            {
              ///  pinchTriggered(static_cast<QPinchGesture *>(pinch));
              std::cout << "Rotate Gesture" << endl;
            return true;
            }

            if (QGesture * translate = event->gesture(panGesture5))
            {
                std::cout << "Translate Gesture" << endl;
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
