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


// Local headers
#include "gui.h"
#include "aboutdlg.h"
//#include "ui_about.h"
#include "configdlg.h"
#include "loadingdlg.h"
#include "openremotedlg.h"
#include "renderingmode.h"

// Qt headers
#include <QMessageBox>
#include <QMapIterator>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QSpacerItem>

#include <QDebug>

#include <vcg/space/point3.h>

#include <cmath>

#include <new>

/*====================================================================
 *
 * RtiViewerDlg class
 *
 *===================================================================*/

/*====================================================================
 *
 * Constructor and related methods
 *
 *===================================================================*/

RtiViewerDlg::RtiViewerDlg(QWidget *parent/*=0*/):
    QWidget(parent),
    rendDlg(NULL),
    title("RTIViewer 1.1.0"),
    filterStr("All (*.ptm *.hsh *.rti *.mview);;Polynomial Texture Maps (*.ptm);; Hemispherical Harmonics Map (*.hsh);; Universal RTI (*.rti);; Multiview RTI (*.mview)"),
    maxZoom(2)
{
    // Initialize all of the viewer widgets

    QFrame * browserFrame = initBrowserFrame();
    QGridLayout* toolbarAndLight = initToolbarAndLight();
    QGroupBox* renderingGroup = initRendGroup();
    QTabWidget * tabWidget = initTabWidget();

    // Lay out all of the widgets on the main window

    QSpacerItem* spacer1 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Maximum);
    QSpacerItem* spacer2 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Maximum);

    QGridLayout * layout = initMainLayout();
    layout->addWidget(browserFrame, 0, 0, 5, 1);
    layout->addLayout(toolbarAndLight, 0, 1, Qt::AlignTop | Qt::AlignHCenter);
    layout->addItem(spacer1, 1, 1, 1, 1);
    layout->addWidget(renderingGroup, 2, 1, Qt::AlignCenter);
    layout->addItem(spacer2, 3, 1, 1, 1);
    layout->addWidget(tabWidget, 4, 1, Qt::AlignBottom | Qt::AlignHCenter);
	
    setLayout(layout);

    // Set the main window attributes

    setWindowState(Qt::WindowMaximized);
    setWindowTitle(title);
    setAcceptDrops(true);

    // Initialize the Http thread and the application settings

    initHttpThread();
    initAppSettings();

}

/*====================================================================
 * Private methods to construct the GUI
 *===================================================================*/

QFrame * RtiViewerDlg::initBrowserFrame()
{
    // Create a new browser

	QGLFormat openGLFormat;
	openGLFormat.setVersion(2, 1);

    browser = new RtiBrowser(650, 650, NULL, maxZoom, this, openGLFormat);

    // Create the browser layout.

    QHBoxLayout* browserLayout = new QHBoxLayout;
    browserLayout->setContentsMargins(0, 0, 0, 0);
    browserLayout->addWidget(browser);

    // Create the browser frame

    QFrame * browserFrame = new QFrame();
    browserFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    browserFrame->setLineWidth(3);
    browserFrame->setLayout(browserLayout);

    // Return the browser frame

    return browserFrame;
}

QGridLayout* RtiViewerDlg::initToolbarAndLight()
{
    // Initialize the toolbar, light, and x, y, and zoom spin boxes

    initToolbar();
    initLight();
    QGroupBox * lightGroupBox = initLightGroup();
    QGroupBox * zoomGroupBox = initZoomGroup();
    QGroupBox * panGroupBox = initPanGroup();

    // Lay everything out

    QVBoxLayout * zoomLayout = initZoomLayout(lightGroupBox, zoomGroupBox, panGroupBox);
    QGridLayout * toolbarAndLight = initToolbarAndLightLayout(zoomLayout);

    // Return the layout

    return toolbarAndLight;
}

