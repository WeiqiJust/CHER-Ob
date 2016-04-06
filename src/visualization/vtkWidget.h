/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:   Min H. Kim (minhkim@cs.yale.edu)

 - License:  GNU General Public License Usage
   Alternatively, this file may be used under the terms of the GNU General
   Public License version 3.0 as published by the Free Software Foundation
   and appearing in the file LICENSE.GPL included in the packaging of this
   file. Please review the following information to ensure the GNU General
   Public License version 3.0 requirements will be met:
   http://www.gnu.org/copyleft/gpl.html.

 - Warranty: This software is distributed WITHOUT ANY WARRANTY; without even
   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
   PURPOSE.

 - Acknowledgments: Some portions of this file are based on the example codes
   of ITK/VTK library from Kitware, QT API from Nokia. I would like to thank
   annonymous help by various software engineering communities.

*****************************************************************************/
#ifndef VTKWIDGET_H
#define VTKWIDGET_H

//#include <GL/glew.h>
//#include <QGLContext>
#include <QGLFormat>
//#include <QWidget>
#include <QtGui>
//#include <QtOpenGL>
//#include <QGLWidget>
#include <QDomDocument>
#include <QVTKWidget.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkCubeSource.h>
#include <vtkActor.h>
#include <QVTKInteractorAdapter.h>
#include <vtkImageViewer2.h>
#include <vtkDataSetMapper.h>
#include <vtkMatrix4x4.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkFollower.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPointData.h>
#include <vtkLightActor.h>
#include <vtkTexture.h>
#include <vtkFloatArray.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkSelectionNode.h>
//----------------------------------------------------------
// 2D images (including DICOM CT images) are factorized by using ITK
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageToVTKImageFilter.h>
#include <itkFlipImageFilter.h>
#include <itkOrientImageFilter.h>

#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkImageSeriesReader.h>

#include <itkBMPImageIO.h>
#include <itkMetaImageIO.h>
#include <itkJPEGImageIO.h>
#include <itkPNGImageIO.h>
#include <itkRawImageIO.h>
#include <itkStimulateImageIO.h>
#include <itkTIFFImageIO.h>
#include <itkVTKImageIO.h>
//----------------------------------------------------------
#include "vtkView.h" //MK: The inclusion of this header should be only here, except implementations.
#include "../io/readCHEROb.h"
#include "../information/bookmarkWidget.h"
#include "../information/bookmarkTreeWidget.h"
#include "../information/metadata.h"
#include "../vtkEnums.h"
#include "../information/fileInfoDialog.h"


//-------------------By YY----------------------------------
//  if RTI viewer is supported
#define SUPPORT_RTI


// VCG RTIviewer import
//#ifdef SUPPORT_RTI
#include "../io/util.h"
#include "../io/rti.h"
#include "../function/rtiBrowser.h"
#include "../io/ptm.h"
#include "../io/hsh.h"
#include "../io/universalrti.h"
#include "../io/multiviewrti.h"
//#endif
#include <vector>
//----------------------------------------------------------

#define DEFAULTVALUE1 (50.0) // for dials
#define DEFAULTVALUE2 (50.0) // for dials

#define DEFAULTINTENSITYL1 (0.75) //for GL light
#define DEFAULTINTENSITYL2 (0.25)

#define RENDERING_TIME_INTERVAL (250) // YY. time interval for re-rendering
class vtk2DInteractionCallback;
class vtk3DInteractionCallback;
class ReadCHEROb;
class InputImageSet;
class Bookmark;

// DON'T USE QVTKWIDGET (IT CANNOT RENDER MESH PROPERLY)
//  QVTKWidget // derived from QWidget
class VtkWidget : public QWidget
{
  Q_OBJECT

public:
  // Constructor/Destructor
  VtkWidget(VtkView *mvcont);
  VtkWidget(QWidget *parent = 0);
  ~VtkWidget();
  void initializeMainWindow();

