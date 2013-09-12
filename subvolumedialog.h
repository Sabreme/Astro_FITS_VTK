#ifndef SUBVOLUMEDIALOG_H
#define SUBVOLUMEDIALOG_H

#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkfitsreader.h>
#include <QDialog>

namespace Ui {
class SubVolumeDialog;
}

class SubVolumeDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SubVolumeDialog(QWidget *parent = 0);
    ~SubVolumeDialog();

    void UpdateQVTKWidget(QVTKWidget *qvtkWidget, vtkFitsReader *source);
    
    Ui::SubVolumeDialog *ui;
};

#endif // SUBVOLUMEDIALOG_H
