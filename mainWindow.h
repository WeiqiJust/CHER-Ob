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
#include "vtkWidget.h" //MK: The inclusion of this header should be only here, except implementations.
#include "dialog.h"

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

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();
  const QString appName() const {return tr("Hyper3D")+appVer(); }
  const QString appVer() const {return tr(" 1.5.0"); }
  const QString appBits() const {return tr(BITS); }

  void getProjectInfo(QString& projectName, QString& location, QString& keyword, QString& affiliation, QString& userName, QString& description);
  void setHandleMenu(QPoint p, Qt::Orientation o, QSplitter *origin);

  static bool QCallBack(const int pos, const char * str);
  static QProgressBar *qb;

  inline VtkView* currentVtkView() const {
    if(mdiArea->currentSubWindow()==0) return 0;
    VtkView *mvc = qobject_cast<VtkView *>(mdiArea->currentSubWindow());
    if(!mvc){
      mvc = qobject_cast<VtkView *>(mdiArea->currentSubWindow()->widget());
      return mvc;
    }
    else return 0;
  }

  inline VtkWidget *VTKA() const {
    if(mdiArea->currentSubWindow()==0) return 0;
    VtkView *mvc = currentVtkView();
    if(!mvc) return 0;
    VtkWidget *glw =  qobject_cast<VtkWidget*>(mvc->currentView());
    if(!glw) return 0;
    return glw;
  }

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
			}
		}
  }

  static QStatusBar *&globalStatusBar()
  {
    static QStatusBar *_qsb=0;
    return _qsb;
  }

  inline void createDataLocation(QString path) {
      QStringList tokens = path.split(QDir::separator());
      QString hyper3D = tokens.takeLast();
      path = tokens.join(QDir::separator());
      path = QDir::toNativeSeparators(path);
      if(!QDir(path).exists()) {
          QDir().mkdir(path);
      }
      path.append(QDir::separator() + hyper3D);
      if(!QDir(path).exists()) {
          QDir().mkdir(path);
      }
  }

  QTabWidget* getTabWidgetTop() {return tabWidgetTop;}
  QTabWidget* getSearchTabWidget();
  void activateTabWidgetTop(int index)
  {
	  if (tabWidgetTop)
		  tabWidgetTop->setCurrentIndex(index);
  }

  friend bool BookmarkWidget::viewBookmark(QTreeWidgetItem* item);
  friend Metadata::Metadata(QString path, VtkWidget *w, bool file);

  CTControl * mCtControl;
  PlotView * mPlotView;
  LightControl *mLightControl;
  LightControlRTI *mLightControlRTI;  //YY
  RenderingDialog* rendDlg; //YY /*!< Widget to choose the rendering mode to apply to RTI image. */
  Information *mInformation;
  BookmarkWidget *mBookmark;
  SearchWidget *mSearch;
  SearchAllWidget *mSearchAll;
  QTextEdit *mInfoText;
  QDir lastUsedDirectory;
  QDir lastSavedDirectory; // DT: Keeps track of screenshot/project saved location
  QString mUserName;
  bool unsavedChanges;

public slots:
  void updateAllViews();
  bool openImages(const QString& fileNameStart = QString(), bool saveRecent = true); // open 2/3D image
  bool openDICOM(const QString& fileNameStart = QString(), bool saveRecent = true); // open DICOM folder
  void updateMenus(); // update toolbars
  void showFileInfo();
  void showProjectInfo();
  void projectInfoChanged(const QString userName, const QString keyword, const QString affiliation, const QString description);

 signals:
  void currentWidgetModeChanged(WidgetMode widgetmode);

protected:
  void closeEvent(QCloseEvent *event);

private:
  bool eventFilter(QObject* obj, QEvent* e);
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void createDockWindows();

  void readSettings();
  void writeSettings();
  void saveRecentFileList(const QString &fileName);
  void updateRecentFileActions();
  void saveRecentProjectList(const QString &projName);
  void updateRecentProjActions();
  bool cpDir(const QString &srcPath, const QString &dstPath);
  bool rmDir(const QString &dirPath);
  void createCTFolder(QString path);
  void createObjectFolder(QString path);
  void updateXML();
  bool readXML(QString fileName, QVector<QString> &objectList, bool import);

