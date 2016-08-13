/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Min H. Kim (minhkim@cs.yale.edu)

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
   anonymous help by various software engineering communities.

*****************************************************************************/
#include <QMouseEvent>
#include <qwt_knob.h>
#include <qwt_round_scale_draw.h>
#include <qwt_math.h>

#include "../mainWindow.h"
#include "CTControl.h"
#include "knobWidget.h"

// Constructor
CTControl::CTControl(QWidget *parent)
    : QWidget(parent)
{
  // DT: These are currently unused - necessary?
  int width = this->frameGeometry().width();
  int height = this->frameGeometry().height();

  // Enums are defined in vtkWidget.h
  mSliceMax = 0;
  mSliceCurrent = 0;
  mOrientationCurrent = FRONTCT;
  mCTVisualization = STACK;
  mVolumeRenderMode = AUTOMATIC;
  mBlendType = CSRAMP;
  mReductionFactor = 1.0f;

  // 3 widgets are combined into one: 2D/3D choice, 2D stack navigation options, and 3D volume rendering options.
  QGridLayout* glayout = new QGridLayout(this); // 'this' must be specified.
  glayout->addWidget(createRenderGroup(),0,0);
  glayout->addWidget(createVolCtrlGroup(),1,0);
  glayout->addWidget(createStackCtrlGroup(),0,1,0,2);
  this->setLayout(glayout);

  updateCTControl();
}

// Destructor
CTControl::~CTControl()
{
}

QGroupBox *CTControl::createVolCtrlGroup()
{
  QGroupBox *groupBox = new QGroupBox(tr("Volume Rendering Control"));
  QVBoxLayout *vboxx = new QVBoxLayout;
  vboxx->setMargin(2);

  volumeRenderMode = new QComboBox();
  volumeRenderMode->addItem("Automatic");
  volumeRenderMode->addItem("GPU Rendering"); //("GPU (Ray Cast Texture)");
  volumeRenderMode->addItem("CPU Rendering"); // ("CPU (Ray Cast Texture)");
  //  volumeRenderMode->addItem("CPU (Ray Cast)");
  volumeRenderMode->setCurrentIndex(0);

  blendTypeCombo = new QComboBox();
  blendTypeCombo->addItem("CT Wood");
  blendTypeCombo->addItem("MIP Blending");
  blendTypeCombo->addItem("Composite Ramp");
  blendTypeCombo->addItem("Composite Shade Ramp");
  blendTypeCombo->addItem("CT Skin");
  blendTypeCombo->addItem("CT Bone");
  blendTypeCombo->addItem("CT Muscle");
  blendTypeCombo->setCurrentIndex(0);

  // Combo box for blend type
  connect(volumeRenderMode, SIGNAL(currentIndexChanged(int)), this, SLOT(sendVolumeRenderMode(int)) );
  connect(blendTypeCombo, SIGNAL(currentIndexChanged(int)), this, SIGNAL(sendBlendTypeChanged(int)));

  QLabel *lcombo1 = new QLabel("Hardware Mode");
  vboxx->addWidget(lcombo1);
  vboxx->addWidget(volumeRenderMode);

  QLabel *lcombo2 = new QLabel("Blend Type");
  vboxx->addWidget(lcombo2);
  vboxx->addWidget(blendTypeCombo);

  // Combo box for image resolution
  resolutionCombo = new QComboBox();
  resolutionCombo->addItem("Full resolution");
  resolutionCombo->addItem("Half resolution");
  resolutionCombo->addItem("Quarter resolution");
  resolutionCombo->setCurrentIndex(0);

  connect(resolutionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(sendReductionFactor(int)));

  QLabel *lcombo = new QLabel("Resolution");
  vboxx->addWidget(lcombo);
  vboxx->addWidget(resolutionCombo);

  groupBox->setLayout(vboxx);
  return groupBox;
}

