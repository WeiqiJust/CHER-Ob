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
#ifndef VTK2DINTERACTIONCALLBACK_HPP
#define VTK2DINTERACTIONCALLBACK_HPP

#include <string>
#include <vector>
#include <QVector>

#include <vtkStringArray.h>
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
#include <vtkImageReslice.h>
#include <vtkPropPicker.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyle.h>
#include <vtkCell.h>
#include <vtkType.h>
#include <vtkAssemblyNode.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkSeedRepresentation.h>
#include <vtkProperty2D.h>
#include <vtkSeedWidget.h>
#include "../mainWindow.h"
#include "../information/informationWidget.h"
#include "myVTKInteractorStyle.h"

#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkInteractorStyleRubberBand2D.h>
#include <vtkJPEGReader.h>
#include <vtkInteractorStyleAreaSelectHover.h>
#include <vtkAreaLayout.h>
#include <vtkHardwareSelector.h>
#include <vtkAreaPicker.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkRenderedAreaPicker.h>
#include <vtkExtractSelectedPolyDataIds.h>
#include <vtkPolyLine.h>
#include <vtkCellArray.h>

#define DEFAULT_WINDOW (255.0)
#define DEFAULT_LEVELS (127.5)

#define GAMMA (2.2f)

#ifndef MAX_LINE
  #define MAX_LINE (256)
#endif


