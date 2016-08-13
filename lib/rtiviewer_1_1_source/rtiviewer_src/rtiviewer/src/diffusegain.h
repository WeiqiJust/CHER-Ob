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


#ifndef DIFFUSEGAIN_H
#define DIFFUSEGAIN_H

#include "renderingmode.h"
#include "rendercontrolutils.h"

//! Widget for Diffuse Gain settings.
/*!
  The class defines the widget that is showed in the Rendering Dialog to set the parameters of the rendering mode Diffuse Gain.
*/
class DiffuseGControl : public QWidget, public RenderControlUtils
{
	Q_OBJECT

public:

	//! Constructor
	/*!
      \param gain value for the gain parameter.
	  \param parent
	*/
    DiffuseGControl(int gain, QWidget *parent = 0);

    bool eventFilter(QObject* watched, QEvent* event);

signals:

	/*!
	  Emitted when the user changes the gain value.
	*/
    void gainChanged(int value);

};


//! Diffuse Gain class.
/*!
  The class defines the rendering mode Diffuse Gain.
*/
class DiffuseGain : public QObject, public RenderingMode
{

	Q_OBJECT

private:

	const float minGain; /*!< Minimum gain value. */
	const float maxGain; /*!< Maximum gain value. */
	float gain; /*!< Current gain value. */

public:

	//! Constructor.
	DiffuseGain();

	//! Deconstructor.
	~DiffuseGain();
	
    virtual QString getTitle();
	virtual QWidget* getControl(QWidget* parent = 0);
	virtual bool isLightInteractive();
	virtual bool supportRemoteView();
	virtual bool enabledLighting();

	virtual void applyPtmLRGB(const PyramidCoeff& coeff, const PyramidRGB& rgb, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer);
	
	virtual void applyPtmRGB(const PyramidCoeff& redCoeff, const PyramidCoeff& greenCoeff, const PyramidCoeff& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer);

	void applyHSH(const PyramidCoeffF& redCoeff, const PyramidCoeffF& greenCoeff, const PyramidCoeffF& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer){}

    float getGain();

private:

	/*!
	  Applies the Diffuse Gain on one pixel.
	  \param a array of six coefficients.
	  \param nu, nv projections of the pixel normal on uv plane.
	  \param lu, lv projection of the light vector on uv plane.
	  \return the output value.
	*/
        float applyModel(const int* a, float nu, float nv, float lu, float lv);

public slots:

	/*!
	  Sets the gain value.
	*/
	void setGain(int value);

signals:

	/*!
	  Emitted to refresh the image in the browser.
	*/
	void refreshImage();
};

#endif /* DIFFUSEGAIN_H */
