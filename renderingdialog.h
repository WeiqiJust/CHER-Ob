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


#ifndef RENDERINGDIALOG_H
#define RENDERINGDIALOG_H

#include "renderingmode.h"

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QWheelEvent>

class MyComboBox: public QComboBox
{

public:
	MyComboBox(QWidget* parent) : QComboBox(parent)
	{

	};

protected:

	void wheelEvent(QWheelEvent* e)
	{
		e->ignore();
	};


};


//! Rendering dialog class.
/*!
  The class defines the widget to select the rendering mode and to show the dialog used to set the parameters. 
*/
class RenderingDialog: public QWidget 
{

Q_OBJECT

// private data members
private:
	
	MyComboBox* modeList; /*!< Combobox to select the rendering mode. */
	QWidget* control; /*!< Widget to set the parameters of the rendering mode. */
	QMap<int, RenderingMode*>* list; /*!< List of rendering modes. */

// construction
public:

	//! Constructor.
	/*!
	  \param l list of rendering modes.
	  \param currRendering current rendeing mode.
	  \param parent
	  \param remote flag for remote RTI image.
	*/
	RenderingDialog(QMap<int, RenderingMode*>* l, int currRendering, QWidget *parent = 0, bool remote = false);


// private Qt slots
private slots:
	
	/*!
	  Invoked when the user changes the rendeing mode.
	*/
	void renderingModeUpdate(int index);

// public Qt slots
public slots:

	/*!
	  Updates the current rendering mode.
	  \param currRendering current rendering mode.
	  \param remote  flag for remote RTI image.
	*/
	void updateRenderingList(int currRendering, bool remote);

    void setRenderingMode(int currRendering);

	void updateRTIRendDlg(QMap<int, RenderingMode*>* l, int currRendering); // YY

//public Qt signal
signals:

	/*!
	  Emitted when the user changes the rendering mode.
	*/
	void renderingModeChanged(int mode);

	/*!
	  Emitted to update the image in the browser.
	*/
	void updateImage();

	/*!
	  Emitted to indicate the finish of the downloading of a remote RTI.
	*/
	void resetRemote();

//accessor
public:
	
	/*!
	  Sets a new rendering mode list.
	*/
	void setRenderingMode(QMap<int, RenderingMode*>* l, int currRendering, bool remote = false);

    /*!
      Get the rendering-specific control.
     */
    QWidget * getRenderingControl();
	
};

#endif /* RENDERINGDIALOG_H */
