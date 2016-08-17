###########################################################################
# Codename: CHER-Ob2 (KAIST-VCLAB)
#
# Writers:   Min H. Kim (minhkim@kaist.ac.kr)
#            Ying Yang  (ying.yang.yy368@yale.edu)
#
# GNU General Public License Usage
# Alternatively, this file may be used under the terms of the GNU General
# Public License version 3.0 as published by the Free Software Foundation
# and appearing in the file LICENSE.GPL included in the packaging of this
# file. Please review the following information to ensure the GNU General
# Public License version 3.0 requirements will be met:
# http://www.gnu.org/copyleft/gpl.html.
#
###########################################################################
# Third-Party APIs List:
# Qt-4.8.5
# VTK-5.10
# ITK-4.4
# QWT-6.1.0
# BLAS/LAPACK (for Windows)
# Armadillo-3.2.2
# OpenEXR-1.7.0 (depends on ilmbase-1.0.2 and zlib-1.2.8)
# boost-1.52.0
# armadillo-3.2.2
# Windows_SDK-7.0
# Mac_OS_X_SDK-10.6
###########################################################################
# Universal
###########################################################################
TEMPLATE = app
# final application name compiled (could be changed)
TARGET   = CHER-Ob
#======================================================================
HEADERS += ../src/mainWindow.h \
    ../src/vtkEnums.h \
    ../src/function/coeffenhanc.h \
    ../src/function/CTControl.h \
    ../src/function/defaultrendering.h \
    ../src/function/detailenhanc.h \
    ../src/function/diffusegain.h \
    ../src/function/dyndetailenhanc.h \
    ../src/function/htmlDelegate.h \
    ../src/function/knobWidget.h \
    ../src/function/lightControl.h \
    ../src/function/lightControlRTI.h \
    ../src/function/loadingdlg.h \
    ../src/function/mkColorConvert.h \
    ../src/function/mkTools.hpp \
    ../src/function/navigation.h \
    ../src/function/normalenhanc.h \
    ../src/function/normalsrendering.h \
    ../src/function/plotView.h \
    ../src/function/projectCITab.h \
    ../src/function/rendercontrolutils.h \
    ../src/function/renderingdialog.h \
    ../src/function/renderingmode.h \
    ../src/function/reportFilter.h \
    ../src/function/reportGenerator.h \
    ../src/function/rtiBrowser.h \
    ../src/function/spectralPlot.h \
    ../src/function/specularenhanc.h \
    ../src/function/unsharpmasking.h \
    ../src/io/hsh.h \
    ../src/io/inputimageset.h \
    ../src/io/multiviewrti.h \
    ../src/io/ptm.h \
    ../src/io/ptmCoeffVectorized.h \
    ../src/io/pyramid.h \
    ../src/io/readCHEROb.h \
    ../src/io/rti.h \
    ../src/io/universalrti.h \
    ../src/io/util.h \
    ../src/io/vtkOpenEXR.h \
    ../src/io/vtkPLYReader2.h \
    ../src/io/vtkVRMLSource2.h \
    ../src/visualization/lscm_engine.h \
    ../src/visualization/myVTKInteractorStyle.h \
    ../src/visualization/vcgRTIviewer.h \
    ../src/visualization/vtk2DInteractionCallback.hpp \
    ../src/visualization/vtk3DInteractionCallback.hpp \
    ../src/visualization/vtkInteractorStyleSurfaceWalkerCamera.h \
    ../src/visualization/vtkView.h \
    ../src/visualization/vtkWidget.h \
    ../src/NL/nl.h \
    ../src/CHE/CHEInfoDialog.h \
    ../src/CHE/CHETab.h \
    ../src/CHE/exportToProjectDialog.h \
    ../src/CHE/importFromCHEDialog.h \
    ../src/CHE/mergeBackToCHEDialog.h \
    ../src/CHE/mergeBackToCHELocationDialog.h \
    ../src/CHE/newCHEDialog.h \
    ../src/CHE/newCHEInfoDialog.h \
    ../src/CHE/saveCHEAsDialog.h \
    ../src/information/annotationFilterDialog.h \
    ../src/information/bookmark.h \
    ../src/information/bookmarkTreeWidget.h \
    ../src/information/bookmarkWidget.h \
    ../src/information/bookmarkXMLReader.h \
    ../src/information/fileInfoDialog.h \
    ../src/information/imageNote.h \
    ../src/information/informationWidget.h \
    ../src/information/metadata.h \
    ../src/information/newProjectDialog.h \
    ../src/information/note.h \
    ../src/information/openWindowDialog.h \
    ../src/information/projectInfoDialog.h \
    ../src/information/removeObjectDialog.h \
    ../src/information/saveProjectAsDialog.h \
    ../src/information/searchWidget.h
