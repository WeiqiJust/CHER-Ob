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

#ifndef DYN_DETAILENHANCEMENT_H
#define DYN_DETAILENHANCEMENT_H

#include "renderingmode.h"
#include "../io/util.h"

#include <vcg/space/point3.h>
#include <vcg/space/point2.h>
#include <vcg/math/gen_normal.h>
#include <vcg/math/base.h>
#include <vcg/math/matrix33.h>

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QImage>

static const int MAX_TILE_SIZE = 32; /*!< Maximum size of the tile in pixel. */

static const int MIN_OFFSET = 1; /*!< Minimum offset in degree from current light vector. */

static const int MAX_OFFSET = 20; /*!< Maximum offset in degree from current light vector. */

/*!
  Sharpness operator.
*/
enum SharpnessMeasuresDyn{
	DYN_MAX_LAPLACE, /*!< Maximum laplacian. */
	DYN_MAX_ENERGY_LAPLACE, /*!< Energy of laplacian. */
	DYN_NORM_L1_SOBEL, /*!< L1-norm of Sobel operator. */
	DYN_NORM_L2_SOBEL, /*!< L2-norm od SObel operator. */
};


/*!
  Type of light sampling.
*/
enum SphereSamplingDyn{
	DYN_UNIFORM, /*!< Isotropic. */
	DYN_NON_UNIFORM, /*!< Anisotropic. */
};


/*!
  Smoothing filter size.
*/
enum SmoothingFilterDyn
{
	DYN_3x3 = 3,
	DYN_5x5 = 5,
	DYN_7x7 = 7,
};



//! Dialog for advanced settings of Dynamic Detail Enhancement (or Dynamic Multi-light Detail Enhancement).
/*!
  The class defines the dialog to set the advanced settings of the rendering mode Dynamic Detail Enhancement (or Dynamic Multi-light Detail Enhancement).
*/
class DynamicDetConfDlg : public QDialog
{
	Q_OBJECT

// private data members
private:
	
	QComboBox* sharpnessOpCmb; /*!< Combobox to select the sharpness operator. */
	QComboBox* sphereSamplCmb; /*!< Combobox to select the type of light sampling. */
	QDoubleSpinBox* k1Snb; /*!< Spinbox to select the weight for the lightness of the tile. */
	QDoubleSpinBox* k2Snb; /*!< Spinbox to select the weight for the sharpness of the tile. */
	QDoubleSpinBox* thresholdSnb; /*!< Spinbox to select a threshold for the enhancement measure. */
	QComboBox* smootingFilterCmb; /*!< Combobox to select the smooting filter size. */
	QSpinBox* iterSmoothingSnb; /*!< Spinbox to select the number of iterations for the smoothing filter. */
	
	QPushButton* applyBtn; /*!< Apply button. */
	QPushButton* exitBtn; /*!< Exit Button*/
	QDialogButtonBox* buttonBox;

public:

	//! Constructor.
	DynamicDetConfDlg(QWidget *parent=0);

	/*!
	  Sets the values in the dialog.
	*/
	void setCurrentValue(SharpnessMeasuresDyn sharpnessOp, SphereSamplingDyn sphereSampl, float k1, float k2, float threshold, SmoothingFilterDyn smoothFilter, int nIter);

// public Qt slots
public slots:
	
	/*!
	  Invoked when the user press the Apply button.
	*/
	void applyPressed();

private slots:

	/*!
	  Invoked when the user changes the weight for the lightness.
	*/
	void k1Changed(double value);
	
	/*!
	  Invoked when the user changes the weight for the sharpness.
	*/
	void k2Changed(double value);

signals:

	/*!
	  Emitted to update the output of the rendering.
	*/
	void configUpdated(SharpnessMeasuresDyn sharpnessOp, SphereSamplingDyn sphereSampl, float k1, float k2, float threshold, SmoothingFilterDyn smoothFilter, int nIter);
};



//! Widget for Dynamic Detail Enhancement (or Dynamic Multi-Light Detail Enhancement) settings.
/*!
  The class defines the widget that is showed in the Rendering Dialog to set the parameters of rendering mode Dynamic Detail Enhancement (or Dynamic Multi-Light Detail Enhancement). 
*/
class DynamicDetailEControl : public QWidget
{
	Q_OBJECT

private:
	
	QComboBox* tileSizeCmb; /*!< Combobox to select the size of the tile. */
	QSpinBox* offsetCmb; /*!< Spinbox to select the offset in degree from the current light direction. */
	QPushButton* advancedBtn; /*!< Button to show the dialog for the advanced settings. */

	DynamicDetConfDlg* config; /*!< Advanced settings dialog. */

public:

	//! Constructor.
	/*!
	  \param tileSize initial size in pixel of the tile.
	  \param offset initial offset in degree from current light direction.
	  \param parent
	*/
	DynamicDetailEControl(unsigned int tileSize, unsigned int offset, QWidget *parent = 0);
	
	//! Deconstructor.
	~DynamicDetailEControl();
	
	/*!
	  Sets the advanced settings.
	*/
	void setAdvacedDlg(QDialog* dlg);

signals:

	/*!
	  Emitted when the user changes the size of the tile.
	*/
	void tileSizeChanged(int size);

	/*!
	  Emitted when the user changes the offset.
	*/
	void offsetChanged(int offset);

public slots:
    void setTileSize(int s);

    void setOffset(int o);

private slots:

	/*!
	  Invoked when the user changes the the size of the tile.
	*/
	void changeTileSize(int s);

	/*!
	  Shows the advanced settinsgs dialog.
	*/
	void showAdvancedSettings();

};


