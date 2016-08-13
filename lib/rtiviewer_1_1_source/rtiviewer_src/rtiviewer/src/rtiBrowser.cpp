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
#include "rtiBrowser.h"
#include "detailenhanc.h"

// Qt headers
#include <QMouseEvent>
#include <QTime>
#include <QApplication>
#include <QColor>
#include <QFileDialog>
#include <QPainter>
#include <QUuid>

#include <QDebug>
#include <QMessageBox>

#include <iostream>

RtiBrowser::RtiBrowser(int w, int h, Rti *image, int maxZ, QWidget *parent, const QGLFormat& format): QGLWidget(format, parent),
gui((RtiViewerDlg*)parent),
img(NULL),
light(0,0,1),
lightChanged(false),
lightChangedRight(false),
dxLight(0.0f),
dyLight(0.0f),
subimg(0,0,0,0),
level(0),
_height(h),
_width(w),
textureHeight(0),
textureWidth(0),
textureData(NULL),
isNewTexture(false),
texName(-1),
viewHeight(0),
viewWidth(0),
maxWidth(2500),
maxHeight(2500),
zoom(1.0),
minZoom(1.0),
zoomIn(QKeySequence::ZoomIn, this),
zoomOut(QKeySequence::ZoomOut, this),
dragPoint(QPoint(-1, -1)),
timer(new QTimer(this)),
maxZoom(maxZ),
dragging(false),
focus(false),
interactive(true),
posUpdated(false),
defaultMode(QKeySequence(Qt::ALT + Qt::Key_Q), this),
smoothMode(QKeySequence(Qt::ALT + Qt::Key_E), this),
contrastMode(QKeySequence(Qt::ALT + Qt::Key_R), this),
enhancedMode(QKeySequence(Qt::ALT + Qt::Key_T), this),
lumUnsharpMode(QKeySequence(Qt::ALT + Qt::Key_Y), this),
lumMode(QKeySequence(Qt::ALT + Qt::Key_A), this),
rgbMode(QKeySequence(Qt::ALT + Qt::Key_S), this),
lumRMode(QKeySequence(Qt::ALT + Qt::Key_D), this),
lumGMode(QKeySequence(Qt::ALT + Qt::Key_F), this),
lumBMode(QKeySequence(Qt::ALT + Qt::Key_G), this),
a0Mode(QKeySequence(Qt::ALT + Qt::Key_U), this),
a1Mode(QKeySequence(Qt::ALT + Qt::Key_I), this),
a2Mode(QKeySequence(Qt::ALT + Qt::Key_O), this),
a3Mode(QKeySequence(Qt::ALT + Qt::Key_J), this),
a4Mode(QKeySequence(Qt::ALT + Qt::Key_K), this),
a5Mode(QKeySequence(Qt::ALT + Qt::Key_L), this),
lightVectorMode(QKeySequence(Qt::ALT + Qt::Key_M), this),
lightVectorMode2(QKeySequence(Qt::ALT + Qt::Key_N), this),
  addingHighlightBox(false),
  editingHighlightBox(false),
  drawingHighlightBox(false),
  haveHighlightBox(false)
{

    currentMode = DEFAULT_MODE;

    // custom settings
    setMinimumSize(650, 650);
    setWindowFlags(Qt::SubWindow);

    // signal-slot connections
    connect(&zoomIn, SIGNAL(activated()), this, SLOT(zoomInActivated()));
    connect(&zoomOut, SIGNAL(activated()), this, SLOT(zoomOutActivated()));
    connect(&defaultMode, SIGNAL(activated()), this, SLOT(defaultModeActivated()));
    connect(&smoothMode, SIGNAL(activated()), this, SLOT(smoothModeActivated()));
    connect(&contrastMode, SIGNAL(activated()), this, SLOT(contrastModeActivated()));
    connect(&enhancedMode, SIGNAL(activated()), this, SLOT(enhancedModeActivated()));
    connect(&lumMode, SIGNAL(activated()), this, SLOT(lumModeActivated()));
    connect(&lumUnsharpMode, SIGNAL(activated()), this, SLOT(lumUnsharpModeActivated()));
    connect(&rgbMode, SIGNAL(activated()), this, SLOT(rgbModeActivated()));
    connect(&lumRMode, SIGNAL(activated()), this, SLOT(lumRModeActivated()));
    connect(&lumGMode, SIGNAL(activated()), this, SLOT(lumGModeActivated()));
    connect(&lumBMode, SIGNAL(activated()), this, SLOT(lumBModeActivated()));
    connect(&a0Mode, SIGNAL(activated()), this, SLOT(a0ModeActivated()));
    connect(&a1Mode, SIGNAL(activated()), this, SLOT(a1ModeActivated()));
    connect(&a2Mode, SIGNAL(activated()), this, SLOT(a2ModeActivated()));
    connect(&a3Mode, SIGNAL(activated()), this, SLOT(a3ModeActivated()));
    connect(&a4Mode, SIGNAL(activated()), this, SLOT(a4ModeActivated()));
    connect(&a5Mode, SIGNAL(activated()), this, SLOT(a5ModeActivated()));
    connect(&lightVectorMode, SIGNAL(activated()), this, SLOT(lightVectorModeActivated()));
    connect(&lightVectorMode2, SIGNAL(activated()), this, SLOT(lightVectorMode2Activated()));

    connect(timer, SIGNAL(timeout()), this, SLOT(fired()));

    // set RTI image if given
    if (image)
        setImage(image);
    setMouseTracking(true);
}


RtiBrowser::~RtiBrowser()
{
    if (img)
        delete img;

    if (textureData)
    {
        delete[] textureData;
        glDeleteTextures(1, &texName);
    }

    if (timer)
        delete timer;
}


void RtiBrowser::setImage(Rti* rti)
{
    if (img)
    {
        delete img;
        img = NULL;
    }

    if (!rti)
    {
        if (textureData)
            delete textureData;
        textureData = NULL;
        return;
    }
    img = rti;

    // Set view
    updateViewSize();

    // Set zoom info
    minZoom = static_cast<float>(viewWidth) / static_cast<float>(img->width());
    zoom = minZoom;

	emit updateZoomValue(zoom, minZoom);

    // Set sub-img
    subimg = QRectF(0.0, 0.0, img->width(), img->height());
    emit viewChanged(subimg);
    level = zoom >= 1 ? 0 :  floor(log(1.0/zoom)/log(2.0));
    updateTexture();
}


