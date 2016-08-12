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
#ifndef VTK3DINTERACTIONCALLBACK_HPP
#define VTK3DINTERACTIONCALLBACK_HPP

#include <vtkVectorText.h>
#include <vtkStringArray.h>
#include <vtkLineSource.h>
#include <vtkCornerAnnotation.h>
#include <vtkDataSetMapper.h>
#include <vtkFollower.h>
#include <vtkSmartPointer.h>
#include <QVTKInteractor.h>
#include <vtkAssembly.h>
#include <vtkPolyData.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkCellPicker.h>
#include <vtkLightCollection.h>
#include <vtkMath.h>
#include <vtkLight.h>
#include <vtkMatrix4x4.h>
#include <vtkSelectionNode.h>
#include <vtkSelection.h>
#include <vtkExtractSelection.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPointData.h>
#include <vtkTexture.h>
#include <vtkImageData.h>
#include <vtkDataArray.h>
#include <vtkCamera.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkTransform.h>
#include <vtkCamera.h>
#include <vtkLightActor.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkJPEGReader.h>
#include <QImage>
#include <vtkImageResample.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPolygon.h>
#include <vtkCellArray.h>
#include <vtkInteractorObserver.h>
#include <vtkLandmarkTransform.h>
#include <vtkPlaneCollection.h>
#include <vtkFrustumSource.h>
#include <vtkSelectEnclosedPoints.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkExtractSelectedPolyDataIds.h>
#include <vtkHardwareSelector.h>
#include <vtkRenderedAreaPicker.h>
#include <vtkExtractSelectedFrustum.h>
#include <QtConcurrentRun>

#include <vtkPointHandleRepresentation2D.h>
#include <vtkSeedRepresentation.h>
#include <vtkProperty2D.h>
#include <vtkSeedWidget.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#include <algorithm> 

#include "../information/informationWidget.h"
#include "../mainWindow.h"
#include "../function/mkTools.hpp"
#include "../io/inputimageset.h"


#define GAMMA (2.2f)


//#define SHOWCAMERALIGHT
//#define SHOWHYPERVALUES

#ifndef MAX_LINE
  #define MAX_LINE (256)
#endif

//---------------------------------------------------------------------------------------------


vtkSmartPointer<vtkRenderer> renderer;
vtkSmartPointer<vtkSphereSource> SS1;
vtkSmartPointer<vtkDataSetMapper> sMap;
vtkSmartPointer<vtkPolyData> emptyPD;

#define MY_CREATE_NEW(class, variable)\
  vtkSmartPointer<class> variable = vtkSmartPointer<class>::New();

struct PointMark
{
	vtkSmartPointer<vtkIdTypeArray> cellIds;
	double positions[3];
	vtkSmartPointer<vtkActor> actor;
};

struct SurfaceMark
{
	vtkSmartPointer<vtkSelectionNode> cellIds;
	std::vector<double*> cornerPoints;
	vtkSmartPointer<vtkActor> actor;
};

// for 3D interaction callback.
class vtk3DInteractionCallback : public vtkCommand
{
	//Q_OBJECT
public:

