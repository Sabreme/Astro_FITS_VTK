#-------------------------------------------------
#
# Project created by QtCreator 2014-02-07T03:45:10
#
#-------------------------------------------------

QT       += core gui

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Astro_FITS_VTK

TEMPLATE = app

CONFIG +=

include(src/src.pri)
include(InfoDlg/InfoDlg.pri)
include(SubVolume/SubVolume.pri)
include(System/System.pri)
include(Slicing/Slicing.pri)
include(Leap/Leap.pri)

unix {
LIBS += -L/home/pmulumba/VTK5.10.1/bin -lQVTK -lvtkVolumeRendering -lvtkHybrid -lvtkWidgets   \
        -lvtkInfovis -lvtkGeovis -lvtkViews -lvtkCharts -lvtkFiltering -lvtkImaging \
        -lvtkGraphics -lvtkGenericFiltering -lvtkIO -lvtkRendering -lvtkCommon  \
        -lcfitsio -lLeap

INCLUDEPATH += /usr/local/include/vtk-5.10
}

win32 {LIBS += -LC:/VTK5.10.1-bin/bin -lQVTK -lvtkVolumeRendering -lvtkHybrid -lvtkWidgets   \
        -lvtkInfovis -lvtkGeovis -lvtkViews -lvtkCharts -lvtkFiltering -lvtkImaging \
        -lvtkGraphics -lvtkGenericFiltering -lvtkIO -lvtkRendering -lvtkCommon  \
        -lcfitsio -lLeap

INCLUDEPATH += C:/VTK5.10.1-src/include/vtk-5.10
}

QMAKE_CXXFLAGS_DEBUG += -std=c++11

QMAKE_CXXFLAGS_RELEASE += -std=c++11

RESOURCES += \
    Icons/Icons.qrc

#unix:!macx: LIBS += -L$$PWD/Resources/LINUX/ -lQVTK -lvtkVolumeRendering -lvtkHybrid -lvtkWidgets \
#                                                  -lvtkFiltering -lvtkImaging -lvtkGraphics  -lvtkIO \
#                                                  -lvtkRendering -lvtkCommon -lLeap -lcfitsio

#INCLUDEPATH += $$PWD/Resources/LINUX

#DEPENDPATH += $$PWD/Resources/LINUX
