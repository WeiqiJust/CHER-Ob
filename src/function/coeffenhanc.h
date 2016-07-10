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


#ifndef COEFFENHANCEMENT_H
#define COEFFENHANCEMENT_H

#include "renderingmode.h"
#include "rendercontrolutils.h"

#include <vcg/space/point3.h>

/**
 * @brief Widget for Coefficient Enhancement settings.
 */

/**
 * The class defines the widget that is showed in the Rendering Dialogto to set the parameters of the rendering mode Coefficient Enhancement.
 */
class CoeffEnhancControl : public QWidget, public RenderControlUtils
{
	Q_OBJECT

public:

	/**
	 * @brief  Constructor.
	 * @param  gain  Value for the gain parameter.
	 */
	CoeffEnhancControl(int gain, QWidget *parent = 0);

	/**
	 * @brief  Filter events, overload from QWidget.
	 */
    bool eventFilter(QObject* watched, QEvent* event);

signals:

	/**
	 * @brief  Emitted when the user changes the gain value.
	 * @param  value  The changed value.
	 */
	void gainChanged(int value);
};


/**
 * The class is Coefficient Enhancement class, which defines the rendering mode Coefficient Enhancement.
 */
class CoeffEnhancement : public QObject, public RenderingMode
{

	Q_OBJECT

private:

	float minGain; /*!< Minimum gain value. */ 
	float maxGain; /*!< Maximum gain value. */
	static float gain; /*!< Current gain value. */ // YY

	int nIter; /*!< Number of smoothing iterations. */

	
public:

	/**
	 * @brief  Constructor.
	 */
	CoeffEnhancement();

	/**
	 * @brief  Deconstructor.
	 */
	~CoeffEnhancement();
	
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

	/**
	 * @brief  Computes the smoothed version of the coefficients.
	 * @param  coeffMap pointer to coefficients map.
	 * @param  width width in pixel of the map.
	 * @param  height height in pixel of the map.
	 * @param  ncomp number of coefficient per pixel,
	 */
	void enhancedCoeff(PTMCoefficient* coeffMap, int width, int height, int ncomp);
	
public slots:

	/**
	 * @brief  Sets the gain value.
	 * @param  value  the input gain value.
	 */
	void setGain(int value);

signals:

	/*!
	  Emitted to refresh the image in the browser.
	*/
	void refreshImage();
};

#endif // COEFFENHANCEMENT_H