  QVTKWidget* mQVTKWidget;
  QString mFilename;
  QString mProjectPath;
  QString mCHE;
  QString mCHEObject;
  vtkSmartPointer<vtkRenderer> mRenderer;

  int getId() {return id;}
  bool isCurrent() { return mvc()->currentId == this->id;}
  bool ReadData(QString filename);
  bool isDICOM() {return mIsDICOM;}
  bool isRTI() {return mIsRTI;}
  void zoomIn();
  void zoomOut();
  void zoomReset();

  VtkView * mvc();
  MainWindow * mw();

  void displayFileInfo();

  void closeFileInfo();
  /****** Modified by Weiqi Shi 1/26/2016 *******/
  //void displayProjectInfo();
  /****** Modified by Weiqi Shi 1/26/2016 *******/

  void setMeasureDistance(bool useRubberband);
  void setVisibilityDistance(bool removeRubberband);
  void rubberbandStart();
  void rubberbandEnd();

  void screenshot();

  void annotate(bool status, NoteMode noteMode = UNDECLARE);
  void setAnnotationColor(const ColorType color);

  WidgetMode getWidgetMode() {return mWidgetMode;}
  int getSliceMax() {return mSliceMax;}
  bool getIsTextureOn() {return mIsTextureOn;}
  bool getIsInterpolationOn() {return mIsInterpolateOn;}
  int getSliceCurrent() {return mSliceCurrent;}
  bool getUseRubberband() {return mUseRubberband;}
  bool getIsRubberbandVisible() {return mIsRubberbandVisible;}
  CTOrientation getOrientationCurrent() {return mOrientationCurrent;}
  CTVisualization getCTVisualization() {return mCTVisualization;}
  int getBlendType() {return mBlendType;}
  float getReductionFactor() {return mReductionFactor;}
  CTVolumeRenderMode getVolumeRenderMode() {return mVolumeRenderMode;}
  bool getIsDirectionalLight() {return mIsDirectionalLight;}
  bool getDisplayPolyIndicateOn() {return mDisplayPolyIndicateOn;}
  bool getDisplayInfoOn() {return mDisplayInfoOn; }
  bool getUserAnnotationOn() {return mUserIsAnnotating;}
  QString getmRgbTextureFilename() {return mRgbTextureFilename;}

  void setSliceMax(int input) {mSliceMax = input;}
  void setSliceCurrent(int input) {mSliceCurrent = input;}
  void setOrientationCurrent(CTOrientation input) {mOrientationCurrent = input;}
  void setCTVisualization(CTVisualization input) {mCTVisualization = input;}
  void setBlendType(int input) {mBlendType = input;}
  void setReductionFactor(float input) {mReductionFactor = input;}
  void setVolumeRenderMode(CTVolumeRenderMode input) {mVolumeRenderMode = input;}
  void setIsDirectionalLight(bool input);

  void setCameraMode(CameraMode3D cameraMode3d);
  void setTrackballCameraMode();
  void setSurfaceWalkerCameraMode();
  void toggleImageProvenanceFeature();

  void setTextureOn (bool texture) {mIsTextureOn = texture; refresh3D(); }
  void connectSignals();

  void setDisplayInfoOn(bool displayon) {mDisplayInfoOn = displayon; refreshInfo(); }
  void setDisplayPolyIndicateOn(bool input);

  void setUserAnnotationOn(bool annotationon) {mUserIsAnnotating = annotationon; refreshInfo(); }

  void setRenderMode3D(RenderMode3D mode) {mRenderMode3D = mode; refreshRenderMode3D();}
  RenderMode3D getRenderMode3D() {return mRenderMode3D;}
  void setOrthogonalView(OrthogonalView3D view);
  void launchSpinView();

  void setFlattenedMesh(vtkPolyData *flatMesh);
  void setNonFlattenedMesh(vtkPolyData *nonFlatMesh);
  void flattenMesh();

