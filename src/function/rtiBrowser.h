/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Ying Yang (ying.yang.yy368@yale.edu)

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

 - Acknowledgments: This EXR file reader is based on the original code of
   exrreadchannel for MATLAB by Edgar Velazquez-Armendariz.

*****************************************************************************/
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
* along with this program.  If not, see <http://www.gnu.org/licenses/>.	    *
*                                                                           *
* THIS IS BASED ON THE ORIGINAL FILE.                                       *
****************************************************************************/


#ifndef RTIBROWSER_H
#define RTIBROWSER_H

#include "../io/rti.h"
#include "renderingmode.h"
#include "../io/util.h"
#include "diffusegain.h"
#include "specularenhanc.h"
#include "normalenhanc.h"
#include "unsharpmasking.h"
#include "coeffenhanc.h"
#include "detailenhanc.h"
#include "dyndetailenhanc.h"
#include "normalsrendering.h"

#include <vector>

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
#include "../vtkEnums.h"

//! RTI browser class.
/*!
  The class defines the browser for RTI image.
*/
class RtiBrowser : public QWidget
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
	RtiBrowser(Rti *image, QWidget * parent);
	~RtiBrowser();

	/*!
	  Sets the RTI image to display in the browser.
	*/
	void setImage(Rti* rti);

	void setImage(Rti* rti, bool FIRST_RTI_UPDATE, bool LOADING_DONE);

	/*!
      Returns the list of rendering modes of the current RTI image.
	*/
    QMap<int, RenderingMode*>* getRenderingModes();

	/*!
	  Return the index of the rendering mode applied in the browser.
	*/
	int getCurrentRendering();

	RenderingRTI getRenderingMode();
	
// private data member
private:

	Rti* img; /*!< RTI image to display. */
    
	vcg::Point3f light; /*!< Light vector. */
	vcg::Point3f lastLight; /*!< Light vector used to update the light direction during the mouse interaction. */
	float dxLight; /*!< Offset on x-axis of light vector. */
	float dyLight; /*!< Offset on y-axis of light vector. */
	
	QRectF subimg; /*!< Sub-image diplaied in the browser. */

	int level; /*!< Mip-mapping level used. */

	int textureHeight; /*!< Height of the texture. */
	int textureWidth; /*!< Width of the texture. */
	unsigned char* textureData; /*!< Texture buffer.  */
	std::vector<unsigned char> data; 
	bool isNewTexture; /*!< Holds whether the texture is new. */

	bool interactive; /*!< Holds whether the browser can update the texture troughout the mouse interaction. If it is false the texture is update only at release event. */
	
	int currentMode; /*!< Current rendering mode applied to the image. */

	bool FIRST_RTI_UPDATE;
	
// private method
private:

	/*!
	  Updates the texture info. If \a refresh is true the texture in the browser is updated.
	*/
    void updateTexture(bool refresh = true);

// Qt signal
signals:

	
	/*!
	  Emitted to permit the interactive updating of the light.
	*/
	void setInteractiveLight(bool value);
	
	/*!
	  Emitted to enable the change of the light direction.
	*/
	void setEnabledLight(bool value);

	void rtiImageChanged(std::vector<unsigned char> data, int textureWidth, int textureHeight, bool FIRST_RTI_UPDATE); 
	void rtiImageLoadingDone(QMap<int, RenderingMode*>* l, int currRendering);

// public Qt slots
public slots:
	
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
};


#endif /* RTIBROWER_H */
