#ifndef INFOBARDIALOG_H
#define INFOBARDIALOG_H

#include <QDialog>
#include <vtkfitsreader.h>

namespace Ui {
class InfoBarDialog;
}

class InfoBarDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InfoBarDialog(QWidget *parent = 0);
    ~InfoBarDialog();

    void UpdateFitsInfo(vtkFitsReader *reader);

    Ui::InfoBarDialog * ui;

private:
    vtkFitsReader * global_Reader;
    
signals:
    
public slots:
    
private slots:
    void on_buttonClose_clicked();
};

#endif // INFOBARDIALOG_H
