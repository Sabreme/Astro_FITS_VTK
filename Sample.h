#ifndef SAMPLE_H
#define SAMPLE_H

#include "Leap.h"
#include <vtkSmartPointer.h>
#include "QVTKWidget.h"
#include "QVTKInteractor.h"
class vtkCamera;
class MainWindow;
//class QVTKWidget;



using namespace Leap;

class SampleListener : public Listener {
  public:
    SampleListener(vtkCamera * camera);
    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);


private:
    vtkCamera* camera_;
    MainWindow * mw_;
    QVTKInteractor * interactor_;



};

#endif // SAMPLE_H