void CTControl::sendReductionFactor(int sliderInput)
{
    float resFactor = 1.0f;
    switch(sliderInput)
    {
        default:
        case 0: // full
            resFactor = 1.f;
            break;
        case 1: // half
            resFactor = 0.5f;
            break;
        case 2: // quarter
            resFactor = 0.25f;
            break;
    }

  emit sendReductionFactorChanged(resFactor);
}


void CTControl::sendVolumeRenderMode(int comboInput)
{
  switch(comboInput)
  {
  default:
  case AUTOMATIC:
    mVolumeRenderMode = AUTOMATIC;
    break;
  case DEFAULTGPU:
    mVolumeRenderMode = DEFAULTGPU;
    break;
  case CPURAYCASTTEXTURE:
    mVolumeRenderMode = CPURAYCASTTEXTURE;
    break;
  case CPURAYCASTONLY:
    mVolumeRenderMode = CPURAYCASTONLY;
    break;
  }

  // update interface
  updateCTControl();

  emit sendVolumeRenderModeChanged(mVolumeRenderMode);
}


QGroupBox *CTControl::createStackCtrlGroup()
{
  QGroupBox *groupBox = new QGroupBox(tr("2D Image Stack Control"));

  QVBoxLayout *vboxx = new QVBoxLayout;

  int width = this->frameGeometry().width();
  int height = this->frameGeometry().height();
  int iheight = 25;
  int iwidth = 60;

  //MK: LCD
  QPalette pal( Qt::black );
  pal.setColor( QPalette::WindowText, Qt::green );

  currentslidelcd = new QLCDNumber;
  currentslidelcd->setSegmentStyle( QLCDNumber::Filled );
  currentslidelcd->setAutoFillBackground( true );
  currentslidelcd->setFixedHeight( iheight );
//  currentslidelcd->setFixedWidth(iwidth);
  currentslidelcd->setPalette( pal );

  endslidelcd = new QLCDNumber;
  endslidelcd->setSegmentStyle( QLCDNumber::Filled );
  endslidelcd->setAutoFillBackground( true );
  endslidelcd->setFixedHeight( iheight );
//  endslidelcd->setFixedWidth(iwidth);
  endslidelcd->setPalette( pal );
  endslidelcd->display(mSliceMax);

  QLabel *currentslice = new QLabel("Current slice");
  QLabel *totalslice = new QLabel("Number of slices");

  QGridLayout *glayout = new QGridLayout;
  glayout->addWidget(currentslice,0,0);
  glayout->addWidget(currentslidelcd,0,1);
  glayout->addWidget(totalslice,1,0);
  glayout->addWidget(endslidelcd,1,1);

  vboxx->addLayout(glayout);
  //-------------------------------------------------------------

  //Create QPushButtons
  int bwidth = 65;
  frontbutton = new QPushButton("Front");
  frontbutton->setMaximumWidth(bwidth);
  topbutton = new QPushButton("Top");
  topbutton->setMaximumWidth(bwidth);
  sidebutton = new QPushButton("Side");
  sidebutton->setMaximumWidth(bwidth);
  updownbutton = new QPushButton("Toggle Upside-Down");

  frontbutton->setCheckable(true);
  topbutton->setCheckable(true);
  sidebutton->setCheckable(true);

  QHBoxLayout* hbox = new QHBoxLayout;
  hbox->addWidget(frontbutton);
  hbox->addWidget(topbutton);
  hbox->addWidget(sidebutton);
  hbox->setMargin(0);
  QVBoxLayout* vbox = new QVBoxLayout;
  vbox->addWidget(updownbutton);
  vbox->setMargin(0);

  //Create QButonGroup and add Buttons here
  //Adding Buttons into QButtonGroup does not change anything visually
  //QButtonGroup is just a logical grouping
  QButtonGroup* group = new QButtonGroup;
  group->addButton(frontbutton);
  group->addButton(topbutton);
  group->addButton(sidebutton);
  group->setExclusive(true); // only one button selected

  //connect callbacks
  connect(frontbutton, SIGNAL(clicked()), this, SLOT(clickFrontButton()));
  connect(topbutton, SIGNAL(clicked()), this, SLOT(clickTopButton()));
  connect(sidebutton, SIGNAL(clicked()), this, SLOT(clickSideButton()));
  connect(updownbutton, SIGNAL(clicked()), this, SLOT(clickUpDownButton()));

  vboxx->addLayout(hbox);
  vboxx->addLayout(vbox);
  //-------------------------------------------------------------
  // CT view slider
  sliceSlider = new QSlider(Qt::Horizontal);
  sliceSlider->setFocusPolicy(Qt::StrongFocus);
  sliceSlider->setTickPosition(QSlider::TicksAbove);
  sliceSlider->setValue(mSliceCurrent);

  connect(sliceSlider, SIGNAL(valueChanged(int)), this, SLOT(addonelcd(int)));
  connect(this, SIGNAL(sendaddonelcd(int)), currentslidelcd, SLOT(display(int)));
  connect(sliceSlider, SIGNAL(valueChanged(int)), this, SLOT(sendUpdateSlideCurrent(int)));

  QVBoxLayout *vbox2 = new QVBoxLayout;
  QHBoxLayout *hbox3 = new QHBoxLayout;

  QLabel *navigatorlabel = new QLabel("Slice Navigator");
  QLabel *lbegin = new QLabel("Start");
  QLabel *lend = new QLabel("End");
  hbox3->addWidget(lbegin);
  hbox3->addStretch(3);
  hbox3->addWidget(navigatorlabel);
  hbox3->addStretch(3);
  hbox3->addWidget(lend);

  vbox2->addLayout(hbox3);
  vbox2->addWidget(sliceSlider);

  vboxx->addLayout(vbox2);
  vboxx->setMargin(2);

  groupBox->setLayout(vboxx);

  return groupBox;
}

