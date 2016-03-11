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
   of ITK/VTK library from Kitware, QT API from Nokia and Meshlab VCG library
   from ISTI CNR. I would like to thank annonymous help by various software
   engineering communities.

*****************************************************************************/

#include "mainWindow.h" // this contains "GLEW.h"

#include <QGLWidget>
#include <QGLContext>
#include <QtGui>
#include <QToolBar>
#include <QProgressBar>
#include <QHttp>
#include <QFileOpenEvent>
#include <QFile>
#include <QtXml>
#include <QDesktopServices>
#include <QLayout>
#include <QMdiArea>

#include "vtkAssembly.h"
#include "vtkCamera.h"

#include "lightControl.h"
#include "plotView.h"
#include "CTControl.h"
#include "informationWidget.h"
#include "bookmarkWidget.h"
#include "searchWidget.h"
#include "lightControlRTI.h" //YY
#include "renderingdialog.h" // YY

QProgressBar *MainWindow::qb;

MainWindow::MainWindow()
{
//  setAttribute(Qt::WA_DeleteOnClose);// crash

  //--------------------------------------------------------------------//
  //MK: this part sets the subwindow activity checking.
  // Finding the active document data relies on this part.
  mdiArea = new QMdiArea;
  mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setCentralWidget(mdiArea);

  connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),this, SLOT(updateMenus()));
  connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),this, SLOT(updateWindowMenu()));
  connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),this, SLOT(updateAllViews()) );//MK

//  connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)),this, SLOT(updateStdDialog()));

  windowMapper = new QSignalMapper(this);
  connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));
  connect(windowMapper, SIGNAL(mapped(QWidget*)),this, SLOT(wrapSetActiveSubWindow(QWidget *)));
  //--------------------------------------------------------------------//


//  httpReq=new QHttp(this);
//  connect(httpReq, SIGNAL(done(bool)), this, SLOT(connectionDone(bool)));

  // this is for document icon instead of QT icon
#if defined(_WIN32) || defined(_WIN64)
  QIcon icon;
  icon.addPixmap(QPixmap(":/icons/Hyper3D_Logo128.png"));
  setWindowIcon(icon);
#endif

  // reading parameter settings from registry
  readSettings();

  createDockWindows(); // docking toolbard on the right
  createActions(); // createActions() should come second.
  // This is important for MAC
  menuBar = new QMenuBar(0);
  this->setMenuBar(menuBar);
  createMenus(); // MenuBar doesn't work on Lion in Deploy mode (due to a bug of QT 4.7.4)
  createToolBars();

  updateRecentFileActions();
  updateRecentProjActions();

  setAcceptDrops(true);
  mdiArea->setAcceptDrops(true);
  setWindowTitle(appName()+appBits());
  setStatusBar(new QStatusBar(this));
  globalStatusBar()=statusBar();

  qb=new QProgressBar(this);
  qb->setMaximum(100);
  qb->setMinimum(0);
  statusBar()->addPermanentWidget(qb,0);

  QDesktopWidget qdw;
  setMaximumSize(qdw.size());
  setWindowFlags(Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint | Qt::CustomizeWindowHint);

  unsavedChanges = false;
  currentProjectMetadata.createElement("hyper3d.metadata");
  mNewProjectDialog = NULL;
  mProjectInfoDialog = NULL;
  isClose = false;

  setUnifiedTitleAndToolBarOnMac(true);

  updateAllViews();
}

// not a problem
void MainWindow::updateAllViews()
{
	
  VtkView *mvc = currentVtkView();
  if(mvc)
  {
    mvc->updateAllViewer();
  }

  if(VTKA()){
      this->mBookmark->refreshBookmarkList();
      VTKA()->update();
  } else {
      currentProjectMetadata.clear();
  }
  if (mdiArea->subWindowList().size() > 0)
  {
	  if (!mdiArea->currentSubWindow()->isHidden())
		mInformation->reloadAnnotation();
	  else
	  {
		mInformation->clearAnnotation();
	  }
  }
  // having this here is important!
  updateMenus();
  updateWindowMenu();
}

//-----------------------------------------------------
// slots for menu

void MainWindow::zoomIn()
{
    if (VTKA()) VTKA()->zoomIn();
}

void MainWindow::zoomOut()
{
    if (VTKA()) VTKA()->zoomOut();
}

void MainWindow::zoomReset()
{
    if (VTKA()) VTKA()->zoomReset();
}

void MainWindow::flattenMesh()
{
    if (VTKA()) VTKA()->flattenMesh();
}

void MainWindow::renderTexture()
{
  QAction *a = qobject_cast<QAction* >(sender());
  if (VTKA()) VTKA()->setTextureOn(a->isChecked() ? true : false);
  updateAllViews();
}

void MainWindow::renderInterpolation()
{
  QAction *a = qobject_cast<QAction* >(sender());
  if (VTKA()) VTKA()->setInterpolateOn(a->isChecked() ? true : false);
  updateAllViews();
}

void MainWindow::renderInformation()
{
  QAction *a = qobject_cast<QAction* >(sender());
  if (VTKA()) VTKA()->setDisplayInfoOn(a->isChecked() ? true : false);
  updateAllViews();
}

