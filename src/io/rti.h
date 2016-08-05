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


#ifndef RTI_H
#define RTI_H

#include "util.h"
#include "../function/renderingmode.h"

#include <vcg/space/point3.h>

#include <QTextStream>
#include <QRect>
#include <QBuffer>
#include <QMap>
#include <QDomElement>
#include "../vtkEnums.h"

//! RTI image abstract class.
/*!
  Abstract class to manage the RTI image.
*/
class Rti
{

// private data member 
protected:

	int h; /*!< Height. */
	int w; /*!< Width. */
	QString filename; /*!< Filename. */
	QString type; /*!< Type of image. */

	int currentRendering; /*!< Current rendering mode applied to the image. */
	QMap<int, RenderingMode*>* list; /*!< List of rendering modes to apply to the image. */

	bool remote; /*!< Holds whether the image is loaded by remote disk. */
	int maxRemoteResolution; /*!< Maximun level of resolution if the image is remote. */
	int minRemoteResolution; /*!< Minimum level of resolution if the image is remote. */

	unsigned int* tiles; /*!< Info about the tiles loaded from the remote server. */


//public method
public:

	//! Constructor.
	Rti():
		maxRemoteResolution(0),
		minRemoteResolution(0),
		tiles(NULL),
		list(NULL)
	{ };


	//! Deconstructor.
	virtual ~Rti()
	{
		if (list)
		{
			for (int i = 0; i < list->size(); i++)
				delete (*list)[i];
			delete list;
		}
		if (tiles)
			delete tiles;
	};


	/*!
	  Loads the image from the disk.
	  \param cb callback to update the progress bar.
	  \return returns 0 if the image was successfully loaded, returns -1 otherwise.
	*/
	virtual int load(CallBackPos * cb = 0) = 0;
	
	/*!
	  Loads the image from the disk.
	  \param str path to file.
	  \param cb callback to update the progress bar.
	  \return returns 0 if the image was successfully loaded, returns -1 otherwise.
	*/
	virtual int load(QString str, CallBackPos * cb = 0) = 0;

	
	/*!
	  Saves the image.
	  \param str path to file.
	  \return returns 0 if the image was successfully loaded, returns -1 otherwise.
	*/
	virtual int save(QString str) = 0;


	/*!
	  Loads the compressed image.
	*/
	virtual int loadCompressed() = 0;

	/*!
	  Loads the specific compressed image.
	  \param name file name.
    */
	virtual int loadCompressed(QString name) = 0;

	/*!
	  Loads the specific compressed tile.
	  \param xinf, yinf, xsup, ysup coordinates of the tile.
	  \param name file name.
	*/
	virtual int loadCompressed(int xinf, int yinf, int xsup, int ysup, QString name) = 0;
	
	/*!
	  Saves the compressed image.
	  \param name file name.
	*/
	virtual int saveCompressed(QString name) = 0;

	/*!
	  Saves a compressed tiles.
	  \param xinf, yinf, xsup, ysup coordinates of the tile.
	  \param reslevel mip-mapping level.
	  \param name file name.
	*/
	virtual int saveCompressed(int xinf, int yinf, int xsup, int ysup, int reslevel, QString name) = 0;

	/*!
	  Creates the texture to display in the browser.
	  \param buffer pointer to the buffer of the texture.
	  \param width reference for the width of the output texture.
	  \param height reference for the height of the output texture.
	  \param light light vector.
	  \param sub sub-image displayed in the browser.
	  \param level mip-mapping level to use.
	  \param mode special rendering mode to apply.
	  \return returns 0.
	*/
	virtual int createImage(unsigned char** buffer, int& width, int& height, const vcg::Point3f& light, const QRectF& sub, int level = 0, int mode = 0) = 0;

	

	/*!
	  Creates a preview of the image.
	  \param width waiting width of the image.
	  \param height waiting height of the image.
	  \return pointer to the preview.
	*/
	virtual QImage* createPreview(int width, int height) = 0;


