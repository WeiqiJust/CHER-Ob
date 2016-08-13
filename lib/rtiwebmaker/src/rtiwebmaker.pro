TEMPLATE       = app
TARGET         = rtiwebmaker
LANGUAGE       = C++

DESTDIR        = bin
CONFIG         += qt debug_and_release network console warn_off

QT             += xml

MOC_DIR        = moc

win32-msvc2005:QMAKE_LFLAGS   += /LARGEADDRESSAWARE
win32-msvc2008:QMAKE_LFLAGS   += /LARGEADDRESSAWARE
win32-msvc2010:QMAKE_LFLAGS   += /LARGEADDRESSAWARE

win32-msvc2005:QMAKE_CXXFLAGS   += /O2 /Ot /Oi /openmp /Zp16 /fp:fast /arch:SSE2
win32-msvc2008:QMAKE_CXXFLAGS   += /O2 /Ot /Oi /openmp /Zp16 /fp:fast /arch:SSE2
win32-msvc2010:QMAKE_CXXFLAGS   += /O2 /Ot /Oi /openmp /Zp16 /fp:fast /arch:SSE2

win32-g++:QMAKE_LIBS += -lgomp
win32-g++:QMAKE_LFLAGS += -m32
win32-g++:QMAKE_CXXFLAGS += -O3 -msse2 -fopenmp -funroll-loops -ffast-math -fforce-addr -fno-math-errno -ftree-vectorize
win32-g++:QMAKE_CFLAGS += -O3 -msse2 -fopenmp -funroll-loops -ffast-math -fforce-addr -fno-math-errno -ftree-vectorize

macx-g++:QMAKE_LIBS += -lgomp
macx-g++:QMAKE_LFLAGS += -m32
macx-g++:QMAKE_CXXFLAGS += -O3 -fopenmp -funroll-loops -ffast-math -fforce-addr -fno-math-errno -ftree-vectorize
macx-g++:QMAKE_CFLAGS += -O3 -fopenmp -funroll-loops -ffast-math -fforce-addr -fno-math-errno -ftree-vectorize

INCLUDEPATH    += ../../../../vcglib \
                  ../../rtiviewer/src \
                  ../../compression/src/ \


SOURCES        =  \
               main.cpp \
               ../../rtiviewer/src/ptm.cpp \
               ../../rtiviewer/src/loadingdlg.cpp \
               ../../rtiviewer/src/diffusegain.cpp \
               ../../rtiviewer/src/specularenhanc.cpp \
               ../../rtiviewer/src/normalenhanc.cpp \
               ../../rtiviewer/src/unsharpmasking.cpp \
               ../../rtiviewer/src/coeffenhanc.cpp\
               ../../rtiviewer/src/detailenhanc.cpp\
               ../../rtiviewer/src/dyndetailenhanc.cpp\
               ../../rtiviewer/src/hsh.cpp\
               ../../rtiviewer/src/universalrti.cpp\
               ../../rtiviewer/src/normalsrendering.cpp\
               ../../rtiviewer/src/rendercontrolutils.cpp

HEADERS        = \
               zorder.h \
               ../../rtiviewer/src/rti.h \
               ../../rtiviewer/src/ptm.h \
               ../../rtiviewer/src/loadingdlg.h \               
               ../../rtiviewer/src/diffusegain.h \
               ../../rtiviewer/src/specularenhanc.h \
               ../../rtiviewer/src/normalenhanc.h \
               ../../rtiviewer/src/unsharpmasking.h \
               ../../rtiviewer/src/coeffenhanc.h\
               ../../rtiviewer/src/detailenhanc.h\
               ../../rtiviewer/src/dyndetailenhanc.h\
               ../../rtiviewer/src/defaultrendering.h\
               ../../rtiviewer/src/hsh.h\
               ../../rtiviewer/src/universalrti.h\
               ../../rtiviewer/src/normalsrendering.h\
               ../../rtiviewer/src/rendercontrolutils.h


#DEFINES += _YES_I_WANT_TO_USE_DANGEROUS_STUFF

mac: LIBS       += ../../compression/src/lib/libjpeg2000.a
win32-msvc2005: LIBS  += ../../compression/src/lib/jpeg2000.lib
win32-msvc2008: LIBS  += ../../compression/src/lib/jpeg2000.lib
win32-msvc2010: LIBS  += ../../compression/src/lib/jpeg2000.lib
win32-g++: LIBS += ../../compression/src/lib/libjpeg2000.a

mac: CONFIG -= app_bundle
