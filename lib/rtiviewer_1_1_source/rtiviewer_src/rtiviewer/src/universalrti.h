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


#ifndef URTI_H
#define URTI_H

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#include <cmath>
#endif

// Local headers
#include "rti.h"


//! URTI class
class UniversalRti : public Rti
{
// private data member
protected:
	
	Rti* image;

public:

	//! Constructor.
	UniversalRti();

	//! Deconstructor.
	virtual ~UniversalRti();


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

public:

	virtual void setRenderingMode(int a) {image->setRenderingMode(a);}

	virtual int getCurrentRendering() {return image->getCurrentRendering();}

	virtual QMap<int, RenderingMode*>* getSupportedRendering() {return image->getSupportedRendering();}
};

#endif //URTI_H
