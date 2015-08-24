include (../defaults.pri)

QT       += core gui

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

QT       += core gui

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Astro_FITS_VTK
TEMPLATE = app


SOURCES += infobardialog.cpp\      

HEADERS  += infobardialog.h \

FORMS    += infobardialog.ui \

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

CONFIG +=

RESOURCES +=

