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
#include "../mainWindow.h"
#include "vtkWidget.h"
//----------------------------------------------------------
#include <omp.h>
#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkVectorText.h>
#include <vtkImageViewer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include "vtkInteractorStyleSurfaceWalkerCamera.h"
#include <vtkCamera.h>
#include <vtkTestUtilities.h>
#include <vtkSmartPointer.h>
#include <vtkRandomGraphSource.h>
#include <vtkGraphLayoutView.h>
#include <vtkImageFlip.h>
#include <vtkImageReslice.h>
#include <vtkMarchingCubes.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkStringArray.h>
#include <vtkImageChangeInformation.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkActor2DCollection.h>
#include <vtkImageResample.h>
#include <vtkVolumeProperty.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkBoxWidget.h>
#include <vtkCommand.h>
#include <vtkPlanes.h>
#include <QVTKInteractor.h>
#include <vtkProperty.h>
#include <vtkTexturedSphereSource.h>
#include <vtkGraphicsFactory.h>
#include <vtkMath.h>
#include <vtkTransform.h>
#include <vtkInformation.h>
#include <vtkProperty.h>
#include <vtkImageShiftScale.h>
#include <vtkGPUVolumeRayCastMapper.h> // GPU volume rendering
#include <vtkSmartVolumeMapper.h> // GPU volume rendering
#include <vtkFixedPointVolumeRayCastMapper.h> // CPU rendering
#include <vtkOutputWindow.h>
#include <vtkFileOutputWindow.h>

#include <vtkDoubleArray.h>
#include <vtkCellData.h>

#include <vtkWindowToImageFilter.h> // Screenshot
#include <vtkPNGWriter.h> // Screenshot
#include <vtkJPEGWriter.h> // Screenshot
#include <vtkBMPWriter.h> // Screenshot

//----------------------------------------------------------
#include <vtkObjectFactory.h>
#include <vtkPNGReader.h>
#include <vtkTIFFReader.h>
#include <vtkBMPReader.h>
#include <vtkJPEGReader.h>
#include <vtkDICOMImageReader.h>
//----------------------------------------------------------
// 3D images are factorized by using VTK
#include <vtkVRMLImporter.h>  // tcoords, normals
#include <vtkPLYReader.h>     // geometry
#include <vtkOBJReader.h>     // tcoords, normals
#include <vtk3DSImporter.h>
#include <vtkSTLReader.h>
//----------------------------------------------------------
// vtk texture mapping
#include <vtkTexture.h>
//----------------------------------------------------------
// this is for user interaction programming
#include <vtkCellPicker.h>
#include <vtkActor2D.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkCallbackCommand.h>
#include <vtkSelectionNode.h>
#include <vtkExtractSelection.h>
#include <vtkIdTypeArray.h>
#include <vtkSelection.h>
#include <vtkExtractSelection.h>
#include <vtkUnstructuredGrid.h>
#include <vtkRendererCollection.h>
#include <vtkAssembly.h>
#include <vtkAssemblyPath.h>
#include <vtkAxesActor.h>
#include <vtkVectorText.h>
#include <vtkConeSource.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkPropPicker.h>
#include <vtkImageActor.h>
#include <vtkInteractorStyleImage.h>

#include "vtk2DInteractionCallback.hpp" // 2D mouse interaction program
#include "vtk3DInteractionCallback.hpp" // 3D mouse interaction program

//----------------------------------------------------------
#include "../function/CTControl.h"
#include "../function/lightControl.h"
#include "../function/plotView.h"
#include "../information/bookmarkWidget.h"
#include "../function/mkTools.hpp"
#include "../io/readCHEROb.h"
#include "../io/inputimageset.h"
#include "../function/lightControlRTI.h" 
#include "../function/renderingdialog.h"
#include "myVTKInteractorStyle.h"

#include <QDebug>

//#include "windows.h"

//----------------------------------------------------------
//#define VTK_CREATE(type, name) vtkSmartPointer<type> name = vtkSmartPointer<type>::New()
#define MAX_PICKCELLARRAY (1)
#define LABELGAPX (5)
#define INFOPANELX (20)
#define INFOPANELY (20)

#define SUPPORT_VRML 100
#define SUPPORT_OBJ  101
#define SUPPORT_PLY  102
#define SUPPORT_3DS  103
#define SUPPORT_STL  104
//#define INFOARRAYSIZE 1000

#define POINTSSIZE (4)
#define LINEWIDTH (2)

// Constructor
VtkWidget::VtkWidget(QWidget * parent)
  :  QWidget()
{
  this->setParent(parent);
  initializeMainWindow();
}

// Multi view constructor
VtkWidget::VtkWidget(VtkView *mvcont)
  :  QWidget()
{
  this->setParent(mvcont);
  id = mvcont->getNextViewerId();
  mFileInfoDialog = NULL;

  if (id > 0)
  {
    copyExistingPointers(mvcont);
  }
  else {
    initializeMainWindow();
  }
}

// NB: vtkSmartPointer and ITK Pointer will be deleted automatically when out-of-scope.
VtkWidget::~VtkWidget(){
  // Widgets
  mQVTKWidget = NULL;
  mLayout = NULL;

  // Data Array => Crash
  //  if (mVtkPolyData) mVtkPolyData->Delete();
  //  if (mRgbTexture) mRgbTexture->Delete();
  //  if (mHyperImageData) mHyperImageData->Delete();

  mWavelengths.clear();
  mHyperPixels.clear();
  mw()->mBookmark->refreshBookmarkList(); // DT: refresh list of bookmarks
  emit currentHyperPixelsChanged(mWavelengths, mHyperPixels, NULL, NULL);
  if(!mw()->VTKA()) emit resetLightControl();

  resetStackControlOnDocks(); // update control CT panels
}

NoteMode VtkWidget::getNoteMode2D() const { return mCallback2D->GetNoteMode(); }
NoteMode VtkWidget::getNoteMode3D() const { return mCallback3D->GetNoteMode(); } 

void VtkWidget::updateCurrentSlice(int index)
{
  if (mIsDICOM){
    //int* Size = mVtkImageViewer->GetInput()->GetDimensions();
    index = (index <= 0 ) ? 0 : index;
    index = (index > mSliceMax) ? mSliceMax : index;

    mSliceCurrent = index;

    if (mVtkImageViewer) {
      mVtkImageViewer->SetSlice(mSliceCurrent);
      mVtkImageViewer->Render();
    }

    // show the rendering window
    if (mQVTKWidget) mQVTKWidget->show();
    if (mQVTKWidget) mQVTKWidget->update(); //MK: this is important!
  }
}


void VtkWidget::updateIntensityL12(double intensity1, double intensity2)
{
//  mIntensityL1 = intensity1;
//  mIntensityL2 = intensity2

  mIntensityL1 = DEFAULTINTENSITYL1 * intensity1;
  mIntensityL2 = DEFAULTINTENSITYL2 * intensity1 / intensity2;

  mLight1->SetIntensity(mIntensityL1);
  mLight2->SetIntensity(mIntensityL2);

  if ( mCallback2D && (mIsDICOM == true) )
  {
    mCallback2D->updateLightingPosition();
    mCallback2D->SetWindowLevels(intensity1, intensity2);
  } else if ( mCallback2D && (mIsDICOM == false) ){
    mCallback2D->SetWindowLevels();
  }
  if (mCallback3D)
  {
    mCallback3D->updateLightingPosition();
  }

  if(mQVTKWidget)  mQVTKWidget->show();
  if(mQVTKWidget)  mQVTKWidget->update();
}


void VtkWidget::updateRTIImageVTK(std::vector<unsigned char> textureData, int textureWidth, int textureHeight, bool FIRST_RTI_RENDERING)
{
	/********************************************/
	//mw()->activateTabWidgetTop(static_cast<int>(LightControlType::RTILIGHTCONTROL));

	// copy texture data into VTK
	if (FIRST_RTI_RENDERING) {

		// YY: activate the correct light control tab in MainWindow
        mw()->activateTabWidgetTop(static_cast<int>(RTILIGHTCONTROL));
		QEventLoop loop;
		QTimer::singleShot(0.01, &loop, SLOT(quit()));
		loop.exec();

		mCTVisualization = STACK;
		mVtkImageData = vtkImageData::New();
		mHyperImageData = vtkImageData::New(); // just space holder for 2D RGB
		ConvertUnsignedCharVecToVTKImageData(textureWidth, textureHeight, textureData);
		RenderingRTIData();

	} else {
		ConvertUnsignedCharVecToVTKImageData(textureWidth, textureHeight, textureData); // copy texture data into VTK
		flipITKtoVTKy(mVtkImageData);
	}
	/********************************************/

	if(mQVTKWidget)  mQVTKWidget->show();
	if(mQVTKWidget)  mQVTKWidget->update();

	QEventLoop loop;
	QTimer::singleShot(0.01, &loop, SLOT(quit()));
	loop.exec();
}

void VtkWidget::updateLightPosition(vtkTransform * transform)
{
  if (mCallback2D)
  {
    mCallback2D->SetLightTransform(transform);
	qDebug()<<"after light transform";
    mCallback2D->updateLightingPosition();
	qDebug()<<"after light transform";
  }
qDebug()<<"in updatelight position";
  if (mCallback3D)
  {
	  qDebug()<<"before activate";
    mw()->activateTabWidgetTop(static_cast<int>(Model3DLIGHTCONTROL));
	qDebug()<<"before set transform";
    mCallback3D->SetLightTransform(transform);
	qDebug()<<"after set transform";
    mCallback3D->updateLightingPosition();
	qDebug()<<"before leight position";
  }

  if(mQVTKWidget)  mQVTKWidget->show();
  qDebug()<<"after shown";
  if(mQVTKWidget)  mQVTKWidget->update();
   qDebug()<<"after update";

}

void VtkWidget::rotateCameraViewUpsideDown()
{
    if (mVtkImageViewer != NULL) {

      // Connect the renderer to the window
      mVtkImageViewer->SetRenderWindow(mQVTKWidget->GetRenderWindow());

      // Flip the ITK input in the VTK camera
      vtkSmartPointer<vtkRenderer> renderer = mVtkImageViewer->GetRenderer();

      // Change camera upside-down
      vtkSmartPointer<vtkCamera> vCamera = renderer->GetActiveCamera();

      // Change up vector
      double vup[3];
      vCamera->GetViewUp(vup);
      vCamera->SetViewUp(-1.*vup[0], -1.*vup[1], -1.*vup[2]);

      renderer->SetActiveCamera(vCamera);
      // renderer->ResetCamera();  // this screws up the camera (let's not use it)

      mVtkImageViewer->Render();

      // Show the rendering window
      if(mQVTKWidget)  mQVTKWidget->show();
      if(mQVTKWidget)  mQVTKWidget->update(); // MK: this is important!
    }
}

void VtkWidget::updateCurrentVisualization(CTVisualization currentVisualization)
{
  if (mIsDICOM){
    mCTVisualization = currentVisualization;
    RenderingStack();
  }
}

void VtkWidget::updateCurrentBlendType(int blendType)
{
  if (mIsDICOM){
    mBlendType = blendType;
//    qDebug() << "blend-type mode: " << mBlendType;
    RenderingStack();
  }
}


void VtkWidget::updateCurrentVolumeRenderMode(CTVolumeRenderMode volrendermode)
{
  if (mIsDICOM){
    mVolumeRenderMode = volrendermode;
//    qDebug() << "volume render mode: " << mVolumeRenderMode;
    RenderingStack();
  }
}


void VtkWidget::updateCurrentReductionFactor(float reductionFactor)
{
  if (mIsDICOM){
    mReductionFactor = reductionFactor;
    this->RenderingStack();
  }
}

void VtkWidget::updateCurrentOrientation(CTOrientation ort)
{
  if (mIsDICOM){
    mOrientationCurrent = ort;
    mVtkImageViewer->SetRenderWindow(mQVTKWidget->GetRenderWindow());

    int* Size = mVtkImageViewer->GetInput()->GetDimensions();
//    qDebug() << "Dimensions of the picture as read with ITK: " << Size[0] << " x " << Size[1] << " x " << Size[2];

    // change orientation
    switch(mOrientationCurrent)
    {
    default:
    case FRONTCT:
      mVtkImageViewer->SetSliceOrientationToXY();
      mSliceMax = Size[2];
      break;
    case TOPCT:
      mVtkImageViewer->SetSliceOrientationToXZ();
      mSliceMax = Size[1];
      break;
    case SIDECT:
      mVtkImageViewer->SetSliceOrientationToYZ();
      mSliceMax = Size[0];
      break;
    }
    //reset the current slide number -> Safer!!! -> fixed
	//mSliceCurrent = 0;
	//mVtkImageViewer->SetSlice(mSliceCurrent);

    updateSlideMaxOnDocks(); // since slicemax is changed by the orientation.

    if (mVtkImageViewer) 
	{
		mVtkImageViewer->UpdateDisplayExtent(); // update ImageViewer2 for new orientation.
		mVtkImageViewer->Render();
	}

    // show the rendering window
    if (mQVTKWidget)
	{
		mQVTKWidget->show();
		mQVTKWidget->update(); //MK: this is important!
	}
  }
}

void VtkWidget::connectSignals()
{
  if (mw())
  {
  // (1) build connections from the CTController to the VtkWidget
  connect(mw()->mCtControl, SIGNAL(sendSlideCurrent(int)), this, SLOT(updateCurrentSlice(int)) );
  connect(mw()->mCtControl, SIGNAL(sendVisualizationChanged(CTVisualization)), this, SLOT(updateCurrentVisualization(CTVisualization)) );
  connect(mw()->mCtControl, SIGNAL(sendOrientationChanged(CTOrientation)), this, SLOT(updateCurrentOrientation(CTOrientation)) );
  connect(mw()->mCtControl, SIGNAL(sendUpDownView()), this, SLOT(rotateCameraViewUpsideDown()) );
  connect(mw()->mCtControl, SIGNAL(sendReductionFactorChanged(float)), this, SLOT(updateCurrentReductionFactor(float)));
  connect(mw()->mCtControl, SIGNAL(sendBlendTypeChanged(int)), this, SLOT(updateCurrentBlendType(int)));
  connect(mw()->mCtControl, SIGNAL(sendVolumeRenderModeChanged(CTVolumeRenderMode)), this, SLOT( updateCurrentVolumeRenderMode(CTVolumeRenderMode)) );
  // connect VTK to Qt
  //connect(mw()->mLightControlRTI, SIGNAL(lightChanged(vcg::Point3f, bool)), this, SLOT(updateRTILightPosition(vcg::Point3f, bool))); //by YY
  //connect(mw()->mLightControl, SIGNAL(signalLightPositionChanged(double, double, double)), this, SLOT(updateLightPosition(double, double, double)) ); //by YY
  connect(mw()->mLightControl, SIGNAL(signalLitCamPosChanged(vtkTransform *)), this, SLOT(updateLightPosition(vtkTransform *)) );
  connect(mw()->mLightControl, SIGNAL(signalIntensityL12(double, double)), this, SLOT(updateIntensityL12(double, double)) );
  // below crash
  //  connect(mw()->mLightControl, SIGNAL(signalLitCamPosChanged(vtkTransform *)), this, SLOT(updateDisplayPanel()) );

  // (2) build connections from the VtkWidget to the CTController
  connect(this, SIGNAL(currentSliceMaxChanged(int)), mw()->mCtControl, SLOT(updateSliceMax(int)));
  connect(this, SIGNAL(currentSliceCurrentChanged(int)), mw()->mCtControl, SLOT(updateSliceCurrent(int)));
  connect(this, SIGNAL(currentWidgetModeChanged(WidgetMode, bool)), mw()->mCtControl, SLOT( updateCtControlWidgetMode(WidgetMode, bool) ) );
  connect(this, SIGNAL(currentSliceOrientationChanged(CTOrientation)), mw()->mCtControl, SLOT(updateSliceOrientation(CTOrientation)));
  connect(this, SIGNAL(currentSliceVisualizationChanged(CTVisualization)), mw()->mCtControl, SLOT(updateSliceVisualization(CTVisualization)));
  connect(this, SIGNAL(currentReductionFactorChanged(float)), mw()->mCtControl, SLOT(updateReductionFactor(float)) );
  connect(this, SIGNAL(currentVolumeRenderModeChanged(CTVolumeRenderMode)), mw()->mCtControl, SLOT(updateVolumeRenderMode(CTVolumeRenderMode)) );
  connect(this, SIGNAL(currentImageChanged()), mw()->mLightControl, SLOT( updateLightingVector() ) ); // update light of the current object by taking the light coordinate
  connect(this, SIGNAL(currentWidgetModeChanged(WidgetMode, bool)), mw()->mLightControl, SLOT( updateLightControl(WidgetMode, bool) ) );
  connect(this, SIGNAL(resetLightControl()), mw()->mLightControl, SLOT( reset() ) );
  connect(this, SIGNAL(currentHyperPixelsChanged(std::vector<float>, std::vector<float>, const int*, const std::string*)), mw()->mPlotView, SLOT(updateSpectralPlot(std::vector<float>, std::vector<float>, const int*, const std::string*)) );

  }
}

void VtkWidget::copyExistingPointers(VtkView *vcont)
{
  // for 3D rendering
  mIsTextureOn = vcont->getViewer(0)->mIsTextureOn;
  mIsDICOM = vcont->getViewer(0)->mIsDICOM;
  mDisplayInfoOn = vcont->getViewer(0)->mDisplayInfoOn;

  mVtkPolyData = vcont->getViewer(0)->mVtkPolyData;
  mRgbTexture = vcont->getViewer(0)->mRgbTexture;
  mHyperImageData = vcont->getViewer(0)->mHyperImageData;
  mMaterials.resize(vcont->getViewer(0)->mMaterials.size()); // resize vector
  std::copy(vcont->getViewer(0)->mMaterials.begin(),vcont->getViewer(0)->mMaterials.end(),mMaterials.begin()); // deep copy
  mChannelNames.resize(vcont->getViewer(0)->mChannelNames.size());
  std::copy(vcont->getViewer(0)->mChannelNames.begin(),vcont->getViewer(0)->mChannelNames.end(),mChannelNames.begin()); // deep copy
  mWavelengths.resize(vcont->getViewer(0)->mWavelengths.size());
  std::copy(vcont->getViewer(0)->mWavelengths.begin(),vcont->getViewer(0)->mWavelengths.end(),mWavelengths.begin()); // deep copy

  Rendering3D();
}

