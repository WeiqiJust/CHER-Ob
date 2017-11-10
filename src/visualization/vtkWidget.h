/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Min H. Kim (minhkim@cs.yale.edu)
			 Weiqi Shi (weiqi.shi@yale.edu)
			 Zeyu Wang (zeyu.wang@yale.edu)

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
  QString mCHE;	// The path of original CHE
  QString mCHEObject;	// The object name without path
  vtkSmartPointer<vtkRenderer> mRenderer;

  int getId() {return id;}
  bool isCurrent() { return mvc()->currentId == this->id;}
  bool ReadData(QString filename);
  bool isDICOM() {return mIsDICOM;}
  bool isRTI() {return mIsRTI;}
  void zoomIn();
  void zoomOut();
  void zoomReset();
  void changeBGColor();

  VtkView * mvc();
  MainWindow * mw();

  void displayFileInfo();

  void closeFileInfo();

  void setMeasureDistance(bool useRubberband);
  void setVisibilityDistance(bool removeRubberband);
  void rubberbandStart();
  void rubberbandEnd();

  QString screenshot(QString location = QString());

  void annotate(bool status, NoteMode noteMode = UNDECLARE, bool visibilityOn = true);
  void setAnnotationColor(const ColorType color);

  WidgetMode getWidgetMode() const {return mWidgetMode;}
  int getSliceMax() const {return mSliceMax;}
  bool getIsTextureOn() const {return mIsTextureOn;}
  bool getIsInterpolationOn() const {return mIsInterpolateOn;}
  int getSliceCurrent() const {return mSliceCurrent;}
  bool getUseRubberband() const {return mUseRubberband;}
  bool getIsRubberbandVisible() const {return mIsRubberbandVisible;}
  CTOrientation getOrientationCurrent() const {return mOrientationCurrent;}
  CTVisualization getCTVisualization() const {return mCTVisualization;}
  int getBlendType() const {return mBlendType;}
  float getReductionFactor() const {return mReductionFactor;}
  CTVolumeRenderMode getVolumeRenderMode() const {return mVolumeRenderMode;}
  bool getIsDirectionalLight() const {return mIsDirectionalLight;}
  bool getDisplayPolyIndicateOn() const {return mDisplayPolyIndicateOn;}
  bool getDisplayInfoOn() const {return mDisplayInfoOn; }
  bool getUserAnnotationOn() const {return mUserIsAnnotating;}
  QString getmRgbTextureFilename() const {return mRgbTextureFilename;}

  NoteMode getNoteMode2D() const;
  NoteMode getNoteMode3D() const;
  //// Modified by Zeyu Wang on Nov 4, 2016 to make note mode consistent when change windows

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
  RenderMode3D getRenderMode3D() const {return mRenderMode3D;}
  void setOrthogonalView(OrthogonalView3D view);
  // video generator for 3D objects
  void setArbitraryView(double angle);
  void getCameraPos(double *campos);
  void setCameraPos(double *campos);
  void computeNormals3D();
  void setPointNoteView(int polygonID, double x, double y, double z, int dolly);
  void setSurfaceNoteView(int polygonID, double x, double y, double z, int dolly);
  void setFrustumNoteView(double angle, double x, double y, double z, int dolly);
  void launchSpinView();

  void setFlattenedMesh(vtkPolyData *flatMesh);
  void setNonFlattenedMesh(vtkPolyData *nonFlatMesh);
  void flattenMesh();

  vtkSmartPointer<vtkPolyData> get3DPolyData()	const {return mVtkPolyData;}

  double get2DImageHeight();
  double get2DImageWidth();
  QPixmap getRTIScreenShot()	{return mRTIScreenShot;}

  friend void BookmarkTreeWidget::dropEvent(QDropEvent *event);
  friend bool BookmarkWidget::viewBookmark(QTreeWidgetItem* item, QString objectPath);
  friend void BookmarkWidget::refreshCurrentFileInfo();
  friend Metadata::Metadata(QString path, VtkWidget* gla, bool file);
  friend FileInfoDialog::FileInfoDialog(VtkWidget* gla);

signals:
  void currentViewerChanged(int currentId);
  void resetLightControl();

  void currentSliceMaxChanged(int sliceMax);
  void currentSliceCurrentChanged(int sliceCurrent);
  void currentSliceOrientationChanged(CTOrientation sliceOrientation);
  void currentSliceVisualizationChanged(CTVisualization ctVisualization);
  void currentVolumeRenderModeChanged(CTVolumeRenderMode volrendermode);
  void currentReductionFactorChanged(float reductionFactor);
  void currentWidgetModeChanged(WidgetMode widgetmode, bool isDirectionalLight);

  void updateMainWindowMenus(); //updates the menus of the MainWindow

  void currentImageChanged();
  void currentHyperPixelsChanged(std::vector<float> wavelengths, std::vector<float> hyperPixels, const int* icoords, const double* dcoords, const std::string* fname);

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
  void removeSurfaceNoteMark(vtkSmartPointer<vtkSelectionNode> cellIds, QVector<double*> cornerPoints, bool isCTVolume);
  void removeFrustumNoteMark(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals);

  void openPointNoteMark(const int cellId);
  void openSurfaceNoteMark(vtkSmartPointer<vtkSelectionNode> cellIds, QVector<double*> cornerPoints, bool isCTVolume);
  void openFrustumNoteMark(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals);
 
  void loadPointNoteMark(const int cellId, const ColorType color, const double* position, bool isDisplay = false);
  void loadSurfaceNoteMark(vtkSmartPointer<vtkSelectionNode> cellIds, QVector<double*> points, const ColorType color, bool isDisplay = false);
  void loadFrustumNoteMark(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals, const ColorType color, bool isDisplay = false);

  void removeSurfaceNote2DMark(double* point);
  void openSurfaceNote2DMark(double* point);
  void loadSurfaceNote2DMark(double* point, const ColorType color, bool isDisplay = false);
  void removePointNote2DMark(double* point);
  void openPointNote2DMark(double* point);
  void loadPointNote2DMark(double* point, const ColorType color, bool isDisplay = false);
  void removePolygonNote2DMark(std::vector<std::pair<double, double> >* polygon);
  void openPolygonNote2DMark(std::vector<std::pair<double, double> >* polygon);
  void loadPolygonNote2DMark(std::vector<std::pair<double, double> >* polygon, const ColorType color, bool isDisplay = false);

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
  vtkSmartPointer<vtkPolyData> tmpPolyData; // a deep copy of mVtkPolyData to compute normals

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

  double mInitCTCamPos[3];
  double mInitCTCamFoc[3];
  double mInitCTCamUp[3];

  // for Volume rendering
  vtkSmartPointer<vtkSmartVolumeMapper> mVolMapper;

//  void flipITKtoVTKx (vtkImageData* imageData);
  void flipITKtoVTKy (vtkImageData* imageData);
  void flipITKtoVTKxy (vtkImageData* imageData);

  VtkView *mparentView;

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
  FileInfoDialog *mFileInfoDialog;
  QPixmap mRTIScreenShot;

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