void CTControl::restoreBookmarkCTStack(int currentSlice, int view)
{
    if(view == FRONTCT) {
        this->clickFrontButton();
    } else if(view == SIDECT) {
        this->clickSideButton();
    } else if(view == TOPCT){
        this->clickTopButton();
    }

    sliceSlider->setValue(currentSlice);
}

void CTControl::restoreBookmarkCTVolume(int ctMode, int ctBlendType, double ctResolution)
{
    if(ctMode == DEFAULTGPU) {
        volumeRenderMode->setCurrentIndex(DEFAULTGPU);
        this->sendVolumeRenderMode(DEFAULTGPU);
    } else if(ctMode == CPURAYCASTTEXTURE) {
        volumeRenderMode->setCurrentIndex(CPURAYCASTTEXTURE);
        this->sendVolumeRenderMode(CPURAYCASTTEXTURE);
    } else if(ctMode == CPURAYCASTONLY) {
        volumeRenderMode->setCurrentIndex(CPURAYCASTONLY);
        this->sendVolumeRenderMode(CPURAYCASTONLY);
    } else if(ctMode == AUTOMATIC){
        volumeRenderMode->setCurrentIndex(AUTOMATIC);
        this->sendVolumeRenderMode(AUTOMATIC);
    }

    if(ctBlendType == WOOD) {
        blendTypeCombo->setCurrentIndex(WOOD);
        emit sendBlendTypeChanged(WOOD);
    } else if(ctBlendType == MIP) {
        blendTypeCombo->setCurrentIndex(MIP);
        emit sendBlendTypeChanged(MIP);
    } else if(ctBlendType == CRAMP) {
        blendTypeCombo->setCurrentIndex(CRAMP);
        emit sendBlendTypeChanged(CRAMP);
    } else if(ctBlendType == CSRAMP){
        blendTypeCombo->setCurrentIndex(CSRAMP);
        emit sendBlendTypeChanged(CSRAMP);
    } else if(ctBlendType == CT_SKIN) {
        blendTypeCombo->setCurrentIndex(CT_SKIN);
        emit sendBlendTypeChanged(CT_SKIN);
    } else if(ctBlendType == CT_BONE) {
        blendTypeCombo->setCurrentIndex(CT_BONE);
        emit sendBlendTypeChanged(CT_BONE);
    } else if(ctBlendType == CT_MUSCLE) {
        blendTypeCombo->setCurrentIndex(CT_MUSCLE);
        emit sendBlendTypeChanged(CT_MUSCLE);
    }

    if(ctResolution == 0.25) {
        resolutionCombo->setCurrentIndex(2);
        this->sendReductionFactor(2);
    } else if(ctResolution == 0.5) {
        resolutionCombo->setCurrentIndex(1);
        this->sendReductionFactor(1);
    } else if(ctResolution == 1.0){
        resolutionCombo->setCurrentIndex(0);
        this->sendReductionFactor(0);
    }
}