void VtkWidget::setAnnotationColor(const ColorType color)
{
	mAnnotationColor = color;
	if (mCallback3D) 
	{
		mCallback3D->SetAnnotationColor(color);
	}
	if (mCallback2D) 
	{
		mCallback2D->SetAnnotationColor(color);
	}
}

void VtkWidget::annotate(bool status, NoteMode noteMode, bool visibilityOn)
{
    mUserIsAnnotating = status;
	if (noteMode != UNDECLARE)
		mNoteMode = noteMode;
    if (mCallback2D) mCallback2D->SetAnnotation(mUserIsAnnotating, mNoteMode, mAnnotationColor, visibilityOn);
    if (mCallback3D) 
	{
		mCallback3D->SetAnnotation(mUserIsAnnotating, mNoteMode, mAnnotationColor, visibilityOn);
		if (!status)
		{
			setCameraMode(mLastCameraMode3D);
		}
		else
		{
			mLastCameraMode3D = mCameraMode3D;
			mCameraMode3D = RUBBERBANDPICKMODE;
		}
	}
	mQVTKWidget->update();
    update();
	if (mWidgetMode == CTSTACK) // In order to show notes correctly.
		updateCurrentOrientation(mOrientationCurrent);
}

void VtkWidget::removePointNoteMark(const int cellId)
{
	mCallback3D->removePointNoteMark(cellId);
}

void VtkWidget::removeSurfaceNoteMark(vtkSmartPointer<vtkSelectionNode> cellIds, QVector<double*> cornerPoints, bool isCTVolume)
{
	mCallback3D->removeSurfaceNoteMark(cellIds, cornerPoints.toStdVector(), isCTVolume);
}

void VtkWidget::removeFrustumNoteMark(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals)
{
	mCallback3D->removeFrustumNoteMark(points, normals);
}

void VtkWidget::openPointNoteMark(const int cellId)
{
	mCallback3D->openPointNoteMark(cellId);
}

void VtkWidget::openSurfaceNoteMark(vtkSmartPointer<vtkSelectionNode> cellIds, QVector<double*> cornerPoints, bool isCTVolume)
{
	mCallback3D->openSurfaceNoteMark(cellIds, cornerPoints.toStdVector(), isCTVolume);
}

void VtkWidget::openFrustumNoteMark(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals)
{
	mCallback3D->openFrustumNoteMark(points, normals);
}

void VtkWidget::loadPointNoteMark(const int cellId, const ColorType color, const double* position, bool isDisplay)
{
	mCallback3D->displayLoadPointNote(cellId, color, position, isDisplay);
}

void VtkWidget::loadSurfaceNoteMark(vtkSmartPointer<vtkSelectionNode> cellIds, QVector<double*> points, const ColorType color, bool isDisplay)
{
	mCallback3D->displayLoadSurfaceNote(cellIds, points, color, isDisplay);
}

void VtkWidget::loadFrustumNoteMark(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals, const ColorType color, bool isDisplay)
{
	mCallback3D->displayLoadFrustumNote(points, normals, color, isDisplay);
}

void VtkWidget::removePointNote2DMark(double* point)
{
	mCallback2D->removePointNoteMark(point);
}

void VtkWidget::removeSurfaceNote2DMark(double* point)
{
	mCallback2D->removeSurfaceNoteMark(point);
}

void VtkWidget::removePolygonNote2DMark(std::vector<std::pair<double, double> >* polygon)
{
	mCallback2D->removePolygonNoteMark(polygon);
	
}

void VtkWidget::openPointNote2DMark(double* point)
{
	mCallback2D->openPointNoteMark(point);
}

void VtkWidget::openSurfaceNote2DMark(double* point)
{
	mCallback2D->openSurfaceNoteMark(point);
}

void VtkWidget::openPolygonNote2DMark(std::vector<std::pair<double, double> >* polygon)
{
	mCallback2D->openPolygonNoteMark(polygon);
	
}

void VtkWidget::loadPointNote2DMark(double* point, const ColorType color, bool isDisplay)
{
	mCallback2D->displayLoadPointNote(point, color, isDisplay);
}

void VtkWidget::loadSurfaceNote2DMark(double* point, const ColorType color, bool isDisplay)
{
	mCallback2D->displayLoadSurfaceNote(point, color, isDisplay);
}

void VtkWidget::loadPolygonNote2DMark(std::vector<std::pair<double, double> >* polygon, const ColorType color, bool isDisplay)
{
	mCallback2D->displayLoadPolygonNote(polygon, color, isDisplay);
	
}

void VtkWidget::setMeasureDistance(bool status)
{
  mUseRubberband = status;
  if (mCallback3D) mCallback3D->SetUseRubberband(mUseRubberband);
  update();
}

void VtkWidget::setVisibilityDistance(bool status)
{
  mIsRubberbandVisible = status;
  if (mCallback3D) mCallback3D->SetVisibilityRubberband(mIsRubberbandVisible);

  if (mQVTKWidget) mQVTKWidget->update(); // DT: user doesn't have to mouse over VTK window to update

  update();
}


void VtkWidget::setOrthogonalView(OrthogonalView3D view)
{
  if (mRenderer == NULL)
    return;
  // move camera to the initial position
  vtkSmartPointer<vtkCamera> camera = mRenderer->GetActiveCamera();
  if (mWidgetMode == CTVOLUME)
  {
	  camera->SetPosition(mInitCTCamPos);
	  camera->SetFocalPoint(mInitCTCamFoc);
	  camera->SetViewUp(mInitCTCamUp);
  }
  else
  {
	  camera->SetPosition(mInitCamPos);
	  camera->SetFocalPoint(mInitCamFoc);
	  camera->SetViewUp(mInitCamUp);
  }
  mRenderer->ResetCamera();

  switch(view)
  {
  default:
  case FRONT3D:
    break;
  case LEFT3D:
    camera->Azimuth(-90);
    break;
  case RIGHT3D:
    camera->Azimuth(90);
    break;
  case TOP3D:
    camera->Elevation(90);
    double vup[3];
    camera->GetViewUp(vup);
    camera->SetViewUp(-1.*vup[0], -1.*vup[1], -1.*vup[2]);
//    camera->OrthogonalizeViewUp(); // Don't use this (bug)
    break;
  case BOTTOM3D:
    camera->Elevation(-90);
    break;
  case BACK3D:
    camera->Azimuth(180);
    break;
  }// end of switch

  mRenderer->ResetCamera();
  mRenderer->ResetCameraClippingRange();
  mRenderer->Modified();

  // move light according to light
  mCallback3D->updateLightingPosition();

  mLight1->Modified();
  mLight2->Modified();

//  mQVTKWidget->show(); // no Render() is required for 3D
  if(mQVTKWidget) mQVTKWidget->update(); //MK: this is important!
}

void VtkWidget::setArbitraryView(double angle)
{
	if (mRenderer == NULL) return;
	// move camera to the initial position
	vtkSmartPointer<vtkCamera> camera = mRenderer->GetActiveCamera();
	if (mWidgetMode == CTVOLUME)
	{
		camera->SetPosition(mInitCTCamPos);
		camera->SetFocalPoint(mInitCTCamFoc);
		camera->SetViewUp(mInitCTCamUp);
	}
	else
	{
		camera->SetPosition(mInitCamPos);
		camera->SetFocalPoint(mInitCamFoc);
		camera->SetViewUp(mInitCamUp);
	}
	mRenderer->ResetCamera();

	// set azimuth to circle around the vertical axis of the object
	camera->Azimuth(angle);

	mRenderer->ResetCamera();
	mRenderer->ResetCameraClippingRange();
	mRenderer->Modified();

	// move light according to light
	mCallback3D->updateLightingPosition();

	mLight1->Modified();
	mLight2->Modified();

	// mQVTKWidget->show(); // no Render() is required for 3D
	if (mQVTKWidget) mQVTKWidget->update(); //MK: this is important!
}

void VtkWidget::getCameraPos(double *campos)
{
	if (mRenderer == NULL) return;
	vtkSmartPointer<vtkCamera> camera = mRenderer->GetActiveCamera();
	camera->GetPosition(campos[0], campos[1], campos[2]);
	camera->GetFocalPoint(campos[3], campos[4], campos[5]);
}

void VtkWidget::setCameraPos(double *campos)
{
	if (mRenderer == NULL) return;
	vtkSmartPointer<vtkCamera> camera = mRenderer->GetActiveCamera();
	camera->SetPosition(campos[0], campos[1], campos[2]);
	camera->SetFocalPoint(campos[3], campos[4], campos[5]);
	
	mRenderer->ResetCameraClippingRange();
	mRenderer->Modified();

	// move light according to light
	mCallback3D->updateLightingPosition();

	mLight1->Modified();
	mLight2->Modified();

	// mQVTKWidget->show(); // no Render() is required for 3D
	if (mQVTKWidget) mQVTKWidget->update(); //MK: this is important!
}

void VtkWidget::setPointNoteView(int polygonID, double x, double y, double z, int dolly)
{
	if (mRenderer == NULL) return;
	// move camera to the initial position
	vtkSmartPointer<vtkCamera> camera = mRenderer->GetActiveCamera();
	if (mNormals) {
		double normalsXYZ[3];
		mNormals->GetTuple(polygonID, normalsXYZ);
		camera->SetPosition(x + dolly*normalsXYZ[0], y + dolly*normalsXYZ[1], z + dolly*normalsXYZ[2]);
	}
	camera->SetFocalPoint(x, y, z);
	//mRenderer->ResetCamera(); // this line makes SetPosition() and SetFocalPoint() invalid
	mRenderer->ResetCameraClippingRange();
	mRenderer->Modified();

	// move light according to light
	mCallback3D->updateLightingPosition();

	mLight1->Modified();
	mLight2->Modified();

	// mQVTKWidget->show(); // no Render() is required for 3D
	if (mQVTKWidget) mQVTKWidget->update(); //MK: this is important!
}

void VtkWidget::setSurfaceNoteView(int polygonID, double x, double y, double z, int dolly)
{
	if (mRenderer == NULL) return;
	// move camera to the initial position
	vtkSmartPointer<vtkCamera> camera = mRenderer->GetActiveCamera();
	if (mNormals) {
		double normalsXYZ[3];
		mNormals->GetTuple(polygonID, normalsXYZ);
		camera->SetPosition(x + dolly*normalsXYZ[0], y + dolly*normalsXYZ[1], z + dolly*normalsXYZ[2]);
	}
	camera->SetFocalPoint(x, y, z);
	//mRenderer->ResetCamera(); // this line makes SetPosition() and SetFocalPoint() invalid
	mRenderer->ResetCameraClippingRange();
	mRenderer->Modified();

	// move light according to light
	mCallback3D->updateLightingPosition();

	mLight1->Modified();
	mLight2->Modified();

	// mQVTKWidget->show(); // no Render() is required for 3D
	if (mQVTKWidget) mQVTKWidget->update(); //MK: this is important!
}

void VtkWidget::setFrustumNoteView(double angle, double x, double y, double z, int dolly)
{
	if (mRenderer == NULL) return;
	// move camera to the initial position
	vtkSmartPointer<vtkCamera> camera = mRenderer->GetActiveCamera();
	if (mWidgetMode == CTVOLUME)
	{
		camera->SetPosition(mInitCTCamPos);
		camera->SetFocalPoint(mInitCTCamFoc);
		camera->SetViewUp(mInitCTCamUp);
	}
	else
	{
		camera->SetPosition(mInitCamPos);
		camera->SetFocalPoint(mInitCamFoc);
		camera->SetViewUp(mInitCamUp);
	}
	mRenderer->ResetCamera();
	camera->Azimuth(angle);
	camera->SetFocalPoint(x, y, z);
	camera->Dolly(dolly);
	mRenderer->ResetCameraClippingRange();
	mRenderer->Modified();

	// move light according to light
	mCallback3D->updateLightingPosition();

	mLight1->Modified();
	mLight2->Modified();

	// mQVTKWidget->show(); // no Render() is required for 3D
	if (mQVTKWidget) mQVTKWidget->update(); //MK: this is important!
}


void VtkWidget::launchSpinView()
{
  if (mRenderer == NULL)
    return;

  // move camera to the initial position
  vtkSmartPointer<vtkCamera> camera = mRenderer->GetActiveCamera();
  camera->SetPosition(mInitCamPos);
  camera->SetFocalPoint(mInitCamFoc);
  camera->SetViewUp(mInitCamUp);
  mRenderer->ResetCamera();

#ifdef __APPLE__
  int ms = 100;
  struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
#endif

  for (int i=0; i<360; i++)
  {
    // move light according to light
    mCallback3D->setCameraAzimuth( (double)i );

    mQVTKWidget->show();
    mQVTKWidget->update(); // no Render() is required for 3D
#ifdef __APPLE__
    nanosleep(&ts, NULL);
#endif
  }

}


void VtkWidget::initializeMainWindow()
{
  // data array
    mVtkImageData = NULL; // RGB for 2D images (including stack)
    mHyperImageData = NULL; // HyperSpectral Data
    mVtkPolyData = NULL; // for 3D surface polygon data
    mRgbTexture = NULL;
    mTCoords = NULL;
    mNormals = NULL;

  mDisplayInfoOn = true;
  mDisplayPolyIndicateOn = false;
  mIsDirectionalLight = true;
  mUseRubberband = false;
  mIsRubberbandVisible = false;
  mUserIsAnnotating = false;
  mWidgetMode = EMPTYWIDGET;
  mQVTKWidget = NULL;
  mLayout = NULL;
  mRenderMode3D = SURFACE3D;
  mCameraMode3D = TRACKBALLMODE;
  mLastCameraMode3D = TRACKBALLMODE;
  mNoteMode = POINTNOTE;
  mAnnotationColor = YELLOW;

//  mRenderMode = DMFlat;
  setAttribute(Qt::WA_DeleteOnClose);

  mNumberOfPoints = 0;
  mNumberOfPolys = 0;
  mNumberOfStrips = 0;
  mNumberOfLines = 0;
  mNumberOfVerts = 0;
  mNumberOfCells = 0;

//#if defined(_WIN32) || defined(_WIN64)
//  // remove VTK error window
//  vtkSmartPointer<vtkFileOutputWindow> w = vtkSmartPointer<vtkFileOutputWindow>::New();
//  w->SetFileName("vtk_errors.txt");
//  vtkOutputWindow::SetInstance(w);
//  w->Delete(); // now SetInstance owns the reference
//#endif

  //================================================================
  // creating connections with controllers.
  // update vtk widget from dock widget (top, front, side)
  //connecting the MainWindow Slots to GLArea signal (simple passthrough)
  if(mw()){
    connect(this,SIGNAL(updateMainWindowMenus()),mw(),SLOT(updateMenus()));
  }else{
    qDebug("The parent of the VtkWidet parent is not a pointer to the MainWindow.");
    return;
  }

  if (mw())
  {
    mSliceCurrent = mw()->mCtControl->GetSliceCurrent();
    mSliceMax = mw()->mCtControl->GetSliceMax();
    mOrientationCurrent = mw()->mCtControl->GetOrientationCurrent();
    mCTVisualization = mw()->mCtControl->GetCTVisualization();
    mBlendType = mw()->mCtControl->GetBlendType();

    mReductionFactor = mw()->mCtControl->GetReductionFactor();
    mVolumeRenderMode = mw()->mCtControl->GetVolumeRenderMode();

    emit currentSliceMaxChanged(mSliceMax);// update dock objects from vtk widget

    double intensity1 = mw()->mLightControl->GetIntensityL1();
    double intensity2 = mw()->mLightControl->GetIntensityL2();
    mIntensityL1 = DEFAULTINTENSITYL1 * intensity1;
    mIntensityL2 = DEFAULTINTENSITYL2 * intensity1 / intensity2;

    connectSignals();
  }
  //================================================================
  // initializing member variables
  // pointer initialization
  mVtkImageViewer = NULL;

  // ITK file IO and ITK-VTK connector
  mBreader = NULL;
  mBconnector = NULL;
  mB3reader = NULL;
  mB3connector = NULL;
  mGDCMIO = NULL;
  inputNames = NULL;
  mCreader = NULL;
  mCconnector = NULL;
  mIsRTI = false;
  mIsDICOM = false;
  mIsTextureOn = true;
  mIsInterpolateOn = false;
  mDisplayInfoOn = true;
  mIsHyperspectral = false;
  //================================================================
  int width = this->frameGeometry().width();
  int height = this->frameGeometry().height();

  this->setMinimumHeight(height/2);
  this->setMinimumWidth(width/2);

  mGLversion = getGLversion();
  char name[256]; memset( name, 0, sizeof(name) );
  sprintf(name, "    Number of Cores: %d",  omp_get_num_procs() );
  QString ncore(name);
  mNumCore = ncore;

  annotationsXml;

  mRTIbrowser = NULL;

}

// under test
void VtkWidget::setOpenGL32Lion() {
  QGLFormat fmt;
  fmt.setProfile(QGLFormat::CoreProfile);
  fmt.setVersion(3,2);
  fmt.setSampleBuffers(true);
  fmt.setSamples(4);
  QGLFormat::setDefaultFormat(fmt);
}

