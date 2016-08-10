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
#include <QMouseEvent>
#include <qwt_knob.h>
#include <qwt_round_scale_draw.h>
#include <qwt_math.h>
//vtk objects
#include <vtkSphereSource.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>

#include <vtkLight.h>
#include <QVTKInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleTrackballActor.h>
//#include <vtkInteractorStyleTrackballActor.h>
//#include <vtkInteractorStyleJoystickCamera.h>
//#include <vtkInteractorStyleJoystickActor.h>
#include <vtkAssembly.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkRendererCollection.h>
#include <vtkVector.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkActorCollection.h>
#include <vtkLightCollection.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkAssemblyPath.h>
#include <vtkAxesActor.h>
#include <vtkVectorText.h>
#include <vtkSelectEnclosedPoints.h>
#include <vtkPointData.h>
#include <vtkObjectFactory.h>
#include <vtkProp3DCollection.h>
#include <vtkMath.h>
#include <vtkLightActor.h>
#include <vtkProp.h>

#include "../mainWindow.h"
#include "lightControl.h"
#include "knobWidget.h"
#include "mkTools.hpp"
#include "../visualization/vtkWidget.h"

#define CAMERA_DOLLY (1.2)  // bigger number makes the object bigger

//---------------------------------------------------------------------------------------------
// for 3D interaction callback.
class vtk3DLightInteractionCallback : public vtkCommand
{
public:

  static vtk3DLightInteractionCallback *New() {
    return new vtk3DLightInteractionCallback;
  }

  // initialization for constructor
  vtk3DLightInteractionCallback() {
    mTransform = vtkSmartPointer<vtkTransform>::New();
  }

  void SetAssemblyActor(vtkAssembly *assembly) {
    this->mAssembly = assembly;
  }

  void SetTransform(vtkTransform *transform) {
    this->mTransform = transform;
  }

  void SetInteractor(QVTKInteractor *interactor) {
    this->Interactor = interactor;
  }

  QVTKInteractor *GetInteractor() {
    return this->Interactor;
  }

/*
* This is wrong!

  void ResetTransform() {
    // pulling out the assembly object in renderer
    vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
    vtkSmartPointer<vtkRenderer> renderer= interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
    vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
    vtkSmartPointer<vtkPropCollection> collection = renderer->GetViewProps();
    collection->InitTraversal();

    this->mTransform->SetMatrix(camera->GetCameraLightTransformMatrix());
    vtkSmartPointer<vtkProp> prop = collection->GetNextProp(); // this is mAssembly
    prop->PokeMatrix(this->mTransform->GetMatrix());
    renderer->ResetCamera();
    camera->Dolly(CAMERA_DOLLY);
    renderer->ResetCameraClippingRange();  // order does matter!

    // update light transform
    vtkSmartPointer<vtkLightCollection> lights = renderer->GetLights();
    lights->InitTraversal();
    vtkSmartPointer<vtkLight> light = lights->GetNextItem();
    light->SetTransformMatrix(this->mTransform->GetMatrix());

    return;
  }
*/

  vtkTransform *GetCamVec() {
    // pulling out the assembly object in renderer
    vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
    vtkSmartPointer<vtkRenderer> renderer= interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
    vtkSmartPointer<vtkPropCollection> collection = renderer->GetViewProps();
    collection->InitTraversal();
    vtkSmartPointer<vtkProp> prop = collection->GetNextProp(); // this is mAssembly

    // get the prop's transform
    mTransform->SetMatrix(prop->GetMatrix());

    // update light transform
    vtkSmartPointer<vtkLightCollection> lights = renderer->GetLights();
    lights->InitTraversal();
    vtkSmartPointer<vtkLight> light = lights->GetNextItem();
    light->SetTransformMatrix(mTransform->GetMatrix());

    return this->mTransform;
  }

