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
* along with this program.  If not, see <http://www.gnu.org/licenses/>.		*
****************************************************************************/


#ifndef SPECULARENHANC_H
#define SPECULARENHANC_H

#include "renderingmode.h"
#include "rendercontrolutils.h"

#include <vcg/space/point3.h>

//! Widget for Specular Enhancement settings.
/*!
  The class defines the widget thta is showed in the Rendering Dialog to set the parameters of the rendering mode Specular Enhancement.
*/
class SpecularEControl : public QWidget, public RenderControlUtils
{
	Q_OBJECT

public:

	//! Constructor
	/*!
	  \param kd value for diffusive constant.
	  \param ks value for specular constant.
	  \param exp value for specular exponent.
	  \param parent
	*/
    SpecularEControl(int kd, int ks, int exp,  int minExp, int maxExp, QWidget *parent = 0);

    bool eventFilter(QObject* watched, QEvent* event);

signals:

	/*!
	  Emitted when the user changes the diffusive constant.
	*/
	void kdChanged(int value);
	
	/*!
	  Emitted when the user changes the specular constant.
	*/
	void ksChanged(int value);

	/*!
	  Emitted when the user changes the specular exponent.
	*/
	void expChanged(int value);

};


//! Specular Enhancement class.
/*!
  The class defines the rendering mode Specular Enhancement.
*/
class SpecularEnhancement : public QObject, public RenderingMode
{

	Q_OBJECT

private:

	const float minKd; /*!< Minumum diffusive constant value. */
	const float maxKd; /*!< Minumum diffusive constant value. */
	static float kd; /*!< Current diffusive constant value. */ //YY static

	const float minKs; /*!< Minumum specular constant value. */
	const float maxKs; /*!< Maximum specular constant value. */
	static float ks; /*!< Current specular constant value. */ // YY

	const int minExp; /*!< Minumum specular exponent value. */
	const int maxExp; /*!< Maximum specular exponent value. */
	static int exp; /*!< Current specular exponent value. */ //YY


public:

	//! Constructor.
	SpecularEnhancement();
	
	//! Deconstructor.
	~SpecularEnhancement();
	
	virtual QString getTitle();
	virtual QWidget* getControl(QWidget* parent = 0);
	virtual bool isLightInteractive();
	virtual bool supportRemoteView();
	virtual bool enabledLighting();

	virtual void applyPtmLRGB(const PyramidCoeff& coeff, const PyramidRGB& rgb, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer);

	virtual void applyPtmRGB(const PyramidCoeff& redCoeff, const PyramidCoeff& greenCoeff, const PyramidCoeff& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer);

	virtual void applyHSH(const PyramidCoeffF& redCoeff, const PyramidCoeffF& greenCoeff, const PyramidCoeffF& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer);

    float getKd();
    float getKs();
    float getExp();
	
public slots:
	
	/*!
	  Sets the diffusive constant value.
	*/
	void setKd(int value);
	
	/*!
	  Sets the specular constant value.
	*/
	void setKs(int value);
	
	/*!
	  Sets the specular exponent value.
	*/
	void setExp(int value);

signals:

	/*!
	  Emitted to refresh the image in the browser.
	*/
	void refreshImage();

};

#endif /* SPECULARENHANCEMENT_H */