QString VtkWidget::getGLversion() {
  char text[256]; memset( text, 0, sizeof(text) );
  if (format.hasOpenGL()) {
//    qDebug() <<"Version " << glGetString(GL_VERSION) <<"\n";
//    qDebug() <<"GLSL "<< glGetString(GL_SHADING_LANGUAGE_VERSION) <<"\n";

    format.setProfile(QGLFormat::CoreProfile);

      //set open gl version to max available, they have to be in order from smallest to biggest

      if (format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_ES_CommonLite_Version_1_0) ||
              format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_ES_Common_Version_1_0) ) {
          format.setVersion(1,0);
      }
      if (format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_1_1) ||
              format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_ES_CommonLite_Version_1_1) ||
              format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_ES_Common_Version_1_1)
              ) {
          format.setVersion(1,1);
      }
      if (format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_1_2)
              ) {
          format.setVersion(1,2);
      }

      if (format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_1_3)
              ) {
          format.setVersion(1,3);
      }

      if (format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_1_4)
              ) {
          format.setVersion(1,4);
      }

      if (format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_1_5)
              ) {
          format.setVersion(1,5);
      }

      if (format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_2_0) ||
              format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_ES_Version_2_0)
              ) {
          format.setVersion(2,0);
      }

      if (format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_2_1)
              ) {
          format.setVersion(2,1);
      }

      if (format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_3_0)
              ) {
          format.setVersion(3,0);
      }

      if (format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_3_1)
              ) {
          format.setVersion(3,1);
      }

      if (format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_3_2)
              ) {
          format.setVersion(3,2);
      }

      if (format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_3_3)
              ) {
          format.setVersion(3,3);
      }

      if (format.openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_4_0)
              ) {
          format.setVersion(4,0);
      }

      sprintf( text, "Supported OpenGL: %i.%i\n", format.majorVersion(), format.minorVersion());
  } else {
      sprintf( text, "OpenGL not supported\n");
  }

  QString outputstr(text);
//  qDebug() << outputstr;
  printFormat(&format);

  return outputstr;

}

void VtkWidget::printFormat(QGLFormat* format)
{
  qDebug("\n *** Format Information ***\n");
  qDebug("Supports OpenGL: ");
  format->hasOpenGL() ? qDebug("Yes\n") : qDebug("No\n");


  qDebug("Double Buffering: ");
  format->doubleBuffer() ? qDebug("Enable\n") : qDebug("Disable\n");


  qDebug("Depth: ");
  format->depth() ? qDebug("Enable\n") : qDebug("Disable\n");


  qDebug("RGBA: ");
  format->rgba() ? qDebug("Enable\n") : qDebug("Disable\n");


  qDebug("ALPHA: ");
  format->alpha() ? qDebug("Enable\n") : qDebug("Disable\n");


  qDebug("Accumulation Buffer: ");
  format->accum() ? qDebug("Enable\n") : qDebug("Disable\n");


  qDebug("Stencil: ");
  format->stencil() ? qDebug("Enable\n") : qDebug("Disable\n");


  qDebug("Stereo: ");
  format->stereo() ? qDebug("Enable\n") : qDebug("Disable\n");


  qDebug("Direct ing: ");
  format->directRendering() ? qDebug("Enable\n") : qDebug("Disable\n");


  qDebug("Overlay Plane: ");
  format->hasOverlay() ? qDebug("Enable\n") : qDebug("Disable\n");


  qDebug("Planes: %d\n", format->plane());
}


void VtkWidget::updateSlideMaxOnDocks()
{
  if(mVtkImageViewer && mIsDICOM)
  {
    // connect the renderer to the window
//    mVtkImageViewer->SetRenderWindow(this->GetRenderWindow());

    int* Size = mVtkImageViewer->GetInput()->GetDimensions();
//    qDebug() << "Dimensions of the picture as read with ITK: " << Size[0] << " x " << Size[1] << " x " << Size[2];

    // update max slices
    mSliceMax = mVtkImageViewer->GetSliceMax();

    // update dock objects from vtk widget
    emit currentSliceMaxChanged(mSliceMax);
  }
}

void VtkWidget::resetStackControlOnDocks()
{
  if(mVtkImageViewer && mIsDICOM)
  {
    // initialize the viewing parameters
    mSliceCurrent = 0;
    mOrientationCurrent = FRONTCT;
    mCTVisualization = STACK;

    // connect the renderer to the window
//    mVtkImageViewer->SetRenderWindow(this->GetRenderWindow());

    // update max slices
    mSliceMax = mVtkImageViewer->GetSliceMax();

#ifdef _DEBUG
//    qDebug() << "mSliceCurrent: " << mSliceCurrent;
//    qDebug() << "mSliceMax: " << mSliceMax;
//    qDebug() << "mOrientationCurrent: " << mOrientationCurrent;
//    qDebug() << "mCTVisualization: " << mCTVisualization;
//    qDebug() << "mVolumeRenderMode: " << mVolumeRenderMode;
//    qDebug() << "mReductionFactor: " << mReductionFactor;
#endif

    // update dock objects from vtk widget
    emit currentSliceCurrentChanged(mSliceCurrent);
    emit currentSliceMaxChanged(mSliceMax);
    emit currentSliceOrientationChanged(mOrientationCurrent);
    emit currentSliceVisualizationChanged(mCTVisualization);
    emit currentReductionFactorChanged(mReductionFactor);
    emit currentVolumeRenderModeChanged(mVolumeRenderMode);

  }
}

VtkView * VtkWidget::mvc() // current VtkView
{
  QObject * curParent = this->parent();
  while(qobject_cast<VtkView *>(curParent) == 0)
  {
    curParent = curParent->parent();
  }
  return qobject_cast<VtkView *>(curParent);
}

MainWindow * VtkWidget::mw() // current MainWindow
{
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
    if (mainwindow)
    {
      return mainwindow;
    }
  }
}

void VtkWidget::drawAxes3D()
{
  vtkSmartPointer<vtkConeSource> coneSource = vtkSmartPointer<vtkConeSource>::New();
  coneSource->Update();

  vtkPolyData* cone = coneSource->GetOutput();

  //create a mapper
  vtkSmartPointer<vtkPolyDataMapper> coneMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  coneMapper->SetInputConnection(coneSource->GetOutputPort());

   // create an actor
   vtkSmartPointer<vtkActor> coneActor = vtkSmartPointer<vtkActor>::New();
   coneActor->SetMapper(coneMapper);
   coneActor->GetProperty()->SetColor(1,1,0);

   // a renderer and render window
   vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
   mQVTKWidget->GetRenderWindow()->AddRenderer(renderer);

   // an interactor
   vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
   renderWindowInteractor->SetRenderWindow(mQVTKWidget->GetRenderWindow());

   // add the actors to the scene
   renderer->AddActor(coneActor);
   renderer->SetBackground(0.5,0.5,0.5);

   // vtkAxesActor is currently not designed to work with vtkInteractorStyleTrackballActor
   // since it is a hybrid object containing both vtkProp3D's and vtkActor2D's, the latter
   // of which does not have a 3D position that can be manipulated
   vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();

   // get a copy of the axes' constituent 3D actors and put them into a vtkAssembly
   // so they can be manipulated as one prop
   vtkSmartPointer<vtkPropCollection> collection = vtkSmartPointer<vtkPropCollection>::New();
   axes->GetActors(collection);

   collection->InitTraversal();

   vtkSmartPointer<vtkAssembly> movableAxes = vtkSmartPointer<vtkAssembly>::New();

   for( int i = 0; i < collection->GetNumberOfItems(); ++i )
     {
     movableAxes->AddPart((vtkProp3D*)collection->GetNextProp());
     }

   renderer->AddActor(movableAxes);

   // create our own labels that will follow and face the camera
   vtkSmartPointer<vtkFollower> xLabel =
     vtkSmartPointer<vtkFollower>::New();
   vtkSmartPointer<vtkVectorText> xText =
     vtkSmartPointer<vtkVectorText>::New();
   vtkSmartPointer<vtkPolyDataMapper> xTextMapper =
     vtkSmartPointer<vtkPolyDataMapper>::New();
   xText->SetText( "X" );
   xTextMapper->SetInputConnection( xText->GetOutputPort() );
   xLabel->SetMapper( xTextMapper );
   xLabel->SetScale( 0.3 );
   xLabel->SetCamera(renderer->GetActiveCamera());
   xLabel->SetPosition(((vtkProp3D*)collection->GetItemAsObject( 3 ))->GetCenter()); // XAxisTip
   xLabel->PickableOff();
   renderer->AddActor( xLabel );

   vtkSmartPointer<vtkFollower> yLabel =
     vtkSmartPointer<vtkFollower>::New();
   vtkSmartPointer<vtkVectorText> yText =
     vtkSmartPointer<vtkVectorText>::New();
   vtkSmartPointer<vtkPolyDataMapper> yTextMapper =
     vtkSmartPointer<vtkPolyDataMapper>::New();
   yText->SetText( "Y" );
   yTextMapper->SetInputConnection( yText->GetOutputPort() );
   yLabel->SetMapper( yTextMapper );
   yLabel->SetScale( 0.3 );
   yLabel->SetCamera(renderer->GetActiveCamera());
   yLabel->SetPosition(((vtkProp3D*)collection->GetItemAsObject( 4 ))->GetCenter()); // YAxisTip
   yLabel->PickableOff();
   renderer->AddActor( yLabel );

   vtkSmartPointer<vtkFollower> zLabel =
     vtkSmartPointer<vtkFollower>::New();
   vtkSmartPointer<vtkVectorText> zText =
     vtkSmartPointer<vtkVectorText>::New();
   vtkSmartPointer<vtkPolyDataMapper> zTextMapper =
     vtkSmartPointer<vtkPolyDataMapper>::New();
   zText->SetText( "Z" );
   zTextMapper->SetInputConnection( zText->GetOutputPort() );
   zLabel->SetMapper( zTextMapper );
   zLabel->SetScale( 0.3 );
   zLabel->SetCamera(renderer->GetActiveCamera());
   zLabel->SetPosition(((vtkProp3D*)collection->GetItemAsObject( 5 ))->GetCenter()); // ZAxisTip
   zLabel->PickableOff();
   renderer->AddActor( zLabel );

   // custom callback to set the positions of the labels
   vtk3DInteractionCallback* callback = vtk3DInteractionCallback::New();
   callback->XLabel = xLabel;
   callback->YLabel = yLabel;
   callback->ZLabel = zLabel;
   callback->Axes = movableAxes;

   renderer->ResetCamera();

   vtkSmartPointer<vtkInteractorStyleTrackballActor> style =
       vtkSmartPointer<vtkInteractorStyleTrackballActor>::New();

   renderWindowInteractor->SetInteractorStyle( style );
   style->AddObserver( vtkCommand::InteractionEvent, callback ); // all interaction

   // begin mouse interaction
   renderWindowInteractor->Initialize();
   callback->Delete();

   mQVTKWidget->update();
}

void VtkWidget::refreshInfo()
{
  if (mCallback3D)
    mCallback3D->SetDisplayInfoOn(mDisplayInfoOn);
  if (mCallback2D)
    mCallback2D->SetDisplayInfoOn(mDisplayInfoOn);

  if (mQVTKWidget) mQVTKWidget->show(); // no Render() is required for 3D
  if (mQVTKWidget) mQVTKWidget->update(); //MK: this is important!
}

void VtkWidget::refreshRenderMode3D()
{
  if (mRenderMode3D == POINTS3D) {
    mActor->GetProperty()->SetRepresentationToPoints();
    mActor->GetProperty()->SetPointSize(POINTSSIZE);
    mActor->GetProperty()->LightingOff();
    mActor->Modified();
  }
  else if (mRenderMode3D == WIREFRAME3D) {
    mActor->GetProperty()->SetRepresentationToWireframe();
    mActor->GetProperty()->SetLineWidth(LINEWIDTH);
    mActor->GetProperty()->LightingOff();
    mActor->Modified();
  }
  else if (mRenderMode3D == SURFACE3D) {
    mActor->GetProperty()->SetRepresentationToSurface();
    mActor->GetProperty()->LightingOn();
    mActor->Modified();
  }
  else {
    mActor->GetProperty()->SetRepresentationToSurface();
    mActor->GetProperty()->LightingOn();
    mActor->Modified();
  }

  if (mQVTKWidget) mQVTKWidget->show(); // no Render() is required for 3D
  if(mQVTKWidget) mQVTKWidget->update(); //MK: this is important!
}

void VtkWidget::refresh3D()
{
  if (mMapper) mMapper->Modified();
  if (mActor) {
    if (this->mIsInterpolateOn) {
      if (mRgbTexture)
          mRgbTexture->InterpolateOn();
    } else {
      if (mRgbTexture)
          mRgbTexture->InterpolateOff();
    }
    mRgbTexture->Modified();

    if (mIsTextureOn)
      mActor->SetTexture(mRgbTexture);
    else
      mActor->SetTexture(NULL);
    mActor->Modified();

    // if texture coordinates are wrong
    if (mVtkPolyData->GetPointData()) {
        if (mVtkPolyData->GetPointData()->GetTCoords()) {
         int numC = mVtkPolyData->GetPointData()->GetTCoords()->GetNumberOfComponents();
         int numT = mVtkPolyData->GetPointData()->GetTCoords()->GetNumberOfTuples();

         if ((numC != 2) || !(numT > 0))
             mActor->SetTexture(NULL);
        }
    }
  }
  if (mLight1) mLight1->Modified();
  if (mLight2) mLight2->Modified();

  if (mRenderer) mRenderer->Modified();

  emit currentImageChanged(); // update lighting

  if (mQVTKWidget) mQVTKWidget->show(); // no Render() is required for 3D
  if (mQVTKWidget) mQVTKWidget->update(); //MK: this is important!
}

void VtkWidget::refresh2D()
{
  if (mVtkImageViewer != NULL) {
    // Give the picker a prop to pick
    vtkSmartPointer<vtkImageActor> imageActor = mVtkImageViewer->GetImageActor();
    // disable interpolation, so we can see each pixel
    if (this->mIsInterpolateOn)
      imageActor->InterpolateOn();
    else
      imageActor->InterpolateOff();
    imageActor->Modified();

    if (mIsDICOM == false)
        mCallback2D->SetWindowLevels();


    // adding a light
    mRenderer = mVtkImageViewer->GetRenderer(); // get the renderer
    mRenderer->Modified();

    if (mQVTKWidget) mQVTKWidget->show(); // no Render() is required for 3D
    if (mQVTKWidget) mQVTKWidget->update(); //MK: this is important!
  }
}

void VtkWidget::setIsDirectionalLight(bool input)
{
  mIsDirectionalLight = input;
  if (mCallback3D != NULL)
    mCallback3D->SetIsDirectLight( input );
  refreshGeometry3D();
}

void VtkWidget::setDisplayPolyIndicateOn(bool input)
{
  mDisplayPolyIndicateOn = input;
  if (mCallback3D != NULL)
    mCallback3D->setDisplayPolyIndicateOn( input );
}

void VtkWidget::setCameraMode(CameraMode3D cameraMode3d)
{
    switch(cameraMode3d)
    {
    case TRACKBALLMODE:
        setTrackballCameraMode();
        break;
    case SURFACEWALKERMODE:
        setSurfaceWalkerCameraMode();
        break;
    default:
        setTrackballCameraMode();
        break;
    }
}

void VtkWidget::setTrackballCameraMode()
{
    if(mCameraMode3D == TRACKBALLMODE)
        return;

    mCameraMode3D = TRACKBALLMODE;
	mLastCameraMode3D = TRACKBALLMODE;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    mCallback3D->GetInteractor()->SetInteractorStyle(style);

    emit currentImageChanged();
    mQVTKWidget->update();
}

void VtkWidget::setSurfaceWalkerCameraMode()
{
    if(mCameraMode3D == SURFACEWALKERMODE)
        return;

    mCameraMode3D = SURFACEWALKERMODE;
	mLastCameraMode3D = SURFACEWALKERMODE;
    vtkSmartPointer<vtkInteractorStyleSurfaceWalkerCamera> style = vtkSmartPointer<vtkInteractorStyleSurfaceWalkerCamera>::New();
    mCallback3D->GetInteractor()->SetInteractorStyle(style);
    style->SetDefaultRenderer(mRenderer);
    style->SetCurrentRenderer(mRenderer);
    style->linkVtkWidget(this);
    style->linkPolyData(mVtkPolyData, mCamera);
/*
    style->AddObserver(vtkCommand::MouseMoveEvent, mCallback3D);
    style->AddObserver(vtkCommand::LeftButtonPressEvent, mCallback3D);
    style->AddObserver(vtkCommand::LeftButtonReleaseEvent, mCallback3D);
    style->AddObserver(vtkCommand::MiddleButtonPressEvent, mCallback3D);
    style->AddObserver(vtkCommand::MiddleButtonReleaseEvent, mCallback3D);
    style->AddObserver(vtkCommand::RightButtonPressEvent, mCallback3D);
    style->AddObserver(vtkCommand::RightButtonReleaseEvent, mCallback3D);
    style->AddObserver(vtkCommand::MouseWheelForwardEvent, mCallback3D);
    style->AddObserver(vtkCommand::MouseWheelBackwardEvent, mCallback3D);
	style->AddObserver(vtkCommand::EndPickEvent, mCallback3D);*/
    emit currentImageChanged();
    mQVTKWidget->update();
}

void VtkWidget::toggleImageProvenanceFeature()
{
    mCallback3D->toggleImageProvenanceFeature();
}

void VtkWidget::computeNormals3D()
{
	tmpPolyData = vtkSmartPointer<vtkPolyData>::New();
	tmpPolyData->DeepCopy(mVtkPolyData);
	// qDebug() << "Looking for cell normals..." << "\n";
	// Count cells
	// vtkIdType numCells = tmpPolyData->GetNumberOfCells();
	// qDebug() << "There are " << numCells << " cells." << "\n";
	// Count triangles
	// vtkIdType numPolys = tmpPolyData->GetNumberOfPolys();
	// qDebug() << "There are " << numPolys << " polys." << "\n";
	mNormals = tmpPolyData->GetCellData()->GetNormals();
	if (mNormals) {
		// qDebug() << "There are " << mNormals->GetNumberOfTuples() << " normals in mNormals" << "\n";
		// double testDouble[3];
		// mNormals->GetTuple(0, testDouble);
		// qDebug() << "Double: " << testDouble[0] << " " << testDouble[1] << " " << testDouble[2] << "\n";
	} else {
		// qDebug() << "No cell normals were found. Computing normals...\n";
		vtkSmartPointer<vtkPolyDataNormals> normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();
#if VTK_MAJOR_VERSION <= 5
		normalGenerator->SetInput(tmpPolyData);
#else
		normalGenerator->SetInputData(tmpPolyData);
#endif
		normalGenerator->ComputePointNormalsOff();
		normalGenerator->ComputeCellNormalsOn();
		normalGenerator->Update();
		tmpPolyData = normalGenerator->GetOutput();
		// Try to read normals again
		mNormals = tmpPolyData->GetCellData()->GetNormals();
	}
	if (mNormals) {
		// qDebug() << "There are " << mNormals->GetNumberOfTuples() << " normals in mNormals" << "\n";
		// double testDouble[3];
		// mNormals->GetTuple(0, testDouble);
		// qDebug() << "Double: " << testDouble[0] << " " << testDouble[1] << " " << testDouble[2] << "\n";
	} else {
		qDebug() << "Normals not found! Normals computation error! \n";
	}
}