  //----------------Begin. By YY------------------------//
  double* GetLightPositionVec() {
	// pulling out the assembly object in renderer
    vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
    vtkSmartPointer<vtkRenderer> renderer= interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
    vtkSmartPointer<vtkPropCollection> collection = renderer->GetViewProps();
    collection->InitTraversal();
    vtkSmartPointer<vtkProp> prop = collection->GetNextProp(); // this is mAssembly

    // get the prop's transform
    mTransform->SetMatrix(prop->GetMatrix());

    // update light transform
    vtkSmartPointer<vtkLightCollection> lights = renderer->GetLights();
    lights->InitTraversal();
    vtkSmartPointer<vtkLight> light = lights->GetNextItem();
    light->SetTransformMatrix(mTransform->GetMatrix());

	double lightPosVec[3];
	light->GetTransformedPosition(lightPosVec);

	///**********Just a test****************/
	//qDebug() << "GetLightPositionVec(): ("<< lightPosVec[0] << ", " << lightPosVec[1] << ", " << lightPosVec[2] << ")" << endl;
	///**********Just a test****************/

	return lightPosVec;
  }
 //----------------End. By YY------------------------//

  virtual void Execute(vtkObject *, unsigned long event, void *)
    {
    vtkSmartPointer<QVTKInteractor> interactor = this->GetInteractor();
    // send out the signal
    vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast( interactor->GetInteractorStyle() );

    // Get the location of the click (in window coordinates)
    int lastPos[2];
    interactor->GetLastEventPosition(lastPos);
    int currPos[2];
    interactor->GetEventPosition(currPos);

    if (event == vtkCommand::LeftButtonPressEvent)
      {
        if (style)  { style->OnLeftButtonDown(); }
      }
    else if (event == vtkCommand::LeftButtonReleaseEvent)
      {
        if (style)  { style->OnLeftButtonUp(); }
      }
    else if (event == vtkCommand::MouseMoveEvent)
      {
        // update and get the camera vector
        GetCamVec();
        if (style)  { style->OnMouseMove(); }
       }
    else if (event == vtkCommand::MiddleButtonPressEvent)
      {
//        if (style)  { style->OnMiddleButtonDown(); }
      }
    else if (event == vtkCommand::MiddleButtonReleaseEvent)
      {
//        if (style)  { style->OnMiddleButtonUp(); }
      }
    else if (event == vtkCommand::RightButtonPressEvent)
      {
//        if (style)  { style->OnRightButtonDown(); }
      }
    else if (event == vtkCommand::RightButtonReleaseEvent)
      {
//        if (style)  { style->OnRightButtonUp(); }
      }
    else if (event == vtkCommand::MouseWheelForwardEvent)
      {
//        if (style)  { style->OnMouseWheelForward(); }
      }
    else if (event == vtkCommand::MouseWheelBackwardEvent)
      {
//        if (style)  { style->OnMouseWheelBackward(); }
      }

    interactor->Render();
    }

private:
  // Pointer to the interactor
  vtkSmartPointer<QVTKInteractor> Interactor;
  // for selection of triangle mesh
  vtkSmartPointer<vtkPolyData> mPolyData;
  // contain assembly
  vtkSmartPointer<vtkAssembly> mAssembly;
  vtkSmartPointer<vtkPropCollection> mCollection;
  vtkSmartPointer<vtkTransform> mTransform;
};

//==========================================================================

