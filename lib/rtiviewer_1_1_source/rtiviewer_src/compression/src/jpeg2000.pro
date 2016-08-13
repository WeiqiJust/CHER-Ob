TEMPLATE      = lib
TARGET         = jpeg2000
LANGUAGE       = C++
CONFIG += qt debug_and_release static warn_off

DESTDIR = ./lib

win32: DEFINES += OPJ_STATIC


win32-msvc2005:QMAKE_LFLAGS   += /LARGEADDRESSAWARE
win32-msvc2008:QMAKE_LFLAGS   += /LARGEADDRESSAWARE

win32-msvc2005:QMAKE_CXXFLAGS   += /O2 /Ot /Oi /openmp /Zp16 /fp:fast /arch:SSE2
win32-msvc2008:QMAKE_CXXFLAGS   += /O2 /Ot /Oi /openmp /Zp16 /fp:fast /arch:SSE2

win32-g++:QMAKE_LIBS +=
win32-g++:QMAKE_LFLAGS += -m32
win32-g++:QMAKE_CXXFLAGS += -O3 -funroll-loops -ffast-math -fforce-addr -fno-math-errno -ftree-vectorize
win32-g++:QMAKE_CFLAGS += -O3 -funroll-loops -ffast-math -fforce-addr -fno-math-errno -ftree-vectorize

#macx-g++:QMAKE_LIBS +=
macx-g++:QMAKE_LFLAGS += -m32
macx-g++:QMAKE_CXXFLAGS += -O3 -funroll-loops -ffast-math -fforce-addr -fno-math-errno -ftree-vectorize
macx-g++:QMAKE_CFLAGS += -O3 -funroll-loops -ffast-math -fforce-addr -fno-math-errno -ftree-vectorize

SOURCES        = jpeg2000.cpp\
				openjpeg/bio.c \
				openjpeg/cio.c \
				openjpeg/dwt.c \
				openjpeg/event.c \
				openjpeg/image.c \
				openjpeg/j2k.c \
				openjpeg/j2k_lib.c \
				openjpeg/jp2.c \
				openjpeg/jpt.c \
				openjpeg/mct.c \
				openjpeg/mqc.c \
				openjpeg/openjpeg.c \
				openjpeg/pi.c \
				openjpeg/raw.c \
				openjpeg/t1.c \
				openjpeg/t1_generate_luts.c \
				openjpeg/t2.c \
				openjpeg/tcd.c \
				openjpeg/tgt.c
				

HEADERS        = jpeg2000.h\
				openjpeg/bio.h \
				openjpeg/cio.h \
				openjpeg/dwt.h \
				openjpeg/event.h \
				openjpeg/fix.h \
				openjpeg/image.h \
				openjpeg/int.h \
				openjpeg/j2k.h \
				openjpeg/j2k_lib.h \
				openjpeg/jp2.h \
				openjpeg/jpt.h \
				openjpeg/mct.h \
				openjpeg/mqc.h \
				openjpeg/openjpeg.h \
				openjpeg/opj_includes.h \
				openjpeg/opj_malloc.h \
				openjpeg/pi.h \
				openjpeg/raw.h \
				openjpeg/t1.h \
				openjpeg/t1_luts.h \
				openjpeg/t2.h \
				openjpeg/tcd.h \
				openjpeg/tgt.h
