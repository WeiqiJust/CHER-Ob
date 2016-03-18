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


#ifndef HSH_H
#define HSH_H

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#include <cmath>
#endif

// Local headers
#include "rti.h"
#include "pyramid.h"
#include "../function/renderingmode.h"
#include "../function/defaultrendering.h"
#include "../function/specularenhanc.h"
#include "../function/normalsrendering.h"

//#include <jpeg2000.h>

#include "../visualization/vtkWidget.h" // YY

// Qt headers
#include <QFile>
#include <QImage>
#include <QVector>


//! HSH class
class Hsh : public Rti
{
// private data member
protected:
	
	QString version; /*!< Version. */
 	QSize mipMapSize[MIP_MAPPING_LEVELS]; /*!< Size of mip-mapping levels. */

	PyramidCoeffF redCoefficients; /*!< Coefficients for red component. */
	PyramidCoeffF greenCoefficients; /*!< Coefficients for green component. */
	PyramidCoeffF blueCoefficients; /*!< Coefficients for blue component. */

	float gmin[9]; /*!< Min coefficient value. */
	float gmax[9]; /*!< Max coefficient value. */

	int bands; /*!< Number of colors. */
	int ordlen; /*!< Number of cofficients per pixel. */

	PyramidNormals normals; /*!< Normals. */

public:

	//! Constructor.
	Hsh();

	//! Deconstructor.
	virtual ~Hsh();
	//~Hsh();

	// protected methods
protected:

	
	

private:// YY
	MainWindow * mw(); 
	void updateCoeffandRender(float* redPtr, float* greenPtr, float* bluePtr, int size, bool FIRST_RTI_RENDERING, bool LOADING_DONE);

public:
	virtual int load(CallBackPos * cb = 0);
	virtual int load(QString name, CallBackPos * cb = 0);
	virtual int save(QString name);
	virtual int loadCompressed();
	virtual int loadCompressed(QString name);
	virtual int loadCompressed(int xinf, int yinf, int xsup, int ysup, QString name);
	virtual int saveCompressed(QString name);
	virtual int saveCompressed(int xinf, int yinf, int xsup, int ysup, int reslevel, QString name);
	virtual int createImage(unsigned char** buffer, int& width, int& height, const vcg::Point3f& light, const QRectF& rect, int level = 0, int mode = 0);
    virtual QImage* createPreview(int width, int height);
	virtual int allocateRemoteImage(QBuffer* b);  
	virtual int loadCompressedHttp(QBuffer* b, int xinf, int yinf, int xsup, int ysup, int level); 
	virtual int loadData(FILE* file, int width, int height, int basisTerm, bool urti, CallBackPos * cb = 0,const QString& text = QString());
	virtual void saveRemoteDescr(QString& filename, int level);

};

#endif //HSH_H
