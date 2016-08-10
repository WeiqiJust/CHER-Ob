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
#ifndef CTCONTROL_H
#define CTCONTROL_H

#include "../visualization/vtkWidget.h" // this contains GLEW.h (this should be placed at the very beginning)

#include <QGLWidget>
#include <QtGui>
#include <QWidget>
#include <QVector>
#include <QSplitter>

// Class list
class MainWindow;

/**
 * This class provides the CT control widget in main frame.
 */
class CTControl : public QWidget
{
  Q_OBJECT

public:
  CTControl(QWidget *parent = 0);
  ~CTControl();

  int GetSliceMax () {return mSliceMax;}
  int GetSliceCurrent() {return mSliceCurrent;}
  CTOrientation GetOrientationCurrent() {return mOrientationCurrent;}
  CTVisualization GetCTVisualization() {return mCTVisualization;}
  CTVolumeRenderMode GetVolumeRenderMode() {return mVolumeRenderMode;}

  int GetBlendType() {return mBlendType;}
  float GetReductionFactor() {return mReductionFactor;}

  void restoreBookmarkCTStack(int currentSlice, int view);
  void restoreBookmarkCTVolume(int ctMode, int ctBlendType, double ctResolution);

  void setVisualizationChanged(CTVisualization type) {mCTVisualization = type;}

signals:
  /**
   * @brief  Add 1 to the current slide before displaying it (because count starts at 0).
   */
  void sendaddonelcd(int value);

  /**
   * @brief  Tells VTK widget to render new slice using currentSlide value.
   */
  void sendSlideCurrent(int currentSlide);

  /**
   * @brief  Tells VTK widget to update the current orientation of the camera.
   */
  void sendOrientationChanged(CTOrientation currentOrient);

  /**
   * @brief  Tells VTK widget to turn the camera upside-down.
   */
  void sendUpDownView();

  /**
   * @brief  Tells VTK widget to change the resolution of the data.
   */
  void sendReductionFactorChanged(float reductionFactor);

  /**
   * @brief  Tells VTK widget to change the blend type of the data when user changes the index of the blend type combo box.
   */
  void sendBlendTypeChanged(int blendType);

  /**
   * @brief  Tells VTK widget to change the visualization from 2D to 3D or vice versa.
   */
  void sendVisualizationChanged(CTVisualization currentVisualization);

  /**
   * @brief  Tells VTK widget to change the 3D render mode.
   */
  void sendVolumeRenderModeChanged(CTVolumeRenderMode currentVolumeRenderMode);

  friend bool BookmarkWidget::viewBookmark(QTreeWidgetItem* item);

  // only max number needs to be updated from the VtkWidget
  // all the rest goes from the CTcontrol to the VtkWidget
public slots:
  void updateSliceMax(int sliceMax);
  void updateSliceCurrent(int sliceCurrent);
  void updateSliceOrientation(CTOrientation sliceOrientation);
  void updateSliceVisualization(CTVisualization ctVisualization);
  void updateVolumeRenderMode(CTVolumeRenderMode ctVolumeRenderMode);
  void updateReductionFactor(float reductionFactor);
  void updateCtControlWidgetMode(WidgetMode widgetmode);

  void setCTStackView();
  void setVTvolumeGPU();

private slots:
  void sendUpdateSlideCurrent(int currentSlide);
  void clickFrontButton();
  void clickTopButton();
  void clickSideButton();
  void clickUpDownButton();
  void addonelcd(int value);


  void sendReductionFactor(int sliderInput);
  void sendVolumeRenderMode(int comboInput);


private:
  void updateCTControl();

  MainWindow* mw(); // find the mainframe

  /* ***************************
   * Setting up GUI
   */
  QSlider* sliceSlider;

  QComboBox* volumeRenderMode;
  QComboBox* resolutionCombo;
  QComboBox* blendTypeCombo;

  QPushButton* frontbutton;
  QPushButton* topbutton;
  QPushButton* sidebutton;
  QPushButton* updownbutton;

  QRadioButton* sliceimageradio;
  QRadioButton* volumegpuradio;

  QLCDNumber *currentslidelcd;
  QLCDNumber *endslidelcd;

  QGroupBox *createRenderGroup();
  QGroupBox *createStackCtrlGroup();
  QGroupBox *createVolCtrlGroup();

  /* ***************************** */

  int mSliceMax; // Total number of 2D slices
  int mSliceCurrent; // Current 2D slice
  CTOrientation mOrientationCurrent; // Current orientation (front, top, side)
  CTVisualization mCTVisualization; // Current rendering mode (2D or 3D volume)
  CTVolumeRenderMode mVolumeRenderMode; // Current volume rendering mode (automatic, CPU, GPU)
  int mBlendType; // Current blend type (CT wood/skin/bone/muscle, composite (shade) ramp, MIP blending)
  float mReductionFactor; // Current resolution (full, half, quarter)

};
#endif // CTCONTROL_H