void RtiViewerDlg::initToolbar()
{
    // Create the toolbar actions

    openLocalAct = new QAction(QIcon(":/images/openlocal.png"),tr("&Open..."), this);
    openRemoteAct = new QAction(QIcon(":/images/openremote.png"), tr("Open remote file"), this);
    aboutAct = new QAction(QIcon(":images/info.png"), tr("Info"), this);
    configAct = new QAction(QIcon(":images/config.png"), tr("Configuration"), this);
    snapshotAct = new QAction(QIcon(":images/snapshot.png"), tr("Snapshot"), this);

    // Create the toolbar

    toolBar = new QToolBar(this);
    toolBar ->setOrientation(Qt::Vertical);
    toolBar->setIconSize(QSize(48,48));

    // Add the actions to the toolbar and connect them to their slots

    toolBar->addAction(openLocalAct);
    connect(openLocalAct, SIGNAL(triggered()), this, SLOT(open()));
    toolBar->addAction(openRemoteAct);
    connect(openRemoteAct, SIGNAL(triggered()), this, SLOT(openRemote()));
    toolBar->addAction(snapshotAct);
    connect(snapshotAct, SIGNAL(triggered()), browser, SLOT(snapshotActivated()));
    toolBar->addAction(configAct);
    connect(configAct, SIGNAL(triggered()), this, SLOT(configure()));
    toolBar->addAction(aboutAct);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void RtiViewerDlg::initLight()
{
    // Create the light control and connect it to the browser

    light = new LightControl(this, 180);

    connect(light, SIGNAL(lightChanged(vcg::Point3f, bool)), browser, SLOT(setLight(vcg::Point3f, bool)));
    connect(light, SIGNAL(lightChanged(vcg::Point3f, bool)), this, SLOT(setLightSpinBoxes(vcg::Point3f)));
    connect(browser, SIGNAL(setInteractiveLight(bool)), light, SLOT(setInteractive(bool)));
    connect(browser, SIGNAL(setEnabledLight(bool)), light, SLOT(setEnabled(bool)));
    connect(browser, SIGNAL(moveLight(float, float)), light, SLOT(moveLightPosition(float, float)));
    connect(browser, SIGNAL(setLightDir(const vcg::Point3f&, bool)), light, SLOT(setLight(const vcg::Point3f&, bool)));
}

QGroupBox * RtiViewerDlg::initLightGroup()
{
    // Create the x light spin box

    xLightSpinner = new QDoubleSpinBox();
    xLightSpinner->setDecimals(2);
    xLightSpinner->setSingleStep(0.01);
    xLightSpinner->setRange(-1.0, 1.0);
    xLightSpinner->setKeyboardTracking(false);
    xLightSpinner->setEnabled(false);

    // Create the y light spin box

    yLightSpinner = new QDoubleSpinBox();
    yLightSpinner->setDecimals(2);
    yLightSpinner->setSingleStep(0.01);
    yLightSpinner->setRange(-1.0, 1.0);
    yLightSpinner->setKeyboardTracking(false);
    yLightSpinner->setEnabled(false);

    // Connect the x and y spin boxes and the slots in this class

    connect(xLightSpinner, SIGNAL(valueChanged(double)), this, SLOT(xLightSpinnerChanged(double)));
    connect(yLightSpinner, SIGNAL(valueChanged(double)), this, SLOT(yLightSpinnerChanged(double)));

    // Lay out the spinners

    QGridLayout * lightSpinLayout = new QGridLayout();
    QLabel* xLabel = new QLabel("x");
    QLabel* yLabel = new QLabel("y");
    lightSpinLayout->setContentsMargins(6, 18, 6, 6);
    lightSpinLayout->addWidget(xLabel, 0, 0);
    lightSpinLayout->addWidget(xLightSpinner, 0, 1);
    lightSpinLayout->addWidget(yLabel, 1, 0);
    lightSpinLayout->addWidget(yLightSpinner, 1, 1);

    // Create and return the light group box.

    QGroupBox * lightSpinGroup = new QGroupBox("Light");
    lightSpinGroup->setLayout(lightSpinLayout);
    lightSpinGroup->setFixedWidth(90);
    lightSpinGroup->setContentsMargins(2, 2, 2, 2);
    return lightSpinGroup;
}

QGroupBox * RtiViewerDlg::initZoomGroup()
{
    // Create the zoom spin box

    zoomFact = new QSpinBox();
    zoomFact->setRange(1, maxZoom*100);
    zoomFact->setSuffix("%");
    zoomFact->setValue(100);
    zoomFact->setKeyboardTracking(false);
    zoomFact->setEnabled(false);

    // Connect the zoom spin box, the browser, and the slots this class

    connect(browser, SIGNAL(updateZoomValue(float, float)), this, SLOT(setZoomValue(float, float)));
    connect(zoomFact, SIGNAL(valueChanged(int)), browser, SLOT(updateZoom(int)));

    // Create and return the zoom group

    QVBoxLayout * zoomLayout = new QVBoxLayout();
    zoomLayout->setContentsMargins(6, 18, 6, 6);
    zoomLayout->addWidget(zoomFact, 0, Qt::AlignHCenter);
    QGroupBox * zoomGroup = new QGroupBox("Zoom");
    zoomGroup->setContentsMargins(2, 2, 2, 2);
    zoomGroup->setFixedWidth(90);
    zoomGroup->setLayout(zoomLayout);
    return zoomGroup;
}

QGroupBox * RtiViewerDlg::initPanGroup()
{
    // Create the x pan spin box

    xPanSpinner = new QDoubleSpinBox();
    xPanSpinner->setDecimals(0);
    xPanSpinner->setSingleStep(1);
    xPanSpinner->setMinimum(0.0);
    xPanSpinner->setKeyboardTracking(false);
    xPanSpinner->setEnabled(false);

    // Create the y pan spin box

    yPanSpinner = new QDoubleSpinBox();
    yPanSpinner->setDecimals(0);
    yPanSpinner->setSingleStep(1);
    yPanSpinner->setMinimum(0.0);
    yPanSpinner->setKeyboardTracking(false);
    yPanSpinner->setEnabled(false);

    // Connect the x and y spin boxes and the slots in this class

    connect(browser, SIGNAL(viewChanged(QRectF)), this, SLOT(viewChanged(QRectF)));
    connect(xPanSpinner, SIGNAL(valueChanged(double)), browser, SLOT(setPanX(double)));
    connect(yPanSpinner, SIGNAL(valueChanged(double)), browser, SLOT(setPanY(double)));

    // Lay out the spinners

    QGridLayout * panSpinLayout = new QGridLayout();
    QLabel* xLabel = new QLabel("x");
    QLabel* yLabel = new QLabel("y");
    panSpinLayout->setContentsMargins(6, 18, 6, 6);
    panSpinLayout->addWidget(xLabel, 0, 0);
    panSpinLayout->addWidget(xPanSpinner, 0, 1);
    panSpinLayout->addWidget(yLabel, 1, 0);
    panSpinLayout->addWidget(yPanSpinner, 1, 1);

    // Create and return the pan group box.

    QGroupBox * panSpinGroup = new QGroupBox("Pan");
    panSpinGroup->setContentsMargins(2, 2, 2, 2);
    panSpinGroup->setFixedWidth(90);
    panSpinGroup->setLayout(panSpinLayout);
    return panSpinGroup;
}

QVBoxLayout* RtiViewerDlg::initZoomLayout(QGroupBox * lightGroupBox, QGroupBox * zoomGroupBox, QGroupBox * panGroupBox)
{
    // Lay out the light, zoom, and pan groups

    QVBoxLayout* zoomLayout = new QVBoxLayout();
    zoomLayout->addWidget(lightGroupBox, 0, Qt::AlignHCenter);
    zoomLayout->addStretch(100);
    zoomLayout->addWidget(zoomGroupBox, 0, Qt::AlignHCenter);
    zoomLayout->addStretch(100);
    zoomLayout->addWidget(panGroupBox, 0, Qt::AlignHCenter);

    // Return the layout

    return zoomLayout;
}

QGridLayout * RtiViewerDlg::initToolbarAndLightLayout(QVBoxLayout * zoomLayout)
{
    // Lay out the toolbar, the light, and the light, zoom, and pan spin boxes

    QGridLayout* toolbarAndLight = new QGridLayout;
    toolbarAndLight->setColumnStretch(1, 100);
    toolbarAndLight->addWidget(toolBar, 0, 0, Qt::AlignTop | Qt::AlignLeft );
    toolbarAndLight->addWidget(light, 0, 1, Qt::AlignCenter);
    toolbarAndLight->addLayout(zoomLayout, 0, 2, Qt::AlignTop);

    // Return the layout

    return toolbarAndLight;
}

QGroupBox * RtiViewerDlg::initRendGroup()
{
    // Create the rendering mode group

    QGroupBox* renderingGroup = new QGroupBox("Rendering mode", this);
    renderingGroup->setFixedWidth(378);

    // Create the rendering dialog and connect it to the browser

    rendDlg = new RenderingDialog(NULL, -1, renderingGroup);
    connect(rendDlg, SIGNAL(renderingModeChanged(int)), browser, SLOT(setRenderingMode(int)));
    connect(rendDlg, SIGNAL(updateImage()), browser, SLOT(updateImage()));

    // Add the rendering dialog to the rendering mode widget

    QVBoxLayout* rendLayout = new QVBoxLayout;
    rendLayout->setContentsMargins(0, 0, 0, 0);
    rendLayout->addWidget(rendDlg);
    renderingGroup->setLayout(rendLayout);

    // Return the rendering mode group

    return renderingGroup;
}

QTabWidget * RtiViewerDlg::initTabWidget()
{
    // Create the tabs for the file info/navigator and bookmarks

    QFrame * fileFrame = initFileFrame();
    QFrame * navigatorFrame = initNavigatorFrame();
    QFrame * fileNavFrame = initFileNavFrame(navigatorFrame, fileFrame);
    initBookmarkControl();
    return initTabs(fileNavFrame);
}

QFrame * RtiViewerDlg::initFileFrame()
{
    // Create the file information frame

    QFrame* fileFrame = new QFrame(this);
    fileFrame->setFrameStyle(QFrame::NoFrame | QFrame::Plain);
    fileFrame->setFixedWidth(350);

    // Create the labels and file information boxes

    QLabel* fileLabel = new QLabel("File");
    QLabel* sizeLabel = new QLabel("Size");
    QLabel* formatLabel = new QLabel("Format");
    filename = new QLineEdit(fileFrame);
    filename->setReadOnly(true);
    filesize = new QLineEdit(fileFrame);
    filesize->setReadOnly(true);
    fileformat = new QLineEdit(fileFrame);
    fileformat->setReadOnly(true);

    // Lay out the file information and add it to the file information group

    QGridLayout* fileLayout = new QGridLayout;
	fileLayout->setContentsMargins(0, 0, 0, 0);
    fileLayout->addWidget(fileLabel, 0, 0);
    fileLayout->addWidget(filename, 0, 1, 1, 3);
    fileLayout->addWidget(sizeLabel, 1, 0);
    fileLayout->addWidget(filesize, 1, 1);
    fileLayout->addWidget(formatLabel, 1, 2);
    fileLayout->addWidget(fileformat, 1, 3);
    fileFrame->setLayout(fileLayout);

    // Return the file information frame

    return fileFrame;
}

QFrame * RtiViewerDlg::initNavigatorFrame()
{
    // Create the navigator

    navigator = new Navigator(this, 346, 221, maxZoom);

    // Create the navigator layout

    QHBoxLayout * navigatorLayout = new QHBoxLayout();
    navigatorLayout->setContentsMargins(0, 0, 0, 0);
    navigatorLayout->addWidget(navigator);

    // Create the navigator frame

    QFrame * navigatorFrame = new QFrame();
    navigatorFrame->setFixedSize(350, 225);
    navigatorFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    navigatorFrame->setLineWidth(2);
    navigatorFrame->setLayout(navigatorLayout);

    // Connect the navigator to the browser

    connect(browser, SIGNAL(sizeChanged(int, int)), navigator, SLOT(updateBrowserSize(int, int)));
    connect(browser, SIGNAL(viewChanged(QRectF)), navigator, SLOT(updateSelection(QRectF)));
    connect(navigator, SIGNAL(selectionChanged(QRectF, bool)), browser, SLOT(updateView(QRectF, bool)));

    // Return the navigator frame

    return navigatorFrame;
}

QFrame *RtiViewerDlg::initFileNavFrame(QFrame * navigatorFrame, QFrame * fileFrame)
{
    // Create the frame containing the file info and the navigator

    QFrame * fileNavFrame = new QFrame(this);
    fileNavFrame->setFrameStyle(QFrame::Plain | QFrame::NoFrame);
    fileNavFrame->setLineWidth(3);

    // Lay out the frame

    QVBoxLayout* fileNavLayout = new  QVBoxLayout();
    fileNavLayout->addWidget(fileFrame);
    fileNavLayout->addWidget(navigatorFrame);
    fileNavFrame->setLayout(fileNavLayout);

    // Return the frame

    return fileNavFrame;
}

void RtiViewerDlg::initBookmarkControl()
{
    // Create the bookmark control

    bookmarkControl = new BookmarkControl(this, browser, light, rendDlg);
}

QTabWidget * RtiViewerDlg::initTabs(QFrame * fileNavFrame)
{
    // Create and return the widget containing tabs for the
    // file info/navigator and bookmarks.

    QTabWidget* tabWidget = new QTabWidget();
    tabWidget->addTab(fileNavFrame, QString ("Navigator"));
    tabWidget->addTab(bookmarkControl, QString("Bookmarks"));
    return tabWidget;
}

QGridLayout * RtiViewerDlg::initMainLayout()
{
    // Create the main window layout and return it.

    QGridLayout* layout = new QGridLayout;
    layout->setContentsMargins(5, 5, 5, 5);

    layout->setColumnStretch(0, 5);
    layout->setColumnStretch(1, 0);

    layout->setRowStretch(0, 3);
    layout->setRowStretch(1, 2);
    layout->setRowStretch(2, 4);

    return layout;
}

void RtiViewerDlg::initHttpThread()
{
    // Create a new HttpThread

    mutex = new QMutex;
    infoReady = new QWaitCondition;
    getter = new HttpThread(*mutex, *infoReady);

    // Connect the HttpThread and the browser

    connect(getter, SIGNAL(errorOccurred(QString)), this, SLOT(httpErrorOccurred(QString)), Qt::QueuedConnection);
    connect(getter, SIGNAL(imageUpdated()), browser, SLOT(updateImage()));
    connect(getter, SIGNAL(downloadFinished()), browser, SLOT(downloadFinished()));
    connect(browser, SIGNAL(viewChanged(QRectF)), getter, SIGNAL(viewUpdated(QRectF)));
    connect(browser, SIGNAL(updateRenderingList(int, bool)), rendDlg, SLOT(updateRenderingList(int, bool)));

    // Start the HttpThread

    getter->start();
}

void RtiViewerDlg::initAppSettings()
{
    // Create new application settings

    settings = new QSettings("VCG", "RTIViewer");

    // Get the current settings (if any) and set the global variables

    int tempW = settings->value("maxWindowWidth", 2000).toInt();
    int tempH = settings->value("maxWindowHeight", 2000).toInt();
    dir.setPath(settings->value("workingDir", "").toString());
    lastUrl.setUrl(settings->value("lastUrl", "").toString());

    // Set the maximum size of the browser window

    browser->setMaxWindowSize(tempW, tempH);
}

/*====================================================================
 *
 * Methods to implement the actions on the toolbar
 *
 *===================================================================*/

/*====================================================================
 * Open a local file
 *===================================================================*/

int RtiViewerDlg::open()
{
    getter->closeConnection();
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), dir.path() , filterStr);
    return openFile(path);
}

