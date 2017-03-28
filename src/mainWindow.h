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
   of ITK/VTK library from Kitware, QT API from Nokia and Meshlab VCG library
   from ISTI CNR. I would like to thank anonymous help by various software
   engineering communities.

*****************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#if defined(_WIN32) || defined(_WIN64)
#pragma warning(disable:4138)
#endif

//#include <GL/glew.h>
#include <QtScript>
#include <QProgressBar>
#include <QDir>
#include <QMainWindow>
#include <QMdiArea>
#include <QStringList>
#include <QColorDialog>
#include <QDomDocument>
#include "visualization/vtkWidget.h" //MK: The inclusion of this header should be only here, except implementations.
#include "information/newProjectDialog.h"
#include "information/projectInfoDialog.h"
#include "information/saveProjectAsDialog.h"
#include "information/removeObjectDialog.h"
#include "information/openWindowDialog.h"
#include "CHE/newCHEDialog.h"
#include "CHE/CHETab.h"
#include "CHE/saveCHEAsDialog.h"
#include "CHE/exportToProjectDialog.h"
#include "CHE/mergeBackToCHEDialog.h"
#include "CHE/importFromCHEDialog.h"
#include "function/reportGenerator.h"
#include "function/videoGenerator.h"
#include "function/navigation.h"
#include "function/projectCITab.h"
#include "function/mapKit/mapWidget.h"

#define MAXRECENTFILES 8

static const QString WORD_SEPARATOR = "_";

//#define SUPPORT_PROJECT
//#define SUPPORT_SPINANIMATION

QT_BEGIN_NAMESPACE
class QAction;
class QShortcut;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QProgressDialog;
QT_END_NAMESPACE

class CTControl;
class PlotView;
class LightControl;
class LightControlRTI; //YY
class RenderingDialog; //YY
class Information;
class Bookmark;
class SearchWidget;
class SearchAllWidget;

#ifdef _WIN32
#define BITS " (32-bit)"
#endif
#ifdef _WIN64
#define BITS " (64-bit)"
#endif
#ifdef __APPLE__
#define BITS " (64-bit)"
#endif
#ifdef __GNUC__
#if __x86_64__ || __ppc64__
#define BITS " (64-bit)"
#warning BITS " (64-bit)"
#else
#define BITS " (32-bit)"
#warning BITS " (32-bit)"
#endif
#endif

