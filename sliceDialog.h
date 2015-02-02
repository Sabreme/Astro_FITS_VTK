#ifndef SLICEDIALOG_H
#define SLICEDIALOG_H

#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkfitsreader.h>
#include <QDialog>
#include "vtkSmartPointer.h"
#include "vtkResliceImageViewer.h"
#include "vtkImagePlaneWidget.h"
#include "vtkDistanceWidget.h"
#include "vtkResliceImageViewerMeasurements.h"

#include <QFutureWatcher>
#include <qcombobox.h>
#include <qpalette.h>
#include <qcolordialog.h>
#include <vtkCubeAxesActor.h>
#include <vtkImageActor.h>
#include <vtkImageViewer2.h>
#include <vtkLookupTable.h>
#include <vtkScalarsToColors.h>
#include <qfiledialog.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkImageProperty.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkCamera.h>


namespace Ui {
class SliceDialog;
}

class SliceDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SliceDialog(QWidget *parent = 0);
    ~SliceDialog();

    void UpdateQVTKWidget(vtkResliceImageViewer *riw, int axis);
    virtual void Render();

    virtual void AlignCamera();
    virtual void CaptureImage();
    virtual void SetInterpolation();
    virtual void SetSlice(int slice);

    //void on_pushButton_clicked();
    
    Ui::SliceDialog *ui;

    void loadColorBoxes();
    void updateImage();

protected:
  vtkSmartPointer< vtkResliceImageViewer > riw[3];
  vtkSmartPointer< vtkImagePlaneWidget > planeWidget[3];
  vtkSmartPointer< vtkDistanceWidget > DistanceWidget[3];
  vtkSmartPointer< vtkResliceImageViewerMeasurements > ResliceMeasurements;

  QComboBox *colourBox[3];

  QColor color1;
  QColor color2;
  QColor color3;
  vtkSmartPointer<  vtkScalarsToColors  > originalTable;
  vtkSmartPointer<  vtkLookupTable      > customLookup;
  vtkSmartPointer<  vtkTextActor        > titleActor;

  vtkSmartPointer<vtkLookupTable > bwLut ;



//  QColorDialog colorDialog;

private slots:
  void on_closeButton_clicked();
  void on_button_Colour_clicked();
  void on_colorButton_clicked();
  void on_colorButton1_clicked();
  void on_colorButton2_clicked();
  void on_colorButton3_clicked();
  void on_pushButtonApply_clicked();
  void on_saveButton_clicked();
  void on_resetButton_clicked();
  void on_SaturationChanged(int value);
  void on_HueChanged(int value);
  void on_TableValueChanged(int value);
//  void on_horizontalSlider_Hue_actionTriggered(int action);
  void on_buttonInsertTitle_clicked();
  void on_pushButton_clicked();
  void on_buttonTitle_clicked();
};

#endif // SLICEDIALOG_H