int RtiViewerDlg::openFile(QString path)
{
    if (path == "") return -1;
    QFileInfo info(path);
    QFile data(path);
    dir.setPath(info.path());
    settings->setValue("workingDir", dir.path());
    if (info.suffix() != "ptm" && info.suffix() != "hsh" && info.suffix() != "rti" && info.suffix() != "mview")
    {
        QMessageBox::critical(this, tr("Opening error"), tr("The file: \n%1\n is invalid.\n Internal format unknown.").arg(path));
        return 0;
    }
    if (data.open(QFile::ReadOnly))
    {
        Rti* image = NULL;
        LoadingDlg* loading = new LoadingDlg(this);
        if (info.suffix() == "ptm")
        {
            QTextStream input(&data);
            image = Ptm::getPtm(input);
            data.close();
        }
        else if (info.suffix() == "hsh")
            image = new Hsh();
        else if (info.suffix() == "rti")
            image = new UniversalRti();
        else if (info.suffix() == "mview")
            image = new MultiviewRti();
        if (image != NULL)
        {
            loading->show();
            image->setFileName(path);
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        }
        else
        {
            delete image;
            loading->close();
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, tr("Opening error"), tr("The file: \n%1\n is invalid.\n Internal format unknown.").arg(path));
            delete loading;
            return 0;
        }
        browser->setImage(NULL);
        navigator->setImage(NULL, 0, 0);
        rendDlg->setRenderingMode(NULL, 0);
        filename->setText("");
        filesize->setText("");
        fileformat->setText("");
        int result = 0;
        try
        {
            result = image->load(LoadingDlg::QCallBack);
        }
        catch (std::bad_alloc&)
        {
            result = -2;
        }

        if (result == 0) //Loads the image info
        {
			loading->close();
            zoomFact->setEnabled(true);
            xLightSpinner->setEnabled(true);
            yLightSpinner->setEnabled(true);
            xPanSpinner->setEnabled(true);
            yPanSpinner->setEnabled(true);
            //Sets the browser image
            browser->setImage(image);
            //Sets the navigator image
            navigator->setImage(image->createPreview(344, 226), image->width(), image->height());
            QApplication::restoreOverrideCursor();
            rendDlg->setRenderingMode(browser->getRenderingModes(), browser->getCurrentRendering());
            
            //Sets file info
            filename->setText(path);
            filesize->setText(tr("%1 x %2").arg(image->width()).arg(image->height()));
            fileformat->setText(image->typeFormat());
            light->setInteractive(true);

            // Open the bookmark file, if any.

            bookmarkControl->loadBookmarks(info);
        }
        else if (result == -1)
        {
            loading->close();
            delete image;
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, tr("Opening error"), tr("The file: \n%1\n is invalid.\n Internal format unknown.").arg(path));
        }
        else if (result == -2)
        {
            loading->close();
            delete image;
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, tr("Opening error"), tr("The virtual memory is not\n suffice to open the file"));
        }
        delete loading;
    }
    return 0;
}