void RtiBrowser::setMaxWindowSize(int w, int h)
{
    maxWidth = w;
    maxHeight = h;
    if (img)
    {
        updateViewSize();
        minZoom = static_cast<float>(viewWidth) / static_cast<float>(img->width());
        if (zoom < minZoom)
            zoom = minZoom;
		emit updateZoomValue(zoom, minZoom);
        updateZoomimg();
        updateTexture();
    }
    int tempH = _height > maxHeight ? maxHeight: _height;
    int tempW = _width > maxWidth ? maxWidth: _width;
    emit sizeChanged(tempW, tempH);
}


QSize RtiBrowser::getSize()
{
    return QSize(_width, _height);
}

QRectF RtiBrowser::getSubimage()
{
    return subimg;
}

QSize RtiBrowser::getImageSize()
{
    return QSize(img->width(), img->height());
}


QMap<int, RenderingMode*>* RtiBrowser::getRenderingModes()
{
    if (img)
        return img->getSupportedRendering();
    return NULL;
}


int RtiBrowser::getCurrentRendering()
{
    if (img)
        return img->getCurrentRendering();
    return -1;
}


void RtiBrowser::initializeGL()
{
	QGLFormat format = this->format();
    unsigned int value = ((unsigned int)QGLFormat::openGLVersionFlags() | (unsigned int)QGLFormat::OpenGL_Version_2_1);
	if (value == 0)
	{
        QString version;
        switch(QGLFormat::openGLVersionFlags())
        {
            case QGLFormat::OpenGL_Version_1_1:
                version = "1.1";
                break;
            case QGLFormat::OpenGL_Version_1_2:
                version = "1.2";
                break;
            case QGLFormat::OpenGL_Version_1_3:
                version = "1.3";
                break;
            case QGLFormat::OpenGL_Version_1_4:
                version = "1.4";
                break;
            case QGLFormat::OpenGL_Version_1_5:
                version = "1.5";
                break;
            case QGLFormat::OpenGL_Version_2_0:
                version = "2.0";
                break;
            default:
                version = "unknown";
                break;
        }
	    QString message("RTIViewer requires your graphics card to support OpenGL version 2.1 or later. Your graphic card currently uses version %1. To solve this problem, please update your graphics drivers. If the updated drivers do not support version 2.1 or later, you may have rendering problems.");
        message.arg(version);
		QMessageBox::warning(this, tr("RTIViewer"), message);
	}

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glViewport(0, 0, _width, _height);
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0.0f, (GLfloat)_width, (GLfloat)_height, 0.0f, -1.0f, 1.0f);

}


void RtiBrowser::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    // Initializes texture for the first time
    if (textureData)
    {
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            glBindTexture(GL_TEXTURE_2D, texName);
            if(isNewTexture)
            {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
                    isNewTexture = false;
            }
    }

    glBegin(GL_POLYGON);
    if (textureData)
    {
            glTexCoord2f(0.0, 0.0);
            glVertex3f((_width - viewWidth)/2.0, (_height - viewHeight)/2.0, 0.0f);
            glTexCoord2f(1.0, 0.0);
            glVertex3f((_width + viewWidth)/2.0, (_height - viewHeight)/2.0, 0.0f);
            glTexCoord2f(1.0, 1.0);
            glVertex3f((_width + viewWidth)/2.0, (_height + viewHeight)/2.0, 0.0f);
            glTexCoord2f(0.0, 1.0);
            glVertex3f((_width - viewWidth)/2.0, (_height + viewHeight)/2.0, 0.0f);
    }
    glEnd();

    // Draw the highlight box for the currently selected bookmark, if any

    if (textureData)
    {
        // If we already have a highlight box, draw it now.

        if (haveHighlightBox)
        {
            // Convert the coordinates of the highlight box from the coordinate
            // system used by the image to the coordinate system used by the
            // browser widget.

            float x1 = highlightBox.topLeft().x();
            float y1 = highlightBox.topLeft().y();
            imageToBrowser(x1, y1);
            float x2 = highlightBox.bottomRight().x();
            float y2 = highlightBox.bottomRight().y();
            imageToBrowser(x2, y2);

            // Disable the texture while we draw the highlight box. If we don't
            // disable the texture, OpenGL ignores the line color commands.

            glDisable(GL_TEXTURE_2D);

            // Draw the highlight box twice, once in black and once in white. This
            // makes it visible against any background color.

            glLineWidth(1.0);
            qglColor(Qt::white);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x1, y1, 0.0f);
            glVertex3f(x2, y1, 0.0f);
            glVertex3f(x2, y2, 0.0f);
            glVertex3f(x1, y2, 0.0f);
            glEnd();

            glLineWidth(2.0);
            qglColor(Qt::black);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x1 - 2, y1 - 2, 0.0f);
            glVertex3f(x2 + 2, y1 - 2, 0.0f);
            glVertex3f(x2 + 2, y2 + 2, 0.0f);
            glVertex3f(x1 - 2, y2 + 2, 0.0f);
            glEnd();

            // Re-enable the texture.

            glEnable(GL_TEXTURE_2D);
        }
    }

    // Draw the highlight box while the user is drawing it.

    if (drawingHighlightBox)
    {
        // Disable the texture while we draw the highlight box. If we don't
        // disable the texture, OpenGL ignores the line color commands.

        glDisable(GL_TEXTURE_2D);

        // Draw the highlight box twice, once in black and once in white. This
        // makes it visible against any background color.

        glLineWidth(1.0);
        qglColor(Qt::white);
        glBegin(GL_LINE_LOOP);
        glVertex2f(bmStartX, bmStartY);
        glVertex2f(bmEndX, bmStartY);
        glVertex2f(bmEndX, bmEndY);
        glVertex2f(bmStartX, bmEndY);
        glEnd();

        // Assume that the upper left corner is the anchor. Test
        // whether other corners are the anchor and adjust the
        // shifts as needed.

        int shiftStartX = -2,
            shiftEndX = 2,
            shiftStartY = -2,
            shiftEndY = 2;
        if (bmStartX > bmEndX)
        {
            shiftStartX = -1 * shiftStartX;
            shiftEndX = -1 * shiftEndX;
        }
        if (bmStartY > bmEndY)
        {
            shiftStartY = -1 * shiftStartY;
            shiftEndY = -1 * shiftEndY;
        }

        glLineWidth(2.0);
        qglColor(Qt::black);
        glBegin(GL_LINE_LOOP);
        glVertex2f(bmStartX + shiftStartX, bmStartY + shiftStartY);
        glVertex2f(bmEndX + shiftEndX, bmStartY + shiftStartY);
        glVertex2f(bmEndX + shiftEndX, bmEndY + shiftEndY);
        glVertex2f(bmStartX + shiftStartX, bmEndY + shiftEndY);
        glEnd();

        // Re-enable the texture.

        glEnable(GL_TEXTURE_2D);

        // Flush the buffer(s) to force immediate drawing.

        glFlush();
    }

    glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1,1,-1,1,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	QFont font;
	font.setStyleStrategy(QFont::NoAntialias);
	font.setFamily("Helvetica");
	font.setPixelSize(12);
	glColor4f(1,0,0,1);
	QString text;
	switch(currentMode)
	{
		case SMOOTH_MODE: text = "SMOOTHED SIGNAL MAP"; break;
		case CONTRAST_MODE: text = "CONTRAST SIGNAL MAP"; break;
		case ENHANCED_MODE: text = "ENHANCED SIGNAL MAP"; break;
		case LUM_UNSHARP_MODE: text = "LUMINANCE (UNSHARP MASKING)"; break;
        case LUM_MODE: text = "LRGB LUMINANCE CHANNEL "; break;
		case RGB_MODE: text = "LRGB RGB COMPONENTS"; break;
		case LUMR_MODE: text = "RGB RED COMPONENT"; break;
		case LUMG_MODE: text = "RGB GREEN COMPONENT"; break;
		case LUMB_MODE: text = "RGB BLUE COMPONENT"; break;
		case A0_MODE: text = "COEFFICIENT A0"; break;
		case A1_MODE: text = "COEFFICIENT A1"; break;
		case A2_MODE: text = "COEFFICIENT A2"; break;
		case A3_MODE: text = "COEFFICIENT A3"; break;
		case A4_MODE: text = "COEFFICIENT A4"; break;
		case A5_MODE: text = "COEFFICIENT A5"; break;
		case LIGHT_VECTOR: text = "LIGHT VECTORS"; break;
		case LIGHT_VECTOR2: text = "LIGHT VECTORS"; break;
		default: text = "";
	}
	renderText(20, _height - 20, text, font);
	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
}