/**
 * This class is the main frame of the software.
 */

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 */
	MainWindow();

	/**
	 * @brief  Set the application name.
	 */
	const QString appName() const {return tr("CHER-Ob")+appVer(); }

	/**
	 * @brief  Set the application version number.
	 */
	const QString appVer() const {return tr(" 1.1.0"); }

	/**
	 * @brief  Set the bits of the running application.
	 */
	const QString appBits() const {return tr(BITS); }

	/**
	 * @brief  Get the project information.
	 * @param  Project name, project absolute path, keyword, affiliation,
	 *         user name and description.
	 */
	void getProjectInfo(QString& projectName, QString& location, QString& keyword,
		QString& affiliation, QString& userName, QString& description);

	/**
	 * @brief  Set the split/unsplit menu that appears right clicking on a splitter's handle.
	 */
	void setHandleMenu(QPoint p, Qt::Orientation o, QSplitter *origin);

	/**
	 * @brief  This is for rendering progress bar.
	 */
	static bool QCallBack(const int pos, const char * str);

	static QProgressBar *qb;

	/**
	 * @brief Get the vtkView instance of the object in current active window.
	 * @return  The vktView instance.
	 */
	inline VtkView* currentVtkView() const 
	{
		if(mdiArea->currentSubWindow()==0) return 0;
		VtkView *mvc = qobject_cast<VtkView *>(mdiArea->currentSubWindow());
		if(!mvc)
		{
		  mvc = qobject_cast<VtkView *>(mdiArea->currentSubWindow()->widget());
		  return mvc;
		}
		else return 0;
	}

	/**
	 * @brief Get the VtkWidget instance of the object in current active window.
	 * @return  The VtkWidget instance.
	 */
	inline VtkWidget *VTKA() const
	{
		if(mdiArea->currentSubWindow()==0) return 0;
		VtkView *mvc = currentVtkView();
		if(!mvc) return 0;
		VtkWidget *glw =  qobject_cast<VtkWidget*>(mvc->currentView());
		if(!glw) return 0;
		return glw;
	}

	/**
	 * @brief Get the VtkWidget instance of the specific object.
	 * @param   path  The absolute path of the object.
	 * @return  The VtkWidget instance.
	 */
	inline VtkWidget *VTKA(const QString path) const 
	{
		if(mdiArea->currentSubWindow()==0) return 0;
		QList<QMdiSubWindow*> windows = mdiArea->subWindowList();
		foreach(QMdiSubWindow *w, windows)
		{
			VtkView* mvc = qobject_cast<VtkView *>(w->widget());
			if(!mvc) continue;
			if (mvc->currentView()->mProjectPath == path)
			{
				VtkWidget *glw =  qobject_cast<VtkWidget*>(mvc->currentView());
				return glw;
			}
		}
		return 0;
	}

	/**
	 * @brief Switch the active window.
	 * @param   path  The absolute path of the object.
	 */
	void switchSubWindow(const QString path) const 
	{
		if(mdiArea->currentSubWindow()==0) return;
		QList<QMdiSubWindow*> windows = mdiArea->subWindowList();
		foreach(QMdiSubWindow *w, windows)
		{
			VtkView* mvc = qobject_cast<VtkView *>(w->widget());
			if(!mvc) continue;
			if (mvc->currentView()->mProjectPath == path)
			{
				mdiArea->setActiveSubWindow(w);
				break;
			}
		}
	}

	/**
	 * @brief Get the globle status bar.
	 */
	static QStatusBar *&globalStatusBar()
	{
		static QStatusBar *_qsb=0;
		return _qsb;
	}

	/**
	 * @brief This function is currently not being used.
	 */
	inline void createDataLocation(QString path)
	{
		QStringList tokens = path.split(QDir::separator());
		QString CHEROb = tokens.takeLast();
		path = tokens.join(QDir::separator());
		path = QDir::toNativeSeparators(path);
		if(!QDir(path).exists())
		{
		  QDir().mkdir(path);
		}
		path.append(QDir::separator() + CHEROb);
		if(!QDir(path).exists())
		{
		  QDir().mkdir(path);
		}
	}

	/**
	 * @brief Get the right tab (application tab).
	 * @return  The right tab widget.
	 */
	QTabWidget* getRightTabWidget()	{return rightTab;}

	/**
	 * @brief Get the top tab of the right tab.
	 * @return  The top tab widget.
	 */
	QTabWidget* getTabWidgetTop() {return tabWidgetTop;}

	/**
	 * @brief Get the search tab, which is in the bottom of the right tab.
	 * @return  The seach tab widget.
	 */
	QTabWidget* getSearchTabWidget();

	/**
	 * @brief Activate one page on the top tab.
	 * @return  The activated page index.
	 */
	void activateTabWidgetTop(int index)
	{
	  if (tabWidgetTop)
		  tabWidgetTop->setCurrentIndex(index);
	}

	/**
	 * @brief Friend class. Open the bookmarked status by double clicked the item.
	 * @param  item  The double clicked item.
	 * @return If the status is correctly restored, return true.
	 */
	friend bool BookmarkWidget::viewBookmark(QTreeWidgetItem* item, QString objectPath);

	/**
	 * @brief Friend class. It is currently not being used.
	 */
	friend Metadata::Metadata(QString path, VtkWidget *w, bool file);

	CTControl * mCtControl;
	PlotView * mPlotView;
	LightControl *mLightControl;
	LightControlRTI *mLightControlRTI;  //YY
	RenderingDialog* rendDlg; //YY /*!< Widget to choose the rendering mode to apply to RTI image. */
	Information *mInformation;
	MapWidget *mGeoInfo;
	BookmarkWidget *mBookmark;
	SearchWidget *mSearch;
	SearchAllWidget *mSearchAll;
	QTextEdit *mInfoText;
	QDir lastUsedDirectory;
	QDir lastSavedDirectory; // DT: Keeps track of screenshot/project saved location
	QString mUserName;
	bool isSaved;

	void updateXML4Map() { updateXML(); }