/*====================================================================
 * Open a remote file
 *===================================================================*/

int RtiViewerDlg::openRemote()
{
    OpenRemoteDlg dlg(lastUrl, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        settings->setValue("lastUrl", lastUrl.toString());
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QString path = lastUrl.path();
        QUrl* url = new QUrl(lastUrl);
        Rti* imageRti;
        getter->closeConnection();
        QFileInfo pathinfo(path);
        url->setPath(pathinfo.path() + "/");
        getter->setUrl(url, pathinfo.fileName());
        //Gets the xml file
        QBuffer info;
        info.open(QIODevice::ReadWrite);
        getter->getInfo(&info);
        mutex->lock();
        infoReady->wait(mutex);
        mutex->unlock();
        info.close();
        if (getter->checkHttpError())
        {
            getter->resetHttpError();
            QApplication::restoreOverrideCursor();
            return -1;
        }
        imageRti = parseXml(&info);
        if (!imageRti)
        {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, "Error", "Remote file contains invalid xml.");
            return -1;
        }
        browser->setImage(NULL);
        navigator->setImage(NULL, 0, 0);
        rendDlg->setRenderingMode(NULL, 0);
        filename->setText("");
        filesize->setText("");
        fileformat->setText("");
        if (imageRti->allocateRemoteImage(&info) != 0)
        {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, "Error", "Remote file contains invalid xml.");
            return -1;
        }

        //Gets the thumbnail
        QBuffer thumb;
        thumb.open(QIODevice::ReadWrite);
        getter->getThumb(&thumb);
        mutex->lock();
        infoReady->wait(mutex);
        mutex->unlock();
        thumb.close();
        if (getter->checkHttpError())
        {
            delete imageRti;
            QApplication::restoreOverrideCursor();
            getter->resetHttpError();
            return -1;
        }
        QImage* image = new QImage();
        if (!image->loadFromData(thumb.buffer()))
        {
            delete imageRti;
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, "Error", "Remote thumb-nail invalid.");
            return -1;
        }
        zoomFact->setEnabled(true);
        xLightSpinner->setEnabled(true);
        yLightSpinner->setEnabled(true);
        xPanSpinner->setEnabled(true);
        yPanSpinner->setEnabled(true);
        //Allocates image
        emit getter->setRti(imageRti);
        browser->setImage(imageRti);
        navigator->setImage(image, imageRti->width(), imageRti->height());
        rendDlg->setRenderingMode(browser->getRenderingModes(), browser->getCurrentRendering(), true);
        filename->setText(lastUrl.toString());
        filesize->setText(tr("%1 x %2").arg(imageRti->width()).arg(imageRti->height()));
        fileformat->setText(imageRti->typeFormat());
        light->setInteractive(true);
        //Gets tiles
        getter->getTiles();
        QApplication::restoreOverrideCursor();

        // Open the bookmark file, if any.

        bookmarkControl->loadBookmarks(pathinfo);
    }
    return 0;
}

