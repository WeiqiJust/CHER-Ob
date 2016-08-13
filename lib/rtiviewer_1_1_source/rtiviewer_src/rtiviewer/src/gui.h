/****************************************************************************
* RTIViewer                                                         o o     *
* Single and Multi-View Reflectance Transformation Image Viewer   o     o   *
*                                                                _   O  _   *
* Copyright	(C) 2008-2010                                          \/)\/    *
* Visual Computing Lab - ISTI CNR					              /\/|      *
* and											                     |      *
* Cultural Heritage Imaging							                 \      *
*																			*
* This program is free software: you can redistribute it and/or modify		*
* it under the terms of the GNU General Public License as published by		*
* the Free Software Foundation, either version 3 of the License, or			*
* (at your option) any later version.										*
*																			*
* This program is distributed in the hope that it will be useful,			*
* but WITHOUT ANY WARRANTY; without even the implied warranty of			*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				*
* GNU General Public License for more details.								*
*																			*
* You should have received a copy of the GNU General Public License			*
* along with this program.  If not, see <http://www.gnu.org/licenses/>.		*
****************************************************************************/


#ifndef RTIVIEWERDLG_H
#define RTIVIEWERDLG_H

// Local headers
#include "rtiBrowser.h"
#include "bookmarkcontrol.h"
#include "lightControl.h"
#include "navigator.h"
#include "renderingdialog.h"
#include "rti.h"
#include "ptm.h"
#include "hsh.h"
#include "universalrti.h"
#include "multiviewrti.h"
#include "httpthread.h"

// Qt headers
#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <QFrame>
#include <QMutex>
#include <QWaitCondition>
#include <QSettings>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGroupBox>
#include <QDoubleSpinBox>

#include <QDesktopServices>
#include <QUrl>

// RtiViewerDlg points to BookmarkControl and RtiBrowser, both of which
// point to RtiViewerDlg. Forward-declare BookmarkControl and RtiBrowser
// to resolve the circular dependencies.

class BookmarkControl;
class RtiBrowser;

/*====================================================================
 *
 * RtiViewerDlg class header
 *
 *===================================================================*/

//! Main window class.
/*!
  The class defines and manages the main window.
*/
class RtiViewerDlg : public QWidget
{
	Q_OBJECT

    /*====================================================================
     * Private data members
     *===================================================================*/

private:
	
	RtiBrowser* browser; /*!< Browser for RTI image. */
	LightControl* light; /*!< Widget to set the light direction. */
    BookmarkControl * bookmarkControl; /*!< Widget to manage bookmarks. */

	Navigator* navigator; /*!< Widget to navigate the image. */
		
	QToolBar* toolBar; /*!< Toolbar. */
	QAction* openLocalAct; /*!< Action to open file from local disk. */
	QAction* openRemoteAct; /*!< Action to open file from remote disk. */
	QAction* aboutAct; /*!< Action to show about info. */
	QAction* configAct; /*!< Action to open application settings window. */
	QAction* snapshotAct; /*!< Action to save a snapshot of current RTI image. */
	
	RenderingDialog* rendDlg; /*!< Widget to choose the rendering mode to apply to RTI image. */

	QLineEdit* filename; /*!< Read-only text box to show the file-path of RTI image. */
	QLineEdit* filesize; /*!< Read-only text box to show the size of RTI image. */
	QLineEdit* fileformat; /*!< Read-only text box to show the format of RTI image. */
	QSpinBox* zoomFact; /*!< Control to set the zoom factor. */
    QDoubleSpinBox* xLightSpinner; /*!< Control to set the x coordinate of the light. */
    QDoubleSpinBox* yLightSpinner; /*!< Control to set the y coordinate of the light. */
    QDoubleSpinBox* xPanSpinner; /*!< Control to set the x coordinate of the pan. */
    QDoubleSpinBox* yPanSpinner; /*!< Control to set the y coordinate of the pan. */

	HttpThread* getter; /*!< Secondary thread to get the RTI image from a remote server. */
	QMutex* mutex; /*!< Mutex to provide a mutual exclusion lock between the GUI thread and the HTTP thread. */
	QWaitCondition* infoReady; /*!< Wait condition to synchronize the GUI thread and the HTTP thread. */
	
	QSettings* settings; /*!< Application settings. */

	QDir dir; /*!< Current working directory. */

	QUrl lastUrl; /*!< Last URL inserted by the user. */

	const QString title;
	const QString filterStr;
	const int maxZoom;


    /*====================================================================
     * Public methods
     *===================================================================*/

public:

	//! Constructor
	/*!
	  Constructs the main windows with parent \a parent. 
	  \param parent parent window.
	*/
	RtiViewerDlg(QWidget *parent=0);

    //! Open an image file
    /*!
      Opens the image file specified by \a path.
      \path Full path of the image file
    */
    int openFile(QString path);