void RtiBrowser::resizeGL(int width, int height)
{
    _width = width;
    _height = height;
    glViewport(0,0, _width, _height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0.0f, (GLfloat)_width, (GLfloat)_height, 0.0f, -1.0f, 1.0f);

    if (img)
    {
        updateViewSize();

        // Update zoom info
        minZoom = static_cast<float>(viewWidth) / static_cast<float>(img->width());
        if (zoom < minZoom)
            zoom = minZoom;

		emit updateZoomValue(zoom, minZoom);
        updateZoomimg();
        updateTexture();
    }
    int h = _height > maxHeight ? maxHeight: _height;
    int w = _width > maxWidth ? maxWidth: _width;
    emit sizeChanged(w, h);
}


void RtiBrowser::mousePressEvent(QMouseEvent *event)
{
    if (!img) return;

    if (addingHighlightBox)
    {
        // Check if this press event is the start of drawing a bookmark
        // highlight box. If it is, set a flag that the user is drawing
        // a highlight box and record the coordinates of the starting point.
        // Note that the event coordinates are relative to the browser widget.

        addingHighlightBox = false;
        drawingHighlightBox = true;
        bmStartX = event->x();
        bmStartY = event->y();
        bmEndX = event->x();
        bmEndY = event->y();
        return;
    }
    else if (editingHighlightBox)
    {
        // Check if this press event is the start of editing (resizing)
        // a highlight box. If it is, set a flag that the user is editing
        // a highlight box and no longer has a highlight box.

        editingHighlightBox = false;
        haveHighlightBox = false;
        drawingHighlightBox = true;

        // Calculate which corner is closest to where the user clicked.
        // We only consider a corner to be "close" if it is less than
        // 20 units away. (The number 20 was chosen by trial-and-error).

        // The highlight box uses the image's coordinate system, so we
        // must convert them to the browser's coordinate system, which
        // is used by the event.

        int closest = -1;
        float shortestDistance = 20.0;

        for (int i = 0; i < 4; i++)
        {
            QPointF point = getCorner(i, highlightBox);
            float x = point.x();
            float y = point.y();
            imageToBrowser(x, y);
            float distance = sqrt((event->x() - x) * (event->x() - x) +
                                  (event->y() - y) * (event->y() - y));

            if (distance < shortestDistance)
            {
                shortestDistance = distance;
                closest = i;
            }
        }

        if (closest != -1)
        {
            // If we found a corner that was close, anchor the highlight box at
            // the opposite corner (bmStartX, bmStartY) and let the user
            // drag the close corner.

            QPointF point = getCorner((closest + 2) % 4, highlightBox);
            float x = point.x();
            float y = point.y();
            imageToBrowser(x, y);
            bmStartX = x;
            bmStartY = y;
            bmEndX = event->x();
            bmEndY = event->y();
        }
        else
        {
            // If no corner is close to where the user clicked, start a
            // new highlight box. This is the same as adding a highlight box.

            bmStartX = event->x();
            bmStartY = event->y();
            bmEndX = event->x();
            bmEndY = event->y();
        }
        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        // Begins a dragging operation.
        dragPoint = event->pos();
        timer->start(1);
        dragging = true;
    }
    else if (event->button() == Qt::MidButton)
    {
        // Begins a operation to modify the light direction.
        QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
        dragPoint = event->pos();
        timer->start(1);
        lightChanged = true;
        dxLight = 0;
        dyLight = 0;
    }
    else if (event->button() == Qt::RightButton)
    {
        QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
        dragPoint = event->pos();
        timer->start(1);
        lightChangedRight = true;
        lastLight = light;
        dxLight = 0;
        dyLight = 0;
    }

}


void RtiBrowser::mouseMoveEvent(QMouseEvent *event)
{
    if (!img) return;

    // If we are adding or editing a highlight box, just return. This will
    // avoid changing the cursor.

    if (addingHighlightBox || editingHighlightBox) return;

    // If the user is drawing a bookmark highlight box, use the current
    // point as the end point of the box and update the texture.

    if (drawingHighlightBox)
    {
        bmEndX = event->x();
        bmEndY = event->y();
        updateTexture();
        return;
    }

    posUpdated = false;
    if (dragging)
    {
        int offx = (dragPoint.x() - event->x())/zoom;
        int offy = (dragPoint.y() - event->y())/zoom;
        if (offx != 0 || offy != 0)
        {
            if (interactive)
            {
                dragPoint = event->pos();
                updateSubImage(offx, offy);
                posUpdated = true;
            }
        }
    }
    else if (lightChanged)
    {
        dxLight = (event->x() - dragPoint.x())/(zoom*2);
        dyLight = (event->y() - dragPoint.y())/(zoom*2);
        posUpdated = true;
    }
    else if (lightChangedRight)
    {
        dxLight = ((event->x() - dragPoint.x())/ static_cast<float>(_width)) * M_PI;
        dyLight = ((event->y() - dragPoint.y())/ static_cast<float>(_height)) * M_PI;
        nextDragPoint = event->pos();
        posUpdated = true;
    }
    else if (event->button() ==  Qt::NoButton)
    {
        if (viewWidth == _width || viewHeight == _height)
        {
            QRect rect((_width - viewWidth)/2.0, (_height - viewHeight)/2.0, viewWidth, viewHeight);
            if (rect.contains(event->pos()))
            {
                if (!focus && !lightChanged && !lightChangedRight)
                {
                    focus = true;
                    QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor));
                }
            }
            else
            {
                if (focus && !lightChanged && !lightChangedRight)
                {
                    QApplication::restoreOverrideCursor();
                    focus = false;
                }
            }
        }
        else if (focus && !lightChanged && !lightChangedRight)
        {
            QApplication::restoreOverrideCursor();
            focus = false;
        }
    }
}