	/*!
	  Allocates the data for a remote RTI image.
	  \param b pointer to the binary data of the XML descriptor of remote RTI 
	  \return returns 0 if the binary data is correct, -1 otherwise.
	*/
	virtual int allocateRemoteImage(QBuffer* b) = 0;
	
	
	/*!
	  Loads the data of a tile.
	  \param b pointer to the buffer with the tile data.
	  \param xinf, yinf, xsup, ysup sub-image coordinates of the tile.
	  \param level mip-mapping level.
	  \return returns 0.
	*/
	virtual int loadCompressedHttp(QBuffer* b, int xinf, int yinf, int xsup, int ysup, int level) = 0; 


	/*!
	  Loads the coefficients block of the image
	  \param file pointer to the image file.
	  \param width width of the image.
	  \param height height of the image.
	  \param basisTerm number of coefficients per component.
	  \param urti holds whether the block is loaded from an URTI image.
	  \param cb callback to update the progress bar.
	  \param text string to display in the progress bar.
	  \return returns 0 if the coefficients were successfully loaded, returns -1 otherwise.
	*/
	virtual int loadData(FILE* file, int width, int height, int basisTerm, bool urti, CallBackPos * cb = 0, const QString& text = QString()) = 0;

	
	/*!
	  Saves the xml descriptor of the remote RTI.
	  \param filename path of the file.
	  \param level number of resolution levels.
	*/
	virtual void saveRemoteDescr(QString& filename, int level) = 0;

public:

	/*!
		Returns the height of the image.
	*/
	int height(){return h;}

	/*!
	  Sets the height of the image.
	*/
	void setHeight(int height){h = height;}

	/*!
	  Returns the width of the image.
	*/
	int width(){return w;}

	/*!
	  Sets the width of the image.
	*/
	void setWidth(int width){w = width;}

	/*!
	  Sets the file path of the image.
	*/
	void setFileName(QString s) {filename = s;}

	/*!
	  Sets the rendering mode to apply to the image.
	*/
	virtual void setRenderingMode(int a) {currentRendering = a;}

	/*!
	  Returns the index of the current rendering mode applied to the image.
	*/
	virtual int getCurrentRendering() {return currentRendering;}

	/*!
	  Retruns the list of rendering modes supported by the image.
	*/
	virtual QMap<int, RenderingMode*>* getSupportedRendering() {return list;}

	/*!
	  Returns the maximum level of resolution for a remote RTI image.
	*/
	int maxResolution(){return maxRemoteResolution;}

	/*!
	  Returns the minimum level of resolution for a remote RTI image.
	*/
	int minResolution(){return minRemoteResolution;}


	/*!
	  Checks if the tile is already received.
	  \param zindex tile index.
	  \param level mip-mapping level.
	*/
	bool isTilePresent(int zindex, int level)
	{
		if (tiles)
			return tiles[zindex] & (1 << level);
		else
			return false;
	}


	/*!
	  Sets the tile as received.
	  \param zindex tile index.
	  \param level mip-mapping level.
	*/
	void setTileLevel(int zindex, int level)
	{
		if (tiles)
			tiles[zindex] |= (1 << level);
	}

	/*!
	  Return the type of the image.
	*/
	QString typeFormat(){return type;}

	/*!
	  Reset the flag for the remote RTI image.
	  The method must be invoked when all tiles are received. 
	*/
	void resetRemote(){remote = false;}
};


/*!
  Rendering mode for RTI image.
*/
/*
enum RenderingRTI
{
	DEFAULT,
    DIFFUSE_GAIN,
	SPECULAR_ENHANCEMENT,
	NORMAL_ENHANCEMENT,
	UNSHARP_MASKING_IMG,
	UNSHARP_MASKING_LUM,
	COEFF_ENHANCEMENT,
	DETAIL_ENHANCEMENT,
	DYN_DETAIL_ENHANCEMENT,
    NORMALS,
};
*/

#endif  /* RTI_H */