private slots:
  void zoomIn();
  void zoomOut();
  void zoomReset();
  void save();
  void saveAs();
  void help();
  void about();
  void viewOptions();
  void switchLayoutDirection();
  void setActiveSubWindow(QWidget *window); // old
  void wrapSetActiveSubWindow(QWidget* window); //Solt Wrapper for QMdiArea
  void closeAllWindows();
  void closeWindow();
  void openWindow();

  void closeAll();
  VtkWidget* newImage();
  void newVtkProject(const QString& projName = QString());// project
  void createNewVtkProject(const QString fullName, const QString name, const USERMODE mode, const QString userName, 
	  const QString object, const QString ct, const QString keyword, const QString affiliation, const QString description);
  bool openProject(QString fileName=QString());
  void saveProject();
  void saveProjectAs();

  bool closeProject();
  void importProject();
  void removeObject();
  void openRecentFile();
  void openRecentProj();
  void helpOnline();
  void showToolbarFile();
  void showToolbarRender();
  void showToolbarInfo();
  void showToolbarView();

  void updateWindowMenu();

  void setSplit(QAction *qa);
  void setUnsplit();
  void setCustomize() { return; }
  void showPolyIndicator() { return; }

  void splitFromHandle(QAction * qa);
  void unsplitFromHandle(QAction * qa);

  void renderPoints()         { VTKA()->setRenderMode3D(POINTS3D); }
  void renderWireframe()      { VTKA()->setRenderMode3D(WIREFRAME3D); }
  void renderSurface()        { VTKA()->setRenderMode3D(SURFACE3D); }
  void maximizeSubWindows()  {VTKA()->mvc()->showMaximized();}
  void renderTexture();
  void renderInterpolation();
  void renderInformation();
  void renderPolyIndicate();
  void setDirectionalLight();

  void measureDistance();
  void removeMeasureDistance();

  void viewFromFront() {VTKA()->setOrthogonalView(FRONT3D);}
  void viewFromTop() {VTKA()->setOrthogonalView(TOP3D);}
  void viewFromLeft() {VTKA()->setOrthogonalView(LEFT3D);}
  void viewFromRight() {VTKA()->setOrthogonalView(RIGHT3D);}
  void viewFromBottom() {VTKA()->setOrthogonalView(BOTTOM3D);}
  void viewFromBack() {VTKA()->setOrthogonalView(BACK3D);}
  void viewSpinMotion() { VTKA()->launchSpinView(); }

  void writeAnnotation();
  void writePointNote() {VTKA()->annotate(true, POINTNOTE); writeAnnotationAct->setChecked(true);}
  void writeSurfaceNote() {VTKA()->annotate(true, SURFACENOTE); writeAnnotationAct->setChecked(true);}
  void writeFrustumNote() {VTKA()->annotate(true, FRUSTUMNOTE); writeAnnotationAct->setChecked(true);}
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
  void removeAnnotation();

  void takeScreenshot();
  void makeBookmark();

  void flattenMesh();
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

  QSignalMapper *windowMapper;

  //---------------------------------------------
  // QMenus:
  QMenuBar *menuBar;
  QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *windowMenu;
  QMenu* recentProjMenu;
  QMenu* recentFileMenu;
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
  QMenu *handleMenu;
  QMenu *splitMenu;
  QMenu *unSplitMenu;
  QMenu *cameraModeMenu;

  //---------------------------------------------
  // QTabs:
  QTabWidget* tabWidgetTop;
  QTabWidget* tabWidgetMid;
  QTabWidget* tabWidgetBottom;

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
  QAction *newVtkProjectAct;
  QAction *openObjectAct;
  QAction *openDICOMAct;
  QAction *removeObjectAct;
  QAction *openProjectAct,*saveProjectAct,*saveProjectAsAct, *importProjectAct;
  QAction *closeProjectAct;
  QAction *recentProjActs[MAXRECENTFILES];
  QAction *recentFileActs[MAXRECENTFILES];
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
  QAction *measureDistanceAct;
  QAction *removeDistanceAct;
  QAction *writeAnnotationAct;
  QActionGroup *annotationModeGroupAct;
  QAction *pointNote;
  QAction *surfaceNote;
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