void RtiBrowser::mouseReleaseEvent(QMouseEvent *event)
{
    setFocus();
    if (!img) return;

    if (drawingHighlightBox)
    {
        // If the user is done drawing a bookmark highlight box, calculate the
        // coordinates of the box in the coordinate system used by the image.
        // Note that bmStartX, bmStartY, etc. give the coordinates of the
        // mouse relative to the browser widget.

        float x1 = bmStartX;
        float y1 = bmStartY;
        browserToImage(x1, y1);
        float x2 = bmEndX;
        float y2 = bmEndY;
        browserToImage(x2, y2);

        // Flip x and y coordinates as necessary so (x1, y1) is the upper left
        // corner of the box and (x2, y2) is the lower right corner.
        //
        // The Qt coordinate system uses the upper left corner as the origin
        // but defines upper left and lower right visually. Thus, the x coordinate
        // of the upper left corner of a box is less than the x coordinate of
        // the lower right corner but the y coordinate of the upper left corner
        // is less than the y coordinate of the lower right corner.

        if (x1 > x2)
        {
            float temp = x1;
            x1 = x2;
            x2 = temp;
        }

        if (y1 > y2)
        {
            float temp = y1;
            y1 = y2;
            y2 = temp;
        }

        // Create a new highlight box, send it to the BookmarkControl,
        // and set it in the browser.

        QRectF rect = QRectF(QPointF(x1,y1), QPointF(x2,y2));

        emit highlightBoxDrawn(rect);
        drawingHighlightBox = false;
        haveHighlightBox = true;
        highlightBox = rect;
        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        timer->stop();
        dragging = false;
        if (dragPoint != event->pos())
        {
            int offx = (dragPoint.x() - event->x())/zoom;
            int offy = (dragPoint.y() - event->y())/zoom;
            updateSubImage(offx, offy);
            updateTexture();
        }
    }
    else if (event->button() == Qt::MidButton)
    {
        QApplication::restoreOverrideCursor();
        timer->stop();
        lightChanged = false;
        dxLight = (event->x() - dragPoint.x())/(zoom*2);
        dyLight = (event->y() - dragPoint.y() )/(zoom*2);
        if (dxLight != 0 || dyLight != 0)
            emit moveLight(dxLight / img->width(), dyLight / img->height());
    }
    else if (event->button() == Qt::RightButton)
    {
        QApplication::restoreOverrideCursor();
        timer->stop();
        lightChangedRight = false;
        if(dynamic_cast<DetailEnhancement*>(img->getSupportedRendering()->value(img->getCurrentRendering())))
        {
            QPoint point = event->pos();
            int x = subimg.x() + (point.x() - (_width - viewWidth)/2)/zoom;
            int y = subimg.y() + (point.y() - (_height - viewHeight)/2)/zoom;
            if (x >= 0 && y >=0 && x < img->width() && y < img->height())
            {
                vcg::Point3f pixelLight = ((DetailEnhancement*) img->getSupportedRendering()->value(img->getCurrentRendering()))->getPixelLight(x, y);
                emit setLightDir(pixelLight, false);
            }
        }
        else
        {
            nextDragPoint= event->pos();
            dxLight = ((event->x() - dragPoint.x())/ static_cast<float>(_width)) * M_PI;
            dyLight = ((event->y() - dragPoint.y())/ static_cast<float>(_height)) * M_PI;
            updateLight();
        }
    }

}


void RtiBrowser::wheelEvent(QWheelEvent *event)
{
    if (!img) return;
	// Updates the zoom info.
	if (zoom == minZoom && event->delta() < 0) return;
	if (zoom == maxZoom && event->delta() > 0) return;
    zoom = zoom + event->delta()*0.001;
    if (zoom > maxZoom)
        zoom = maxZoom;
    else if (zoom < minZoom)
        zoom = minZoom;
	emit updateZoomValue(zoom, minZoom);
    updateZoomimg();
    updateTexture();
}


void RtiBrowser::mousefloatClickEvent(QMouseEvent *event)
{
    if (!img) return;
    if (event->button() == Qt::LeftButton)
    {
        QPoint point = event->pos();
        QPoint center(subimg.x() + (point.x() - (_width - viewWidth)/2)/zoom, subimg.y() + (point.y() - (_height - viewHeight)/2)/zoom);
        if (zoom < maxZoom)
        {
            zoom *= 1.4f;
            if (zoom > maxZoom)
                zoom = maxZoom;
			emit updateZoomValue(zoom, minZoom);
            updateZoomimg();
        }
        subimg.moveCenter(center);
        updateSubImage(0, 0);
        updateTexture();
    }
}


void RtiBrowser::leaveEvent(QEvent *event)
{
    if (focus)
    {
        QApplication::restoreOverrideCursor();
        focus = false;
    }
}



void RtiBrowser::fired()
{
    if(interactive && posUpdated)
    {
        if (dragging)
            updateTexture();
        else if (lightChanged)
            emit moveLight(dxLight / img->width(), dyLight / img->height());
        else if (lightChangedRight)
            updateLight();
    }
}



void RtiBrowser::zoomOutActivated()
{
    if (!img) return;
    if (zoom == minZoom) return;
    if (zoom <= 1)
    {
        int x = floor(log(2.0/zoom)/log(2.0));
        float temp = 1.0 / pow(2.0, x);
        zoom = temp < minZoom ? minZoom : temp;
    }
    else
        zoom  = ceil(zoom - 1);

	emit updateZoomValue(zoom, minZoom);
    updateZoomimg();
    updateTexture();
}


