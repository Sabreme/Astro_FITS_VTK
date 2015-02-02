#ifndef LEAPDIALOG_H
#define LEAPDIALOG_H

#include <QDialog>

#include "vtkPlaneWidget.h"
#include "vtkArrowSource.h"
#include "vtkSphereSource.h"
#include "vtkPointWidget.h"
#include "vtkActor.h"

namespace Ui {
class LeapDialog;
}

class LeapDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LeapDialog(QWidget *parent = 0);
    ~LeapDialog();

    void leapDiagnostic();


    
private:
    Ui::LeapDialog *ui;

    vtkPlaneWidget * leapDbgPlaneWidget;        // leap Diagnostic DIsplay
    vtkArrowSource * leapDbgArrow;
    vtkPlaneSource * leapDbgPlane;
    vtkSphereSource * leapDbgSphere;
    vtkPointWidget * leapDbgPointWidget;

    vtkActor       * leapDbgSphereActor;
    vtkActor       * leapDbgArrowActor;
};

#endif // LEAPDIALOG_H
