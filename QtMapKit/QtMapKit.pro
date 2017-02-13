#-------------------------------------------------
#
# Project created by QtCreator 2013-07-08T08:47:20
#
#-------------------------------------------------

QT       += webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = qtmapkit

SOURCES += \
    QMMapView.cpp \
    QMCoordinate.cpp \
    QMCoordinateRegion.cpp \
    QMCoordinateSpan.cpp

HEADERS += QtMapKit.h\
    qtmapkit_global.h \
    QMMapView.h \
    QMCoordinate.h \
    QMCoordinateRegion.h \
    QMCoordinateSpan.h

RESOURCES += \
    qtmapkit_resources.qrc

OTHER_FILES += \
    gmap.html \
    gmap.js