public slots:
	/**
	 * @brief Update all the menu and toolbar.
	 */
	void updateAllViews();

	/**
	 * @brief  Open, load or import a single object (2/3D image) to the software. Open means it is first introduced to the
	           project/CHE. Load means the object is in the current project/CHE, it is just reload to the software.Import means 
			   the object and its related files already exist in some other peojects/CHEs and they are imported to current project/CHE.
	 * @param  fileNameStart  The object absolute location. If it is not specified, then user will be asked to do it.
	 * @param  CHEName        The origianl CHE that the object belongs to.
	 * @param  saveRecent     Whether to save the object as Recent File.
	 * @param  createFolder   Whether to create a new folder for this object. If the object is opened or imported, then new folder
	 *                        should be created. If it is loaded, then not.
	 * @param  import         Whether the object is imported.
	 * @return The code of open result. SCCUEED means the object is correctly opend. FAILED means it is not opened. EXISTED means
	 *         the object already exists and is opened.
	 */
	OPENRESULT openImages(const QString& fileNameStart = QString(), const QString& CHEName = QString(),
	  bool saveRecent = true, bool createFolder = true, bool import = false);

	/**
	 * @brief  Open, load or import a DICOM folder (CT data) to the software. Open means it is first introduced to the
	           project/CHE. Load means the object is in the current project/CHE, it is just reload to the software.Import means 
			   the object and its related files already exist in some other peojects/CHEs and they are imported to current project/CHE.
	 * @param  fileNameStart  The CT folders' absolute location. If it is not specified, then user will be asked to do it.
	 * @param  CHEName        The origianl CHE that the CT belongs to.
	 * @param  saveRecent     Whether to save the CT as Recent File.
	 * @param  createFolder   Whether to create a new folder for this CT data. If it is opened or imported, then new folder
	 *                        should be created. If it is loaded, then not.
	 * @param  import         Whether the CT is imported.
	 * @param  type           Open DICOM as 2d stack ("STACK") or volume ("VOLUME").
	 * @return The code of open result. SCCUEED means the CT is correctly opend. FAILED means it is not opened. EXISTED means
	 *         the CT already exists and is opened.
	 */
	OPENRESULT openDICOM(const QString& fileNameStart = QString(), const QString& CHEName = QString(),
	  bool saveRecent = true, bool createFolder = true, bool import = false, QString type = QString());

	/**
	 * @brief  Update toolbars.
	 */
	void updateNoteMode();
	void updateMenus();

	/**
	 * @brief  Open the file info dialog for the object in current window.
	 */
	void showFileInfo();

	/**
	 * @brief  Open the project info dialog.
	 */
	void showProjectInfo();

	/**
	 * @brief  Change the current project info based on the modification in the project info dialog.
	 */
	void projectInfoChanged(const QString userName, const QString keyword, const QString affiliation, const QString description);

signals:
	/**
	 * @brief  Emit the signal when current widget mode changes.
	 * @param  The new changed mode.
	 */
	void currentWidgetModeChanged(WidgetMode widgetmode);

protected:
	/**
	 * @brief  Handle the close event when user press the red cross on the right corner.
	 */
	void closeEvent(QCloseEvent *event);