Rti* RtiViewerDlg::parseXml(const QBuffer* b)
{
    QDomDocument doc;
    doc.setContent(b->buffer(), false);
    QDomNode root = doc.firstChild();
    QDomElement infoNode = root.firstChildElement("Info");
    if (infoNode.isNull())
        return NULL;
    bool error;
    //type info
    QString type = infoNode.attribute("type");
    if (type.isEmpty())
        return NULL;
    if (type == "LRGB PTM" || type == "JPEG-LRGB PTM")
        return new LRGBPtm();
    else if (type == "HSH")
        return new Hsh();
    return NULL;
}

void RtiViewerDlg::httpErrorOccurred(QString error)
{
    getter->resetHttpError();
    QMessageBox::critical(this, "HTTP Error", error);
}

/*====================================================================
 * Configure the viewer
 *===================================================================*/

void RtiViewerDlg::configure()
{
	int currentW = settings->value("maxWindowWidth").toInt();
	int currentH = settings->value("maxWindowHeight").toInt();
	//Shows the configuration dialog.
	ConfigDlg* dlg = new ConfigDlg(currentW, currentH, browser->getSize(), this);
	if (dlg->exec() == 1) //User changed the application settings.
	{
		QSize newSize = dlg->getCurrentSize();
		if (newSize.height() != currentH  || newSize.width() != currentW)
		{
			//Saves the new appication settings.
			settings->setValue("maxWindowWidth", newSize.width());
			settings->setValue("maxWindowHeight", newSize.height());
			settings->sync();
			browser->setMaxWindowSize(newSize.width(), newSize.height());
		}
	}
	delete dlg;
}