void VtkWidget::Rendering3D()
{
    mw()->activateTabWidgetTop(static_cast<int>(Model3DLIGHTCONTROL));
	QEventLoop loop;
	QTimer::singleShot(0.01, &loop, SLOT(quit()));
	loop.exec();

    mWidgetMode = MODEL3D;
    emit currentWidgetModeChanged(mWidgetMode, getIsDirectionalLight());

  if (mQVTKWidget != NULL)
    delete mQVTKWidget;

  mQVTKWidget = new QVTKWidget(this);

  if (mLayout!=NULL)
    delete mLayout;
  mLayout = new QVBoxLayout( this ); // size stretching trick
  mLayout->setMargin( 0 );
  mLayout->addWidget( mQVTKWidget, 1 );
  this->setLayout(mLayout);

//  if (mVtkPolyData->GetPointData()) {
//      if (mVtkPolyData->GetPointData()->GetTCoords()) {
//          qDebug() << "number of components (2) " << mVtkPolyData->GetPointData()->GetTCoords()->GetNumberOfComponents();
//          qDebug() << "number of texture coordinates (2) " << mVtkPolyData->GetPointData()->GetTCoords()->GetNumberOfTuples();
//      }
//  }

  // Mapper: Move the data from VTK into OpenGL
  //mMapper = vtkSmartPointer<vtkDataSetMapper>::New();
  mMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
//  mMapper->SetInput(mVtkPolyData);
//  mMapper->Update(); // vtkStreamingDemandDrivenPipeline (0x110e451f0): Input for connection index 0 on input port index 0 for algorithm vtkDataSetMapper(0x10d420a10) is NULL, but a vtkDataSet is required.
  mMapper->SetInputConnection(mVtkPolyData->GetProducerPort()); // this is new version (on July 16)
  mMapper->ScalarVisibilityOff();


//    #define _DEBUG // this is OK
//    #ifdef _DEBUG
//    vtkSmartPointer<vtkImageData> outputrgb = vtkImageData::SafeDownCast(mRgbTexture->GetInput());
//    mkDebug md;  md.qDebugImageData(outputrgb); // fine
//    //  vtkSmartPointer<vtkImageData> outputhyper = vtkImageData::SafeDownCast(mHyperImageData);
//    //      mkDebug md;  md.qDebugImageData(outputrgb, outputhyper); // fine
//    //imagedata1:  512   512   1 :  3
//    //imagedata1 (10,10,0,1):  19
//    #endif
//------------------------------------------------------------------------------------------
  // Material Property Control
  // Actor: For setting colors, surface properties, and the position of the object
  mActor = vtkSmartPointer<vtkActor>::New();
  mActor->SetMapper(mMapper);
  //VTK Shading option.
  mActor->GetProperty()->SetInterpolationToFlat();
//  mActor->GetProperty()->SetInterpolationToGouraud();
//  mActor->GetProperty()->SetInterpolationToPhong();

  mNumberOfPoints = mVtkPolyData->GetNumberOfPoints();
  mNumberOfPolys = mVtkPolyData->GetNumberOfPolys();
  mNumberOfStrips = mVtkPolyData->GetNumberOfStrips();
  mNumberOfLines = mVtkPolyData->GetNumberOfLines();
  mNumberOfVerts = mVtkPolyData->GetNumberOfVerts();
  mNumberOfCells = mVtkPolyData->GetNumberOfCells();

  // texture rendering option
  // disable interpolation, so we can see each pixel

  if (this->mIsInterpolateOn)
	  if (mRgbTexture) mRgbTexture->InterpolateOn();
	  else if (mRgbTexture) mRgbTexture->InterpolateOff();

  //mRgbTexture->RepeatOff(); // by default repeat is on

  if (!mMaterials.empty() && mMaterials[0].fileType != EMPTYFILE) {
	  int i=0;
	  std::string texscfn = mMaterials[i].textureFilename;
	  QString texfn = texscfn.c_str();
	  //qDebug() << "texture filename: " << texfn;

	  vtkSmartPointer<vtkProperty> property = vtkSmartPointer<vtkProperty>::New();
	  property = mActor->GetProperty();
	  double ambient[3];    ambient[0] = mMaterials[i].Ka[0];     ambient[1] = mMaterials[i].Ka[1];     ambient[2] = mMaterials[i].Ka[2];
	  property->SetAmbientColor(ambient); // for surface
	  double diffuse[3];    diffuse[0] = mMaterials[i].Kd[0];     diffuse[1] = mMaterials[i].Kd[1];     diffuse[2] = mMaterials[i].Kd[2];
	  property->SetDiffuseColor(diffuse); // for surface

	  double specular[3];    specular[0] = mMaterials[i].Ks[0];     specular[1] = mMaterials[i].Ks[1];     specular[2] = mMaterials[i].Ks[2];
	  property->SetSpecularColor(specular); // for surface
  } // if there is materials, otherwise
  else {
	  vtkSmartPointer<vtkProperty> property = vtkSmartPointer<vtkProperty>::New();
	  property = mActor->GetProperty();
	  double ambient[3] = {0.2, 0.2, 0.2};
	  property->SetAmbientColor(ambient); // for surface
	  double diffuse[3] = {1., 1., 1.};
	  property->SetDiffuseColor(diffuse); // for surface
	  double specular[3] = {1., 1., 1.};
	  property->SetSpecularColor(specular); // for surface
  }// end of if

  //=============================================================================
  // This part cause a problem in the deploy mode.
  //texture
  if (mIsTextureOn && !mRgbTextureFilename.isEmpty())
  { qDebug() << "rendering mRgbTextureFilename = " << mRgbTextureFilename;
  mActor->SetTexture(mRgbTexture);
  }
  else
	  mActor->SetTexture(NULL);

  // if texture coordinates are wrong
  if (mVtkPolyData->GetPointData()) {
	  if (mVtkPolyData->GetPointData()->GetTCoords()) {
		  int numC = mVtkPolyData->GetPointData()->GetTCoords()->GetNumberOfComponents();
		  int numT = mVtkPolyData->GetPointData()->GetTCoords()->GetNumberOfTuples();

		  if ((numC != 2) || !(numT > 0))
			  mActor->SetTexture(NULL);
	  }
  }

  //=============================================================================

    setRenderMode3D(mRenderMode3D);
//  qDebug() << "mRenderMode3D " << mRenderMode3D;
  //------------------------------------------------------------------------------------------

  // VTK/Qt wedded
  // Renderer: The rectangle of the computer screen that VTK draws into
  mRenderer = vtkSmartPointer<vtkRenderer>::New();
  mActor->PickableOn();
  mRenderer->AddActor(mActor);// Add actor to renderer

//  vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
//  double* world = coordinate->GetComputedWorldValue(mRenderer);
//  qDebug() << "World coordinate: " << world[0] << ", " << world[1] << ", " << world[2] ;

  // Window: The window, including title bar and decorations
  mQVTKWidget->GetRenderWindow()->AddRenderer(mRenderer);

  // get the camera
  mCamera = mRenderer->GetActiveCamera();

  mCamera->GetPosition(mInitCamPos);
  mCamera->GetFocalPoint(mInitCamFoc);

  //  Y vector for camera
  mCamera->GetViewUp(mInitCamUp);
  mCamera->SetParallelProjection(FALSE); // set the camera perspective projection

  // adding a light
  mLight1 = vtkSmartPointer<vtkLight>::New();
  mLight1->SetPosition(mInitCamPos);
  mLight1->SetFocalPoint(mInitCamFoc);
  mLight1->SetPositional(0);
  mLight1->SetIntensity(mIntensityL1);
//  mLight1->SetLightTypeToCameraLight(); // from camera nomal to focal
  // A CameraLight is also attached to the camera, but is not necessarily
  // located at the camera's position.  CameraLights are defined in a
  // coordinate space where the camera is located at (0, 0, 1), looking
  // towards (0, 0, 0) at a distance of 1, with up being (0, 1, 0).

  mLight2 = vtkSmartPointer<vtkLight>::New();
  mLight2->SetPosition(mInitCamPos);
  mLight2->SetFocalPoint(mInitCamFoc);
  mLight2->SetIntensity(mIntensityL2);
  mLight2->SetPositional(0);
  mLight2->SetLightTypeToHeadlight(); // illuminate from camera position to focal point
  // A Headlight is always located at the camera and is pointed at the
  // camera's focal point.  The renderer is free to modify the position and
  // focal point of the camera at any time.

  mRenderer->AddLight(mLight1);
  mRenderer->AddLight(mLight2);
  // reset camera
  mRenderer->ResetCamera();
  mRenderer->ResetCameraClippingRange();

  //  vtkRenderLargeImage *renderLarge = vtkRenderLargeImage::New();
  //	renderLarge->SetInput(mVtkRen);
  //	renderLarge->SetMagnification(4);
  //--------------------------------------------------------------------------------------
  // Set up the default interaction
  //vtkSmartPointer<vtkInteractorStyleImage> imageStyle =  vtkSmartPointer<vtkInteractorStyleImage>::New(); // 2D image
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); // 3D image
  //vtkSmartPointer<vtkInteractorStyleSurfaceWalkerCamera> style = vtkSmartPointer<vtkInteractorStyleSurfaceWalkerCamera>::New(); // 3D image Kazim
  vtkSmartPointer<QVTKInteractor> interactor = vtkSmartPointer<QVTKInteractor>::New();
  interactor->SetRenderWindow(mQVTKWidget->GetRenderWindow());
  interactor->GetInteractorStyle()->SetDefaultRenderer(mRenderer);
  interactor->SetInteractorStyle(style);

  // create new customized callback
  mCallback3D =  vtkSmartPointer<vtk3DInteractionCallback>::New();
  mCallback3D->SetInteractor(interactor); // set the callback to the interactor
  mCallback3D->SetRgbTexture(mRgbTexture);
  mCallback3D->SetHyperImageData(mHyperImageData);
  mCallback3D->SetPolyData(mVtkPolyData);
  mCallback3D->SetGLversion(mGLversion);
  mCallback3D->SetNumCore(mNumCore);
  mCallback3D->SetModelDetail(mNumberOfPoints, mNumberOfPolys, mNumberOfStrips, mNumberOfLines, mNumberOfVerts, mNumberOfCells);
  QFileInfo fi(mFilename);
  QString simplefn = fi.fileName();
  mCallback3D->SetFilename(simplefn);
  QFileInfo fi2(mRgbTextureFilename);
  QString simplefn2 = fi2.fileName();
  mCallback3D->SetTextureFilename(simplefn2);
  mCallback3D->inputImageSet = inputImageSet;

  // link surface walker camera will poly data
  //style->linkPolyData(mVtkPolyData, mCamera);
  //style->linkVtkWidget(this);

  // connect the customized callback to the interactor
 /* style->AddObserver(vtkCommand::MouseMoveEvent, mCallback3D);
  style->AddObserver(vtkCommand::LeftButtonPressEvent, mCallback3D);
  style->AddObserver(vtkCommand::LeftButtonReleaseEvent, mCallback3D);
  style->AddObserver(vtkCommand::MiddleButtonPressEvent, mCallback3D);
  style->AddObserver(vtkCommand::MiddleButtonReleaseEvent, mCallback3D);
  style->AddObserver(vtkCommand::RightButtonPressEvent, mCallback3D);
  style->AddObserver(vtkCommand::RightButtonReleaseEvent, mCallback3D);
  style->AddObserver(vtkCommand::MouseWheelForwardEvent, mCallback3D);
  style->AddObserver(vtkCommand::MouseWheelBackwardEvent, mCallback3D);
  style->AddObserver(vtkCommand::EndPickEvent, mCallback3D);*/

  interactor->AddObserver(vtkCommand::MouseMoveEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::LeftButtonPressEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::LeftButtonReleaseEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::MiddleButtonPressEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::MiddleButtonReleaseEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::RightButtonPressEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::RightButtonReleaseEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::MouseWheelForwardEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::MouseWheelBackwardEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::PickEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::StartPickEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::EndPickEvent, mCallback3D);
  // connect VTK signal to QT slot
  mEventQtSlot = vtkSmartPointer<vtkEventQtSlotConnect>::New();
  // get key pressed with high priority (1.0)
  if (mIsDICOM == false)
    mEventQtSlot->Connect(style, vtkCommand::RightButtonPressEvent, this, SLOT( getHyperPixelsSignals(vtkObject*, unsigned long, void*, void*) ) );
  //------------------------------------------------------------------------------------------
  vtkSmartPointer<vtkCornerAnnotation> annotation = vtkSmartPointer<vtkCornerAnnotation>::New();
  mCallback3D->SetInfoAnnotation(annotation);
  mRenderer->AddViewProp(annotation);
  mCallback3D->displayInfoAnnotation();
  mCallback3D->SetDisplayInfoOn(mDisplayInfoOn);
  mCallback3D->mViewStyle = style;
  //------------------------------------------------------------------------------------------
  // keyboard connection

  // start the interactor
  interactor->Initialize();

  //drawAxes3D();
  //------------------------------------------------------------------------------------------
  emit currentImageChanged(); // update lighting

  mQVTKWidget->show(); // no Render() is required for 3D
  mQVTKWidget->update(); //MK: this is important!
}

void VtkWidget::refreshGeometry3D()
{
  mMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mMapper->SetInputConnection(mVtkPolyData->GetProducerPort()); // this is new version (on July 16)
  mMapper->ScalarVisibilityOff();

  mActor = vtkSmartPointer<vtkActor>::New();
  mActor->SetMapper(mMapper);
  mActor->GetProperty()->SetInterpolationToFlat();
  // set ambient light when directional light is off, by Zeyu Wang on March 28, 2017
  if (!getIsDirectionalLight()) {
	  mActor->GetProperty()->SetAmbient(1);
	  mActor->GetProperty()->SetDiffuse(0);
  }

  mNumberOfPoints = mVtkPolyData->GetNumberOfPoints();
  mNumberOfPolys = mVtkPolyData->GetNumberOfPolys();
  mNumberOfStrips = mVtkPolyData->GetNumberOfStrips();
  mNumberOfLines = mVtkPolyData->GetNumberOfLines();
  mNumberOfVerts = mVtkPolyData->GetNumberOfVerts();
  mNumberOfCells = mVtkPolyData->GetNumberOfCells();

  if (this->mIsInterpolateOn)
    if (mRgbTexture) mRgbTexture->InterpolateOn();
  else
    if (mRgbTexture) mRgbTexture->InterpolateOff();

  if (!mMaterials.empty() && mMaterials[0].fileType != EMPTYFILE) {
    int i=0;
    std::string texscfn = mMaterials[i].textureFilename;
    QString texfn = texscfn.c_str();

    vtkSmartPointer<vtkProperty> property = vtkSmartPointer<vtkProperty>::New();
    property = mActor->GetProperty();
    double ambient[3];    ambient[0] = mMaterials[i].Ka[0];     ambient[1] = mMaterials[i].Ka[1];     ambient[2] = mMaterials[i].Ka[2];
    property->SetAmbientColor(ambient);
    double diffuse[3];    diffuse[0] = mMaterials[i].Kd[0];     diffuse[1] = mMaterials[i].Kd[1];     diffuse[2] = mMaterials[i].Kd[2];
    property->SetDiffuseColor(diffuse);

    double specular[3];    specular[0] = mMaterials[i].Ks[0];     specular[1] = mMaterials[i].Ks[1];     specular[2] = mMaterials[i].Ks[2];
    property->SetSpecularColor(specular);
  }
  else {
    vtkSmartPointer<vtkProperty> property = vtkSmartPointer<vtkProperty>::New();
    property = mActor->GetProperty();
    double ambient[3] = {0.2, 0.2, 0.2};
    property->SetAmbientColor(ambient);
    double diffuse[3] = {1., 1., 1.};
    property->SetDiffuseColor(diffuse);
    double specular[3] = {1., 1., 1.};
    property->SetSpecularColor(specular);
  }

    if (mIsTextureOn && !mRgbTextureFilename.isEmpty())
    {
        mActor->SetTexture(mRgbTexture);
    }
    else
        mActor->SetTexture(NULL);

    if (mVtkPolyData->GetPointData()) {
        if (mVtkPolyData->GetPointData()->GetTCoords()) {
         int numC = mVtkPolyData->GetPointData()->GetTCoords()->GetNumberOfComponents();
         int numT = mVtkPolyData->GetPointData()->GetTCoords()->GetNumberOfTuples();

         if ((numC != 2) || !(numT > 0))
             mActor->SetTexture(NULL);
        }
    }

  setRenderMode3D(mRenderMode3D);

  mRenderer->RemoveAllProps();
  mRenderer->AddActor(mActor);

  mCallback3D->SetPolyData(mVtkPolyData);
  mCallback3D->SetModelDetail(mNumberOfPoints, mNumberOfPolys, mNumberOfStrips, mNumberOfLines, mNumberOfVerts, mNumberOfCells);

  // connect VTK signal to QT slot
  //mEventQtSlot = vtkSmartPointer<vtkEventQtSlotConnect>::New();
  // get key pressed with high priority (1.0)
  //if (mIsDICOM == false)
  //  mEventQtSlot->Connect(style, vtkCommand::RightButtonPressEvent, this, SLOT( getHyperPixelsSignals(vtkObject*, unsigned long, void*, void*) ) );
  //------------------------------------------------------------------------------------------
  vtkSmartPointer<vtkCornerAnnotation> annotation = vtkSmartPointer<vtkCornerAnnotation>::New();
  mCallback3D->SetInfoAnnotation(annotation);
  mRenderer->AddViewProp(annotation);
  mCallback3D->displayInfoAnnotation();
  mCallback3D->SetDisplayInfoOn(mDisplayInfoOn);
  //------------------------------------------------------------------------------------------
  // keyboard connection

  // start the interactor
  //interactor->Initialize();

  //drawAxes3D();
  //------------------------------------------------------------------------------------------
  emit currentImageChanged(); // update lighting

  //mQVTKWidget->show(); // no Render() is required for 3D
  mQVTKWidget->repaint();
  mQVTKWidget->update(); //MK: this is important!
}