LightControl::LightControl(QWidget *parent)
    : QWidget(parent)
{
  int width = this->frameGeometry().width();
  int height = this->frameGeometry().height();
//  this->setMinimumSize(QSize(width, height));

  mIntensityL1 = 1.0;
  mIntensityL2 = 1.0;

  mPreviousL1 = mIntensityL1;
  mPreviousL2 = mIntensityL2;

  // create vtkwidget
  drawVtkObject();

  // light control widget
  QGridLayout* glayout = new QGridLayout;
  QVBoxLayout* vlayout = new QVBoxLayout;
  QHBoxLayout* hlayout = new QHBoxLayout;
  QGridLayout* g2layout = new QGridLayout;
  glayout->setMargin(0);
  vlayout->setMargin(0);
  hlayout->setMargin(0);
  g2layout->setMargin(0);

  resetbut = new QPushButton("Light Reset",parent);
  mLitControl1dial = new Knob( "Brightness (Key Light)", 0.0, 100.0, parent );
  mLitControl1dial->setValue(DEFAULTVALUE1);
  mLitControl2dial = new Knob( "Contrast (Back Light)", 0.0, 100.0, parent );
  mLitControl2dial->setValue(DEFAULTVALUE2);

  //MK: LCD
  QPalette pal( Qt::black );
  pal.setColor( QPalette::WindowText, Qt::green );

  mLitControl1lcd = new QLCDNumber(parent);
  mLitControl1lcd->setSegmentStyle( QLCDNumber::Filled );
  mLitControl1lcd->setAutoFillBackground( true );
  mLitControl1lcd->setFixedHeight( 25 );
  mLitControl1lcd->setPalette( pal );
  mLitControl1lcd->display(mLitControl1dial->value());

  mLitControl2lcd = new QLCDNumber(parent);
  mLitControl2lcd->setSegmentStyle( QLCDNumber::Filled );
  mLitControl2lcd->setAutoFillBackground( true );
  mLitControl2lcd->setFixedHeight( 25 );
  mLitControl2lcd->setPalette( pal );
  mLitControl2lcd->display(mLitControl2dial->value());

  // connect lcd display
  connect( mLitControl1dial, SIGNAL( valueChanged( double ) ), mLitControl1lcd, SLOT(display(double)));
  connect( mLitControl2dial, SIGNAL( valueChanged( double ) ), mLitControl2lcd, SLOT(display(double)));

  // connect main display
  connect( mLitControl1dial, SIGNAL( valueChanged( double ) ), this, SLOT(changeBrightnessLocal(double)) );
  connect( mLitControl2dial, SIGNAL( valueChanged( double ) ), this, SLOT(changeContrastLocal(double)) );

//  mLitControl2dial->setFixedHeight(parent->sizeHint().height() / 24);
//  mLitControl1dial->setFixedHeight(parent->sizeHint().height() / 24);

  g2layout->addWidget(mLitControl1lcd,0,0);
  g2layout->addWidget(mLitControl1dial,1,0);
  g2layout->addWidget(mLitControl2lcd,0,1);
  g2layout->addWidget(mLitControl2dial,1,1);

  vlayout->addLayout(g2layout,1);
  vlayout->addWidget(resetbut);

  QHBoxLayout* hbox = new QHBoxLayout;
  hbox->addWidget(mQvtkWidget,1);

  glayout->addLayout(hbox,0,0);
  glayout->addLayout(vlayout,0,1);
  this->setLayout(glayout);

  connect(resetbut,SIGNAL(clicked()),this,SLOT(reset()));

  updateLightControl(EMPTYWIDGET);
}

vtkTransform* LightControl::GetCamVec() {
     return mCallback->GetCamVec();
}

//-----------------Begin. By YY---------------------//
double* LightControl::GetLightPositionVec() {
	//double *lpos1 = mCallback->GetLightPositionVec();
	//qDebug() << "Light Position**: ("<< lpos1[0] << ", " << lpos1[1] << ", " << lpos1[2] << ")" << endl;
	return mCallback->GetLightPositionVec();
}
//-----------------End. By YY---------------------//

// control both light1 and 2
void LightControl::changeBrightnessLocal(double intensity1) {
  double intensity2 = mLitControl2dial->value();

  mIntensityL1 = intensity1/DEFAULTVALUE1;
  mIntensityL2 = intensity2/DEFAULTVALUE2;

#ifdef CHANGECONTROLVIEW
  mLight1->SetIntensity(mIntensityL1);
  mLight2->SetIntensity(mIntensityL2);
#endif

  emit signalIntensityL12(mIntensityL1, mIntensityL2);

  mQvtkWidget->update();
}

