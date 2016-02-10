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


#ifndef RTIBROWSER_H
#define RTIBROWSER_H

//#include "gui.h"
#include "rti.h"
#include "renderingmode.h"
#include "util.h"
//#include "diffusegain.h"
//#include "specularenhanc.h"
//#include "normalenhanc.h"
//#include "unsharpmasking.h"
//#include "coeffenhanc.h"
//#include "detailenhanc.h"
//#include "dyndetailenhanc.h"
//#include "normalsrendering.h"

#include <vcg/space/point3.h>
#include <vcg/math/matrix33.h>

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#include <cmath>
#endif

// Qt headers
#include <QGLWidget>
#include <QShortcut>
#include <QTimer>

// RtiViewerDlg and RtiBrowser point to each other. Forward declare
// RtiViewerDlg to resolve the circular dependency.

class RtiViewerDlg;

//! RTI browser class.
/*!
  The class defines the browser for RTI image.
*/
class RtiBrowser : public QGLWidget
{
	Q_OBJECT


public:
	
	//! Constructors
	/*!
	  \param w width of the browser.
	  \param h height of the browser.
	  \param image RTI image to display.
	  \param maxZ maximum zoom level.
	  \param parent
	*/
	RtiBrowser(int w, int h, Rti *image, int maxZ, QWidget * parent, const QGLFormat& format);
	~RtiBrowser();

	/*!
	  Sets the RTI image to display in the browser.
	*/
	void setImage(Rti* rti);

	/*!
	  Returns the browser size.
	*/
	QSize getSize();

    QRectF getSubimage();

    QSize getImageSize();

	/*!
      Returns the list of rendering modes of the current RTI image.
	*/
    QMap<int, RenderingMode*>* getRenderingModes();

	/*!
	  Return the index of the rendering mode applied in the browser.
	*/
	int getCurrentRendering();

protected:

	/*!
	  Initialazes the OpenGL environment.
	*/
	void initializeGL();

	/*!
	  Paint event handler.
	*/
	void paintGL();

	/*!
	  Resize event handler.
	*/
	void resizeGL(int width, int height);
	
	/*!
	  Mouse press event handler.
	*/
	void mousePressEvent(QMouseEvent *event);
	
	/*!
	  Mouse move event handler.
	*/
	void mouseMoveEvent(QMouseEvent *event);
	
	/*!
	  Mouse release event handler.
	*/
	void mouseReleaseEvent(QMouseEvent *event);
	
	/*!
          Mouse float click event handler.
	*/
    void mousefloatClickEvent(QMouseEvent *event);
	
	/*!
	  Wheel event handler.
	*/
	void wheelEvent(QWheelEvent * event);

	/*!
	  Leave event handler.
	*/
	void leaveEvent(QEvent *event);
	
// private data member
private:

	Rti* img; /*!< RTI image to display. */
    RtiViewerDlg* gui; /*! Pointer to the main RTIViewer window */
	
	vcg::Point3f light; /*!< Light vector. */
	bool lightChanged; /*!< Holds whether the light direction is changing by the user with the middle button of the mouse. */
	bool lightChangedRight; /*!< Holds whether the light direction is changing by the user with the right button of the mouse. */
	vcg::Point3f lastLight; /*!< Light vector used to update the light direction during the mouse interaction. */
	float dxLight; /*!< Offset on x-axis of light vector. */
	float dyLight; /*!< Offset on y-axis of light vector. */
	
	QRectF subimg; /*!< Sub-image diplaied in the browser. */
	
	int level; /*!< Mip-mapping level used. */
	
	int _height; /*!< Height of the browser. */
	int _width; /*!< Width of the browser. */

	int maxHeight; /*!< Max browsing window height. */
	int maxWidth; /*!< Max browsing window width. */
	
	int textureHeight; /*!< Height of the texture. */
	int textureWidth; /*!< Width of the texture. */
	unsigned char* textureData; /*!< Texture buffer.  */
	bool isNewTexture; /*!< Holds whether the texture is new. */
	GLuint texName; /*!< Texture name. */

	int viewHeight; /*!< Height of the current view. */
	int viewWidth; /*!< Width of the current view. */

    float zoom; /*!< Current zoom level. */
	float minZoom; /*!< Minimum zoom level. */
	const float maxZoom; /*!< Maximum zoom level. */
	QShortcut zoomIn; /*!< Shortcut for zoom in action.  */
	QShortcut zoomOut; /*!< Shortcut for zoom out action. */
	
	QPoint dragPoint; /*!< Second-last point received from a move event */
	QPoint nextDragPoint; /*!< Last point received from a move event */

	bool dragging; /*!< Holds whether the user executes a dragging operation. */
	bool focus; /*!< Holds whether the cursor of the mouse is over the image and the image can be panned. */
	
	QTimer* timer; /*!< Timer to manage the paint event. */
	
	bool interactive; /*!< Holds whether the browser can update the texture troughout the mouse interaction. If it is false the texture is update only at release event. */
	bool posUpdated; /*!< Holds whether the position of the image in the browser is changed. */ 

