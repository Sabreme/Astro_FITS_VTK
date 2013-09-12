#ifndef SUBVOLUME_H
#define SUBVOLUME_H

#include <QMainWindow>
#include <QApplication>

namespace Ui {
    class SubVolume;
}

class SubVolume : public QMainWindow
{
    Q_OBJECT

public:
    explicit SubVolume(QWidget *parent = 0);
    ~SubVolume();

protected:
        void ModifiedHandler();

private:
    Ui::SubVolume *ui;
};

#endif // SUBVOLUME_H
