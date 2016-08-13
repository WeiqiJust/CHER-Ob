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


#ifndef DETAILENHANCEMENT_H
#define DETAILENHANCEMENT_H

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
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QImage>
#include <QVector>

/*!
  Number of light samples.
*/
enum OffsetNum{
	OFFSET_5 = 9, /*!< 9 samples. */
	OFFSET_10 = 25, /*!< 25 samples. */
	OFFSET_15 = 49, /*!< 49 samples. */
};


/*!
  Size of the tile.
*/
enum TileSize{
	TILE_SIZE_1 = 8, /*!< 8 pixel. */
	TILE_SIZE_2 = 16, /*!< 16 pixel. */
	TILE_SIZE_3 = 32, /*!< 32 pixel. */
};


/*!
  Sharpness operator.
*/
enum SharpnessMeasures{
	MAX_LAPLACE, /*!< Maximum laplacian. */
	MAX_ENERGY_LAPLACE, /*!< Maximum energy of laplacian. */
	NORM_L1_SOBEL, /*!< L1-norm of Sobel operator. */
	NORM_L2_SOBEL, /*!< L2-norm of Sobel operator. */
};


/*!
  Type of light sampling.
*/
enum SphereSampling
{
	UNIFORM, /*!< Isotropic */
	NON_UNIFORM, /*!< Anisotropic. */
};


/*!
  Smoothing filter size.
*/
enum SmoothingFilter
{
	FILTER_3x3 = 3,
	FILTER_5x5 = 5,
	FILTER_7x7 = 7,
};


//! Dialog for advanced settings of Detail Enhancement (or Static Multi-light Detail Enhancement).
/*!
  The class defines the dialog to set the advanced settings of the rendering mode Detail Enhancement (or Static Multi-light Detail Enhancement).
*/
class DetailConfDlg : public QDialog
{
	Q_OBJECT

// private data members
private:
	
	QComboBox* localOffsetCmb; /*!< Combobox to select the number of light samples. */
	QComboBox* tilesSizeCmb; /*!< Combobox to select the size of the tile. */
	QComboBox* minLevelCmb; /*!< Combobox to select the initial number of tiles. */
	QComboBox* sharpOperatorCmb; /*!< Combobox to select the sharpness operator. */
	QComboBox* sphereSamplCmb; /*!< Combobox to select the type of light sampling. */
	QDoubleSpinBox* k1Snb; /*!< Spinbox to select the weight for the lightness of the tile. */
	QDoubleSpinBox* k2Snb; /*!< Spinbox to select the weight for the sharpness of the tile. */
	QDoubleSpinBox* thresholdSnb; /*!< Spinbox to select a threshold for the enhancement measure. */
	QComboBox* filterCmb; /*!< Combobox to select the smooting filter size. */
	QSpinBox* nIterCmb; /*!< Spinbox to select the number of iterations for the smoothing filter. */

	QPushButton* okBtn; /*!< Ok button. */
	QPushButton* cancelBtn; /*!< Cancel button. */
	QDialogButtonBox* buttonBox; 

	//OffsetNum nOffset; /*!< Current number of light samples. */
	//TileSize minTileSize; /*!< Current size of the tile. */
	//int minLevel; /*!< Current initial number of tiles. */
	//SharpnessMeasures sharpnessOperator; /*!< Current sharpness operator. */
	//SphereSampling sphereSampl; /*!< Current type of light sampling. */
	//float k1; /*!< Current weight for lightness. */
	//float k2; /*!< Current weight for sharpness. */
	//float threshold; /*!< Current threshold for enhancement measure. */
	//SmoothingFilter filter; /*!< Current smoothing filter size. */
	//int nIterSmoothing; /*!< Current number of iterations for the smoothing filter. */

	static OffsetNum nOffset; /*!< Current number of light samples. */
	static TileSize minTileSize; /*!< Current size of the tile. */
	static int minLevel; /*!< Current initial number of tiles. */
	static SharpnessMeasures sharpnessOperator; /*!< Current sharpness operator. */
	static SphereSampling sphereSampl; /*!< Current type of light sampling. */
	static float k1; /*!< Current weight for lightness. */
	static float k2; /*!< Current weight for sharpness. */
	static float threshold; /*!< Current threshold for enhancement measure. */
	static SmoothingFilter filter; /*!< Current smoothing filter size. */
	static int nIterSmoothing; /*!< Current number of iterations for the smoothing filter. */

public:

	//! Constructor.
	DetailConfDlg(QWidget *parent=0);