// Note: some of this code was borrowed and modified from the VTK screenshot tutorial/example code.
//
// http://www.vtk.org/Wiki/VTK/Examples/Cxx/Utilities/Screenshot
//
QString VtkWidget::screenshot(QString location)
{
    vtkSmartPointer<vtkRenderWindow> renderWindow = mRenderer->GetRenderWindow();

    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(renderWindow);

    // windowToImageFilter->SetMagnification(3);
    // set the resolution of the output image (3 times the current resolution of vtk render window)
    // DT: this screws up the camera--zooms it in a lot and moves it around. Commented out for now

    windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
    windowToImageFilter->Update();

	QString fn;
	if (location.isEmpty())
	{
		QStringList filters;
		filters.push_back("PNG (.png)");
		filters.push_back("JPEG (.jpg)");
		filters.push_back("Bitmap (.bmp)");

		QFileDialog* saveShot = new QFileDialog(this, tr("Save Screenshot"), mw()->lastSavedDirectory.path(), tr("PNG (.png)"));
		saveShot->setAcceptMode(QFileDialog::AcceptSave);
		saveShot->setFileMode(QFileDialog::AnyFile);
		saveShot->setDefaultSuffix("png");

		fn = saveShot->getSaveFileName(this, "Save Screenshot", mw()->lastSavedDirectory.path(),
											   tr("PNG (*.png);;JPG (*.jpg *.jpeg);;Bitmap (*.bmp)"));
	}
	else
	{
		fn = location;
	}

    if(!fn.isEmpty()) {
        QString path = mw()->lastSavedDirectory.absoluteFilePath(fn);
        path.truncate(path.lastIndexOf("/"));
        mw()->lastSavedDirectory.setPath(path);

        QStringList extensions;
        extensions << "png" << "jpg" << "jpeg" << "bmp";
        QStringList split = fn.split(".");
        QString ext = split[split.size() - 1];
        ext = ext.toLower();
        if(!extensions.contains(ext)) {
            fn.append(".png");
            ext = "png";
        }

        // convert QString to C string
        QByteArray ba = fn.toUtf8();
        const char* name = ba.constData();

        if(ext == "jpg" || ext == "jpeg") {
            vtkSmartPointer<vtkJPEGWriter> writer = vtkSmartPointer<vtkJPEGWriter>::New();
            writer->SetFileName(name);
            writer->SetInputConnection(windowToImageFilter->GetOutputPort());
            writer->Write();
        } else if(ext == "bmp") {
            vtkSmartPointer<vtkBMPWriter> writer = vtkSmartPointer<vtkBMPWriter>::New();
            writer->SetFileName(name);
            writer->SetInputConnection(windowToImageFilter->GetOutputPort());
            writer->Write();
        } else {
            vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
            writer->SetFileName(name);
            writer->SetInputConnection(windowToImageFilter->GetOutputPort());
            writer->Write();
        }
    }

    if(mQVTKWidget) mQVTKWidget->update();
	return fn;
}

void VtkWidget::saveFileInfo(QWidget* editBox)
{
    QTextEdit *box = qobject_cast<QTextEdit*>(editBox);
    if (box == 0) return;
    QFileInfo finfo(mFilename);
	/*********** Modified by Weiqi Shi 1/19/2016*************/
    QString path = mProjectPath;
	/*********** Modified by Weiqi Shi 1/19/2016*************/
    QStringList tokens = path.split(QDir::separator());
    QString CHEROb = tokens.takeLast();
    path = tokens.join(QDir::separator());
    path = QDir::toNativeSeparators(path);
    if(!QDir(path).exists()) {
        QDir().mkdir(path);
    }

    path.append(QDir::separator() + CHEROb);
    if(!QDir(path).exists()) {
        QDir().mkdir(path);
    }

    path.append(QDir::separator() + finfo.fileName().replace(" ", "_"));
    if(!QDir(path).exists()) {
        QDir().mkdir(path);
    }

    path.append(QDir::separator());
    path.append("about.html");

    QFile f(path);
    f.open(QFile::WriteOnly | QFile::Text);
    f.resize(0);
    QTextStream out(&f);
    out << box->toHtml();
    f.close();
}

/****** Modified by Weiqi Shi 1/26/2016 *******/
void VtkWidget::displayFileInfo()
{
    //Metadata* dialog = new Metadata(mProjectPath, this, true);
    //dialog->exec();
	if (mFileInfoDialog)
	{
		mFileInfoDialog->hide();
		delete mFileInfoDialog;
	}
	mFileInfoDialog = new FileInfoDialog(this);
}

void VtkWidget::closeFileInfo()
{
    //Metadata* dialog = new Metadata(mProjectPath, this, true);
    //dialog->exec();
	if (mFileInfoDialog)
	{
		mFileInfoDialog->hide();
		delete mFileInfoDialog;
	}
	mFileInfoDialog = NULL;
}


/*void VtkWidget::displayProjectInfo()
{
    Metadata* dialog = new Metadata();
    dialog->exec();
    delete dialog;
}*/
/****** Modified by Weiqi Shi 1/26/2016 *******/

void VtkWidget::zoomIn()
{
    if (mCallback2D) mCallback2D->zoomIn();
    if (mCallback3D) mCallback3D->zoomIn();
}

void VtkWidget::zoomOut()
{
    if (mCallback2D) mCallback2D->zoomOut();
    if (mCallback3D) mCallback3D->zoomOut();
}

void VtkWidget::zoomReset()
{
//    mRenderer = mVtkImageViewer->GetRenderer(); // get the renderer
////    mRenderer->GetActiveCamera()->


    mRenderer->ResetCamera();
    mRenderer->ResetCameraClippingRange();
    mRenderer->Modified();

    emit currentImageChanged(); // update lighting
    if (mQVTKWidget) mQVTKWidget->show(); // no Render() is required for 3D
    if (mQVTKWidget) mQVTKWidget->update(); //MK: this is important!
}

void VtkWidget::changeBGColor()
{
	double backgroundRGB[3];
	mRenderer->GetBackground(backgroundRGB[0], backgroundRGB[1], backgroundRGB[2]);
	if (backgroundRGB[0] > 0) mRenderer->SetBackground(0, 0, 0);
	else mRenderer->SetBackground(0.5, 0.5, 0.5);
	if (mQVTKWidget) mQVTKWidget->show();
    if (mQVTKWidget) mQVTKWidget->update();
}

void VtkWidget::updateDisplayPanel()
{
  if(mCallback2D) mCallback2D->displayInfoAnnotation(true); //this screws up camera position
  if(mCallback3D) mCallback3D->displayInfoAnnotation(true); //this screws up camera position
  if(mQVTKWidget) mQVTKWidget->show(); // no Render() is required for 3D
  if(mQVTKWidget) mQVTKWidget->update(); //MK: this is important!
}

QImage VtkWidget::vtkImageDataToQImage(vtkImageData* imageData)
{
  if (!imageData)
    {
    return QImage();
    }
  imageData->Update();
  //todo retrieve just the UpdateExtent
  int width = imageData->GetDimensions()[0];
  int height = imageData->GetDimensions()[1];
  QImage image(width, height, QImage::Format_RGB32);
  QRgb* rgbPtr = reinterpret_cast<QRgb*>(image.bits()) +
    width * (height-1);
  unsigned char* colorsPtr = reinterpret_cast<unsigned char*>(
    imageData->GetScalarPointer());
  // mirror vertically

  for(int row = 0; row < height; ++row)
    {
    for (int col = 0; col < width; ++col)
      {
      // Swap rgb
      *(rgbPtr++) = QColor(colorsPtr[0], colorsPtr[1], colorsPtr[2]).rgb();
      colorsPtr +=  3;
      }
    rgbPtr -= width * 2;
    }
  return image;
}

void VtkWidget::ImageShifter(vtkImageData *imageData)
{
  // DICOM: My data has a range of -868 to 2312
  // I expect the output to be 0 to 255
  vtkSmartPointer<vtkImageShiftScale> Shifter = vtkSmartPointer<vtkImageShiftScale>::New();
  // range shifting
  double range[2];
  imageData->GetScalarRange(range);

  double scale = 255.0/(range[1] - range[0]);
  double shift = -range[0];

  Shifter->SetInput( imageData );
  Shifter->SetShift( shift );
  Shifter->SetScale( scale );
  //Shifter->SetOutputScalarTypeToUnsignedChar();
  Shifter->SetOutputScalarTypeToFloat(); // float value from 0.f-255.f
  Shifter->Update();

  imageData->DeepCopy(Shifter->GetOutput());
}

void VtkWidget::CreateRGBImage(vtkImageData* image, int width, int height, int channels = 3)
{
  image->SetDimensions(width, height, 1);

  image->SetNumberOfScalarComponents(channels);
  image->SetScalarTypeToUnsignedChar();
  image->AllocateScalars();

  for( int x = 0; x < width; x++)
    {
    for( int y = 0; y < height; y++)
      {
      unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer(x,y,0));
      if(x < width/2)
     {
     pixel[0] = 255;
     }
      else
     {
     pixel[0] = 0;
     }
      pixel[1] = 0;
      pixel[2] = 255;
      }
    }

  image->Modified();
}

// ITK coordinate conversion should be done in here!
void VtkWidget::Rendering2D()
{
  if (mQVTKWidget != NULL)
    delete mQVTKWidget;

  mQVTKWidget = new QVTKWidget(this);

  if (mLayout!=NULL)
    delete mLayout;
  mLayout = new QVBoxLayout( this ); // size stretching trick
  mLayout->setMargin( 0 );
  mLayout->addWidget( mQVTKWidget, 1 );
  this->setLayout(mLayout);

  if (mVtkImageData==NULL)
    return;


  // because the 2D stack view requires sophiscated controls,
  // we have a member viewer class. So, if there is an object already,
  // we don't need to re-read anythings.
  mVtkImageViewer = vtkSmartPointer<vtkImageViewer2>::New();

  int dimss[3];
  mVtkImageData->GetDimensions(dimss);
  if (dimss[2] > 1){
      mWidgetMode = CTSTACK;
      //flipITKtoVTKxy(mVtkImageData); // left-right flip bug fix (2013-07-14)
  }
  else {
      mWidgetMode = IMAGE2D;
      flipITKtoVTKy(mVtkImageData); // left-right flip bug fix (2013-07-14)
  }
  emit currentWidgetModeChanged(mWidgetMode, getIsDirectionalLight());
//  mkDebug md; md.qDebugImageData(mVtkImageData); // fine
//    imagedata1:  626   832   1 :  3
//    imagedata2:  626   832   1 :  8
//    imagedata1 (10,10,0,1):  53
//    imagedata2 (10,10,0,1):  0.0317993

  //=======================================================================================
  // This part is the crashing point in the deploy mode
  // connect the data to the imageviewer

  mVtkImageViewer->SetInput(mVtkImageData); // we use only mVTKImageData for RGB display

//  vtkSmartPointer<vtkImageData> tempImageData = vtkSmartPointer<vtkImageData>::New();
//  int dimss[3];  mVtkImageData->GetDimensions(dimss);
//  CreateRGBImage(tempImageData, dimss[0], dimss[1]);
//  tempImageData->ShallowCopy(mVtkImageData);
//  tempImageData->Update();

//  mVtkImageViewer->SetInput(tempImageData); // we use only mVTKImageData for RGB display

//   mkDebug md; md.qDebugImageData(mVtkImageViewer->GetInput());
  //=======================================================================================

  // but hyperspectral data is stored in mHyperImageData;
  mVtkImageViewer->UpdateDisplayExtent(); // update ImageViewer2 for hard reset (this is equivalent to Update() in VTK objects)

  // connect the renderer to the window
  mVtkImageViewer->SetRenderWindow(mQVTKWidget->GetRenderWindow());
  //this->GetRenderWindow()->AddRenderer(mVtkImageViewer->GetRenderer());


  //-------------------------------------------------------------------------
  // adding a light
  mRenderer = mVtkImageViewer->GetRenderer(); // get the renderer

  // adding a light
  mLight1 = vtkSmartPointer<vtkLight>::New();
  mLight1->SetFocalPoint(0,0,0);
  mLight1->SetPosition(mRenderer->GetActiveCamera()->GetPosition());
  mLight1->SetIntensity(mIntensityL1);

  mLight2 = vtkSmartPointer<vtkLight>::New();
  mLight2->SetFocalPoint(0,0,0);
  mLight2->SetPosition(mRenderer->GetActiveCamera()->GetPosition());
  mLight2->SetIntensity(mIntensityL2);

  mRenderer->AddLight(mLight1);
  mRenderer->AddLight(mLight2);

  mVtkImageViewer->SetRenderer(mRenderer); // set the renderer back
  //-------------------------------------------------------------------------
  // Picker to pick pixels
//  vtkSmartPointer<vtkPropPicker> propPicker = vtkSmartPointer<vtkPropPicker>::New();
  vtkSmartPointer<vtkCellPicker> propPicker = vtkSmartPointer<vtkCellPicker>::New();
  propPicker->PickFromListOn();
  // Give the picker a prop to pick
  vtkSmartPointer<vtkImageActor> imageActor = mVtkImageViewer->GetImageActor();
  propPicker->AddPickList(imageActor);

  // disable interpolation, so we can see each pixel
  if (this->mIsInterpolateOn)
    imageActor->InterpolateOn();
  else
    imageActor->InterpolateOff();
  //-------------------------------------------------------------------------

  // connect the image viewer to the interactor (only for CT)
  // scrolling becomes brightness/contrast controls.
  // Otherwise, the mouse interaction becomes moving object in OpenGL
  mVtkImageViewer->SetupInteractor(mQVTKWidget->GetRenderWindow()->GetInteractor()); // connect imageviewer2 to window interactor

  // connect the customized callback to the interactor
//  if (mIsDICOM) { // for medical images
    // currrently this interactor cannot be supported with CT medical imaging data
    mVtkImageViewer->GetRenderer()->ResetCamera();
//  } else { // for normal images
    // summary the mouse events are as follows:
    // + Left Mouse button triggers window level events
    // + CTRL Left Mouse spins the camera around its view plane normal
    // + SHIFT Left Mouse pans the camera
    // + CTRL SHIFT Left Mouse dollys (a positional zoom) the camera
    // + Middle mouse button pans the camera
    // + Right mouse button dollys the camera.
    // + SHIFT Right Mouse triggers pick events
    
	//vtkSmartPointer<vtkInteractorStyleImage> style =  vtkSmartPointer<vtkInteractorStyleImage>::New(); // 2D image
	//vtkSmartPointer<vtkInteractorStyle> style =  vtkSmartPointer<vtkInteractorStyle>::New(); // 2D image
	vtkSmartPointer<myVTKInteractorStyle> style =  vtkSmartPointer<myVTKInteractorStyle>::New(); // 2D image
    //vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); // 3D image

    vtkSmartPointer<QVTKInteractor> interactor = vtkSmartPointer<QVTKInteractor>::New();
    interactor->SetRenderWindow(mQVTKWidget->GetRenderWindow());
    interactor->GetInteractorStyle()->SetDefaultRenderer(mRenderer);
    interactor->SetInteractorStyle(style);

    //---------------------------------------------------------------------------------------------
    // create new customized callback
    mCallback2D =  vtkSmartPointer<vtk2DInteractionCallback>::New();
    mCallback2D->SetInteractor(interactor); // set the callback to the interactor
    mCallback2D->SetViewer(mVtkImageViewer);
    mCallback2D->SetGLversion(mGLversion);
    mCallback2D->SetNumCore(mNumCore);
    mCallback2D->SetPicker(propPicker);
    QFileInfo fi(mFilename);
    QString simplefn = fi.baseName();
    mCallback2D->SetFilename(simplefn);
    mCallback2D->SetHyperImageData(mHyperImageData); // for hyperspectral data reading
    vtkSmartPointer<vtkCornerAnnotation> annotation = vtkSmartPointer<vtkCornerAnnotation>::New();
    mCallback2D->SetInfoAnnotation(annotation);
    mRenderer->AddViewProp(annotation);
    mCallback2D->SetIsDICOM(mIsDICOM);
    if (mIsDICOM == true)
        mCallback2D->SetWindowLevels();
    mCallback2D->displayInfoAnnotation();
    mCallback2D->SetDisplayInfoOn(mDisplayInfoOn);

	interactor->AddObserver(vtkCommand::MouseMoveEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::LeftButtonPressEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::LeftButtonReleaseEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::MiddleButtonPressEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::MiddleButtonReleaseEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::RightButtonPressEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::RightButtonReleaseEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::MouseWheelForwardEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::MouseWheelBackwardEvent, mCallback2D);
	interactor->AddObserver(vtkCommand::SelectionChangedEvent, mCallback2D);
	interactor->AddObserver(vtkCommand::PickEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::StartPickEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::EndPickEvent, mCallback2D);
    //---------------------------------------------------------------------------------------------

    // connect VTK signal to QT slot
    mEventQtSlot = vtkSmartPointer<vtkEventQtSlotConnect>::New();
    // get key pressed with high priority (1.0)
    if (mIsDICOM == false)
        mEventQtSlot->Connect(style, vtkCommand::RightButtonPressEvent, this, SLOT( getHyperPixelsSignals(vtkObject*, unsigned long, void*, void*) ) );

    // start the interactor
    interactor->Initialize();

    // update current view
//    qDebug() << "mOrientationCurrent: " << mOrientationCurrent;
//    qDebug() << "mSliceCurrent: " << mSliceCurrent;

    updateCurrentOrientation(mOrientationCurrent); // update orientation
    updateCurrentSlice(mSliceCurrent);

//  }

  //------------------------------------------------------------------------------------------
  // Create rendering for a single image
  mVtkImageViewer->Render();
  mQVTKWidget->show(); // no Render() is required for 3D
  mQVTKWidget->update(); //MK: this is important!
}