void RtiBrowser::zoomInActivated()
{
    if (!img) return;
    if (zoom == maxZoom) return;
    if (zoom >= 1)
        zoom = floor(zoom + 1);
    else
    {
        int x = ceil(log(1.0/(zoom*2.0))/log(2.0));
        zoom = 1/pow(2.0, x);
    }

	emit updateZoomValue(zoom, minZoom);
    updateZoomimg();
    updateTexture();
}

void RtiBrowser::addHighlightBox()
{
    // Start drawing a new highlight box.

    addingHighlightBox = true;
}

void RtiBrowser::editHighlightBox()
{
    // Start editing an existing highlight box.

    editingHighlightBox = true;
}

void RtiBrowser::setHighlightBox(bool drawHighlightBox, QRectF box)
{
    // Set the highlight box. This is sent by the BookmarkControl
    // when the user selects a new bookmark.

    haveHighlightBox = drawHighlightBox;
    highlightBox = box;
    updateTexture();
}

void RtiBrowser::deleteHighlightBox()
{
    // Delete the existing highlight box.

    haveHighlightBox = false;
}

QPointF RtiBrowser::getCorner(int corner, QRectF rect)
{
    // Get the specified corner of a rectangle.

    if (corner == 0)
        return rect.topLeft();
    else if (corner == 1)
        return rect.topRight();
    else if (corner == 2)
        return rect.bottomRight();
    else // if (corner == 3)
        return rect.bottomLeft();
}

void RtiBrowser::imageToBrowser(float & x, float & y)
{
    // Convert a point from the coordinates used by the image to the
    // coordinates used by the browser widget. For more information,
    // see the comments in browserToImage().

    x = (_width - viewWidth)/2.0 + (x - subimg.x()) * zoom;
    y = (_height - viewHeight)/2.0 + (y - subimg.y()) * zoom;
}

void RtiBrowser::browserToImage(float & x, float & y)
{
    // Convert a point from the coordinates used by the browser widget
    // to the coordinates used by the image.

    // Working backwards:
    //
    // a) The browser window might be larger than the viewport, so
    //    (_width - viewWidth)/2.0 is the space from the left side
    //    of the browser window to the left edge of the viewport.
    //
    // b) The X coordinate is relative to the browser widget.
    //
    // c) The X coordinate minus the space calculated in (a) gives the
    //    X coordinate relative to the viewport.
    //
    // d) The value calculated in (c) divided by the zoom gives the X
    //    coordinate in the coordinates used by the subimage being displayed.
    //
    // e) The value in (d) shifted by the X origin of the subimage gives
    //    the X coordinate in the coordinates used by the image.

    x = subimg.x() + (x  - (_width - viewWidth)/2.0)/zoom;
    y = subimg.y() + (y  - (_height - viewHeight)/2.0)/zoom;
}

void RtiBrowser::setLight(vcg::Point3f l, bool refresh)
{
    light = l;
    if (img)
    {
        if (refresh)
            updateTexture();
    }
}


void RtiBrowser::updateLight()
{
    dragPoint = nextDragPoint;
    float ro, theta, phi;
    lastLight.ToPolarRad(ro, theta, phi);
    if (theta - dxLight < 0.0)
        dxLight = theta;
    else if (theta - dxLight > M_PI)
        dxLight = theta - M_PI;
    if (phi - dyLight > M_PI / 2.0)
        dyLight = phi - M_PI / 2.0;
    else if (phi - dyLight < -M_PI / 2.0)
        dyLight = M_PI / 2.0 + phi;
    vcg::Matrix33f rotX, rotY;
    rotX.SetRotateRad(dyLight, vcg::Point3f(1,0,0));
    rotY.SetRotateRad(dxLight, vcg::Point3f(0,1,0));
    vcg::Point3f temp = rotX*rotY*lastLight;
    if (temp[2] < 0)
        temp[2] = 0;
    temp.Normalize();
    emit setLightDir(temp, true);
    lastLight = temp;
    dxLight = 0;
    dyLight = 0;
}


void RtiBrowser::updateViewSize()
{
    float ratio = static_cast<float>(img->width()) / static_cast<float>(img->height());
    int h = _height > maxHeight ? maxHeight: _height;
    int w = _width > maxWidth ? maxWidth: _width;

    if (img->height() > h)
        viewHeight = h;
    else
        viewHeight = img->height();

    if (img->width() > w)
        viewWidth = w;
    else
        viewWidth = img->width();

    float ratio2 = static_cast<float>(viewWidth) / static_cast<float>(viewHeight);
    if (ratio2 != ratio)
    {
        if (viewWidth / ratio <= h)
            viewHeight = viewWidth / ratio;
        else
            viewWidth = viewHeight * ratio;
    }
}


void RtiBrowser::updateZoomimg()
{
    int h = _height > maxHeight ? maxHeight: _height;
    int w = _width > maxWidth ? maxWidth: _width;

    if (img->width() * zoom < w)
        viewWidth = img->width() * zoom;
    else
        viewWidth  = w;

    if (img->height() * zoom < h)
        viewHeight = img->height() * zoom;
    else
        viewHeight = h;

    float h1 = viewHeight / zoom;
    if ( h1 > img->height())
        h1 = img->height();

    float w1 = viewWidth / zoom;
    if (w1 > img->width())
        w1 = img->width();

    float px = subimg.x() + subimg.width() / 2.0 - w1 / 2.0;
    if (px < 0)
        px = 0;
    else if (px + w1 > img->width())
        px = img->width() - w1;

    float py = subimg.y() + subimg.height() / 2.0 - h1 / 2.0;
    if (py < 0)
        py = 0;
    else if (py + h1 > img->height())
        py = img->height() - h1;

    subimg = QRectF(px, py, w1, h1);
    emit viewChanged(subimg);
    level = zoom >= 1 ? 0 :  floor(log(1.0/zoom)/log(2.0));
}


void RtiBrowser::updateTexture(bool refresh)
{
    if (textureData)
        delete textureData;
    QTime first = QTime::currentTime();
    img->createImage(&textureData, textureWidth, textureHeight, light, subimg, level, currentMode);
    isNewTexture = true;
    if (refresh)
        updateGL();
    QTime second = QTime::currentTime();
    if (first.msecsTo(second) > 120)
    {
        emit setInteractiveLight(false);
        interactive = false;
    }
    else
    {
        emit setInteractiveLight(true);
        interactive = true;
    }
}


void RtiBrowser::updateSubImage(int offx, int offy)
{
    float x, y;
    if (subimg.x() + offx < 0)
        x = 0;
    else if (subimg.x() + offx + subimg.width() > img->width())
        x = img->width() - subimg.width();
    else
        x = subimg.x() + offx;

    if (subimg.y() + offy < 0)
        y = 0;
    else if (subimg.y() + offy + subimg.height() > img->height())
        y = img->height() - subimg.height();
    else
        y = subimg.y() + offy;

    subimg.moveTo(x, y);
    emit viewChanged(subimg);
}