SOURCES += ../src/main.cpp \
    ../src/mainWindow.cpp \
    ../src/function/coeffenhanc.cpp \
    ../src/function/CTControl.cpp \
    ../src/function/detailenhanc.cpp \
    ../src/function/diffusegain.cpp \
    ../src/function/dyndetailenhanc.cpp \
    ../src/function/htmlDelegate.cpp \
    ../src/function/knobWidget.cpp \
    ../src/function/lightControl.cpp \
    ../src/function/lightControlRTI.cpp \
    ../src/function/loadingdlg.cpp \
    ../src/function/mkColorConvert.cpp \
    ../src/function/navigation.cpp \
    ../src/function/normalenhanc.cpp \
    ../src/function/normalsrendering.cpp \
    ../src/function/plotView.cpp \
    ../src/function/projectCITab.cpp \
    ../src/function/rendercontrolutils.cpp \
    ../src/function/renderingdialog.cpp \
    ../src/function/reportFilter.cpp \
    ../src/function/reportGenerator.cpp \
    ../src/function/rtiBrowser.cpp \
    ../src/function/spectralPlot.cpp \
    ../src/function/specularenhanc.cpp \
    ../src/function/unsharpmasking.cpp \
    ../src/io/hsh.cpp \
    ../src/io/inputimageset.cpp \
    ../src/io/multiviewrti.cpp \
    ../src/io/ptm.cpp \
    ../src/io/readCHEROb.cpp \
    ../src/io/universalrti.cpp \
    ../src/io/vtkOpenEXR.cpp \
    ../src/io/vtkPLYReader2.cpp \
    ../src/io/vtkVRMLSource2.cxx \
    ../src/CHE/CHEInfoDialog.cpp \
    ../src/CHE/CHETab.cpp \
    ../src/CHE/exportToProjectDialog.cpp \
    ../src/CHE/importFromCHEDialog.cpp \
    ../src/CHE/mergeBackToCHEDialog.cpp \
    ../src/CHE/mergeBackToCHELocationDialog.cpp \
    ../src/CHE/newCHEDialog.cpp \
    ../src/CHE/newCHEInfoDialog.cpp \
    ../src/CHE/saveCHEAsDialog.cpp \
    ../src/information/annotationFilterDialog.cpp \
    ../src/information/bookmark.cpp \
    ../src/information/bookmarkTreeWidget.cpp \
    ../src/information/bookmarkWidget.cpp \
    ../src/information/bookmarkXMLReader.cpp \
    ../src/information/fileInfoDialog.cpp \
    ../src/information/imageNote.cpp \
    ../src/information/informationWidget.cpp \
    ../src/information/metadata.cpp \
    ../src/information/newProjectDialog.cpp \
    ../src/information/note.cpp \
    ../src/information/openWindowDialog.cpp \
    ../src/information/projectInfoDialog.cpp \
    ../src/information/removeObjectDialog.cpp \
    ../src/information/saveProjectAsDialog.cpp \
    ../src/information/searchWidget.cpp \
    ../src/NL/nl_single_file.c \
    ../src/visualization/lscm_engine.cpp \
    ../src/visualization/vcgRTIviewer.cpp \
    ../src/visualization/vtkInteractorStyleSurfaceWalkerCamera.cpp \
    ../src/visualization/vtkView.cpp \
    ../src/visualization/vtkWidget.cpp

#======================================================================
RESOURCES = CHEROb.qrc
# these are required for deployment for Mac
QT += opengl
QT += xml
QT += xmlpatterns
QT += network
QT += script
QT += webkit
QT += sql


CONFIG += qt opengl warn_on thread rtti embed_manifest_exe stl

# install
target.path = ../
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS images
sources.path = .
INSTALLS += target sources

