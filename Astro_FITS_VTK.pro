#-------------------------------------------------
#
# Project created by QtCreator 2014-02-07T03:45:10
#
#-------------------------------------------------

QT       += core gui

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Astro_FITS_VTK
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    vtkfitsreader.cpp \
    vtkFrameRateWidget.cpp \
    subvolumedialog.cpp \
    sliceDialog.cpp \
    vtkimplicitcustomplanewidget.cpp \
    infobardialog.cpp \
    leapdialog.cpp \
    vtkLeapMarkerWidget.cxx

HEADERS  += mainwindow.h \
    vtkfitsreader.h \
    main.h \
    OrientationAxes.h \
    ScalarBar.h \
    vtkFrameRateWidget.h \
    subvolumedialog.h \
    XYHighlightSelection.h \
    ZHighlightSelection.h \
    sliceDialog.h \       
    Sample.h \
    vtkimplicitcustomplanewidget.h \
    infobardialog.h \
    leapdialog.h \
    vtkLeapMarkerWidget.h

FORMS    += mainwindow.ui \
    subvolumedialog.ui \
    sliceDialog.ui \
    InfoBarDialog.ui \
    leapdialog.ui

unix {LIBS += -L/home/pmulumba/VTK5.10.1/bin -lQVTK -lvtkVolumeRendering -lvtkHybrid -lvtkWidgets   \
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

QMAKE_CXXFLAGS_DEBUG += -g  -std=c++0x

CONFIG +=

RESOURCES += \
    Icons/Icons.qrc