void RtiBrowser::setRenderingMode(int mode)
{
    img->setRenderingMode(mode);
    QMap<int, RenderingMode*>* list = img->getSupportedRendering();
    RenderingMode* rendering = list->value(mode);
    emit setInteractiveLight(rendering->isLightInteractive());
    emit setEnabledLight(rendering->enabledLighting());
    interactive = rendering->isLightInteractive();
    updateTexture();
}


void RtiBrowser::updateImage()
{
    updateTexture();
}


void RtiBrowser::updateView(QRectF rect, bool resize)
{
    subimg = rect;
    int h = _height > maxHeight ? maxHeight: _height;
    int w = _width > maxWidth ? maxWidth: _width;

	if (resize)
	{
		float zoom1 = h / rect.height();
		float zoom2 = w / rect.width();
		zoom = zoom1 > zoom2? zoom2 : zoom1;
		if (zoom > maxZoom)
			zoom = maxZoom;
		else if (zoom < minZoom)
			zoom = minZoom;
	}
	
	emit updateZoomValue(zoom, minZoom);
	updateZoomimg();
    updateTexture();
}


void RtiBrowser::defaultModeActivated()
{
    if (!img) return;
    currentMode = DEFAULT_MODE;
    updateTexture();
}


void RtiBrowser::smoothModeActivated()
{
    if (!img) return;
    currentMode = SMOOTH_MODE;
    updateTexture();
}


void RtiBrowser::contrastModeActivated()
{
    if (!img) return;
    currentMode = CONTRAST_MODE;
    updateTexture();
}


void RtiBrowser::enhancedModeActivated()
{
    if (!img) return;
    currentMode = ENHANCED_MODE;
    updateTexture();
}


void RtiBrowser::lumModeActivated()
{
    if (!img) return;
    currentMode = LUM_MODE;
    updateTexture();
}


void RtiBrowser::lumUnsharpModeActivated()
{
    if (!img) return;
    currentMode = LUM_UNSHARP_MODE;
    updateTexture();
}


void RtiBrowser::rgbModeActivated()
{
    if (!img) return;
    currentMode = RGB_MODE;
    updateTexture();
}


void RtiBrowser::lumRModeActivated()
{
    if (!img) return;
    currentMode = LUMR_MODE;
    updateTexture();
}


void RtiBrowser::lumGModeActivated()
{
    if (!img) return;
    currentMode = LUMG_MODE;
    updateTexture();
}


void RtiBrowser::lumBModeActivated()
{
    if (!img) return;
    currentMode = LUMB_MODE;
    updateTexture();
}


void RtiBrowser::downloadFinished()
{
    if(!img) return;
    img->resetRemote();
    emit updateRenderingList(img->getCurrentRendering(), false);
}


void RtiBrowser::a0ModeActivated()
{
    if (!img) return;
    currentMode = A0_MODE;
    updateTexture();
}


void RtiBrowser::a1ModeActivated()
{
    if (!img) return;
    currentMode = A1_MODE;
    updateTexture();
}


void RtiBrowser::a2ModeActivated()
{
    if (!img) return;
    currentMode = A2_MODE;
    updateTexture();
}


void RtiBrowser::a3ModeActivated()
{
    if (!img) return;
    currentMode = A3_MODE;
    updateTexture();
}


void RtiBrowser::a4ModeActivated()
{
    if (!img) return;
    currentMode = A4_MODE;
    updateTexture();
}


void RtiBrowser::a5ModeActivated()
{
    if (!img) return;
    currentMode = A5_MODE;
    updateTexture();
}


void RtiBrowser::lightVectorModeActivated()
{
    if (!img) return;
    currentMode = LIGHT_VECTOR;
    updateTexture();
}


void RtiBrowser::lightVectorMode2Activated()
{
    if (!img) return;
    currentMode = LIGHT_VECTOR2;
    updateTexture();
}


void RtiBrowser::snapshotActivated()
{
    if (!img) return;
    QString fileName = QFileDialog::getSaveFileName(this->parentWidget(), tr("Save snapshot"), gui->getRTIFileInfo().absolutePath() + "/snapshot.jpg", tr("JPEG (*.jpg *.jpeg);; PNG (*.png)"));
    if (fileName == "") return;
	unsigned char* imgBuffer;
	int tempW = 0;
	int tempH = 0;
	img->createImage(&imgBuffer, tempW, tempH, light, subimg);  
    QImage snapshotImg(tempW, tempH, QImage::Format_RGB888);
    QRgb value;
    for (int j = 0; j < tempH; j++)
    {
        for (int i = 0; i < tempW; i++)
        {
            int offset = j * tempW + i;
            value = qRgb(imgBuffer[offset*4], imgBuffer[offset*4 + 1], imgBuffer[offset*4 + 2]);
            snapshotImg.setPixel(i, j, value);
        }
    }

    // Draw the bookmark's highlight box, if any.

    if (haveHighlightBox)
    {
        // Create a new QPainter to draw on the image

        QPainter painter(&snapshotImg);

        // Create a new pen with color white and width 1.

        QPen pen(Qt::white, 1);
        pen.setJoinStyle(Qt::MiterJoin);
        painter.setPen(pen);

        // Create two boxes and draw them. We draw two boxes (one black and
        // one white) so the box will show up against any background.

        // We use integers because floating point calculations occasionally
        // lead to unexpected results. In particular, the outer edge of the
        // outer box is supposed to be two pixels away from the inner box, but it
        // is sometimes one pixel and sometimes three pixels away when floating
        // point calculations are used. This problem does not occur with integer
        // calculations.

        // The coordinates of the highlight box are relative to the upper left
        // corner of the entire picture, while the coordinates of the snapshot
        // are relative to the upper left corner of the current sub-image. Therefore,
        // we transform the highlight box coordinates to sub-image coordinates by
        // subtracting the upper left coordinates of the sub-image, which are relative
        // to the entire picture.

        int x1 = static_cast<int>(highlightBox.topLeft().x() - subimg.topLeft().x()),
            y1 = static_cast<int>(highlightBox.topLeft().y() - subimg.topLeft().y()),
            x2 = static_cast<int>(highlightBox.bottomRight().x() - subimg.topLeft().x()),
            y2 = static_cast<int>(highlightBox.bottomRight().y() - subimg.topLeft().y());

        QPoint points[5] =
        {
            QPoint(x1, y1),
            QPoint(x2, y1),
            QPoint(x2, y2),
            QPoint(x1, y2),
            QPoint(x1, y1)
        };
        painter.drawPolyline(points, 5);

        // Set the pen color to black and the width to 2. Note that we
        // call setPen again. This is because setPen copies the QPen
        // object and changes to pen color and width aren't passed to
        // QPainter without the second call. (Yes, I know this is obvious
        // in C++, but I've got a Java mind...)

        pen.setColor(Qt::black);
        pen.setWidth(2);
        painter.setPen(pen);

        // Create an outer box and draw it. Whether we add/subtract 1 or 2
        // depends on how the line (which is two pixels wide) is centered on
        // the specified pixel. In particular, the second pixel of horizontal
        // lines is drawn above the specified pixel and the second pixel of
        // vertical lines is drawn to the left of the specified pixel.

        points[0] = QPoint(x1 - 1, y1 - 1);
        points[1] = QPoint(x2 + 2, y1 - 1);
        points[2] = QPoint(x2 + 2, y2 + 2);
        points[3] = QPoint(x1 - 1, y2 + 2);
        points[4] = QPoint(x1 - 1, y1 - 1);

        painter.drawPolyline(points, 5);
    }

    snapshotImg.save(fileName, 0, 100);
	delete[] imgBuffer;

    saveSnapshotMetadata(fileName);
}