  friend void BookmarkTreeWidget::dropEvent(QDropEvent *event);
  friend bool BookmarkWidget::viewBookmark(QTreeWidgetItem* item);
  friend void BookmarkWidget::refreshCurrentFileInfo();
  friend Metadata::Metadata(QString path, VtkWidget* gla, bool file);
  /****** Modified by Weiqi Shi 1/27/2016 *******/
  friend FileInfoDialog::FileInfoDialog(VtkWidget* gla);
  /****** Modified by Weiqi Shi 1/27/2016 *******/

signals:
  void currentViewerChanged(int currentId);
  void resetLightControl();

  void currentSliceMaxChanged(int sliceMax);
  void currentSliceCurrentChanged(int sliceCurrent);
  void currentSliceOrientationChanged(CTOrientation sliceOrientation);
  void currentSliceVisualizationChanged(CTVisualization ctVisualization);
  void currentVolumeRenderModeChanged(CTVolumeRenderMode volrendermode);
  void currentReductionFactorChanged(float reductionFactor);
  void currentWidgetModeChanged(WidgetMode widgetmode);

  void updateMainWindowMenus(); //updates the menus of the MainWindow

  void currentImageChanged();
  void currentHyperPixelsChanged(std::vector<float> wavelengths, std::vector<float> hyperPixels, const int* icoords, const std::string* fname);

public slots:
  void updateDisplayPanel();
  void updateCurrentSlice(int index);
  void updateCurrentOrientation(CTOrientation ort);
  void updateCurrentVisualization(CTVisualization currentVisualization);
  void rotateCameraViewUpsideDown();
  void updateCurrentReductionFactor(float reductionFactor);
  void updateCurrentBlendType(int blendType);
  void updateCurrentVolumeRenderMode(CTVolumeRenderMode volrendermode);
  void updateLightPosition(vtkTransform * transform);
  void setInterpolateOn(bool isInterpolateOn) {
    this->mIsInterpolateOn = isInterpolateOn;
      switch(mWidgetMode)
      {
      //{EMPTYWIDGET=0, IMAGE2D, MODEL3D, CTSTACK, CTVOLUME, RTI2D};
      default:
      case EMPTYWIDGET:
          break;
      case IMAGE2D:
          refresh2D();
          break;
      case MODEL3D:
          refresh3D();
          break;
      case CTSTACK:
          refresh2D();
          break;
      case CTVOLUME:
          refresh3D();
          break;
      case RTI2D:
          refresh2D();
          break;
      }
  }
  void removePointNoteMark(const int cellId); 
  void removeSurfaceNoteMark(vtkSmartPointer<vtkSelectionNode> cellIds);
  void removeFrustumNoteMark(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals);
 
  void loadPointNoteMark(const int cellId, const ColorType color);
  void loadSurfaceNoteMark(vtkSmartPointer<vtkSelectionNode> cellIds, const ColorType color);
  void loadFrustumNoteMark(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals, const ColorType color);

  void removeSurfaceNote2DMark(double* point);
  void loadSurfaceNote2DMark(double* point, const ColorType color);
  void removePointNote2DMark(double* point);
  void loadPointNote2DMark(double* point, const ColorType color);

protected slots:
  void updateIntensityL12(double intensity1, double intensity2);
  void getHyperPixelsSignals(vtkObject*, unsigned long, void*, void*);
  void saveFileInfo(QWidget* editBox);

protected:
  void copyExistingPointers(VtkView *vcont);
  vtkSmartPointer<vtkEventQtSlotConnect> mEventQtSlot;

  void focusInEvent ( QFocusEvent * event );
  void ImageShifter(vtkImageData *imageData);


private:
  std::vector<float> mHyperPixels;
  std::vector<float> mWavelengths;
  vtkSmartPointer<vtkLightActor> mLightActor;