void MainWindow::renderPolyIndicate()
{
  QAction *a = qobject_cast<QAction* >(sender());
  if (VTKA()) VTKA()->setDisplayPolyIndicateOn(a->isChecked() ? true : false);
  updateAllViews();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	isClose = true;
    mdiArea->closeAllSubWindows();
	isClose = false;
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

bool MainWindow::closeAll()
{
    if(closeProject()) 
	{
		isClose = true;	// skip the eventfilter
        mdiArea->closeAllSubWindows();
		isClose = false;
		return true;
    }
	return false;
}

void MainWindow::closeAllWindows()
{   
    if(!VTKA()) 
	{
        closeAll();
		return;
    }
	else
	{
		foreach(QMdiSubWindow* subWindow, mdiArea->subWindowList())
		{
			mdiArea->setActiveSubWindow(subWindow);
			if (!this->mInformation->checkObjectSaved())
			{ 
				int ret = QMessageBox::warning(this, tr("Warning"),
								   tr("The current object has unsaved changes. Do you wish to save before closing this window?"),
								   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
								   QMessageBox::Save);
				if(ret == QMessageBox::Cancel) 
				{
					return;
				}
				else if (ret == QMessageBox::Save)
				{
					this->mInformation->saveObjectNotes();
				}
				else
				{
					this->mInformation->removeUnSavedNotes();
				}
			}
			VTKA()->closeFileInfo();
			this->mInformation->closeObjectNotes();
			subWindow->hide();
		}
		updateMenus();
		updateAllViews();
	}	
}

void MainWindow::closeWindow()
{   
    if(!VTKA()) {
        closeAll();
		return;
    }
	else
	{
		if (!this->mInformation->checkObjectSaved())
		{ 
			int ret = QMessageBox::warning(this, tr("Warning"),
							   tr("The current object has unsaved changes. Do you wish to save before closing this window?"),
							   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
							   QMessageBox::Save);
			if(ret == QMessageBox::Cancel) 
			{
				return;
			}
			else if (ret == QMessageBox::Save)
			{
				this->mInformation->saveObjectNotes();
			}
			else
			{
				this->mInformation->removeUnSavedNotes();
			}
		}
		VTKA()->closeFileInfo();
		this->mInformation->closeObjectNotes();
	}
	QMdiSubWindow* subWindow = mdiArea->currentSubWindow();
	subWindow->hide();
	mdiArea->activateNextSubWindow();
	//qDebug()<<"window size"<<mdiArea->subWindowList().size();
	updateMenus();
    updateAllViews();
}

void MainWindow::openWindow()
{
	OpenWindowDialog* dialog = new OpenWindowDialog();
	QList<QMdiSubWindow*> subWindows = mdiArea->subWindowList();
	foreach(QMdiSubWindow* w, subWindows)
	{
		if (!w->isHidden())
			continue;
		VtkView* mvc = qobject_cast<VtkView *>(w->widget());
        QStringList list = mvc->currentView()->mFilename.split(QDir::separator());
		QString name = list[list.size()-1];
		dialog->addItem(name);
	}
	dialog->exec();
	QStringList selection = dialog->selectedItems();
	if (selection.size() == 0)
		return;
	for (int i = 0; i < selection.size(); i++)
	{
		foreach(QMdiSubWindow* w, subWindows)
		{
			VtkView* mvc = qobject_cast<VtkView *>(w->widget());
			QStringList list = mvc->currentView()->mFilename.split(QDir::separator());
			QString name = list[list.size()-1];
			if (name == selection[i] && w->isHidden())
			{
				w->show();
				mdiArea->setActiveSubWindow(w);
				break;
			}
		}
	}
	delete dialog;
	updateAllViews();
}

bool MainWindow::closeProject()
{
	if(((!this->mInformation->checkAllSaved()) && VTKA())) {
        QMessageBox mb;
        QString message;

        message = tr("The current project has unsaved changes. Do you wish to save before closing this project?"),
        mb.setText(message);
        mb.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        mb.setDefaultButton(QMessageBox::Save);
        int ret = mb.exec();

        if(ret == QMessageBox::Save) this->saveProject();
        else if(ret == QMessageBox::Cancel) return false;
    }

    unsavedChanges = false;
    currentProjectName = QString();
    currentProjectSave = QString();
	currentProjectFullName = QString();
    currentProjectMetadata.clear();
    QDomElement root = currentProjectMetadata.createElement("hyper3d.metadata");
    currentProjectMetadata.appendChild(root);
	this->mInformation->closeAllNotes();
	if (mProjectInfoDialog)
	{
		mProjectInfoDialog->hide();
		delete mProjectInfoDialog;
		mProjectInfoDialog = NULL;
	}

    QList<QMdiSubWindow*> windows = mdiArea->subWindowList();
    foreach(QMdiSubWindow *w, windows)
    {
        VtkView* mvc = qobject_cast<VtkView *>(w->widget());
        QString file = mvc->currentView()->mFilename;
        QFileInfo fi(file);
		VtkWidget* gla = mvc->currentView();
		gla->closeFileInfo();

        w->setWindowTitle(fi.fileName());
    }

    updateMenus();

    return true;
}

void MainWindow::updateXML()
{
	QDomDocument doc;
    QDomElement root, info, layouts;
    QDomProcessingInstruction instruct = doc.createProcessingInstruction("xml", "version=\"1.0\"");
    doc.appendChild(instruct);

    root = doc.createElement("hyper3d.project");
    root.setAttribute("name", currentProjectName);
    root.setAttribute("date", QDateTime::currentDateTimeUtc().toString());
	root.setAttribute("user", mUserName);
    doc.appendChild(root);

	QDomElement projectInfo = doc.createElement("project");
	root.appendChild(projectInfo);
	QDomElement keyword = doc.createElement("keyword");
	keyword.appendChild(doc.createTextNode(currentProjectKeyword));
	QDomElement affiliation = doc.createElement("affiliation");
	affiliation.appendChild(doc.createTextNode(currentProjectAffiliation));
	QDomElement description = doc.createElement("description");
	description.appendChild(doc.createTextNode(currentProjectDescription));
	projectInfo.appendChild(keyword);
	projectInfo.appendChild(affiliation);
	projectInfo.appendChild(description);
	
    QList<QMdiSubWindow*> windows = mdiArea->subWindowList();
    foreach(QMdiSubWindow *w, windows)
	{
        VtkView* mvc = qobject_cast<VtkView *>(w->widget());
        VtkWidget* gla = mvc->currentView();
        QString file = gla->mFilename;
        QFileInfo finfo(file);
        QDomElement item = doc.createElement("item");
        item.setAttribute("filename", file);

        vtkSmartPointer<vtkCamera> camera = gla->mRenderer->GetActiveCamera();
        double pos[3]; // camera position
        double foc[3]; // focal point
        double angle; // view angle
        double view[3]; // view up direction
        double clip[2]; // clipping range
        double scale; // parallel scale

        camera->GetPosition(pos);
        camera->GetFocalPoint(foc);
        angle = camera->GetViewAngle();
        camera->GetViewUp(view);
        camera->GetClippingRange(clip);
        scale = camera->GetParallelScale();

        QDomElement position = doc.createElement("camera_position");
        item.appendChild(position);
        QDomElement pos_x = doc.createElement("x");
        QDomElement pos_y = doc.createElement("y");
        QDomElement pos_z = doc.createElement("z");
        position.appendChild(pos_x);
        position.appendChild(pos_y);
        position.appendChild(pos_z);
        pos_x.appendChild(doc.createTextNode(QString::number(pos[0])));
        pos_y.appendChild(doc.createTextNode(QString::number(pos[1])));
        pos_z.appendChild(doc.createTextNode(QString::number(pos[2])));

        QDomElement focal = doc.createElement("focal_point");
        item.appendChild(focal);
        QDomElement foc_x = doc.createElement("x");
        QDomElement foc_y = doc.createElement("y");
        QDomElement foc_z = doc.createElement("z");
        focal.appendChild(foc_x);
        focal.appendChild(foc_y);
        focal.appendChild(foc_z);
        foc_x.appendChild(doc.createTextNode(QString::number(foc[0])));
        foc_y.appendChild(doc.createTextNode(QString::number(foc[1])));
        foc_z.appendChild(doc.createTextNode(QString::number(foc[2])));

        QDomElement ang = doc.createElement("camera_angle");
        item.appendChild(ang);
        ang.appendChild(doc.createTextNode(QString::number(angle)));

        QDomElement viewup = doc.createElement("view_up");
        item.appendChild(viewup);
        QDomElement viewup_x = doc.createElement("x");
        QDomElement viewup_y = doc.createElement("y");
        QDomElement viewup_z = doc.createElement("z");
        viewup.appendChild(viewup_x);
        viewup.appendChild(viewup_y);
        viewup.appendChild(viewup_z);
        viewup_x.appendChild(doc.createTextNode(QString::number(view[0])));
        viewup_y.appendChild(doc.createTextNode(QString::number(view[1])));
        viewup_z.appendChild(doc.createTextNode(QString::number(view[2])));

        QDomElement clipping = doc.createElement("clipping_range");
        item.appendChild(clipping);
        QDomElement clip_near = doc.createElement("near_plane");
        QDomElement clip_far = doc.createElement("far_plane");
        clipping.appendChild(clip_near);
        clipping.appendChild(clip_far);
        clip_near.appendChild(doc.createTextNode(QString::number(clip[0])));
        clip_far.appendChild(doc.createTextNode(QString::number(clip[1])));

        QDomElement parallel = doc.createElement("parallel_scale");
        item.appendChild(parallel);
        parallel.appendChild(doc.createTextNode(QString::number(scale)));

        QDomElement filetype = doc.createElement("filetype");
        item.appendChild(filetype);
        filetype.appendChild(doc.createTextNode(QString::number(gla->getWidgetMode())));

        // Filetype-specific information
        switch(gla->getWidgetMode())
        {
        case IMAGE2D:
        {
            QDomElement interp = doc.createElement("interpolation_on");
            item.appendChild(interp);
            if (gla->getIsInterpolationOn()) {
                interp.appendChild(doc.createTextNode(QString::number(INTERPOLATION_ON)));
            } else {
                interp.appendChild(doc.createTextNode(QString::number(INTERPOLATION_OFF)));
            }
            break;
        }

        case MODEL3D:
        {
            QDomElement dir = doc.createElement("directional_light");
            item.appendChild(dir);
            dir.appendChild(doc.createTextNode(QString::number(gla->getIsDirectionalLight())));

            vtkSmartPointer<vtkAssembly> assembly = this->mLightControl->GetAssembly();
            double orientation[3];
            assembly->GetOrientation(orientation);

            QDomElement ort = doc.createElement("light_vector");
            item.appendChild(ort);
            QDomElement ort_x = doc.createElement("x");
            QDomElement ort_y = doc.createElement("y");
            QDomElement ort_z = doc.createElement("z");
            ort.appendChild(ort_x);
            ort.appendChild(ort_y);
            ort.appendChild(ort_z);
            ort_x.appendChild(doc.createTextNode(QString::number(orientation[0])));
            ort_y.appendChild(doc.createTextNode(QString::number(orientation[1])));
            ort_z.appendChild(doc.createTextNode(QString::number(orientation[2])));

            QDomElement brtns = doc.createElement("brightness");
            item.appendChild(brtns);
            brtns.appendChild(doc.createTextNode(QString::number(this->mLightControl->GetIntensityL1())));

            QDomElement ctrst = doc.createElement("contrast");
            item.appendChild(ctrst);
            ctrst.appendChild(doc.createTextNode(QString::number(this->mLightControl->GetIntensityL2())));

            if (gla->getIsInterpolationOn()) {
                QDomElement interp = doc.createElement("interpolation_on");
                item.appendChild(interp);
                interp.appendChild(doc.createTextNode(QString::number(INTERPOLATION_ON)));
            } else if(!gla->getmRgbTextureFilename().isEmpty()){
                QDomElement interp = doc.createElement("interpolation_on");
                item.appendChild(interp);
                interp.appendChild(doc.createTextNode(QString::number(INTERPOLATION_OFF)));
            }

            QDomElement display = doc.createElement("display_mode");
            item.appendChild(display);
            if(gla->getRenderMode3D() == POINTS3D) {
                display.appendChild(doc.createTextNode(QString::number(POINTS3D)));
            } else if(gla->getRenderMode3D() == WIREFRAME3D) {
                display.appendChild(doc.createTextNode(QString::number(WIREFRAME3D)));
            } else {
                display.appendChild(doc.createTextNode(QString::number(SURFACE3D)));
            }

            if(gla->getIsTextureOn()) {
                QDomElement texture = doc.createElement("texture_on");
                item.appendChild(texture);
                texture.appendChild(doc.createTextNode(QString::number(TEXTURE_ON)));
				qDebug() << "texture on";
            } else if (!gla->getmRgbTextureFilename().isEmpty()){
                QDomElement texture = doc.createElement("texture_on");
                item.appendChild(texture);
                texture.appendChild(doc.createTextNode(QString::number(TEXTURE_OFF)));
				qDebug() << "texture off";
            }
            break;
        }

        case CTSTACK:
        {
            QDomElement brtns = doc.createElement("brightness");
            item.appendChild(brtns);
            brtns.appendChild(doc.createTextNode(QString::number(this->mLightControl->GetIntensityL1())));

            QDomElement ctrst = doc.createElement("contrast");
            item.appendChild(ctrst);
            ctrst.appendChild(doc.createTextNode(QString::number(this->mLightControl->GetIntensityL2())));

            QDomElement interp = doc.createElement("interpolation_on");
            item.appendChild(interp);
            if (gla->getIsInterpolationOn()) {
                interp.appendChild(doc.createTextNode(QString::number(INTERPOLATION_ON)));
            } else {
                interp.appendChild(doc.createTextNode(QString::number(INTERPOLATION_OFF)));
            }

            QDomElement slice = doc.createElement("current_slice");
            item.appendChild(slice);
            slice.appendChild(doc.createTextNode(QString::number(this->mCtControl->GetSliceCurrent())));

            QDomElement ort = doc.createElement("current_orientation");
            item.appendChild(ort);
            ort.appendChild(doc.createTextNode(QString::number(this->mCtControl->GetOrientationCurrent())));
            break;
        }

        case CTVOLUME:
        {
            vtkSmartPointer<vtkAssembly> assembly = this->mLightControl->GetAssembly();
            double orientation[3];
            assembly->GetOrientation(orientation);

            QDomElement ort = doc.createElement("light_vector");
            item.appendChild(ort);
            QDomElement ort_x = doc.createElement("x");
            QDomElement ort_y = doc.createElement("y");
            QDomElement ort_z = doc.createElement("z");
            ort.appendChild(ort_x);
            ort.appendChild(ort_y);
            ort.appendChild(ort_z);
            ort_x.appendChild(doc.createTextNode(QString::number(orientation[0])));
            ort_y.appendChild(doc.createTextNode(QString::number(orientation[1])));
            ort_z.appendChild(doc.createTextNode(QString::number(orientation[2])));

            QDomElement mode = doc.createElement("render_mode");
            item.appendChild(mode);
            mode.appendChild(doc.createTextNode(QString::number(gla->getVolumeRenderMode())));

            QDomElement blend = doc.createElement("blend_type");
            item.appendChild(blend);
            blend.appendChild(doc.createTextNode(QString::number(gla->getBlendType())));

            QDomElement resolution = doc.createElement("resolution");
            item.appendChild(resolution);
            resolution.appendChild(doc.createTextNode(QString::number(gla->getReductionFactor())));
            break;
        }

        case RTI2D:
        {
            break;
        }

        default:
            break;
        }

        root.appendChild(item);
    }

    info = doc.createElement("info");
    info.setAttribute("date", QDateTime::currentDateTimeUtc().toString());
    root.appendChild(info);

    layouts = doc.createElement("layouts");
    root.appendChild(layouts);

    QFile f(currentProjectSave);
    if(!f.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox mb;
        mb.critical(this, tr("Project Error"), tr("Failed to save the project! Cannot open XML file."));
        f.close();
        return;
    }
    f.resize(0);
    QTextStream out(&f);
    out << doc.toString();
    f.close();
}


bool MainWindow::readXML(QString fileName, QVector<QString> &objectList, bool import)
{
  QFileInfo fi(fileName);
  double pos[3]; // camera position
  double foc[3]; // focal point
  double angle; // view angle
  double view[3]; // view up direction
  double clip[2]; // clipping range
  double scale; // parallel scale
  int filetype;
  double brightness = this->mLightControl->GetIntensityL1();
  double contrast = this->mLightControl->GetIntensityL2();
  int directionalLightOn;
  int interpolationOn;
  int mode3D;
  int textureOn;
  int ctSlice = 0;
  int ctOrientation = 0;
  int ctMode;
  int ctBlendType;
  double ctResolution;
  double orientation[3];

  vtkSmartPointer<vtkAssembly> assembly = this->mLightControl->GetAssembly();
  assembly->GetOrientation(orientation);

  QFile file(fi.absoluteFilePath());
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QMessageBox::critical(this, tr("Project Error"), tr("Cannot open project."));
      return false;
  }

  QDomDocument doc;
  doc.setContent(&file);
  qDebug() << fi.absoluteFilePath();
  QDomNodeList list = doc.elementsByTagName("hyper3d.project");
  if(list.isEmpty()) {
      QString message = fi.fileName() + tr(" is not a valid Hyper3D project file.");
      QMessageBox::critical(this, tr("Project Error"), message);
      return false;
  }

  currentProjectFullName = QDir::toNativeSeparators(QDir::currentPath());
  QDir currentFolder(QDir::currentPath());

  QDomElement root = list.at(0).toElement();
  QString name = root.attribute("name");
  mUserName = root.attribute("user");

  //If the readXML is called by importProject, then project name should not be changed to the imporedt project name
  //Otherwise, it should be called by openProject, then project name should be changed.
  if (!import)	currentProjectName = name;	
  qDebug()<<"Project Name"<<currentProjectName;

  QDomNodeList projectInfo = root.elementsByTagName("project");
  if (projectInfo.isEmpty())
  qDebug()<<projectInfo.length();
  currentProjectKeyword = projectInfo.at(0).toElement().elementsByTagName("keyword").at(0).toElement().text();
  currentProjectAffiliation = projectInfo.at(0).toElement().elementsByTagName("affiliation").at(0).toElement().text();
  currentProjectDescription = projectInfo.at(0).toElement().elementsByTagName("description").at(0).toElement().text();

  qDebug()<<"in read xml"<<mUserName;
  qDebug()<<"in read xml"<<currentProjectKeyword;
  qDebug()<<"in read xml"<<currentProjectAffiliation;
  qDebug()<<"in read xml"<<currentProjectDescription;


  QDomNodeList items = root.elementsByTagName("item");
  QList<QDomElement> eltList;
  QList<QMdiSubWindow*> windowList;
  
  for(int i = 0; i < items.length(); i++) {
    QDomElement elt = items.at(i).toElement();
    QString fn = elt.attribute("filename");
	fn = QDir::toNativeSeparators(fn);
	objectList.push_back(fn);
	QFileInfo finfo(fn);
    QDomNodeList ftlist = elt.elementsByTagName("filetype");
    filetype = ftlist.at(0).toElement().text().toInt();
    bool success = false;
	if (fn != QString())
	{
		if(filetype == CTSTACK || filetype == CTVOLUME) 
		{
			qDebug()<<"in read XML open DICOM"<<fn;
			success = openDICOM(fn, false);
		} 
		else 
		{
			success = openImages(fn, false);
		}
	}
    if(!success) continue;

    // DT: Assumes that windows are sorted from first created to last created (default behavior).
    QList<QMdiSubWindow*> windows = mdiArea->subWindowList();
    QMdiSubWindow* w = windows.at(windows.length()-1);

    windowList.append(w);
    eltList.append(elt);
  }

  mdiArea->tileSubWindows();

  for(int i = 0; i < windowList.length(); i++) {
      QMdiSubWindow* w = windowList.at(i);
      QDomElement elt = eltList.at(i);
      QDomNodeList ftlist = elt.elementsByTagName("filetype");
      filetype = ftlist.at(0).toElement().text().toInt();

      if(!w) continue;

      mdiArea->setActiveSubWindow(w);
      VtkView* mvc = qobject_cast<VtkView *>(w->widget());
      VtkWidget* gla = mvc->currentView();
      QString file = gla->mFilename;
      QFileInfo fi(file);
      w->setWindowTitle(currentProjectName + QString(" : ") + fi.fileName());

      vtkSmartPointer<vtkCamera> camera = gla->mRenderer->GetActiveCamera();
      QDomNodeList list1 = elt.elementsByTagName("camera_position");
      if(!list1.isEmpty()) {
          pos[0] = list1.at(0).toElement().elementsByTagName("x").at(0).toElement().text().toDouble();
          pos[1] = list1.at(0).toElement().elementsByTagName("y").at(0).toElement().text().toDouble();
          pos[2] = list1.at(0).toElement().elementsByTagName("z").at(0).toElement().text().toDouble();
          camera->SetPosition(pos);
      }
      QDomNodeList list2 = elt.elementsByTagName("focal_point");
      if(!list2.isEmpty()) {
          foc[0] = list2.at(0).toElement().elementsByTagName("x").at(0).toElement().text().toDouble();
          foc[1] = list2.at(0).toElement().elementsByTagName("y").at(0).toElement().text().toDouble();
          foc[2] = list2.at(0).toElement().elementsByTagName("z").at(0).toElement().text().toDouble();
          camera->SetFocalPoint(foc);
      }
      QDomNodeList list3 = elt.elementsByTagName("camera_angle");
      if(!list3.isEmpty()) {
          angle = list3.at(0).toElement().text().toDouble();
          camera->SetViewAngle(angle);
      }
      QDomNodeList list4 = elt.elementsByTagName("view_up");
      if(!list4.isEmpty()) {
          view[0] = list4.at(0).toElement().elementsByTagName("x").at(0).toElement().text().toDouble();
          view[1] = list4.at(0).toElement().elementsByTagName("y").at(0).toElement().text().toDouble();
          view[2] = list4.at(0).toElement().elementsByTagName("z").at(0).toElement().text().toDouble();
          camera->SetViewUp(view);
      }
      QDomNodeList list5 = elt.elementsByTagName("clipping_range");
      if(!list5.isEmpty()) {
          clip[0] = list5.at(0).toElement().elementsByTagName("near_plane").at(0).toElement().text().toDouble();
          clip[1] = list5.at(0).toElement().elementsByTagName("far_plane").at(0).toElement().text().toDouble();
          camera->SetClippingRange(clip);
      }
      QDomNodeList list6 = elt.elementsByTagName("parallel_scale");
      if(!list6.isEmpty()) {
          scale = list6.at(0).toElement().text().toDouble();
          camera->SetParallelScale(scale);
      }

      if(filetype == IMAGE2D) {
          list = elt.elementsByTagName("interpolation_on");
          if(!list.isEmpty()) {
              interpolationOn = list.at(0).toElement().text().toInt();
              if(interpolationOn == INTERPOLATION_ON) {
                  this->renderModeInterpolationAct->setChecked(true);
                  gla->setInterpolateOn(true);
              } else {
                  this->renderModeInterpolationAct->setChecked(false);
                  gla->setInterpolateOn(false);
              }
          }
      } else if(filetype == MODEL3D) {
          list = elt.elementsByTagName("directional_light");
          if(!list.isEmpty()) {
              directionalLightOn = list.at(0).toElement().text().toInt();
              if(directionalLightOn) {
                  this->setDirectionalLightAct->setChecked(true);
                  this->mLightControl->setDirectionalLight(true);
              } else {
                  this->setDirectionalLightAct->setChecked(false);
                  this->mLightControl->setDirectionalLight(false);
              }
          } else {
              this->setDirectionalLightAct->setChecked(true);
              this->mLightControl->setDirectionalLight(true);
          }
          list = elt.elementsByTagName("light_vector");
          if(!list.isEmpty()) {
              orientation[0] = list.at(0).toElement().elementsByTagName("x").at(0).toElement().text().toDouble();
              orientation[1] = list.at(0).toElement().elementsByTagName("y").at(0).toElement().text().toDouble();
              orientation[2] = list.at(0).toElement().elementsByTagName("z").at(0).toElement().text().toDouble();
          }
          list = elt.elementsByTagName("brightness");
          if(!list.isEmpty()) {
              brightness = list.at(0).toElement().text().toDouble();
          }
          list = elt.elementsByTagName("contrast");
          if(!list.isEmpty()) {
              contrast = list.at(0).toElement().text().toDouble();
          }

          list = elt.elementsByTagName("interpolation_on");
          if(!list.isEmpty()) {
              interpolationOn = list.at(0).toElement().text().toInt();
              qDebug() << interpolationOn;
              if(interpolationOn == INTERPOLATION_ON) {
                  this->renderModeInterpolationAct->setChecked(true);
                  gla->setInterpolateOn(true);
              } else {
                  this->renderModeInterpolationAct->setChecked(false);
                  gla->setInterpolateOn(false);
              }
          }
          list = elt.elementsByTagName("display_mode");
          if(!list.isEmpty()) {
              mode3D = list.at(0).toElement().text().toInt();
              if(mode3D == POINTS3D) {
                  this->renderModePointsAct->setChecked(true);
                  this->renderModeWireAct->setChecked(false);
                  this->renderModeFlatAct->setChecked(false);
                  gla->setRenderMode3D(POINTS3D);
              } else if(mode3D == WIREFRAME3D) {
                  this->renderModePointsAct->setChecked(false);
                  this->renderModeWireAct->setChecked(true);
                  this->renderModeFlatAct->setChecked(false);
                  gla->setRenderMode3D(WIREFRAME3D);
              } else if (mode3D == SURFACE3D){
                  this->renderModePointsAct->setChecked(false);
                  this->renderModeWireAct->setChecked(false);
                  this->renderModeFlatAct->setChecked(true);
                  gla->setRenderMode3D(SURFACE3D);
              }
          }
          list = elt.elementsByTagName("texture_on");
          if(!list.isEmpty()) {
              textureOn = list.at(0).toElement().text().toInt();
              if(textureOn == TEXTURE_ON) {
                  this->renderModeTextureAct->setChecked(true);
                  this->renderModeTextureAct->setIcon(QIcon(":/images/textureson.png"));
                  gla->setTextureOn(true);
              } else if(textureOn == TEXTURE_OFF){
                  this->renderModeTextureAct->setChecked(false);
                  this->renderModeTextureAct->setIcon(QIcon(":/images/texturesoff.png"));
                  gla->setTextureOn(false);
              }
          }
          this->mLightControl->restoreBookmarkLight(orientation, brightness, contrast, MODEL3D);

      } else if(filetype == CTSTACK) {
          list = elt.elementsByTagName("brightness");
          if(!list.isEmpty()) {
              brightness = list.at(0).toElement().text().toDouble();
          }
          list = elt.elementsByTagName("contrast");
          if(!list.isEmpty()) {
              contrast = list.at(0).toElement().text().toDouble();
          }

          list = elt.elementsByTagName("interpolation_on");
          if(!list.isEmpty()) {
              interpolationOn = list.at(0).toElement().text().toInt();
              if(interpolationOn == INTERPOLATION_ON) {
                  this->renderModeInterpolationAct->setChecked(true);
                  gla->setInterpolateOn(true);
              } else {
                  this->renderModeInterpolationAct->setChecked(false);
                  gla->setInterpolateOn(false);
              }
          }

          list = elt.elementsByTagName("current_slice");
          if(!list.isEmpty()) {
              ctSlice = list.at(0).toElement().text().toInt();
          }
          list = elt.elementsByTagName("current_orientation");
          if(!list.isEmpty()) {
              ctOrientation = list.at(0).toElement().text().toInt();
          }

          this->mLightControl->restoreBookmarkLight(orientation, brightness, contrast, CTSTACK);
          this->mCtControl->restoreBookmarkCTStack(ctSlice, ctOrientation);

          if(!list1.isEmpty()) camera->SetPosition(pos);
          if(!list2.isEmpty()) camera->SetFocalPoint(foc);
          if(!list3.isEmpty()) camera->SetViewAngle(angle);
          if(!list4.isEmpty()) camera->SetViewUp(view);
          if(!list5.isEmpty()) camera->SetClippingRange(clip);
          if(!list6.isEmpty()) camera->SetParallelScale(scale);

      } else if(filetype == CTVOLUME) {
          list = elt.elementsByTagName("render_mode");
          if(!list.isEmpty()) {
              ctMode = list.at(0).toElement().text().toInt();
          }
          list = elt.elementsByTagName("blend_type");
          if(!list.isEmpty()) {
              ctBlendType = list.at(0).toElement().text().toInt();
          }
          list = elt.elementsByTagName("resolution");
          if(!list.isEmpty()) {
              ctResolution = list.at(0).toElement().text().toDouble();
          }
          list = elt.elementsByTagName("light_vector");
          if(!list.isEmpty()) {
              orientation[0] = list.at(0).toElement().elementsByTagName("x").at(0).toElement().text().toDouble();
              orientation[1] = list.at(0).toElement().elementsByTagName("y").at(0).toElement().text().toDouble();
              orientation[2] = list.at(0).toElement().elementsByTagName("z").at(0).toElement().text().toDouble();
          }
          qDebug() << orientation[0] << orientation[1] << orientation[2];

          this->mCtControl->restoreBookmarkCTVolume(ctMode, ctBlendType, ctResolution);
          //this->mLightControl->restoreBookmarkLight(orientation, brightness, contrast, CTVOLUME);

          if(!list1.isEmpty()) camera->SetPosition(pos);
          if(!list2.isEmpty()) camera->SetFocalPoint(foc);
          if(!list3.isEmpty()) camera->SetViewAngle(angle);
          if(!list4.isEmpty()) camera->SetViewUp(view);
          if(!list5.isEmpty()) camera->SetClippingRange(clip);
          if(!list6.isEmpty()) camera->SetParallelScale(scale);

          if(!list1.isEmpty()) camera->SetPosition(pos);
          if(!list2.isEmpty()) camera->SetFocalPoint(foc);
          if(!list3.isEmpty()) camera->SetViewAngle(angle);
          if(!list4.isEmpty()) camera->SetViewUp(view);
          if(!list5.isEmpty()) camera->SetClippingRange(clip);
          if(!list6.isEmpty()) camera->SetParallelScale(scale);

          if (gla->mQVTKWidget) gla->mQVTKWidget->show();
          if (gla->mQVTKWidget) gla->mQVTKWidget->update();
      } else if(filetype == RTI2D) {
      } else {
      }
  }
  return true;
}

