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

class TouchInteractorStyleTrackBallCamera : public vtkInteractorStyleTrackballCamera
{
    public:
    static TouchInteractorStyleTrackBallCamera* New();

    virtual void OnRightButtonDown()
    {
        vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();

        std::cout << "Translation triggered" << endl;
    }

    virtual void OnRightButtonUp()
    {
        vtkInteractorStyleTrackballCamera::OnMiddleButtonUp();

        std::cout << "Translation Finished" << endl;

    }


    virtual void OnLeftButtonDown()
    {
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }

};

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

vtkStandardNewMacro(TouchInteractorStyleTrackBallCamera);
vtkStandardNewMacro(InteractorStyleDefaultTrackBall);

QVTKTouchWidget::QVTKTouchWidget(QWidget *parent) :
    QVTKWidget(parent),
    horizonatlOffset(0),
    verticalOffset(0),
    rotationAngle(0),
    scaleFactor(1),
    currentStepScaleFactor(1)
{

}

void QVTKTouchWidget::enableGestures()
{
    std::cout << "gestures enabled" << endl;
    this->gesturesActive = true;
    this->setAttribute(Qt::WA_AcceptTouchEvents);

    this->grabGesture(Qt::TapGesture);
    this->grabGesture(Qt::TapAndHoldGesture);
    this->grabGesture(Qt::PinchGesture);


    TouchInteractorStyleTrackBallCamera * style =
         TouchInteractorStyleTrackBallCamera::New();

    this->GetInteractor()->SetInteractorStyle(style);

    }

    void QVTKTouchWidget::dissableGestures()
    {
        std::cout << "gestures dissabled" << endl;

        this->gesturesActive = false;
        this->setAttribute(Qt::WA_AcceptTouchEvents,false);

        this->ungrabGesture(Qt::SwipeGesture);
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
            if (event->type() == QEvent::TouchBegin ||
                    event->type() == QEvent::TouchEnd ||
                    event->type() == QEvent::TouchUpdate )
            {
                QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent*>(event)->touchPoints();
                //std::cout << "TouchPoints: " << touchPoints.count() << endl;

                std::cout << "TouchPoints: " << touchPoints.count() << '\t'
                          << "even Type: " << static_cast<QTouchEvent*>(event)->type() << endl;
                count = touchPoints.count();
            }
            if (event->type() == QEvent::Gesture)
                return gestureEvent(static_cast<QGestureEvent*> (event));
            else
                return QVTKWidget::event(event);
        }
        else
        {
            return QVTKWidget::event(event);
        }
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

        std::cout << "Position Reset" << endl;

    }

    void QVTKTouchWidget::setGesturesActive(bool status)
    {
        this->gesturesActive = status;

    }


    bool QVTKTouchWidget::gestureEvent(QGestureEvent *event)
    {
        if (gesturesActive)
        {
            if (QGesture *pinch = event->gesture(Qt::PinchGesture))
                pinchTriggered(static_cast<QPinchGesture *>(pinch));
            return true;
        }
    }    

    void QVTKTouchWidget::pinchTriggered(QPinchGesture *pinch)
    {
        QPinchGesture::ChangeFlags changeFlags = pinch->changeFlags();
        if (changeFlags & QPinchGesture::ScaleFactorChanged) {
            std::cout << "SCALING" << endl;

            vtkRenderWindowInteractor *iren = this->mRenWin->GetInteractor();

            vtkRenderer * renderer = this->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

            // Calculate the focal depth since we'll be using it a lot

            vtkCamera *camera = renderer->GetActiveCamera();

            QPinchGesture *ppinch = static_cast<QPinchGesture*> (pinch);

            camera->Dolly(ppinch->scaleFactor());
            renderer->ResetCameraClippingRange();
        }
        if (pinch->state() == Qt::GestureFinished) {
            scaleFactor *= currentStepScaleFactor;
            currentStepScaleFactor = 1;
        }

    }


    //void TouchWidget::printPanGestureInfo(QPanGesture *gesture)
    //{
    //    qDebug() << "Delta: " << gesture->delta().x() << "," <<  gesture->delta().y() << endl;
    ////    qDebug() << "Delta: " << gesture->delta().x() << "," <<  gesture->delta().y() << endl;
    //}

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
