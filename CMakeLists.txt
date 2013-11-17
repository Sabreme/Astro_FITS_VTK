if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("debug")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib-debug)
    set(LIB_INSTALL_DIR ${CMAKE_SOURCE_DIR}/lib-debug)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin-dbg)
else()
    message("not debug")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
    set(LIB_INSTALL_DIR ${CMAKE_SOURCE_DIR}/lib)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
endif()

CMAKE_MINIMUM_REQUIRED (VERSION 2.8.6)
PROJECT (Astro_FITS_VTK)

SET (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O2 -g -std=c++0x -gdwarf-2"  )

FIND_PACKAGE(Qt4 REQUIRED)
#set(VTK_DIR "PATH/TO/VTK/BUILD/DIRECTORY")
find_package(VTK REQUIRED)

include(${VTK_USE_FILE})
include(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

FILE (GLOB SOURCES *.cpp *.cxx *.cc *.C *.c)
FILE (GLOB HEADERS *.h *.hpp)
FILE (GLOB UI *.ui)

SET(SOURCES
    main.h main.cpp mainwindow.cpp vtkfitsreader.cpp
    fits_results.cpp subvolumedialog.cpp sliceDialog.cpp
    vtkFrameRateWidget.cpp
    #processfile.cpp
)
SET(HEADERS
    mainwindow.h vtkfitsreader.h fits_results.h
    OrientationAxes.h
    ScalarBar.h
    subvolumedialog.h
    sliceDialog.h

    XYHighlightSelection.h
    ZHighlightSelection.h
    PlaneSelection.h
    vtkFrameRateWidget.h
    #processfile.h

)
SET(UI
    mainwindow.ui    
    subvolumedialog.ui
    sliceDialog.ui
)

QT4_WRAP_UI(UI_MOC ${UI})
QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${VTK_INCLUDE_DIRS} )

ADD_EXECUTABLE(Astro_FITS_VTK ${SOURCES} ${HEADERS_MOC} ${UI_MOC} ${HEADERS})
target_link_libraries(Astro_FITS_VTK ${VTK_LIBRARIES} ${QT_LIBRARIES})

link_directories("/usr/lib/x86_64-linux-gnu")
target_link_libraries(Astro_FITS_VTK cfitsio QVTK)

FOREACH(LIBRARY ${VTK_LIBRARIES})
message("Loading VTK_Library: " ${LIBRARY})
endforeach()

FOREACH(LIBRARY ${QT_LIBRARIES})
message("Loading QT_Library: " ${LIBRARY})
endforeach()
