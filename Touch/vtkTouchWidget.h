#ifndef TOUCHWIDGET_H
#define TOUCHWIDGET_H

#include "QVTKWidget.h"
#include <QtGui>
#include <iostream>

#include "vtkCamera.h"
#include "vtkSystemIncludes.h"
#include <src/mainwindow.h>
//#include "src/InteractionClasses.h"
#include "vtkActor2D.h"
#include "vtkProp.h"
#include "vtkProp3D.h"
#include "vtkActor.h"



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
    void setUserTestMode(bool status);
    void setTransformsOn(bool status);    
    bool GetTransformsOn();
    void SetSubVolumeOn(bool status);
    bool GetSubVolumeOn();


    vtkActor2D *      fingerActor1;
    vtkActor2D *      fingerActor2;
    vtkActor2D *      fingerActor3;
    vtkActor2D *      fingerActor4;
    vtkActor2D *      fingerActor5;




protected:
    bool event(QEvent *event);
    //void paintEvent(QPaintEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);



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
    int touchTransformBuffer = 0;
    int touchGestureCounter = 0;
    int rotationPointBuffer = 0;
    int touchEventDelay = 8;         /// Used as a buffer to simulate delay

    Qt::GestureType pan2Finger;    

    vtkCamera * camera;

    bool gesturesActive = false;

    bool userTestRunning = false;

    bool rightMouseClick = false;
    bool transformsOn = true;
    bool subVolumeOn = false;



    double colorRotation[3] = {0.1, 0.4, 0.5};

    double colorTranslate1[3] = {0.5, 0.6, 0.7};
    double colorTranslate2[3] = {0.5, 1, 0.7};

    double colorScale1[3] = {0.7, 0.6, 0.8};
    double colorScale2[3] = {0.7, 1, 0.8};

    double colorSpin1[3] = {0.8, 0.8, 0.8};
    double colorSpin2[3] = {0.8, 1, 0.8};
    double colorSpin3[3] = {0.1, 1, 0.8};

signals:

    void rotateTriggered();
    void translateTriggered();
    void scaleTriggered();    

    void rotationPressed();
    void rotationReleased();

    void translationAction();           /// USED FOR TRANSLATION LABEL UPDATES
    void translationPressed();
    void translationReleased();

    void transformationTriggered();

    void finger1Pressed();
    void finger2Pressed();

    void userTestTranslateCount();

public slots:

};

#endif // TOUCHWIDGET_H
