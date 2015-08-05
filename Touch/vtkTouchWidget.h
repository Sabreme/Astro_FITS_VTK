#ifndef TOUCHWIDGET_H
#define TOUCHWIDGET_H

#include "QVTKWidget.h"
#include <QtGui>
#include <iostream>

#include "vtkCamera.h"
#include "vtkSystemIncludes.h"


using namespace std;


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

    vtkCamera * camera;

    bool gesturesActive = false;

signals:

public slots:

};

#endif // TOUCHWIDGET_H
