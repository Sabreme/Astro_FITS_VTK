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

    Result recognize(QGesture *state,
                                                                QObject *,
                                                                QEvent *event)
    {
        QPanGesture *q = static_cast<QPanGesture *>(state);

        const QTouchEvent *ev = static_cast<const QTouchEvent *>(event);

        QGestureRecognizer::Result result;
        switch (event->type()) {
        case QEvent::TouchBegin: {
            result = QGestureRecognizer::MayBeGesture;
            QTouchEvent::TouchPoint p = ev->touchPoints().at(0);

            q->setOffset(QPointF());
            q->setLastOffset(q->offset());
            //d->lastOffset = d->offset = QPointF();
            break;
        }
        case QEvent::TouchEnd: {
            if (q->state() != Qt::NoGesture) {
                if (ev->touchPoints().size() == 2) {
                    QTouchEvent::TouchPoint p1 = ev->touchPoints().at(0);
                    QTouchEvent::TouchPoint p2 = ev->touchPoints().at(1);
                    q->setLastOffset(q->offset());
                    q->setOffset(
                            QPointF(p1.pos().x() - p1.startPos().x() + p2.pos().x() - p2.startPos().x(),
                                  p1.pos().y() - p1.startPos().y() + p2.pos().y() - p2.startPos().y()) / 2);
                }
                result = QGestureRecognizer::FinishGesture;
            } else {
                result = QGestureRecognizer::CancelGesture;
            }
            break;
        }
        case QEvent::TouchUpdate: {
            if (ev->touchPoints().size() >= 2) {
                QTouchEvent::TouchPoint p1 = ev->touchPoints().at(0);
                QTouchEvent::TouchPoint p2 = ev->touchPoints().at(1);
                std::cout << "TouchUpdate in translate" << endl;
                q->setLastOffset(q->offset());
                q->setOffset(
                        QPointF(p1.pos().x() - p1.startPos().x() + p2.pos().x() - p2.startPos().x(),
                              p1.pos().y() - p1.startPos().y() + p2.pos().y() - p2.startPos().y()) / 2);
                if (q->offset().x() > 20  || q->offset().y() > 20 ||
                    q->offset().x() < -20 || q->offset().y() < -20) {
                    q->setHotSpot(p1.startScreenPos());
                    result = QGestureRecognizer::TriggerGesture;
                } else {
                    result = QGestureRecognizer::MayBeGesture;
                }
            }
            break;
        }
        default:
            result = QGestureRecognizer::Ignore;
            break;
        }
        return result;
    }

    void reset(QGesture *state)
    {
        QPanGesture *pan = static_cast<QPanGesture*>(state);

        pan->setOffset(QPointF());
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

protected:
    bool event(QEvent *event);
    void paintEvent(QPaintEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);



private:
    bool gestureEvent(QGestureEvent *event);    
    void pinchTriggered(QPinchGesture *);

    float horizonatlOffset;
    float verticalOffset;
    float rotationAngle;
    float scaleFactor;
    float currentStepScaleFactor;

    Qt::GestureType panGesture5;

    TranslateRecognizer * translateGesture;

    vtkCamera * camera;

    bool gesturesActive = false;

signals:
    void scaleTriggered();

public slots:

};

#endif // TOUCHWIDGET_H