// We have to add 1 to the current value because the slice count starts at 0.
void CTControl::addonelcd(int value)
{
    //qDebug() << value << '\n';
  ++value;
  emit sendaddonelcd(value);
}

void CTControl::clickFrontButton()
{
  mOrientationCurrent = FRONTCT;
//  mSliceCurrent = 0;
  emit sendOrientationChanged(mOrientationCurrent);
  sendUpdateSlideCurrent(mSliceCurrent);
  updateCTControl();
}

void CTControl::clickTopButton()
{
  mOrientationCurrent = TOPCT;
//  mSliceCurrent = 0;
  emit sendOrientationChanged(mOrientationCurrent);
  sendUpdateSlideCurrent(mSliceCurrent);
  updateCTControl();
}

void CTControl::clickSideButton()
{
  mOrientationCurrent = SIDECT;
//  mSliceCurrent = 0;
  emit sendOrientationChanged(mOrientationCurrent);
  sendUpdateSlideCurrent(mSliceCurrent);
  updateCTControl();
}

void CTControl::clickUpDownButton()
{
  emit sendUpDownView();
  updateCTControl();
}

QGroupBox *CTControl::createRenderGroup()
{
    QGroupBox *groupBox = new QGroupBox(tr("Rendering Mode"));

    sliceimageradio = new QRadioButton(tr("2D Rendering"));
    volumegpuradio = new QRadioButton(tr("Volume Rendering"));

    sliceimageradio->setChecked(true);

    connect(sliceimageradio, SIGNAL(clicked()), this, SLOT(setCTStackView()));
    connect(volumegpuradio, SIGNAL(clicked()), this, SLOT(setVTvolumeGPU()));

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(sliceimageradio);
    vbox->addStretch(1);
    vbox->addWidget(volumegpuradio);
    groupBox->setLayout(vbox);

    return groupBox;
}

void CTControl::setCTStackView()
{
	if (mCTVisualization == STACK)
		return;
	mCTVisualization = STACK;
	emit sendVisualizationChanged(mCTVisualization);
	// update interface
	updateCTControl();
}

void CTControl::setVTvolumeGPU()
{
	if (mCTVisualization == VOLUMEGPU)
		return;
	mCTVisualization = VOLUMEGPU;
	emit sendVisualizationChanged(mCTVisualization);
	// update interface
	updateCTControl();
}


void CTControl::updateSliceMax(int sliceMax)
{
  mSliceMax = sliceMax;
  updateCTControl();
}

void CTControl::sendUpdateSlideCurrent(int currentSlide)
{
  mSliceCurrent = currentSlide;
//  qDebug() << "current slices (dock) input:" << mSliceCurrent;
  emit sendSlideCurrent(mSliceCurrent);
}


void CTControl::updateReductionFactor(float reductionFactor)
{
  mReductionFactor = reductionFactor;
//  qDebug() << "current reduction factor:" << mReductionFactor;
  updateCTControl();
}


void CTControl::updateSliceCurrent(int sliceCurrent)
{
  mSliceCurrent = sliceCurrent;
//  qDebug() << "current slices (dock) output:" << mSliceCurrent;
  updateCTControl();
}