// Save project. It saves the info of all the layers and the layer themselves.
void MainWindow::saveProject()
{
	if (currentProjectFullName.isEmpty())
	{
		QMessageBox mb;
        mb.critical(this, tr("Save Project Error"), tr("Failed to Save Project! Cannot Find the Project."));
		return;
	}
    if(currentProjectSave.isEmpty()) 
	{
		QMessageBox mb;
        mb.critical(this, tr("Save Project Error"), tr("Failed to Save Project! Cannot Find the Project."));
		return;
    }
	this->mInformation->saveAllNotes();
	this->mInformation->closeAllNotes();

    unsavedChanges = false;
	updateXML();
    updateMenus();
    updateAllViews();
    QFileInfo finfo(currentProjectSave);
    saveRecentProjectList(QDir::fromNativeSeparators(finfo.absoluteFilePath()));
}

void MainWindow::saveProjectAs()
{
	SaveProjectAsDialog* dialog = new SaveProjectAsDialog(mUserName, currentProjectKeyword, currentProjectAffiliation, currentProjectDescription, this->lastUsedDirectory.path());
	dialog->exec();
	if (!dialog->checkOk())
		return;

	//QString fn = QFileDialog::getExistingDirectory(this, tr("Save Project As.."), lastSavedDirectory.path().append(""),
	//													QFileDialog::ShowDirsOnly
    //                                                   | QFileDialog::DontResolveSymlinks);
	QString fn = dialog->getProjectPath();
	fn = QDir::toNativeSeparators(fn);

	if(!fn.isEmpty() && QDir(fn).isReadable())
	{
		QString previousName = currentProjectName;
		currentProjectName = dialog->getProjectName();
		mUserName = dialog->getUserName();
		currentProjectKeyword = dialog->getKeyword();
		currentProjectAffiliation = dialog->getAffiliation();
		currentProjectDescription = dialog->getDescription();
		QString newProjectPath = fn;
		if (newProjectPath[newProjectPath.size()-1] == QDir::separator())
		{
			newProjectPath.append(currentProjectName);
		}
		else
		{
			newProjectPath.append(QDir::separator() + currentProjectName);
		}
		if (!QDir().exists(newProjectPath))	QDir().mkdir(newProjectPath);

		cpDir(currentProjectFullName, newProjectPath);

		QString previousXML = newProjectPath;
		previousXML.append(QDir::separator() + previousName + QString(".xml"));
		QFile file(previousXML);
		file.remove();
        currentProjectSave = newProjectPath;
		currentProjectSave.append(QDir::separator() + currentProjectName + QString(".xml"));
		currentProjectFullName = newProjectPath;
        lastSavedDirectory.setPath(currentProjectFullName);
		QList<QMdiSubWindow*> windows = mdiArea->subWindowList();
		this->mInformation->refresh();
		foreach(QMdiSubWindow *w, windows)
		{
			VtkView* mvc = qobject_cast<VtkView *>(w->widget());
			QString file = mvc->currentView()->mFilename;
			QFileInfo fi(file);
			mvc->currentView()->mFilename = currentProjectFullName + QDir::separator() + fi.fileName() + QDir::separator() + fi.fileName();
			mvc->currentView()->mProjectPath = currentProjectFullName + QDir::separator() + fi.fileName();
			w->setWindowTitle(currentProjectFullName + QString(" : ") + fi.fileName());
			this->mInformation->initAnnotation(mvc->currentView()->mProjectPath);
		}
		setWindowTitle(appName()+appBits()+QString(" ")+currentProjectName);
		updateXML();
		
        return;
    } 
	else if (!fn.isEmpty())
	{
		QMessageBox mb;
        mb.critical(this, tr("Save Project Error"), tr("Failed to Save Project! The Path Does not Exist."));
        return;
    }
}

void MainWindow::importProject()
{
	QString fileName = QFileDialog::getOpenFileName(this,tr("Import Project File"), lastUsedDirectory.path(),
                                            "Hyper3D Project (*.xml)");
	fileName = QDir::toNativeSeparators(fileName);
	QFileInfo fi(fileName);
	lastUsedDirectory = fi.absoluteDir();
	if(fileName != QString("") && fi.suffix().toLower()!="xml")
	{
		QMessageBox::critical(this, tr("Project Error"), "Unknown project file extension.");
		return;
	}
	qb->show();
	QVector<QString> objectList;
	if (!readXML(fileName, objectList, true))
	  return;

	
	if (objectList.size() > 0)
	{
		this->mInformation->refresh();
		for (int i = 0; i < objectList.size(); i++)
		{
			QFileInfo finfo(objectList[i]);
			qDebug()<<"import project"<<objectList[i];
			QDir currentDir(currentProjectFullName);
			QDir objectDir = finfo.absoluteDir();
			bool ok = objectDir.cdUp();
			if (!ok || objectDir != currentDir)
			{
				objectDir = finfo.absoluteDir();
				if (currentDir.cd(objectDir.dirName()))
				{
					cpDir(objectDir.absolutePath(), currentDir.absolutePath());
					this->mInformation->initAnnotation(currentDir.absolutePath());
				}
				else
				{
					qDebug()<<"The Object Directory does not exist!"<<currentDir<<objectDir.absolutePath();
				}

			}
		}
	}
	unsavedChanges = false;
	if(this->VTKA() == 0)  return;
	qb->reset();
	saveRecentProjectList(fileName);
	updateXML();
	updateMenus();
	updateAllViews();
}

void MainWindow::createObjectFolder(QString path)
{
	QString filePath = QDir::toNativeSeparators(path);
	QStringList nameElement = filePath.split(QDir::separator());
	QString fileName = nameElement[nameElement.size() - 1];
	QString note = currentProjectFullName;
	QString bookMark = currentProjectFullName;
	note.append(QDir::separator() + fileName + QDir::separator() + QString("Note"));
	bookMark.append(QDir::separator() + fileName + QDir::separator() + QString("BookMark"));
	QDir currentFolder(currentProjectFullName);
	if (currentFolder.exists(fileName))
	{
		qDebug()<<"note"<<note;
		if (!QDir().exists(note))
			QDir().mkpath(note);
		if (!QDir().exists(bookMark))
			QDir().mkpath(bookMark);
	}
	else
	{
		QDir().mkpath(note);
		QDir().mkpath(bookMark);
	}
	QString newObjectPath = currentProjectFullName;
	newObjectPath.append(QDir::separator() + fileName + QDir::separator() + fileName);	// the copied file and other staff are in the subfolder with its own name
	qDebug()<<"in create object folder"<<newObjectPath;
	QFile::copy(filePath, newObjectPath);

	// we also copy texture file if existing
	if (VTKA()->getIsTextureOn()) {
		QString textureFileName = VTKA()->getmRgbTextureFilename();
		if (!textureFileName.isEmpty()) {
			QString newTexturePath = currentProjectFullName;
			QStringList strList = textureFileName.split("/");
			newTexturePath.append(QDir::separator() + fileName + QDir::separator() + strList[strList.size() - 1]);
			QFile::copy(textureFileName, newTexturePath);
		}
	}
}

void MainWindow::createCTFolder(QString path)
{
	QString filePath = QDir::toNativeSeparators(path);
	QStringList nameElement = filePath.split(QDir::separator());
	QString fileName = nameElement[nameElement.size() - 1];
	QString note = currentProjectFullName;
	QString bookMark = currentProjectFullName;
	note.append(QDir::separator() + fileName + QDir::separator() + QString("Note"));
	bookMark.append(QDir::separator() + fileName + QDir::separator() + QString("BookMark"));
	QString newCTPath = currentProjectFullName;
	newCTPath.append(QDir::separator() + fileName + QDir::separator() + fileName);
	QDir currentFolder(currentProjectFullName);
	if (currentFolder.exists(QString(fileName)))
	{
		if (!QDir().exists(note))
			QDir().mkpath(note);
		if (!QDir().exists(bookMark))
			QDir().mkpath(bookMark);
		if (!QDir().exists(newCTPath))
			QDir().mkpath(newCTPath);
	}
	else
	{
		QDir().mkpath(note);
		QDir().mkpath(bookMark);
		QDir().mkpath(newCTPath);
	}
	cpDir(filePath, newCTPath);
}

bool MainWindow::openProject(QString fileName)
{
  if (!this->closeAll())
	  return false;

  if (fileName.isEmpty())
    fileName = QFileDialog::getOpenFileName(this,tr("Open Project File"), lastUsedDirectory.path(),
                                            "Hyper3D Project (*.xml)");

  if (fileName.isEmpty()) return false;
  fileName = QDir::toNativeSeparators(fileName);

  QFileInfo fi(fileName);
  lastUsedDirectory = fi.absoluteDir();

  if((fi.suffix().toLower()!="xml") )
  {
    QMessageBox::critical(this, tr("Project Error"), "Unknown project file extension.");
    return false;
  }
  this->mInformation->refresh();
  // this change of dir is needed for subsequent textures/materials loading
  QDir::setCurrent(fi.absoluteDir().absolutePath());
  qb->show();
  qDebug()<<"Open project path"<<QDir::currentPath();
  currentProjectSave = QDir::toNativeSeparators(fileName);
  QVector<QString> objectList;
  if (!readXML(fileName, objectList, false))
	  return false;

  unsavedChanges = false;
  setWindowTitle(appName()+appBits()+QString(" ")+currentProjectName);
  updateMenus();
  if(this->VTKA() == 0)  return false;
  qb->reset();
  saveRecentProjectList(fileName);
  updateAllViews();
  return true;
}

VtkWidget* MainWindow::newImage()
{
    //----------------------------------------------------------------------------------------------
    // Ver.1 (VTKwidget with VTKView)
    VtkView *mvcont = new VtkView(mdiArea);
    QMdiSubWindow* subWindow = mdiArea->addSubWindow(mvcont);
	subWindow->installEventFilter(this);
    VtkWidget *gla = new VtkWidget(mvcont);
    mvcont->addView(gla, Qt::Horizontal);// this fuction may destroy the renderer on mdiArea
    connect(mvcont,SIGNAL(updateMainWindowMenus()),this,SLOT(updateAllViews()));

    gla->mvc()->showMaximized(); // make the mdi window a full screen view
    updateAllViews();

    //if(!currentProjectName.isEmpty())
    //    unsavedChanges = true;

    return gla;
}

bool MainWindow::cpDir(const QString &srcPath, const QString &dstPath)
{
     QDir dir(srcPath);
    if (! dir.exists())
        return false;

    foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString dst_path = dstPath + QDir::separator() + d;
        dir.mkpath(dst_path);
        cpDir(srcPath+ QDir::separator() + d, dst_path);
    }

    foreach (QString f, dir.entryList(QDir::Files)) {
        QFile::copy(srcPath + QDir::separator() + f, dstPath + QDir::separator() + f);
    }
	return true;
}

bool MainWindow::rmDir(const QString &dirPath)
{
    bool result = true;
    QDir dir(dirPath);

    if (dir.exists(dirPath)) 
	{
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) 
		{
            if (info.isDir()) 
			{
                result = rmDir(info.absoluteFilePath());
            }
            else 
			{
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) 
			{
                return result;
            }
        }
		if (dirPath != currentProjectFullName)
		{
			result = dir.rmdir(dirPath);
		}
    }
    return result;
}


void MainWindow::newVtkProject(const QString& projName)
{
    if (!this->closeAll())
		return;
    QString projectName, projectPath;
    bool ok = false;
	if (mNewProjectDialog)
	{
		delete mNewProjectDialog;
	}
	
	mNewProjectDialog = new NewProjectDialog(this->lastUsedDirectory.path());
	connect(mNewProjectDialog, SIGNAL(nextPressed(QString, QString, USERMODE, QString, QString, QString, QString, QString, QString)), 
		this, SLOT(createNewVtkProject(QString, QString, USERMODE, QString, QString, QString, QString, QString, QString)));
	mNewProjectDialog->exec();
}