//-----------------------------------------------------------------------------------------
// this is GPU-based rendering (this part is the only different from the CPU-based rendering codes)
class vtkBoxWidgetCallback : public vtkCommand
{
public:
  static vtkBoxWidgetCallback * New()
   {return new vtkBoxWidgetCallback; }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkBoxWidget * widget = reinterpret_cast<vtkBoxWidget*>(caller);
    if(this->Mapper)
    {
      vtkPlanes *planes = vtkPlanes::New();
      widget->GetPlanes(planes);
      this->Mapper->SetClippingPlanes(planes);
      planes->Delete();
    }
  }

  void SetMapper(vtkSmartVolumeMapper * m)
  { this->Mapper = m; }

protected:
  vtkBoxWidgetCallback()
    { this->Mapper = 0; }
  vtkSmartVolumeMapper * Mapper;
};
//-----------------------------------------------------------------------------------------
//=========================================================================
// Acknowledgement:
// Some portions of this function, RenderingVolume(), are based on
// a demo code (GPURenderDemo.cxx), written by Ken Martin, Will Schroeder, Bill Lorensen at Kitware.
//=========================================================================
void VtkWidget::RenderingVolume(int blendType, float reductionFactor, CTVolumeRenderMode volumeRenderMode)
{
  if (mVtkImageData==NULL)
    return;

  mWidgetMode = CTVOLUME;
  emit currentWidgetModeChanged(mWidgetMode, getIsDirectionalLight());

  int count = 1;
  char *dirname = NULL;
  double opacityWindow = 4096;
  double opacityLevel = 2048;
  //int blendType = 0;
  //double reductionFactor = 1.0;
  double frameRate = 10.0;
  char *fileName=0;
  int fileType=0;

  int clip = 1; // for clipping of bounding volume

  bool independentComponents=true;

  // Create the renderer, render window and interactor
  // connect the interactor to the renderer
  // VTK/Qt wedded
  if (mQVTKWidget != NULL)
    delete mQVTKWidget;

  mQVTKWidget = new QVTKWidget(this);

  if (mLayout!=NULL)
    delete mLayout;
  mLayout = new QVBoxLayout( this ); // size stretching trick
  mLayout->setMargin( 0 );
  mLayout->addWidget( mQVTKWidget, 1 );
  this->setLayout(mLayout);

  mRenderer = vtkSmartPointer<vtkRenderer>::New();

  //=======================================================================================================
  // Set up the default interaction
  //vtkSmartPointer<vtkInteractorStyleImage> imageStyle =  vtkSmartPointer<vtkInteractorStyleImage>::New(); // 2D image
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); // 3D image
  vtkSmartPointer<QVTKInteractor> interactor = vtkSmartPointer<QVTKInteractor>::New();
  interactor->SetRenderWindow(mQVTKWidget->GetRenderWindow());
  interactor->GetInteractorStyle()->SetDefaultRenderer(mRenderer);
  interactor->SetInteractorStyle(style);
  // SetDesiredUpdateRate - the vtkRenderWindow divides it
  // allocated time across all renderers, and the renderer
  // divides it time across all props. If clip is
  // true then there are two props
  interactor->SetDesiredUpdateRate(frameRate / (1+clip) );
  //=======================================================================================================

  // resample the input source
  vtkSmartPointer<vtkImageResample> resample = vtkSmartPointer<vtkImageResample>::New();
  if ( reductionFactor < 1.0 )
    {
      resample->SetInput( mVtkImageData );
      resample->SetAxisMagnificationFactor(0, reductionFactor);
      resample->SetAxisMagnificationFactor(1, reductionFactor);
      resample->SetAxisMagnificationFactor(2, reductionFactor);
    }

//  //BTX
//  // The possible values for the default and current render mode ivars
//    enum
//    {
//      DefaultRenderMode=0,
//      RayCastAndTextureRenderMode,
//      RayCastRenderMode,
//      TextureRenderMode,
//      GPURenderMode,
//      UndefinedRenderMode,
//      InvalidRenderMode
//    };
//  //ETX
  // Create our volume and mapper
//  vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
  vtkVolume* volume = vtkVolume::New();
  //-----------------------------------------------------------------------------------------
  //MK: select VolumeMapping mode
  mVolMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
  switch (volumeRenderMode)
  {
  default:
  case AUTOMATIC:
    mVolMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::DefaultRenderMode);
    break;
  case DEFAULTGPU:
    mVolMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::GPURenderMode);
    break;
    // without tweak of VTK library
    // error message on Lion "Rendering failed because the following OpenGL extensions are required but not supported"
  case CPURAYCASTONLY:
    mVolMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::RayCastRenderMode);
    break;
  case CPURAYCASTTEXTURE:
    mVolMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::TextureRenderMode);
    break;
  }

  // Add a box widget if the clip option was selected
  //vtkSmartPointer<vtkBoxWidget> box = vtkSmartPointer<vtkBoxWidget>::New();
  vtkBoxWidget* box = vtkBoxWidget::New();
  if (clip)
  {
      box->SetInteractor(interactor);
      box->SetPlaceFactor(1.01);
      if ( reductionFactor < 1.0 )
      {
        box->SetInput(resample->GetOutput());
      }
      else
      {
        box->SetInput(mVtkImageData);
      }

      box->SetDefaultRenderer(mRenderer);
      box->InsideOutOn();
      box->PlaceWidget();

      vtkBoxWidgetCallback* callback = vtkBoxWidgetCallback::New();
      callback->SetMapper(mVolMapper);
      box->AddObserver(vtkCommand::InteractionEvent, callback);
      callback->Delete();

      box->EnabledOn();
      box->GetSelectedFaceProperty()->SetOpacity(0.0);
  }//clip
  //-----------------------------------------------------------------------------------------

  if ( reductionFactor < 1.0 )//shrink
  {
      mVolMapper->SetInputConnection( resample->GetOutputPort() );
      mVolMapper->Update();
  }
  else // go straight
  {
      mVolMapper->SetInput( mVtkImageData );
      mVolMapper->Update();
  }

  // Set the sample distance on the ray to be 1/2 the average spacing
//  double spacing[3];
//  if ( reductionFactor < 1.0 )
//    {
//    resample->GetOutput()->GetSpacing(spacing);
//    resample->Update();
//    }
//  else
//    {
//    mVtkImageData->GetSpacing(spacing);
//    mVtkImageData->Update();
//    }

//  mapper->SetSampleDistance( (spacing[0]+spacing[1]+spacing[2])/6.0 );
//  mapper->SetMaximumImageSampleDistance(10.0);


  // Create our transfer function
  vtkSmartPointer<vtkColorTransferFunction>colorFun = vtkSmartPointer<vtkColorTransferFunction>::New();
  vtkSmartPointer<vtkPiecewiseFunction>opacityFun = vtkSmartPointer<vtkPiecewiseFunction>::New();

  // Create the property and attach the transfer functions
  vtkSmartPointer<vtkVolumeProperty>property = vtkSmartPointer<vtkVolumeProperty>::New();
  property->SetIndependentComponents(independentComponents);
  property->SetColor( colorFun );
  property->SetScalarOpacity( opacityFun );
  property->SetInterpolationTypeToLinear();

  // connect up the volume to the property and the mapper
  volume->SetProperty( property );
  volume->SetMapper( mVolMapper );

  // Depending on the blend type selected as a command line option,
  // adjust the transfer function
  switch ( blendType )
    {
    case WOOD: // custom (fitted to the wooden object ), inherited from CT_Skin
      colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );         // -3024
      colorFun->AddRGBPoint( -500, .62, .62, .62, 0.5, 0.0 );    // -1000
      colorFun->AddRGBPoint( 1000, .88, .88, .88, 0.33, 0.45 );  // -500
      colorFun->AddRGBPoint( 3071, .83, .83, .83, 0.5, 0.0 );    // 3071

      opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );   // -3024
      opacityFun->AddPoint(-500, 0, 0.5, 0.0 );    // -1000
      opacityFun->AddPoint(500, 1.0, 0.33, 0.45 );  // -500
      opacityFun->AddPoint(3071, 1.0, 0.5, 0.0);    // 3071

      mVolMapper->SetBlendModeToComposite();
      property->ShadeOn();
      property->SetAmbient(0.1);
      property->SetDiffuse(0.9);
      property->SetSpecular(0.5);
      property->SetSpecularPower(10.0);
      property->SetScalarOpacityUnitDistance(0.8919);
      break;
//==============================================================================
      // RGB_Composite
      // Use compositing and functions set to highlight red/green/blue regions
      // in RGB data. Not for use on single component data
//      opacityFun->AddPoint(0, 0.0);
//      opacityFun->AddPoint(5.0, 0.0);
//      opacityFun->AddPoint(30.0, 0.05);
//      opacityFun->AddPoint(31.0, 0.0);
//      opacityFun->AddPoint(90.0, 0.0);
//      opacityFun->AddPoint(100.0, 0.3);
//      opacityFun->AddPoint(110.0, 0.0);
//      opacityFun->AddPoint(190.0, 0.0);
//      opacityFun->AddPoint(200.0, 0.4);
//      opacityFun->AddPoint(210.0, 0.0);
//      opacityFun->AddPoint(245.0, 0.0);
//      opacityFun->AddPoint(255.0, 0.5);

//      mVolMapper->SetBlendModeToComposite();
//      property->ShadeOff();
//      property->SetScalarOpacityUnitDistance(1.0);
//==============================================================================

    // MIP
    // Create an opacity ramp from the window and level values.
    // Color is white. Blending is MIP.
    case MIP:
      colorFun->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0 );
      opacityFun->AddSegment( opacityLevel - 0.5*opacityWindow, 0.0,
                              opacityLevel + 0.5*opacityWindow, 1.0 );
      mVolMapper->SetBlendModeToMaximumIntensity();
      break;

    // CompositeRamp
    // Create a ramp from the window and level values. Use compositing
    // without shading. Color is a ramp from black to white.
    case CRAMP:
      colorFun->AddRGBSegment(opacityLevel - 0.5*opacityWindow, 0.0, 0.0, 0.0,
                              opacityLevel + 0.5*opacityWindow, 1.0, 1.0, 1.0 );
      opacityFun->AddSegment( opacityLevel - 0.5*opacityWindow, 0.0,
                              opacityLevel + 0.5*opacityWindow, 1.0 );
      mVolMapper->SetBlendModeToComposite();
      property->ShadeOff();
      break;

    // CompositeShadeRamp
    // Create a ramp from the window and level values. Use compositing
    // with shading. Color is white.
    case CSRAMP:
      colorFun->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0 );
      opacityFun->AddSegment( opacityLevel - 0.5*opacityWindow, 0.0,
                              opacityLevel + 0.5*opacityWindow, 1.0 );
      mVolMapper->SetBlendModeToComposite();
      property->ShadeOn();
      break;

    // CT_Skin
    // Use compositing and functions set to highlight skin in CT data
    // Not for use on RGB data
    case CT_SKIN:
      colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
      colorFun->AddRGBPoint( -1000, .62, .36, .18, 0.5, 0.0 );
      colorFun->AddRGBPoint( -500, .88, .60, .29, 0.33, 0.45 );
      colorFun->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );

      opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
      opacityFun->AddPoint(-1000, 0, 0.5, 0.0 );
      opacityFun->AddPoint(-500, 1.0, 0.33, 0.45 );
      opacityFun->AddPoint(3071, 1.0, 0.5, 0.0);

      mVolMapper->SetBlendModeToComposite();
      property->ShadeOn();
      property->SetAmbient(0.1);
      property->SetDiffuse(0.9);
      property->SetSpecular(0.2);
      property->SetSpecularPower(10.0);
      property->SetScalarOpacityUnitDistance(0.8919);
      break;

    // CT_Bone
    // Use compositing and functions set to highlight bone in CT data
    // Not for use on RGB data
    case CT_BONE:
      colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
      colorFun->AddRGBPoint( -16, 0.73, 0.25, 0.30, 0.49, .61 );
      colorFun->AddRGBPoint( 641, .90, .82, .56, .5, 0.0 );
      colorFun->AddRGBPoint( 3071, 1, 1, 1, .5, 0.0 );

      opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
      opacityFun->AddPoint(-16, 0, .49, .61 );
      opacityFun->AddPoint(641, .72, .5, 0.0 );
      opacityFun->AddPoint(3071, .71, 0.5, 0.0);

      mVolMapper->SetBlendModeToComposite();
      property->ShadeOn();
      property->SetAmbient(0.1);
      property->SetDiffuse(0.9);
      property->SetSpecular(0.2);
      property->SetSpecularPower(10.0);
      property->SetScalarOpacityUnitDistance(0.8919);
      break;

    // CT_Muscle
    // Use compositing and functions set to highlight muscle in CT data
    // Not for use on RGB data
    case CT_MUSCLE:
      colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
      colorFun->AddRGBPoint( -155, .55, .25, .15, 0.5, .92 );
      colorFun->AddRGBPoint( 217, .88, .60, .29, 0.33, 0.45 );
      colorFun->AddRGBPoint( 420, 1, .94, .95, 0.5, 0.0 );
      colorFun->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );

      opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
      opacityFun->AddPoint(-155, 0, 0.5, 0.92 );
      opacityFun->AddPoint(217, .68, 0.33, 0.45 );
      opacityFun->AddPoint(420,.83, 0.5, 0.0);
      opacityFun->AddPoint(3071, .80, 0.5, 0.0);

      mVolMapper->SetBlendModeToComposite();
      property->ShadeOn();
      property->SetAmbient(0.1);
      property->SetDiffuse(0.9);
      property->SetSpecular(0.2);
      property->SetSpecularPower(10.0);
      property->SetScalarOpacityUnitDistance(0.8919);
      break;

    default:
       vtkGenericWarningMacro("Unknown blend type.");
       break;
    }

  //------------------------------------------------------------------------------------------
  /* [Ver.1]
  // Connect it all. Note that funny arithematic on the
  // SetDesiredUpdateRate - the vtkRenderWindow divides it
  // allocated time across all renderers, and the renderer
  // divides it time across all props. If clip is
  // true then there are two props
  vtkSmartPointer<QVTKInteractor> interactor = vtkSmartPointer<QVTKInteractor>::New();
  interactor->SetRenderWindow(this->GetRenderWindow());
  interactor->SetDesiredUpdateRate(frameRate / (1+clip) );
  interactor->GetInteractorStyle()->SetDefaultRenderer(mRenderer);

  //MK: change the interaction style with trackball style
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
      vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
  interactor->SetInteractorStyle(style);
  interactor->Initialize();
  */
  //------------------------------------------------------------------------------------------

  // create new customized callback
  mCallback3D =  vtkSmartPointer<vtk3DInteractionCallback>::New();
  mCallback3D->SetInteractor(interactor); // set the callback to the interactor
  mCallback3D->SetRgbTexture(mRgbTexture);
  mCallback3D->SetHyperImageData(mHyperImageData);
  mCallback3D->SetPolyData(mVtkPolyData);
  mCallback3D->SetGLversion(mGLversion);
  mCallback3D->SetNumCore(mNumCore);
  mCallback3D->SetModelDetail(mNumberOfPoints, mNumberOfPolys, mNumberOfStrips, mNumberOfLines, mNumberOfVerts, mNumberOfCells);
  QFileInfo fi(mFilename);
  QString simplefn = fi.fileName();
  mCallback3D->SetFilename(simplefn);
  mCallback3D->SetTextureFilename(mRgbTextureFilename);
  mCallback3D->mViewStyle = style;
  mCallback3D->SetCTVolume(true);



  int dimss[3];
  mVtkImageData->GetDimensions(dimss);
  mCallback3D->SetDimensions(dimss);

  // connect the renderer to the rendering window
  mRenderer->AddVolume(volume);// Add interActor to renderer

  // reset camera
  mRenderer->ResetCamera();
  mRenderer->ResetCameraClippingRange();

  vtkSmartPointer<vtkCamera> camera = mRenderer->GetActiveCamera();
  camera->GetPosition(mInitCTCamPos);
  camera->GetFocalPoint(mInitCTCamFoc);
  camera->GetViewUp(mInitCTCamUp);

  // connect the customized callback to the interactor
 /* style->AddObserver(vtkCommand::MouseMoveEvent, mCallback3D);
  style->AddObserver(vtkCommand::LeftButtonPressEvent, mCallback3D);
  style->AddObserver(vtkCommand::LeftButtonReleaseEvent, mCallback3D);
  style->AddObserver(vtkCommand::MiddleButtonPressEvent, mCallback3D);
  style->AddObserver(vtkCommand::MiddleButtonReleaseEvent, mCallback3D);
  style->AddObserver(vtkCommand::RightButtonPressEvent, mCallback3D);
  style->AddObserver(vtkCommand::RightButtonReleaseEvent, mCallback3D);
  style->AddObserver(vtkCommand::MouseWheelForwardEvent, mCallback3D);
  style->AddObserver(vtkCommand::MouseWheelBackwardEvent, mCallback3D);
  style->AddObserver(vtkCommand::EndPickEvent, mCallback3D);*/

  interactor->AddObserver(vtkCommand::MouseMoveEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::LeftButtonPressEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::LeftButtonReleaseEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::MiddleButtonPressEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::MiddleButtonReleaseEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::RightButtonPressEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::RightButtonReleaseEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::MouseWheelForwardEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::MouseWheelBackwardEvent, mCallback3D);
  interactor->AddObserver(vtkCommand::EndPickEvent, mCallback3D);

  //------------------------------------------------------------------------------------------
  vtkSmartPointer<vtkCornerAnnotation> annotation = vtkSmartPointer<vtkCornerAnnotation>::New();
  mCallback3D->SetInfoAnnotation(annotation);
  mRenderer->AddViewProp(annotation);
  mCallback3D->displayInfoAnnotation();
  mCallback3D->SetDisplayInfoOn(mDisplayInfoOn);
  //------------------------------------------------------------------------------------------
  // connect VTK signal to QT slot
  mEventQtSlot = vtkSmartPointer<vtkEventQtSlotConnect>::New();
  // get key pressed with high priority (1.0)
  mEventQtSlot->Connect(style, vtkCommand::RightButtonPressEvent, this, SLOT( getHyperPixelsSignals(vtkObject*, unsigned long, void*, void*) ) );

  // start the interactor
  interactor->Initialize();

  //drawAxes3D();
  //------------------------------------------------------------------------------------------
  mQVTKWidget->GetRenderWindow()->AddRenderer(mRenderer);

  mQVTKWidget->show(); // no Render() is required for 3D


  // vacate intermediate memories
  volume->Delete();
  box->Delete();

  update(); //MK: this is important!
}