	/*!
	  Sets the values in the dialog.
	  \param o number of samples.
	  \param size size of the tile.
	  \param level initial tiles number.
	  \param m sharpness operator.
	  \param ss type of light sampling.
	  \param v1, v2 weight for lightness and sharpness.
	  \param t threshold.
	  \param f smooting filter size.
	  \param nIter number of iterations for the smoothing  filter.
	*/
	void setCurrentValue(OffsetNum o, TileSize size, int level, SharpnessMeasures m, SphereSampling ss, float v1, float v2, float t, SmoothingFilter f, int nIter);

// public Qt slots
public slots:
	
	/*!
	  Invoked when the user press the Ok button.
	*/
	void okPressed();

private slots:

	/*!
	  Invoked when the user changes the weight for the lightness.
	*/
	void k1Changed(double k);
	
	/*!
	  Invoked when the user changes the weight for the sharpness.
	*/
	void k2Changed(double k);

signals:

	/*!
	  Emitted to update the output of the rendering.
	*/
	void configUpdated(OffsetNum o, TileSize size, int level, SharpnessMeasures m, SphereSampling ss, float v1, float v2, float t, SmoothingFilter f, int nIter);

};


//! Widget for Detail Enhancement (or Static Multi-Light Detail Enhancement) settings.
/*!
  The class defines the widget that is showed in the Rendering Dialog to set the parameters of rendering mode Detail Enhancement (or Static Multi-Light Detail Enhancement). 
*/
class DetailEnhDlg : public QWidget
{
	Q_OBJECT

private:

	QPushButton* advancedBtn; /*!< Button to show the dialog for advanced settings. */

	DetailConfDlg* config; /*!< Advanced setting dialog. */

public:
	
	//! Constructor.
	DetailEnhDlg(QWidget *parent = 0);
	
	//! Deconstructor.
	~DetailEnhDlg();
	
	/*!
	  Sets the advanced settings dialog. 
	*/
	void setAdvacedDlg(QDialog* dlg);

private slots:

	/*!
	  Shows the advanced settings dialog.
	*/
	void showAdvancedSettings();
};


//! Detail Enhancement (or Static Multi-Light Detail Enhancement) class
/*!
  The class defines the renndering mode Detail Enhancement (or Static Multi-Light Detail Enhancement).
*/
class DetailEnhancement : public QObject, public RenderingMode
{

	Q_OBJECT

private:

	unsigned char* detailsBuffer; /*!< Pointer to output texture. */
	
	std::vector<vcg::Point3f> tilesLight; /*!< Vector with the light vectors selected for each tile.  */
	std::vector<vcg::Point2f> tilesCenter; /*!< Vector with info about the center of the tiles. */

	std::vector<vcg::Point3f> defaultSamples; /*!< Initial light samples. */
	
	bool bufferReady; /*!< Holds whether the ouput texture is already computed. */

	int* zMatrix; /*!< Z-matrix for relationship among tiles of different level. */

	int maxLevel; /*!< Maximum level of subdivision in tiles. */
	static int minLevel; /*!< Minimum level of subdivision in tiles. */ // YY

	QWidget* loadParent; /*!< Parent for loading window. */

	const PyramidCoeff* coefficient; /*!< Pointer to coefficients for LRGB-PTM. */
	const PyramidRGB* color; /*!< Pointer to rgb components for LRGB-PTM. */
	const PyramidCoeff* coefficientR; /*!< Pointer to red component for RGB-PTM. */
	const PyramidCoeff* coefficientG; /*!< Pointer to green component for RGB-PTM. */
	const PyramidCoeff* coefficientB; /*!< Pointer to blue component for RGB-PTM. */
	bool lrgb; /*!< Holds whether if the image is a LRGB-PTM or RGB-PTM. */

	//YY
	static OffsetNum nOffset; /*!< Current number of light samples. */
	static TileSize minTileSize; /*!< Current size of the tile. */
	static SharpnessMeasures sharpnessOperator; /*!< Current sharpness operator. */
	static SphereSampling sphereSampl; /*!< Current type of light sampling. */
	static float k1; /*!< Current weight for lightness. */
	static float k2; /*!< Current weight for sharpness. */
	static float threshold; /*!< Current threshold for enhancement measure. */
	static SmoothingFilter filter; /*!< Current smoothing filter size. */
	static int nIterSmoothing; /*!< Current number of iterations for the smoothing filter. */
	