    //! Get the zoom value
    /*!
      Get the current value of the zoom as an integer.
      \return The zoom value
    */
    int getZoom();

    /*!
      Get the full path of the currently open RTI file
      \return The file path
    */

    QFileInfo getRTIFileInfo();


    /*====================================================================
     * Protected methods
     *===================================================================*/

protected:

    void dragEnterEvent ( QDragEnterEvent * event )
    {
        if (event->mimeData()->hasUrls())
            event->acceptProposedAction();
    }

    void dropEvent ( QDropEvent * event )
    {
        if (event->mimeData()->hasUrls())
        {
            QString path = (event->mimeData()->urls()).at(0).path();
            path.remove(0, 1);
            openFile(path);
        }
    }

    /*====================================================================
     * Private methods
     *===================================================================*/

private:

    QFrame * initBrowserFrame();

    QGridLayout* initToolbarAndLight();

    void initToolbar();

    void initLight();

    QGroupBox *initLightGroup();

    QGroupBox *initZoomGroup();

    QGroupBox *initPanGroup();

    QVBoxLayout* initZoomLayout(QGroupBox * lightGroupBox, QGroupBox * zoomGroupBox, QGroupBox * panGroupBox);

    QGridLayout * initToolbarAndLightLayout(QVBoxLayout * xyzoomLayout);

    QGroupBox * initRendGroup();

    QTabWidget * initTabWidget();

    QFrame * initFileFrame();

    QFrame *initNavigatorFrame();

    QFrame *initFileNavFrame(QFrame * navigatorFrame, QFrame * fileFrame);

    void initBookmarkControl();

    QTabWidget *initTabs(QFrame * fileNavFrame);

    QGridLayout * initMainLayout();

    void initHttpThread();

    void initAppSettings();

    /*!
	  Parses xml file used in the visualization of remote RTI image.
	  \param b pointer to file buffer
	  \return a pointer to a RTI image.
	*/
	Rti* parseXml(const QBuffer* b);

    void spinnerChanged(QDoubleSpinBox * spinner1, QDoubleSpinBox * spinner2, float value, bool xChanged);

    /*====================================================================
     * Private slots
     *===================================================================*/

private slots:

	/*!
	  Manages error of HTTP thread.
	  \param error description of error
	*/
	void httpErrorOccurred(QString error);

    /*====================================================================
     * Public slots
     *===================================================================*/

public slots:

	/*!
	  Manages the opening of a file from the local disk. 
	  \return -1 if the opening fails, 0 otherwise
	*/
	int open();

	/*!
	  Manages the opening of a file from a remote disk.
      \return -1 if the opening fails, 0 otherwise
	*/
	int openRemote();

	/*!
      Shows the About dialog.
	*/
	void about();

	/*!
	  Shows and manages the application settings.
	*/
	void configure();

    /*!
      Set the zoom value.
    */
    void setZoomValue(float value, float minValue);

    /*!
      Set the zoom value.
    */
    void setZoomValue(int value);

    /*!
      Set the light spin boxes.
     */
    void setLightSpinBoxes(vcg::Point3f light);

    /*!
      Update the interface after the X light spin box changes.
     */
    void xLightSpinnerChanged(double x);

    /*!
      Update the interface after the Y light spin box changes.
     */
    void yLightSpinnerChanged(double y);

    /*!
      Update the interface after the view changes.
     */
    void viewChanged(QRectF subimg);

};


/*====================================================================
 *
 * FileOpenEater class header
 *
 *===================================================================*/

class FileOpenEater : public QObject
{
	Q_OBJECT

public:
	FileOpenEater(RtiViewerDlg *_mainWindow)
	{
		mainWindow= _mainWindow;
		noEvent=true;
	}

	RtiViewerDlg *mainWindow;
	bool noEvent;

protected:

	bool eventFilter(QObject *obj, QEvent *event)
	{
		if (event->type() == QEvent::FileOpen)
		{
			noEvent=false;
			QFileOpenEvent *fileEvent = static_cast<QFileOpenEvent*>(event);
			mainWindow->openFile(fileEvent->file());
			return true;
		} 
		else
			return QObject::eventFilter(obj, event);
	}
};


/*====================================================================
 *
 * AboutDlg class header
 * NOTE: This has been moved to a separate file
 *
 *===================================================================*/

/*
class AboutDlg: public QDialog
{
	Q_OBJECT
public:
	AboutDlg(QWidget *parent = 0) : QDialog(parent)
	{
	}

public slots:

	void showManual(QString file)
	{
		QString currentDir = QDir::currentPath();
		QString path = tr("file:///%1/%2").arg(currentDir).arg(file);
		QDesktopServices::openUrl(QUrl(path));
	}

};
*/

#endif /* RTIVIEWERDLG_H */

