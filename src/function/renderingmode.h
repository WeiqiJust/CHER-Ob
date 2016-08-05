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


#ifndef RENDERINGMODE_H
#define RENDERINGMODE_H

#include "../io/pyramid.h"
#include "../io/util.h"

#include <QString>
#include <QWidget>
#include <QDialog>

//! Rendering mode abstract class
/*!
  Abstract class for the rendering modes to apply to RTI image.
*/
class RenderingMode
{
	
//public method
public:

	//! Deconstructor
	virtual ~RenderingMode() {}

	/*!
	  Returns the name of the rendering mode.
	  \return a string.
	*/
	virtual QString getTitle() = 0;

	/*!
	  Returns the widget to set the parameters of the rendering mode.
	  \param parent parent of the widget.
	  \return widget of the parameters.
    */
	virtual QWidget* getControl(QWidget* parent = 0)= 0;
	
	/*!
	  Returns info about the updating of light vector throughout the mouse action.
	  \return \a true if the light vector updating is interactive, \a false otherwise (the light
				is updated only when the mouse interaction is finished).
	*/
	virtual bool isLightInteractive() = 0;

	/*!
	  Returns info about the application of the rendering mode when a remote RTI image is opened.
	  \return \a true if the mode can be apply throughout the opening of a remote RTI image, \a false otherwise.
				When all tiles of remote RTI image is received the mode can be applied again, even if the method returns \a false.
	*/
	virtual bool supportRemoteView() = 0;

	/*!
	  Returns info about the type of rendering mode: static (the user can not select the light vector direction);
	  dynamic (the user can select the light vector direction).
	  \return \a true if the mode is dynamic, false if it is static.
	*/
	virtual bool enabledLighting() = 0;

	/*!
	  Applies the rendering mode to a LRGB-PTM.
	  \param coeff luminance coefficients.
	  \param rgb RGB components.
	  \param mipMapSize size in pixel of mip-mapping levels.
	  \param normals normals per pixel.
	  \param info several info needed to the rendering(sub-image coordinates, mip-mapping level, light vector).
	  \param buffer pointer to the buffer to fill with the output of the rendering mode. 
	*/
	virtual void applyPtmLRGB(const PyramidCoeff& coeff, const PyramidRGB& rgb, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer) = 0;

	/*!
	  Applies the rendering mode to a RGB-PTM.
	  \param redCoeff coefficients of the red component.
	  \param greenCoeff coefficients of the green component.
	  \param blueCoeff coefficients of the blue component.
	  \param mipMapSize size in pixel of mip-mapping levels.
	  \param normals normals per pixel.
	  \param info several info needed to the rendering(sub-image coordinates, mip-mapping level, light vector).
	  \param buffer pointer to the buffer to fill with the output of the rendering mode. 
	*/
	virtual void applyPtmRGB(const PyramidCoeff& redCoeff, const PyramidCoeff& greenCoeff, const PyramidCoeff& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer) = 0;

	/*!
	  Applies the rendering mode to a HSH image.
	  \param redCoeff coefficients of the red component.
	  \param greenCoeff coefficients of the green component.
	  \param blueCoeff coefficients of the blue component.
	  \param mipMapSize size in pixel of mip-mapping levels.
	  \param normals normals per pixel.
	  \param info several info needed to the rendering(sub-image coordinates, mip-mapping level, light vector).
	  \param buffer pointer to the buffer to fill with the output of the rendering mode. 
	*/
	virtual void applyHSH(const PyramidCoeffF& redCoeff, const PyramidCoeffF& greenCoeff, const PyramidCoeffF& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer) = 0;

};

#endif