void MainWindow::createNewVtkProject(const QString fullName, const QString name, const USERMODE mode, const QString userName, 
									 const QString object, const QString ct,  const QString keyword, const QString affiliation, const QString description)
{
	this->mInformation->refresh();
	currentProjectFullName = fullName.simplified(); // remove leading/trailing whitespace
	currentProjectFullName = QDir::toNativeSeparators(currentProjectFullName);
	qDebug()<<"New project path " <<currentProjectFullName;
	lastUsedDirectory.setPath(currentProjectFullName);
	
	currentProjectName = name.simplified(); // remove leading/trailing whitespace
	setWindowTitle(appName()+appBits()+QString(" ")+currentProjectName);
	currentProjectSave = currentProjectFullName;
	currentProjectSave.append(QDir::separator() + currentProjectName + QString(".xml"));

	currentProjectKeyword = keyword;
	currentProjectAffiliation = affiliation;
	currentProjectDescription = description;
	mUserName = userName;
	if (QDir().exists(currentProjectFullName))	// if the folder exists, remove all the content since user already choosed to overwrite
	{
		rmDir(currentProjectFullName);
	}
	if (!QDir().exists(currentProjectFullName))
		QDir().mkdir(currentProjectFullName);
	QDir::setCurrent(currentProjectFullName);
	if (!object.isEmpty())
	{
		QString objectName = object.simplified();
		QStringList objectNameList = objectName.split(";");
		for (int i = 0; i < objectNameList.size(); i++)
		{
			if (objectNameList[i] == QString())
				continue;
			QFileInfo checkFile(objectNameList[i]);
			if (!checkFile.exists() || !checkFile.isFile()) 
			{
				qDebug()<<"load object Error";
				QMessageBox::critical(this, tr("Object Opening Error"), "Unable to open " + objectNameList[i] + ".");
			}
			else
			{
				openImages(objectNameList[i]);
			}
		}
		
	}
	if (!ct.isEmpty())
	{
		QString CTFullName = ct.simplified();
		if (!QDir().exists(ct)) 
		{
			qDebug()<<"load ct Error";
			QMessageBox::critical(this, tr("DICOM Opening Error"), "Unable to open " + CTFullName + ".");
			
		}
		else
		{
			openDICOM(ct);
		}
	}

    // DT: change the window titles to reflect new project name.
    QList<QMdiSubWindow*> windows = mdiArea->subWindowList();
    foreach(QMdiSubWindow *w, windows)
    {
        VtkView* mvc = qobject_cast<VtkView *>(w->widget());
        QString file = mvc->currentView()->mFilename;
        QFileInfo fi(file);

        w->setWindowTitle(currentProjectFullName + QString(" : ") + fi.fileName());
    }

	unsavedChanges = false;
	updateXML();
    updateMenus();
}


//MK: DICOM input should be directory.
bool MainWindow::openDICOM(const QString& fileNameStart, bool saveRecent)
{
	QString dirName;
	if (fileNameStart.isEmpty())
	{
		dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), lastUsedDirectory.path(),
													   QFileDialog::ShowDirsOnly
													   | QFileDialog::DontResolveSymlinks);
	}
	else 
	{
		dirName = fileNameStart;
	}

	//  QTime allFileTime;
	//  allFileTime.start();
	if (!dirName.isEmpty())
	{
		QStringList nameElement = dirName.split(QDir::separator());
		qDebug()<<"open CT dire name"<<dirName;
		QString fileName = nameElement[nameElement.size() - 1];
		QString currentWindowPath = currentProjectFullName;
		currentWindowPath.append(QDir::separator() + fileName);
		//save path away so we can use it again
		QString path = dirName;
		#ifdef __APPLE__
		path.truncate(path.lastIndexOf("/")); // Mac
		#else
		path.truncate(path.lastIndexOf("\\")); // PC
		#endif
		if (fileNameStart.isEmpty())
		{
			lastUsedDirectory.setPath(path);
		}

		// DT: create new VtkWidget
		newImage();

		qb->show();
		QTime t;t.start();

		dirName = QDir::toNativeSeparators(dirName);
		QFileInfo fi(dirName);

		bool open = VTKA()->ReadData(dirName);
		//    qDebug() << t.elapsed() << "milliseconds";
		qb->reset();

		if(open && saveRecent)
		{
		  saveRecentFileList(dirName);
		}
		else if(!open)
		{
		  if(mdiArea->currentSubWindow())
		  {
				isClose = true;
				mdiArea->closeActiveSubWindow();
				isClose = false;
		  }
		  if(fi.exists()) 
		  {
			  QMessageBox::critical(this, tr("DICOM Opening Error"), "Unable to open " + fi.fileName() + ".");
		  } else 
		  {
			  QMessageBox::critical(this, tr("DICOM Opening Error"), "Unable to open " + fi.fileName() + ": Directory not found.");
		  }
		  updateAllViews();
		  return false;
		}

		QDir dir = fi.absoluteDir();
		bool ok = dir.cdUp();
		QDir currentFolder(currentProjectFullName);
		if (!ok || dir != currentFolder)
		{
			createCTFolder(dirName);
		}
		QString newFileName = currentWindowPath;
		newFileName.append(QDir::separator() + fileName);
		VTKA()->mFilename = newFileName;
		qDebug()<<"in open CT"<<newFileName;
		VTKA()->mProjectPath = currentWindowPath;

		// update window name
		if(!currentProjectName.isEmpty()) {
		  currentVtkView()->prjName = currentProjectName + QString(" : ") + fi.fileName();
		} else {
		  currentVtkView()->prjName = fi.fileName();
		}
		currentVtkView()->setWindowTitle(currentVtkView()->prjName);
		updateXML();
		updateAllViews();
		return true;
	}
	else return false;
}

void MainWindow::removeObject()
{
	RemoveObjectDialog* dialog = new RemoveObjectDialog();
	QList<QMdiSubWindow*> subWindows = mdiArea->subWindowList();
	foreach(QMdiSubWindow* w, subWindows)
	{
		VtkView* mvc = qobject_cast<VtkView *>(w->widget());
        QStringList list = mvc->currentView()->mFilename.split(QDir::separator());
		QString name = list[list.size()-1];
		dialog->addItem(name);
	}
	dialog->exec();
	QStringList selection = dialog->selectedItems();
	if (selection.size() == 0)
	{
		delete dialog;
		return;
	}
	bool isExport = dialog->checkExport();
	for (int i = 0; i < selection.size(); i++)
	{
		foreach(QMdiSubWindow* w, subWindows)
		{
			VtkView* mvc = qobject_cast<VtkView *>(w->widget());
			QString filePath = mvc->currentView()->mProjectPath;
			QStringList list = mvc->currentView()->mFilename.split(QDir::separator());
			QString name = list[list.size()-1];
			if (name == selection[i])
			{
				QString mPath;
				if (isExport)
				{
					mPath = dialog->getPath();
					if (!mPath.isEmpty())
					{
						mPath = QDir::toNativeSeparators(mPath);
						mPath.append(QDir::separator() + name);
						QDir().mkdir(mPath);
						cpDir(filePath, mPath);
					}
					else 
					{
						delete dialog;
						return;
					}
				}
				rmDir(filePath);
				isClose = true;
				mdiArea->removeSubWindow(w);
				isClose = false;
				break;
			}
		}
	}
	delete dialog;
	updateXML();
	updateAllViews();
}

bool MainWindow::openImages(const QString& fileNameStart, bool saveRecent)
{
	//MK: file filters
	//[3d] *.ply *.obj *.stl *.3ds, *.wrl
	//[2d] *.png, *.jpg, *.tif, *.bmp, *.exr

	//  QStringList filters;
	//  filters.push_back("Images (*.ply *.obj *.wrl *.png *.jpg *.tif *.bmp *.exr *.dcm)");
	//  filters.push_back("*.ply");
	//  filters.push_back("*.obj");
	//  filters.push_back("*.wrl");
	//  filters.push_back("*.exr");
	//  filters.push_back("*.png");
	//  filters.push_back("*.jpg");
	//  filters.push_back("*.tif");
	//  filters.push_back("*.bmp");
	//  filters.push_back("*.dcm");
	////  filters.push_back("*");

	//-------------YY------------------------//
	QStringList filters;
	filters.push_back("Images (*.ply *.obj *.wrl *.png *.jpg *.tif *.bmp *.exr *.dcm *rti *ptm *hsh *mview)");
	filters.push_back("*.ply");
	filters.push_back("*.obj");
	filters.push_back("*.wrl");
	filters.push_back("*.exr");
	filters.push_back("*.png");
	filters.push_back("*.jpg");
	filters.push_back("*.tif");
	filters.push_back("*.bmp");
	filters.push_back("*.dcm");
	filters.push_back("*.rti");
	filters.push_back("*.ptm");
	filters.push_back("*.hsh");
	filters.push_back("*.mview");
	//-------------YY------------------------//

	//MK: open file open dialog
	QString fileName;
	if (fileNameStart.isEmpty())
	{
		fileName = QFileDialog::getOpenFileName(this,tr("Open a file"), lastUsedDirectory.path(), filters.join(";;")); // entering the last path
	}
	else 
	{
		fileName = fileNameStart;
	}

	if (!fileName.isEmpty())
	{
		fileName = QDir::toNativeSeparators(fileName);
		QFileInfo fi(fileName);
		if (!fi.isFile())
		{
			QMessageBox::critical(this, tr("Opening Object Error"), "Unable to open " + fi.fileName() + ": It is not a File!");
			return false;
		}
		QStringList nameElement = fileName.split(QDir::separator());
		QString fileNameElement = nameElement[nameElement.size() - 1];
		QString currentWindowPath = currentProjectFullName;
		currentWindowPath.append(QDir::separator() + fileNameElement);
		//save path away so we can use it again
		QString path = fileName;
		qDebug()<<"In open image!!!!!";
		#ifdef __APPLE__
		path.truncate(path.lastIndexOf("/")); // Mac
		#else
		path.truncate(path.lastIndexOf("\\")); // PC
		#endif
		if (fileNameStart.isEmpty())
		{
			lastUsedDirectory.setPath(path);
		}

		newImage();
		qDebug()<<fileName;

		bool open = VTKA()->ReadData(fileName);
		
		//this->mInformation->startAnnotation();
		if(open && saveRecent)
		{
			saveRecentFileList(fileName);
		}
		else if(!open)
		{
			if(mdiArea->currentSubWindow())
			{
				isClose = true;
				mdiArea->closeActiveSubWindow();
				isClose = false;
			}

			if(fi.exists()) 
			{
				QMessageBox::critical(this, tr("Opening Error"), "Unable to open " + fi.fileName() + ". ");
			} 
			else 
			{
				QMessageBox::critical(this, tr("Opening Error"), "Unable to open " + fi.fileName() + ": File not found. ");
			}

			updateAllViews();
			return false;
		}

		QDir dir = fi.absoluteDir();
		bool ok = dir.cdUp();
		QDir currentFolder(currentProjectFullName);
		if (!ok || dir != currentFolder)
		{
			qDebug()<<"create Object folder"<<fileName<<currentProjectFullName;
			createObjectFolder(fileName);
		}
		QString newFileName = currentWindowPath;
		newFileName.append(QDir::separator() + fileNameElement);
		VTKA()->mFilename = newFileName;
		VTKA()->mProjectPath = currentWindowPath;
		this->mInformation->initAnnotation(currentWindowPath);

		// update window name
		if(!currentProjectName.isEmpty()) {
		  currentVtkView()->prjName = currentProjectName + QString(" : ") + fi.fileName();
		} else {
		  currentVtkView()->prjName = fi.fileName();
		}
		currentVtkView()->setWindowTitle(currentVtkView()->prjName);
		updateXML();

		updateAllViews();
		return true;
	}
	else return false;
}



void MainWindow::save()
{
#ifdef SUPPORT_SAVE
    if (activeGLArea() && activeGLArea()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
#endif
}

void MainWindow::saveAs()
{
#ifdef SUPPORT_SAVE
    if (activeGLArea() && activeGLArea()->saveAs())
        statusBar()->showMessage(tr("File saved"), 2000);
#endif
}

void MainWindow::help()
{
  // add help dialog later
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Hyper3D") + appVer(),
                      tr("<h2>Hyper3D</h2>"
                         "<p>Yale Computer Graphics Laboratory</p>"
                         "<p>Principal Investigator: Prof. Holly Rushmeier"
                         "<p>Authors: Min H. Kim, David Tidmarsh"
                         "<p>Hyper3D is an application that visualizes various types of image data,"
                         " e.g., hyperspectral images and 3D models, as well as CT medical images in 3D, etc."
                         "<p>This project was funded by a 2011-2012 grant from the Seaver Institute."
                         "<p>We are grateful to John ffrench and Irma Passeri of the Yale University Art Gallery"
                         " for their help with the project."
                         "<p>We also thank the Getty Research Institute and the Canadian Heritage Information Network"
                         " for allowing us to adapt the Categories for the Description of Works of Art and the"
                         " CHIN Natural Sciences Data Dictionary, respectively, for use in this software's"
                         " metadata system:"
                         "<p>Categories for the Description of Works of Art (CDWA). [online]. Murtha Baca and"
                         " Patricia Harpring, eds. Los Angeles: J. Paul Getty Trust and the College Art Association,"
                         " 2000. Revised 2009."
                         " http://www.getty.edu/research/publications/electronic_publications/cdwa/index.html"
                         "<p>http://www.pro.rcip-chin.gc.ca/bd-dl/ddrcip_sn-chindd_ns/description-about-eng.jsp"));
}

void MainWindow::viewOptions()
{
   QMessageBox::about(this, tr("view test"),
            tr("<b>Hyper3D</b> is three-dimensional refletance transformation imaging"));
}

void MainWindow::updateRecentFileActions()
{
  //bool activeDoc = (bool) !mdiArea->subWindowList().empty() && mdiArea->currentSubWindow();

  QSettings settings("Yale Graphics Lab", "Hyper3D");
  QStringList files = settings.value("recentFileList").toStringList();

  int numRecentFiles = qMin(files.size(), (int)MAXRECENTFILES);

  for (int i = 0; i < numRecentFiles; ++i)
  {
    if( QFileInfo(files[i]).exists() == true)
    {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
    //    qDebug() << "active doc:" << activeDoc << " " << "recent file list:" << text;
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);

        //MK: conditional opening (allowed after project)
    //    recentFileActs[i]->setEnabled(activeDoc);
        //MK: force to allow it for convinience
        recentFileActs[i]->setEnabled(true);
    } else {
        recentFileActs[i]->setText("Not available");
        recentFileActs[i]->setEnabled(false);
    }
  }

  for (int j = numRecentFiles; j < MAXRECENTFILES; ++j)
  {
//    qDebug() << "recent file list:" << j << " " << recentFileActs[j]->text();
    recentFileActs[j]->setVisible(false);
  }
}

void MainWindow::updateRecentProjActions()
{
  QSettings settings("Yale Graphics Lab", "Hyper3D");
  QStringList projs = settings.value("recentProjList").toStringList();

  int numRecentProjs = qMin(projs.size(), (int)MAXRECENTFILES);
  for (int i = 0; i < numRecentProjs; ++i)
  {
    QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(projs[i]).fileName());
    recentProjActs[i]->setText(text);
    recentProjActs[i]->setData(projs[i]);
    recentProjActs[i]->setEnabled(true);
  }
  for (int j = numRecentProjs; j < MAXRECENTFILES; ++j)
    recentProjActs[j]->setVisible(false);
}

// this function update the app settings with the current recent file list
// and update the loaded mesh counter
void MainWindow::saveRecentFileList(const QString &fileName)
{
  QFileInfo fi(fileName);
  // DT: This should fix bug where filenames were duplicated because of slash/backslash differences.
  QString fn = QDir::fromNativeSeparators(fi.absoluteFilePath());

  QSettings settings("Yale Graphics Lab", "Hyper3D");
  QStringList files = settings.value("recentFileList").toStringList();
  files.removeAll(fn);
  files.prepend(fn);
  while (files.size() > MAXRECENTFILES)
    files.removeLast();

  //avoid the slash/back-slash path ambiguity
  for(int ii = 0;ii < files.size();++ii)
    files[ii] = QDir::fromNativeSeparators(files[ii]);
  settings.setValue("recentFileList", files);

  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
    if (mainWin) mainWin->updateRecentFileActions();
  }
}

