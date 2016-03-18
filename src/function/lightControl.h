/****************************************************************************

 - Codename: Hyper3D (Yale Computer Graphics Group)

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
#ifndef LIGHTCONTROL_H
#define LIGHTCONTROL_H

#include "../visualization/vtkWidget.h" // this contains GLEW.h (this should be placed at the very beginning)

#include <QtGui>
#include <QWidget>
#include <QVector>

// Class list
class MainWindow;
class MeshDocument;
class Knob;
class WheelBox;
class vtk3DLightInteractionCallback;
class vtkAssembly;

class LightControl : public QWidget
{
  Q_OBJECT

public:
  LightControl(QWidget *parent = 0);
  ~LightControl();
  double GetIntensityL1() {return mIntensityL1;}
  double GetIntensityL2() {return mIntensityL2;}
  vtkTransform* GetCamVec();
  double* GetLightPositionVec(); // By YY
  void SetIntensityL1(double intensity) { mIntensityL1 = intensity; }
  void SetIntensityL2(double intensity) { mIntensityL2 = intensity; }
  void setDirectionalLight(bool isDirectionalLight);
  // DT: used to restore light settings from a bookmark.
  void restoreBookmarkLight(double orientation[3], double brightness, double contrast, int filetype);
  vtkSmartPointer<vtkAssembly> GetAssembly() {return mAssembly;}

protected:
  QVTKWidget* mQvtkWidget;
  void drawVtkObject();

private:
  MainWindow * mw();
  QPushButton* resetbut;
  QPushButton* light1but;

  Knob *mLitControl2dial;
  Knob *mLitControl1dial;
  QLCDNumber *mLitControl1lcd;
  QLCDNumber *mLitControl2lcd;

  vtkSmartPointer<vtkLight> mLight1;
  vtkSmartPointer<vtkLight> mLight2;

  double mIntensityL1;
  double mIntensityL2;

  double mPreviousL1;
  double mPreviousL2;

  // to connect vtk to qt
  vtkSmartPointer<vtk3DLightInteractionCallback> mCallback;
  vtkSmartPointer<vtkEventQtSlotConnect> mEventQtSlot;

  vtkSmartPointer<vtkAssembly> mAssembly;
signals:
  void signalLitCamPosChanged(vtkTransform* transform);
  void signalLightPositionChanged(double lightX, double lightY, double lightZ); // by YY
  void signalIntensityL12(double intensity1, double intensity2);

public slots:
  void reset();


private slots:
  void getLitVectorControl(vtkObject*, unsigned long, void*, void*);
  void updateLightingVector();
  void updateLightControl(WidgetMode widgetmode);
  void changeBrightnessLocal(double intensity);
  void changeContrastLocal(double intensity);

};
#endif // LIGHTCONTROL_H