/*====================================================================
 * Display the About dialog
 *===================================================================*/

void RtiViewerDlg::about()
{
	AboutDlg *dlg = new AboutDlg(this);
//	Ui::aboutDialog aboutdlg;
//	aboutdlg.setupUi(dlg);
	dlg->exec();
	delete dlg;
}

/*====================================================================
 *
 * Slots to update widgets in the GUI
 *
 *===================================================================*/

/*====================================================================
 * Update the zoom spin box
 *===================================================================*/

void RtiViewerDlg::setZoomValue(float value, float minValue)
{
    zoomFact->setMinimum(minValue*100);
    disconnect(zoomFact, SIGNAL(valueChanged(int)), 0, 0);
    zoomFact->setValue(value*100);
    connect(zoomFact, SIGNAL(valueChanged(int)), browser, SLOT(updateZoom(int)));
}

void RtiViewerDlg::setZoomValue(int value)
{
    zoomFact->setValue(value);
}

/*====================================================================
 * Handle changes for the light spin boxes
 *===================================================================*/

void RtiViewerDlg::setLightSpinBoxes(vcg::Point3f light)
{
    // Update the light spin boxes.

    // We temporarily block the signals so that the picture isn't
    // updated three times -- when the light control changes value
    // (which signals this method), when the x spin box is changed,
    // and when the y spin box is changed.

    xLightSpinner->blockSignals(true);
    xLightSpinner->setValue((double)light.X());
    xLightSpinner->blockSignals(false);

    yLightSpinner->blockSignals(true);
    yLightSpinner->setValue((double)light.Y());
    yLightSpinner->blockSignals(false);
}