void MainWindow::saveRecentProjectList(const QString &projName)
{
  QString pn = QDir::fromNativeSeparators(projName);

  QSettings settings("Yale Graphics Lab", "Hyper3D");
  QStringList files = settings.value("recentProjList").toStringList();
  files.removeAll(pn);
  files.prepend(pn);
  while (files.size() > MAXRECENTFILES)
    files.removeLast();

  for(int ii = 0;ii < files.size();++ii)
    files[ii] = QDir::fromNativeSeparators(files[ii]);

  settings.setValue("recentProjList", files);

  foreach (QWidget *widget, QApplication::topLevelWidgets())
  {
    MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
    if (mainWin)
      mainWin->updateRecentProjActions();
  }
}

void MainWindow::updateMenus()
{
  bool activeDoc = false;
  bool isHidden = false;
  bool projectOpen = !currentProjectName.isEmpty();
  if (mdiArea)
  {
	foreach(QMdiSubWindow *w, mdiArea->subWindowList())
	{
	  if (!w->isHidden())
	  {
		  activeDoc = true;
		  break;
	  }
	}
	foreach(QMdiSubWindow *w, mdiArea->subWindowList())
	{
	  if (w->isHidden())
	  {
		  isHidden = true;
		  break;
	  }
	}
  }
  else
    return;
  openProjectAct->setEnabled(true);
  openObjectAct->setEnabled(projectOpen);
  openDICOMAct->setEnabled(projectOpen);
  importProjectAct->setEnabled(projectOpen);
  removeObjectAct->setEnabled(projectOpen);
  
  saveProjectAsAct->setEnabled(projectOpen && activeDoc);
  saveProjectAct->setEnabled(projectOpen && activeDoc);
  closeProjectAct->setEnabled(projectOpen);
  updateRecentProjActions();
  updateRecentFileActions();
  recentFileMenu->setEnabled(projectOpen);

//  editMenu->setEnabled(activeDoc && !editMenu->actions().isEmpty());
  renderMenu->setEnabled(activeDoc);
  viewMenu->setEnabled(activeDoc);
  toolsMenu->setEnabled(activeDoc);
  windowsMenu->setEnabled(projectOpen);
  closeWindowAct->setEnabled(activeDoc);
  closeAllAct->setEnabled(activeDoc);
  openWindowAct->setEnabled(isHidden);
  viewFromMenu->setEnabled(activeDoc);
  renderModeInterpolationAct->setEnabled(activeDoc);

  renderToolBar->setEnabled(activeDoc);
  viewToolBar->setEnabled(activeDoc);
  windowToolBar->setEnabled(activeDoc);
  infoToolBar->setEnabled(activeDoc);
  measureToolBar->setEnabled(activeDoc);
  annotationToolBar->setEnabled(activeDoc);

  renderModeInfoAct->setEnabled(activeDoc);
  showPolyIndicateAct->setEnabled(activeDoc);
  measureDistanceAct->setEnabled(activeDoc);
  removeDistanceAct->setEnabled(activeDoc);

  writeAnnotationAct->setEnabled(activeDoc);
  if (writeAnnotationAct->isChecked())
  {
	  this->mInformation->startAnnotation();
  }
  else
  {
	  this->mInformation->finishAnnotation();
  }
  annotationModeMenu->setEnabled(activeDoc);
  removeAnnotationAct->setEnabled(activeDoc);

  screenshotAct->setEnabled(activeDoc);
  bookmarkAct->setEnabled(activeDoc);
  fileInfoAct->setEnabled(activeDoc);
  projectInfoAct->setEnabled(projectOpen);

  zoomInAct->setEnabled(activeDoc);
  zoomOutAct->setEnabled(activeDoc);
  zoomResetAct->setEnabled(activeDoc);

  showToolbarStandardAct->setChecked(mainToolBar->isVisible());
  showToolbarRenderAct->setChecked(renderToolBar->isVisible());
  showToolbarViewAct->setChecked(viewToolBar->isVisible());
  showToolbarInfoAct->setChecked(infoToolBar->isVisible());
  this->mSearch->refreshSearchTab(activeDoc);
  this->mSearchAll->refreshSearchTab(activeDoc);

  if(activeDoc && VTKA()){

    const RenderMode3D &rm=VTKA()->getRenderMode3D();
    switch (rm) {
      case POINTS3D:
        renderModePointsAct->setChecked(true);
        break;
      case WIREFRAME3D:
        renderModeWireAct->setChecked(true);
        break;
      case SURFACE3D:
        renderModeFlatAct->setChecked(true);
        break;
    default: break;
    }

    const WidgetMode &wm = VTKA()->getWidgetMode();
    emit currentWidgetModeChanged(wm);

    renderModeInterpolationAct->setChecked( VTKA()->getIsInterpolationOn() );
    renderModeInfoAct->setChecked( VTKA()->getDisplayInfoOn() );
    showPolyIndicateAct->setChecked( VTKA()->getDisplayPolyIndicateOn() );

    renderModeTextureAct->setIcon( VTKA()->getIsTextureOn() ? QIcon(":/images/textureson.png") : QIcon(":/images/texturesoff.png") );
    renderModeTextureAct->setChecked( VTKA()->getIsTextureOn() );
    renderModeInterpolationAct->setIcon( VTKA()->getIsInterpolationOn() ? QIcon(":/images/interpolation.png") : QIcon(":/images/interpolation.png") );
    setDirectionalLightAct->setIcon( VTKA()->getIsDirectionalLight() ? QIcon(":/images/ctrllighton.png") : QIcon(":/images/lighton.png") );

    measureDistanceAct->setIcon(VTKA()->getUseRubberband() ? QIcon(":/images/ruler_on.png") : QIcon(":/images/ruler_off.png") );
    measureDistanceAct->setChecked(VTKA()->getUseRubberband() );

//    removeDistanceAct->setIcon( VTKA()->getIsRubberbandVisible() ? QIcon(":/images/remove_ruler_on.png") : QIcon(":/images/remove_ruler_off.png") );
//    removeDistanceAct->setChecked( VTKA()->getIsRubberbandVisible() );

    writeAnnotationAct->setIcon(VTKA()->getUserAnnotationOn() ? QIcon(":/images/annotation_on.png") : QIcon(":/images/annotation_off.png") );
    writeAnnotationAct->setChecked(VTKA()->getUserAnnotationOn() );
	if (!VTKA()->getUserAnnotationOn())
	{
		this->mInformation->closeObjectNotes();
	}
	
    QString tfn = VTKA()->getmRgbTextureFilename();

    switch (wm) {
    case EMPTYWIDGET:
        renderMenu->setEnabled(false);
        viewFromMenu->setEnabled(false);
		annotationModeMenu->setEnabled(false);
        renderToolBar->setEnabled(false);
        measureToolBar->setEnabled(false);
        annotationToolBar->setEnabled(false);
        toolsMenu->setEnabled(false);
        viewToolBar->setEnabled(false);
        break;
    case IMAGE2D:
        renderMenu->setEnabled(false);
        viewFromMenu->setEnabled(false);
        renderToolBar->setEnabled(false);
        measureToolBar->setEnabled(false);
        annotationToolBar->setEnabled(true);
        toolsMenu->setEnabled(true);
		annotationModeMenu->setEnabled(true);
		frustumNote->setEnabled(false);
        showPolyIndicateAct->setEnabled(false);
        measureDistanceAct->setEnabled(false);
        removeDistanceAct->setEnabled(false);
        viewToolBar->setEnabled(false);
        break;
    case MODEL3D:
        renderMenu->setEnabled(true);
        viewFromMenu->setEnabled(true);
		annotationModeMenu->setEnabled(true);
		frustumNote->setEnabled(true);
        renderToolBar->setEnabled(true);
        measureToolBar->setEnabled(true);
        annotationToolBar->setEnabled(true);
        toolsMenu->setEnabled(true);
        viewToolBar->setEnabled(true);


        // DT: preventing crash when these buttons are clicked w/ no texture
        if(tfn.isEmpty()) {
            renderModeInterpolationAct->setEnabled(false);
            renderModeTextureAct->setEnabled(false);
        } else {
            renderModeInterpolationAct->setEnabled(true);
            renderModeTextureAct->setEnabled(true);
        }
        break;
    case CTSTACK:
//        qDebug() << "this is the ct stack mode";
        renderMenu->setEnabled(false);
        viewFromMenu->setEnabled(false);
        renderToolBar->setEnabled(false);
        measureToolBar->setEnabled(false);
        annotationToolBar->setEnabled(true);
		annotationModeMenu->setEnabled(true);
        toolsMenu->setEnabled(true);
        measureDistanceAct->setEnabled(false);
        removeDistanceAct->setEnabled(false);
        viewToolBar->setEnabled(false);
        break;
    case CTVOLUME:
//        qDebug() << "this is the ct volume mode";
        renderMenu->setEnabled(false);
        viewFromMenu->setEnabled(false);
        renderToolBar->setEnabled(false);
        measureToolBar->setEnabled(true); // bug
        annotationToolBar->setEnabled(true);
		annotationModeMenu->setEnabled(true);
        toolsMenu->setEnabled(true);
        viewToolBar->setEnabled(false);

        // DT: preventing crash when these buttons are clicked w/ no texture
        if(tfn.isEmpty()) {
            renderModeInterpolationAct->setEnabled(false);
            renderModeTextureAct->setEnabled(false);
        } else {
            renderModeInterpolationAct->setEnabled(true);
            renderModeTextureAct->setEnabled(true);
        }
        break;
    case RTI2D:
        renderMenu->setEnabled(false);
        viewFromMenu->setEnabled(false);
        renderToolBar->setEnabled(false);
        measureToolBar->setEnabled(false);
        annotationToolBar->setEnabled(true);
        toolsMenu->setEnabled(true);
		annotationModeMenu->setEnabled(true);
        measureDistanceAct->setEnabled(false);
        removeDistanceAct->setEnabled(false);
        viewToolBar->setEnabled(false);
        break;
    default: break;
    }
  }// end of if
}


void MainWindow::updateWindowMenu()
{
  windowsMenu->clear();
  windowsMenu->addAction(closeAllAct);
  windowsMenu->addAction(closeWindowAct);
  windowsMenu->addAction(openWindowAct);
  windowsMenu->addSeparator();
  windowsMenu->addAction(windowsTileAct);
  windowsMenu->addAction(windowsMaximizeAct);
  windowsMenu->addAction(windowsNextAct);

  int activeWindows = 0;
  if (mdiArea)
  {
	foreach(QMdiSubWindow *w, mdiArea->subWindowList())
	{
	  if (!w->isHidden())
	  {
		activeWindows++;
	  }
	}
  }

  windowsTileAct->setEnabled(activeWindows > 1);

#ifdef SUPPORT_WINDOWS_CASCADE
  windowsMenu->addAction(windowsCascadeAct);
  windowsCascadeAct->setEnabled(mdiArea-> subWindowList().size()>1);
#endif

  windowsMaximizeAct->setEnabled(activeWindows > 0);

  windowsNextAct->setEnabled( activeWindows > 1);

#ifdef SUPPORT_SPINANIMATION
  windowsMenu->addSeparator();
  windowsMenu->addAction( viewSpinAct );
#endif

  if((mdiArea-> subWindowList().size()>0)){
#ifdef SUPPORT_SPLITWINDOW
    // Split/Unsplit SUBmenu
    splitModeMenu = windowsMenu->addMenu(tr("&Split current view"));

    splitModeMenu->addAction(setSplitHAct);
    splitModeMenu->addAction(setSplitVAct);

    windowsMenu->addAction(setUnsplitAct);
#endif

#ifdef SUPPORT_SPLITWINDOW
    //Enabling the actions
    VtkView *mvc = currentVtkView();
    if(mvc)
    {
      setUnsplitAct->setEnabled(mvc->viewerCounter()>1);
      VtkWidget* current = mvc->currentView();
      if(current)
      {
        setSplitHAct->setEnabled(current->size().height()/2 > current->minimumSizeHint().height());
        setSplitVAct->setEnabled(current->size().width()/2 > current->minimumSizeHint().width());

        windowsMenu->addSeparator();

      }
    }
#endif
  }

  QList<QMdiSubWindow*> windows = mdiArea->subWindowList();

  if(windows.size() > 0)
      windowsMenu->addSeparator();

  int i=0;
  foreach(QWidget *w,windows)
  {
	if (w->isHidden())
		continue;
    QString text = tr("&%1. %2").arg(i+1).arg(QFileInfo(w->windowTitle()).fileName());
    QAction *action  = windowsMenu->addAction(text);
    action->setCheckable(true);
    action->setChecked(w == mdiArea->currentSubWindow());
    // the signal to activate the selected window
    connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
    windowMapper->setMapping(action, w);
    ++i;
  }
}

void MainWindow::openRecentFile()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action)
  {
    QString filePath = action->data().toString();
    QFileInfo fi(filePath);
    if( QFileInfo(filePath).exists() == false)
      return;
//    qDebug() << "recent file: " << fileName;
    if (fi.isDir()){
      openDICOM(filePath);
    }else{
      openImages(filePath);
    }
  }
}

void MainWindow::openRecentProj()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action)	openProject(action->data().toString());
}

void MainWindow::setSplit(QAction *qa)
{
  VtkView *mvc = currentVtkView();
  if(mvc)
  {
    VtkWidget *glwClone = new VtkWidget(mvc);

    if(qa->text() == tr("&Horizontally"))
      mvc->addView(glwClone, Qt::Vertical);
    else if(qa->text() == tr("&Vertically"))
      mvc->addView(glwClone, Qt::Horizontal);

    updateAllViews();
    glwClone->update();
  }

}

void MainWindow::setUnsplit()
{
  VtkView *mvc = currentVtkView();
  if(mvc)
  {
    assert(mvc->viewerCounter() >1);

    mvc->removeView(mvc->currentView()->getId());

    updateAllViews();
  }
}