//#define SHOWHYPERVALUES
//-----------------------------------------------------------------------------------------
// MK: customize my interaction style for VTK object
class vtk2DInteractionCallback : public vtkCommand
{
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
	 return NULL;
   }

  static vtk2DInteractionCallback *New() {
    return new vtk2DInteractionCallback;
  }

  // initialization for constructor
  vtk2DInteractionCallback() {
    this->Interactor = NULL;
    this->Slicing = NULL;
    this->ImageReslice = NULL;
    this->Viewer     = NULL;
    this->CurrPicker     = NULL;
    this->mInfoAnnotation = NULL;
    this->mLightTransform = vtkSmartPointer<vtkTransform>::New();
    this->mIsDICOM = false;
	this->mUserIsAnnotating = false;
	this->mNoteMode = POINTNOTE;
	this->mColor = YELLOW;
	this->mPointNoteHighlight = -1;
    this->mSurfaceNoteHighlight = -1;
    this->mPolygonNoteHighlight = -1;
  }

  ~vtk2DInteractionCallback() {
    this->Interactor = NULL;
    this->Slicing = NULL;
    this->ImageReslice = NULL;
    this->Viewer     = NULL;
    this->CurrPicker     = NULL;
    this->mInfoAnnotation = NULL;
    this->mLightTransform = NULL;
  }

  NoteMode GetNoteMode() {
	  return mNoteMode;
  }

  void SetHyperImageData(vtkImageData* hypertexture)
  {
    this->mHyperImageData = hypertexture;
  }

  void SetDisplayInfoOn(bool displayon)
  {
    this->mDisplayInfoOn = displayon;
    if (mDisplayInfoOn && mInfoAnnotation)
    {
      mInfoAnnotation->VisibilityOn();
    } else {
      mInfoAnnotation->VisibilityOff();
    }
  }

  void SetImageReslice(vtkImageReslice *reslice) {
    this->ImageReslice = reslice;
  }

  vtkImageReslice *GetImageReslice() {
    return this->ImageReslice;
  }

  void SetInteractor(QVTKInteractor *interactor) {
    this->Interactor = interactor;
  }

  void SetGLversion (QString version) {
    this->mGLversion = version;
  }

  void SetNumCore (QString version) {
    this->mNumCore = version;
  }

  void SetIsDICOM (bool is) {
    this->mIsDICOM = is;
  }

  QVTKInteractor *GetInteractor() {
    return this->Interactor;
  }

  std::vector<float> GetHyperPixels() {
    return this->mHyperPixels;
  }

  void GetPoint(int icoord[3]) {
    this->CurrPicker->GetPointIJK(icoord);
  }

  void SetPicker(vtkCellPicker *picker)
    {
    this->CurrPicker = picker;
    }

  void SetAnnotation(bool status, NoteMode noteMode, ColorType color = YELLOW, bool visibilityOn = true)
  {
	  //turnoffHighlight();
	  if (noteMode != POINTNOTE && noteMode != SURFACENOTE && noteMode != POLYGONNOTE)
	  {
		  qDebug() << "Node Type is not correct!" << noteMode;
		  return;
	  }
      this->mUserIsAnnotating = status;
	  this->mNoteMode = noteMode;
	  this->mColor = color;
	  if (mUserIsAnnotating)
	  {
		  //qDebug()<<"Load surface 2D size"<<mSelectedSurface.size() << " point 2D size "<<mSelectedPoint.size();
		  initNote();
		  if (visibilityOn)
		  {
			  for (int i = 0; i < mSelectedPoint.size(); i++)
			  {
				 mSelectedPoint[i].second->VisibilityOn();
			  }
			  for (int i = 0; i < mSelectedSurface.size(); i++)
			  {
				 mSelectedSurface[i].second->VisibilityOn();
			  }
			  for (int i = 0; i < mSelectedPolygon.size(); i++)
			  {
				 mSelectedPolygon[i].second->VisibilityOn();
			  }
			  
		  }
		  mw()->mInformation->startAnnotation();
		  mw()->VTKA()->update();
	  }
	  else
	  {
		  for (int i = 0; i <  mSelectedPoint.size(); i++)
		  {
			  mSelectedPoint[i].second->VisibilityOff();
		  }
		  for (int i = 0; i <  mSelectedSurface.size(); i++)
		  {
			  mSelectedSurface[i].second->VisibilityOff();
		  }
		  for (int i = 0; i < mSelectedPolygon.size(); i++)
		  {
			  mSelectedPolygon[i].second->VisibilityOff();
		  }
		  
		  mw()->mInformation->finishAnnotation();
		  finishNote();
	  }
  }

  void SetAnnotationColor(ColorType color)
  {
	  //turnoffHighlight();
	  this->mColor = color;
	  mw()->VTKA()->update();
  }

  void initNote()
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
      vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
	  if (mNoteMode == SURFACENOTE || mNoteMode == FRUSTUMNOTE)
	  {
		 // vtkSmartPointer<vtkInteractorStyleRubberBandPick> style 
		//	  = vtkSmartPointer<vtkInteractorStyleRubberBandPick>::New();
		  vtkSmartPointer<vtkInteractorStyleRubberBand2D> style 
			  = vtkSmartPointer<vtkInteractorStyleRubberBand2D>::New();
		  mAreaPicker = vtkSmartPointer<vtkRenderedAreaPicker>::New();
		  style->AddObserver(vtkCommand::SelectionChangedEvent, this);
		  interactor->SetInteractorStyle(style);
		  style->SetCurrentRenderer(renderer);
		  interactor->SetPicker(mAreaPicker);
	  }
	  else
	  {
		  vtkSmartPointer<myVTKInteractorStyle> style = vtkSmartPointer<myVTKInteractorStyle>::New();
		  interactor->SetInteractorStyle(style);
		  style->SetCurrentRenderer(renderer);
	  }
  }	

  void finishNote()
  {
	  //vtkSmartPointer<vtkInteractorStyleImage> style =  vtkSmartPointer<vtkInteractorStyleImage>::New();
	  vtkSmartPointer<myVTKInteractorStyle> style =  vtkSmartPointer<myVTKInteractorStyle>::New();
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
      vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
	  interactor->SetInteractorStyle(style);
  }

  void SetFilename(QString filename)
  {
    this->mFilename = filename;
  }

  void SetViewer(vtkImageViewer2 *viewer)
  {
    this->Viewer = viewer;
	mRadius = Viewer->GetInput()->GetLength()/80;
  }

  void SetLightTransform (vtkTransform *transform) {
    this->mLightTransform = transform;
  }

  vtkTransform * GetLightTransform () {
    return this->mLightTransform;
  }

  void SetWindowLevels(double levels = 1.0, double window = 1.0) {
    // set minimum
    window = window>0.1 ? window : 0.1;
    levels = levels>0.1 ? levels : 0.1;
    if (mIsDICOM)
    {
        this->Viewer->SetColorLevel( 2 * DEFAULT_LEVELS /levels); // brightness
        this->Viewer->SetColorWindow( 2 * DEFAULT_WINDOW /window); // constrast
    } else {
        this->Viewer->SetColorLevel( DEFAULT_LEVELS /levels); // brightness
        this->Viewer->SetColorWindow( DEFAULT_WINDOW /window); // constrast
    }
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

      if (picker->GetCellId() != -1)
      {
		  double* point = new double[3];
          picker->GetPickPosition(point);

		  int* pointImageCoordinate = new int[3];
		  picker->GetPointIJK(pointImageCoordinate);

          if(!mw()->mInformation) return;

		   for (int i = 0; i < mSelectedPoint.size(); i++)
		  {
			  const double* center = mSelectedPoint[i].first;
			  double distant = sqrt(pow(center[0] - point[0], 2) + pow(center[1] - point[1],2));
			  if (distant <= mRadius)
			  {
				  return;
			  }	
		  }
		  qDebug() << "Draw Point Note";
		  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();

		  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
		  actor->SetMapper(mapper);
		  actor->PickableOff();
		  actor->GetProperty()->LightingOn();
		  actor->GetProperty()->SetColor(ColorPixel[mColor][0], ColorPixel[mColor][1], ColorPixel[mColor][2]);
		  actor->GetProperty()->SetLineWidth(2);
		  actor->VisibilityOn();
      
		  renderer->AddActor(actor);
		  mSelectedPoint.push_back(std::make_pair(point, actor));
		  displayPointNote(mapper, point);
		  mw()->mInformation->createPointNote2D(point, pointImageCoordinate, mColor);
	  }

  }

  void drawSurfaceNote(unsigned int* rect)
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
	  vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

	  double *point = new double[4];
	  double x0 = static_cast<double>(rect[0]);
	  double y0 = static_cast<double>(rect[1]);
	  double x1 = static_cast<double>(rect[2]);
	  double y1 = static_cast<double>(rect[3]);

	  vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
	  picker->SetTolerance(0.0005);
	  double* pos;
	  int startPointImageCoordinate[3], endPointImageCoordinate[3];

	  picker->Pick(x0, y0, 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	  if (picker->GetCellId() == -1)
	  {
		  return;
	  }
	  pos = picker->GetPickPosition();
	  point[0] = pos[0];
	  point[1] = pos[1];
	  picker->GetPointIJK(startPointImageCoordinate);

	  picker->Pick(x1, y1, 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	  if (picker->GetCellId() == -1)
	  {
		  return;
	  }

	  pos = picker->GetPickPosition();
	  point[2] = pos[0];
	  point[3] = pos[1];
	  picker->GetPointIJK(endPointImageCoordinate);
	  int imageCoordinate[4];
	  imageCoordinate[0] = startPointImageCoordinate[0];
	  imageCoordinate[1] = startPointImageCoordinate[1];
	  imageCoordinate[2] = endPointImageCoordinate[0];
	  imageCoordinate[3] = endPointImageCoordinate[1];

	  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();

	  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	  actor->SetMapper(mapper);
	  actor->PickableOff();
	  actor->GetProperty()->LightingOn();
	  actor->GetProperty()->SetColor(ColorPixel[mColor][0], ColorPixel[mColor][1], ColorPixel[mColor][2]);
	  actor->GetProperty()->SetLineWidth(2);
	  actor->VisibilityOn();

	  renderer->AddActor(actor);

	  mSelectedSurface.push_back(std::make_pair(point, actor));

	  displaySurfaceNote(mapper, point);

	  mw()->mInformation->createSurfaceNote2D(point, imageCoordinate, mColor);

  }


  void drawPolygonNote()
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
	  vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

	  /*Points.push_back(vtkSmartPointer<vtkPoints>::New());
	  PolyLine.push_back(vtkSmartPointer<vtkPolyLine>::New());
	  CellArray.push_back(vtkSmartPointer<vtkCellArray>::New());
	  PolyData.push_back(vtkSmartPointer<vtkPolyData>::New());
	  PolyDataMapper.push_back(vtkSmartPointer<vtkPolyDataMapper>::New());
	  Actor.push_back(vtkSmartPointer<vtkActor>::New());*/
	  Points.push_back(vtkPoints::New());
	  PolyLine.push_back(vtkPolyLine::New());
	  CellArray.push_back(vtkCellArray::New());
	  PolyData.push_back(vtkPolyData::New());
	  PolyDataMapper.push_back(vtkPolyDataMapper::New());
	  Actor.push_back(vtkActor::New());

	  std::vector<std::pair<double, double> >* polygonPointer;
	  std::vector<std::pair<int, int> >* polygonImagePointer;
	  vtkPolyDataMapper* polygonPolyDataMapper;
	  vtkActor* polygonActor;
	  vtkSmartPointer<vtkPolyDataMapper> polygonPolyDataMapperSmart;
	  vtkSmartPointer<vtkActor> polygonActorSmart;
	  bool isSuccess = false;

	  int currPos[2];
	  interactor->GetEventPosition(currPos);
	  vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
	  picker->SetTolerance(0.0005);
	  picker->Pick(currPos[0], currPos[1], 0, renderer);

	  if (picker->GetCellId() != -1)
	  {
		  double* point = new double[3];
		  picker->GetPickPosition(point);
		  int* pointImageCoordinate = new int[3];
		  picker->GetPointIJK(pointImageCoordinate);

		  if(!mw()->mInformation) return;

		  polygonPoints.push_back(std::make_pair(point[0], point[1]));
		  polygonImagePoints.push_back(std::make_pair(pointImageCoordinate[0], pointImageCoordinate[1]));
		  std::vector<std::pair<double, double> >::iterator it;
		  for (it = polygonPoints.begin(); it != polygonPoints.end(); it++)
			  Points.back()->InsertNextPoint(it->first, it->second, 0);

		  PolyLine.back()->GetPointIds()->SetNumberOfIds(polygonPoints.size());
		  for (int i = 0; i < polygonPoints.size(); i++)
			  PolyLine.back()->GetPointIds()->SetId(i, i);
		  if (polygonPoints.size() > 2 && abs(polygonPoints.back().first - polygonPoints.front().first) < 10
			  && abs(polygonPoints.back().second - polygonPoints.front().second) < 10)
		  {
			  PolyLine.back()->GetPointIds()->SetId(polygonPoints.size()-1, 0);
			  polygonPointer = new std::vector<std::pair<double, double> >;
			  polygonImagePointer = new std::vector<std::pair<int, int> >;
			  polygonPolyDataMapper = vtkPolyDataMapper::New();
			  polygonActor = vtkActor::New();
			  *polygonPointer = polygonPoints;
			  *polygonImagePointer = polygonImagePoints;
			  PolyDataMapper.back()->ShallowCopy(polygonPolyDataMapper);
			  Actor.back()->ShallowCopy(polygonActor);
			  polygonPolyDataMapperSmart.TakeReference(polygonPolyDataMapper);
			  polygonActorSmart.TakeReference(polygonActor);
			  mSelectedPolygon.push_back(std::make_pair(polygonPointer, polygonActorSmart));

			  std::vector<vtkActor*>::iterator itActorUpdateRenderer;
			  for (itActorUpdateRenderer = Actor.begin(); itActorUpdateRenderer != Actor.end(); itActorUpdateRenderer++)
				  renderer->RemoveActor(*itActorUpdateRenderer);
			  displayPolygonNote(polygonPolyDataMapperSmart, polygonPointer);
			  polygonActorSmart->SetMapper(polygonPolyDataMapperSmart);
			  polygonActorSmart->PickableOff();
			  polygonActorSmart->GetProperty()->LightingOn();
			  polygonActorSmart->GetProperty()->SetColor(ColorPixel[mColor][0], ColorPixel[mColor][1], ColorPixel[mColor][2]);
			  polygonActorSmart->GetProperty()->SetLineWidth(2);
			  polygonActorSmart->VisibilityOn();
			  renderer->AddActor(polygonActorSmart);
			  mw()->mInformation->createPolygonNote2D(polygonPointer, polygonImagePointer, mColor);

			  // Clear tmp drawings, non smart pointers should release memory manually
			  polygonPoints.clear();
			  polygonImagePoints.clear();
			  std::vector<vtkPoints*>::iterator itPoints, itPointsEnd = Points.end();
			  itPointsEnd--;
			  for (itPoints = Points.begin(); itPoints != itPointsEnd; itPoints++)
				  (*itPoints)->Delete();
			  Points.clear();
			  std::vector<vtkPolyLine*>::iterator itPolyLine, itPolyLineEnd = PolyLine.end();
			  itPolyLineEnd--;
			  for (itPolyLine = PolyLine.begin(); itPolyLine != itPolyLineEnd; itPolyLine++)
				  (*itPolyLine)->Delete();
			  PolyLine.clear();
			  std::vector<vtkCellArray*>::iterator itCellArray, itCellArrayEnd = CellArray.end();
			  itCellArrayEnd--;
			  for (itCellArray = CellArray.begin(); itCellArray != itCellArrayEnd; itCellArray++)
				  (*itCellArray)->Delete();
			  CellArray.clear();
			  std::vector<vtkPolyData*>::iterator itPolyData, itPolyDataEnd = PolyData.end();
			  itPolyDataEnd--;
			  for (itPolyData = PolyData.begin(); itPolyData != itPolyDataEnd; itPolyData++)
				  (*itPolyData)->Delete();
			  PolyData.clear();
			  std::vector<vtkPolyDataMapper*>::iterator itPolyDataMapper, itPolyDataMapperEnd = PolyDataMapper.end();
			  itPolyDataMapperEnd--;
			  for (itPolyDataMapper = PolyDataMapper.begin(); itPolyDataMapper != itPolyDataMapperEnd; itPolyDataMapper++)
				  (*itPolyDataMapper)->Delete();
			  PolyDataMapper.clear();
			  std::vector<vtkActor*>::iterator itActor, itActorEnd = Actor.end();
			  itActorEnd--;
			  for (itActor = Actor.begin(); itActor != itActorEnd; itActor++)
				  (*itActor)->Delete();
			  Actor.clear();

			  qDebug() << "Draw Polygon Note";
			  isSuccess = true;
		  }
		  if (!isSuccess)
		  {
			  CellArray.back()->InsertNextCell(PolyLine.back());
			  PolyData.back()->SetPoints(Points.back());
			  PolyData.back()->SetLines(CellArray.back());
			  PolyDataMapper.back()->SetInput(PolyData.back());
			  Actor.back()->SetMapper(PolyDataMapper.back());
			  Actor.back()->PickableOff();
			  Actor.back()->GetProperty()->LightingOn();
			  Actor.back()->GetProperty()->SetColor(ColorPixel[mColor][0], ColorPixel[mColor][1], ColorPixel[mColor][2]);
			  Actor.back()->GetProperty()->SetLineWidth(2);
			  Actor.back()->VisibilityOn();
			  renderer->AddActor(Actor.back());
		  }
	  }
	  
  }
  void displayPointNote(vtkSmartPointer<vtkPolyDataMapper> mapper, double* select)
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();

	  vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

	  double p0[3] = {select[0], select[1], 0.0};
	  double p1[3] = {select[0] - mRadius, select[1], 0.0};
	  double p2[3] = {select[0] + mRadius, select[1], 0.0};
	  double p3[3] = {select[0], select[1] + mRadius, 0.0};
	  double p4[3] = {select[0], select[1] - mRadius, 0.0};
	  qDebug()<<"radius = " <<mRadius;
	  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

	  points->InsertNextPoint(p0);
	  points->InsertNextPoint(p1);
	  points->InsertNextPoint(p2);
	  points->InsertNextPoint(p3);
	  points->InsertNextPoint(p4);

	  vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
	  polyLine->GetPointIds()->SetNumberOfIds(8);
	  polyLine->GetPointIds()->SetId(0,0);
	  polyLine->GetPointIds()->SetId(1,1);
	  polyLine->GetPointIds()->SetId(2,0);
	  polyLine->GetPointIds()->SetId(3,2);
	  polyLine->GetPointIds()->SetId(4,0);
	  polyLine->GetPointIds()->SetId(5,3);
	  polyLine->GetPointIds()->SetId(6,0);
	  polyLine->GetPointIds()->SetId(7,4);


	  vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	  cells->InsertNextCell(polyLine);

	  vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	  polyData->SetPoints(points);
	  polyData->SetLines(cells);
	  mapper->SetInput(polyData);
  }

  void displaySurfaceNote(vtkSmartPointer<vtkPolyDataMapper> mapper, double* select)
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
	  vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
 
	  double p0[3] = {select[0], select[1], 0.0};
	  double p1[3] = {select[2], select[1], 0.0};
	  double p2[3] = {select[0], select[3], 0.0};
	  double p3[3] = {select[2], select[3], 0.0};

	  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

	  points->InsertNextPoint(p0);
	  points->InsertNextPoint(p1);
	  points->InsertNextPoint(p2);
	  points->InsertNextPoint(p3);

	  vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
      polyLine->GetPointIds()->SetNumberOfIds(5);

	  polyLine->GetPointIds()->SetId(0,0);
	  polyLine->GetPointIds()->SetId(1,1);
	  polyLine->GetPointIds()->SetId(2,3);
	  polyLine->GetPointIds()->SetId(3,2);
	  polyLine->GetPointIds()->SetId(4,0);

	  vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	  cells->InsertNextCell(polyLine);
 
      vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
      polyData->SetPoints(points);
      polyData->SetLines(cells);
	  mapper->SetInput(polyData);
  }

  void displayPolygonNote(vtkSmartPointer<vtkPolyDataMapper> mapper, std::vector<std::pair<double, double> >* select)
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
	  vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

	  vtkSmartPointer<vtkPoints> Points = vtkSmartPointer<vtkPoints>::New();
	  vtkSmartPointer<vtkPolyLine> PolyLine = vtkSmartPointer<vtkPolyLine>::New();
	  vtkSmartPointer<vtkCellArray> CellArray = vtkSmartPointer<vtkCellArray>::New();
	  vtkSmartPointer<vtkPolyData> PolyData = vtkSmartPointer<vtkPolyData>::New();

	  std::vector<std::pair<double, double> >::iterator it;
	  for (it = select->begin(); it != select->end(); it++)
		  Points->InsertNextPoint(it->first, it->second, 0);;
	  PolyLine->GetPointIds()->SetNumberOfIds(select->size());
	  for (int i = 0; i < select->size(); i++)
		  PolyLine->GetPointIds()->SetId(i, i);
	  if (select->size() > 2 && abs(select->back().first - select->front().first) < 10
		  && abs(select->back().second - select->front().second) < 10)
		  PolyLine->GetPointIds()->SetId(select->size()-1, 0);
	  CellArray->InsertNextCell(PolyLine);
	  PolyData->SetPoints(Points);
	  PolyData->SetLines(CellArray);
	  mapper->SetInput(PolyData);
	  
  }

  void highlightPointNote(int id)
  {
	  double* color = new double[3];
	  if (mPointNoteHighlight != -1 || mSurfaceNoteHighlight != -1 || mPolygonNoteHighlight != -1)
	  {
		  //turnoffHighlight();
	  }
	  mPointNoteHighlight = id;
	  mSelectedPoint[mPointNoteHighlight].second->GetProperty()->GetColor(color);
	  mSelectedPoint[mPointNoteHighlight].second->GetProperty()->SetColor(1-color[0], 1-color[1], 1-color[2]);
  }

  void highlightSurfaceNote(int id)
  {
	  double* color = new double[3];
	  if (mPointNoteHighlight != -1 || mSurfaceNoteHighlight != -1 || mPolygonNoteHighlight != -1)
	  {
		  //turnoffHighlight();
	  }
	  mSurfaceNoteHighlight = id;
	  mSelectedSurface[mSurfaceNoteHighlight].second->GetProperty()->GetColor(color);
	  mSelectedSurface[mSurfaceNoteHighlight].second->GetProperty()->SetColor(1-color[0], 1-color[1], 1-color[2]);
  }

  void highlightPolygonNote(int id)
  {
	  double* color = new double[3];
	  if (mPointNoteHighlight != -1 || mSurfaceNoteHighlight != -1 || mPolygonNoteHighlight != -1)
	  {
		  //turnoffHighlight();
	  }
	  mPolygonNoteHighlight = id;
	  mSelectedPolygon[mPolygonNoteHighlight].second->GetProperty()->GetColor(color);
	  mSelectedPolygon[mPolygonNoteHighlight].second->GetProperty()->SetColor(1-color[0], 1-color[1], 1-color[2]);
	  
  }

  void turnoffHighlight()
  {
	   double* color = new double[3];
	   if (mPointNoteHighlight != -1 && mPointNoteHighlight < mSelectedPoint.size())
	   {
		   mSelectedPoint[mPointNoteHighlight].second->GetProperty()->GetColor(color);
		   mSelectedPoint[mPointNoteHighlight].second->GetProperty()->SetColor(1-color[0], 1-color[1], 1-color[2]);
		   mPointNoteHighlight = -1;
	   }
	   if (mSurfaceNoteHighlight != -1 && mSurfaceNoteHighlight < mSelectedSurface.size()) 
	   {
		   mSelectedSurface[mSurfaceNoteHighlight].second->GetProperty()->GetColor(color);		 
		   mSelectedSurface[mSurfaceNoteHighlight].second->GetProperty()->SetColor(1-color[0], 1-color[1], 1-color[2]);		  
		   mSurfaceNoteHighlight = -1;		  
	   }
	   if (mPolygonNoteHighlight != -1 && mPolygonNoteHighlight < mSelectedSurface.size()) 
	   {
		   mSelectedPolygon[mPolygonNoteHighlight].second->GetProperty()->GetColor(color);		 
		   mSelectedPolygon[mPolygonNoteHighlight].second->GetProperty()->SetColor(1-color[0], 1-color[1], 1-color[2]);		  
		   mPolygonNoteHighlight = -1;		  
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

      picker->Pick(currPos[0], currPos[1], 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

      if (picker->GetCellId() != -1)
      {
		  double pos[3];
          picker->GetPickPosition(pos);
		  if(!mw()->mInformation) return false;
		  for (int i = 0; i < mSelectedPoint.size(); i++)
		  {
			  if (!mSelectedPoint[i].second->GetVisibility())
				  continue;
			  const double* center = mSelectedPoint[i].first;
			  double distant = sqrt(pow(center[0] - pos[0], 2) + pow(center[1] - pos[1],2));
			  //qDebug()<<"radius = "<<mRadius<<" distant = "<<distant;
			  if (distant <= mRadius)
			  {
				  //qDebug() << "found Point Note!!";
				  //highlightPointNote(i);
				  mw()->mInformation->openPointNote2D(mSelectedPoint[i].first);
				  return true;
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
      picker->SetTolerance(0.0005);

      picker->Pick(currPos[0], currPos[1], 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

      if (picker->GetCellId() != -1)
      {
		  double pos[3];
          picker->GetPickPosition(pos);
		  if(!mw()->mInformation) return false;

		  for (int i = 0; i < mSelectedSurface.size(); i++)
		  {
			  if (!mSelectedSurface[i].second->GetVisibility())
				  continue;
			  const double* edge = mSelectedSurface[i].first;
			  double maxX = std::max(edge[0], edge[2]);
			  double minX = std::min(edge[0], edge[2]);
			  double maxY = std::max(edge[1], edge[3]);
			  double minY = std::min(edge[1], edge[3]);
			  
			  if (pos[0] >= minX && pos[0] <= maxX && pos[1] >= minY && pos[1] <= maxY)
			  {
				  //qDebug() << "found!!";
				  //qDebug()<<pos[0]<<pos[1] << edge[0] << edge[1] << edge[2] << edge[3];
				  //highlightSurfaceNote(i);
				  mw()->mInformation->openSurfaceNote2D(mSelectedSurface[i].first);
				  return true;
			  }
		  }         
      }
	  return false;
  }

  bool choosePolygonNote()
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
      vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

	  int currPos[2]; 
      interactor->GetEventPosition(currPos);
      vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
      picker->SetTolerance(0.0005);

      picker->Pick(currPos[0], currPos[1], 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

      if (picker->GetCellId() != -1)
      {
		  double pos[3];
          picker->GetPickPosition(pos);
		  int posImg[3];
		  picker->GetPointIJK(posImg);
		  if(!mw()->mInformation) return false;
		  for (int i = 0; i < mSelectedPolygon.size(); i++)
		  {
			  if (!mSelectedPolygon[i].second->GetVisibility())
				  continue;
			  std::vector<std::pair<double, double> >* select = mSelectedPolygon[i].first;
			  double maxX = 0, maxY = 0, minX = 0xFFFF, minY = 0xFFFF;
			  std::vector<std::pair<double, double> >::iterator it;
			  for (it = select->begin(); it != select->end(); it++)
			  {
				  if (it->first > maxX) maxX = it->first;
				  if (it->first < minX) minX = it->first;
				  if (it->second > maxY) maxY = it->second;
				  if (it->second < minY) minY = it->second;
			  }
			  if (pos[0] >= minX && pos[0] <= maxX && pos[1] >= minY && pos[1] <= maxY)
			  {
				  //qDebug() << "found!!";
				  //qDebug() << pos[0] << pos[1] << maxX << minX << maxY << minY;
				  //highlightPolygonNote(i);
				  mw()->mInformation->openPolygonNote2D(mSelectedPolygon[i].first);
				  return true;
			  }
		  }
	  }
	  return false;
	  
  }

  void removePointNoteMark(double* point)
  {
	  bool erase = false;
	  for (int i = 0; i < mSelectedPoint.size(); ++i) 
	  {
		  const double* select = mSelectedPoint[i].first;
		  bool isSame = true;
		  for (int j = 0; j < 3; j++)
		  {
			  if (select[j] != point[j]) //// HAZARD TEST DOUBLE
			  {
				  isSame = false;
				  break;
			  }
		  }
		  if (isSame)
		  {
			  mSelectedPoint[i].second->VisibilityOff();
			  mSelectedPoint.erase(mSelectedPoint.begin() + i);
			  erase = true;
			  break;
		  }
	  }
	  if (!erase)	qDebug() << "Cannot Find the Exact PointNote to remove!" << endl;
  }

  void removeSurfaceNoteMark(double* point)
  {
	  qDebug()<<"in remove surface note mark";
	  bool erase = false;
	  for (int i = 0; i < mSelectedSurface.size(); ++i) 
	  {
		  const double* select = mSelectedSurface[i].first;
		  bool isSame = true;
		  for (int j = 0; j  < 4; j++)
		  {
			  if (select[j] != point[j]) //// HAZARD TEST DOUBLE
			  {
				  isSame = false;
				  break;
			  }
		  }
		  if (isSame)
		  {
			  mSelectedSurface[i].second->VisibilityOff();
			  mSelectedSurface.erase(mSelectedSurface.begin() + i);
			  erase = true;
			  break;
		  }
	  }
	  if (!erase)	qDebug() << "Cannot Find the Exact SurfaceNote to remove!" << endl;
  }

  void removePolygonNoteMark(std::vector<std::pair<double, double> >* polygon)
  {
	  bool erase = false;
	  for (int i = 0; i < mSelectedPolygon.size(); ++i) 
	  {
		  std::vector<std::pair<double, double> >* select = mSelectedPolygon[i].first;
		  bool isSame = true;
		  std::vector<std::pair<double, double> >::iterator it1, it2;
		  for (it1 = select->begin(), it2 = polygon->begin(); it1 != select->end() && it2 != polygon->end(); ++it1, ++it2)
		  {
			  if (abs(it1->first - it2->first) > 0.000001 || abs(it1->second - it2->second) > 0.000001)
			  {
				  isSame = false;
				  break;
			  }
		  }
		  if (isSame)
		  {
			  mSelectedPolygon[i].second->VisibilityOff();
			  mSelectedPolygon.erase(mSelectedPolygon.begin() + i);
			  erase = true;
			  break;
		  }
	  }
	  if (!erase)	qDebug() << "Cannot Find the Exact PolygonNote to remove!" << endl;
	  
  }

  void openPointNoteMark(double* point)
  {
	  for (int i = 0; i < mSelectedPoint.size(); ++i) 
	  {
		  const double* select = mSelectedPoint[i].first;
		  bool isSame = true;
		  for (int j = 0; j < 3; j++)
		  {
			  if (select[j] != point[j]) //// HAZARD TEST DOUBLE
			  {
				  isSame = false;
				  break;
			  }
		  }
		  if (isSame)
		  {
			  mSelectedPoint[i].second->VisibilityOn();
			  break;
		  }
	  }
  }

  void openSurfaceNoteMark(double* point)
  {
	  for (int i = 0; i < mSelectedSurface.size(); ++i) 
	  {
		  const double* select = mSelectedSurface[i].first;
		  bool isSame = true;
		  for (int j = 0; j  < 4; j++)
		  {
			  if (select[j] != point[j]) //// HAZARD TEST DOUBLE
			  {
				  isSame = false;
				  break;
			  }
		  }
		  if (isSame)
		  {
			  mSelectedSurface[i].second->VisibilityOn();
			  break;
		  }
	  }
  }

  void openPolygonNoteMark(std::vector<std::pair<double, double> >* polygon)
  {
	  for (int i = 0; i < mSelectedPolygon.size(); ++i) 
	  {
		  std::vector<std::pair<double, double> >* select = mSelectedPolygon[i].first;
		  bool isSame = true;
		  std::vector<std::pair<double, double> >::iterator it1, it2;
		  for (it1 = select->begin(), it2 = polygon->begin(); it1 != select->end() && it2 != polygon->end(); ++it1, ++it2)
		  {
			  if (abs(it1->first - it2->first) > 0.000001 || abs(it1->second - it2->second) > 0.000001)
			  {
				  isSame = false;
				  break;
			  }
		  }
		  if (isSame)
		  {
			  mSelectedPolygon[i].second->VisibilityOn();
			  break;
		  }
	  }
	  
  }

  void displayLoadPointNote(double* point, const ColorType color, bool isDisplay = false)
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
	  vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
	  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	  actor->SetMapper(mapper);
	  actor->PickableOff();
	  actor->GetProperty()->LightingOn();
      actor->GetProperty()->SetColor(ColorPixel[color][0], ColorPixel[color][1], ColorPixel[color][2]);
      actor->GetProperty()->SetLineWidth(2);
	  if (isDisplay)
		  actor->VisibilityOn();
	  else
		  actor->VisibilityOff();

      renderer->AddActor(actor);
	  mSelectedPoint.push_back(std::make_pair(point, actor));

	  displayPointNote(mapper, point);
  }

  void displayLoadSurfaceNote(double* point, const ColorType color, bool isDisplay = false)
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
	  vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
	  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	  actor->SetMapper(mapper);
	  actor->PickableOff();
	  actor->GetProperty()->LightingOn();
      actor->GetProperty()->SetColor(ColorPixel[color][0], ColorPixel[color][1], ColorPixel[color][2]);
      actor->GetProperty()->SetLineWidth(2);
	  if (isDisplay)
		  actor->VisibilityOn();
	  else
		  actor->VisibilityOff();

      renderer->AddActor(actor);
	  mSelectedSurface.push_back(std::make_pair(point, actor));

	  displaySurfaceNote(mapper, point);
  }

  void displayLoadPolygonNote(std::vector<std::pair<double, double> >* polygon, const ColorType color, bool isDisplay = false)
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
	  vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
	  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	  actor->SetMapper(mapper);
	  actor->PickableOff();
	  actor->GetProperty()->LightingOn();
      actor->GetProperty()->SetColor(ColorPixel[color][0], ColorPixel[color][1], ColorPixel[color][2]);
      actor->GetProperty()->SetLineWidth(2);
	  if (isDisplay)
		  actor->VisibilityOn();
	  else
		  actor->VisibilityOff();

      renderer->AddActor(actor);
	  mSelectedPolygon.push_back(std::make_pair(polygon, actor));

	  displayPolygonNote(mapper, polygon);

	  
  }

  void updateLightingPosition() {
    vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
    vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
	if (!renderer)
		return;
    vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
    vtkSmartPointer<vtkLightCollection> lights = renderer->GetLights();
	if (lights)
	{
		lights->InitTraversal();
		vtkSmartPointer<vtkLight> light1 = lights->GetNextItem();
		if (light1)
		{
			light1->SetPosition(camera->GetPosition());
			light1->SetTransformMatrix(mLightTransform->GetMatrix());
		}

		vtkSmartPointer<vtkLight> light2 = lights->GetNextItem();
		if (light2)
		{
			light2->SetPosition(camera->GetPosition()); // correct
		}
		#ifdef SHOWCAMERALIGHT
		displayInfoAnnotation(true);
		#endif
	}
  }

  void SetInfoAnnotation (vtkCornerAnnotation *annotation) {
    this->mInfoAnnotation = annotation;

    mInfoAnnotation->SetLinearFontScaleFactor(2);
    mInfoAnnotation->SetNonlinearFontScaleFactor(1);
    mInfoAnnotation->SetMaximumFontSize(14); // decide the font size
  // not working
  //  mInfoAnnotation->SetPosition(INFOPANELX, INFOPANELY); // lower left
  //  mInfoAnnotation->SetPosition2(0.8,0.8); // upper right

    vtkSmartPointer<vtkTextProperty> tproperty = mInfoAnnotation->GetTextProperty();
    tproperty->SetFontFamilyToArial();
  //  tproperty->SetFontSize(12); // this doesn't work here
    tproperty->BoldOn();
    tproperty->ShadowOn();
    tproperty->SetColor(1, 1, 1);
    mInfoAnnotation->SetTextProperty(tproperty);
  }

  void displayInfoAnnotation(bool start = false)
  {
    vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();

    std::string svalue = "";
    std::string shvalue = "";
    char text[MAX_LINE]; memset( text, 0, sizeof(text) );
    int dims[3] = {0,0,0}; // RGB image dimension
    int hdims[3] = {0,0,0}; // hyperspectral image dimension
    int icoords[3] = {0,0,0};
    int numCompo = 0;

    vtkSmartPointer<vtkRenderer> renderer = this->Viewer->GetRenderer();
    vtkSmartPointer<vtkImageActor> actor = this->Viewer->GetImageActor();
    vtkSmartPointer<vtkImageData> image = this->Viewer->GetInput();

    if (image)
    {
        image->GetDimensions(dims);
        numCompo = image->GetNumberOfScalarComponents();
    }

    if ( start )
    {
//      image->UpdateInformation();
//      image->SetUpdateExtentToWholeExtent();
//      image->Update();


      // this is proppicker
      this->CurrPicker->Pick(interactor->GetEventPosition()[0],
                         interactor->GetEventPosition()[1],
                         0.0, renderer);
  #ifdef SUPPORT_LASTPICK
      this->LastPicker->Pick(interactor->GetLastEventPosition()[0],
                         interactor->GetLastEventPosition()[1],
                         0.0, renderer);
  #endif

//      vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
//      picker->Pick(interactor->GetEventPosition()[0], interactor->GetEventPosition()[1], 0, renderer);
      this->CurrPicker->GetPointIJK(icoords); // Reading the texture coordinates.

      // There could be other props assigned to this picker, so
      // make sure we picked the image actor
      vtkSmartPointer<vtkAssemblyPath> path = this->CurrPicker->GetPath();
      bool validPick = false;

      if (path) // correct!
        {
        vtkCollectionSimpleIterator sit;
        path->InitTraversal(sit);
        vtkSmartPointer<vtkAssemblyNode> node;
        for (int i = 0; i < path->GetNumberOfItems() && !validPick; ++i)
          {
          node = path->GetNextNode(sit);
          if (actor == vtkImageActor::SafeDownCast(node->GetViewProp()))
            {
            validPick = true;
            }
          }
        }

      if (!validPick)
        {
  //        std::string emptymessage = "Off Image \n";
  //        this->Annotation->SetText(0, emptymessage.c_str());
  //        interactor->Render();
        // Pass the event further on
  //      return;
        }

      // Get the world coordinates of the pick
      double currPos[3];
      this->CurrPicker->GetPickPosition(currPos); // suspicious
  #ifdef SUPPORT_LASTPICK
      double lastPos[3];
      this->LastPicker->GetPickPosition(lastPos);
  #endif

      //------------------------------------------------------------------------------------------
      //MK: here currPos is not correct, we need to interpolate the position to get an image value:
      //vtkSmartPointer<vtkPointData> pd = this->Viewer->GetImageActor()->GetInput()->GetPointData();
      vtkSmartPointer<vtkPointData> pd = image->GetPointData();
      if( !pd ) return;

      vtkSmartPointer<vtkPointData> outPD = vtkSmartPointer<vtkPointData>::New();
      outPD->InterpolateAllocate( pd, 1, 1 );
      // Use tolerance as a function of size of source data
      double tol2 = image->GetLength();
      tol2 = tol2 ? (tol2*tol2 / 1000.0) : 0.001;

      // Find the cell that contains global coordinates
      int subId;
      double pcoords[3], weights[8];
      //MK: pcoords is normalized coordinates.
      //vtkSmartPointer<vtkCell> cell = this->Viewer->GetImageActor()->GetInput()->FindAndGetCell(currPos, NULL, -1, tol2, subId, pcoords, weights ); // find cell from global coordinates
      vtkSmartPointer<vtkCell> cell = image->FindAndGetCell(currPos, NULL, -1, tol2, subId, pcoords, weights ); // find cell from global coordinates
      // this above pcoord is the normalized real coordinates.

      if( cell )
        {
          //------------------------------------------------------------------------------------------
          // get the image coordinates  for reading hyperspectral data
          // NB: pcoords is not real coordinates.
          //currPos[0] = pcoords[0]*dims[0];     currPos[1] = pcoords[1]*dims[1];     currPos[2] = pcoords[2]*dims[2];
          int axis = this->Viewer->GetSliceOrientation();
          switch (axis)
            {
            case vtkImageViewer2::SLICE_ORIENTATION_XZ:
              icoords[0] = icoords[0];
              icoords[1] = this->Viewer->GetSlice();
              icoords[2] = icoords[2];
              break;
            case vtkImageViewer2::SLICE_ORIENTATION_YZ:
              icoords[0] = this->Viewer->GetSlice();
              icoords[1] = icoords[1];
              icoords[2] = icoords[2];
              break;
            default:  // vtkImageViewer2::SLICE_ORIENTATION_XY
              icoords[0] = icoords[0];
              icoords[1] = icoords[1];
              icoords[2] = this->Viewer->GetSlice();
              break;
            }
          //------------------------------------------------------------------------------------------

          //----------------------------------------------------------------
          // for RGB screen values
          // Interpolate the point data
          outPD->InterpolatePoint( pd, 0, cell->PointIds, weights );
          double* tuple = outPD->GetScalars()->GetTuple( 0 );
          int components1 = outPD->GetScalars()->GetNumberOfComponents();
          svalue += "( ";
          for(int i=0;i<components1;++i)
          {
            svalue += vtkVariant(tuple[i]).ToString();
            // remove comma at the end
            if (i != (components1 - 1))
            {
              svalue += ", ";
            }
          }
          svalue += " )";
//          sprintf( text, " [Num Ch.: %i ]", components1 );
//          svalue += text;
          //----------------------------------------------------------------
          // just read hyperspectral pixel value from the coordinates.
          if (mHyperImageData != NULL)
              mHyperImageData->GetDimensions(hdims);
          int components2 = 0;
          shvalue += "( ";
          if (hdims[0] != 0 || hdims[1] != 0 ) // if there is hyperspectral data
          {
              components2 = mHyperImageData->GetNumberOfScalarComponents();
              std::vector<float> hyperPixels;
//              hyperPixels.push_back(0);
              for(int i=0 ;i<components2;++i) // start from 3 (RGB excluded)
              {
                float value = mHyperImageData->GetScalarComponentAsFloat( icoords[0], icoords[1], icoords[2], i);
                hyperPixels.push_back(value);
                shvalue += vtkVariant(value).ToString();
                // remove comma at the end
                if (i != (components2 - 1))
                {
                  shvalue += ", ";
                }
              }
//              hyperPixels.push_back(0);
              shvalue += " )";
              sprintf( text, " [Num Ch.: %i ]", components2 );
              shvalue += text;

              // vector copy
              if (hyperPixels.size() > 0)
              {
                mHyperPixels.resize(hyperPixels.size());
                std::copy(hyperPixels.begin(), hyperPixels.end(), mHyperPixels.begin());
              }
          } else { // in case only RGB
            components2 = image->GetNumberOfScalarComponents();
            std::vector<float> hyperPixels;
            hyperPixels.push_back(0);
            for(int i=components2-1;i>=0;i--)
            {
              float value = image->GetScalarComponentAsFloat( icoords[0], icoords[1], icoords[2], i); // icoords is not correct
              //float value = tuple[i]; // from interpolation reading
              value = pow(value/255.f,GAMMA);
              hyperPixels.push_back(value);
              shvalue += vtkVariant(value).ToString();
              // remove comma at the end
              if (i != 0)
              {
                shvalue += ", ";
              }
            }
            hyperPixels.push_back(0);
            shvalue += " )";
            sprintf( text, " [Num Ch.: %i ]", components2 );
            shvalue += text;

            // vector copy
            if (hyperPixels.size() > 0)
            {
              mHyperPixels.resize(hyperPixels.size());
              std::copy(hyperPixels.begin(), hyperPixels.end(), mHyperPixels.begin());
            }
          } // end of else
        } // end of if
    } // end of if

    //----------------------------------------------------------------------------
    // not correct for CT data
    std::string messageUL = "\n<window>    \n<level>    ";
    std::string messageUR = "\n    ";
    messageUR += mGLversion.toStdString();
    messageUR += mNumCore.toStdString();

    std::string messageLR = "";
    std::string messageLL = "";

    /*
     // DT: Commented out since this is in the File Info dialog now.
    messageLL += "    Geometry Filename: ";
    messageLL += mFilename.toLocal8Bit().constData();
    messageLL += "\n    Image Dimensions: ";
//    if (start) {
//      sprintf( text, " ( %i x %i x %i )", hdims[0]>dims[0] ? hdims[0]:dims[0], hdims[1]>dims[1] ? hdims[1]:dims[1], hdims[2]>dims[2] ? hdims[2]:dims[2]);
        if (mIsDICOM)
            sprintf( text, " ( %i x %i x %i )", dims[0], dims[1], dims[2]);
        else
            sprintf( text, " ( %i x %i x %i )", dims[0], dims[1], numCompo);
      messageLL += text;
//    }
*/
    messageLL += "\n\n\n    Image Coordinates: ";
    if (start) {
      sprintf( text, "( %i,  %i, %i )", icoords[0], icoords[1], icoords[2] ); // vtk original coordinates
      messageLL += text;
    }

    messageLL += "\n    Image Pixel Values: ";
    if (start) {
      messageLL += svalue;
    }

#ifdef SHOWHYPERVALUES
    messageLL += "\n    Hyperspectral Texture Values: ";
    if (start) {
      messageLL += shvalue;
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
  } // end of display

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


  // 1) middle mouse + move = camera pan
  // 2) left mouse + move = window/level
  // 3) right mouse + move = camera zoom
  // 4) middle mouse wheel scroll = zoom
  // 5) 'r' = reset window/level
  // 6) shift + 'r' = reset camera
  virtual void Execute(vtkObject *, unsigned long event, void * callData)
  {
	  vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
	  vtkSmartPointer<vtkInteractorStyle> style = vtkInteractorStyle::SafeDownCast(interactor->GetInteractorStyle()); // unstable

	  displayInfoAnnotation(true);
	  //qDebug()<<"Execute " <<event;
	  if (event == vtkCommand::LeftButtonPressEvent)
	  {
		  this->Slicing = 1;
		  if (mUserIsAnnotating)
		  {
			  if (!choosePointNote() && !chooseSurfaceNote() && !choosePolygonNote())
			  {
				  //turnoffHighlight();
			  }
		  }
		  if (style)  { style->OnLeftButtonDown(); }
	  }
	  else if (event == vtkCommand::LeftButtonReleaseEvent)
	  {
		  this->Slicing = 0;
		  if (style)  { style->OnLeftButtonUp(); }
	  }
	  else if (event == vtkCommand::RightButtonPressEvent)
	  {
		  //        if (style)  { style->OnRightButtonDown(); }
		  if (mUserIsAnnotating && mNoteMode == POINTNOTE)
		  {
			  drawPointNote();
		  }
		  else if (mUserIsAnnotating && mNoteMode == POLYGONNOTE)
		  {
			  drawPolygonNote();
		  }
	  }
	  else if (event == vtkCommand::RightButtonReleaseEvent)
	  {
		  //        if (style)  { style->OnRightButtonUp(); }
	  }
	  else if (event == vtkCommand::MiddleButtonPressEvent)
	  {
		  if (style)  { style->OnMiddleButtonDown(); }
	  }
	  else if (event == vtkCommand::MiddleButtonReleaseEvent)
	  {
		  if (style)  { style->OnMiddleButtonUp(); }
	  }
	  else if (event == vtkCommand::MouseWheelForwardEvent)
	  {
		  if (style)  { style->OnMouseWheelForward(); }
	  }
	  else if (event == vtkCommand::MouseWheelBackwardEvent)
	  {
		  if (style)  { style->OnMouseWheelBackward(); }
	  }
	  else if (event == vtkCommand::MouseMoveEvent)
	  {
#ifdef SUPPORT_LASTPICK
		  if (this->Slicing)
		  {
			  vtkSmartPointer<vtkImageReslice> reslice = this->ImageReslice;

			  // Increment slice position by deltaY of mouse
			  int deltaY = lastPos[1] - currPos[1];

			  reslice->GetOutput()->UpdateInformation();
			  double sliceSpacing = reslice->GetOutput()->GetSpacing()[2];
			  vtkSmartPointer<vtkMatrix4x4> matrix = reslice->GetResliceAxes();
			  // move the center point that we are slicing through
			  double point[4];
			  double center[4];
			  point[0] = 0.0;
			  point[1] = 0.0;
			  point[2] = sliceSpacing * deltaY;
			  point[3] = 1.0;
			  matrix->MultiplyPoint(point, center);
			  matrix->SetElement(0, 3, center[0]);
			  matrix->SetElement(1, 3, center[1]);
			  matrix->SetElement(2, 3, center[2]);
			  interactor->Render();
		  }
#endif
		  if (style)  { style->OnMouseMove(); }
	  }
	  else if (event == vtkCommand::SelectionChangedEvent)
	  {
		  //qDebug()<<"Selection Changes";
		  unsigned int* rect = reinterpret_cast<unsigned int*> ( callData );
		  if (mUserIsAnnotating && rect[0] != rect[2] && rect[1] != rect[3] && mNoteMode == SURFACENOTE)
		  {
			  drawSurfaceNote(rect);
		  }
	  }
	  // end of else if

	  updateLightingPosition();// this should come first
	  interactor->Render();
  }// end of execute
private:
  vtkSmartPointer<QVTKInteractor>       Interactor;  // Pointer to the interactor
  vtkSmartPointer<vtkImageReslice>      ImageReslice;// Pointer to vtkImageReslice
  vtkSmartPointer<vtkImageViewer2>      Viewer;      // Pointer to the viewer
//  vtkSmartPointer<vtkPropPicker>        CurrPicker;      // Pointer to the picker
//  vtkSmartPointer<vtkPropPicker>        LastPicker;      // Pointer to the picker
  vtkSmartPointer<vtkCellPicker>        CurrPicker;      // Pointer to the picker
  vtkSmartPointer<vtkCellPicker>        LastPicker;      // Pointer to the picker
  vtkSmartPointer<vtkCornerAnnotation>  mInfoAnnotation;  // Pointer to the annotation
  vtkSmartPointer<vtkTransform>         mLightTransform; // pointer to the light

  vtkSmartPointer<vtkAreaPicker> mAreaPicker;
  
  bool mIsDICOM;
  QString mFilename;
  int Slicing; // Actions (slicing only, for now)

  // texture data
  vtkSmartPointer<vtkImageData> mHyperImageData; // for hyperspectral data

  QString mGLversion;
  QString mNumCore;
  std::vector<float> mHyperPixels;

  bool mDisplayInfoOn;
  bool mUserIsAnnotating;
  NoteMode mNoteMode;
  std::vector<std::pair<double*, vtkSmartPointer<vtkActor> > > mSelectedSurface;
  std::vector<std::pair<double*, vtkSmartPointer<vtkActor> > > mSelectedPoint;
  std::vector<std::pair<double, double> > polygonPoints;
  std::vector<std::pair<int, int> > polygonImagePoints;
  std::vector<std::pair<std::vector<std::pair<double,double> >*, vtkSmartPointer<vtkActor> > > mSelectedPolygon;
  int mPointNoteHighlight;
  int mSurfaceNoteHighlight;
  int mPolygonNoteHighlight;
  double mRadius;
  ColorType mColor;

  // For proper display of polygons
  /*std::vector<vtkSmartPointer<vtkPoints> > Points;
  std::vector<vtkSmartPointer<vtkPolyLine> > PolyLine;
  std::vector<vtkSmartPointer<vtkCellArray> > CellArray;
  std::vector<vtkSmartPointer<vtkPolyData> > PolyData;
  std::vector<vtkSmartPointer<vtkPolyDataMapper> > PolyDataMapper;
  std::vector<vtkSmartPointer<vtkActor> > Actor;*/
  std::vector<vtkPoints*> Points;
  std::vector<vtkPolyLine*> PolyLine;
  std::vector<vtkCellArray*> CellArray;
  std::vector<vtkPolyData*> PolyData;
  std::vector<vtkPolyDataMapper*> PolyDataMapper;
  std::vector<vtkActor*> Actor;
};

#endif // VTK2DINTERACTIONCALLBACK_HPP