//! Dynamic Detail Enhancement (or Dynamic Multi-Light Detail Enhancement) class
/*!
  The class defines the rendering mode Dynamic Detail Enhancement (or Dynamic Multi-Light Detail Enhancement).
*/
class DynamicDetailEnh : public QObject, public RenderingMode
{

	Q_OBJECT

private:

	int degreeOffset; /*!< Current offset in degree from current light vector. */
	int tileSize; /*!< Current size of the tile. */
	SharpnessMeasuresDyn sharpnessOp; /*!< Current sharpness operator. */

	SphereSamplingDyn sphereSampl; /*!< Current type of light sampling. */
	float k1; /*!< Current weight for lightness. */
	float k2; /*!< Current weight for sharpness. */
	float threshold; /*!< Current threshold for enhancement measure. */
	SmoothingFilterDyn filter; /*!< Current smoothing filter size. */
	int nIterFilter; /*!< Current number of iteration for the smoothing filter. */

	unsigned char* bufferPtr; /*!< Pointer to output texture buffer.*/
	int w; /*!< Width of output texture. */
	int h; /*!< Height of the output texture. */
	const PTMCoefficient* coefficient; /*!< Pointer to luminance coefficients for LRGB-PTM. */
	const unsigned char* color; /*!< Pointer to RGB components for LRGB-PTM. */
	bool lrgb; /*!< Flag to indicate the type of PTM.*/
	const PTMCoefficient *red; /*!< Pointer to red component for RGB-PTM. */
	const PTMCoefficient *green; /*!< Pointer to green component for RGB-PTM. */
	const PTMCoefficient *blue; /*!< Pointer to blue component for RGB-PTM. */

	std::vector<vcg::Point3f> lights; /*!< List of selected light vectors for each tiles. */
	
	int drawingMode; /*!< Special rendering mode.*/

public:

	//! Constructor.
	DynamicDetailEnh();

	//! Deconstructor.
	~DynamicDetailEnh();
	
	virtual QString getTitle();
	virtual QWidget* getControl(QWidget* parent);
	virtual bool isLightInteractive();
	virtual bool supportRemoteView();
	virtual bool enabledLighting();

	virtual void applyPtmLRGB(const PyramidCoeff& coeff, const PyramidRGB& rgb, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer);

	virtual void applyPtmRGB(const PyramidCoeff& redCoeff, const PyramidCoeff& greenCoeff, const PyramidCoeff& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer);

	void applyHSH(const PyramidCoeffF& redCoeff, const PyramidCoeffF& greenCoeff, const PyramidCoeffF& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer){}

    int getDegreeOffset();
    int getTileSize();
    SharpnessMeasuresDyn getSharpnessOperator();
    SphereSamplingDyn getSphereSampling();
    float getK1();
    float getK2();
    float getThreshold();
    SmoothingFilterDyn getFilter();
    int getNIterSmoothing();

private:

	/*!
	  Computes the dynamic detail enhancement.
	  \param info rendering info.
	  \param levelWidth width of the mip-mapping level.
	*/
	void calcDetails(RenderingInfo info, int levelWidth);


	/*!
	  Computes the samples from a basis light vector.
	  \param base basis light vector.
	  \return a vector of samples.
	*/
	std::vector<vcg::Point3f>* getLightSamples(const vcg::Point3f& base);


	/*!
	  Return the array of better light vectors for a specific tile.
	  \param x, y coordinates of the left-top corner of the tile.
	  \param tileW width of the tile.
	  \param tileH width of the tile.
	  \param width width of the image.
	  \param lightSamples vector of samples to try.
	  \return the array of better light vectors. 
	*/
	std::vector<vcg::Point3f>* getBestLight(int x, int y, int tileW, int tileH, int width, const std::vector<vcg::Point3f>& lightSamples);


	/*!
	  Computes the value of the current sharpness operator on the input image.
	  \param image pointer to input image.
	  \param width width of the image.
	  \param height height of the image.
	  \return value of sharpness operator.
	*/
        float computeSharpOperator(int* image, int width, int height);


	/*!
	  Computes the light vector to apply at the pixel (x,y) by linear interpolation.
	  \param x, y coordinates of the pixel.
	  \param width width of the image.
	  \param height height of the image.
	  \param nx number of tiles on x-axis.
	  \param ny number of tiles on y-axis.
	  \return a light vector.
	*/
	vcg::Point3f getLight(int x, int y, int width, int height, int nx, int ny);


	/*!
	  Computes the global smoothing of the light vectors.
	  For each tile we select the vector nearer to the avarage of light vectors of the neighbouring tiles.
	  \param source list of best light vectors for aech tile.
	  \param dest destination vector for the output light vectors.
	  \param nx number of tiles on x-axis.
	  \param ny number of tiles on y-axis.
	*/
	void calcLocalLight(std::vector<std::vector<vcg::Point3f>*>& source, std::vector<vcg::Point3f>& dest, int nx, int ny);


	/*!
	  Computes the smoothing of the light vectors on the grid of tiles.
	  \param light vector of light vectors.
	  \param nx number of tiles on x-axis.
	  \param ny number of tiles on y-axis.
	*/
	void calcSmoothing(std::vector<vcg::Point3f>& light, int nx, int ny);

public slots:

	/*!
	  Invoked when user changes the size of the tile.
	*/
	void setTileSize(int s);
	
	/*!
	  Invoked when user changes the offset from current light direction.
	*/
	void setOffset(int x);

	/*!
	  Updates the advanced settings.
	*/
	void updateConfig(SharpnessMeasuresDyn m, SphereSamplingDyn ss, float v1, float v2, float t, SmoothingFilterDyn f, int nIter);

signals:

	/*!
	  Emitted to refresh the image in the browser.
	*/
	void refreshImage();

    void tileSizeChanged(int s);

    void offsetChanged(int o);
};


#endif