//set the split/unsplit menu that appears right clicking on a splitter's handle
void MainWindow::setHandleMenu(QPoint point, Qt::Orientation orientation, QSplitter *origin){
    VtkView *mvc =  currentVtkView();
    int epsilon =10;
    splitMenu->clear();
    unSplitMenu->clear();
    //the viewer to split/unsplit is chosen through picking

    //Vertical handle allows to split horizontally
    if(orientation == Qt::Vertical)
    {
      splitUpAct->setData(point);
      splitDownAct->setData(point);

      //check if the viewer on the top is splittable according to its size
      int pickingId = mvc->getViewerByPicking(QPoint(point.x(), point.y()-epsilon));
      if(pickingId>=0)
        splitUpAct->setEnabled(mvc->getViewer(pickingId)->size().width()/2 > mvc->getViewer(pickingId)->minimumSizeHint().width());

      //the viewer on top can be closed only if the splitter over the handle that orginated the event has one child
      bool unSplittabilityUp = true;
      VtkSplitter * upSplitter = qobject_cast<VtkSplitter *>(origin->widget(0));
      if(upSplitter)
        unSplittabilityUp = !(upSplitter->count()>1);
      unsplitUpAct->setEnabled(unSplittabilityUp);

      //check if the viewer below is splittable according to its size
      pickingId = mvc->getViewerByPicking(QPoint(point.x(), point.y()+epsilon));
      if(pickingId>=0)
        splitDownAct->setEnabled(mvc->getViewer(pickingId)->size().width()/2 > mvc->getViewer(pickingId)->minimumSizeHint().width());

      //the viewer below can be closed only if the splitter ounder the handle that orginated the event has one child
      bool unSplittabilityDown = true;
      VtkSplitter * downSplitter = qobject_cast<VtkSplitter *>(origin->widget(1));
      if(downSplitter)
        unSplittabilityDown = !(downSplitter->count()>1);
      unsplitDownAct->setEnabled(unSplittabilityDown);

      splitMenu->addAction(splitUpAct);
      splitMenu->addAction(splitDownAct);

      unsplitUpAct->setData(point);
      unsplitDownAct->setData(point);

      unSplitMenu->addAction(unsplitUpAct);
      unSplitMenu->addAction(unsplitDownAct);
    }
    //Horizontal handle allows to split vertically
    else if (orientation == Qt::Horizontal)
    {
      splitRightAct->setData(point);
      splitLeftAct->setData(point);

      //check if the viewer on the right is splittable according to its size
      int pickingId =mvc->getViewerByPicking(QPoint(point.x()+epsilon, point.y()));
      if(pickingId>=0)
        splitRightAct->setEnabled(mvc->getViewer(pickingId)->size().height()/2 > mvc->getViewer(pickingId)->minimumSizeHint().height());

      //the viewer on the rigth can be closed only if the splitter on the right the handle that orginated the event has one child
      bool unSplittabilityRight = true;
      VtkSplitter * rightSplitter = qobject_cast<VtkSplitter *>(origin->widget(1));
      if(rightSplitter)
        unSplittabilityRight = !(rightSplitter->count()>1);
      unsplitRightAct->setEnabled(unSplittabilityRight);

      //check if the viewer on the left is splittable according to its size
      pickingId =mvc->getViewerByPicking(QPoint(point.x()-epsilon, point.y()));
      if(pickingId>=0)
        splitLeftAct->setEnabled(mvc->getViewer(pickingId)->size().height()/2 > mvc->getViewer(pickingId)->minimumSizeHint().height());

      //the viewer on the left can be closed only if the splitter on the left of the handle that orginated the event has one child
      bool unSplittabilityLeft = true;
      VtkSplitter * leftSplitter = qobject_cast<VtkSplitter *>(origin->widget(0));
      if(leftSplitter)
        unSplittabilityLeft = !(leftSplitter->count()>1);
      unsplitLeftAct->setEnabled(unSplittabilityLeft);

      splitMenu->addAction(splitRightAct);
      splitMenu->addAction(splitLeftAct);

      unsplitRightAct->setData(point);
      unsplitLeftAct->setData(point);

      unSplitMenu->addAction(unsplitRightAct);
      unSplitMenu->addAction(unsplitLeftAct);
    }

    handleMenu->popup(point);
}

void MainWindow::getProjectInfo(QString& projectName, QString& location, QString& keyword, QString& affiliation, QString& userName, QString& description)
{
	projectName = currentProjectName;
	location = currentProjectFullName;
	keyword = currentProjectKeyword;
	affiliation = currentProjectAffiliation;
	userName = mUserName;
	description = currentProjectDescription;

}

void MainWindow::splitFromHandle(QAction *qa )
{
  VtkView *mvc = currentVtkView();
  QPoint point = qa->data().toPoint();
  int epsilon =10;

  if(qa->text() == tr("&Right"))
    point.setX(point.x()+ epsilon);
  else if(qa->text() == tr("&Left"))
    point.setX(point.x()- epsilon);
  else if(qa->text() == tr("&Up"))
    point.setY(point.y()- epsilon);
  else if(qa->text() == tr("&Down"))
    point.setY(point.y()+ epsilon);

  int newCurrent = mvc->getViewerByPicking(point);
  mvc->updateCurrent(newCurrent);

  QAction *act;
  if(qa->text() == tr("&Right")||qa->text() == tr("&Left"))
    act= new QAction(tr("&Horizontally"), this);
  else if(qa->text() == tr("&Up")||qa->text() == tr("&Down"))
    act= new QAction(tr("&Vertically"), this);

  setSplit(act);
}

void MainWindow::unsplitFromHandle(QAction * qa)
{
  VtkView *mvc = currentVtkView();

  QPoint point = qa->data().toPoint();
  int epsilon =10;

  if(qa->text() == tr("&Right"))
    point.setX(point.x()+ epsilon);
  else if(qa->text() == tr("&Left"))
    point.setX(point.x()- epsilon);
  else if(qa->text() == tr("&Up"))
    point.setY(point.y()- epsilon);
  else if(qa->text() == tr("&Down"))
    point.setY(point.y()+ epsilon);

  int newCurrent = mvc->getViewerByPicking(point);
  mvc->updateCurrent(newCurrent);

  setUnsplit();
}

void MainWindow::setDirectionalLight()
{
  QAction *a = qobject_cast<QAction* >(sender());
  if (this->VTKA() ) {
    this->VTKA()->setIsDirectionalLight( a->isChecked() ? true : false );
    this->mLightControl->setDirectionalLight(VTKA()->getIsDirectionalLight());
  }
  updateMenus();
}

void MainWindow::setCameraModeTrackball()
{
    VTKA()->setCameraMode(TRACKBALLMODE);
}

void MainWindow::setCameraModeSurfaceWalker()
{
    VTKA()->setCameraMode(SURFACEWALKERMODE);
}

void MainWindow::toggleImageProvenanceFeature()
{
    VTKA()->toggleImageProvenanceFeature();
}

bool MainWindow::eventFilter(QObject * obj, QEvent * e)
{
    switch (e->type())
    {
        case QEvent::Close:
        {
			if (!isClose)
			{
				closeWindow();
				e->ignore();
				return true;
			}
            break;
        }
        default:
            qt_noop();
    }
    return QObject::eventFilter(obj, e);
}

void MainWindow::createActions()
{
	//===================================================================================================
	//MK: File Actions
	newVtkProjectAct = new QAction(QIcon(":/images/new_project.png"),tr("New Project..."), this);
	newVtkProjectAct->setShortcutContext(Qt::ApplicationShortcut);
	newVtkProjectAct->setShortcut(Qt::CTRL+Qt::Key_N);
	newVtkProjectAct->setStatusTip(tr("Create a new project"));
	connect(newVtkProjectAct, SIGNAL(triggered()), this, SLOT(newVtkProject()));

	openProjectAct = new QAction(QIcon(":/images/open_project.png"),tr("&Open Project..."), this);
	openProjectAct->setShortcutContext(Qt::ApplicationShortcut);
	openProjectAct->setShortcut(Qt::CTRL+Qt::Key_O);
	openProjectAct->setStatusTip(tr("Open a project file"));
	connect(openProjectAct, SIGNAL(triggered()), this, SLOT(openProject()));

	saveProjectAct = new QAction(QIcon(":/images/save_project.png"),tr("&Save Project"), this);
	saveProjectAct->setShortcutContext(Qt::ApplicationShortcut);
	saveProjectAct->setShortcut(Qt::CTRL+Qt::Key_S);
	saveProjectAct->setStatusTip(tr("Save the project"));
	connect(saveProjectAct, SIGNAL(triggered()), this, SLOT(saveProject()));

	saveProjectAsAct = new QAction(QIcon(":/images/save_project.png"),tr("Save Project As..."), this);
	saveProjectAsAct->setStatusTip(tr("Save the project as a new file"));
	connect(saveProjectAsAct, SIGNAL(triggered()), this, SLOT(saveProjectAs()));

	closeProjectAct = new QAction(tr("Close Project"), this);
	closeProjectAct->setShortcutContext(Qt::ApplicationShortcut);
	closeProjectAct->setStatusTip(tr("Close the opened project"));
	connect(closeProjectAct, SIGNAL(triggered()),this, SLOT(closeAll()));

	importProjectAct = new QAction(tr("Import Project..."), this);
	importProjectAct->setShortcutContext(Qt::ApplicationShortcut);
	importProjectAct->setShortcut(Qt::CTRL+Qt::Key_P);
	importProjectAct->setStatusTip(tr("Import an exisitng project into the current one and merge together"));
	connect(importProjectAct, SIGNAL(triggered()), this, SLOT(importProject()));

    setCustomizeAct	  = new QAction(tr("P&references..."),this);
    setCustomizeAct->setShortcutContext(Qt::ApplicationShortcut);
    setCustomizeAct->setShortcut(Qt::CTRL+Qt::Key_R);
    setCustomizeAct->setStatusTip(tr("Customize user preferences"));
    connect(setCustomizeAct, SIGNAL(triggered()), this, SLOT(setCustomize()));

    openObjectAct = new QAction(QIcon(":/images/open2-3d.png"),tr("&Import Object..."), this);
    openObjectAct->setShortcutContext(Qt::ApplicationShortcut);
    openObjectAct->setShortcut(Qt::CTRL+Qt::Key_I);
    openObjectAct->setStatusTip(tr("Open an existing 2D or 3D object"));
    connect(openObjectAct, SIGNAL(triggered()), this, SLOT(openImages()));

    openDICOMAct = new QAction(QIcon(":/images/openct.png"),tr("Import C&T Directory..."), this);
    openDICOMAct->setShortcutContext(Qt::ApplicationShortcut);
    openDICOMAct->setShortcut(Qt::CTRL+Qt::Key_T);
    openDICOMAct->setStatusTip(tr("Open an existing CT directory"));
    connect(openDICOMAct, SIGNAL(triggered()), this, SLOT(openDICOM()));

	removeObjectAct = new QAction(tr("&Remove Object"), this);
    removeObjectAct->setStatusTip(tr("Remove existing 2D or 3D object from Project"));
    connect(removeObjectAct, SIGNAL(triggered()), this, SLOT(removeObject()));

	for (int i = 0; i < MAXRECENTFILES; ++i)
	{
		recentProjActs[i] = new QAction(this);
		recentProjActs[i]->setVisible(true);
		recentProjActs[i]->setEnabled(true);
		recentProjActs[i]->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F1+i));
		connect(recentProjActs[i],SIGNAL(triggered()),this,SLOT(openRecentProj()));

		recentFileActs[i] = new QAction(this);
		recentFileActs[i]->setVisible(true);
		recentFileActs[i]->setEnabled(false);
		recentFileActs[i]->setShortcut(QKeySequence(Qt::ALT + Qt::Key_1+i));
		connect(recentFileActs[i], SIGNAL(triggered()),this, SLOT(openRecentFile()));
	}

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    onlineHelpAct = new QAction(tr("Developers' &Website"), this);
    connect(onlineHelpAct, SIGNAL(triggered()), this, SLOT(helpOnline()));

    zoomInAct = new QAction(tr("Zoom In"), this);
    zoomInAct->setShortcutContext(Qt::ApplicationShortcut);
    zoomInAct->setShortcut(Qt::CTRL+Qt::Key_Equal); // Key_Plus
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom Out"), this);
    zoomOutAct->setShortcutContext(Qt::ApplicationShortcut);
    zoomOutAct->setShortcut(Qt::CTRL+Qt::Key_Minus);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    zoomResetAct = new QAction(tr("Reset View"), this);
    zoomResetAct->setShortcutContext(Qt::ApplicationShortcut);
    zoomResetAct->setShortcut(Qt::CTRL+Qt::Key_0);
    connect(zoomResetAct, SIGNAL(triggered()), this, SLOT(zoomReset()));


    //===================================================================================================
    //MK: Window Actions

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show Hyper3D About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    helpAct = new QAction(tr("&Help"), this);
    helpAct->setStatusTip(tr("Show Help document"));
    connect(helpAct, SIGNAL(triggered()), this, SLOT(help()));

    //================================================================================
    //Render Actions for Toolbar and Menu
    renderModeGroupAct = new QActionGroup(this);

    renderModePointsAct	  = new QAction(QIcon(":/images/points.png"),tr("&Points"), renderModeGroupAct);//
    renderModePointsAct->setCheckable(true);
    connect(renderModePointsAct, SIGNAL(triggered()), this, SLOT(renderPoints()));

    renderModeWireAct		  = new QAction(QIcon(":/images/wire.png"),tr("&Wireframe"), renderModeGroupAct);//
    renderModeWireAct->setCheckable(true);
    connect(renderModeWireAct, SIGNAL(triggered()), this, SLOT(renderWireframe()));

    renderModeFlatAct		  = new QAction(QIcon(":/images/flat.png"),tr("&Surface"), renderModeGroupAct);//
    renderModeFlatAct->setCheckable(true);
    connect(renderModeFlatAct, SIGNAL(triggered()), this, SLOT(renderSurface()));

    renderModeTextureAct  = new QAction(QIcon(":/images/textureson.png"),tr("&Texture"),this);//
    renderModeTextureAct->setCheckable(true);
    renderModeTextureAct->setChecked(true);
    connect(renderModeTextureAct, SIGNAL(triggered()), this, SLOT(renderTexture()));

    renderModeInterpolationAct  = new QAction(QIcon(":/images/interpolation.png"),tr("&Interpolation"),this);//
    renderModeInterpolationAct->setCheckable(true);
    renderModeInterpolationAct->setChecked(false);
    connect(renderModeInterpolationAct, SIGNAL(triggered()), this, SLOT(renderInterpolation()));

    renderModeInfoAct  = new QAction(QIcon(":/images/info.png"),tr("&Information"),this);//
    renderModeInfoAct->setCheckable(true);
    renderModeInfoAct->setChecked(true);
    connect(renderModeInfoAct, SIGNAL(triggered()), this, SLOT(renderInformation()));

    showPolyIndicateAct= new QAction (QIcon(":/images/polygonarrow.png"), tr("Show Polygon &Indicator"), this);
    showPolyIndicateAct->setCheckable(true);
    showPolyIndicateAct->setChecked(false);
    connect(showPolyIndicateAct, SIGNAL(triggered()), this, SLOT(renderPolyIndicate()));

    setDirectionalLightAct	  = new QAction(QIcon(":/images/ctrllighton.png"),tr("&Directional Light On/Off"),this);//
    setDirectionalLightAct->setCheckable(true);
    setDirectionalLightAct->setChecked(true);
    connect(setDirectionalLightAct, SIGNAL(triggered()), this, SLOT(setDirectionalLight()));

    setTrackballCameraAct = new QAction(tr("Trackball"),this);
    connect(setTrackballCameraAct, SIGNAL(triggered()), this, SLOT(setCameraModeTrackball()));
    setSurfaceWalkerCameraAct = new QAction(tr("Surface Walker"),this);
    connect(setSurfaceWalkerCameraAct, SIGNAL(triggered()), this, SLOT(setCameraModeSurfaceWalker()));
    showImageProvenanceAct = new QAction(tr("Toggle Provenance"),this);
    connect(showImageProvenanceAct, SIGNAL(triggered()), this, SLOT(toggleImageProvenanceFeature()));

    //================================================================================
    // view
    showToolbarStandardAct = new QAction (tr("&File Reading"), this);
    showToolbarStandardAct->setCheckable(true);
    showToolbarStandardAct->setChecked(true);
    connect(showToolbarStandardAct, SIGNAL(triggered()), this, SLOT(showToolbarFile()));

    showToolbarRenderAct = new QAction (tr("&Render Options"), this);
    showToolbarRenderAct->setCheckable(true);
    showToolbarRenderAct->setChecked(true);
    connect(showToolbarRenderAct, SIGNAL(triggered()), this, SLOT(showToolbarRender()));

    showToolbarInfoAct = new QAction (tr("&Information"), this);
    showToolbarInfoAct->setCheckable(true);
    showToolbarInfoAct->setChecked(true);
    connect(showToolbarInfoAct, SIGNAL(triggered()), this, SLOT(showToolbarInfo()));

    showToolbarViewAct = new QAction (tr("&View"), this);
    showToolbarViewAct->setCheckable(true);
    showToolbarViewAct->setChecked(true);
    connect(showToolbarViewAct, SIGNAL(triggered()), this, SLOT(showToolbarView()));

    // Action Menu Windows
    windowsTileAct	  = new QAction(QIcon(":/images/winTile.png"),tr("&Tile"),this);
    connect(windowsTileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    windowsCascadeAct	  = new QAction(QIcon(":/images/winCas.png"),tr("&Cascade"),this);
    connect(windowsCascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    windowsMaximizeAct	  = new QAction(QIcon(":/images/winMaximize.png"),tr("&Maximize"),this);
    connect(windowsMaximizeAct, SIGNAL(triggered()), this, SLOT(maximizeSubWindows()));

    windowsNextAct	  = new QAction(QIcon(":/images/winNext.png"),tr("&Next"),this);
    connect(windowsNextAct, SIGNAL(triggered()), mdiArea, SLOT(activateNextSubWindow()));

    closeAllAct = new QAction(tr("Close &All Windows"), this);
	closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, SIGNAL(triggered()), this, SLOT(closeAllWindows()));

	closeWindowAct = new QAction(tr("Close Current Window"), this);
    closeWindowAct->setShortcutContext(Qt::WidgetShortcut);
    closeWindowAct->setShortcut(Qt::CTRL+Qt::Key_W);
    closeWindowAct->setStatusTip(tr("Close the opened 2D image or 3D object window"));
    connect(closeWindowAct, SIGNAL(triggered()), this, SLOT(closeWindow()));

	openWindowAct = new QAction(tr("Open Closed Window"), this);
    openWindowAct->setStatusTip(tr("Open the closed window for 2D image or 3D object"));
    connect(openWindowAct, SIGNAL(triggered()), this, SLOT(openWindow()));

    setSplitGroupAct = new QActionGroup(this);	setSplitGroupAct->setExclusive(true);
    setSplitHAct	  = new QAction(QIcon(":/images/splitH.png"),tr("&Horizontally"),setSplitGroupAct);//
    setSplitVAct	  = new QAction(QIcon(":/images/splitV.png"),tr("&Vertically"),setSplitGroupAct);//

    connect(setSplitGroupAct, SIGNAL(triggered(QAction *)), this, SLOT(setSplit(QAction *)));
    setUnsplitAct = new QAction(tr("&Close current view"),this);
    connect(setUnsplitAct, SIGNAL(triggered()), this, SLOT(setUnsplit()));

    viewFromGroupAct =  new QActionGroup(this);	viewFromGroupAct->setExclusive(true);
    viewFrontAct	  = new QAction(QIcon(":/images/viewFront.png"),tr("Front"),viewFromGroupAct);
    viewLeftAct	    = new QAction(QIcon(":/images/viewLeft.png"),tr("Left"),viewFromGroupAct);
    viewRightAct	  = new QAction(QIcon(":/images/viewRight.png"),tr("Right"),viewFromGroupAct);
    viewTopAct	    = new QAction(QIcon(":/images/viewTop.png"),tr("Top"),viewFromGroupAct);
    viewBottomAct	  = new QAction(QIcon(":/images/viewBottom.png"),tr("Bottom"),viewFromGroupAct);
    viewBackAct	    = new QAction(QIcon(":/images/viewBack.png"),tr("Back"),viewFromGroupAct);
    connect(viewTopAct, SIGNAL(triggered()), this, SLOT(viewFromTop()) );
    connect(viewBottomAct, SIGNAL(triggered()), this, SLOT(viewFromBottom()) );
    connect(viewLeftAct, SIGNAL(triggered()), this, SLOT(viewFromLeft()) );
    connect(viewRightAct, SIGNAL(triggered()), this, SLOT(viewFromRight()) );
    connect(viewFrontAct, SIGNAL(triggered()), this, SLOT(viewFromFront()) );
    connect(viewBackAct, SIGNAL(triggered()), this, SLOT(viewFromBack()) );
#ifdef SUPPORT_SPINANIMATION
    viewSpinAct	    = new QAction(QIcon(":/images/spin.png"),tr("Spin"),this);
    connect(viewSpinAct, SIGNAL(triggered()), this, SLOT(viewSpinMotion()) );
#endif
    writeAnnotationAct = new QAction (QIcon(":/images/annotation_off.png"), tr("Write Annotation"), this);
    writeAnnotationAct->setCheckable(true);
    connect(writeAnnotationAct, SIGNAL(triggered()), this, SLOT(writeAnnotation()));

	annotationModeGroupAct =  new QActionGroup(this);	annotationModeGroupAct->setExclusive(true);
	pointNote = new QAction(tr("Point Note"), annotationModeGroupAct);
    surfaceNote = new QAction(tr("Surface Note"), annotationModeGroupAct);
    frustumNote	= new QAction(tr("Frustum Note"), annotationModeGroupAct);
    connect(pointNote, SIGNAL(triggered()), this, SLOT(writePointNote()));
    connect(surfaceNote, SIGNAL(triggered()), this, SLOT(writeSurfaceNote()));
    connect(frustumNote, SIGNAL(triggered()), this, SLOT(writeFrustumNote()));

	annotationColorGroupAct =  new QActionGroup(this);	annotationColorGroupAct->setExclusive(true);
	annotationMaroon = new QAction(QIcon(":/images/color_maroon.png"), tr("Marron/Object"), annotationColorGroupAct);
	annotationRed = new QAction(QIcon(":/images/color_red.png"), tr("Red/Measurements"), annotationColorGroupAct);
	annotationOrange = new QAction(QIcon(":/images/color_orange.png"), tr("Orange/Creation"), annotationColorGroupAct);
    annotationYellow = new QAction(QIcon(":/images/color_yellow.png"), tr("Yellow/Materials"), annotationColorGroupAct);
	annotationLime = new QAction(QIcon(":/images/color_lime.png"), tr("Lime/Descriptions"), annotationColorGroupAct);
	annotationGreen = new QAction(QIcon(":/images/color_green.png"), tr("Green/Conservation"), annotationColorGroupAct);
	annotationAqua = new QAction(QIcon(":/images/color_aqua.png"), tr("Aqua/Analyses"), annotationColorGroupAct);
	annotationBlue = new QAction(QIcon(":/images/color_blue.png"), tr("Blue/Related"), annotationColorGroupAct);
	annotationPink = new QAction(QIcon(":/images/color_pink.png"), tr("Pink/Administration"), annotationColorGroupAct);
    annotationPurple = new QAction(QIcon(":/images/color_purple.png"), tr("Purple/Documentation"), annotationColorGroupAct);
	annotationWhite = new QAction(QIcon(":/images/color_white.png"), tr("White/Others"), annotationColorGroupAct);
	connect(annotationMaroon, SIGNAL(triggered()), this, SLOT(setAnnotationColorMarron()));
    connect(annotationRed, SIGNAL(triggered()), this, SLOT(setAnnotationColorRed()));
	connect(annotationOrange, SIGNAL(triggered()), this, SLOT(setAnnotationColorOrange()));
	connect(annotationYellow, SIGNAL(triggered()), this, SLOT(setAnnotationColorYellow()));
	connect(annotationLime, SIGNAL(triggered()), this, SLOT(setAnnotationColorLime()));
	connect(annotationGreen, SIGNAL(triggered()), this, SLOT(setAnnotationColorGreen()));
	connect(annotationAqua, SIGNAL(triggered()), this, SLOT(setAnnotationColorAqua()));
	connect(annotationBlue, SIGNAL(triggered()), this, SLOT(setAnnotationColorBlue()));
    connect(annotationPink, SIGNAL(triggered()), this, SLOT(setAnnotationColorPink()));
    connect(annotationPurple, SIGNAL(triggered()), this, SLOT(setAnnotationColorPurple()));
	connect(annotationWhite, SIGNAL(triggered()), this, SLOT(setAnnotationColorWhite()));

    removeAnnotationAct = new QAction (QIcon(":/images/remove_annotation_on.png"), tr("Remove Annotation"), this);
    removeAnnotationAct->setCheckable(true);
    connect(removeAnnotationAct, SIGNAL(triggered()), this, SLOT(removeAnnotation()));

    screenshotAct = new QAction (QIcon(":/images/snapshot.png"), tr("Screenshot"), this);
    screenshotAct->setShortcutContext(Qt::ApplicationShortcut);
    screenshotAct->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_S);
    screenshotAct->setCheckable(false);
    connect(screenshotAct, SIGNAL(triggered()), this, SLOT(takeScreenshot()));

    bookmarkAct = new QAction (QIcon(":/images/bookmark.png"), tr("Bookmark View"), this);
    bookmarkAct->setCheckable(false);
    bookmarkAct->setShortcutContext(Qt::ApplicationShortcut);
    bookmarkAct->setShortcut(Qt::CTRL+Qt::Key_B);
    connect(bookmarkAct, SIGNAL(triggered()), this, SLOT(makeBookmark()));

    fileInfoAct = new QAction (QIcon(":/images/fileinfo.png"), tr("File Info..."), this);
    fileInfoAct->setCheckable(false);
    fileInfoAct->setShortcutContext(Qt::ApplicationShortcut);
    fileInfoAct->setShortcut(Qt::CTRL+Qt::Key_F);
    connect(fileInfoAct, SIGNAL(triggered()), this, SLOT(showFileInfo()));

    projectInfoAct = new QAction (QIcon(":/images/projectinfo.png"), tr("Project Info..."), this);
    projectInfoAct->setCheckable(false);
    projectInfoAct->setShortcutContext(Qt::ApplicationShortcut);
    projectInfoAct->setShortcut(Qt::CTRL+Qt::Key_P);
    connect(projectInfoAct, SIGNAL(triggered()), this, SLOT(showProjectInfo()));

    measureDistanceAct = new QAction(QIcon(":/images/ruler_off.png"), tr("Measuring Tool"), this);
    measureDistanceAct->setCheckable(true);
    connect(measureDistanceAct, SIGNAL(triggered()), this, SLOT(measureDistance()));

    removeDistanceAct = new QAction(QIcon(":/images/remove_ruler_on.png"), tr("Clear Measuring Tool"), this);
    removeDistanceAct->setCheckable(false);
    connect(removeDistanceAct, SIGNAL(triggered()), this, SLOT(removeMeasureDistance()));

    flattenShortcut = new QShortcut(QKeySequence(tr("Shift+F")), this);
    connect(flattenShortcut, SIGNAL(activated()), this, SLOT(flattenMesh()));
}