// control only light2
void LightControl::changeContrastLocal(double intensity2) {
  double intensity1 = mLitControl1dial->value();

  mIntensityL1 = intensity1/DEFAULTVALUE1;
  mIntensityL2 = intensity2/DEFAULTVALUE2;

#ifdef CHANGECONTROLVIEW
  mLight1->SetIntensity(mIntensityL1);
  mLight2->SetIntensity(mIntensityL2);
#endif

  emit signalIntensityL12(mIntensityL1, mIntensityL2);

  mQvtkWidget->update();
}

void LightControl::updateLightingVector()
{
	//----------------Begin. By YY-------------------//
  double *lightPosVec = LightControl::GetLightPositionVec();
  emit signalLightPositionChanged(lightPosVec[0], lightPosVec[1], lightPosVec[2]);
  emit signalLitCamPosChanged(LightControl::GetCamVec());
  //----------------End. By YY-------------------//


  // emit signalLitCamPosChanged(mCallback->GetCamVec());
}

void LightControl::updateLightControl(WidgetMode widgetmode)
{
    QPalette pal( Qt::black );
    pal.setColor( QPalette::WindowText, Qt::green );

    QPalette paldisabled( Qt::black );
    paldisabled.setColor( QPalette::WindowText, Qt::black );

    switch (widgetmode) {
    case EMPTYWIDGET:
        mLitControl1lcd->setPalette( paldisabled );
        mLitControl2lcd->setPalette( paldisabled );
        mLitControl1dial->setEnabled(false);
        mLitControl2dial->setEnabled(false);
        mQvtkWidget->setEnabled(false);
        resetbut->setEnabled(false);
        break;
    case IMAGE2D:
        mLitControl1lcd->setPalette( paldisabled );
        mLitControl2lcd->setPalette( paldisabled );
        mLitControl1dial->setEnabled(false);
        mLitControl2dial->setEnabled(false);
        mQvtkWidget->setEnabled(false);
        resetbut->setEnabled(false);
        break;
    case MODEL3D:
        mLitControl1lcd->setPalette( pal );
        mLitControl2lcd->setPalette( pal );
        mLitControl1dial->setEnabled(true);
        mLitControl2dial->setEnabled(true);
        mQvtkWidget->setEnabled(true);
        resetbut->setEnabled(true);
        break;
    case CTSTACK:
        mLitControl1lcd->setPalette( pal );
        mLitControl2lcd->setPalette( pal );

        mLitControl1dial->setEnabled(true);
        mLitControl2dial->setEnabled(true);
        mQvtkWidget->setEnabled(false);
        resetbut->setEnabled(false);
        break;
    case CTVOLUME:
        mLitControl1lcd->setPalette( paldisabled );
        mLitControl2lcd->setPalette( paldisabled );

        mLitControl1dial->setEnabled(false);
        mLitControl2dial->setEnabled(false);
        mQvtkWidget->setEnabled(true);
        resetbut->setEnabled(true);
        break;
    case RTI2D:
        /*mLitControl1lcd->setPalette( paldisabled );
        mLitControl2lcd->setPalette( paldisabled );

        mLitControl1dial->setEnabled(false);
        mLitControl2dial->setEnabled(false);
        mQvtkWidget->setEnabled(true);
        resetbut->setEnabled(true);*/

		//----Begin. By YY-----------------------//
		mLitControl1lcd->setPalette( pal );
        mLitControl2lcd->setPalette( pal );
		mLitControl1dial->setEnabled(true);
        mLitControl2dial->setEnabled(true);
        mQvtkWidget->setEnabled(true);
        resetbut->setEnabled(true);
		//----End. By YY-----------------------//
        break;
    default: break;
    }
}


MainWindow * LightControl::mw()
{
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
    if (mainwindow)
    {
      return mainwindow;
    }
  }
}