void RtiViewerDlg::xLightSpinnerChanged(double x)
{
    spinnerChanged(xLightSpinner, yLightSpinner, x, true);
}

void RtiViewerDlg::yLightSpinnerChanged(double y)
{
    spinnerChanged(yLightSpinner, xLightSpinner, y, false);
}

void RtiViewerDlg::spinnerChanged(QDoubleSpinBox * spinner1, QDoubleSpinBox * spinner2, float value, bool xChanged)
{
    // With the spinner, it is possible to set (x,y) to invalid coordinates,
    // such as (1.0, 1.0). When a new x (or y) value is set, check that the
    // existing y (or x) value is still legal. If it is not legal, set it
    // to the maximum legal value.
    //
    // Note that we block the other spinner while changing it so we don't
    // enter this method a second time.

    spinner2->blockSignals(true);
    double otherCoord = trunc(100.0 * sqrt(1.0 - value*value))/100.0;
    if (spinner2->value() >= 0.0)
    {
        if (spinner2->value() > otherCoord)
            spinner2->setValue(otherCoord);
    }
    else
    {
        otherCoord = -1.0 * otherCoord;
        if (spinner2->value() < otherCoord)
            spinner2->setValue(otherCoord);
    }
    spinner2->blockSignals(false);

    // Compute the new z value based on the x and y values.

    float x, y, z;
    if (xChanged)
    {
        x = spinner1->value();
        y = spinner2->value();
    }
    else
    {
        x = spinner2->value();
        y = spinner1->value();
    }
    z = sqrt(1.0 - x*x - y*y);

    // Set the light to the new value. This will cause the light control
    // to be updated and the picture to be displayed with the new light
    // direction.

    vcg::Point3f point = vcg::Point3f(x, y, z);
    light->setLight(point, true);
}

/*====================================================================
 * Handle changes for the pan spin boxes
 *===================================================================*/

void RtiViewerDlg::viewChanged(QRectF subimg)
{
    // This slot is called when the view in the browser changes.

    // Get the size of the current image from the browser.

    QSize imageSize = browser->getImageSize();

    // Set the x and y pan values and their maximums. The maximum
    // value for each is the size of the image minus the size of
    // the sub-image (view).

    xPanSpinner->setValue(subimg.x());
    xPanSpinner->setMaximum(imageSize.width() - subimg.width());
    yPanSpinner->setValue(subimg.y());
    yPanSpinner->setMaximum(imageSize.height() - subimg.height());
}

/*====================================================================
 *
 * Miscellaneous methods
 *
 *===================================================================*/

/*====================================================================
 * Get the current zoom value as an integer
 *===================================================================*/

int RtiViewerDlg::getZoom()
{
    return zoomFact->value();
}

QFileInfo RtiViewerDlg::getRTIFileInfo()
{
    return QFileInfo(filename->text());
}