void MainWindow::createMenus()
{
  // Menu
  fileMenu = menuBar->addMenu(tr("&File"));
  fileMenu->addAction(newVtkProjectAct);
  fileMenu->addAction(openProjectAct);
  fileMenu->addAction(saveProjectAct);
  fileMenu->addAction(saveProjectAsAct);
  fileMenu->addAction(closeProjectAct);
  
  fileMenu->addSeparator();
  fileMenu->addAction(openObjectAct);
  fileMenu->addAction(openDICOMAct);
  fileMenu->addAction(importProjectAct);
  fileMenu->addAction(removeObjectAct);

  fileMenu->addSeparator();

  recentProjMenu = fileMenu->addMenu(tr("Recent Projects"));
  recentFileMenu = fileMenu->addMenu(tr("Recent Files"));

  for (int i = 0; i < MAXRECENTFILES; ++i)
  {
    recentProjMenu->addAction(recentProjActs[i]);
    recentFileMenu->addAction(recentFileActs[i]);
  }
  
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);
  // Menu Edit
//  editMenu = menuBar->addMenu(tr("&Edit"));
//  editMenu->addAction(setCustomizeAct);

  // Render menu
  renderMenu		= menuBar->addMenu(tr("&Render"));

  renderModeMenu=renderMenu->addMenu(tr("Render &Mode"));
  renderModeMenu->addActions(renderModeGroupAct->actions());
  renderModeMenu->addSeparator();
  renderModeMenu->addAction(renderModeTextureAct);

  renderMenu->addSeparator();

  lightingModeMenu=renderMenu->addMenu(tr("&Lighting"));
  lightingModeMenu->addAction(setDirectionalLightAct);

  renderMenu->addSeparator();

  cameraModeMenu = renderMenu->addMenu(tr("&Camera"));
  cameraModeMenu->addAction(setTrackballCameraAct);
  cameraModeMenu->addAction(setSurfaceWalkerCameraAct);
  cameraModeMenu->addAction(showImageProvenanceAct);

  //menuBar->addSeparator();

  // Menu: View
  viewMenu		= menuBar->addMenu(tr("&View"));
  viewMenu->addAction(renderModeInfoAct);
  viewMenu->addSeparator();

  viewMenu->addAction(renderModeInterpolationAct);
  viewMenu->addSeparator();
  viewMenu->addAction(showPolyIndicateAct);
  viewMenu->addSeparator();
  viewMenu->addAction(zoomResetAct);
  viewMenu->addAction(zoomInAct);
  viewMenu->addAction(zoomOutAct);
  viewMenu->addSeparator();

  // View From SUBmenu
  viewFromMenu = viewMenu->addMenu(tr("&View From"));
  foreach(QAction *ac, viewFromGroupAct->actions())
    viewFromMenu->addAction(ac);

  menuBar->addSeparator();

  // Menu Preferences
  toolsMenu = menuBar->addMenu(tr("&Tools"));
  toolsMenu->addAction(measureDistanceAct);
  toolsMenu->addAction(removeDistanceAct);
  toolsMenu->addSeparator();
  //toolsMenu->addAction(writeAnnotationAct);
  annotationModeMenu = toolsMenu->addMenu(tr("Write Annotation"));
  foreach(QAction *ac, annotationModeGroupAct->actions())
    annotationModeMenu->addAction(ac);
  annotationModeMenu->addSeparator();
  annotationColorMenu = annotationModeMenu->addMenu(tr("Color"));
  foreach(QAction *ac, annotationColorGroupAct->actions())
    annotationColorMenu->addAction(ac);
  toolsMenu->addAction(removeAnnotationAct);
  toolsMenu->addSeparator();
  toolsMenu->addAction(screenshotAct);
  toolsMenu->addSeparator();
  toolsMenu->addAction(bookmarkAct);
  toolsMenu->addSeparator();
  toolsMenu->addAction(fileInfoAct);
  toolsMenu->addAction(projectInfoAct);

  menuBar->addSeparator();

  // Menu: Windows
  windowsMenu = menuBar->addMenu(tr("&Windows"));
  connect(windowsMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

  toolBarMenu	= windowsMenu->addMenu(tr("&ToolBars"));
  toolBarMenu->addAction(showToolbarStandardAct);
  toolBarMenu->addAction(showToolbarRenderAct);
  toolBarMenu->addAction(showToolbarInfoAct);
  connect(toolBarMenu,SIGNAL(aboutToShow()),this,SLOT(updateMenus()));



  // Menu: Help
  helpMenu = menuBar->addMenu(tr("&Help"));
  helpMenu->addAction(helpAct);
  helpMenu->addAction(onlineHelpAct);
  helpMenu->addAction(aboutAct);
}

void MainWindow::helpOnline()
{
    QDesktopServices::openUrl(QUrl("http://sourceforge.net/projects/hyper3d/support"));
}