PROJECTDIR = sources.path

######################### IMPORTANT ##########################
#!Remember to modify the path where you install the libraries.

INCLUDEPATH += /usr/local/include/ITK-4.4
INCLUDEPATH += /usr/local/include/vtk-5.10
INCLUDEPATH += /usr/local/include/OpenEXR
INCLUDEPATH += /usr/local/qwt-6.1.0/lib/qwt.framework/Headers
INCLUDEPATH += /usr/local/include/boost
INCLUDEPATH += /usr/include/armadillo_bits
INCLUDEPATH += /usr/local/include/vcg/vcglib
INCLUDEPATH += /usr/local/include/vcg/compression/src
######################### IMPORTANT ##########################

###########################################################################
# Mac OS X (64bit)
###########################################################################
macx-g++{
CONFIG(release){
  # Optimization with OpenMP
  QMAKE_LIBS += -lgomp
  QMAKE_LFLAGS += -m32
  QMAKE_CXXFLAGS += -O3 -msse2 -fopenmp -funroll-loops -ffast-math -fforce-addr -fno-math-errno -ftree-vectorize
  QMAKE_CFLAGS += -O3 -msse2 -fopenmp -funroll-loops -ffast-math -fforce-addr -fno-math-errno -ftree-vectorize
  # Remove debugging options in release mode makes files much smaller
  QMAKE_CFLAGS_RELEASE -= -g
  QMAKE_CXXFLAGS_RELEASE -= -g
}
  # for deploy
  QMAKE_LFLAGS_SONAME  = -Wl,-install_name,@executable_path/../Frameworks/

  # framework dependencay
#  QMAKE_LFLAGS += -F/Library/Frameworks

  QMAKE_CXXFLAGS += -fpermissive

  QMAKE_BUNDLEID_PREFIX = YaleGraphics
  QMAKE_PINFO_BUNDLEID = YaleGraphics
  # for removing warning in compiling in Lion
  #QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.6.sdk
  #QMAKE_MAC_SDK = /Users/minhkim/MacOSX10.6.sdk

  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.5

  # icon folder
  ICON = ./icons/CHEROb_Logo.icns

  # target system
  CONFIG += x86_64 # only 64bit mac (Lion)
 # CONFIG -= debug_and_release
 # CONFIG *= release
 # CONFIG *= debug
 # CONFIG *= release
 # CONFIG *= debug_and_release

  # for OpenGL
  LIBS += -framework Foundation -framework Cocoa

  # macports include path
  INCLUDEPATH += .
  INCLUDEPATH += /opt/local/include
  INCLUDEPATH += /usr/local/include
  INCLUDEPATH += /usr/include/
  INCLUDEPATH += /usr/include/c++/4.2.1
  INCLUDEPATH += /usr/include/c++/4.2.1/backward

  # gcc compiling frags
  QMAKE_CXXFLAGS += -Wno-deprecated # header for C++ includes, or <iostream> instead of the deprecated header <iostream.h>

  # Info.plist control (yourcompany stuff)
  QMAKE_INFO_PLIST = Info.plist

  # Intermediate files are created in an hidden folder
  MOC_DIR = .moc
  OBJECTS_DIR = .obj


}


###########################################################################
# THIRD PARTY LIBRARIES
###########################################################################
#===========================================================
# Kitware ITK Library (http://www.vtk.org/) - DICOM IO and Image Processing,  BSD license
#!Remember to modify the path where you install the libraries.

CONFIG += ITK_USE_FILE