  // all the data set shoulld be transfered as vtkDataSetMapper
  void Rendering3D();
  void RenderingRTIData(); // by YY
  void refresh3D();
  void refresh2D();
  void refreshInfo();
  void refreshRenderMode3D();
  void drawAxes3D();
  void CreateRGBImage(vtkImageData* image, int width, int height, int channels);
  void RenderingStack(); // radio button selection
  void Rendering2D();
  void RenderingVolume(int blendType = 0, float reductionFactor = 1.0f, CTVolumeRenderMode volumeRenderMode = CPURAYCASTTEXTURE); // default -> cpu rendering

  void refreshGeometry3D();

  int id;  //the very important unique id of each subwindow.

  QImage vtkImageDataToQImage(vtkImageData* imageData);

  void testObject();

  QString getGLversion();
  void printFormat(QGLFormat* format);
  QString mGLversion;
  QString mNumCore;

  void updateSlideMaxOnDocks();
  void resetStackControlOnDocks();

  bool ReadHDRImage(QString filename);
  bool ReadColorImage(QString filename);
  bool ReadDICOM(QString filename);
  bool ReadDICOMDir(QString filename);
#ifdef SUPPORT_RTI
  bool ReadRTI(QString filename);
#endif

  bool Read3DModels(QString filename);

  void debugMatrix(vtkMatrix4x4 *mat);

  // for displaying 2D images with OpenGL
  // vtkImageViewer and vtkImageViewer2 do not work with vtkGDCMImageReader
  vtkSmartPointer<vtkImageViewer2> mVtkImageViewer; // NB: vtkSmartPointer and ITK Pointer will be deleted automatically.
  QVBoxLayout* mLayout;

  //MK: Don't use vtkSmartPointer with Data Arrays -> Crash!!!
  // this member contains pointer to the image data that was read by ITK and VTK
  vtkImageData* mVtkImageData; // RGB for 2D images (including stack)
  vtkImageData* mHyperImageData; // HyperSpectral Data
  vtkPolyData* mVtkPolyData; // for 3D surface polygon data
  vtkTexture* mRgbTexture;
  vtkDataArray* mTCoords;
  vtkDataArray* mNormals;

  // for 2/3D rendering
  QString mTFilename;
  vtkSmartPointer<vtkPolyDataMapper> mMapper;  // The previous version is vtkDataSetMapper. To use hardware selector, switch to vtkPolyDataMapper
  vtkSmartPointer<vtkActor> mActor;
  vtkSmartPointer<vtkCamera> mCamera;
  vtkSmartPointer<vtkLight> mLight1;
  vtkSmartPointer<vtkLight> mLight2;
  WidgetMode mWidgetMode;

  double mInitCamPos[3];
  double mInitCamFoc[3];
  double mInitCamUp[3];

  // for Volume rendering
  vtkSmartPointer<vtkSmartVolumeMapper> mVolMapper;

//  void flipITKtoVTKx (vtkImageData* imageData);
  void flipITKtoVTKy (vtkImageData* imageData);
  void flipITKtoVTKxy (vtkImageData* imageData);

  VtkView *mparentView;

  int muWidth,muHeight;

  bool mIsHyperspectral;
  bool mIsDICOM;
  bool mIsRTI;
  bool mIsTextureOn;
  bool mDisplayInfoOn;
  bool mDisplayPolyIndicateOn;
  bool mUseRubberband;
  bool mIsRubberbandVisible;
  bool mIsDirectionalLight;
  bool mUserIsAnnotating;

  QComboBox* monthCombo;
  QSpinBox* daySpin;

  //-----------------------------------------------------------
  // MK: Note that ITK image data maintains the input data in the reading module.
  //     We have to keep the original data as a member
  // NB: vtkSmartPointer and ITK Pointer will be deleted automatically.
  //-----------------------------------------------------------
  // DICOM Single Image Reading Members
  //-----------------------------------------------------------
  // ITK: image data structure configuration (Black/White)
  typedef itk::Image<float, 2>                  mBImageType; // DICOM should be float
  typedef itk::ImageFileReader<mBImageType>       mBReaderType;
  typedef itk::ImageToVTKImageFilter<mBImageType> mBFilterType;

