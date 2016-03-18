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

#include <QMouseEvent>

#include "../mainWindow.h"  // contains GLEW.H
#include "vtkWidget.h"
#include "vtkView.h"



// constructors
VtkSplitter::VtkSplitter( QWidget * parent):QSplitter(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
}

VtkSplitter::VtkSplitter(Qt::Orientation orientation, QWidget *parent):QSplitter(orientation,parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
}

QSplitterHandle *VtkSplitter::createHandle()
 {
     return new VtkSplitterHandle(orientation(), this);
 }

VtkView *VtkSplitter::getRootContainer()
{
  VtkSplitter * parentVtkSplitter = qobject_cast<VtkSplitter *>(this);
  VtkView* mvc = qobject_cast<VtkView *>(parentVtkSplitter);
  while(!mvc)
  {
    parentVtkSplitter = qobject_cast<VtkSplitter *>(parentVtkSplitter->parent());
    mvc= qobject_cast<VtkView *>(parentVtkSplitter);
  }
  return mvc;
}

VtkSplitterHandle::VtkSplitterHandle(Qt::Orientation orientation, QSplitter *parent):QSplitterHandle(orientation, parent){}

void VtkSplitterHandle::mousePressEvent ( QMouseEvent * e )
{
  QSplitterHandle::mousePressEvent(e);

  if(e->button()== Qt::RightButton)
  {
    MainWindow *window = qobject_cast<MainWindow *>(QApplication::activeWindow());
    if (window) window->setHandleMenu(mapToGlobal(e->pos()), orientation(), splitter());
  }
}

VtkView::VtkView(QWidget *parent)
: VtkSplitter(parent)
{
//  this->setParent(parent);
  setAttribute(Qt::WA_DeleteOnClose);//MK
  setChildrenCollapsible(false);
  currentId=-1;
}

VtkView::~VtkView()
{
  //MK: we don't have anything to delete
//  foreach(VtkWidget* viewer, viewerList)
//  {
//    delete viewer;
//  }
}


int VtkView::getNextViewerId(){
  int newId=-1;

  foreach(VtkWidget* view, viewerList)
  {
    if(newId < view->getId()) newId = view->getId();
  }

  return ++newId;
}

void VtkView::addOrphanView(VtkWidget* viewer){
  viewerList.append(viewer);
  addWidget(viewer);

  currentId = viewer->getId();
  //action for new viewer
  connect(viewer, SIGNAL(currentViewerChanged(int)), this, SLOT(updateCurrent(int)));
  return;
}

void VtkView::initializeView(VtkWidget* viewer){
  if (viewerCounter()>0){
    viewerList.clear();
  }
  viewerList.append(viewer);
  addWidget(viewer);

  currentId = viewer->getId();
  //action for new viewer
  connect(viewer, SIGNAL(currentViewerChanged(int)), this, SLOT(updateCurrent(int)));
  return;
}

void VtkView::addView(VtkWidget* viewer,Qt::Orientation orient){
  /* The Viewers are organized like a BSP tree.
  Every new viewer is added within an Horizontal splitter. Its orientation could change according to next insertions.
  HSplit
  /   \
  View1   VSplit
  /   \
  View2  HSplit
  /
  View3
  In the GUI, when a viewer is splitted, the new one appears on its right (the space is split in two equal portions).
  */

  //CASE 0: only when the first viewer is opened, just add it and return;
  if (viewerCounter()==0)
  {
    viewerList.append(viewer);
    addWidget(viewer);
    currentId = viewer->getId();
    //action for new viewer
    connect(viewer, SIGNAL(currentViewerChanged(int)), this, SLOT(updateCurrent(int)));
    return;
  }

  //CASE 1: happens only at the FIRST split;
  if (viewerCounter()==1)
  {
    //MK: add a new viewer class
    viewerList.append(viewer);
    addWidget(viewer);

    //MK: separating window
    this->setOrientation(orient);
    QList<int> sizes;
    if(this->orientation()== Qt::Horizontal){
      sizes.append(this->width()/2);
      sizes.append(this->width()/2);
    }
    else{
      sizes.append(this->height()/2);
      sizes.append(this->height()/2);
    }

    this->setSizes(sizes);
    this->setHandleWidth(2);
    this->setChildrenCollapsible(false);

    //MK: give a new id
    currentId = viewer->getId();

    //action for new viewer
    connect(viewer, SIGNAL(currentViewerChanged(int)), this, SLOT(updateCurrent(int)));
    return;
  }

  // Generic Case: Each splitter Has ALWAYS two children.
  viewerList.append(viewer);
  VtkWidget* currentGLA = this->currentView();
  VtkSplitter* currentVtkSplitter = qobject_cast<VtkSplitter *>(currentGLA->parent());
  QList<int> parentSizes = currentVtkSplitter->sizes();

  int splittedIndex = currentVtkSplitter->indexOf(currentGLA);
  VtkSplitter* newVtkSplitter = new VtkSplitter(orient);
  currentVtkSplitter->insertWidget(splittedIndex,newVtkSplitter);

  newVtkSplitter->addWidget(viewer);
  newVtkSplitter->addWidget(currentGLA);

  QList<int> sizes;
  if(orient== Qt::Horizontal){
    sizes.append(currentVtkSplitter->width()/2);
    sizes.append(currentVtkSplitter->width()/2);
  }
  else{
    sizes.append(currentVtkSplitter->height()/2);
    sizes.append(currentVtkSplitter->height()/2);
  }
  currentVtkSplitter->setSizes(parentSizes);
  newVtkSplitter->setSizes(sizes);
  newVtkSplitter->setHandleWidth(2);
  newVtkSplitter->setChildrenCollapsible(false);

  currentId = viewer->getId();
  //action for new viewer
  connect(viewer, SIGNAL(currentViewerChanged(int)), this, SLOT(updateCurrent(int)));
  return;
}

