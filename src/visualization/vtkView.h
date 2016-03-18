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
   of Meshlab VCG library from ISTI CNR. I would like to thank
   annonymous help by various software engineering communities.

*****************************************************************************/
#ifndef VTKVIEW_H
#define VTKVIEW_H

#include <QSplitter>

class VtkWidget;
class VtkView;
class MainWindow;

class VtkSplitter : public QSplitter
{
  Q_OBJECT

public:
  VtkSplitter ( QWidget * parent = 0 );
  VtkSplitter(Qt::Orientation orientation, QWidget *parent = 0);

  VtkView *getRootContainer();

protected:
  QSplitterHandle *createHandle();
};

class VtkSplitterHandle : public QSplitterHandle
{
  Q_OBJECT

public:
  VtkSplitterHandle(Qt::Orientation orientation, QSplitter *parent);

protected:
  void mousePressEvent ( QMouseEvent * e );
};

class VtkView : public VtkSplitter
{
  Q_OBJECT

public:
  VtkView(QWidget *parent = 0);
    ~VtkView();

  VtkWidget *copyCurrentView();
  void addView(VtkWidget* viewer,  Qt::Orientation);
  void addOrphanView(VtkWidget* viewer);
  void initializeView(VtkWidget* viewer);
  void removeView(int);

  VtkWidget* currentView();

  int getNextViewerId();
  int viewerCounter();

  void updateAllViewer();
  void update(int id);

  VtkWidget* getViewer(int id);
  int getViewerByPicking(QPoint);

  int currentId;
  QString prjName;

protected:
  void closeEvent(QCloseEvent *event);

signals:
    void updateMainWindowMenus(); //updates the menus of the MainWindow

public slots:
  // Called when we change viewer, set the current viewer
    void updateCurrent(int current);

private:
  QList<VtkWidget *> viewerList; // widgets for the OpenGL contexts and images
  MainWindow * findmw();

};


#endif // VTKVIEW_H