void LightControl::getLitVectorControl(vtkObject*, unsigned long, void*, void*)
{
  updateLightingVector();
}

void LightControl::drawVtkObject()
{
  //=================================================================
  // Minimul Pipeline for rendering VTK OpenGL
  //=================================================================
  mQvtkWidget = new QVTKWidget(this);
//  mQvtkWidget->setMinimumSize(QSize(150, 150));

  // create light
  mLight1 = vtkSmartPointer<vtkLight>::New();
  mLight1->SetFocalPoint(0,0,0);
  mLight1->SetPosition(0,0,1);
  mLight1->SetIntensity(DEFAULTINTENSITYL1);

  mLight2 = vtkSmartPointer<vtkLight>::New();
  mLight2->SetFocalPoint(0,0,0);
  mLight2->SetPosition(0,0,1);
  mLight2->SetIntensity(DEFAULTINTENSITYL2);

  // create objects
  vtkSmartPointer<vtkSphereSource> sphereCenter = vtkSmartPointer<vtkSphereSource>::New();
  vtkSmartPointer<vtkPolyDataMapper> sphereCenterMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkActor> sphereCenterActor = vtkSmartPointer<vtkActor>::New();
  sphereCenterMapper->SetInput(sphereCenter->GetOutput());// connect pipeline
  sphereCenterActor->SetMapper(sphereCenterMapper);
  sphereCenterActor->SetPosition(0,0,0);
  // configure object
  sphereCenter->SetThetaResolution(30);
  sphereCenter->SetPhiResolution(30);
  sphereCenter->SetRadius(30);

  // create symbolic light
  vtkSmartPointer<vtkSphereSource> sphereOrbit = vtkSmartPointer<vtkSphereSource>::New();
  vtkSmartPointer<vtkPolyDataMapper> sphereOrbitMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkActor> sphereOrbitActor = vtkSmartPointer<vtkActor>::New();
  sphereOrbitMapper->SetInput(sphereOrbit->GetOutput());// connect pipeline
  sphereOrbitActor->SetMapper(sphereOrbitMapper);
  sphereOrbitActor->SetPosition(0,0,37); // set the object position

  vtkSmartPointer<vtkProperty> sphereOrbitproperty = sphereOrbitActor->GetProperty();
  sphereOrbitproperty->SetColor(1,0.75,0); // sphere color orange
  sphereOrbitproperty->LightingOff(); // turn off shading
  sphereOrbit->SetThetaResolution(15);
  sphereOrbit->SetPhiResolution(15);
  sphereOrbit->SetRadius(5);

  mAssembly = vtkSmartPointer<vtkAssembly>::New();
  mAssembly->AddPart(sphereCenterActor);
  mAssembly->AddPart(sphereOrbitActor);
  mAssembly->SetOrigin(0,0,0); // rotation axis
  mAssembly->SetDragable(1);

  // Bring the objects to the renderer
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  renderer->AddLight(mLight1);
  renderer->AddLight(mLight2);
  renderer->AddActor(mAssembly);

  // set the renderer to VTK OpenGL Widget Window
  mQvtkWidget->GetRenderWindow()->AddRenderer(renderer);

  // use default setting
  vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
  renderer->SetActiveCamera(camera);
  renderer->ResetCamera();
  camera->Dolly(CAMERA_DOLLY); // order does matter!
  renderer->ResetCameraClippingRange();// order does matter!

  //------------------------------------------------------------------------------------------
  // Set up the default interaction
  vtkSmartPointer<QVTKInteractor> interactor = vtkSmartPointer<QVTKInteractor>::New();
  interactor->SetRenderWindow(mQvtkWidget->GetRenderWindow());
  interactor->GetInteractorStyle()->SetDefaultRenderer(renderer);

  vtkSmartPointer<vtkInteractorStyleTrackballActor> style = vtkSmartPointer<vtkInteractorStyleTrackballActor>::New(); // actor
  interactor->SetInteractorStyle(style);

  // create new customized callback
  mCallback =  vtkSmartPointer<vtk3DLightInteractionCallback>::New();
  mCallback->SetInteractor(interactor); // set the callback to the interactor
  mCallback->SetAssemblyActor(mAssembly);
  mCallback->GetCamVec();
  mCallback->GetLightPositionVec();

  // connect the customized callback to the interactor
//  style->AddObserver( vtkCommand::InteractionEvent, mCallback );
  style->AddObserver(vtkCommand::MouseMoveEvent, mCallback);
  style->AddObserver(vtkCommand::LeftButtonPressEvent, mCallback);
  style->AddObserver(vtkCommand::LeftButtonReleaseEvent, mCallback);
  style->AddObserver(vtkCommand::MiddleButtonPressEvent, mCallback);
  style->AddObserver(vtkCommand::MiddleButtonReleaseEvent, mCallback);
  style->AddObserver(vtkCommand::RightButtonPressEvent, mCallback);
  style->AddObserver(vtkCommand::RightButtonReleaseEvent, mCallback);
  style->AddObserver(vtkCommand::MouseWheelForwardEvent, mCallback);
  style->AddObserver(vtkCommand::MouseWheelBackwardEvent, mCallback);

  // connect VTK signal to QT slot
  mEventQtSlot = vtkSmartPointer<vtkEventQtSlotConnect>::New();
  // get key pressed with high priority (1.0)
  mEventQtSlot->Connect(style, vtkCommand::MouseMoveEvent, this, SLOT( getLitVectorControl(vtkObject*, unsigned long, void*, void*) ) );

  // start the interactor
  interactor->Initialize();

  mQvtkWidget->update();
  //------------------------------------------------------------------------------------------
}