	int currentMode; /*!< Current rendering mode applied to the image. */

    bool addingHighlightBox;
    bool editingHighlightBox;
    bool drawingHighlightBox;
    bool haveHighlightBox;
    float bmStartX;
    float bmStartY;
    float bmEndX;
    float bmEndY;
    QRectF highlightBox;

	// Shortcut for special rendering mode.
	QShortcut defaultMode;
//	QShortcut normalsMode;
	QShortcut lumUnsharpMode;
	QShortcut smoothMode;
	QShortcut contrastMode;
	QShortcut enhancedMode;
	QShortcut lumMode;
	QShortcut rgbMode;
	QShortcut lumRMode;
	QShortcut lumGMode;
	QShortcut lumBMode;
	QShortcut a0Mode;
	QShortcut a1Mode;
	QShortcut a2Mode;
	QShortcut a3Mode;
	QShortcut a4Mode;
	QShortcut a5Mode;
	QShortcut lightVectorMode;
	QShortcut lightVectorMode2;

	
// private method
private:


	/*!
	  Updates the view size
	*/
	void updateViewSize();

	/*!
	  Updates the view size according to the zoom
	*/
	void updateZoomimg();
	
	/*!
	  Updates the texture info. If \a refresh is true the texture in the browser is updated.
	*/
    void updateTexture(bool refresh = true);

	/*!
	  Moves the sub-image.
	  \param offx, offy offset.
    */
	void updateSubImage(int offx, int offy);

	/*!
	  Updates the light vector in the light control.
	*/
	void updateLight();

    QPointF getCorner(int corner, QRectF rect);

    void imageToBrowser(float & x, float & y);

    void browserToImage(float & x, float & y);

    void saveSnapshotMetadata(QString fileName);

    void createParameterChild(QDomDocument xmp, QDomElement parametersBag, QString name, float value);

// private Qt slots
private slots:

	// Slots for shortcut.
	void zoomInActivated();
	void zoomOutActivated();
	void defaultModeActivated();
	void lumModeActivated();
	void smoothModeActivated();
	void contrastModeActivated();
	void enhancedModeActivated();
	void lumUnsharpModeActivated();
	void rgbModeActivated();
	void lumRModeActivated();
	void lumGModeActivated();
	void lumBModeActivated();
	void a0ModeActivated();
	void a1ModeActivated();
	void a2ModeActivated();
	void a3ModeActivated();
	void a4ModeActivated();
	void a5ModeActivated();
	void lightVectorModeActivated();
	void lightVectorMode2Activated();
	void fired();

// Qt signal
signals:

	/*!
	  Emitted to keep updated the size of the browser.
	*/
	void sizeChanged(int w, int h);

	/*!
	  Emitted to keep updated the size of the current view.
	*/
	void viewChanged(QRectF rect);

	
	/*!
	  Emitted to permit the interactive updating of the light.
	*/
	void setInteractiveLight(bool value);
	
	/*!
	  Emitted to enable the change of the light direction.
	*/
	void setEnabledLight(bool value);

	/*!
	  Emitted to update the current rendering mode.
	*/
	void updateRenderingList(int currentRendering, bool remote);

	/*!
	  Emitted to move the light direction.
	*/
	void moveLight(float offx, float offy);

	/*!
	  Emitted to set the light direction.
	*/
	void setLightDir(const vcg::Point3f& l, bool refresh);

    /*!
	  Emitted to update the zoom value.
	*/
	void updateZoomValue(float value, float minValue);

    /*!
      Emitted when a highlight box has been drawn.
    */
    void highlightBoxDrawn(QRectF box);

// public Qt slots
public slots:
	
	/*!
	  Updates the sub-image rectangle.
	*/
	void updateView(QRectF rect, bool resize);

	/*!
	  Updates the light vector.
	*/
	void setLight(vcg::Point3f l, bool refresh = true); 
	
	/*!
	  Sets the current rendeing mode.
	*/
	void setRenderingMode(int mode);

	/*!
	  Updates the texture in the browser.
	*/
	void updateImage();

	/*!
	  Updates info of a remote RTI image.
	*/
	void downloadFinished();

	/*!
	  Sets the size of the browsing windows.
	*/
	void setMaxWindowSize(int w, int h);

	/*!
	  Saves a snapshot of the image in the current view of the browser.
	*/
	void snapshotActivated();

	/*!
	  Set the zoom factor.
	*/
	void updateZoom(int zoomVal);

    /*!
      Set the x coordinate of the pan.
    */
    void setPanX(double x);

    /*!
      Set the y coordinate of the pan.
    */
    void setPanY(double y);

    /*!
      Set the pan.
    */
    void setPan(QPointF point);

    /*!
      Set a flag that the user will draw a highlight box.
    */
    void addHighlightBox();

    /*!
      Set a flag that the user will change a highlight box.
    */
    void editHighlightBox();

    /*!
      Set a new highlight box.
    */
    void setHighlightBox(bool drawHighlightBox, QRectF box);

    /*!
      Delete the highlight box.
    */
    void deleteHighlightBox();

};


#endif /* RTIBROWER_H */