    MainWindow* mw()
    {
      foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
        if (mainwindow)
        {
          return mainwindow;
        }
      }

    }

  static vtk3DInteractionCallback *New() {
    return new vtk3DInteractionCallback;
  }

  // initialization for constructor
  vtk3DInteractionCallback(): XLabel( 0 ), YLabel( 0 ),ZLabel( 0 ),Axes(0)
  {
    mDisplayPolyIndicateOn = false;
    mIsDirectionalLight = true;

    mNumberOfPoints = 0;
    mNumberOfPolys = 0;
    mNumberOfStrips = 0;
    mNumberOfLines = 0;
    mNumberOfVerts = 0;
    mNumberOfCells = 0;
	count = 0;
    mRubberDist = 0.;
	mPointNoteHighlight = -1;
    mSurfaceNoteHighlight = -1;
    mFrustumNoteHighlight = -1;

	mNoteMode = UNDECLARE;
	mColor = YELLOW;

    mInfoAnnotation = NULL;
    mUseRubberband = false;
	mUserIsAnnotating = false;
    mWasReadyRubberband = false; // this is one start point clicked.
	isCTVolume = false;
    mRubberband = vtkSmartPointer<vtkLineSource>::New();

    // for selection
    mSelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mSelectedActor = vtkSmartPointer<vtkActor>::New();

    // for selection
    mSelectedMapper2 = vtkSmartPointer<vtkDataSetMapper>::New();
    mSelectedActor2 = vtkSmartPointer<vtkActor>::New();
	mSurfaceMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mSurfaceSelector = vtkSmartPointer<vtkActor>::New();

    // for text information on mesh
    mFollower = vtkSmartPointer<vtkFollower>::New(); // equivalent to actor
    mTextMapper = vtkSmartPointer<vtkPolyDataMapper>::New();

    // for best image overlay
    //mBestImageActor = vtkSmartPointer<vtkActor2D>::New();
    mBestImageActor = vtkSmartPointer<vtkActor>::New();
    quad = vtkSmartPointer<vtkPolyData>::New();
    jPEGReader = vtkSmartPointer<vtkJPEGReader>::New();
    mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    polygons = vtkSmartPointer<vtkCellArray>::New();
    polygon = vtkSmartPointer<vtkPolygon>::New();
	//mSelectedPoints = vtkSmartPointer<vtkIdTypeArray>::New();
    newTextureLoaded = false;
    currentBestImage = 0;
    imageadded = false;
    checkBestImageRequested = true;
    imageOverlayOn = false; 

    // for picker cell array
//    mPickCellArray = vtkSmartPointer<vtkIdTypeArray>::New();
//    mPickCellArray->SetNumberOfComponents(MAX_PICKCELLARRAY);

    mLightTransform = vtkSmartPointer<vtkTransform>::New();
    mTransform = vtkSmartPointer<vtkTransform>::New();
  }

  void SetPolyData(vtkPolyData *polyData) {
    this->mPolyData = polyData;
  }

  vtkPolyData *GetImageReslice() {
    return this->mPolyData;
  }

  void SetLightTransform (vtkTransform *transform) {
    this->mLightTransform = transform;
  }

  vtkTransform * GetLightTransform () {
    return this->mLightTransform;
  }

  void SetTransform (vtkTransform *transform) {
      this->mTransform = transform;
  }

  void SetCTVolume(bool isVolume) {
	  this->isCTVolume = isVolume;
  }

  void SetDimensions(int* diss)
  {
	  if (isCTVolume)
	  {
		  mDimensions[0] = diss[0];
		  mDimensions[1] = diss[1];
		  mDimensions[2] = diss[2];
	  }
  }

  void LoadNewBestImage(int k)
  {
	  qDebug() << "k = " << k << "currentBestImage" << currentBestImage;
      if(k == currentBestImage && k != 0)
          return;

      currentBestImage = k;
	  qDebug() << "Before RERENDER";
      char *myname = (char*)inputImageSet->imageSet[k]->mFileName.toStdString().c_str();
      qDebug() << "after";
      jPEGReader->SetFileName(myname);
      jPEGReader->Update();
	  
      currentImageWidth = jPEGReader->GetOutput()->GetDimensions()[0];
      currentImageHeight = jPEGReader->GetOutput()->GetDimensions()[1];
      //mBestImageActor->SetTexture(texture);
      newTextureLoaded = true;
      mBestImageActor->SetVisibility(1);
      //GetInteractor()->Render();
      qDebug() << "RERENDER";
  }

  // this function is buggy
  void SetBestImage(int k) {

      int window_w = GetInteractor()->GetRenderWindow()->GetSize()[0];
      int window_h = GetInteractor()->GetRenderWindow()->GetSize()[1];
      double windowAspect = (double)window_w / (double)window_h;

      if(!imageadded)
      {
          vtkSmartPointer<vtkFloatArray> textureCoordinates = vtkSmartPointer<vtkFloatArray>::New();
          textureCoordinates->SetNumberOfComponents(3);
          textureCoordinates->SetName("TextureCoordinates");
          float tuple[3] = {0.0, 0.0, 0.0};
          textureCoordinates->InsertNextTuple(tuple);
          tuple[0] = 1.0; tuple[1] = 0.0; tuple[2] = 0.0;
          textureCoordinates->InsertNextTuple(tuple);
          tuple[0] = 1.0; tuple[1] = 1.0; tuple[2] = 0.0;
          textureCoordinates->InsertNextTuple(tuple);
          tuple[0] = 0.0; tuple[1] = 1.0; tuple[2] = 0.0;
          textureCoordinates->InsertNextTuple(tuple);
          quad->GetPointData()->SetTCoords(textureCoordinates);

          mapper->SetInput(quad);

          polygon->GetPointIds()->SetNumberOfIds(4); //make a quad
          polygon->GetPointIds()->SetId(0, 0);
          polygon->GetPointIds()->SetId(1, 1);
          polygon->GetPointIds()->SetId(2, 2);
          polygon->GetPointIds()->SetId(3, 3);
          polygons->InsertNextCell(polygon);
          quad->SetPolys(polygons);
          mBestImageActor->SetMapper(mapper);
          mBestImageActor->GetProperty()->SetOpacity(1.0);

          imageadded = true;

          vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
          points->InsertNextPoint(0,0,0);
          points->InsertNextPoint(1,0,0);
          points->InsertNextPoint(1,1,0);
          points->InsertNextPoint(0,1,0);
          quad->SetPoints(points);
      }

      //if(k != currentBestImage)
      //  mBestImageActor->SetVisibility(0);
      //QtConcurrent::run(this, &vtk3DInteractionCallback::LoadNewBestImage,k);  // why need multiple threads???
	  qDebug() << "in Set Best Image k = "<<k;
      LoadNewBestImage(k);
       
      double imageAspect = (double)currentImageWidth / (double)currentImageHeight;

      double scale_ratio = 1;

      if(windowAspect > imageAspect)
      {
          int new_height = window_h - 40;
          scale_ratio = (double)new_height / (double)currentImageHeight;
      }
      else
      {
          int new_width = window_w - 40;
          scale_ratio = (double)new_width / (double)currentImageWidth;
      }

      int newWidth = currentImageWidth * scale_ratio;
      int newHeight = currentImageHeight * scale_ratio;

      int x0 = (window_w - newWidth) / 2;
      int y0 = (window_h - newHeight) / 2;
      int x1 = x0 + newWidth;
      int y1 = y0 + newHeight;

      qDebug() << "MOVE IMAGE";
	  
      vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
      picker->SetTolerance(0.0005);
      picker->Pick(window_w/2, window_h/2, 0, GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
      vtkIdType cellID = picker->GetCellId();
      vtkSmartPointer<vtkRenderer> currentRenderer = GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
      if(cellID != -1)
      {
          vtkSmartPointer<vtkIdList> vertexList = vtkSmartPointer<vtkIdList>::New();
          mPolyData->GetCellPoints(cellID,vertexList);
          vtkIdType vID0 = vertexList->GetId(0);
          vtkIdType vID1 = vertexList->GetId(1);
          vtkIdType vID2 = vertexList->GetId(2);

          double v0[3], v1[3], v2[3];
          mPolyData->GetPoints()->GetPoint(vID0,v0);
          mPolyData->GetPoints()->GetPoint(vID1,v1);
          mPolyData->GetPoints()->GetPoint(vID2,v2);

          double v0Disp[3], v1Disp[3], v2Disp[3];
          vtkInteractorObserver::ComputeWorldToDisplay(currentRenderer,v0[0],v0[1],v0[2],v0Disp);
          vtkInteractorObserver::ComputeWorldToDisplay(currentRenderer,v1[0],v1[1],v1[2],v1Disp);
          vtkInteractorObserver::ComputeWorldToDisplay(currentRenderer,v2[0],v2[1],v2[2],v2Disp);
          v0Disp[2] = 0.1; v1Disp[2] = 0.1; v2Disp[2] = 0.1;

          double v0Im[3], v1Im[3], v2Im[3];
          inputImageSet->imageSet[currentBestImage]->projectWorldToImage(v0[0],v0[1],v0[2],v0Im);
          inputImageSet->imageSet[currentBestImage]->projectWorldToImage(v1[0],v1[1],v1[2],v1Im);
          inputImageSet->imageSet[currentBestImage]->projectWorldToImage(v2[0],v2[1],v2[2],v2Im);

          int imWidth = inputImageSet->imageSet[currentBestImage]->imW;
          int imHeight = inputImageSet->imageSet[currentBestImage]->imH;
          double v0Plane[3], v1Plane[3], v2Plane[3];
          v0Plane[0] = (v0Im[0] / imWidth) * (x1 - x0) + x0;    v0Plane[1] = (y1 - y0) - (v0Im[1] / imHeight) * (y1 - y0) + y0;
          v1Plane[0] = (v1Im[0] / imWidth) * (x1 - x0) + x0;    v1Plane[1] = (y1 - y0) - (v1Im[1] / imHeight) * (y1 - y0) + y0;
          v2Plane[0] = (v2Im[0] / imWidth) * (x1 - x0) + x0;    v2Plane[1] = (y1 - y0) - (v2Im[1] / imHeight) * (y1 - y0) + y0;
          v0Plane[2] = 0.1; v1Plane[2] = 0.1; v2Plane[2] = 0.1;

          vtkSmartPointer<vtkLandmarkTransform> landmarkTransform = vtkSmartPointer<vtkLandmarkTransform>::New();
          vtkSmartPointer<vtkPoints> sourcePts = vtkSmartPointer<vtkPoints>::New();
          sourcePts->InsertNextPoint(v0Plane);
          sourcePts->InsertNextPoint(v1Plane);
          sourcePts->InsertNextPoint(v2Plane);
          vtkSmartPointer<vtkPoints> targetPts = vtkSmartPointer<vtkPoints>::New();
          targetPts->InsertNextPoint(v0Disp);
          targetPts->InsertNextPoint(v1Disp);
          targetPts->InsertNextPoint(v2Disp);
          landmarkTransform->SetSourceLandmarks(sourcePts);
          landmarkTransform->SetTargetLandmarks(targetPts);
          landmarkTransform->SetModeToSimilarity();
          landmarkTransform->Update();

          double dBLh[] = {x0,y0,0.1,1.0};
          double dBRh[] = {x1,y0,0.1,1.0};
          double dTRh[] = {x1,y1,0.1,1.0};
          double dTLh[] = {x0,y1,0.1,1.0};
          landmarkTransform->GetMatrix()->MultiplyPoint(dBLh,dBLh);
          landmarkTransform->GetMatrix()->MultiplyPoint(dBRh,dBRh);
          landmarkTransform->GetMatrix()->MultiplyPoint(dTRh,dTRh);
          landmarkTransform->GetMatrix()->MultiplyPoint(dTLh,dTLh);

          vtkInteractorObserver::ComputeDisplayToWorld(currentRenderer,dBLh[0],dBLh[1],dBLh[2],wBL);
          vtkInteractorObserver::ComputeDisplayToWorld(currentRenderer,dBRh[0],dBRh[1],dBRh[2],wBR);
          vtkInteractorObserver::ComputeDisplayToWorld(currentRenderer,dTRh[0],dTRh[1],dTRh[2],wTR);
          vtkInteractorObserver::ComputeDisplayToWorld(currentRenderer,dTLh[0],dTLh[1],dTLh[2],wTL);
      }
      else
      {
          vtkInteractorObserver::ComputeDisplayToWorld(currentRenderer,x0,y0,0.1,wBL);
          vtkInteractorObserver::ComputeDisplayToWorld(currentRenderer,x1,y0,0.1,wBR);
          vtkInteractorObserver::ComputeDisplayToWorld(currentRenderer,x1,y1,0.1,wTR);
          vtkInteractorObserver::ComputeDisplayToWorld(currentRenderer,x0,y1,0.1,wTL);
      }

        // Create a plane
        vtkPoints *points = quad->GetPoints();
        points->SetPoint(0, wBL[0], wBL[1], wBL[2]);
        points->SetPoint(1, wBR[0], wBR[1], wBR[2]);
        points->SetPoint(2, wTR[0], wTR[1], wTR[2]);
        points->SetPoint(3, wTL[0], wTL[1], wTL[2]);
        points->Modified();

        if(newTextureLoaded)
        {
            vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
            texture->SetInputConnection(jPEGReader->GetOutputPort());
            mBestImageActor->SetTexture(texture);
            reRenderBestImage = true;
        }

        if(reRenderBestImage)
        {
            reRenderBestImage = false;
            GetInteractor()->Render();
        }
  }


  void SetModelDetail (int numberOfPoints, int numberOfPolys, int numberOfStrips, int numberOfLines, int numberOfVerts, int numberOfCells)
  {
    mNumberOfPoints = numberOfPoints;
    mNumberOfPolys = numberOfPolys;
    mNumberOfStrips = numberOfStrips;
    mNumberOfLines = numberOfLines;
    mNumberOfVerts = numberOfVerts;
    mNumberOfCells = numberOfCells;
  }

  void SetRgbTexture(vtkTexture* texture)
  {
    this->mRgbTexture = texture;
  }

  void SetAnnotationColor(ColorType color)
  {
	  //turnoffHighlight();
	  this->mColor = color;
	  mw()->VTKA()->update();
  }

  void SetAnnotation(bool status, NoteMode NoteMode, ColorType color = YELLOW, bool visibilityOn = true)
  {
	  //turnoffHighlight();
      this->mUserIsAnnotating = status;
	  this->mNoteMode = NoteMode;
	  this->mColor = color;
	  if (mUserIsAnnotating)
	  {
		  mw()->mInformation->startAnnotation();
		 initNote();
		 if (visibilityOn)
		 {
			 for (int i = 0; i < mSelectedFrustum.size(); i++)
			 {
				 mSelectedFrustum[i].second->VisibilityOn();
			 }
			 for (int i = 0; i < mSelectedPoints.size(); i++)
			 {
				 mSelectedPoints[i].actor->VisibilityOn();
			 }
			 for (int i = 0; i < mSelectedSurface.size(); i++)
			 {
				 mSelectedSurface[i].actor->VisibilityOn();
			 }
		 }
         mSelectedActor2->VisibilityOn();
		 mSurfaceSelector->VisibilityOn();
		 mw()->VTKA()->update();
	  }
	  else
	  {
		  mw()->mInformation->finishAnnotation();
		  mFollower->VisibilityOff();
          mSelectedActor2->VisibilityOff();
		  mSurfaceSelector->VisibilityOff();
		  for (int i = 0; i < mSelectedPoints.size(); i++)
		  {
			 mSelectedPoints[i].actor->VisibilityOff();
		  }
		  for (int i = 0; i < mSelectedFrustum.size(); i++)
		  {
			  mSelectedFrustum[i].second->VisibilityOff();
		  }
		  for (int i = 0; i < mSelectedSurface.size(); i++)
		 {
			 mSelectedSurface[i].actor->VisibilityOff();
		 }
	  }
  }

  void SetUseRubberband(bool status)
  {
    //qDebug() << "swithing use rubberband: " << status;
    mWasReadyRubberband = false; // new
    this->mUseRubberband = status;
  }

  void SetVisibilityRubberband(bool status)
  {
    mWasReadyRubberband = false;
    //qDebug() << "swithing use rubberband: " << status;
    this->mIsRubberbandVisible = status;
      if (status == true) {
          mFollower->VisibilityOn();
          mSelectedActor2->VisibilityOn();
      } else {
          mFollower->VisibilityOff();
          mSelectedActor2->VisibilityOff();
      }

  }

  void SetHyperImageData(vtkImageData* hypertexture)
  {
    this->mHyperImageData = hypertexture;
  }

  void SetIsDirectLight(bool input) {
    this->mIsDirectionalLight = input;
    GetInteractor()->Render();
  }

  void SetFilename(QString filename) {
    this->mFilename = filename;
  }

  void setDisplayPolyIndicateOn(bool input)
  {
    this->mDisplayPolyIndicateOn = input;
    if (mDisplayPolyIndicateOn)
      mSelectedActor->VisibilityOn();
    else
      mSelectedActor->VisibilityOff();
  }

  void SetDisplayInfoOn(bool displayon)
  {
    this->mDisplayInfoOn = displayon;
    if (mDisplayInfoOn && mInfoAnnotation)
    {
      mInfoAnnotation->VisibilityOn();
      mFollower->VisibilityOn();
      mSelectedActor->VisibilityOn();
      mSelectedActor2->VisibilityOn();
    } else {
      mInfoAnnotation->VisibilityOff();
      mFollower->VisibilityOff();
      mSelectedActor->VisibilityOff();
      mSelectedActor2->VisibilityOff();
    }
  }

  void toggleImageProvenanceFeature() {
      imageOverlayOn = !imageOverlayOn;
	  
      if(imageOverlayOn)
      {
          GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(mBestImageActor);
      }
      else
      {
          GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->RemoveActor(mBestImageActor);
      }
      GetInteractor()->Render();
  }

  void SetTextureFilename (QString filename) {
    this->mRgbTextureFilename = filename;
  }

  void SetGLversion (QString version) {
    this->mGLversion = version;
  }

  void SetNumCore (QString version) {
    this->mNumCore = version;
  }

  vtkTransform * GetTransform () {
    return this->mTransform;
  }

  void SetInteractor(QVTKInteractor *interactor) {
    this->mInteractor = interactor;
//	this->mDefaultInteractorStyle = interactor->GetInteractorStyle();
  }

  QVTKInteractor *GetInteractor() {
    return this->mInteractor;
  }

  void displayAxes() {
    if (this->Axes) {
    this->Axes->InitPathTraversal();
    vtkAssemblyPath *path = 0;
    int count = 0;
    vtkFollower* followers[3] = { this->XLabel, this->YLabel, this->ZLabel };
    int followerId = 0;
    while( (path = this->Axes->GetNextPath()) != NULL )
      {
      if( count++ > 2 )
        {
        vtkProp3D *prop3D = static_cast<vtkProp3D *>(path->GetLastNode()->GetViewProp());
        if ( prop3D )
          {
          prop3D->PokeMatrix(path->GetLastNode()->GetMatrix());
          followers[followerId]->SetPosition(prop3D->GetCenter());
          followerId++;
          prop3D->PokeMatrix(NULL);
          }
        }
      }
    }
  }

  void setCameraAzimuth(double angle) {
    vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
    vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

    // get the camera transform matrix
    vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
    camera->Azimuth(angle);
    updateLightingPosition();
  }

  void updateLightingPosition() {
    vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
    vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

    // get the camera transform matrix
    vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();

    double cpos[3], cfocal[3];
    double camX[3], camY[3], camZ[3]; // camera Vector_xyz
    camera->GetPosition(cpos);
    camera->GetFocalPoint(cfocal);

    //  Y vector for camera
    camera->GetViewUp(camY);

    // Z vector for camera
    vtkMath::Subtract(cpos,cfocal,camZ);
    vtkMath::Normalize(camZ);

    // X vector for camera (cross-product of Z and Y)
    vtkMath::Cross(camY, camZ, camX);
    // make a transform matrix for camera normal rotate the camera at the focal point
    vtkSmartPointer<vtkMatrix4x4> cmatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    cmatrix->SetElement(0,0,camX[0]);
    cmatrix->SetElement(1,0,camX[1]);
    cmatrix->SetElement(2,0,camX[2]);
    cmatrix->SetElement(3,0,0);
    cmatrix->SetElement(0,1,camY[0]);
    cmatrix->SetElement(1,1,camY[1]);
    cmatrix->SetElement(2,1,camY[2]);
    cmatrix->SetElement(3,1,0);
    cmatrix->SetElement(0,2,camZ[0]);
    cmatrix->SetElement(1,2,camZ[1]);
    cmatrix->SetElement(2,2,camZ[2]);
    cmatrix->SetElement(3,2,0);
    cmatrix->SetElement(0,3,cfocal[0]);
    cmatrix->SetElement(1,3,cfocal[1]);
    cmatrix->SetElement(2,3,cfocal[2]);
    cmatrix->SetElement(3,3,1);
    // get the light transform matrix
    vtkSmartPointer<vtkLightCollection> lights = renderer->GetLights();
    lights->InitTraversal();
    vtkSmartPointer<vtkLight> light1 = lights->GetNextItem();
    vtkSmartPointer<vtkMatrix4x4> lmatrix = mLightTransform->GetMatrix();
    vtkSmartPointer<vtkTransform> trans = vtkSmartPointer<vtkTransform>::New();
    //================================================================
    // apply the camera transform T_c and the light transform T_l (T = T_c * T_l) in order to update the camera up vector transform
    trans->SetMatrix(cmatrix); // apply camera vector transform

    trans->Concatenate(lmatrix); // apply light vector transform
    //================================================================
    light1->SetTransformMatrix(trans->GetMatrix());
//    vtkSmartPointer<vtkLight> light2 = lights->GetNextItem();
//    light2->SetPosition(camera->GetPosition()); // correct

#ifdef SHOWCAMERALIGHT
    displayInfoAnnotation(true);
#endif
  }

  void displaySelectPoly()
  {
    // get the event current position
    vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
    int lastPos[2];
    interactor->GetLastEventPosition(lastPos);
    int currPos[2];
    interactor->GetEventPosition(currPos);
    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance(0.0005);
    // Pick from this location. (screen location)
    picker->Pick(currPos[0], currPos[1], 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

    if(picker->GetCellId() != -1)
    {
      vtkSmartPointer<vtkIdTypeArray> ids = vtkSmartPointer<vtkIdTypeArray>::New();
      ids->SetNumberOfComponents(1);
      ids->InsertNextValue(picker->GetCellId());

      vtkSmartPointer<vtkSelectionNode> selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
      selectionNode->SetFieldType(vtkSelectionNode::CELL);
      selectionNode->SetContentType(vtkSelectionNode::INDICES);
      selectionNode->SetSelectionList(ids);

      vtkSmartPointer<vtkSelection> selection = vtkSmartPointer<vtkSelection>::New();
      selection->AddNode(selectionNode);

      vtkSmartPointer<vtkExtractSelection> extractSelection = vtkSmartPointer<vtkExtractSelection>::New();
      extractSelection->SetInput(0, this->mPolyData);
      extractSelection->SetInput(1, selection);

      extractSelection->Update();

      // In selection
      vtkSmartPointer<vtkUnstructuredGrid> selected = vtkSmartPointer<vtkUnstructuredGrid>::New();
      selected->ShallowCopy(extractSelection->GetOutput());

      mSelectedMapper->SetInputConnection(selected->GetProducerPort());

      mSelectedActor->SetMapper(mSelectedMapper);
      mSelectedActor->GetProperty()->EdgeVisibilityOn();
      mSelectedActor->GetProperty()->SetEdgeColor(1,0,0);
      mSelectedActor->GetProperty()->SetLineWidth(10);

      if (mDisplayInfoOn)
        mSelectedActor->VisibilityOn();
      else
        mSelectedActor->VisibilityOff();
    }
  }

  void SetInfoAnnotation (vtkCornerAnnotation *annotation) {
    this->mInfoAnnotation = annotation;

    mInfoAnnotation->SetLinearFontScaleFactor(2);
    mInfoAnnotation->SetNonlinearFontScaleFactor(1);
    mInfoAnnotation->SetMaximumFontSize(14); // decide the font size

    vtkSmartPointer<vtkTextProperty> tproperty = mInfoAnnotation->GetTextProperty();
    tproperty->SetFontFamilyToArial();
  //  tproperty->SetFontSize(12); // this doesn't work here
    tproperty->BoldOn();
    tproperty->ShadowOn();
    tproperty->SetColor(1, 1, 1);
    mInfoAnnotation->SetTextProperty(tproperty);
  }

  std::vector<float> GetHyperPixels() {
    return this->mHyperPixels;
  }

  void GetPixelValues(std::string &scoords, std::string &svalues, std::string &shvalues)
  {
    char text[MAX_LINE]; memset( text, 0, sizeof(text) );
    scoords = "";
    svalues = "( ";
    shvalues = "( ";

    // get the event current position
    vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
    int lastPos[2];
    interactor->GetLastEventPosition(lastPos);
    int currPos[2];
    interactor->GetEventPosition(currPos);
    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();

    picker->PickTextureDataOn(); // default is off

    picker->SetTolerance(0.0005);

    int icoords[3] = {0,0,0};
    // Pick from this location. (screen location)

    picker->Pick(currPos[0], currPos[1], 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

    //------------------------------------------------------------------------
    // (1) find texture coordinate from pointdata
    picker->GetPointIJK(icoords); // Reading the texture coordinates.
    sprintf( text, "( %i, %i )", icoords[0], icoords[1] );
    scoords = text;

    // (2) then read the pixel value from rgbtexture and hypertexture.

    vtkSmartPointer<vtkImageData> imagergb = NULL;
    if (mRgbTexture) imagergb = mRgbTexture->GetInput();

    if (imagergb != NULL)
    {
      int components = imagergb->GetNumberOfScalarComponents();
      for (int c = 0; c < components; ++c)
        {
          float value = imagergb->GetScalarComponentAsFloat( icoords[0], icoords[1], icoords[2], c);
          svalues += vtkVariant(value).ToString(); // type: float
          if (c != (components - 1))
            {
            svalues += ", ";
            }
        }
    }
    svalues += " )";

//    sprintf( text, " [Num Ch.: %i ]", components );
//    svalues += text;

    // (3) then read the pixel value from rgbtexture and hypertexture.
    int components2 = 0;
    int hdims[3];

    if (mHyperImageData) mHyperImageData->GetDimensions(hdims);
    if (hdims[0] != 0 || hdims[1] != 0 ) // hyperspectral data
    {
      components2 = mHyperImageData->GetNumberOfScalarComponents();
      std::vector<float> hyperPixels;
      //hyperPixels.push_back(0);
      for (int c = 0; c < components2; ++c)
        {
          float value = mHyperImageData->GetScalarComponentAsFloat( icoords[0], icoords[1], icoords[2], c);
          hyperPixels.push_back(value);
          shvalues += vtkVariant(value).ToString(); // type: float
          if (c != (components2 - 1))
            {
            shvalues += ", ";
            }
        }
      //hyperPixels.push_back(0);
      shvalues += " )";
      sprintf( text, " [Num Ch.: %i ]", components2  );
      shvalues += text;
      // vector copy
      if (hyperPixels.size() > 0)
      {
        mHyperPixels.resize(hyperPixels.size());
        std::copy(hyperPixels.begin(), hyperPixels.end(), mHyperPixels.begin());
      }
    } else { // only RGB data -> need to be flipped
      if (imagergb != NULL) {
        components2 = imagergb->GetNumberOfScalarComponents();
        std::vector<float> hyperPixels;
        hyperPixels.push_back(0); // put zero at the end of spectral range (only for RGB)
        for (int c = components2-1; c >= 0 ; c--)
          {
            float value = imagergb->GetScalarComponentAsFloat( icoords[0], icoords[1], icoords[2], c);
            value = pow(value/255.f,GAMMA);
            hyperPixels.push_back(value);
            shvalues += vtkVariant(value).ToString(); // type: float
            if (c != 0)
              {
              shvalues += ", ";
              }
          }
        hyperPixels.push_back(0);
        shvalues += " )";
        sprintf( text, " [Num Ch.: %i ]", components2 );
        shvalues += text;
        // vector copy
        if (hyperPixels.size() > 0)
        {
          mHyperPixels.resize(hyperPixels.size());
          std::copy(hyperPixels.begin(), hyperPixels.end(), mHyperPixels.begin());
        }
      }
    }
  }

  void displayInfoAnnotation(bool start = false)
  {
    vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
    vtkSmartPointer<vtkRenderer> renderer = NULL;
    vtkSmartPointer<vtkCamera> camera = NULL;
    vtkSmartPointer<vtkCellPicker> picker = NULL;
    vtkSmartPointer<vtkLightCollection> lights = NULL;
    vtkSmartPointer<vtkImageData> texture = NULL;
    if (mRgbTexture) texture = mRgbTexture->GetInput();

    int lastPos[2];
    int currPos[2];
    double worldPosition[3] = {0., 0., 0.}; // initialize bug fix by Min [2013-05-13]
    double picknormal[3] = {0., 0., 0.}; // initialize bug fix by Min [2013-05-13]

    if ( start )
    {
      renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
      camera = renderer->GetActiveCamera();
      //--------------------------------------------------------------------------
      // Get the location of the click (in window coordinates)
      interactor->GetLastEventPosition(lastPos); // not working
      interactor->GetEventPosition(currPos);
      // get window dimension
  //    int* dimens = interactor->GetRenderWindow()->Getstart()->GetFirstRenderer()->GetSize();

      picker = vtkSmartPointer<vtkCellPicker>::New();
      picker->SetTolerance(0.0005);

      // Pick from this location. (screen location)
      picker->Pick(currPos[0], currPos[1], 0, renderer);

      if (picker->GetCellId() != -1)
      {
          double *temp;
          temp = picker->GetPickPosition();
          if (temp != NULL)
          {
              worldPosition[0] = temp[0];
              worldPosition[1] = temp[1];
              worldPosition[2] = temp[2];
          }
      }

      if (picker->GetCellId() != -1)
      {
          double *temp;
          temp = picker->GetPickNormal();
          if (temp != NULL)
          {
              picknormal[0] = temp[0];
              picknormal[1] = temp[1];
              picknormal[2] = temp[2];
          }
      }
    } // end of if

    //----------------------------------------------------------------------------
    char text[MAX_LINE]; memset( text, 0, sizeof(text) );

    // not correct for CT data
    std::string messageUL = "\n<window>    \n<level>    ";
    std::string messageUR = "\n    ";
    messageUR += mGLversion.toStdString();
    messageUR += mNumCore.toStdString();

    std::string messageLR = "";
    std::string messageLL = "";

    // DT: commented out now that we have the File Info dialog
    /*
    messageLL += "    Geometry Filename: ";
    messageLL += mFilename.toLocal8Bit().constData();
    messageLL += "\n    Texture Filename: ";
    messageLL += mRgbTextureFilename.toLocal8Bit().constData();
    messageLL += "\n    Texture File Resolution: ";
      int hdims[3] = {0,0,0};
      int ldims[3] = {0,0,0};
      if (mRgbTexture != NULL)
      {
        mHyperImageData->GetDimensions(hdims);
        hdims[2] = mHyperImageData->GetNumberOfScalarComponents();
        mRgbTexture->GetInput()->GetDimensions(ldims);
        ldims[2] = mRgbTexture->GetInput()->GetNumberOfScalarComponents();
      }
      if (hdims[0] != 0 || hdims[1] != 0 ) // hyperspectral data
      {
        sprintf( text, "(%i x %i x %i)", hdims[0], hdims[1], hdims[2] );
      } else
      {
        sprintf( text, "(%i x %i x %i)", ldims[0], ldims[1], ldims[2] );
      }
      messageLL += text;
    messageLL += "\n    Model Info: ";
//    sprintf( text, "(Points) %i, (Polys) %i, (Cells) %i", mNumberOfPoints, mNumberOfPolys, mNumberOfCells);
    sprintf( text, "(Points) %i, (Polys) %i", mNumberOfPoints, mNumberOfCells);
      messageLL += text;*/

    messageLL += "\n\n\n\n\n    Polygon ID: ";
    if (start) {
      sprintf( text, "(%i)", picker->GetCellId() );
      messageLL += text;
    }
    messageLL += "\n    Polygon Normal: ";
    if (start) {
      sprintf( text, "(%5.5f,  %5.5f,  %5.5f)", picknormal[0], picknormal[1], picknormal[2] );
      messageLL += text;
    }
    messageLL += "\n    Screen Coordinates: ";
    if (start) {
      sprintf( text, "(%3i,  %3i)", currPos[0], currPos[1]); // vtk original coordinates
      messageLL += text;
    }
    // the worldPosition values goes to strange, when it's out of the range.
    messageLL += "\n    World Coordinates: ";
    if (start) {
      sprintf( text, "(%5.5f,  %5.5f,  %5.5f)", worldPosition[0], worldPosition[1], worldPosition[2] );
      messageLL += text;
    }
    messageLL += "\n    Measured Distance: ";
    if (start) {
      double temp = mRubberDist/10.;
      sprintf( text, "(%5.2fcm)", temp );
      messageLL += text;
    }
    std::string scoords = "";
    std::string svalues = "";
    std::string shvalues = "";
    messageLL += "\n    Texture Coordinates: ";
    if (start) {
      if (texture != NULL) {
        GetPixelValues(scoords,svalues,shvalues);
      }
      messageLL += scoords;
    }

    messageLL += "\n    Screen Pixel Values: ";
    if (start) {
      messageLL += svalues;
    }
#ifdef SHOWHYPERVALUES
    messageLL += "\n    Hyperspectral Texture Values: ";
    if (start) {
      messageLL += shvalues;
    }
#endif

#ifdef SHOWCAMERALIGHT
    messageLL += "\n    Light1 Position: ";
    if (start) {
      lights = renderer->GetLights();
      lights->InitTraversal();
      vtkSmartPointer<vtkLight> light1  = lights->GetNextItem();
      double lpos1[3], int1;
      double zero[3] = {0,0,0};
      light1->GetTransformedPosition(lpos1);
      int1 = light1->GetIntensity();
      sprintf( text, "(%5.5f,  %5.5f,  %5.5f) (I: %1.2f)", lpos1[0], lpos1[1], lpos1[2], int1);
      messageLL += text;
    }
    messageLL += "\n    Light2 Position: ";
    if (start) {
      vtkSmartPointer<vtkLight> light2  = lights->GetNextItem();
      double lpos2[3], int2;
      light2->GetTransformedPosition(lpos2);
      int2 = light2->GetIntensity();
      sprintf( text, "(%5.5f,  %5.5f,  %5.5f) (I: %1.2f)", lpos2[0], lpos2[1], lpos2[2], int2);
      messageLL += text;
    }
    messageLL += "\n    Camera Position: ";
    if (start) {
      double *cpos;
      cpos = camera->GetPosition();
      sprintf( text, "(%5.5f,  %5.5f,  %5.5f)", cpos[0], cpos[1], cpos[2]);
      messageLL += text;
    }
    messageLL += "\n    Camera Dist: ";
    if (start) {
      double dist;
      dist = camera->GetDistance();
      sprintf( text, "(%5.5f)", dist);
      messageLL += text;
    }
#endif
    messageLL += "\n";

    if (mInfoAnnotation)
    {
      mInfoAnnotation->SetText(0, messageLL.c_str()); // lower left
      mInfoAnnotation->SetText(1, messageLR.c_str()); // lower right
      //mInfoAnnotation->SetText(2, messageUR.c_str()); // upper left
//      mInfoAnnotation->SetText(3, messageUL.c_str()); // upper right
    }
  }

  void initNote()
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
      vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

	  if (mNoteMode == SURFACENOTE || mNoteMode == FRUSTUMNOTE)
	  {
		  vtkSmartPointer<vtkInteractorStyleRubberBandPick> style 
			  = vtkSmartPointer<vtkInteractorStyleRubberBandPick>::New();
		  mAreaPicker = vtkSmartPointer<vtkRenderedAreaPicker>::New();

		  interactor->SetInteractorStyle(style);
		  style->SetCurrentRenderer(renderer);
		  interactor->SetPicker(mAreaPicker);
	  }
	  else
	  {
		  interactor->SetInteractorStyle(mViewStyle);
	  }
  }	  

  void displayPointNote(vtkSmartPointer<vtkDataSetMapper> mapper, vtkSmartPointer<vtkIdTypeArray> point, const double* position)
  {
	  if (!isCTVolume)
	  {
		  vtkSmartPointer<vtkSelectionNode> selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
		  selectionNode->SetFieldType(vtkSelectionNode::CELL);
		  selectionNode->SetContentType(vtkSelectionNode::INDICES);
		  selectionNode->SetSelectionList(point);

		  vtkSmartPointer<vtkSelection> selection = vtkSmartPointer<vtkSelection>::New();
		  selection->AddNode(selectionNode);

		  vtkSmartPointer<vtkExtractSelection> extractSelection = vtkSmartPointer<vtkExtractSelection>::New();
		  extractSelection->SetInput(0, this->mPolyData);
		  extractSelection->SetInput(1, selection);

		  extractSelection->Update();

		  // In selection
		  vtkSmartPointer<vtkUnstructuredGrid> selected = vtkSmartPointer<vtkUnstructuredGrid>::New();
		  selected->ShallowCopy(extractSelection->GetOutput());
		  mapper->SetInputConnection(selected->GetProducerPort());
	  }
	  else
	  {
		  double p0[3] = {position[0], position[1], position[2]};
		  double p1[3] = {position[0], position[1] - mDimensions[1]/50, position[2]};
		  double p2[3] = {position[0], position[1] + mDimensions[1]/50, position[2]};
		  double p3[3] = {position[0] - mDimensions[0]/50, position[1], position[2]};
		  double p4[3] = {position[0] + mDimensions[0]/50, position[1], position[2]};
		  double p5[3] = {position[0], position[1], position[2] - mDimensions[2]/50};
		  double p6[3] = {position[0], position[1], position[2] + mDimensions[2]/50};
		  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

		  points->InsertNextPoint(p0);
		  points->InsertNextPoint(p1);
		  points->InsertNextPoint(p2);
		  points->InsertNextPoint(p3);
		  points->InsertNextPoint(p4);
		  points->InsertNextPoint(p5);
		  points->InsertNextPoint(p6);

		  vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
		  polyLine->GetPointIds()->SetNumberOfIds(12);
		  polyLine->GetPointIds()->SetId(0,0);
		  polyLine->GetPointIds()->SetId(1,1);
		  polyLine->GetPointIds()->SetId(2,0);
		  polyLine->GetPointIds()->SetId(3,2);
		  polyLine->GetPointIds()->SetId(4,0);
		  polyLine->GetPointIds()->SetId(5,3);
		  polyLine->GetPointIds()->SetId(6,0);
		  polyLine->GetPointIds()->SetId(7,4);
		  polyLine->GetPointIds()->SetId(8,0);
		  polyLine->GetPointIds()->SetId(9,5);
		  polyLine->GetPointIds()->SetId(10,0);
		  polyLine->GetPointIds()->SetId(11,6);


		  vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
		  cells->InsertNextCell(polyLine);

		  vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
		  polyData->SetPoints(points);
		  polyData->SetLines(cells);
		  mapper->SetInput(polyData);
	  }
  }
  
  void displaySurfaceNote(vtkSmartPointer<vtkDataSetMapper> mapper, vtkSmartPointer<vtkSelectionNode> points, QVector<double*> conrnerPoints)
  {
	  if (!isCTVolume)
	  {
		  vtkSmartPointer<vtkSelectionNode> selectedNodes = vtkSmartPointer<vtkSelectionNode>::New();
		  selectedNodes->DeepCopy(points);

		  vtkSmartPointer<vtkExtractSelectedPolyDataIds> extr = vtkSmartPointer<vtkExtractSelectedPolyDataIds>::New();
		  if (selectedNodes)
		  {
			  extr->SetInput(0, this->mPolyData);
			  vtkSmartPointer<vtkSelection> selection = vtkSmartPointer<vtkSelection>::New();
			  selection->AddNode(selectedNodes);
			  extr->SetInput(1, selection);
			  extr->Update();
			  mapper->SetInput(extr->GetOutput());
		  }
	  }
	  else
	  {
		  vtkSmartPointer<vtkPoints> corners = vtkSmartPointer<vtkPoints>::New();

		  corners->InsertNextPoint(conrnerPoints[0]);
		  corners->InsertNextPoint(conrnerPoints[1]);
		  corners->InsertNextPoint(conrnerPoints[2]);
		  corners->InsertNextPoint(conrnerPoints[3]);

		  vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
		  polyLine->GetPointIds()->SetNumberOfIds(8);
		  polyLine->GetPointIds()->SetId(0,0);
		  polyLine->GetPointIds()->SetId(1,1);
		  polyLine->GetPointIds()->SetId(2,1);
		  polyLine->GetPointIds()->SetId(3,2);
		  polyLine->GetPointIds()->SetId(4,2);
		  polyLine->GetPointIds()->SetId(5,3);
		  polyLine->GetPointIds()->SetId(6,3);
		  polyLine->GetPointIds()->SetId(7,0);

		  vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
		  cells->InsertNextCell(polyLine);

		  vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
		  polyData->SetPoints(corners);
		  polyData->SetLines(cells);
		  mapper->SetInput(polyData);
	  }
  }

  bool displayFrustumNote(vtkSmartPointer<vtkDataSetMapper> mapper, vtkSmartPointer<vtkPlanes> planes)
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
	  vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

	  vtkSmartPointer<vtkExtractSelectedFrustum> extractor = vtkSmartPointer<vtkExtractSelectedFrustum>::New();
	  extractor->SetInput(this->mPolyData);
	  extractor->PreserveTopologyOff();
	  extractor->SetFrustum(planes);
	  extractor->Update();

	  vtkDataSet* polyData  = vtkDataSet::SafeDownCast(extractor->GetOutput());
	  qDebug()<<"selected points"<<polyData->GetNumberOfPoints();
	  mapper->SetInput(vtkDataSet::SafeDownCast(extractor->GetOutput()));
	  if (!mapper->GetInput()->GetNumberOfCells() && !mapper->GetInput()->GetNumberOfPoints())
		  return false;
	  return true;
  }


  void displayPointNoteInfo(int cellId, double* pos)
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
      vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
	  char text1[512]; memset( text1, 0, sizeof(text1) );
	  sprintf( text1, "Polygon ID (%d) World Coordinates: [%5.5f,  %5.5f,  %5.5f]",cellId, pos[0], pos[1], pos[2] );

	  vtkSmartPointer<vtkVectorText> vtext = vtkSmartPointer<vtkVectorText>::New();

      vtext->SetText(text1);
      mTextMapper->SetInputConnection( vtext->GetOutputPort() );
      mFollower->SetMapper( mTextMapper );
      mFollower->SetScale( 0.001 );
      mFollower->SetTexture( mRgbTexture );

      vtkSmartPointer<vtkProperty> property = mFollower->GetProperty();
      property->SetColor(1,0.75,0); // sphere color orange
      property->SetAmbient(1.0);

      mFollower->SetCamera(renderer->GetActiveCamera());
      //picker->GetPickPosition( pos );
      mFollower->SetPosition( pos ); //((vtkProp3D*)collection->GetItemAsObject( 3 ))->GetCenter());
      mFollower->PickableOff();
      renderer->AddActor( mFollower );
      mFollower->VisibilityOn();
  }

  void highlightPointNote(int id)
  {
	  double* color = new double[3];
	  if (mPointNoteHighlight != -1 || mSurfaceNoteHighlight != -1 || mFrustumNoteHighlight != -1)
	  {
		  //turnoffHighlight();
	  }
	  mPointNoteHighlight = id;
	  mSelectedPoints[mPointNoteHighlight].actor->GetProperty()->GetEdgeColor(color);
	  mSelectedPoints[mPointNoteHighlight].actor->GetProperty()->SetEdgeColor(1-color[0], 1-color[1], 1-color[2]);
  }

  void highlightSurfaceNote(int id)
  {
	  double* color = new double[3];
	  if (mPointNoteHighlight != -1 || mSurfaceNoteHighlight != -1 || mFrustumNoteHighlight != -1)
	  {
		  //turnoffHighlight();
	  }
	  mSurfaceNoteHighlight = id;
	  mSelectedSurface[mSurfaceNoteHighlight].actor->GetProperty()->GetColor(color);
	  mSelectedSurface[mSurfaceNoteHighlight].actor->GetProperty()->SetColor(1-color[0], 1-color[1], 1-color[2]);
  }

  void highlightFrustumNote(int id)
  {
	  double* color = new double[3];
	  if (mPointNoteHighlight != -1 || mSurfaceNoteHighlight != -1 || mFrustumNoteHighlight != -1)
	  {
		  //turnoffHighlight();
	  }
	  mFrustumNoteHighlight = id;
	  mSelectedFrustum[mFrustumNoteHighlight].second->GetProperty()->GetColor(color);
	  mSelectedFrustum[mFrustumNoteHighlight].second->GetProperty()->SetColor(1-color[0], 1-color[1], 1-color[2]);
  }

  void turnoffHighlight()
  {
	   double* color = new double[3];
	   if (mPointNoteHighlight != -1 && mPointNoteHighlight < mSelectedPoints.size())
	   {
		   mSelectedPoints[mPointNoteHighlight].actor->GetProperty()->GetEdgeColor(color);
		   mSelectedPoints[mPointNoteHighlight].actor->GetProperty()->SetEdgeColor(1-color[0], 1-color[1], 1-color[2]);
		   mPointNoteHighlight = -1;
	   }
	   if (mSurfaceNoteHighlight != -1 && mSurfaceNoteHighlight < mSelectedSurface.size()) 
	   {
		   mSelectedSurface[mSurfaceNoteHighlight].actor->GetProperty()->GetColor(color);		 
		   mSelectedSurface[mSurfaceNoteHighlight].actor->GetProperty()->SetColor(1-color[0], 1-color[1], 1-color[2]);		  
		   mSurfaceNoteHighlight = -1;		  
	   }
	   if (mFrustumNoteHighlight != -1 && mFrustumNoteHighlight < mSelectedFrustum.size())
	   {
		   mSelectedFrustum[mFrustumNoteHighlight].second->GetProperty()->GetColor(color);
		   mSelectedFrustum[mFrustumNoteHighlight].second->GetProperty()->SetColor(1-color[0], 1-color[1], 1-color[2]);
		   mFrustumNoteHighlight = -1;
	   }
  }

  bool choosePointNote()
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
	  vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
	  int currPos[2];
	  interactor->GetEventPosition(currPos);

	  vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
	  picker->SetTolerance(0.0005);

	  // Pick from this location. (screen location)
	  picker->Pick(currPos[0], currPos[1], 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	  double* worldPosition = picker->GetPickPosition();
	  if(picker->GetCellId() != -1)
	  {
		  for (int i = 0; i < mSelectedPoints.size(); i++)
		  {
			  if (!mSelectedPoints[i].actor->GetVisibility())
				  continue;
			  if (!isCTVolume && mSelectedPoints[i].cellIds->LookupValue(picker->GetCellId()) != -1)
			  {
				  //highlightPointNote(i);
				  mw()->mInformation->openPointNote(picker->GetCellId());
				  displayPointNoteInfo(picker->GetCellId(), worldPosition);
				  return true;
			  }
			  else if (isCTVolume)
			  {
				  double dis = sqrt(pow(mSelectedPoints[i].positions[0] - worldPosition[0], 2) 
					  + pow(mSelectedPoints[i].positions[1] - worldPosition[1], 2)
					  + pow(mSelectedPoints[i].positions[2] - worldPosition[2], 2));
				  if (dis <= double(std::max(std::max(mDimensions[0], mDimensions[1]), mDimensions[2]))/50)
				  {
					  mw()->mInformation->openPointNote(mSelectedPoints[i].cellIds->GetValue(0));
					  displayPointNoteInfo(mSelectedPoints[i].cellIds->GetValue(0), worldPosition);
				      return true;

				  }
			  }
		  }
	    
	  }
	  return false;
  }

  bool chooseSurfaceNote()
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
      vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
      int currPos[2];
      interactor->GetEventPosition(currPos);
	  vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
	  picker->Pick(currPos[0], currPos[1], 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	  if(picker->GetCellId() != -1)
	  {
		  for (int i = 0; i < mSelectedSurface.size(); i++)
		  {
			  if (!mSelectedSurface[i].actor->GetVisibility())
				  continue;
			  if (!isCTVolume && mSelectedSurface[i].cellIds->GetSelectionList()->LookupValue(picker->GetCellId()) != -1)
			  {
				  //highlightSurfaceNote(i);
				  //qDebug()<<"Selected surface "<<i<<endl;
				  mw()->mInformation->openSurfaceNote(mSelectedSurface[i].cellIds, mSelectedSurface[i].cornerPoints, false);
				  return true;
			  }
			  else if (isCTVolume)
			  {
				  double* worldPosition = picker->GetPickPosition();
				  vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
				  double* cameraPos = camera->GetPosition();

				  double direction[3] = {worldPosition[0] - cameraPos[0], worldPosition[1] - cameraPos[1], worldPosition[2] - cameraPos[2]};

				  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
				  for (int j = 0; j < 4; j++)
				  {
					  points->InsertNextPoint(mSelectedSurface[i].cornerPoints[j][0],
						  mSelectedSurface[i].cornerPoints[j][1], mSelectedSurface[i].cornerPoints[j][2]);
				  }
 
				  // Create the polygon
				  vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();
				  polygon->GetPoints()->DeepCopy(points);
				  polygon->GetPointIds()->SetNumberOfIds(4); // 4 corners of the square
				  polygon->GetPointIds()->SetId(0, 0);
				  polygon->GetPointIds()->SetId(1, 1);
				  polygon->GetPointIds()->SetId(2, 2);
				  polygon->GetPointIds()->SetId(3, 3);
 
				  double end[3] = {cameraPos[0] + direction[0]*1000, cameraPos[1] + direction[1]*1000,
					  cameraPos[2] + direction[2]*1000};
				  double tolerance = 1;
				  // Outputs
				  double t; // Parametric coordinate of intersection (0 (corresponding to world) to 1 (corresponding to end))
				  double x[3]; // The coordinate of the intersection
				  double pcoords[3];
				  int subId;
 
				  vtkIdType iD = polygon->IntersectWithLine(cameraPos, end, tolerance, t, x, pcoords, subId);
				  if (iD)
				  {
					  mw()->mInformation->openSurfaceNote(mSelectedSurface[i].cellIds, mSelectedSurface[i].cornerPoints, true);
					  return true;
				  }
			  }
		  }
	  }
	  return false;
  }

  bool chooseFrustumNote()
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
      vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
      int currPos[2];
      interactor->GetEventPosition(currPos);
	  vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
	  picker->Pick(currPos[0], currPos[1], 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	  double* worldPosition = picker->GetPickPosition();
	  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
      points->InsertNextPoint(worldPosition);
 
	  vtkSmartPointer<vtkPolyData> pointsPolydata = vtkSmartPointer<vtkPolyData>::New();
	  pointsPolydata->SetPoints(points);
	  for (int i = 0; i < mSelectedFrustum.size(); i++)
	  {
		  if (!mSelectedFrustum[i].second->GetVisibility())
			  continue;
		  vtkSmartPointer<vtkFrustumSource> frustum = vtkSmartPointer<vtkFrustumSource>::New();
		  frustum->SetPlanes(mSelectedFrustum[i].first);
		  vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints = vtkSmartPointer<vtkSelectEnclosedPoints>::New();
		  selectEnclosedPoints->SetInput(pointsPolydata);
		  selectEnclosedPoints->SetSurface(frustum->GetOutput());
		  selectEnclosedPoints->Update();
		  if(selectEnclosedPoints->IsInside(0))
		  {
			  //highlightFrustumNote(i);
			  //qDebug()<<"Selected Frustum "<<i<<endl;
			  mw()->mInformation->openFrustumNote(mSelectedFrustum[i].first);
			  return true;
		  }
	  }
	  return false;
  }

  void displayLoadPointNote(const int cellId, const ColorType color, const double* position)
  {
      vtkSmartPointer<vtkRenderer> renderer = this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
	  vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
      vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	  actor->SetMapper(mapper);
	  actor->GetProperty()->EdgeVisibilityOn();
      actor->GetProperty()->SetEdgeColor(ColorPixel[color][0], ColorPixel[color][1], ColorPixel[color][2]);
      actor->GetProperty()->SetLineWidth(5);
      actor->VisibilityOff();
      renderer->AddActor(actor);
	  vtkSmartPointer<vtkIdTypeArray> cell = vtkSmartPointer<vtkIdTypeArray>::New();
	  cell->InsertNextValue(cellId);
	  PointMark pointNote;
	  pointNote.cellIds = cell;
	  pointNote.actor = actor;
	  pointNote.positions[0] = position[0];
	  pointNote.positions[1] = position[1];
	  pointNote.positions[2] = position[2];
      mSelectedPoints.push_back(pointNote);
      qDebug()<<"load point note" << cellId;
	  displayPointNote(mapper, cell, position);  
  }

  void displayLoadSurfaceNote(vtkSmartPointer<vtkSelectionNode> cellIds, QVector<double*> points, const ColorType color)
  {
	  vtkSmartPointer<vtkRenderer> renderer = this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
	  vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
      vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	  actor->SetMapper(mapper);
	  actor->PickableOff();
	  actor->GetProperty()->LightingOn();
      actor->GetProperty()->SetColor(ColorPixel[color][0], ColorPixel[color][1], ColorPixel[color][2]);
      actor->GetProperty()->SetLineWidth(5);
      actor->VisibilityOff();
      renderer->AddActor(actor);
	  vtkSmartPointer<vtkSelectionNode> newCellIds = vtkSmartPointer<vtkSelectionNode>::New();
	  newCellIds->DeepCopy(cellIds);
	  SurfaceMark surfaceNote;
	  surfaceNote.cellIds = newCellIds;
	  surfaceNote.cornerPoints = points.toStdVector();
	  surfaceNote.actor = actor;
	  mSelectedSurface.push_back(surfaceNote);
	  displaySurfaceNote(mapper, newCellIds, points);
	  qDebug()<<"load Surface note" << mSelectedSurface.size();
  }

  void displayLoadFrustumNote(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals, const ColorType color)
  {
	  if (points->GetNumberOfPoints() != 6 || normals->GetSize() != 18)
	  {
		  qDebug() << "Cannot Remove Empty Frustum Note!" << points->GetNumberOfPoints() << normals->GetSize();
		  return;
	  }
      vtkSmartPointer<vtkRenderer> renderer = this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
	  vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	  vtkSmartPointer<vtkPlanes> frustum = vtkSmartPointer<vtkPlanes>::New();
	  vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
	  vtkSmartPointer<vtkDataArray> newNormals = normals;
	  newPoints->DeepCopy(points);
	  newNormals->DeepCopy(normals);
	  /*
	  for (int i = 0; i < 6; i++)
	  {
		  double point[3];
		  points->GetPoint(i, point);
		  //qDebug()<<"Points"<<point[0]<<point[1]<<point[2];
	  }
	  for (int i = 0; i < 6; i++)
	  {
		qDebug()<<newNormals->GetVariantValue(3*i).ToDouble()<<newNormals->GetVariantValue(3*i + 1).ToDouble()<<newNormals->GetVariantValue(3*i + 2).ToDouble();
	  }*/
	  frustum->SetPoints(newPoints);
	  frustum->SetNormals(newNormals);
	  mSelectedFrustum.push_back(std::make_pair(frustum, actor));
 
	  actor->SetMapper(mapper);
	  actor->GetProperty()->LightingOn();
	  actor->GetProperty()->SetColor(ColorPixel[color][0], ColorPixel[color][1], ColorPixel[color][2]);
	  actor->GetProperty()->SetLineWidth(5);
	  renderer->AddActor(actor);
      displayFrustumNote(mapper, frustum);
	  actor->VisibilityOff();
	  qDebug()<<"load Frustum note" << mSelectedFrustum.size();

  }

  void drawPointNote()
  {
      vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
      vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

      int currPos[2];
      interactor->GetEventPosition(currPos);
      vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
      picker->SetTolerance(0.0005);

      // Pick from this location. (screen location)
      picker->Pick(currPos[0], currPos[1], 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	  double* worldPosition = picker->GetPickPosition();
      if (picker->GetCellId() != -1)
      {
          picker->GetPickPosition(mAnnotationSpot);
		  //qDebug() << "annotation world spot:" << worldPosition[0] << worldPosition[1] << worldPosition[2];

          //qDebug() << "annotation spot:" << mAnnotationSpot[0] << mAnnotationSpot[1] << mAnnotationSpot[2];

		  if(!mw()->mInformation) return;
		  for (int i = 0; i < mSelectedPoints.size(); i++)
		  {
			  if (mSelectedPoints[i].cellIds->LookupValue(picker->GetCellId()) != -1)
				  return;
		  }

          mw()->mInformation->createPointNote(mAnnotationSpot, picker->GetCellId(), mColor);

		  vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	      vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
		  actor->SetMapper(mapper);
		  actor->GetProperty()->EdgeVisibilityOn();
          actor->GetProperty()->SetEdgeColor(ColorPixel[mColor][0], ColorPixel[mColor][1], ColorPixel[mColor][2]);
          actor->GetProperty()->SetLineWidth(5);
	      actor->VisibilityOn();
	      renderer->AddActor(actor);

		  vtkSmartPointer<vtkIdTypeArray> cellId = vtkSmartPointer<vtkIdTypeArray>::New();
		  cellId->InsertNextValue(picker->GetCellId());

		  PointMark pointNote;
		  pointNote.cellIds = cellId;
		  pointNote.actor = actor;
		  pointNote.positions[0] = worldPosition[0];
		  pointNote.positions[1] = worldPosition[1];
		  pointNote.positions[2] = worldPosition[2];
		  mSelectedPoints.push_back(pointNote);
          //mSelectedPoints.push_back(std::make_pair(cellId, actor));

          displayPointNote(mapper, cellId, worldPosition);
		  displayPointNoteInfo(picker->GetCellId(), worldPosition);
      }
	  else 
	  {
          mFollower->VisibilityOff();
          mSelectedActor2->VisibilityOff();
      }
  }

  void drawSurfaceNote()
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
	  vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

	  vtkSmartPointer<vtkHardwareSelector> sel = vtkSmartPointer<vtkHardwareSelector>::New();
	  sel->SetRenderer(renderer);
	  double x0 = renderer->GetPickX1();
	  double y0 = renderer->GetPickY1();
	  double x1 = renderer->GetPickX2();
	  double y1 = renderer->GetPickY2();
	  QVector<double*> points;

	  vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
      picker->SetTolerance(0.0005);
      picker->Pick(x0, y0, 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	  if (picker->GetCellId() == -1)
		  return;
	  double* worldPosition1 = picker->GetPickPosition();
	  points.push_back(worldPosition1);

	  vtkSmartPointer<vtkCellPicker> picker1 = vtkSmartPointer<vtkCellPicker>::New();
      picker1->SetTolerance(0.0005);
	  picker1->Pick(x0, y1, 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	  if (picker1->GetCellId() == -1)
		  return;
	  double* worldPosition2 = picker1->GetPickPosition();
	  points.push_back(worldPosition2);

	  vtkSmartPointer<vtkCellPicker> picker2 = vtkSmartPointer<vtkCellPicker>::New();
      picker2->SetTolerance(0.0005);
	  picker2->Pick(x1, y1, 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	  if (picker2->GetCellId() == -1)
		  return;
	  double* worldPosition3 = picker2->GetPickPosition();
	  points.push_back(worldPosition3);

	  vtkSmartPointer<vtkCellPicker> picker3 = vtkSmartPointer<vtkCellPicker>::New();
      picker3->SetTolerance(0.0005);
	  picker3->Pick(x1, y0, 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	  if (picker3->GetCellId() == -1)
		  return;
	  double* worldPosition4 = picker3->GetPickPosition();
	  points.push_back(worldPosition4);

	  //qDebug()<<"selected points : "<< x0 <<" " <<y0<<" "<<x1 <<" " <<y1;
	  
	  sel->SetArea(static_cast<int>(x0),static_cast<int>(y0),static_cast<int>(x1), static_cast<int>(y1));
	  vtkSmartPointer<vtkSelection> res;

	  res.TakeReference(sel->Select());
	  if (!res)
	  {
     		qDebug() << "Selection not supported.";
			return;
	  }	
	  vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
      vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	  actor->SetMapper(mapper);
	  actor->PickableOff();
	  actor->GetProperty()->LightingOn();
      actor->GetProperty()->SetColor(ColorPixel[mColor][0], ColorPixel[mColor][1], ColorPixel[mColor][2]);
      actor->GetProperty()->SetLineWidth(5);
      actor->VisibilityOn();
      renderer->AddActor(actor);
	  if (!res->GetNode(0) && !isCTVolume)
		  return;

	  SurfaceMark surfaceNote;
	  surfaceNote.cellIds = res->GetNode(0);
	  // Deep copy four worldPoints since they will be disconstructed with picker
	  double* conerPoint1 = new double[3];
	  double* conerPoint2 = new double[3];
	  double* conerPoint3 = new double[3];
	  double* conerPoint4 = new double[3];
	  for(int i = 0; i < 3; i++)
	  {
		 conerPoint1[i] = worldPosition1[i];
		 conerPoint2[i] = worldPosition2[i];
		 conerPoint3[i] = worldPosition3[i];
		 conerPoint4[i] = worldPosition4[i];
	  }
	  surfaceNote.cornerPoints.push_back(conerPoint1);
	  surfaceNote.cornerPoints.push_back(conerPoint2);
	  surfaceNote.cornerPoints.push_back(conerPoint3);
	  surfaceNote.cornerPoints.push_back(conerPoint4);

	  surfaceNote.actor = actor;
	  mSelectedSurface.push_back(surfaceNote);
      displaySurfaceNote(mapper, res->GetNode(0), points);
	  mw()->mInformation->createSurfaceNote(res->GetNode(0), points, mColor, isCTVolume);
  }

  void drawFrustumNote()
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
	  vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
	  vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	  if (!displayFrustumNote(mapper, mAreaPicker->GetFrustum()))
		  return;

	  mSelectedFrustum.push_back(std::make_pair(mAreaPicker->GetFrustum(), actor));

	  vtkSmartPointer<vtkDataArray> normal = mAreaPicker->GetFrustum()->GetNormals();
	  actor->SetMapper(mapper);
	  actor->GetProperty()->LightingOn();
	  qDebug()<<"Frustum color" << mColor;
	  actor->GetProperty()->SetColor(ColorPixel[mColor][0], ColorPixel[mColor][1], ColorPixel[mColor][2]);
	  actor->GetProperty()->SetLineWidth(5);
	  actor->VisibilityOn();
	  renderer->AddActor(actor);
      
	  mw()->mInformation->createFrustumNote(mAreaPicker->GetFrustum()->GetPoints(), mAreaPicker->GetFrustum()->GetNormals(), mColor);
	  mAreaPicker = vtkSmartPointer<vtkRenderedAreaPicker>::New(); //refresh the area picker so that the frustum will not be pointed to the same address 
	  interactor->SetPicker(mAreaPicker);
  }

  void removePointNoteMark(int cellId)
  {
	  for (int i = 0; i < mSelectedPoints.size(); i++)
	  {
		  if (mSelectedPoints[i].cellIds->LookupValue(cellId) != -1)
		  {
			  mSelectedPoints[i].actor->VisibilityOff();
			  mSelectedPoints.erase(mSelectedPoints.begin() + i);
		  }
	  }
	  mFollower->VisibilityOff();
  }

  void removeSurfaceNoteMark(vtkSmartPointer<vtkSelectionNode> cellIds, std::vector<double*> cornerPoints, bool isCTVolume)
  {
	  bool erase = false;
	  if (!isCTVolume)
	  {
		  if (!cellIds->GetSelectionList())
		  {
			  qDebug() << "Cannot Remove Empty Surface Note!" << endl;
			  return;
		  }
		  
		  for (int i = 0; i < mSelectedSurface.size(); i++)
		  {
			  if (cellIds->GetSelectionList()->GetSize() != mSelectedSurface[i].cellIds->GetSelectionList()->GetSize())
				  continue;
			  bool elementExist = true;
			  for (int j = 0; j < mSelectedSurface[i].cellIds->GetSelectionList()->GetSize(); j++)
			  {
				  if (cellIds->GetSelectionList()->LookupValue(mSelectedSurface[i].cellIds->GetSelectionList()->GetVariantValue(j)) == -1)
				  {
					  elementExist = false;
					  break;
				  }
			  }
			  if (elementExist)
			  {
				  mSelectedSurface[i].actor->VisibilityOff();
				  mSelectedSurface.erase(mSelectedSurface.begin() + i);
				  erase = true;
				  break;
			  }
		  }
	  }
	  else
	  {
		  for (int i = 0; i < mSelectedSurface.size(); i++)
		  {
			  bool isSame = true;
			  for (int j = 0; j < 4; j++)
			  {
				  if (cornerPoints[j][0] != mSelectedSurface[i].cornerPoints[j][0] || cornerPoints[j][1] != mSelectedSurface[i].cornerPoints[j][1]
				  || cornerPoints[j][2] != mSelectedSurface[i].cornerPoints[j][2])
				  {
					  isSame = false;
					  break;
				  }
			  }
			  if (!isSame)
				  continue;
			  
			  mSelectedSurface[i].actor->VisibilityOff();
			  mSelectedSurface.erase(mSelectedSurface.begin() + i);
			  erase = true;
			  break;
		  }
	  }
	  if (!erase)	qDebug() << "Cannot Find the Exact SurfaceNote to remove!"<<endl;
  }

  void removeFrustumNoteMark(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals)
  {
	  if (points->GetNumberOfPoints() != 6 || normals->GetSize() != 18)
	  {
		  qDebug() << "Cannot Remove Empty Frustum Note!" << endl;
		  return;
	  }
	  bool isFound = false;
	  for (int i = 0; i < mSelectedFrustum.size(); ++i) 
	  {
		  bool elementExist = true;
		  for (int j = 0; j < 6; j++)
	      {
			  vtkSmartPointer<vtkPlane> plane = mSelectedFrustum[i].first->GetPlane(i);
			  bool planeExist = false;
			  for (int k = 0; k < 6; k++)
			  {
				  double pos[3], norm[3], planeNorm[3];
				  points->GetPoint(k, pos);
				  norm[0] = normals->GetVariantValue(3*k).ToDouble();
				  norm[1] = normals->GetVariantValue(3*k + 1).ToDouble();
				  norm[2] = normals->GetVariantValue(3*k + 2).ToDouble();
				  plane->GetNormal(planeNorm);
				  if (plane->EvaluateFunction(pos) == 0 && norm[0] == planeNorm[0] && norm[1] == planeNorm[1] && norm[2] == planeNorm[2])
				  {
					  planeExist = true;
					  break;
				  }
			  }
			  if (!planeExist)
			  {
			     elementExist = false;
				 break;
			  }
	      }
	      if (elementExist)
	      {
			  mSelectedFrustum[i].second->VisibilityOff();
			  mSelectedFrustum.erase(mSelectedFrustum.begin() + i);
		      isFound = true;
		      break;
	      }
	  }
	  if (!isFound)	qDebug() << "Cannot Find the Exact FrustumNote to remove!"<<endl;
  }

   void openPointNoteMark(int cellId)
  {
	  for (int i = 0; i < mSelectedPoints.size(); i++)
	  {
		  if (mSelectedPoints[i].cellIds->LookupValue(cellId) != -1)
		  {
			  mSelectedPoints[i].actor->VisibilityOn();
			  break;
		  }
	  }
  }

  void openSurfaceNoteMark(vtkSmartPointer<vtkSelectionNode> cellIds, std::vector<double*> cornerPoints, bool isCTVolume)
  {
	  bool open = false;
	  if (!isCTVolume)
	  {
		  if (!cellIds->GetSelectionList())
		  {
			  qDebug() << "Cannot open Empty Surface Note!" << endl;
			  return;
		  }
		 
		  for (int i = 0; i < mSelectedSurface.size(); i++)
		  {
			  if (cellIds->GetSelectionList()->GetSize() != mSelectedSurface[i].cellIds->GetSelectionList()->GetSize())
				  continue;
			  bool elementExist = true;
			  for (int j = 0; j < mSelectedSurface[i].cellIds->GetSelectionList()->GetSize(); j++)
			  {
				  if (cellIds->GetSelectionList()->LookupValue(mSelectedSurface[i].cellIds->GetSelectionList()->GetVariantValue(j)) == -1)
				  {
					  elementExist = false;
					  break;
				  }
			  }
			  if (elementExist)
			  {
				  mSelectedSurface[i].actor->VisibilityOn();
				  open = true;
				  break;
			  }
		  }
	  }
	  else
	  {
		  for (int i = 0; i < mSelectedSurface.size(); i++)
		  {
			  bool isSame = true;
			  for (int j = 0; j < 4; j++)
			  {
				  if (cornerPoints[j][0] != mSelectedSurface[i].cornerPoints[j][0] || cornerPoints[j][1] != mSelectedSurface[i].cornerPoints[j][1]
				  || cornerPoints[j][2] != mSelectedSurface[i].cornerPoints[j][2])
				  {
					  isSame = false;
					  break;
				  }
			  }
			  if (!isSame)
				  continue;
			  
			 mSelectedSurface[i].actor->VisibilityOn();
			 open = true;
			 break;
		  }
	  }
  }

  void openFrustumNoteMark(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals)
  {
	  if (points->GetNumberOfPoints() != 6 || normals->GetSize() != 18)
	  {
		  qDebug() << "Cannot open Empty Frustum Note!" << endl;
		  return;
	  }
	  bool isFound = false;
	  for (int i = 0; i < mSelectedFrustum.size(); ++i) 
	  {
		  bool elementExist = true;
		  for (int j = 0; j < 6; j++)
	      {
			  vtkSmartPointer<vtkPlane> plane = mSelectedFrustum[i].first->GetPlane(i);
			  bool planeExist = false;
			  for (int k = 0; k < 6; k++)
			  {
				  double pos[3], norm[3], planeNorm[3];
				  points->GetPoint(k, pos);
				  norm[0] = normals->GetVariantValue(3*k).ToDouble();
				  norm[1] = normals->GetVariantValue(3*k + 1).ToDouble();
				  norm[2] = normals->GetVariantValue(3*k + 2).ToDouble();
				  plane->GetNormal(planeNorm);
				  if (plane->EvaluateFunction(pos) == 0 && norm[0] == planeNorm[0] && norm[1] == planeNorm[1] && norm[2] == planeNorm[2])
				  {
					  planeExist = true;
					  break;
				  }
			  }
			  if (!planeExist)
			  {
			     elementExist = false;
				 break;
			  }
	      }
	      if (elementExist)
	      {
			  mSelectedFrustum[i].second->VisibilityOn();
		      isFound = true;
		      break;
	      }
	  }
	  if (!isFound)	qDebug() << "Cannot Find the Exact FrustumNote to open!"<<endl;
  }

  void getRubberbandStart()
  {
    mIsRubberbandVisible = true;
    mFollower->VisibilityOff();
    mSelectedActor2->VisibilityOff();


    vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
    vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

    int currPos[2];
    interactor->GetEventPosition(currPos);
    vtkSmartPointer<vtkCellPicker> currpicker = vtkSmartPointer<vtkCellPicker>::New();
    currpicker->SetTolerance(0.0005);  

    // Pick from this location. (screen location)
    currpicker->Pick(currPos[0], currPos[1], 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

    if (currpicker->GetCellId() != -1)
    {
      currpicker->GetPickPosition(mRubberStart);
//      qDebug() << "rubberband start: " << mRubberStart[0] << " " << mRubberStart[1] << " " << mRubberStart[2];
    }
  }

  void drawRubberbandLine()
  {
    if (mWasReadyRubberband && mUseRubberband ) {
      vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
      vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

      // detect the current world coordinates
      int currPos[2];
      interactor->GetEventPosition(currPos);
      vtkSmartPointer<vtkCellPicker> currpicker = vtkSmartPointer<vtkCellPicker>::New();
      currpicker->SetTolerance(0.0005);

      // Pick from this location. (screen location)
      currpicker->Pick(currPos[0], currPos[1], 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
//      if (currpicker->GetCellId() != -1)
//      {
        currpicker->GetPickPosition(mRubberEnd);
        // draw a line
//        vtkSmartPointer<vtkLineSource> line = vtkSmartPointer<vtkLineSource>::New();
        mRubberband->SetPoint1(mRubberStart);
        mRubberband->SetPoint2(mRubberEnd);
        mRubberband->Update();

        mSelectedMapper2->SetInputConnection( mRubberband->GetOutputPort() ); // for selected line drawing
        mSelectedMapper2->Update();

        mSelectedActor2->SetMapper(mSelectedMapper2);
        mSelectedActor2->GetProperty()->SetColor(1,0.75,0);
        mSelectedActor2->GetProperty()->SetLineWidth(6);
        mSelectedActor2->VisibilityOn();

        renderer->AddActor( mSelectedActor2 );
//      }
//      else {
        mFollower->VisibilityOff();
        mSelectedActor2->VisibilityOn();

//        mSelectedActor2->VisibilityOff();
//      }
      //interactor->Render();
    }
  }


  void displayRubberbandDist()
  {
    vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
    vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
    vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();

    // Get the location of the click (in window coordinates)
    int currPos[2];
    interactor->GetEventPosition(currPos);

    vtkSmartPointer<vtkCellPicker> currpicker = vtkSmartPointer<vtkCellPicker>::New();
    currpicker->SetTolerance(0.0005);

    // Pick from this location. (screen location)
    currpicker->Pick(currPos[0], currPos[1], 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

    if (currpicker->GetCellId() != -1)
    {
      currpicker->GetPickPosition(mRubberEnd);

//      qDebug() << "rubberband end (point): " << mRubberEnd[0] << " " << mRubberEnd[1] << " " << mRubberEnd[2];

      mRubberDist = sqrt(vtkMath::Distance2BetweenPoints(mRubberStart,mRubberEnd));

      char text1[512]; memset( text1, 0, sizeof(text1) );
      sprintf( text1, "   (%.2fcm)", mRubberDist/10. );

      vtkSmartPointer<vtkVectorText> vtext = vtkSmartPointer<vtkVectorText>::New();

      vtext->SetText(text1);
      mTextMapper->SetInputConnection( vtext->GetOutputPort() );
      mFollower->SetMapper( mTextMapper );
      mFollower->SetScale( camera->GetDistance()/100 ); // change the scale dynamically, according to camera distance

      vtkSmartPointer<vtkProperty> property = mFollower->GetProperty();
      property->SetColor(1,0.75,0); // sphere color orange
      property->LightingOff(); // turn off shading


      mFollower->SetCamera(renderer->GetActiveCamera());
      double pos[3];
      currpicker->GetPickPosition(pos);
      mFollower->SetPosition( pos ); //((vtkProp3D*)collection->GetItemAsObject( 3 ))->GetCenter());
      mFollower->PickableOff();
      mFollower->VisibilityOn();

      renderer->AddActor( mFollower );
      }
    else {
        mFollower->VisibilityOff();
        mSelectedActor2->VisibilityOff();
    }
    displayInfoAnnotation(true);
    //interactor->Render();
  }

  void zoomIn(){
    vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
    vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast( interactor->GetInteractorStyle() );
    if (style)  { style->OnMouseWheelForward(); }
  }

  void zoomOut(){
    vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
    vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast( interactor->GetInteractorStyle() );
    if (style)  { style->OnMouseWheelBackward(); }
  }

  virtual void Execute(vtkObject *, unsigned long event, void *) {
	//qDebug () << "Call of Execute" << count;
    // get the camera transform matrix
    vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
    vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
    vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
    camera->GetViewUp(mLastCamY);

    double camPos[3], focalPos[3];
    camera->GetPosition(camPos);
    camera->GetFocalPoint(focalPos);
    vtkMath::Subtract(focalPos,camPos,mLastViewVec);
    vtkMath::Normalize(mLastViewVec);

    vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast( interactor->GetInteractorStyle() ); //NB smartpointer is not working
    displayInfoAnnotation(true);
	
	count++;

      if (event == vtkCommand::LeftButtonPressEvent)
      {
		//Annotation and Rubberband should be preemptive, and the tool menu should be updated, I will fix this later.
		if (mUserIsAnnotating)
        {
			/*switch(mNoteMode)
			{
				case POINTNOTE:
					choosePointNote();
					break;
				case SURFACENOTE:
					chooseSurfaceNote();
					break;
				case FRUSTUMNOTE:
					chooseFrustumNote();
					break;
				case UNDECLARE:
					qDebug() << "The Note Mode is not declared";
				default:
					qDebug() << "The Note Mode is not supported";
			}*/
			//qDebug()<<"left!!!!!!!!";
			if (!choosePointNote() && !chooseSurfaceNote() && !chooseFrustumNote())
			{
				//turnoffHighlight();
			}
        }
        else if (mUseRubberband  && mWasReadyRubberband)
        {
           displayRubberbandDist();
           mWasReadyRubberband = false;
        }
        else if (mUseRubberband && !mWasReadyRubberband)
        {
          mWasReadyRubberband = true; // toggle bool
          getRubberbandStart();
        }
        if (style)  {style->OnLeftButtonDown();}
      }
    else if (event == vtkCommand::LeftButtonReleaseEvent)
      {
        if (style)  { style->OnLeftButtonUp(); }
        checkBestImageRequested = true;

      }
    else if (event == vtkCommand::MiddleButtonPressEvent)
      {
        if (style)  { style->OnMiddleButtonDown(); }
      }
    else if (event == vtkCommand::MiddleButtonReleaseEvent)
      {
        if (style)  { style->OnMiddleButtonUp(); }
      }
    else if (event == vtkCommand::RightButtonPressEvent)
      {
		if (mUserIsAnnotating)
        {
			//
			if (mNoteMode == POINTNOTE)
			{
				
				drawPointNote();
			}
			else
			{
				qDebug() << "Not in the Point Note mode, cannot put a note!";
			}
        }
      }
    else if (event == vtkCommand::RightButtonReleaseEvent)
      {
//        if (style)  { style->OnRightButtonUp(); }
      }
    else if (event == vtkCommand::MouseMoveEvent)
      {
        if (mUseRubberband) {
          drawRubberbandLine();
        } else {
          if (mDisplayInfoOn && mDisplayPolyIndicateOn)
            displaySelectPoly();
        }
        if (style)
        {
            style->OnMouseMove();
        }
       }
    else if (event == vtkCommand::MouseWheelForwardEvent)
      {
        if (style)  { style->OnMouseWheelForward(); }
        reRenderBestImage = true;
      }
    else if (event == vtkCommand::MouseWheelBackwardEvent)
      {
        if (style)  { style->OnMouseWheelBackward(); }
        reRenderBestImage = true;
      }
	else if (event == vtkCommand::EndPickEvent)
	{
		if (mUserIsAnnotating)
		{
			switch(mNoteMode)
			{
				case SURFACENOTE:
					drawSurfaceNote();
					break;
				case FRUSTUMNOTE:
					drawFrustumNote();
					break;
				case UNDECLARE:
					qDebug() << "The Note Mode is not declared";
				default:
					qDebug() << "The Note Mode is not supported";
			}
            
		}
	}


    // detect the difference
    camera->GetViewUp(mCurrCamY);
    // only change light position when the camera vector is changed
    if ( (vtkMath::Distance2BetweenPoints(mLastCamY, mCurrCamY) != 0) &&   mIsDirectionalLight)
    {
      updateLightingPosition();
    }

    if(imageOverlayOn && inputImageSet->isValid())
    {
        camera->GetPosition(camPos);
        camera->GetFocalPoint(focalPos);
        vtkMath::Subtract(focalPos,camPos,mCurrViewVec);
        vtkMath::Normalize(mCurrViewVec);
        int bestInputImage = currentBestImage;
		
        if(checkBestImageRequested)
        {
            bestInputImage = inputImageSet->getClosestImage(camPos, mCurrViewVec); 
            checkBestImageRequested = false;
        }
		SetBestImage(bestInputImage);
        if(newTextureLoaded)
        {
            newTextureLoaded = false;
        }
    }
  }


public:

  vtkSmartPointer<vtkFollower> XLabel;
  vtkSmartPointer<vtkFollower> YLabel;
  vtkSmartPointer<vtkFollower> ZLabel;
  vtkSmartPointer<vtkAssembly> Axes;

  InputImageSet *inputImageSet;
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> mViewStyle;

private:
  // Pointer to the interactor
  vtkSmartPointer<QVTKInteractor> mInteractor;
  vtkSmartPointer<vtkAreaPicker> mAreaPicker;

  // for selection of triangle mesh
  vtkSmartPointer<vtkPolyData> mPolyData;
  vtkSmartPointer<vtkDataSetMapper> mSelectedMapper;
  vtkSmartPointer<vtkActor> mSelectedActor;

  vtkSmartPointer<vtkDataSetMapper> mSelectedMapper2;
  vtkSmartPointer<vtkActor> mSelectedActor2;

  vtkSmartPointer<vtkDataSetMapper> mSurfaceMapper;
  vtkSmartPointer<vtkActor> mSurfaceSelector;

  // for printing the information on mesh directly
  vtkSmartPointer<vtkFollower> mFollower;
  vtkSmartPointer<vtkPolyDataMapper> mTextMapper;

  // texture data
  vtkSmartPointer<vtkTexture> mRgbTexture;
  vtkSmartPointer<vtkImageData> mHyperImageData;

  vtkSmartPointer<vtkCornerAnnotation>  mInfoAnnotation;  // Pointer to the annotation

  vtkSmartPointer<vtkTransform> mTransform; // for object (space holder)
  vtkSmartPointer<vtkTransform> mLightTransform; // for light

  vtkSmartPointer<vtkLineSource> mRubberband;

  QString mGLversion;
  QString mNumCore;
  QString mFilename;
  QString mRgbTextureFilename;

  NoteMode mNoteMode;
  ColorType mColor;

  int mNumberOfPoints;
  int mNumberOfPolys;
  int mNumberOfStrips;
  int mNumberOfLines;
  int mNumberOfVerts;
  int mNumberOfCells;

  double mRubberStart[3];
  double mRubberEnd[3];
  double mRubberDist;
  bool mWasReadyRubberband;

  bool mIsDirectionalLight;

  double mAnnotationSpot[3];

  std::vector<float> mHyperPixels;
  std::vector<PointMark> mSelectedPoints;  //used for point note
  std::vector<std::pair<vtkSmartPointer<vtkPlanes>, vtkSmartPointer<vtkActor> > > mSelectedFrustum;   //used for frustum note
  std::vector<SurfaceMark> mSelectedSurface;  //used for surface note
  int mPointNoteHighlight;
  int mSurfaceNoteHighlight;
  int mFrustumNoteHighlight;
  bool mDisplayInfoOn;
  bool mUseRubberband;
  bool mIsRubberbandVisible;
  bool mDisplayPolyIndicateOn;
  bool mUserIsAnnotating;
  bool isCTVolume;

  double mCurrCamY[3]; // camera up vector change detection for speed up
  double mLastCamY[3];

  double mLastViewVec[3];
  double mCurrViewVec[3];
  int mDimensions[3];

  // for image overlays
  vtkSmartPointer<vtkActor> mBestImageActor;
  vtkSmartPointer<vtkPolyData> quad;
  vtkSmartPointer<vtkJPEGReader> jPEGReader;
  vtkSmartPointer<vtkPolyDataMapper> mapper;
  vtkSmartPointer<vtkCellArray> polygons;
  vtkSmartPointer<vtkPolygon> polygon;
  int currentBestImage;
  int currentImageWidth, currentImageHeight;
  bool checkBestImageRequested;
  bool imageadded;
  bool newTextureLoaded;
  bool reRenderBestImage;
  bool imageOverlayOn;
  double wBL[3], wBR[3], wTR[3], wTL[3];
  int count;
};
#endif // VTK3DINTERACTIONCALLBACK_HPP
