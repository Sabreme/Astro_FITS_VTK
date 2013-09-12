#include <QtGui/QApplication>
#include "mainwindow.h"

#include <iostream>
#include <QDebug>

MainWindow * mw;

QString backlog;

void myMsgHandler(QtMsgType, const char * message)
{
    //std::cerr << message;
    if(mw != NULL) {
        if (backlog.size() != 0){
            backlog += message;
            mw->log(backlog);
        }
        else
        {
            mw->log(message);
        }
    }
    else {
        //std::cerr << message;
        backlog += message;
    }
}

int main(int argc, char *argv[])
{
    qInstallMsgHandler(myMsgHandler);
    QApplication a(argc, argv);
    MainWindow w;
    mw = &w;    

    qDebug();
    w.show();    

    return a.exec();
}