void MainWindow::createToolBars()
{
  //MK: add main toolbar ==============================================
  mainToolBar = addToolBar(tr("File Reading Toolbar"));
  mainToolBar->setObjectName("toolbarStandard");
  mainToolBar->setIconSize(QSize(32,32));
  mainToolBar->setMovable(false); // set the toolbar not movable

  mainToolBar->addAction(this->newVtkProjectAct);
  mainToolBar->addSeparator();
  mainToolBar->addAction(this->openProjectAct);
  mainToolBar->addAction(this->openObjectAct);
  mainToolBar->addAction(this->openDICOMAct);
  mainToolBar->addSeparator();
  mainToolBar->addAction(this->saveProjectAsAct);

  mainToolBar->setVisible(true);
  mainToolBar->setEnabled(true);

  //MK: add render toolbar ==============================================
  renderToolBar = addToolBar(tr("Render Option Toolbar"));
  renderToolBar->setObjectName("toolbarRender");
  renderToolBar->setIconSize(QSize(32,32));
  renderToolBar->setMovable(false); // set the toolbar not movable

  renderToolBar->addSeparator();
  renderToolBar->addAction(setDirectionalLightAct);

  renderToolBar->addSeparator();
  QMenu *cameraDropMenu = new QMenu();
  cameraDropMenu->addAction(setTrackballCameraAct);
  cameraDropMenu->addAction(setSurfaceWalkerCameraAct);
  cameraDropMenu->addAction(showImageProvenanceAct);
  QToolButton *cameraToolButton = new QToolButton();
  cameraToolButton->setText(tr("Camera"));
  cameraToolButton->setIcon(QIcon(":/images/camera_icon.png"));
  cameraToolButton->setMenu(cameraDropMenu);
  cameraToolButton->setPopupMode(QToolButton::InstantPopup);
  renderToolBar->addWidget(cameraToolButton);

  renderToolBar->addSeparator();
  renderToolBar->addActions(renderModeGroupAct->actions());
  renderToolBar->addSeparator();
  renderToolBar->addAction(renderModeTextureAct);

  //MK: add info toolbar ==============================================
  infoToolBar = addToolBar(tr("Information Toolbar"));
  infoToolBar->setObjectName("toolbarInformation");
  infoToolBar->setIconSize(QSize(32,32));
  infoToolBar->setMovable(false); // set the toolbar not movable

  infoToolBar->addSeparator();
  infoToolBar->addAction(renderModeInterpolationAct);

  infoToolBar->addSeparator();
  infoToolBar->addAction(renderModeInfoAct);

  infoToolBar->addSeparator();
  infoToolBar->addAction(showPolyIndicateAct);


  //MK: add render toolbar ==============================================
  measureToolBar = addToolBar(tr("Measure"));
  measureToolBar->setObjectName("measureDistance");
  measureToolBar->setIconSize(QSize(32,32));
  measureToolBar->setMovable(false); // set the toolbar not movable
  measureToolBar->addSeparator();
  measureToolBar->addAction(measureDistanceAct);
  measureToolBar->addAction(removeDistanceAct);

  annotationToolBar = addToolBar(tr("Annotation"));
  annotationToolBar->setObjectName("annotation");
  annotationToolBar->setIconSize(QSize(32,32));
  annotationToolBar->setMovable(false);
  annotationToolBar->addSeparator();
  annotationToolBar->addAction(writeAnnotationAct);
  annotationToolBar->addAction(removeAnnotationAct);
  annotationToolBar->addSeparator();
  annotationToolBar->addAction(screenshotAct);
  annotationToolBar->addSeparator();
  annotationToolBar->addAction(bookmarkAct);
  annotationToolBar->addSeparator();
  annotationToolBar->addAction(fileInfoAct);
  annotationToolBar->addAction(projectInfoAct);

  windowToolBar = addToolBar(tr("Window"));
  windowToolBar->setObjectName("windowToolbar");
  windowToolBar->setIconSize(QSize(32,32));
  windowToolBar->setMovable(false); // set the toolbar not movable
  windowToolBar->addSeparator();
  windowToolBar->addAction(windowsTileAct);
  windowToolBar->addAction(windowsMaximizeAct);
#ifdef SUPPORT_WINDOW_CASCADE
  windowToolBar->addAction(windowsCascadeAct);
#endif
  windowToolBar->addAction(windowsNextAct);


  //MK: add info toolbar ==============================================
  viewToolBar = addToolBar(tr("View Toolbar"));
  viewToolBar->setObjectName("toolbarView");
  viewToolBar->setIconSize(QSize(32,32));
  viewToolBar->setMovable(false); // set the toolbar not movable

  viewToolBar->addSeparator();
  viewToolBar->addAction(viewFrontAct);
  viewToolBar->addAction(viewLeftAct);
  viewToolBar->addAction(viewRightAct);
  viewToolBar->addAction(viewTopAct);
  viewToolBar->addAction(viewBottomAct);
  viewToolBar->addAction(viewBackAct);
#ifdef  SUPPORT_SPINANIMATION
  viewToolBar->addAction(viewSpinAct);
#endif

  viewToolBar->addSeparator(); // at the end of toolbar
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::showToolbarFile(){
    mainToolBar->setVisible(!mainToolBar->isVisible());
}

void MainWindow::showToolbarRender(){
  renderToolBar->setVisible(!renderToolBar->isVisible());
}

void MainWindow::showToolbarInfo(){
  infoToolBar->setVisible(!infoToolBar->isVisible());
}

void MainWindow::showToolbarView(){
  viewToolBar->setVisible(!viewToolBar->isVisible());
}

QWidget * MainWindow::mountWidgetCenter(QWidget *inputWidget)
{
  QWidget * outputWidget = new QWidget();
  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(inputWidget);
  outputWidget->setLayout(layout);
  outputWidget->show();
  return outputWidget;
}

void MainWindow::createDockWindows()
{
   //QTabWidget* tabWidgetTop = NULL, *tabWidgetMid = NULL, *tabWidgetBottom = NULL;
   tabWidgetTop = NULL; // YY
   tabWidgetMid = NULL;
   tabWidgetBottom = NULL;
   //this->setDockOptions(QMainWindow::ForceTabbedDocks);

  //MK: main window size
  // [ver.1]-----------------------------------------------
  int mwWidth = this->sizeHint().width();
  int mwHeight = this->sizeHint().height();
  // [ver.2]-----------------------------------------------
//  int mwWidth = this->frameGeometry().width();
//  int mwHeight = this->frameGeometry().height();

  int dockwidth = 440;
  //-----------------------------------------------------
  // MK: light control dock
  QDockWidget *dockTop = new QDockWidget(this);
  dockTop->setObjectName("DockTop");
  dockTop->setAllowedAreas(Qt::RightDockWidgetArea); //Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea
//  dockTop->setFeatures(QDockWidget::NoDockWidgetFeatures); // remove the floating features.
//  dockTop->setFloating(false);
  dockTop->setFeatures(QDockWidget::DockWidgetFloatable);

  if(mwHeight < 600) {
      dockTop->setMinimumHeight(300);
      dockTop->setMaximumHeight(300);
  } else {
      dockTop->setMinimumHeight(230);
      dockTop->setMaximumHeight(230);
  }
  dockTop->setMinimumWidth(dockwidth);
  dockTop->setMaximumWidth(dockwidth);

  tabWidgetTop = new QTabWidget;
  tabWidgetMid = new QTabWidget;

  //mLightControl = new LightControl(this);
  //mLightControlRTI = new LightControlRTI(this, 160); // YY
  //tabWidgetTop->addTab(mLightControl, tr("Light Control 3D") ); // YY
  //tabWidgetTop->addTab(mLightControlRTI, tr("Light Control RTI") ); // YY

  
  //YY
  mLightControl = new LightControl(this);
  tabWidgetTop->addTab(mLightControl, tr("Light Control 3D") );
  activateTabWidgetTop(static_cast<int>(LightControlType::Model3DLIGHTCONTROL));

  
  mLightControlRTI = new LightControlRTI(this, 160);

  QGroupBox* renderingGroup = new QGroupBox("Rendering Mode", this);
  renderingGroup->setFixedWidth(250);
  renderingGroup->setFixedHeight(162);
  
  rendDlg = new RenderingDialog(NULL, -1, renderingGroup);
  QVBoxLayout* rendLayout = new QVBoxLayout;
  rendLayout->setContentsMargins(0, 0, 0, 0);
  rendLayout->addWidget(rendDlg);
  renderingGroup->setLayout(rendLayout);
  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(mLightControlRTI);
  layout->addWidget(renderingGroup);
  QWidget *windowTemp = new QWidget;
  windowTemp->setLayout(layout);
  tabWidgetTop->addTab(windowTemp, tr("Light Control RTI") );

  
  connect(this, SIGNAL(currentWidgetModeChanged(WidgetMode)), mLightControl, SLOT( updateLightControl(WidgetMode) ) );
  emit currentWidgetModeChanged(EMPTYWIDGET);

  tabWidgetTop->show();
  dockTop->setWidget(tabWidgetTop);

  addDockWidget(Qt::RightDockWidgetArea, dockTop);
//  viewMenu->addAction(dockTop->toggleViewAction());
  //-----------------------------------------------------
  // MK: multi orthogonal view setting
  QDockWidget *dockMid = new QDockWidget(this);
  dockMid->setObjectName("dockMid");
//  dockMid->setFeatures(QDockWidget::NoDockWidgetFeatures); // remove the floating features.
//  dockMid->setFloating(false);
  dockMid->setFeatures(QDockWidget::DockWidgetFloatable);
  dockMid->setAllowedAreas(Qt::RightDockWidgetArea);

  if(mwHeight < 600) {
      dockMid->setMinimumHeight(335);
      dockMid->setMaximumHeight(335);
  } else {
      dockMid->setMinimumHeight(300);
      dockMid->setMaximumHeight(300);
  }
  dockMid->setMinimumWidth(dockwidth);
  dockMid->setMaximumWidth(dockwidth);
  //-----------------------------------------------------

  // CT control
  mCtControl = new CTControl;
  mPlotView = new PlotView;

  connect(this, SIGNAL(currentWidgetModeChanged(WidgetMode)), mCtControl, SLOT( updateCtControlWidgetMode(WidgetMode) ) );
  emit currentWidgetModeChanged(EMPTYWIDGET);

  if (mwHeight < 600) {
      tabWidgetTop->addTab(mCtControl, tr("CT Image Control") );
      tabWidgetTop->addTab(mPlotView, tr("Spectrum") ); // test for API
  } else {
      tabWidgetMid->addTab(mCtControl, tr("CT Image Control") ); // CT rendering control
      tabWidgetMid->addTab(mPlotView, tr("Spectrum") ); // test for API

      dockMid->setWidget(tabWidgetMid);
      addDockWidget(Qt::RightDockWidgetArea, dockMid);
  }

//  qDebug() << "mwHeight: " << mwHeight; // Macbook Pro Retina 700
  mInformation = new Information(this);
  mBookmark = new BookmarkWidget(this);
  mSearch = new SearchWidget(this);
  mSearchAll = new SearchAllWidget(this);
  if (mwHeight < 600)
  {
      tabWidgetMid->addTab(mInformation, tr("Annotations") );
      tabWidgetMid->addTab(mBookmark, tr("Bookmarks") );
	  tabWidgetMid->addTab(mSearch, tr("Search") );
	  tabWidgetMid->addTab(mSearchAll, tr("SearchAll") );

      dockMid->setWidget(tabWidgetMid);
      addDockWidget(Qt::RightDockWidgetArea, dockMid);
  } else {
      QDockWidget *dockBot = new QDockWidget(this);
      dockBot->setObjectName("DocBot");
      dockBot->setFeatures(QDockWidget::DockWidgetFloatable);
      dockBot->setAllowedAreas(Qt::RightDockWidgetArea);

      dockBot->setMinimumHeight(335);
      dockBot->setMaximumHeight(335);
      dockBot->setMinimumWidth(dockwidth);
      dockBot->setMaximumWidth(dockwidth);

      tabWidgetBottom = new QTabWidget;
      tabWidgetBottom->addTab(mInformation, tr("Annotations") );
      tabWidgetBottom->addTab(mBookmark, tr("Bookmarks") );
	  tabWidgetBottom->addTab(mSearch, tr("Search") );
	  tabWidgetBottom->addTab(mSearchAll, tr("SearchAll") );

      dockBot->setWidget(tabWidgetBottom);
      addDockWidget(Qt::RightDockWidgetArea, dockBot);
 }

  if(tabWidgetTop) tabWidgetTop->show();
  if(tabWidgetMid) tabWidgetMid->show();
  if(tabWidgetBottom) tabWidgetBottom->show();
}

QTabWidget* MainWindow::getSearchTabWidget()
{
	int mwHeight = this->sizeHint().height();
	if (mwHeight < 600)
	{
		return tabWidgetMid;
	}
	else
	{
		return tabWidgetBottom;
	}
}

void MainWindow::writeSettings()
{
  QSettings settings("Yale Graphics Lab", "Hyper3D");
  settings.beginGroup("MainWindow");
  settings.setValue("size", size());
  settings.setValue("geometry", saveGeometry());
  settings.setValue("state", saveState()); // it will save the location of windows and dock locations too.
  settings.setValue("lastUsedDirectory", lastUsedDirectory.path());
//  qDebug() << "saving last used directory: " << lastUsedDirectory.path();
  settings.endGroup();
  settings.sync();
}

void MainWindow::readSettings()
{
    QSettings settings("Yale Graphics Lab", "Hyper3D");
    settings.beginGroup("MainWindow");
    //restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());// it will save the location of windows and dock locations too.
    QString path = settings.value("lastUsedDirectory").toString();
    QFileInfo pathfi(path);
    if (pathfi.exists())
        lastUsedDirectory = path;
    showMaximized();
    //resize(settings.value("size").toSize());
//    qDebug() << "reading last used directory: " << lastUsedDirectory.path();
    settings.endGroup();
}

void MainWindow::writeAnnotation()
{
  QAction *a = qobject_cast<QAction* >(sender());
  bool answer = a->isChecked() ? true : false;
  if (VTKA())
    VTKA()->annotate(answer);
  updateMenus();
}

void MainWindow::removeAnnotation()
{
  int ret = QMessageBox::warning(this, tr("Warning"),
                               tr("Do you want to remove all the notes in current window?"),
                               QMessageBox::Yes | QMessageBox::No,
                               QMessageBox::No);
  removeAnnotationAct->setChecked(false);
  if(ret == QMessageBox::No) return;
  writeAnnotationAct->setChecked(false);
  this->mInformation->removeAllNotes();
  
  if (VTKA())
    VTKA()->annotate(false);

  updateMenus();
}

void MainWindow::takeScreenshot()
{
  if (VTKA())
    VTKA()->screenshot();

  updateMenus();
}

void MainWindow::makeBookmark()
{
  if (mBookmark)
    mBookmark->createBookmark();

  updateMenus();
}

void MainWindow::showFileInfo()
{
    if (VTKA())
        VTKA()->displayFileInfo();

    updateMenus();
}

void MainWindow::showProjectInfo()
{
    if (!currentProjectName.isEmpty())
	{
		if (mProjectInfoDialog != NULL)
		{
			mProjectInfoDialog->hide();
			delete mProjectInfoDialog;
		}
		mProjectInfoDialog = new ProjectInfoDialog(currentProjectName, currentProjectFullName, 
			currentProjectKeyword, currentProjectAffiliation, mUserName, currentProjectDescription);
		connect(mProjectInfoDialog, SIGNAL(projectInfoChanged(const QString, const QString, const QString, const QString)), 
			this, SLOT(projectInfoChanged(const QString, const QString, const QString, const QString)));
		mProjectInfoDialog->exec();
	}
    updateMenus();
}

void MainWindow::projectInfoChanged(const QString userName, const QString keyword, const QString affiliation, const QString description)
{
	mUserName = userName;
	currentProjectKeyword = keyword;
	currentProjectAffiliation = affiliation;
	currentProjectDescription = description;
	updateXML();
}

void MainWindow::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        qApp->setLayoutDirection(Qt::RightToLeft);
    else
        qApp->setLayoutDirection(Qt::LeftToRight);
}

//MK:  doesn't make it different
void MainWindow::wrapSetActiveSubWindow(QWidget* window){
  QMdiSubWindow* subwindow;
  subwindow = dynamic_cast<QMdiSubWindow*>(window);
  if(subwindow!= NULL){
    mdiArea->setActiveSubWindow(subwindow);
//    VTKA()->setFocus();
  }else{
    qDebug("Type of window is not a QMdiSubWindow*");
  }
}

//MK: this is old
void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

//MK this is for rendering progress bar
bool MainWindow::QCallBack(const int pos, const char * str)
{
  int static lastPos=-1;
  if(pos==lastPos) return true;
  lastPos=pos;

  static QTime currTime;
  if(currTime.elapsed()< 100) return true;
  currTime.start();
  MainWindow::globalStatusBar()->showMessage(str,5000);
  qb->show();
  qb->setEnabled(true);
  qb->setValue(pos);
  MainWindow::globalStatusBar()->update();
  qApp->processEvents();

  return true;
}

void MainWindow::measureDistance()
{
  QAction *a = qobject_cast<QAction* >(sender());
  bool answer = a->isChecked() ? true : false;

  if (VTKA())
  {
    VTKA()->setMeasureDistance( answer );
  }

  updateMenus();
}

void MainWindow::removeMeasureDistance()
{
  QAction *a = qobject_cast<QAction* >(sender());
  bool answer = a->isChecked() ? true : false;

  if (VTKA())
    VTKA()->setVisibilityDistance( answer );

  updateMenus();
}
