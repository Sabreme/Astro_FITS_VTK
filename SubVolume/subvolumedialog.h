#ifndef SUBVOLUMEDIALOG_H
#define SUBVOLUMEDIALOG_H

#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <System/vtkfitsreader.h>
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

    void UpdateQVTKWidget(QVTKWidget *qvtkWidget, vtkFitsReader *source, double subVolBounds[6]);

    //void on_pushButton_clicked();
    
    Ui::SubVolumeDialog *ui;

signals:
    void windowClosing(bool);
private slots:
    void on_buttonBox_accepted();
};

#endif // SUBVOLUMEDIALOG_H