	QImage* vectImage; /*!< Image with the drawing of light vector selected for each tile. */

public:

	//! Constructor.
	DetailEnhancement();
	
	//! Deconstructor.
	~DetailEnhancement();
	
	virtual QString getTitle();
	virtual QWidget* getControl(QWidget* parent = 0);
	virtual bool isLightInteractive();
	virtual bool supportRemoteView();
	virtual bool enabledLighting();

	virtual void applyPtmLRGB(const PyramidCoeff& coeff, const PyramidRGB& rgb, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer);

	virtual void applyPtmRGB(const PyramidCoeff& redCoeff, const PyramidCoeff& greenCoeff, const PyramidCoeff& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer);

	void applyHSH(const PyramidCoeffF& redCoeff, const PyramidCoeffF& greenCoeff, const PyramidCoeffF& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer){}

    OffsetNum getNOffset();
    TileSize getMinTileSize();
    int getMinLevel();
    SharpnessMeasures getSharpnessOperator();
    SphereSampling getSphereSampling();
    float getK1();
    float getK2();
    float getThreshold();
    SmoothingFilter getFilter();
    int getNIterSmoothing();

	/*!
	  Returns the light vector used for a specific pixel.
	  \param x, y  coordinates of the pixel.
	  \return a light vector.
	*/
	const vcg::Point3f& getPixelLight(int x, int y);
	
private:

	/*!
	  Returns the color value of pixel (x,y) in the image with the drawing of the light vectors.
	  \param offset offset in the detail buffer.
	  \param x, y coordinates of the pixel.
	  \return the pixel color.
	*/
	int getLightVectImagePixel(int offset, int x, int y);


	/*!
	  Computes the detail enhancement.
	  \param mipMapSize size of mip-mapping levels.
	  \param cb callback to update a progress bar.
	*/
	void calcDetails(const QSize* mipMapSize, CallBackPos * cb = 0);


	/*!
	  Return the array of better light vectors for a specific tile.
	  \param level mip-mapping level.
	  \param x, y coordinates of the left-top corner of the tile.
	  \param tileW width of the tile.
	  \param tileH height of the tile.
	  \param width width of the image.
	  \param lightSample vector of light samples to try.
	  \return the array of the better light vectors. 
	*/
	std::vector<vcg::Point3f>* getBestLight(int level, int x, int y, int tileW, int tileH, int width, const std::vector<vcg::Point3f>& lightSample);


	/*!
	  Computes the value of the current sharpness operator on the input image.
	  \param image pointer to input image.
	  \param width width of the image.
	  \param height height of the image.
	  \return value of the sharpness operator.
	*/
        float computeSharpOperator(int* image, int width, int height);

	
	/*!
	  Computes the light vector to apply at the pixel (x,y) by linear interpolation.
	  \param x, y coordinates of the pixel.
	  \param width width of the image.
	  \param height height of the image.
	  \return a light vector.
	*/
	vcg::Point3f getLight(int x, int y, int width, int height);


	/*!
	  Computes the smoothing of the light vectors on a square grid of tiles.
	  \param source vector of original light vectors.
	  \param dest destination vector for the smoothed light vectors.
	  \param size side size of the square grid of tiles.
	*/
	void calcSmooting(std::vector<vcg::Point3f>& source, std::vector<vcg::Point3f>& dest, int size);


	/*!
	  Computes the samples from a basis light vector.
	  \param base basis light vector.
	  \return a vector of samples.
	*/
	std::vector<vcg::Point3f>* getLightSamples(const vcg::Point3f& base);
	

	/*!
	  Creates the image with the drawing of light vector selected for each tile. 
	*/
	void generateVectImage();


	/*!
	  Computes the global smoothing of the light vectors on a square grid of tiles.
	  For each tile we select the vector nearer to the avarage of light vectors of the neighbouring tiles.
	  \param source list of best light vectors for each tile.
	  \param dest destination vector for the output light vectors.
	  \param size side size of the square grid of tiles.
	*/
	void calcLocalLight(std::vector<std::vector<vcg::Point3f>*>& source, std::vector<vcg::Point3f>& dest, int size);

public slots:


    /*!
      Updates the advanced settings.
    */
    void updateConfig(OffsetNum o, TileSize size, int level, SharpnessMeasures m, SphereSampling ss, float v1, float v2, float t, SmoothingFilter f, int nIter);

signals:

	/*!
	  Emitted to refresh the image in the browser.
	*/
	void refreshImage();
};

#endif /* DETAILSENHANCEMENT_H */
