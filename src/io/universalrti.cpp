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


#include "universalrti.h"
//#include "ptm.h"
#include "hsh.h"

#include <QTime>

UniversalRti::UniversalRti() :
	Rti()
{
	image = NULL;
}


UniversalRti::~UniversalRti()
{
	if (image)
		delete image;
}


int UniversalRti::load(CallBackPos *cb)
{
	if (filename.isEmpty())
		return -1;
	else
		return load(filename, cb);
}


int UniversalRti::load(QString name, CallBackPos *cb)
{
#ifdef PRINT_DEBUG
	QTime first = QTime::currentTime();
#endif

	remote = false;
    if (cb != NULL)	(*cb)(0, "Loading RTI...");
	filename = name;

#ifdef WIN32
  #ifndef __MINGW32__
	FILE* file;
	if (fopen_s(&file, filename.toStdString().c_str(), "rb") != 0)
		return -1;
  #else
	FILE* file = fopen(filename.toStdString().c_str(), "rb");
	if (file == NULL)
		return -1;
  #endif
#else
	FILE* file = fopen(filename.toStdString().c_str(), "rb");
	if (file == NULL)
		return -1;
#endif

	unsigned char c;

	bool eof, error;
	QString str;
	do
	{
		str = getLine(file, &eof);
		if (eof) return -1;
	} while (str.startsWith("#"));

	int rtiType = str.toInt(&error);
	if (!error) return -1;
	

	//Gets width, height, number of color
	str = getLine(file, &eof);
	if (eof) return -1;
	QStringList list = str.split(' ',  QString::SkipEmptyParts);
	if (list.size() != 3)
		return -1;
	w = list[0].toInt(&error);
	if (!error) return -1;
	h = list[1].toInt(&error);
	if (!error) return -1;
	int ncolor = list[2].toInt(&error);
	if (!error) return -1;

	//Gets number of basis term, basis type, element size.
	str = getLine(file, &eof);
	if (eof) return -1;
	list = str.split(' ',  QString::SkipEmptyParts);
	if (list.size() != 3)
		return -1;
	int basisTerm = list[0].toInt(&error);
	if (!error) return -1;
	int basisType = list[1].toInt(&error);
	if (!error) return -1;
	int elemSize = list[2].toInt(&error);
	if (!error) return -1;
	
	if (basisTerm == 1)
		basisTerm = 4;
	else if (basisTerm == 2)
		basisTerm = 9;

	switch(rtiType)
	{
        case 0: type = "RTI"; return -1; break;
		case 1: 
            type = "RTI PTM";
			//if (basisType == 1)
			//	image = new LRGBPtm();
			//else
			//	image = new RGBPtm();
			//((Ptm*)image)->setVersion("PTM_1.2");
			return -1;
			break;
        case 2: type = "RTI SH"; return -1;break;
		case 3: 
            type = "RTI HSH";
			image = new Hsh();
			break;
        case 4: type = "RTI ADAPTIVE PTM"; return -1; break;
        default: type = "RTI"; return -1;
	}
    QString text = "Loading RTI...";
	int ret = image->loadData(file, w, h, basisTerm, true, cb, text);
	if (ret != 0)
		return -1;

	if (cb != NULL)	(*cb)(99, "Done");

#ifdef PRINT_DEBUG
	QTime second = QTime::currentTime();
        float diff = first.msecsTo(second) / 1000.0;
    printf("RTI Loading: %.5f s\n", diff);
#endif

	return 0;
}


int UniversalRti::loadData(FILE* file, int width, int height, int basisTerm, bool urti, CallBackPos * cb,const QString& text)
{
	return 0;
}

int UniversalRti::save(QString name)
{
	// Not implemented for now...
	return 0;
}


int UniversalRti::loadCompressed()
{
	if (filename.isEmpty())
		return -1;
	else
		return loadCompressed(filename);
}


int UniversalRti::loadCompressed(QString name)
{
	return loadCompressed(0,0,w,h,name);
}


int UniversalRti::loadCompressed(int xinf, int yinf, int xsup, int ysup, QString name)
{
	return image->loadCompressed(xinf, yinf, xsup, ysup, name);
}


int UniversalRti::saveCompressed(QString name)
{
	return saveCompressed(0,0,w,h,0,name);
}


int UniversalRti::saveCompressed(int xinf, int yinf, int xsup, int ysup, int reslevel, QString name)
{
	return image->saveCompressed(xinf, yinf, xsup, ysup, reslevel, name);
}


int UniversalRti::createImage(unsigned char** buffer, int& width, int& height, const vcg::Point3f& light, const QRectF& rect, int level, int mode)
{
	image->createImage(buffer, width, height, light, rect, level, mode);
	return 0;
}


QImage* UniversalRti::createPreview(int width, int height)
{
	return image->createPreview(width, height);
}


int UniversalRti::allocateRemoteImage(QBuffer* b)
{
	return image->allocateRemoteImage(b);
}


int UniversalRti::loadCompressedHttp(QBuffer* b, int xinf, int yinf, int xsup, int ysup, int level)
{
	return image->loadCompressedHttp(b, xinf, yinf, xsup, ysup, level);
}


void UniversalRti::saveRemoteDescr(QString& filename, int level)
{
	image->saveRemoteDescr(filename, level);
}