  // ITK file IO and ITK-VTK connector
  mBReaderType::Pointer     mBreader;
  mBFilterType::Pointer     mBconnector;
  //-----------------------------------------------------------
  // DICOM Directory Reading Members
  //-----------------------------------------------------------
  typedef float   mB3PixelType;
  typedef itk::Image< mB3PixelType, 3 >  mB3ImageType;
  typedef itk::ImageSeriesReader< mB3ImageType > mB3ReaderType;
  typedef itk::ImageToVTKImageFilter<mB3ImageType> mB3FilterType;
  typedef itk::GDCMImageIO                  mB3ImageIOType;
  typedef itk::GDCMSeriesFileNames          mB3InputNamesGeneratorType;
  //-----------------------------------------------------------
  mB3ReaderType::Pointer     mB3reader;
  mB3FilterType::Pointer     mB3connector;
  mB3ImageIOType::Pointer    mGDCMIO;
  mB3InputNamesGeneratorType::Pointer inputNames;
  //-----------------------------------------------------------
  // A Color Image Reading Members
  //-----------------------------------------------------------
  // ITK: image data structure configuration
  typedef itk::RGBPixel< unsigned char >        mCPixelType;
  typedef itk::Image< mCPixelType, 2 >            mCImageType;
  typedef itk::ImageFileReader<mCImageType>       mCReaderType;
  typedef itk::ImageToVTKImageFilter<mCImageType> mCFilterType;

  // ITK file IO and ITK-VTK connector
  mCReaderType::Pointer     mCreader;
  mCFilterType::Pointer     mCconnector;
  //-----------------------------------------------------------

  int mSliceMax;
  int mSliceCurrent;
  CTOrientation mOrientationCurrent;
  CTVisualization mCTVisualization;
  int mBlendType;
  float mReductionFactor;
  CTVolumeRenderMode mVolumeRenderMode;
  double mLitVec[3];

  vtkSmartPointer<vtk2DInteractionCallback> mCallback2D;
  vtkSmartPointer<vtk3DInteractionCallback> mCallback3D;

  double mIntensityL1, mIntensityL2;
  void setOpenGL32Lion();

  int mNumberOfPoints;
  int mNumberOfPolys;
  int mNumberOfStrips;
  int mNumberOfLines;
  int mNumberOfVerts;
  int mNumberOfCells;

  QString mRgbTextureFilename;
  std::vector<std::string> mChannelNames;
  QGLFormat format;

  std::vector<Material> mMaterials;

  bool mIsInterpolateOn;
  RenderMode3D mRenderMode3D;
  CameraMode3D mCameraMode3D;
  CameraMode3D mLastCameraMode3D;  // used for area pick mode switch
  InputImageSet *inputImageSet;
  NoteMode mNoteMode;
  ColorType mAnnotationColor;
  /****** Modified by Weiqi Shi 1/27/2016 *******/
  FileInfoDialog *mFileInfoDialog;
  /****** Modified by Weiqi Shi 1/27/2016 *******/

  QDomDocument annotationsXml;

  // for RTI
private:
   void ConvertUnsignedCharVecToVTKImageData(int iwidth, int iheight, std::vector<unsigned char> textureData); // assign the textureData blow to mVtkImageData for visualiztion

public:
   int getRerenderingTimeInterval() {return RENDERING_TIME_INTERVAL;};

public slots:
	void updateRTIImageVTK(std::vector<unsigned char> textureData, int textureWidth, int textureHeight, bool FIRST_RTI_RENDERING);

public:
	 RtiBrowser* mRTIbrowser; /*!< Browser for RTI image. */
};

#endif // VTKWIDGET_H