void CTControl::updateSliceOrientation(CTOrientation sliceOrientation)
{
  mOrientationCurrent = sliceOrientation;
  updateCTControl();
}


void CTControl::updateSliceVisualization(CTVisualization ctVisualization)
{
  mCTVisualization = ctVisualization;
  updateCTControl();
}

void CTControl::updateVolumeRenderMode(CTVolumeRenderMode ctVolumeRenderMode)
{
  mVolumeRenderMode = ctVolumeRenderMode;
  updateCTControl();
}

// DT: Unused as of right now
/*
void CTControl::updateImageData()
{
  emit sendVisualizationChanged(mCTVisualization);
  updateCTControl();
}
*/

// this part is buggy
void CTControl::updateCTControl()
{

    if (mw()->VTKA())
	{
		WidgetMode wm = mw()->VTKA()->getWidgetMode();
		updateCtControlWidgetMode(wm); // this part is buggy
    }
	else
	{
		return;
	}

    // update measuretoolbar
    if (mw()->VTKA()) {
        mw()->updateMenus(); // new by Min [2013-04-18]
    }


  //MK: updating CT control should include only display max number on lcd and current slice number on the slider and the lcd
  // updating LCD
  endslidelcd->display(mSliceMax+1);
  currentslidelcd->display(sliceSlider->value()+1);

  // slider control
  sliceSlider->setMaximum(mSliceMax);
  sliceSlider->setTickInterval(mSliceMax/10);
  sliceSlider->setSingleStep(1);
  sliceSlider->setValue(mSliceCurrent);

  if (mReductionFactor == 1.f)
      resolutionCombo->setCurrentIndex(0);
  else if (mReductionFactor == 0.5f)
      resolutionCombo->setCurrentIndex(1);
  else if (mReductionFactor == 0.25f)
      resolutionCombo->setCurrentIndex(2);

  blendTypeCombo->setCurrentIndex(mBlendType);
  volumeRenderMode->setCurrentIndex(mVolumeRenderMode);
//  qDebug() << "current render mode:" << mVolumeRenderMode;

  // update the orientation buttons
  switch (mOrientationCurrent)
  {
  default:
  case FRONTCT:
    frontbutton->setChecked(true);
    break;
  case TOPCT:
    topbutton->setChecked(true);
    break;
  case SIDECT:
    sidebutton->setChecked(true);
    break;
  }

  //qDebug()<<"UpdateCTControl";
  // update interface
  switch (mCTVisualization)
  {
  default:
  case STACK:
    sliceimageradio->setEnabled(true);
    sliceimageradio->setChecked(true);
    volumegpuradio->setEnabled(true);
    volumegpuradio->setChecked(false);

    sliceSlider->setEnabled(true);
    frontbutton->setEnabled(true);
    topbutton->setEnabled(true);
    sidebutton->setEnabled(true);
    updownbutton->setEnabled(true);

    resolutionCombo->setEnabled(false);
    blendTypeCombo->setEnabled(false);
    volumeRenderMode->setEnabled(false);
    break;
  case VOLUMEGPU:
    sliceimageradio->setEnabled(true);
    sliceimageradio->setChecked(false);
    volumegpuradio->setEnabled(true);
    volumegpuradio->setChecked(true);

    // disabling buttons
    sliceSlider->setEnabled(false);
    frontbutton->setEnabled(false);
    topbutton->setEnabled(false);
    sidebutton->setEnabled(false);
    updownbutton->setEnabled(false);

    resolutionCombo->setEnabled(true);
    blendTypeCombo->setEnabled(true);
    volumeRenderMode->setEnabled(true);
    break;
  }
}