macx-g++{
INCLUDEPATH += /usr/local/include/ITK-4.4
LIBS += -L/usr/local/lib \
-lITKBiasCorrection-4.4 -lITKBioCell-4.4 -lITKCommon-4.4 -lITKDICOMParser-4.4 -lITKDeprecated-4.4 -lITKEXPAT-4.4 -lITKFEM-4.4 -lITKIOBMP-4.4 \
-lITKIOBioRad-4.4 -lITKIOCSV-4.4 -lITKIOGE-4.4 -lITKIOGIPL-4.4 -lITKIOHDF5-4.4 -lITKIOIPL-4.4 -lITKIOImageBase-4.4 -lITKIOJPEG-4.4 \
-lITKIOLSM-4.4 -lITKIOMesh-4.4 -lITKIOMeta-4.4 -lITKIONIFTI-4.4 -lITKIONRRD-4.4 -lITKIOPNG-4.4 -lITKIOSiemens-4.4 -lITKIOSpatialObjects-4.4 \
-lITKIOStimulate-4.4 -lITKIOTIFF-4.4 -lITKIOTransformBase-4.4 -lITKIOTransformHDF5-4.4 -lITKIOTransformInsightLegacy-4.4 -lITKIOTransformMatlab-4.4 \
-lITKIOVTK-4.4 -lITKIOXML-4.4 -lITKKLMRegionGrowing-4.4 -lITKLabelMap-4.4 -lITKMesh-4.4 -lITKMetaIO-4.4 -lITKNrrdIO-4.4 -lITKOptimizers-4.4 \
-lITKOptimizersv4-4.4 -lITKPath-4.4 -lITKPolynomials-4.4 -lITKQuadEdgeMesh-4.4 -lITKReview-4.4 -lITKSpatialObjects-4.4 -lITKStatistics-4.4 \
-lITKVNLInstantiation-4.4 -lITKVTK-4.4 -lITKVideoCore-4.4 -lITKVideoIO-4.4 -lITKWatersheds-4.4 -lITKgiftiio-4.4 -lITKniftiio-4.4 -lITKznz-4.4 \
-litkNetlibSlatec-4.4 -litkhdf5-4.4 -litkhdf5_cpp-4.4 -litkjpeg-4.4 -litkopenjpeg-4.4 -litkpng-4.4 \
-litksys-4.4 -litktiff-4.4 -litkv3p_lsqr-4.4 -litkv3p_netlib-4.4 -litkvcl-4.4 -litkvnl-4.4 -litkvnl_algo-4.4 -litkzlib-4.4 \
-litkgdcmCommon-4.4 -litkgdcmDICT-4.4 -litkgdcmDSED-4.4 -litkgdcmIOD-4.4 -litkgdcmMSFF-4.4 -litkgdcmjpeg12-4.4 \
-litkgdcmjpeg16-4.4 -litkgdcmjpeg8-4.4 -litkgdcmuuid-4.4  -lITKIOGDCM-4.4
}
#===========================================================

macx-g++{
INCLUDEPATH += /usr/local/include/vtk-5.10
LIBS    += -L/usr/local/lib/vtk-5.10 \
-lMapReduceMPI -lQVTK -lmpistubs -lvtkCharts -lvtkCommon -lvtkDICOMParser -lvtkFiltering -lvtkGenericFiltering -lvtkGeovis \
-lvtkGraphics -lvtkHybrid -lvtkIO -lvtkImaging -lvtkInfovis -lvtkNetCDF -lvtkRendering -lvtkViews \
-lvtkVolumeRendering -lvtkWidgets -lvtkalglib -lvtkexoIIc -lvtkexpat -lvtkfreetype -lvtkftgl -lvtkhdf5 -lvtkjpeg -lvtklibxml2 \
-lvtkmetaio -lvtkpng -lvtkproj4 -lvtksqlite -lvtksys -lvtkverdict -lvtkzlib -lvtkQtChart
}

#===========================================================
# Armadillo (http://http://arma.sourceforge.net) - BLAS and LAPACK wrapper
macx-g++{
INCLUDEPATH += /usr/include/armadillo_bits
LIBS += -L/usr/local/lib -O1 -larmadillo
LIBS += -framework Accelerate
}
#===========================================================
# OpenEXR > 1.7.X
macx-g++{
    INCLUDEPATH += /usr/local/include/OpenEXR
    LIBS += -L/usr/local/lib -lHalf -lIex -lIlmImf -lIlmThread -lImath
}

#===========================================================
# QWT library
CONFIG += qwt QwtDll
DEFINES += QWT_DLL
macx-g++{
    INCLUDEPATH += /usr/local/qwt-6.1.0/lib/qwt.framework/Versions/Current/Headers
    INCLUDEPATH += /usr/local/qwt-6.1.0/lib/qwt.framework/Headers
    QMAKE_LFLAGS += -L/usr/local/qwt-6.1.0/lib/6.1.0
    LIBS += -F/usr/local/qwt-6.1.0/lib -framework qwt
}
