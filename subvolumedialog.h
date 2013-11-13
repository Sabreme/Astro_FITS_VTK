#ifndef SUBVOLUMEDIALOG_H
#define SUBVOLUMEDIALOG_H

#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkfitsreader.h>
#include <QDialog>
#include <vtkCubeSource.h>

#include <QFutureWatcher>

namespace Ui {
class SubVolumeDialog;
}

class SubVolumeDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SubVolumeDialog(QWidget *parent = 0);
    ~SubVolumeDialog();

    void UpdateQVTKWidget(QVTKWidget *qvtkWidget, vtkFitsReader *source, vtkCubeSource *subVolume);

    //void on_pushButton_clicked();
    
    Ui::SubVolumeDialog *ui;
};

#endif // SUBVOLUMEDIALOG_H