// this part is buggy
void CTControl::updateCtControlWidgetMode(WidgetMode widgetmode)
{
    QPalette pal( Qt::black );
    pal.setColor( QPalette::WindowText, Qt::green );

    QPalette paldisabled( Qt::black );
    paldisabled.setColor( QPalette::WindowText, Qt::black );

//    qDebug() << "widgetmode: " << widgetmode; // {EMPTYWIDGET=0, IMAGE2D, MODEL3D, CTSTACK, CTVOLUME, RTI2D};

    switch (widgetmode) {
    case EMPTYWIDGET:
        currentslidelcd->setPalette( paldisabled );
        endslidelcd->setPalette( paldisabled );
        sliceimageradio->setEnabled(false);
        volumegpuradio->setEnabled(false);
        sliceSlider->setEnabled(false);
        frontbutton->setEnabled(false);
        topbutton->setEnabled(false);
        sidebutton->setEnabled(false);
        updownbutton->setEnabled(false);
        resolutionCombo->setEnabled(false);
        blendTypeCombo->setEnabled(false);
        volumeRenderMode->setEnabled(false);
        break;
    case IMAGE2D:
        currentslidelcd->setPalette( paldisabled );
        endslidelcd->setPalette( paldisabled );
        sliceimageradio->setEnabled(false);
        volumegpuradio->setEnabled(false);
        sliceSlider->setEnabled(false);
        frontbutton->setEnabled(false);
        topbutton->setEnabled(false);
        sidebutton->setEnabled(false);
        updownbutton->setEnabled(false);
        resolutionCombo->setEnabled(false);
        blendTypeCombo->setEnabled(false);
        volumeRenderMode->setEnabled(false);
        break;
    case MODEL3D:
        currentslidelcd->setPalette( paldisabled );
        endslidelcd->setPalette( paldisabled );
        sliceimageradio->setEnabled(false);
        volumegpuradio->setEnabled(false);
        sliceSlider->setEnabled(false);
        frontbutton->setEnabled(false);
        topbutton->setEnabled(false);
        sidebutton->setEnabled(false);
        updownbutton->setEnabled(false);
        resolutionCombo->setEnabled(false);
        blendTypeCombo->setEnabled(false);
        volumeRenderMode->setEnabled(false);
        break;
    case CTSTACK:
        currentslidelcd->setPalette( pal );
        endslidelcd->setPalette( pal );
        sliceimageradio->setEnabled(true);
        volumegpuradio->setEnabled(true);
        sliceSlider->setEnabled(true);
        frontbutton->setEnabled(true);
        topbutton->setEnabled(true);
        sidebutton->setEnabled(true);
        updownbutton->setEnabled(true);
        resolutionCombo->setEnabled(false);
        blendTypeCombo->setEnabled(false);
        volumeRenderMode->setEnabled(false);
        break;
    case CTVOLUME:
        currentslidelcd->setPalette( paldisabled );
        endslidelcd->setPalette( paldisabled );
        sliceimageradio->setEnabled(true);
        volumegpuradio->setEnabled(true);
        sliceSlider->setEnabled(false);
        frontbutton->setEnabled(false);
        topbutton->setEnabled(false);
        sidebutton->setEnabled(false);
        updownbutton->setEnabled(false);
        resolutionCombo->setEnabled(true);
        blendTypeCombo->setEnabled(true);
        volumeRenderMode->setEnabled(true);
        break;
    case RTI2D:
        currentslidelcd->setPalette( paldisabled );
        endslidelcd->setPalette( paldisabled );
        sliceimageradio->setEnabled(false);
        volumegpuradio->setEnabled(false);
        sliceSlider->setEnabled(false);
        frontbutton->setEnabled(false);
        topbutton->setEnabled(false);
        sidebutton->setEnabled(false);
        updownbutton->setEnabled(false);
        resolutionCombo->setEnabled(false);
        blendTypeCombo->setEnabled(false);
        volumeRenderMode->setEnabled(false);
        break;
    default: break;
    }
}

// MK: find out main window
MainWindow * CTControl::mw()
{
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
    if (mainwindow)
    {
      return mainwindow;
    }
  }

}