void RtiBrowser::saveSnapshotMetadata(QString fileName)
{
    // This function saves the metadata about a snapshot to an XMP file.

    // Create a new DOM document.

    QDomDocument xmp;

    // Add the XML declaration

    QDomProcessingInstruction header = xmp.createProcessingInstruction( "xml", "version=\"1.0\"" );
    xmp.appendChild(header);

    // Create the rdf:RDF element.
    //
    // Note that QDomDocument writes namespace declarations on every element
    // that uses a namespace prefix or has an attribute that uses a namespace
    // prefix. This leads to an unnecessarily verbose XML document. As a workaround,
    // we declare the namespaces we need on the root element and use
    // QDomDocument.createElement instead of QDomDocument.createElementNS. Yech.

    QDomElement rdf = createChild(xmp, xmp, "rdf:RDF");
    rdf.setAttribute("xmlns:rdf", rdfURI);
    rdf.setAttribute("xmlns:rti", rtiURI);
    rdf.setAttribute("xmlns:xmp", xmpURI);
    rdf.setAttribute("xmlns:dc", dcURI);
    rdf.setAttribute("xmlns:stDim", stDimURI);

    // Add the rdf:Description child

    QDomElement description = createChild(xmp, rdf, "rdf:Description");
    description.setAttribute("rdf:about", "");

    // Add information about the RTIViewer.

    createChild(xmp, description, "xmp:CreatorTool", rtiViewerURL);
    createChild(xmp, description, "xmp:CreateDate", QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss"));

    // Add information about the RTI file from which the snapshot was created.

    QDomElement sourceImage = createChild(xmp, description, "rti:RTIImage");
    sourceImage.setAttribute("rdf:parseType", "Resource");

    createChild(xmp, sourceImage, "dc:source", gui->getRTIFileInfo().absoluteFilePath());
    QString imageType = img->typeFormat();
    imageType.replace(" ", "-");
    imageType = imageType.toLower();
    createChild(xmp, sourceImage, "dc:format", "image/x-rti-" + imageType);

    QDomElement imageSize = createChild(xmp, sourceImage, "rti:Size");
    createChild(xmp, imageSize, "stDim:w", QString::number(img->width()));
    createChild(xmp, imageSize, "stDim:h", QString::number(img->height()));
    createChild(xmp, imageSize, "stDim:unit", "pixel");

    // Add information about the snapshot.

    QDomElement snapshot = createChild(xmp, description, "rti:Snapshot");
    sourceImage.setAttribute("rdf:parseType", "Resource");

    // Create the dc:Identifier and rti:Size children.

    QString uuid(QUuid::createUuid().toString().mid(1,36).prepend("uuid:"));
    createChild(xmp, snapshot, "dc:Identifier", uuid);

    QDomElement subimageSize = createChild(xmp, snapshot, "rti:Size");
    createChild(xmp, subimageSize, "stDim:w", QString::number(subimg.width()));
    createChild(xmp, subimageSize, "stDim:h", QString::number(subimg.height()));
    createChild(xmp, subimageSize, "stDim:unit", "pixel");

    // Create the rti:RenderingInfo child.

    QDomElement info = createChild(xmp, snapshot, "rti:RenderingInfo");
    info.setAttribute("rdf:parseType", "Resource");

    // Create the rti:Zoom, rti:Pan, and rti:Incidence children.

    createChild(xmp, info, "rti:Zoom", QString::number(zoom * 100.0));

    QDomElement pan = createChild(xmp, info, "rti:Pan");
    pan.setAttribute("rdf:parseType", "Resource");
    createChild(xmp, pan, "rti:x", QString::number(subimg.topLeft().x()));
    createChild(xmp, pan, "rti:y", QString::number(subimg.topLeft().y()));

    QDomElement incidence = createChild(xmp, info, "rti:Incidence");
    incidence.setAttribute("rdf:parseType", "Resource");
    createChild(xmp, incidence, "rti:x", QString::number(light.X()));
    createChild(xmp, incidence, "rti:y", QString::number(light.Y()));

    // Create the rti:RenderingMode child and rti:Parameters child (if any)

    QDomElement renderingMode = createChild(xmp, info, "rti:RenderingMode");
    renderingMode.setAttribute("rdf:parseType", "Resource");
    QDomComment renderingModeComment = xmp.createComment(
        "0=DEFAULT,\n1=DIFFUSE_GAIN,\n2=SPECULAR_ENHANCEMENT,\n3=NORMAL_ENHANCEMENT,\n4=UNSHARP_MASKING_IMG,\n5=UNSHARP_MASKING_LUM,\n6=COEFF_ENHANCEMENT,\n7=DETAIL_ENHANCEMENT,\n8=DYN_DETAIL_ENHANCEMENT\n9=NORMALS_VISUALIZATION");
    renderingMode.appendChild(renderingModeComment);

    int mode = getCurrentRendering();
    createChild(xmp, renderingMode, "rti:RenderingModeID", QString::number(mode));

    if (mode != NORMALS)
    {
        QDomElement parameters = createChild(xmp, renderingMode, "rti:Parameters");
        QDomElement parametersBag = createChild(xmp, parameters, "rdf:Bag");

        switch (mode)
        {
/*
            case DEFAULT:

                {
                    // If we are using the default rendering, check if the image is a multi-
                    // view image by trying to cast the RenderingMode to DefaultMRti. If the
                    // cast succeeds, get the horizontal and vertical views.

                    // For more information about compiling this code and other issues, see
                    // the comments in BookmarkControl.displayRendering().

                    DefaultMRti* mrti = static_cast<DefaultMRti*> (getRenderingModes()->value(mode));
                    if (mrti != 0)
                    {
                        bookmark.params.append(RenderingParam("horizontalShift", mrti->getCurrentPositionX())); // WON'T COMPILE: getCurrentPositionX() does not exist
                        bookmark.params.append(RenderingParam("nearestViewpoint", mrti->getUseFlow())); // WON'T COMPILE: getUseFlow() does not exist
                        bookmark.params.append(RenderingParam("verticalShift", mrti->getCurrentPositionY())); // WON'T COMPILE: getCurrentPositionX() does not exist
                    }
                }
                break;
            */

            case DIFFUSE_GAIN:

                {
                    DiffuseGain* dg = static_cast<DiffuseGain*> (getRenderingModes()->value(mode));
                    createParameterChild(xmp, parametersBag, "gain", dg->getGain());
                }
                break;

            case SPECULAR_ENHANCEMENT:

                {
                    SpecularEnhancement* se = static_cast<SpecularEnhancement*> (getRenderingModes()->value(mode));
                    createParameterChild(xmp, parametersBag, "diffuseColor", se->getKd());
                    createParameterChild(xmp, parametersBag, "specularity", se->getKs());
                    createParameterChild(xmp, parametersBag, "highlightSize", se->getExp());
                }
                break;

            case NORMAL_ENHANCEMENT:

                {
                    NormalEnhancement* ne = static_cast<NormalEnhancement*> (getRenderingModes()->value(mode));
                    createParameterChild(xmp, parametersBag, "gain", ne->getGain());
    //                createParameterChild(xmp, parametersBag, "kd", ne->getKd());
                    createParameterChild(xmp, parametersBag, "environment", ne->getEnvIll());
                }
                break;

            case UNSHARP_MASKING_IMG:

                {
                    UnsharpMasking* um_img = static_cast<UnsharpMasking*> (getRenderingModes()->value(mode));
                    createParameterChild(xmp, parametersBag, "gain", um_img->getGain());
                }
                break;

            case UNSHARP_MASKING_LUM:

                {
                    UnsharpMasking* um_lum = static_cast<UnsharpMasking*> (getRenderingModes()->value(mode));
                    createParameterChild(xmp, parametersBag, "gain", um_lum->getGain());
                }
                break;

            case COEFF_ENHANCEMENT:

                {
                    CoeffEnhancement* ce = static_cast<CoeffEnhancement*> (getRenderingModes()->value(mode));
                    createParameterChild(xmp, parametersBag, "gain", ce->getGain());
                }
                break;

            case DETAIL_ENHANCEMENT:

                {
                    DetailEnhancement* de = static_cast<DetailEnhancement*> (getRenderingModes()->value(mode));
                    createParameterChild(xmp, parametersBag, "localOffset", (float)de->getNOffset());
                    createParameterChild(xmp, parametersBag, "tileSize", (float)de->getMinTileSize());
                    createParameterChild(xmp, parametersBag, "numberInitialTiles", (float)de->getMinLevel());
                    createParameterChild(xmp, parametersBag, "sharpnessOperator", (float)de->getSharpnessOperator());
                    createParameterChild(xmp, parametersBag, "lightSampling", (float)de->getSphereSampling());
                    createParameterChild(xmp, parametersBag, "k1Sharpness", de->getK1());
                    createParameterChild(xmp, parametersBag, "k2Lightness", de->getK2());
                    createParameterChild(xmp, parametersBag, "threshold", de->getThreshold());
                    createParameterChild(xmp, parametersBag, "smoothingFilter", (float)de->getFilter());
                    createParameterChild(xmp, parametersBag, "numberIterationSmoothing", (float)de->getNIterSmoothing());
                }
                break;

            case DYN_DETAIL_ENHANCEMENT:

                {
                    DynamicDetailEnh* dde = static_cast<DynamicDetailEnh*> (getRenderingModes()->value(mode));
                    createParameterChild(xmp, parametersBag, "degreeOffset", (float)dde->getDegreeOffset());
                    createParameterChild(xmp, parametersBag, "tileSize", (float)dde->getTileSize());
                    createParameterChild(xmp, parametersBag, "sharpnessOperator", (float)dde->getSharpnessOperator());
                    createParameterChild(xmp, parametersBag, "lightSampling", (float)dde->getSphereSampling());
                    createParameterChild(xmp, parametersBag, "k1Sharpness", dde->getK1());
                    createParameterChild(xmp, parametersBag, "k2Lightness", dde->getK2());
                    createParameterChild(xmp, parametersBag, "threshold", dde->getThreshold());
                    createParameterChild(xmp, parametersBag, "smoothingFilter", (float)dde->getFilter());
                    createParameterChild(xmp, parametersBag, "numberIterationSmoothing", (float)dde->getNIterSmoothing());
                }
                break;

            default:

                break;
        }

    }

    // Open the metadata file, save the DOM document, and close the file.

    int fileExtensionPos = fileName.lastIndexOf(".") + 1;
    fileName.replace(fileExtensionPos, fileName.length() - fileExtensionPos, "xmp");
    QFile xmpFile(fileName);
    xmpFile.open(QIODevice::WriteOnly);
    QTextStream out(&xmpFile);
    xmp.save(out, 2);
    xmpFile.close();
}

void RtiBrowser::createParameterChild(QDomDocument xmp, QDomElement parametersBag, QString name, float value)
{
    QDomElement parameterLi = createChild(xmp, parametersBag, "rdf:li");
    QDomElement param = createChild(xmp, parameterLi, "rti:Parameter");
    param.setAttribute("name", name);
    param.setAttribute("value", QString::number(value));
}

void RtiBrowser::updateZoom(int zoomVal)
{
	zoom = zoomVal / 100.0;
	if (zoom < minZoom)
		zoom = minZoom;
	if (zoom > maxZoom)
		zoom = maxZoom;
    updateZoomimg();
    updateTexture();
}

void RtiBrowser::setPanX(double x)
{
    updateView(QRectF(QPointF(x, subimg.y()), subimg.size()), false);
}

void RtiBrowser::setPanY(double y)
{
    updateView(QRectF(QPointF(subimg.x(), y), subimg.size()), false);
}

void RtiBrowser::setPan(QPointF point)
{
    updateView(QRectF(point, subimg.size()), false);
}