void LightControl::reset()
{
  mAssembly->SetOrientation(0,0,1);

  mLitControl1dial->setValue(DEFAULTVALUE1);
  mLitControl2dial->setValue(DEFAULTVALUE2);

  mQvtkWidget->update();

  updateLightingVector();// update main window
}

void LightControl::setDirectionalLight(bool isDirectionalLight)
{
  // dont change the member light values
  if (isDirectionalLight) {
    mLitControl1dial->setValue(DEFAULTVALUE1*mPreviousL1);
    mLitControl2dial->setValue(DEFAULTVALUE2*mPreviousL2);

    mLitControl1dial->setEnabled(true);
    mLitControl2dial->setEnabled(true);
  } else {
    mPreviousL1 = mIntensityL1;
    mPreviousL2 = mIntensityL2;

    mLitControl1dial->setValue(DEFAULTVALUE1*2);
    mLitControl2dial->setValue(0); // DEFAULTVALUE2

    mLitControl1dial->setEnabled(false);
    mLitControl2dial->setEnabled(false);
  }
  updateLightingVector();// update main window
}

void LightControl::restoreBookmarkLight(double orientation[3], double brightness, double contrast, int filetype)
{
    if(filetype != CTSTACK)
        mAssembly->SetOrientation(orientation[0], orientation[1], orientation[2]);

    if(filetype != CTVOLUME) {
        mLitControl1dial->setValue(brightness*DEFAULTVALUE1);
        mLitControl2dial->setValue(contrast*DEFAULTVALUE2);

        mLitControl1lcd->display(brightness*DEFAULTVALUE1);
        mLitControl2lcd->display(contrast*DEFAULTVALUE2);
    }

    if (mQvtkWidget) mQvtkWidget->show();
    if (mQvtkWidget) mQvtkWidget->update();

    if (mw()->VTKA()->mQVTKWidget) mw()->VTKA()->mQVTKWidget->show();
    if (mw()->VTKA()->mQVTKWidget) mw()->VTKA()->mQVTKWidget->update();

    if(filetype != CTVOLUME)
        updateLightingVector();

}

LightControl::~LightControl()
{
}