private:
	/**
	 * @brief  Filter close event.
	 */
	bool eventFilter(QObject* obj, QEvent* e);

	/**
	 * @brief  Create and define QActions.
	 */
	void createActions();

	/**
	 * @brief  Create the menus and add actions.
	 */
	void createMenus();

	/**
	 * @brief  Create the toolbar and add actions.
	 */
	void createToolBars();

	/**
	 * @brief  Create the status bar.
	 */
	void createStatusBar();

	/**
	 * @brief  Create the Dock window and application tab.
	 */
	void createDockWindows();

	/**
	 * @brief  Create and add the navigation tab in dock window.
	 */
	void createNavigationDockWindows();

	/**
	 * @brief  Create and add the categorized info tab in dock window for project.
	 */
	void createClassifiedInfoDockWindows();

	/**
	 * @brief  Create and add the CHE tab in dock window for CHE.
	 * @param  info  The CHE info that is used create content of the tab.
	 */
	void createCHEDockWindows(const CHEInfoBasic* info);

	/**
	 * @brief  Read system settings.
	 */
	void readSettings();

	/**
	 * @brief  Write system settings.
	 */
	void writeSettings();

	/**
	 * @brief  Save the current project as Recent Project.
	 * @param  projName  The current project name.
	 */
	void saveRecentProjectList(const QString &projName);

	/**
	 * @brief  Save the current CHE as Recent CHE.
	 * @param  projName  The current CHE name.
	 */
	void saveRecentCHEList(const QString &projName);

	/**
	 * @brief  Save the current object as Recent File.
	 * @param  fileName  The current object name.
	 */
	void saveRecentFileList(const QString &fileName);

	/**
	 * @brief  Update the list of Recent Project in menu.
	 */
	void updateRecentProjActions();

	/**
	 * @brief  Update the list of Recent CHE in menu.
	 */
	void updateRecentCHEActions();

	/**
	 * @brief  Update the list of Recent File in menu.
	 */
	void updateRecentFileActions();

	/**
	 * @brief  Copy the folder and its all files and subfolders to specific place.
	 * @param  srcPath  The origianl folder's absolute path.
	 * @param  dstPath  The destination folder's absolute path.
	 */
	bool cpDir(const QString &srcPath, const QString &dstPath);

	/**
	 * @brief  Remove the folder and its all files and subfolders.
	 * @param  dirPath  The folder's absolute path.
	 */
	bool rmDir(const QString &dirPath);

	/**
	 * @brief  Create the folder for CT data. The folder includes a Note, a BookMark, a CT file subfolder.
	 * @param  path  The folder's absolute path.
	 */
	void createCTFolder(QString path);

	/**
	 * @brief  Create the folder for an object. The folder includes a Note, a BookMark subfolder.
	 * @param  path  The folder's absolute path.
	 */
	void createObjectFolder(QString path);

	/**
	 * @brief  Load the whole project/CHE from the .xml file.
	 * @param  fileName    The aboslute path of the .xml file.
	 * @param  objectList  The vector of <the full path of object, the full path of its original CHE>.
	 * @param  objectType  The vector of object type. The objects are related to ObjectList.
	 * @param  filterList  It only works when import a CHE/project. If the object name is in the filter list,
	 *                     this object should not be imported.
	 * @param  import      Whether the object is imported.
	 * @param  readCHE     Whether the xml file is a CHE file.
	 * @return True if correctly load the project/CHE.
	 */
	bool readXML(QString fileName, QVector<QPair<QString, QString> > &objectList, QVector<int> &objectType,
		QVector<QString> filterList, bool import, bool readCHE);	

	/**
	 * @brief  Export the project as a .xml file.
	 * @param  Absolute path of the xml file location, project name, user name, keyword, affiliation, description.
	 */
	void exportProjectXML(const QString path, const QString name, const QString userName, const QString keyword, 
	  const QString affiliation, const QString description);

	///*** Geological information should be stored in XML ***///

