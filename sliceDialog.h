#ifndef SLICEDIALOG_H
#define SLICEDIALOG_H

#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkfitsreader.h>
#include <QDialog>
#include <vtkCubeSource.h>

#include <QFutureWatcher>

namespace Ui {
class SliceDialog;
}

class SliceDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SliceDialog(QWidget *parent = 0);
    ~SliceDialog();

    void UpdateQVTKWidget(QVTKWidget *qvtkWidget, vtkFitsReader *source);

    //void on_pushButton_clicked();
    
    Ui::SliceDialog *ui;
};

#endif // SLICEDIALOG_H
