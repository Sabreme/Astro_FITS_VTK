#ifndef TOUCHWIDGET_H
#define TOUCHWIDGET_H

#include "QVTKWidget.h"
#include <QtGui>
#include <iostream>

#include "vtkCamera.h"
#include "vtkSystemIncludes.h"
#include <src/mainwindow.h>



using namespace std;

class TranslateRecognizer : public QGestureRecognizer
{
    QGesture * create(QObject *target)
    {
        if (target && target->isWidgetType()) {
#if defined(Q_OS_WIN) && !defined(QT_NO_NATIVE_GESTURES)
            // for scroll areas on Windows we want to use native gestures instead
            if (!qobject_cast<QAbstractScrollArea *>(target->parent()))
                static_cast<QWidget *>(target)->setAttribute(Qt::WA_AcceptTouchEvents);
#else
            static_cast<QWidget *>(target)->setAttribute(Qt::WA_AcceptTouchEvents);
#endif
        }
        return new QPanGesture;
    }

    Result recognize(QGesture *state,QObject *,QEvent *event)
    {
        QPanGesture *q = static_cast<QPanGesture *>(state);

        bool closeTouch = false;

        const QTouchEvent *ev = static_cast<const QTouchEvent *>(event);

        QGestureRecognizer::Result result;
        switch (event->type())
        {
        case QEvent::TouchBegin: {
            result = QGestureRecognizer::MayBeGesture;
            QTouchEvent::TouchPoint p = ev->touchPoints().at(0);

            q->setOffset(p.pos());
            q->setLastOffset(q->offset());

            //d->lastOffset = d->offset = QPointF();
            break;
        }
        case QEvent::TouchEnd:
        {
            if (q->state() != Qt::NoGesture)
            {
                if (ev->touchPoints().size() == 2)
                {
                    QTouchEvent::TouchPoint p1 = ev->touchPoints().at(0);
                    QTouchEvent::TouchPoint p2 = ev->touchPoints().at(1);

                    //                    /////////////////////////////////////////////////////
                    //                    /// WE CHECK TO SEE IF THE TOUCH POINTS ARE WITHIN Close PROXIMITY
                    //                    ///
                    //                    ///


                    //                    QPointF distance =  p1.pos() - p2.pos();
                    //                    int distPos = distance.manhattanLength();

                    //                    distance = p1.startPos() - p2.startPos();
                    //                    int distStart = distance.manhattanLength();


                    //                    //                std::cout << "Start Difference: " << distStart << "\t"
                    //                    //                          << "Pos Difference: " << distPos << "\t";

                    //                    if((distStart < 100) && (distPos < 100))
                    //                    {
                    //                        closeTouch = true;
                    //                    }


                    //                    /////////////////////////////////////////////////////

                    //                    if (closeTouch)
                    //                    {


                    QPointF startPos = (p1.startPos() + p2.startPos()) / 2;
                    QPointF newPos = (p1.pos() + p2.pos())/2;

//                    q->setLastOffset(q->offset());
//                    q->setOffset(newPos);




                    q->setLastOffset(q->offset());
                    q->setOffset(newPos);
                }

                result = QGestureRecognizer::FinishGesture;
            } /// if (q->state() != Qt::NoGesture)
            else
            {
                result = QGestureRecognizer::CancelGesture;
                closeTouch = false;
            }
            break;
        }   /// case TouchEnd
        case QEvent::TouchUpdate:
        {

            if (ev->touchPoints().size() == 2)
            {

                QTouchEvent::TouchPoint p1 = ev->touchPoints().at(0);
                QTouchEvent::TouchPoint p2 = ev->touchPoints().at(1);

                /////////////////////////////////////////////////////
                /// WE CHECK TO SEE IF THE TOUCH POINTS ARE WITHIN Close PROXIMITY
                ///
                ///


                QPointF distance =  p1.pos() - p2.pos();
                int distPos = distance.manhattanLength();

                distance = p1.startPos() - p2.startPos();
                int distStart = distance.manhattanLength();


                //                std::cout << "Start Difference: " << distStart << "\t"
                //                          << "Pos Difference: " << distPos << "\t";

                if((distStart < 100) && (distPos < 100))
                {
                    closeTouch = true;
                }


                /////////////////////////////////////////////////////
                // If the fingers are close proximity to each other. we take the average between them and set them of as a new offset

                if (closeTouch)
                {

                   qreal offsetX, offsetY, lastOffsetX, lastOffsetY;


                   QPointF lastPos = (p1.lastPos() + p2.lastPos()) / 2;
                   QPointF newPos = (p1.pos() + p2.pos())/2;

                    q->setLastOffset(lastPos);
                    q->setOffset(newPos);


//                    offsetX = q->offset().x();
//                    offsetY = q->offset().y();

//                    lastOffsetX = q->offset().x();
//                    lastOffsetY = q->offset().y();


                    if (
                            (q->offset().x() > 1  || q->offset().y() > 1 ||
                             q->offset().x() < -1 || q->offset().y() < -1))
                    {
                        q->setHotSpot(newPos);
                        result = QGestureRecognizer::TriggerGesture;
                    }
                    else
                    {
                        result = QGestureRecognizer::MayBeGesture;
                    }

                }   //If(closeTouch)
                else

                    result = QGestureRecognizer::MayBeGesture;

            } // if (touchPoints.size > 2)
            break;

        }    // Case::TouchUpdate

        default:
            result = QGestureRecognizer::Ignore;
            break;

        }   //switch (event->type())

        return result;
    } // Result recognise()



    void reset(QGesture *state)
    {
        QPanGesture *pan = static_cast<QPanGesture*>(state);

        //pan->setOffset(QPointF());
        pan->setAcceleration(0);

        QGestureRecognizer::reset(state);
    }

};


class QVTKTouchWidget : public QVTKWidget
{
    Q_OBJECT
public:

    explicit QVTKTouchWidget(QWidget *parent = 0);
    void enableGestures();
    void dissableGestures();

    void printGestureInfo(QGestureEvent *event);
    void setGesturesActive(bool status);
    void setUserTestMode(bool status);
    void setTransformsOn(bool status);


protected:
    bool event(QEvent *event);
    void paintEvent(QPaintEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);



private:
    bool gestureEvent(QGestureEvent *event);    
    void pinchTriggered(QPinchGesture *);
    void panTriggered(QPanGesture *);

    float horizonatlOffset;
    float verticalOffset;
    float rotationAngle;
    float scaleFactor;
    float lastScaleFactor;
    bool scaleSequenceNew;
    int lastGesture = 0;        /// 1 = Rotation, 2 = Translation, 3 = Scaling
    int touchPointBuffer = 0;
    int touchGestureCounter = 0;
    int rotationPointBuffer = 0;
    int touchEventDelay = 8;         /// Used as a buffer to simulate delay

    Qt::GestureType pan2Finger;

    TranslateRecognizer * translateGesture;

    vtkCamera * camera;

    bool gesturesActive = false;

    bool userTestRunning = false;

    bool transformsOn = true;

signals:

    void rotateTriggered();
    void translateTriggered();
    void scaleTriggered();

    void rotationPressed();
    void rotationReleased();

    void translationAction();           /// USED FOR TRANSLATION LABEL UPDATES
    void translationPressed();
    void translationReleased();

    void userTestTranslateCount();

public slots:

};

#endif // TOUCHWIDGET_H