void VtkWidget::testObject()
{
  /*
      //=================================================================
      // This is working for Image reading
      //=================================================================
      //QVTKWidget widget;// this crashes
      //widget.resize(256,256);
      m_qvtkWidget = new QVTKWidget(this);

      vtkPNGReader* reader = vtkPNGReader::New();
    //  char* fname = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/vtk.png");
      char* fname = "./icons/CHEROb_Logo128.png";
      reader->SetFileName(fname);
      //delete [] fname;

      vtkImageViewer* image_view = vtkImageViewer::New();
      image_view->SetInputConnection(reader->GetOutputPort());

      m_qvtkWidget->SetRenderWindow(image_view->GetRenderWindow());
      image_view->SetupInteractor(m_qvtkWidget->GetRenderWindow()->GetInteractor());

      image_view->SetColorLevel(138.5);
      image_view->SetColorWindow(233);

      m_qvtkWidget->show();
      m_qvtkWidget->resize(256,256);
      //=================================================================
  */
  /*
//  testObject();

#if defined(_WIN32) || defined(_WIN64)
  //QString fn("C:\\Users\\mhk5\\Documents\\Visual Studio 2008\\Projects\\CHEROb\\exampledata\\totem\\totem5_T5k1.ply");
  QString fn("C:\\Users\\mhk5\\Documents\\Visual Studio 2008\\Projects\\CHEROb\\exampledata\\bunny\\reconstruction\\bun_zipper_res4.ply");
#else
//  QString fn("/Users/minhkim/Research/codes/C++/3D_Graphics/CHEROb/exampledata/totem/totem5_T5k1.ply");
  QString fn("/Users/minhkim/Research/codes/C++/3D_Graphics/CHEROb/exampledata/totem/totem5_T5k1.jpg");
#endif
//  qDebug() << fn;
  ReadData(fn);
  */
  /*
  //---------------------------------------
  // Model:
  // Geometry
  VTK_CREATE(vtkVectorText, text);
  text->SetText("VTK and Qt!");
  VTK_CREATE(vtkElevationFilter, elevation);
  elevation->SetInputConnection(text->GetOutputPort());
  elevation->SetLowPoint(0,0,0);
  elevation->SetHighPoint(10,0,0);
  //---------------------------------------
  mVtkPolyData = elevation->GetOutput();
  mVtkPolyData->Update();

  Rendering3D();
  */
}


bool VtkWidget::ReadData(QString filename)
{
  //MK: find out file type
  QFileInfo fi(filename);
  mFilename = fi.absoluteFilePath();	//Use the original file path to read data instead of the file that will be copied into project folder. This path will be updated later in mainWindow.cpp

  /**
   *  NOTICE: here needs further improvements. The Project Path should be specified by User 
   *  Corresponding modification should be made for bookmark. All staff(bookmark, information)
   *  should be saved in the project folder
   */
  //mProjectPath = mw()->getDataLocation();
  /****** NOTICE End *****/

  QString extension = fi.suffix().toLower();
  std::string fnstr = filename.toLocal8Bit().constData(); // QString -> Std. String

  // setting for QVTK
  this->setObjectName(filename); //QString::fromUtf8("qvtkWidget")

// These are moved to the lowest level functions (July 17)
//  mRgbTexture = vtkSmartPointer<vtkTexture>::New();
//  mHyperImageData = vtkSmartPointer<vtkImageData>::New();

  //[3D]===============================================================
  //*.ply *.obj, *.wrl
  if (extension==tr("obj") || extension==tr("ply") || extension==tr("wrl") ) {
    if (!Read3DModels(filename)) return false;
  }
  //[2D]===============================================================
  else if (extension==tr("png") || extension==tr("tif") || extension==tr("tiff") ||
           extension==tr("bmp") || extension==tr("jpg") || extension==tr("jpeg")  ) {
    if (!ReadColorImage(filename)) return false;
  }
  else if (extension==tr("exr")) {
    if (!ReadHDRImage(filename)) return false;
  }
#ifdef SUPPORT_MHA
  else if (extension==tr("mha") || extension==tr("mhd") || extension==tr("spr") ||
           extension==tr("sdt")) {
    if (!ReadDICOM(filename)) return false;
  }
#endif
  //[RTI]===============================================================
#ifdef SUPPORT_RTI
  else if (extension==tr("ptm") || extension==tr("hsh") || extension==tr("rti") ||
           extension==tr("mview")) {
    if (!ReadRTI(filename)) return false;
  }
#endif
  //[DICOM]===============================================================
  else if (fi.isDir()==true) {
    if (!ReadDICOMDir(filename)) return false;
	
  }
  else {
    if (!ReadDICOM(filename)) return false;
  }

  //---------------------------------------------
  // Nikon D70 Measurement
  //          start   	end	  bandwidth	  average wave	 peak
  //  Blue:	    380	  580	      200	         480 	       470
  //  Green:	  390	  700	      310	         545        540
  //  Red:	    480	  750	      270	         615	       590
// space holder for spectral wavelength
  if(mChannelNames.size() <= 4)  // for RGB camera
  {
    mWavelengths.clear();
    mWavelengths.push_back(380); //UV
    mWavelengths.push_back(480); //b
    mWavelengths.push_back(545); //g
    mWavelengths.push_back(615); //r
    mWavelengths.push_back(750); //IR
  }

//  qDebug() << "mWavelengths size: " << mWavelengths.size();

//---------------------------------------------
// QSI 8 channel
//  channel  1 :  half :  B
//  channel  2 :  half :  G
//  channel  3 :  half :  R
//  channel  4 :  half :  w370nm
//  channel  5 :  half :  w417nm
//  channel  6 :  half :  w475nm
//  channel  7 :  half :  w525nm
//  channel  8 :  half :  w585nm
//  channel  9 :  half :  w628nm
//  channel  10 :  half :  w675nm
//  channel  11 :  half :  w755nm

// this messed up volume rendering tonality => Don't use this
//  if (mVtkImageData)
//    ImageShifter(mVtkImageData);
  resetStackControlOnDocks();
  return true;
}

bool VtkWidget::Read3DModels(QString filename)
{
  mIsDICOM = false;

  ReadCHEROb * r3d = new ReadCHEROb;

  if (!(r3d->read3D(filename, mChannelNames, mWavelengths, mRgbTextureFilename, mMaterials, mVtkPolyData, mRgbTexture, mHyperImageData, mIsTextureOn)))
    return false;

  int slashpos = filename.lastIndexOf(QDir::separator());
  QString directory = filename;
  directory.chop(filename.length() - slashpos - 1);

  inputImageSet = new InputImageSet();
  inputImageSet->loadImageSetFromPMVSOutput(directory);

//  qDebug() << "IsTextureOn: " << mIsTextureOn;
//  qDebug() << "mChannelNames size after read3D: " << mChannelNames.size();

    #ifdef _DEBUG
//  vtkSmartPointer<vtkImageData> outputrgb = vtkImageData::SafeDownCast(mRgbTexture->GetInput());
//  mkDebug md;  md.qDebugImageData(outputrgb); // fine
//  vtkSmartPointer<vtkImageData> outputhyper = vtkImageData::SafeDownCast(mHyperImageData);
//      mkDebug md;  md.qDebugImageData(outputrgb, outputhyper); // fine
    //imagedata1:  512   512   1 :  3
    //imagedata1 (10,10,0,1):  19
    #endif

  Rendering3D();
  return true;
}

bool VtkWidget::ReadDICOMDir(QString filename)
{
  // find file data
  QFileInfo fi(filename);
  std::string fnstr = filename.toLocal8Bit().constData(); // QString -> Std. String
  const char *filenamesc = fnstr.c_str();

  mIsDICOM = true;

  //-----------------------------------------------------------
  // DICOM Directory Reading Members
  //-----------------------------------------------------------
  mB3reader = mB3ReaderType::New();
  mB3connector = mB3FilterType::New();
  mGDCMIO = mB3ImageIOType::New();
  inputNames = mB3InputNamesGeneratorType::New();

  mB3reader->GlobalWarningDisplayOff();
  mB3connector->GlobalWarningDisplayOff();
  mGDCMIO->GlobalWarningDisplayOff();
  inputNames->GlobalWarningDisplayOff();
  //-----------------------------------------------------------

  inputNames->SetInputDirectory( filenamesc );
  const mB3ReaderType::FileNamesContainer &filenames = inputNames->GetInputFileNames();

  mB3reader->SetImageIO( mGDCMIO );
  mB3reader->SetFileNames( filenames );
  try
    {
    mB3reader->Update();
    }
  catch (itk::ExceptionObject &excp)
    {
//    qDebug() << "Exception thrown while reading the series: " << excp.GetDescription();
    return false;
    }

  // ITK-VTK connector
  mB3connector->SetInput(mB3reader->GetOutput());
  mB3connector->Update();

  // flipping ITK (DICOM) image for converting ITK coordinates to VTK world coordinates.
  mVtkImageData = vtkImageData::New();
  mVtkImageData = mB3connector->GetOutput();
  mVtkImageData->Update();


  flipITKtoVTKxy(mVtkImageData);
  RenderingStack();
  return true;
}

bool VtkWidget::ReadDICOM(QString filename)
{
  // find file data
  QFileInfo fi(filename);
  QString extension = fi.suffix().toLower();
  std::string fnstr = filename.toLocal8Bit().constData(); // QString -> Std. String
  const char *filenamesc = fnstr.c_str();

  mIsDICOM = true;

  //-----------------------------------------------------------
  // DICOM Single Image Reading Members
  //-----------------------------------------------------------
  // ITK file IO and ITK-VTK connector
  mB3reader = mB3ReaderType::New();
  mB3connector = mB3FilterType::New();
  mGDCMIO = mB3ImageIOType::New();
  inputNames = mB3InputNamesGeneratorType::New();

  mB3reader->GlobalWarningDisplayOff();
  mB3connector->GlobalWarningDisplayOff();
  mGDCMIO->GlobalWarningDisplayOff();
  inputNames->GlobalWarningDisplayOff();
  //-----------------------------------------------------------

  // ITK file reading selection
//  if (extension==tr("mha") || extension==tr("mhd")) {
//    mBreader->SetImageIO( itk::MetaImageIO::New() );
//  }
//  else if (extension==tr("spr") || extension==tr("sdt")) {
//    mBreader->SetImageIO( itk::StimulateImageIO::New() );
//  }
//  else {
//    mBreader->SetImageIO( itk::GDCMImageIO::New() );
//  }
  // ITK: read a single file
  mB3reader->SetImageIO( mGDCMIO );
  mB3reader->SetFileName(filenamesc);
  try
    {
    mB3reader->Update();
    }
  catch (itk::ExceptionObject &excp)
    {
//    qDebug() << "Exception thrown while reading the series: " << excp.GetDescription();
    return false;
    }

  // ITK-VTK connector
  mB3connector->SetInput(mB3reader->GetOutput());
  mB3connector->Update();

  // flipping ITK (DICOM) image for converting ITK coordinates to VTK world coordinates.
  mVtkImageData = vtkImageData::New();
  mVtkImageData = mB3connector->GetOutput();
  mVtkImageData->Update();

  flipITKtoVTKxy(mVtkImageData);

  RenderingStack();
  return true;
}


void VtkWidget::RenderingRTIData()
{
	mIsRTI = true;

	if (mQVTKWidget != NULL) {
		delete mQVTKWidget;
	}

  mQVTKWidget = new QVTKWidget(this);

  if (mLayout!=NULL)
    delete mLayout;
  mLayout = new QVBoxLayout( this ); // size stretching trick
  mLayout->setMargin( 0 );
  mLayout->addWidget( mQVTKWidget, 1 );
  this->setLayout(mLayout);

  if (mVtkImageData==NULL) {
    return;
  }

  // because the 2D stack view requires sophiscated controls,
  // we have a member viewer class. So, if there is an object already,
  // we don't need to re-read anythings.
  mVtkImageViewer = vtkSmartPointer<vtkImageViewer2>::New();

  //int dimss[3];
  //mVtkImageData->GetDimensions(dimss);
  //if (dimss[2] > 1){
  //    mWidgetMode = CTSTACK;
  //    flipITKtoVTKxy(mVtkImageData); // left-right flip bug fix (2013-07-14)
  //}
  //else {
  //    mWidgetMode = IMAGE2D;
  //    flipITKtoVTKy(mVtkImageData); // left-right flip bug fix (2013-07-14)
  //}
  mWidgetMode = RTI2D;
  flipITKtoVTKy(mVtkImageData); // left-right flip bug fix (2013-07-14)
  emit currentWidgetModeChanged(mWidgetMode, getIsDirectionalLight());

//  mkDebug md; md.qDebugImageData(mVtkImageData); // fine
//    imagedata1:  626   832   1 :  3
//    imagedata2:  626   832   1 :  8
//    imagedata1 (10,10,0,1):  53
//    imagedata2 (10,10,0,1):  0.0317993

  //=======================================================================================
  // This part is the crashing point in the deploy mode
  // connect the data to the imageviewer

  mVtkImageViewer->SetInput(mVtkImageData); // we use only mVTKImageData for RGB display

//  vtkSmartPointer<vtkImageData> tempImageData = vtkSmartPointer<vtkImageData>::New();
//  int dimss[3];  mVtkImageData->GetDimensions(dimss);
//  CreateRGBImage(tempImageData, dimss[0], dimss[1]);
//  tempImageData->ShallowCopy(mVtkImageData);
//  tempImageData->Update();

//  mVtkImageViewer->SetInput(tempImageData); // we use only mVTKImageData for RGB display

//   mkDebug md; md.qDebugImageData(mVtkImageViewer->GetInput());
  //=======================================================================================

  // but hyperspectral data is stored in mHyperImageData;
  mVtkImageViewer->UpdateDisplayExtent(); // update ImageViewer2 for hard reset (this is equivalent to Update() in VTK objects)

  // connect the renderer to the window
  mVtkImageViewer->SetRenderWindow(mQVTKWidget->GetRenderWindow());
  //this->GetRenderWindow()->AddRenderer(mVtkImageViewer->GetRenderer());


  //-------------------------------------------------------------------------
  // adding a light
  mRenderer = mVtkImageViewer->GetRenderer(); // get the renderer

  //mQVTKWidget->GetRenderWindow()->AddRenderer(mRenderer);
  //// get the camera
  //mCamera = mRenderer->GetActiveCamera();
  //mCamera->GetPosition(mInitCamPos);
  //mCamera->GetFocalPoint(mInitCamFoc);
  ////  Y vector for camera
  //mCamera->GetViewUp(mInitCamUp);
  //mCamera->SetParallelProjection(FALSE); // set the camera perspective projection

  // adding a light
  mLight1 = vtkSmartPointer<vtkLight>::New();
  mLight1->SetFocalPoint(0,0,0);
  mLight1->SetPosition(mRenderer->GetActiveCamera()->GetPosition());
  mLight1->SetIntensity(mIntensityL1);

  mLight2 = vtkSmartPointer<vtkLight>::New();
  mLight2->SetFocalPoint(0,0,0);
  mLight2->SetPosition(mRenderer->GetActiveCamera()->GetPosition());
  mLight2->SetIntensity(mIntensityL2);
 

  mRenderer->AddLight(mLight1);
  mRenderer->AddLight(mLight2);

  mVtkImageViewer->SetRenderer(mRenderer); // set the renderer back
  //-------------------------------------------------------------------------
  // Picker to pick pixels
//  vtkSmartPointer<vtkPropPicker> propPicker = vtkSmartPointer<vtkPropPicker>::New();
  vtkSmartPointer<vtkCellPicker> propPicker = vtkSmartPointer<vtkCellPicker>::New();
  propPicker->PickFromListOn();
  // Give the picker a prop to pick
  vtkSmartPointer<vtkImageActor> imageActor = mVtkImageViewer->GetImageActor();
  propPicker->AddPickList(imageActor);
  // disable interpolation, so we can see each pixel
  if (this->mIsInterpolateOn) {
	  // qDebug() << "RTI on A" << endl;
    imageActor->InterpolateOn();
  }
  else {
	 // qDebug() << "RTI off B" << endl;
    imageActor->InterpolateOff();
  }
  //-------------------------------------------------------------------------

  // connect the image viewer to the interactor (only for CT)
  // scrolling becomes brightness/contrast controls.
  // Otherwise, the mouse interaction becomes moving object in OpenGL
  mVtkImageViewer->SetupInteractor(mQVTKWidget->GetRenderWindow()->GetInteractor()); // connect imageviewer2 to window interactor

  // connect the customized callback to the interactor
//  if (mIsDICOM) { // for medical images
    // currrently this interactor cannot be supported with CT medical imaging data
    mVtkImageViewer->GetRenderer()->ResetCamera();
//  } else { // for normal images
    // summary the mouse events are as follows:
    // + Left Mouse button triggers window level events
    // + CTRL Left Mouse spins the camera around its view plane normal
    // + SHIFT Left Mouse pans the camera
    // + CTRL SHIFT Left Mouse dollys (a positional zoom) the camera
    // + Middle mouse button pans the camera
    // + Right mouse button dollys the camera.
    // + SHIFT Right Mouse triggers pick events
    //vtkSmartPointer<vtkInteractorStyleImage> style =  vtkSmartPointer<vtkInteractorStyleImage>::New(); // 2D image
	vtkSmartPointer<myVTKInteractorStyle> style =  vtkSmartPointer<myVTKInteractorStyle>::New(); // YY 2D image
  //  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); // 3D image

    vtkSmartPointer<QVTKInteractor> interactor = vtkSmartPointer<QVTKInteractor>::New();
    interactor->SetRenderWindow(mQVTKWidget->GetRenderWindow());
    interactor->GetInteractorStyle()->SetDefaultRenderer(mRenderer);
    interactor->SetInteractorStyle(style);

    //---------------------------------------------------------------------------------------------
    // create new customized callback
    mCallback2D =  vtkSmartPointer<vtk2DInteractionCallback>::New();
    mCallback2D->SetInteractor(interactor); // set the callback to the interactor
    mCallback2D->SetViewer(mVtkImageViewer);
    mCallback2D->SetGLversion(mGLversion);
    mCallback2D->SetNumCore(mNumCore);
    mCallback2D->SetPicker(propPicker);
    QFileInfo fi(mFilename);
    QString simplefn = fi.baseName();
    mCallback2D->SetFilename(simplefn);
    mCallback2D->SetHyperImageData(mHyperImageData); // for hyperspectral data reading
    vtkSmartPointer<vtkCornerAnnotation> annotation = vtkSmartPointer<vtkCornerAnnotation>::New();
    mCallback2D->SetInfoAnnotation(annotation);
    mRenderer->AddViewProp(annotation);
    mCallback2D->SetIsDICOM(mIsDICOM);
    if (mIsDICOM == true)
        mCallback2D->SetWindowLevels();
    mCallback2D->displayInfoAnnotation();
    mCallback2D->SetDisplayInfoOn(mDisplayInfoOn);

    interactor->AddObserver(vtkCommand::MouseMoveEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::LeftButtonPressEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::LeftButtonReleaseEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::MiddleButtonPressEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::MiddleButtonReleaseEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::RightButtonPressEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::RightButtonReleaseEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::MouseWheelForwardEvent, mCallback2D);
    interactor->AddObserver(vtkCommand::MouseWheelBackwardEvent, mCallback2D);
    //---------------------------------------------------------------------------------------------

    // connect VTK signal to QT slot
    mEventQtSlot = vtkSmartPointer<vtkEventQtSlotConnect>::New();
    // get key pressed with high priority (1.0)
    if (mIsDICOM == false)
        mEventQtSlot->Connect(style, vtkCommand::RightButtonPressEvent, this, SLOT( getHyperPixelsSignals(vtkObject*, unsigned long, void*, void*) ) );

    // start the interactor
    interactor->Initialize();

	//emit currentImageChanged(); // update lighting

    // update current view
//    qDebug() << "mOrientationCurrent: " << mOrientationCurrent;
//    qDebug() << "mSliceCurrent: " << mSliceCurrent;

//    cout << "here";
    updateCurrentOrientation(mOrientationCurrent); // update orientation
    updateCurrentSlice(mSliceCurrent);

//  }

  //------------------------------------------------------------------------------------------
  // Create rendering for a single image
  mVtkImageViewer->Render();

  mQVTKWidget->show(); // no Render() is required for 3D
  mQVTKWidget->update(); //MK: this is important!
}

