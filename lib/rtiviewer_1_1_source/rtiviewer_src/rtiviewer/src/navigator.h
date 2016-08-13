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


#ifndef NAVIGATOR_H
#define NAVIGATOR_H


#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#include <cmath>
#endif

#include <QWidget>
#include <QTimer>


//! Navigator class.
/*!
  The class defines the widget to select the sub-image to display in the browser. 
*/
class Navigator: public QWidget
{
	Q_OBJECT


public:

	//! Constructor.
	/*!
	  \param parent
	  \param h height of the widget.
	  \param w width of the widget.
	  \param zoom maximum zoom level.
	*/
	Navigator(QWidget *parent, int h, int w, int zoom);

	//! Deconstructor.
	~Navigator();


protected:

	/*!
	  Paint event handler.
    */
	void paintEvent(QPaintEvent *event);
	
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
	  Wheel event handler.
    */
	void wheelEvent(QWheelEvent * event);


// private data members
private:


        const float maxZoom; /*!< Maximum zoom level. */

	int height; /*!< Height of the widget. */
	int width; /*!< Width of the widget. */

	QImage* image; /*!< Image to display in the widget. */
	
	int rtiWidth; /*!< Width of original RTI image. */
	int rtiHeight; /*!< Height of the original RTI image. */
	
	
    int browserWidth; /*!< Width of the image displayed in the widget. */
    int browserHeight; /*!< Height of the image displayed in the widget. */

    QRectF pos; /*!< Left-top corner cordinates of the image displayed in the widget. */
	QRectF selection;  /*!< Coordinates of the Sub-image in the current view of the browser. */

	QPoint dragPoint; /*!< Last point saves throughout a dragging event. */

	bool resize; /*!< Holds whether the current mouse event is a resize event. */
	bool dragging; /*!< Holds whether the current mouse event is a dragging event. */

	QTimer* timer; /*!< Timer to manage the paint event. */

//private method
private:

	/*!
	  Updates the position of the sub-image in the current view of the browser.
	  \param offx, offy offset.
	*/
	void updateSelectionPos(int offx, int offy);

	/*!
	  Updates the size of the sub-image in the current view of the browser.
	  \param offx, offy size offset.
	*/
	void updateSelectionSize(int offx, int offy);

	/*!
	  Updates the sub-image in the browser.
	*/
	void updateSubImage(bool resize);


signals:

	/*!
	  Emitted to update the sub-image rectangle in the current view of the browser.
	  \param rect sub-image rectangle.
	*/
	void selectionChanged(QRectF rect, bool resize);
	
public slots:

	/*!
	  Updates the browser size.
	  \param w width of the browser.
	  \param h height of the browser.
	*/
	void updateBrowserSize(int w, int h);

	/*!
	  Updates the sub-image rectangle in the widget.
	*/
	void updateSelection(QRectF rect);

//accessor
public:
	
	/*!
	  Sets the image to display in the widget.
	  \param img image.
	  \param rtiW height of the original RTI image.
	  \param rtiH width of the original RTI image.
	*/
	void setImage(QImage* img, int rtiW, int rtiH);

};

#endif /*NAVIGATOR_H */
