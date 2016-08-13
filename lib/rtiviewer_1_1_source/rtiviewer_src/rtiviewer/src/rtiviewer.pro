TEMPLATE = app
TARGET = RTIViewer
LANGUAGE = C++
CONFIG += qt debug_and_release xml network opengl warn_off #console

DESTDIR = bin
QT += opengl xml network

UI_DIR = ui
MOC_DIR = moc

win32-msvc2005:QMAKE_LFLAGS   += /LARGEADDRESSAWARE 
win32-msvc2008:QMAKE_LFLAGS   += /LARGEADDRESSAWARE
win32-msvc2010:QMAKE_LFLAGS   += /LARGEADDRESSAWARE

win32-msvc2005:QMAKE_CXXFLAGS   += /O2 /Ot /Oi /openmp /Zp16 /fp:fast /arch:SSE2
win32-msvc2008:QMAKE_CXXFLAGS   += /O2 /Ot /Oi /openmp /Zp16 /fp:fast /arch:SSE2
win32-msvc2010:QMAKE_CXXFLAGS   += /O2 /Ot /Oi /openmp /Zp16 /fp:fast /arch:SSE2

win32-g++:QMAKE_LIBS += -lgomp
win32-g++:QMAKE_LFLAGS += -m32
win32-g++:QMAKE_CXXFLAGS += -msse2 -fopenmp
#win32-g++:QMAKE_CXXFLAGS += -O3 -msse2 -fopenmp -funroll-loops -ffast-math -fforce-addr -fno-math-errno -ftree-vectorize

win32-g++:QMAKE_CFLAGS += -msse2 -fopenmp
#win32-g++:QMAKE_CFLAGS += -O3 -msse2 -fopenmp -funroll-loops -ffast-math -fforce-addr -fno-math-errno -ftree-vectorize

macx-g++:QMAKE_LIBS += -lgomp
macx-g++:QMAKE_LFLAGS += -m32
#macx-g++:QMAKE_CXXFLAGS += -msse2 -fopenmp
#macx-g++:QMAKE_CFLAGS += -msse2 -fopenmp
macx-g++:QMAKE_CXXFLAGS += -O3 -msse2 -fopenmp -funroll-loops -ffast-math -fforce-addr -fno-math-errno -ftree-vectorize

macx-g++:QMAKE_CFLAGS += -O3 -msse2 -fopenmp -funroll-loops -ffast-math -fforce-addr -fno-math-errno -ftree-vectorize

# macx-g++:QMAKE_LFLAGS = -O3
# macx-g++:QMAKE_CXXFLAGS = -O3
# macx-g++:QMAKE_CFLAGS = -O3

INCLUDEPATH += ../../../../vcglib \
    ../../compression/src/ \
    ../../rtiwebmaker/src

SOURCES = ptm.cpp \
    gui.cpp \
    main.cpp \
    rtiBrowser.cpp \
    lightControl.cpp \
    renderingdialog.cpp \
    diffusegain.cpp \
    specularenhanc.cpp \
    navigator.cpp \
    loadingdlg.cpp \
    openremotedlg.cpp \
    httpthread.cpp \
    normalenhanc.cpp \
    unsharpmasking.cpp \
    coeffenhanc.cpp \
    detailenhanc.cpp \
    dyndetailenhanc.cpp \
    hsh.cpp \
    universalrti.cpp \
    multiviewrti.cpp \
    rendercontrolutils.cpp \
    bookmarkcontrol.cpp \
    normalsrendering.cpp \
    aboutdlg.cpp

HEADERS = rti.h \
    ptm.h \
    gui.h \
    ../../rtiwebmaker/src/zorder.h \
    rtiBrowser.h \
    lightControl.h \
    renderingdialog.h \
    renderingmode.h \
    diffusegain.h \
    specularenhanc.h \
    navigator.h \
    loadingdlg.h \
    openremotedlg.h \
    httpthread.h \
    util.h \
    normalenhanc.h \
    unsharpmasking.h \
    coeffenhanc.h \
    detailenhanc.h \
    dyndetailenhanc.h \
    configdlg.h \
    pyramid.h \
    hsh.h \
    universalrti.h \
    multiviewrti.h \
    defaultrendering.h \
    ptmCoeffVectorized.h \
    rendercontrolutils.h \
    bookmarkcontrol.h\
    SysInfo.h\
    normalsrendering.h \
    aboutdlg.h

# FORMS =

RESOURCES = rtiviewer.qrc
win32:RC_FILE = rtiviewer.rc

mac:QMAKE_INFO_PLIST = ../install/Info.plist

# to add MacOS icon
mac:ICON = images/rtiviewer.icns
DEFINES += PRINT_DEBUG
#DEFINES += _YES_I_WANT_TO_USE_DANGEROUS_STUFF
mac:LIBS += ../../compression/src/lib/libjpeg2000.a
win32-msvc2005:LIBS += ../../compression/src/lib/jpeg2000.lib
win32-msvc2008:LIBS += ../../compression/src/lib/jpeg2000.lib
win32-msvc2010:LIBS += ../../compression/src/lib/jpeg2000.lib
win32-g++:LIBS += ../../compression/src/lib/libjpeg2000.a