private slots:
	/**
	 * @brief  Functions related to corresponding QActions.
	 */
	void zoomIn();
	void zoomOut();
	void zoomReset();
	void changeBGColor();
	void save();
	void saveAs();
	void removeObject();
	void openRecentProj();
	void openRecentCHE();
	void openRecentFile();

	/**
	 * @brief  Handle new project button and create newProjectDialog. 
	 */
	void newVtkProject();

	/**
	 * @brief  Create the new project based on the information in newProjectDialog
	 * @param  Project Full Path, Project Name, User Mode,  User Name, Imported Object Full Path, 
		       Imported CT Full Path, Keyword, Affiliation, Description.
	 */
	void createNewVtkProject(const QString fullName, const QString name, const USERMODE mode, const QString userName, 
	  const QString object, const QString ct, const QString keyword, const QString affiliation, const QString description);

	/**
	 * @brief  Open an existing project
	 * @param  projName  Project Name.  
	 */
	bool openProject(QString fileName = QString());

	/**
	 * @brief  Save the current project.
	 */
	void saveProject();

	/**
	 * @brief  Save the current project as another project and switch to that project.
	 */
	void saveProjectAs();

	/**
	 * @brief  Close current project.
	 * @return True if it is closed, false the close is canceled.
	 */
	bool closeProject();

	/**
	 * @brief  Import another project into current project by copying and loading all the objects.
	 */
	void importProject();

	/**
	 * @brief  Import a CHE into current project by copying and loading all the objects.
	 */
	void importCHE();

	/**
	 * @brief  Merge all the objects in current project to their orignal CHEs.
	 */
	void mergeProjectToCHE();

	/**
	 * @brief  Handle new CHE button and create newCHEDialog.
	 */
	void newCHE();

	/**
	 * @brief  Create the new CHE based on the information in newCHEDialog
	 * @param  Project Full Path, Project Name, User Mode,  CHE text info, User name, Imported Object Full Path, 
		       Imported CT Full Path.
	 */
	void createNewCHE(const QString fullName, const QString name, const USERMODE mode, const CHEInfoBasic* info,
	  const QString userName, const QString object, const QString ct);

	/**
	 * @brief  Open an existing project
	 * @param  projName  Project Name.  
	 */
	bool openCHE(QString fileName = QString());

	/**
	 * @brief  Save the current CHE.
	 */
	void saveCHE();

	/**
	 * @brief  Save the current CHE as another CHE and switch to that CHE.
	 */
	void saveCHEAs();

	/**
	 * @brief  Close current CHE.
	 * @return True if it is closed, false the close is canceled.
	 */
	bool closeCHE();

	/**
	 * @brief  Export current CHE to a new project.
	 */
	void exportCHEToProject();

	/**
	 * @brief  Functions related to corresponding QActions.
	 */
	void help();
	void about();
	void viewOptions();
	void switchLayoutDirection();
	void setActiveSubWindow(QWidget *window); // old
	void wrapSetActiveSubWindow(QWidget* window); //Solt Wrapper for QMdiArea
	void closeAllWindows();
	void closeWindow();
	void openWindow();
	bool closeAll();

	/**
	 * Create a new window for the object.
	 */
	VtkWidget* newImage();

	/**
	 * @brief  Functions related to corresponding QActions.
	 */
	void helpOnline();
	void showToolbarFile();
	void showToolbarRender();
	void showToolbarInfo();
	void showToolbarView();

	/**
	 * @brief  Update window menu.
	 */
	void updateWindowMenu();

	/**
	 * @brief  Update project/CHE.xml file.
	 */
	void updateXML();

	/**
	 * @brief  Set the split window.
	 */
	void setSplit(QAction *qa);

	/**
	 * @brief  Set unsplit window.
	 */
	void setUnsplit();

	void setCustomize() { return; }

	void splitFromHandle(QAction * qa);
	void unsplitFromHandle(QAction * qa);

	/**
	 * @brief  Render 3D object in different mode.
	 */
	void renderPoints()         { VTKA()->setRenderMode3D(POINTS3D); }
	void renderWireframe()      { VTKA()->setRenderMode3D(WIREFRAME3D); }
	void renderSurface()        { VTKA()->setRenderMode3D(SURFACE3D); }

	/**
	 * @brief  Set the subwindow maximized.
	 */
	void maximizeSubWindows()   {VTKA()->mvc()->showMaximized();}

	/**
	 * @brief  Render 3D object with texture.
	 */
	void renderTexture();

	/**
	 * @brief  Interpolate 3D object.
	 */
	void renderInterpolation();

	/**
	 * @brief  Show information on the left bottom corner.
	 */
	void renderInformation();

	/**
	 * @brief  Highlight the selected polygon. It is currently not being used.
	 */
	void renderPolyIndicate();

	/**
	 * @brief  Set directional light for 3D object.
	 */
	void setDirectionalLight();

	/**
	 * @brief  Enable the measurement for 3D object.
	 */
	void measureDistance();

	/**
	 * @brief  Remove the measurement.
	 */
	void removeMeasureDistance();

	/**
	 * @brief  View 3D object in different directions.
	 */
	void viewFromFront() {VTKA()->setOrthogonalView(FRONT3D);}
	void viewFromTop() {VTKA()->setOrthogonalView(TOP3D);}
	void viewFromLeft() {VTKA()->setOrthogonalView(LEFT3D);}
	void viewFromRight() {VTKA()->setOrthogonalView(RIGHT3D);}
	void viewFromBottom() {VTKA()->setOrthogonalView(BOTTOM3D);}
	void viewFromBack() {VTKA()->setOrthogonalView(BACK3D);}
	void viewSpinMotion() { VTKA()->launchSpinView(); }

	/**
	 * @brief  Generate report. Report can be presented in pdf or html.
	 */
	void generateReport();

	/**
	 * @brief  Generate video.
	 */
	void generateVideo();

	/**
	 * @brief  Enable annotation.
	 */
	void writeAnnotation();

	/**
	 * @brief  Choose different note types. Polygon note only works for 2D. Frustum note only works for 3D.
	 */
	void writePointNote();
	void writeSurfaceNote();
	void writePolygonNote();
	void writeFrustumNote();

	/**
	 * @brief  Set different colors for note.
	 */
	void setAnnotationColorRed() {VTKA()->setAnnotationColor(RED);}
	void setAnnotationColorMarron() {VTKA()->setAnnotationColor(MAROON);}
	void setAnnotationColorOrange() {VTKA()->setAnnotationColor(ORANGE);}
	void setAnnotationColorYellow() {VTKA()->setAnnotationColor(YELLOW);}
	void setAnnotationColorLime() {VTKA()->setAnnotationColor(LIME);}
	void setAnnotationColorGreen() {VTKA()->setAnnotationColor(GREEN);}
	void setAnnotationColorAqua() {VTKA()->setAnnotationColor(AQUA);}
	void setAnnotationColorBlue() {VTKA()->setAnnotationColor(BLUE);}
	void setAnnotationColorPink() {VTKA()->setAnnotationColor(PINK);}
	void setAnnotationColorPurple() {VTKA()->setAnnotationColor(PURPLE);}
	void setAnnotationColorWhite() {VTKA()->setAnnotationColor(WHITE);}

	/**
	 * @brief  Remove all the notes and annotation for object in current active window.
	 */
	void removeAnnotation();

	/**
	 * @brief  Filter notes based on users. Only the notes with selected users will be shown.
	 */
	void filterAnnotation();

	/**
	 * @brief  Take screenshot for the object in current window.
	 */
	void takeScreenshot();

	/**
	 * @brief  Create bookmark.
	 */
	void makeBookmark();

	/**
	 * @brief  Flatten the mesh.
	 */
	void flattenMesh();

	/**
	 * @brief  Choose different camera mode. These functions are not being used.
	 */
	void setCameraModeTrackball();
	void setCameraModeSurfaceWalker();
	void toggleImageProvenanceFeature();