void VtkWidget::ConvertUnsignedCharVecToVTKImageData(int iwidth, int iheight, std::vector<unsigned char> textureData)
{
	// prepare for RTI image rendering

	//***************Method 1*********************works, but slow/
	//int offset = 0;
	//mVtkImageData = vtkImageData::New();
	//mVtkImageData->SetDimensions(iheight,iwidth,  1);
	//mVtkImageData->SetNumberOfScalarComponents(3);
	//mVtkImageData->SetScalarTypeToUnsignedChar();
	//mVtkImageData->AllocateScalars();

	//for (int i = 0; i < iheight; i++) {
	//	for (int j = 0; j < iwidth; j++) {
	//		for (int n = 0; n < 3; n++) {
	//			unsigned char val = textureData[offset];
	//			mVtkImageData->SetScalarComponentFromFloat(i,j,0,n,val); //B
	//			offset += 1;
	//		}
	//		offset += 1;
	//	}
	//}
	/*******************************************/

	///***************Method 2*********************a bug when performing first visualization speed ok/
	//mVtkImageData = vtkImageData::New();
	//mVtkImageData->SetDimensions(iwidth, iheight, 1);
	//mVtkImageData->SetNumberOfScalarComponents(3);
	//mVtkImageData->SetScalarTypeToUnsignedChar();
	//mVtkImageData->AllocateScalars();
	//
	//vtkUnsignedCharArray *scalars = vtkUnsignedCharArray::New();
	//int offset = 0;
	//int count = 0;
	//for (int i = 0; i < iwidth; i++) {
	//	for (int j = 0; j < iheight; j++) {
	//		for (int n = 0; n < 3; n++) {
	//			scalars->InsertValue(count, textureData[offset]);
	//			count += 1;
	//			offset += 1;

	//			if (textureData[offset] == 0 && textureData[offset+1]==0 && textureData[offset+2]==0)
	//				qDebug() << "whywhyw";
	//		}
	//		offset += 1;
	//	}
	//} 
	//mVtkImageData->GetPointData()->SetScalars(scalars);
	//mVtkImageData->Update();
	//mVtkImageData->Modified();
	///***************Method 2*********************/


	///***************Method 3*********************works, with a good visualization speed/
	//mVtkImageData = vtkImageData::New();
	mVtkImageData->SetDimensions(iwidth, iheight, 1);
	mVtkImageData->SetNumberOfScalarComponents(3);
	mVtkImageData->SetScalarTypeToUnsignedChar();
	mVtkImageData->AllocateScalars();

	unsigned char *ptr = static_cast<unsigned char*>(mVtkImageData->GetScalarPointer());
	uchar *rPtr = new uchar[iheight * iwidth];
	uchar *gPtr = new uchar[iheight * iwidth];
	uchar *bPtr = new uchar[iheight * iwidth];
	int offset = 0;
	int count = 0;
	for (int i = 0; i < iwidth; i++) {
		for (int j = 0; j < iheight; j++) {
			for (int n = 0; n < 3; n++) {
				*ptr++ = textureData[offset];

				if (n == 0) { *(rPtr + count) = textureData[offset]; }
				else if (n == 1) { *(gPtr + count) = textureData[offset]; }
				else { *(bPtr + count) = textureData[offset]; }

				offset += 1;
			}
			count += 1;
			offset += 1;
		}
	}

	// save an image //QImage::Format_RGB888 QImage::Format_Indexed8
	QImage qImage(iwidth, iheight, QImage::Format_RGB888);
	count = 0;
	for (int i = 0; i < iheight; i++) {
		for (int j = 0; j < iwidth; j++) { 
			qImage.setPixel(j, i, qRgb(rPtr[count], gPtr[count], bPtr[count])); count += 1; 
		}
	}
	QPixmap pixmap( QPixmap::fromImage(qImage) );
	mRTIScreenShot.convertFromImage(qImage);

	delete []rPtr;
	delete []gPtr;
	delete []bPtr;

	mVtkImageData->Update();
	mVtkImageData->Modified();
	///***************Method 3*********************/

}

bool VtkWidget::ReadRTI(QString filename)
{
	mRTIbrowser = new RtiBrowser(NULL, this);
	mRTIbrowser->setVisible(false);
	connect(mw()->mLightControlRTI, SIGNAL(lightChanged(vcg::Point3f, bool)), mRTIbrowser, SLOT(setLight(vcg::Point3f, bool)));
	connect(mRTIbrowser, SIGNAL(rtiImageChanged(std::vector<unsigned char>, int, int, bool)), this, SLOT(updateRTIImageVTK(std::vector<unsigned char>, int, int, bool)));
	connect(mw()->rendDlg, SIGNAL(renderingModeChanged(int)), mRTIbrowser, SLOT(setRenderingMode(int))); 
	connect(mw()->rendDlg, SIGNAL(updateImage()), mRTIbrowser, SLOT(updateImage())); 
	connect(mRTIbrowser, SIGNAL(setInteractiveLight(bool)), mw()->mLightControlRTI, SLOT(setInteractive(bool)));
	connect(mRTIbrowser, SIGNAL(setEnabledLight(bool)), mw()->mLightControlRTI, SLOT(setEnabled(bool)));
	connect(mRTIbrowser, SIGNAL(rtiImageLoadingDone(QMap<int, RenderingMode*>*, int)), mw()->rendDlg, SLOT(updateRTIRendDlg(QMap<int, RenderingMode*>*, int)));


	// read RTI image
	QFileInfo fi(filename);
	QFile data(filename);
	QString extension = fi.suffix().toLower();
	std::string fnstr = filename.toLocal8Bit().constData(); // QString -> Std. String
	const char *filenamesc = fnstr.c_str();

	if (data.open(QFile::ReadOnly))
	{
		Rti* image = NULL;

		// RTI file reading selection
		if (extension==tr("ptm") ) {
			QTextStream input(&data);
			image = Ptm::getPtm(input);
			data.close();
		}
		else if (extension==tr("hsh") )
			image = new Hsh();
		else if (extension==tr("rti") )
			image = new UniversalRti();
		else if (extension==tr("mview") )
			image = new MultiviewRti();

		if (!image) {
			delete image;
			return false;
		} else {
			image->setFileName(filename);
			int result = 0;
			try
			{
				result = image->load();
			}
			catch (std::bad_alloc&)
			{
				result = -2;
			}

			if (result != 0) {
				delete image;
				return false;
			}
		}
	} else { 
		return false;
	}
	return true;
}


void VtkWidget::RenderingStack()
{
  // MK: let's keep the controlling handle on the CTControl widget.
//  qDebug() << "blend type: " << mBlendType;

  // MK: when we come back from the volume rendering to 2D stack view,
  //     it becomes slower
  switch(mCTVisualization)
  {
  default:
  case STACK:
    Rendering2D();
	mw()->mInformation->draw2DNoteMark(mProjectPath);
	if (getUserAnnotationOn())
	{
		annotate(true, mNoteMode, true);
	}
    break;
  case VOLUMEGPU:
    RenderingVolume(mBlendType, mReductionFactor, mVolumeRenderMode);
	mw()->mInformation->draw3DNoteMark(mProjectPath);
	if (getUserAnnotationOn())
	{
		annotate(true, mNoteMode, true);
	}
    break;
  }
}

bool VtkWidget::ReadHDRImage(QString filename)
{
  ReadCHEROb *rh;
  vtkTexture* texture;

  // new file should be open with mCTVisualization == STACK always
  mCTVisualization = STACK;

  if (!(rh->readEXR(filename, mChannelNames, mWavelengths, texture, mVtkImageData, mHyperImageData)) )
    return false;

  mIsHyperspectral = true;

  //mkDebug md; md.qDebugImageData(mVtkImageData, mHyperImageData); // fine
  //imagedata1:  626   832   1 :  3
  //imagedata2:  626   832   1 :  8
  //imagedata1 (10,10,0,1):  53
  //imagedata2 (10,10,0,1):  0.0317993

  RenderingStack();

  return true;
}

bool VtkWidget::ReadColorImage(QString filename)
{
  // find file data
  QFileInfo fi(filename);
  QString extension = fi.suffix().toLower();
  std::string fnstr = filename.toLocal8Bit().constData(); // QString -> Std. String
  const char *filenamesc = fnstr.c_str();

  //-----------------------------------------------------------
  // A Color Image Reading Members
  //-----------------------------------------------------------
  // ITK file IO and ITK-VTK connector
  mCreader = mCReaderType::New();
  mCconnector = mCFilterType::New();
  //-----------------------------------------------------------

  // new file should be open with mCTVisualization == STACK always
  mCTVisualization = STACK;

  // ITK file reading selection
  if (extension==tr("png")) {
    mCreader->SetImageIO( itk::PNGImageIO::New() );
  }
  else if (extension==tr("jpg") || extension==tr("jpeg")) {
    mCreader->SetImageIO( itk::JPEGImageIO::New() );
  }
  else if (extension==tr("tif") || extension==tr("tiff")) {
    mCreader->SetImageIO( itk::TIFFImageIO::New() );
  }
  else if (extension==tr("bmp")) {
    mCreader->SetImageIO( itk::BMPImageIO::New() );
  }
  else {
    return false;
  }

  // ITK: read a single file
  mCreader->SetFileName(filenamesc);
  try
    {
    mCreader->Update();
    }
  catch (itk::ExceptionObject &excp)
    {
//    qDebug() << "Exception thrown while reading the series: " << excp.GetDescription();
    return false;
    }

  // ITK-VTK connector
  mCconnector->SetInput(mCreader->GetOutput());
  mCconnector->Update();

  if (mVtkImageData != NULL)
      mVtkImageData->Delete();
  else
      mVtkImageData = vtkImageData::New();

  mVtkImageData = mCconnector->GetOutput();
  mVtkImageData->Update();

  mHyperImageData = vtkImageData::New(); // just space holder for 2D RGB

  RenderingStack();
  //------------------------------------------------------------------------------

  return true;
}

// image flip (left-right) bug fix [2013-04-17, Min]
//void VtkWidget::flipITKtoVTKx (vtkImageData* imageData)
//{
//    // [ITK (DICOM) -> VTK] Image flipping (coordinate system conversion)
//    // for image to be displayed correctly, it has to be flipped about the X axis
//    vtkSmartPointer<vtkImageFlip> flipperX = vtkSmartPointer<vtkImageFlip>::New();
//    flipperX->SetFilteredAxis(0);
//    flipperX->SetInput(imageData); //-> already left-right flipped at this stage
//    flipperX->Update();

//  // make a deep copy (not crash in the deploy mode)
//  vtkImageData *imageData2 = vtkImageData::New();
//  imageData2 = flipperX->GetOutput();
//  imageData2->Update();

//  imageData->ShallowCopy(imageData2);
//  imageData->Modified();
//  return;
//}


// image flip (left-right) bug fix [2013-04-17, Min]
void VtkWidget::flipITKtoVTKy (vtkImageData* imageData)
{
    // DT: EXR images do not need to be flipped.
    if(mIsHyperspectral) return;

    // [ITK (DICOM) -> VTK] Image flipping (coordinate system conversion)
    // for image to be displayed correctly, it has to be flipped about the X axis
    vtkSmartPointer<vtkImageFlip> flipperY = vtkSmartPointer<vtkImageFlip>::New();
    flipperY->SetFilteredAxis(1);
    flipperY->SetInput(imageData); //-> already left-right flipped at this stage
    flipperY->Update();

  // make a deep copy (not crash in the deploy mode)
  vtkImageData *imageData2 = vtkImageData::New();
  imageData2 = flipperY->GetOutput();
  imageData2->Update();

  imageData->ShallowCopy(imageData2);
  imageData->Modified();
  return;
}


// image flip (left-right) bug fix [2013-04-17, Min]
void VtkWidget::flipITKtoVTKxy (vtkImageData* imageData)
{
    // [ITK (DICOM) -> VTK] Image flipping (coordinate system conversion)
    // for image to be displayed correctly, it has to be flipped about the X axis
    vtkSmartPointer<vtkImageFlip> flipperX = vtkSmartPointer<vtkImageFlip>::New();
    vtkSmartPointer<vtkImageFlip> flipperY = vtkSmartPointer<vtkImageFlip>::New();
    flipperX->SetFilteredAxis(0);
    flipperY->SetFilteredAxis(1);
    flipperX->SetInput(imageData); //-> already left-right flipped at this stage
    flipperX->Update();
    flipperY->SetInput(flipperX->GetOutput());
    flipperY->Update();

  // make a deep copy (not crash in the deploy mode)
  vtkImageData *imageData2 = vtkImageData::New();
  imageData2 = flipperY->GetOutput();
  imageData2->Update();

  imageData->ShallowCopy(imageData2);
  imageData->Modified();
  return;
}


void VtkWidget::focusInEvent ( QFocusEvent * e )
{
  QWidget::focusInEvent(e);
  emit currentViewerChanged(id);
}

//void VtkWidget::paintEvent(QPaintEvent */*event*/)
// {
//  QPainter painter(this);
//  painter.beginNativePainting();
//  painter.endNativePainting();
//}


void VtkWidget::getHyperPixelsSignals(vtkObject*, unsigned long, void*, void*)
{
  int icoords[3] = {0,0,0};
  if (mCallback2D) {
    mHyperPixels = mCallback2D->GetHyperPixels();
    mCallback2D->GetPoint(icoords);
  }
  if (mCallback3D) {
    mHyperPixels = mCallback3D->GetHyperPixels();
  }
  std::string fname = this->mFilename.toStdString();
  emit currentHyperPixelsChanged(mWavelengths, mHyperPixels, &icoords[0], &fname);
}

void VtkWidget::flattenMesh()
{
    if(mCallback3D->GetInteractor()->GetInteractorStyle()->IsA("vtkInteractorStyleSurfaceWalkerCamera"))
    {
        vtkSmartPointer<vtkInteractorStyleSurfaceWalkerCamera> swCam = vtkInteractorStyleSurfaceWalkerCamera::SafeDownCast(mCallback3D->GetInteractor()->GetInteractorStyle());
        swCam->flattenSubset();
    }
}

double VtkWidget::get2DImageHeight()
{
	if (mWidgetMode != IMAGE2D)
		return -1;
	else
		return mVtkImageData->GetDimensions()[1];
}

double VtkWidget::get2DImageWidth()
{
	if (mWidgetMode != IMAGE2D)
		return -1;
	else
		return mVtkImageData->GetDimensions()[0];
}

void VtkWidget::setFlattenedMesh(vtkPolyData *flatMesh)
{
    mVtkPolyData = flatMesh;
    refreshGeometry3D();
}

void VtkWidget::setNonFlattenedMesh(vtkPolyData *nonFlatMesh)
{
    mVtkPolyData = nonFlatMesh;
    refreshGeometry3D();
}