void VtkView::removeView(int viewerId)
{
  VtkWidget* viewer;
  for (int i=0; i< viewerList.count(); i++)
  {
   if(viewerList.at(i)->getId() == viewerId)
      viewer = viewerList.at(i);
  }
  assert(viewer);

  VtkSplitter* parentVtkSplitter = qobject_cast<VtkSplitter *>(viewer->parent());
  int currentIndex = parentVtkSplitter->indexOf(viewer);

  viewer->deleteLater();
  // Very basic case of just two son of the MultiviewContainer.
  if(viewerList.count()==2)
  {
    viewerList.removeAll(viewer);
    currentId = viewerList.first()->getId();

    return;
  }

  // generic tree with more of two leaves (some splitter involved)
  // two cases
  // 1) the deleted object is a direct child of the root
  // 2) otherwise; e.g. parent->parent exists.


  // First Case: deleting the direct son of the root (the mvc)
  // the sibling content (that is a splitter) surely will be moved up
  if(parentVtkSplitter == this)
  {
    int insertIndex;
    if(currentIndex == 0) insertIndex = 1;
    else insertIndex = 0;

    VtkSplitter *siblingVtkSplitter = qobject_cast<VtkSplitter *>(this->widget(insertIndex));
    assert(siblingVtkSplitter);

    QWidget *sonLeft = siblingVtkSplitter->widget(0);
    QWidget *sonRight = siblingVtkSplitter->widget(1);
    this->setOrientation(siblingVtkSplitter->orientation());
    this->insertWidget(0,sonLeft);
    this->insertWidget(1,sonRight);
    siblingVtkSplitter->deleteLater();
    viewerList.removeAll(viewer);
    currentId = viewerList.first()->getId();

    return;
  }

// Final case. Very generic, not son of the root.

VtkSplitter* parentParentVtkSplitter = qobject_cast<VtkSplitter *>(parentVtkSplitter->parent());
assert(parentParentVtkSplitter);
int parentIndex= parentParentVtkSplitter->indexOf(parentVtkSplitter);

int siblingIndex;
if(currentIndex == 0) siblingIndex = 1;
else siblingIndex = 0;

QWidget  *siblingWidget = parentVtkSplitter->widget(siblingIndex);
assert(siblingWidget);
parentParentVtkSplitter->insertWidget(parentIndex,siblingWidget);
parentVtkSplitter->deleteLater();
viewerList.removeAll(viewer);
currentId = viewerList.first()->getId();

}

void VtkView::updateCurrent(int current){
  int previousCurrentId = currentId;
  currentId=current;
  if(getViewer(previousCurrentId))
        update(previousCurrentId);
  emit updateMainWindowMenus();
}

VtkWidget * VtkView::getViewer(int id)
{
  foreach ( VtkWidget* viewer, viewerList)
    if (viewer->getId() == id)
      return viewer;
  return 0;
}

int VtkView::getViewerByPicking(QPoint p){
  foreach ( VtkWidget* viewer, viewerList){
    QPoint pViewer = viewer->mapFromGlobal(p);
    if(viewer->visibleRegion().contains(pViewer))
      return viewer->getId();
  }
  return -1;
}

VtkWidget* VtkView::currentView(){
  return getViewer(currentId);
}


int VtkView::viewerCounter(){
  return viewerList.count();
}

void VtkView::updateAllViewer(){
  foreach ( VtkWidget* viewer, viewerList)
    viewer->update();
}


void VtkView::update(int id){
  getViewer(id)->update();
}

MainWindow * VtkView::findmw()
{
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
    if (mainwindow)
    {
      return mainwindow;
    }
  }
}

void VtkView::closeEvent( QCloseEvent *event )
{
//  bool close = true;
//  int ii = 0;
//  while(close && (ii < viewerList.size()))
//  {
//    close = viewerList.at(ii)->readyToClose();
//    ++ii;
//  }
//  if (close)
//    event->accept();
//  else
//    event->ignore();
}