private:
	QWidget * mountWidgetCenter(QWidget *inputWidget);
	QMdiArea *mdiArea;
	QBuffer myLocalBuf;
	QByteArray toolbarState;
	QString currentProjectKeyword;
	QString currentProjectAffiliation;
	QString currentProjectDescription;
	QString currentProjectName;
	QString currentProjectFullName;
	QString currentProjectSave;
	QDomDocument currentProjectMetadata;
	NewProjectDialog *mNewProjectDialog;
	ProjectInfoDialog *mProjectInfoDialog;
	bool isClose;
	bool isCHE;
	Navigation* mNavigation;
	ProjectClassifiedInfoTab * mClassifiedInfoTab;
	CHETab* mCHETab;
	QVector<QString> mObjectList;	// save objects' names in the project/CHE
	QSignalMapper *windowMapper;

	//---------------------------------------------
	// QMenus:
	QMenuBar *menuBar;
	QMenu *fileMenu;
	QMenu *CHEMenu;
	QMenu *editMenu;
	QMenu *windowMenu;
	QMenu *newMenu;
	QMenu *openMenu;
	QMenu *recentProjMenu;
	QMenu *recentCHEMenu;
	QMenu *recentFileMenu;
	QMenu *renderMenu;
	QMenu *renderModeMenu;
	QMenu *lightingModeMenu;
	QMenu *viewMenu;
	QMenu *toolBarMenu;
	QMenu *windowsMenu;
	QMenu *toolsMenu;
	QMenu *helpMenu;
	QMenu *splitModeMenu;
	QMenu *viewFromMenu;
	QMenu *annotationModeMenu;
	QMenu *annotationColorMenu;
	QToolButton *colorToolButton;
	QMenu *colorDropMenu;
	QMenu *handleMenu;
	QMenu *splitMenu;
	QMenu *unSplitMenu;
	QMenu *cameraModeMenu;

	//---------------------------------------------
	// QTabs:
	QTabWidget *tabWidgetTop;
	QTabWidget *tabWidgetMid;
	QTabWidget *tabWidgetBottom;
	QTabWidget* rightTab;

	//---------------------------------------------
	// QToolbars
	QToolBar *mainToolBar;
	QToolBar *renderToolBar;
	QToolBar *infoToolBar;
	QToolBar *measureToolBar;
	QToolBar *annotationToolBar;
	QToolBar *windowToolBar;
	QToolBar *viewToolBar;

	//---------------------------------------------
	// QActions:
	QAction *exitAct;
	QAction *helpAct;
	QAction *aboutAct;
	QAction *viewAct;
	QAction *setCustomizeAct;
	QAction *openObjectAct;
	QAction *openDICOMAct;
	QAction *removeObjectAct;
	QAction *saveAct;
	QAction *saveAsAct;
	QAction *closeAct;
	QAction *newVtkProjectAct;
	QAction *openProjectAct;
	QAction *importProjectAct;
	QAction *importCHEAct;
	QAction *mergeProjectToCHEAct;
	QAction *recentProjActs[MAXRECENTFILES];
	QAction *recentCHEActs[MAXRECENTFILES];
	QAction *recentFileActs[MAXRECENTFILES];
	QAction *newCHEAct;
	QAction *openCHEAct;
	QActionGroup *renderModeGroupAct;
	QAction *renderModePointsAct;
	QAction *renderModeWireAct;
	QAction *renderModeFlatAct;
	QAction *renderModeTextureAct;
	QAction *renderModeInterpolationAct;
	QAction *renderModeInfoAct;
	QAction *setDirectionalLightAct;
	QAction *showPolyIndicateAct;
	QAction *fullScreenAct;
	QAction *showToolbarStandardAct;
	QAction *showToolbarRenderAct;
	QAction *showToolbarInfoAct;
	QAction *showToolbarViewAct;
	QAction *windowsTileAct;
	QAction *windowsCascadeAct;
	QAction *windowsNextAct;
	QAction *windowsMaximizeAct;
	QAction *closeAllAct;
	QAction *closeWindowAct;
	QAction *openWindowAct;

	QAction *zoomResetAct;
	QAction *zoomInAct;
	QAction *zoomOutAct;
	QAction *changeBGColorAct;

	QAction *setSplitHAct;
	QAction *setSplitVAct;
	QActionGroup *setSplitGroupAct;
	QAction *setUnsplitAct;
	QActionGroup *splitGroupAct;
	QActionGroup *unsplitGroupAct;
	QAction *splitUpAct;
	QAction *splitDownAct;
	QAction *unsplitUpAct;
	QAction *unsplitDownAct;
	QAction *splitRightAct;
	QAction *splitLeftAct;
	QAction *unsplitRightAct;
	QAction *unsplitLeftAct;
	QActionGroup *viewFromGroupAct;
	QAction *viewTopAct;
	QAction *viewBottomAct;
	QAction *viewLeftAct;
	QAction *viewRightAct;
	QAction *viewFrontAct;
	QAction *viewBackAct;
	QAction *viewSpinAct;
	QAction *generateReportAct;
	QAction *generateVideoAct;
	QAction *measureDistanceAct;
	QAction *removeDistanceAct;
	QAction *writeAnnotationAct;
	QActionGroup *annotationModeGroupAct;
	QAction *pointNote;
	QAction *surfaceNote;
	QAction *polygonNote;
	QAction *frustumNote;
	QActionGroup *annotationColorGroupAct;
	QAction *annotationMaroon;
	QAction *annotationRed;
	QAction *annotationOrange;
	QAction *annotationYellow;
	QAction *annotationLime;
	QAction *annotationGreen;
	QAction *annotationAqua;
	QAction *annotationBlue;
	QAction *annotationPink;
	QAction *annotationPurple;
	QAction *annotationWhite;
	QAction *removeAnnotationAct;
	QAction *filterAnnotationAct;
	QAction *screenshotAct;
	QAction *bookmarkAct;
	QAction *fileInfoAct;
	QAction *projectInfoAct;
	QAction *onlineHelpAct;

	QAction *setTrackballCameraAct;
	QAction *setSurfaceWalkerCameraAct;
	QAction *showImageProvenanceAct;
	QShortcut *flattenShortcut;
};

#endif
