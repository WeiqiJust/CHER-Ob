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


#include "hsh.h"
#include "../../rtiwebmaker/src/zorder.h"
#include "../mainWindow.h" //YY

//#include <vcg/math/lin_algebra.h>
//#include <vcg/math/matrix33.h>

#include <eigenlib/Eigen/Eigen>

#include <QTime>
#include <QDebug>

#if _MSC_VER
#include <windows.h>
#elif __MINGW32__
#define WIN32_WINNT 0x0500
#define WINVER 0x0500
#include <windows.h>
#endif

Hsh::Hsh() :
	Rti()
{
	currentRendering = DEFAULT;
	// Create list of supported rendering mode.
	list = new QMap<int, RenderingMode*>();
	list->insert(DEFAULT, new DefaultRendering());
    list->insert(NORMALS, new NormalsRendering());
    list->insert(SPECULAR_ENHANCEMENT ,new SpecularEnhancement());
}


Hsh::~Hsh()
{

}


int Hsh::load(CallBackPos *cb)
{
	if (filename.isEmpty())
		return -1;
	else
		return load(filename, cb);
}


int Hsh::load(QString name, CallBackPos *cb)
{
#ifdef PRINT_DEBUG
	QTime first = QTime::currentTime();
#endif

	remote = false;
	if (cb != NULL)	(*cb)(0, "Loading HSH...");
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

	type = "HSH";

	//parse comments		
	c = fgetc(file);
	if (feof(file))
		return -1;
	while(c=='#')		
	{
		while (c != '\n')
		{
			c = fgetc(file);
			if (feof(file))
				return -1;
		}
		c = fgetc(file);
	}
	if (feof(file))
		return -1;
	//rewind one character
	fseek(file, -1, SEEK_CUR);

	//read width
	fread(&w, sizeof(int), 1, file);
	//read height
	fread(&h, sizeof(int), 1, file);	
	//read number of colors per pixel
	fread(&bands, sizeof(int), 1, file);
	//read number of coefficients per pixel
	fread(&ordlen, sizeof(int), 1, file);

#if _MSC_VER || __MINGW32__
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	if (w*h*(ordlen*ordlen + 1)*16 > statex.ullAvailVirtual*0.95)
		return -2;
#endif

	if (feof(file))
		return -1;
	
	ordlen *= ordlen;
	
	QString text = "Loading HSH...";
	if (loadData(file, w, h, ordlen, false, cb, text) != 0)
		return -1;

	if (cb != NULL)	(*cb)(99, "Done");

#ifdef PRINT_DEBUG
	QTime second = QTime::currentTime();
        float diff = first.msecsTo(second) / 1000.0;
        printf("HSH Loading: %.5f s\n", diff);
#endif

	return 0;
}

// YY
MainWindow * Hsh::mw() // current MainWindow
{
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
    if (mainwindow)
    {
      return mainwindow;
    }
  }
  return NULL;
}

//YY
void Hsh::updateCoeffandRender(float* redPtr, float* greenPtr, float* bluePtr, int size, bool FIRST_RTI_RENDERING, bool LOADING_DONE)
{
	// progressively update these three coefficient arrays
	redCoefficients.setLevel(redPtr, size, 0);
	greenCoefficients.setLevel(greenPtr, size, 0);
	blueCoefficients.setLevel(bluePtr, size, 0);

	mw()->VTKA()->mRTIbrowser->setImage(this, FIRST_RTI_RENDERING, LOADING_DONE);
}

// YY: data reading thread. see blow for the original version: loadData()
int Hsh::loadData(FILE* file, int width, int height, int basisTerm, bool urti, CallBackPos * cb,const QString& text)
{
	bool FIRST_RTI_RENDERING = true;
	bool LOADING_DONE = false;

	QTime myTimer;
	myTimer.start();

	type = "HSH";
	w = width;
	h = height;

	ordlen = basisTerm;
	bands = 3;
	fread(gmin, sizeof(float), basisTerm, file);
	fread(gmax, sizeof(float), basisTerm, file);

	if (feof(file))
		return -1;

	int offset = 0;

	int size = w * h * basisTerm;
	float* redPtr = new float[size];
	float* greenPtr = new float[size];
	float* bluePtr = new float[size];

	for (int i = 0; i < size; i++) {
		redPtr[i] = 0.0;
		greenPtr[i] = 0.0;
		bluePtr[i] = 0.0;
	}
	
	unsigned char c;
	mipMapSize[0] = QSize(w, h);

	if (!urti)
	{
		for(int j = 0; j < h; j++)
		{
			if (cb != NULL && j % 50 == 0)(*cb)(j * 50.0 / h, text);
			for(int i = 0; i < w; i++)
			{				
				offset = j * w + i;
				for (int k = 0; k < basisTerm; k++)
				{
					if (feof(file))
						return -1;
					fread(&c, sizeof(unsigned char), 1, file);
					redPtr[offset*basisTerm + k] = (((float)c) / 255.0) * (gmax[k] - gmin[k]) + gmin[k];
				}
				for (int k = 0; k < basisTerm; k++)
				{
					if (feof(file))
						return -1;
					fread(&c, sizeof(unsigned char), 1, file);
					greenPtr[offset*basisTerm + k] = (((float)c) / 255.0) * (gmax[k] - gmin[k]) + gmin[k];
				}
				for (int k = 0; k < basisTerm; k++)
				{
					if (feof(file))
						return -1;
					fread(&c, sizeof(unsigned char), 1, file);
					bluePtr[offset*basisTerm + k] = (((float)c) / 255.0) * (gmax[k] - gmin[k]) + gmin[k];
				}
			}

			// YY
			if (FIRST_RTI_RENDERING) // render first image 
			{
				updateCoeffandRender(redPtr, greenPtr, bluePtr, size, FIRST_RTI_RENDERING, LOADING_DONE);
				FIRST_RTI_RENDERING = false;	
				myTimer.restart();
			} else if (myTimer.elapsed() >= mw()->VTKA()->getRerenderingTimeInterval()) {
				updateCoeffandRender(redPtr, greenPtr, bluePtr, size, FIRST_RTI_RENDERING, LOADING_DONE);
				myTimer.restart();
			}
		}
		LOADING_DONE = true;
		updateCoeffandRender(redPtr, greenPtr, bluePtr, size, FIRST_RTI_RENDERING, LOADING_DONE);
	}
	else
	{	
		//int segNum = 10;
		//int i, j;
		for(int j = 0; j < h; j++)
		{   
			//// set a locker
			//boost::unique_lock<boost::mutex> locker(myMutex); 

			// prepare data
			if (cb != NULL && j % 50 == 0)(*cb)(j * 50 / h, text);
			for(int i = 0; i < w; i++)
			{				
				offset = j * w + i;
				for (int k = 0; k < basisTerm; k++)
				{
					if (feof(file))
						return -1;
					fread(&c, sizeof(unsigned char), 1, file);
					redPtr[offset*basisTerm + k] = (((float)c) / 255.0) * gmin[k] + gmax[k];
				}
				for (int k = 0; k < basisTerm; k++)
				{
					if (feof(file))
						return -1;
					fread(&c, sizeof(unsigned char), 1, file);
					greenPtr[offset*basisTerm + k] = (((float)c) / 255.0) * gmin[k] + gmax[k];
				}
				for (int k = 0; k < basisTerm; k++)
				{
					if (feof(file))
						return -1;
					fread(&c, sizeof(unsigned char), 1, file);
					bluePtr[offset*basisTerm + k] = (((float)c) / 255.0) * gmin[k] + gmax[k];
				}
			}

			/*if (j == h - 1) {
				DATA_FINISHED_READING = true;
			}*/

			/******************Method 1*****************************/
			//// unlock
			//if ((j+1)%int(floor(double(h/segNum))) == 0 || j == h -1) {
			//	// progressively update these three coefficient arrays
			//	redCoefficients.setLevel(redPtr, size, 0);
			//	greenCoefficients.setLevel(greenPtr, size, 0);
			//	blueCoefficients.setLevel(bluePtr, size, 0);

			//	/*DATA_PROCESSED = true;
			//	locker.unlock();
			//	cond.notify_one();
			//	boost::this_thread::sleep_for(boost::chrono::milliseconds(100));*/

			//	mw()->VTKA()->progressiveDisplay(FIRST_RTI_RENDERING);
			//	FIRST_RTI_RENDERING = false;

			//	//boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
			//}

			if (FIRST_RTI_RENDERING) // render first image 
			{
				//// progressively update these three coefficient arrays
				//redCoefficients.setLevel(redPtr, size, 0);
				//greenCoefficients.setLevel(greenPtr, size, 0);
				//blueCoefficients.setLevel(bluePtr, size, 0);

				//mw()->VTKA()->progressiveDisplay(FIRST_RTI_RENDERING);
				/*FIRST_RTI_RENDERING = false;
				myTimer.restart();*/

				updateCoeffandRender(redPtr, greenPtr, bluePtr, size, FIRST_RTI_RENDERING, LOADING_DONE);
				FIRST_RTI_RENDERING = false;
				myTimer.restart();
				
			} else if (myTimer.elapsed() >= mw()->VTKA()->getRerenderingTimeInterval()) {
				//// progressively update these three coefficient arrays
				//redCoefficients.setLevel(redPtr, size, 0);
				//greenCoefficients.setLevel(greenPtr, size, 0);
				//blueCoefficients.setLevel(bluePtr, size, 0);
				/*FIRST_RTI_RENDERING = false;
				myTimer.restart();*/

				//mw()->VTKA()->progressiveDisplay(FIRST_RTI_RENDERING);

				updateCoeffandRender(redPtr, greenPtr, bluePtr, size, FIRST_RTI_RENDERING, LOADING_DONE);
				myTimer.restart();
			}
		}
		//qDebug() << "Reading RTI Time Elapsed: " << myTimer.elapsed();
		/*redCoefficients.setLevel(redPtr, size, 0);
		greenCoefficients.setLevel(greenPtr, size, 0);
		blueCoefficients.setLevel(bluePtr, size, 0);
		mw()->VTKA()->progressiveDisplay(FIRST_RTI_RENDERING);
		FIRST_RTI_RENDERING = false;*/

		LOADING_DONE = true;
		updateCoeffandRender(redPtr, greenPtr, bluePtr, size, FIRST_RTI_RENDERING, LOADING_DONE);
	}
	
	fclose(file);

	/*mipMapSize[0] = QSize(w, h);

	redCoefficients.setLevel(redPtr, size, 0);
	greenCoefficients.setLevel(greenPtr, size, 0);
	blueCoefficients.setLevel(bluePtr, size, 0);*/
	
	// Computes mip-mapping.
	if (cb != NULL)	(*cb)(50, "Mip mapping generation...");
	
	for (int level = 1; level < MIP_MAPPING_LEVELS; level++)
	{
		int width = mipMapSize[level - 1].width();
		int height = mipMapSize[level - 1].height();
		int width2 = ceil(width / 2.0);
		int height2 = ceil(height / 2.0);
		size = width2*height2*basisTerm;
		redCoefficients.setLevel(new float[size], size, level);
		greenCoefficients.setLevel(new float[size], size, level);
		blueCoefficients.setLevel(new float[size], size, level);
		int th_id;
		#pragma omp parallel for
		for (int i = 0; i < height - 1; i+=2)
		{
			th_id = omp_get_thread_num();
			if (th_id == 0)
			{
				if (cb != NULL && i % 50 == 0)	(*cb)(50 + (level-1)*8 + i*8.0/height, "Mip mapping generation...");
			}
			for (int j = 0; j < width - 1; j+=2)
			{
				int index1 = (i * width + j)*ordlen;
				int index2 = (i * width + j + 1)*ordlen;
				int index3 = ((i + 1) * width + j)*ordlen;
				int index4 = ((i + 1) * width + j + 1)*ordlen;
				int offset = (i/2 * width2 + j/2)*ordlen;
				for (int k = 0; k < basisTerm; k++)
				{
					redCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k, index3 + k , index4 + k);
					greenCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k, index3 + k , index4 + k);
					blueCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k, index3 + k , index4 + k);
				}
			}
		}
		if (width2 % 2 != 0)
		{
			for (int i = 0; i < height - 1; i+=2)
			{
				int index1 = ((i + 1) * width - 1)*ordlen;
				int index2 = ((i + 2) * width - 1)*ordlen;
				int offset = ((i/2 + 1) * width2 - 1)*ordlen;
				for (int k = 0; k < basisTerm; k++)
				{
					redCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k);
					greenCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k);
					blueCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k);
				}
			}
		}
		if (height % 2 != 0)
		{
			for (int i = 0; i < width - 1; i+=2)
			{
				int index1 = ((height - 1) * width + i)*ordlen;
				int index2 = ((height - 1) * width + i + 1)*ordlen;
				int offset = ((height2 - 1) * width2 + i/2)*ordlen;
				for (int k = 0; k < basisTerm; k++)
				{
					redCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k);
					greenCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k);
					blueCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k);
				}
			}
		}
		if (height % 2 != 0 && width % 2 != 0)
		{
			int index1 = (height*width - 1)*ordlen;
			int offset = (height2*width2 - 1)*ordlen;
			for (int k = 0; k < basisTerm; k++)
			{
				redCoefficients.calcMipMapping(level, offset + k, index1 + k);
				greenCoefficients.calcMipMapping(level, offset + k, index1 + k);
				blueCoefficients.calcMipMapping(level, offset + k, index1 + k);
			}
		}
		mipMapSize[level] = QSize(width2, height2);
	}

	//Compute normals.
	if (cb != NULL) (*cb)(75 , "Normals generation...");
	Eigen::Vector3d l0(sin(M_PI/4)*cos(M_PI/6), sin(M_PI/4)*sin(M_PI/6), cos(M_PI/4));
	Eigen::Vector3d l1(sin(M_PI/4)*cos(5*M_PI / 6), sin(M_PI/4)*sin(5*M_PI / 6), cos(M_PI/4));
	Eigen::Vector3d l2(sin(M_PI/4)*cos(3*M_PI / 2), sin(M_PI/4)*sin(3*M_PI / 2), cos(M_PI/4));
    float hweights0[16], hweights1[16], hweights2[16];
	getHSH(M_PI / 4, M_PI / 6, hweights0, ordlen);
	getHSH(M_PI / 4, 5*M_PI / 6, hweights1, ordlen);
	getHSH(M_PI / 4, 3*M_PI / 2, hweights2, ordlen);
	
	
	Eigen::Matrix3d L;
	L.setIdentity();
	L.row(0) = l0;
	L.row(1) = l1;
	L.row(2) = l2;
	Eigen::Matrix3d LInverse = L.inverse();
	
	for (int level = 0; level < MIP_MAPPING_LEVELS; level++)
	{
		const float* rPtr = redCoefficients.getLevel(level);
		const float* gPtr = greenCoefficients.getLevel(level);
		const float* bPtr = blueCoefficients.getLevel(level);
		vcg::Point3f* temp = new vcg::Point3f[mipMapSize[level].width()*mipMapSize[level].height()];
		if (cb != NULL) (*cb)(75 + level*6, "Normal generation...");

		#pragma omp parallel for
		for (int y = 0; y < mipMapSize[level].height(); y++)
		{
			for (int x = 0; x < mipMapSize[level].width(); x++)
			{
				int offset= y * mipMapSize[level].width() + x;
				Eigen::Vector3d f(0, 0, 0);
				for (int k = 0; k < ordlen; k++)
				{
					f(0) += rPtr[offset*ordlen + k] * hweights0[k];
					f(1) += rPtr[offset*ordlen + k] * hweights1[k];
					f(2) += rPtr[offset*ordlen + k] * hweights2[k];
				}
				for (int k = 0; k < ordlen; k++)
				{
					f(0) += gPtr[offset*ordlen + k] * hweights0[k];
					f(1) += gPtr[offset*ordlen + k] * hweights1[k];
					f(2) += gPtr[offset*ordlen + k] * hweights2[k];
				}
				for (int k = 0; k < ordlen; k++)
				{
					f(0) += bPtr[offset*ordlen + k] * hweights0[k];
					f(1) += bPtr[offset*ordlen + k] * hweights1[k];
					f(2) += bPtr[offset*ordlen + k] * hweights2[k];
				}
				f /= 3.0;
				Eigen::Vector3d normal = LInverse * f;
				temp[offset] = vcg::Point3f(normal(0), normal(1), normal(2));
				temp[offset].Normalize();
			}
		}
		normals.setLevel(temp, mipMapSize[level].width()*mipMapSize[level].height(), level);
	}
	
	return 0;
}

//int Hsh::loadData(FILE* file, int width, int height, int basisTerm, bool urti, CallBackPos * cb,const QString& text)
//{
//	// YY: use thread
//	boost::unique_lock<boost::mutex> locker(myMutex); 
//	DATA_PROCESSED = true;
//	locker.unlock();
//	cond.notify_one();
//	boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
//
//	QTime myTimer;
//	myTimer.start();
//
//	type = "HSH";
//	w = width;
//	h = height;
//
//	ordlen = basisTerm;
//	bands = 3;
//	fread(gmin, sizeof(float), basisTerm, file);
//	fread(gmax, sizeof(float), basisTerm, file);
//
//	if (feof(file))
//		return -1;
//
//	int offset = 0;
//
//	int size = w * h * basisTerm;
//	float* redPtr = new float[size];
//	float* greenPtr = new float[size];
//	float* bluePtr = new float[size];
//	
//	unsigned char c;
//
//	if (!urti)
//	{
//		for(int j = 0; j < h; j++)
//		{
//			if (cb != NULL && j % 50 == 0)(*cb)(j * 50.0 / h, text);
//			for(int i = 0; i < w; i++)
//			{				
//				offset = j * w + i;
//				for (int k = 0; k < basisTerm; k++)
//				{
//					if (feof(file))
//						return -1;
//					fread(&c, sizeof(unsigned char), 1, file);
//					redPtr[offset*basisTerm + k] = (((float)c) / 255.0) * (gmax[k] - gmin[k]) + gmin[k];
//				}
//				for (int k = 0; k < basisTerm; k++)
//				{
//					if (feof(file))
//						return -1;
//					fread(&c, sizeof(unsigned char), 1, file);
//					greenPtr[offset*basisTerm + k] = (((float)c) / 255.0) * (gmax[k] - gmin[k]) + gmin[k];
//				}
//				for (int k = 0; k < basisTerm; k++)
//				{
//					if (feof(file))
//						return -1;
//					fread(&c, sizeof(unsigned char), 1, file);
//					bluePtr[offset*basisTerm + k] = (((float)c) / 255.0) * (gmax[k] - gmin[k]) + gmin[k];
//				}
//			}
//		}
//	}
//	else
//	{
//		for(int j = 0; j < h; j++)
//		{
//			if (cb != NULL && j % 50 == 0)(*cb)(j * 50 / h, text);
//			for(int i = 0; i < w; i++)
//			{				
//				offset = j * w + i;
//				for (int k = 0; k < basisTerm; k++)
//				{
//					if (feof(file))
//						return -1;
//					fread(&c, sizeof(unsigned char), 1, file);
//					redPtr[offset*basisTerm + k] = (((float)c) / 255.0) * gmin[k] + gmax[k];
//				}
//				for (int k = 0; k < basisTerm; k++)
//				{
//					if (feof(file))
//						return -1;
//					fread(&c, sizeof(unsigned char), 1, file);
//					greenPtr[offset*basisTerm + k] = (((float)c) / 255.0) * gmin[k] + gmax[k];
//				}
//				for (int k = 0; k < basisTerm; k++)
//				{
//					if (feof(file))
//						return -1;
//					fread(&c, sizeof(unsigned char), 1, file);
//					bluePtr[offset*basisTerm + k] = (((float)c) / 255.0) * gmin[k] + gmax[k];
//				}
//			}
//		}
//		qDebug() << "Reading RTI Time Elapsed: " << myTimer.elapsed();
//	}
//	
//	fclose(file);
//
//	mipMapSize[0] = QSize(w, h);
//
//	redCoefficients.setLevel(redPtr, size, 0);
//	greenCoefficients.setLevel(greenPtr, size, 0);
//	blueCoefficients.setLevel(bluePtr, size, 0);
//	
//	// Computes mip-mapping.
//	if (cb != NULL)	(*cb)(50, "Mip mapping generation...");
//	
//	for (int level = 1; level < MIP_MAPPING_LEVELS; level++)
//	{
//		int width = mipMapSize[level - 1].width();
//		int height = mipMapSize[level - 1].height();
//		int width2 = ceil(width / 2.0);
//		int height2 = ceil(height / 2.0);
//		size = width2*height2*basisTerm;
//		redCoefficients.setLevel(new float[size], size, level);
//		greenCoefficients.setLevel(new float[size], size, level);
//		blueCoefficients.setLevel(new float[size], size, level);
//		int th_id;
//		#pragma omp parallel for
//		for (int i = 0; i < height - 1; i+=2)
//		{
//			th_id = omp_get_thread_num();
//			if (th_id == 0)
//			{
//				if (cb != NULL && i % 50 == 0)	(*cb)(50 + (level-1)*8 + i*8.0/height, "Mip mapping generation...");
//			}
//			for (int j = 0; j < width - 1; j+=2)
//			{
//				int index1 = (i * width + j)*ordlen;
//				int index2 = (i * width + j + 1)*ordlen;
//				int index3 = ((i + 1) * width + j)*ordlen;
//				int index4 = ((i + 1) * width + j + 1)*ordlen;
//				int offset = (i/2 * width2 + j/2)*ordlen;
//				for (int k = 0; k < basisTerm; k++)
//				{
//					redCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k, index3 + k , index4 + k);
//					greenCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k, index3 + k , index4 + k);
//					blueCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k, index3 + k , index4 + k);
//				}
//			}
//		}
//		if (width2 % 2 != 0)
//		{
//			for (int i = 0; i < height - 1; i+=2)
//			{
//				int index1 = ((i + 1) * width - 1)*ordlen;
//				int index2 = ((i + 2) * width - 1)*ordlen;
//				int offset = ((i/2 + 1) * width2 - 1)*ordlen;
//				for (int k = 0; k < basisTerm; k++)
//				{
//					redCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k);
//					greenCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k);
//					blueCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k);
//				}
//			}
//		}
//		if (height % 2 != 0)
//		{
//			for (int i = 0; i < width - 1; i+=2)
//			{
//				int index1 = ((height - 1) * width + i)*ordlen;
//				int index2 = ((height - 1) * width + i + 1)*ordlen;
//				int offset = ((height2 - 1) * width2 + i/2)*ordlen;
//				for (int k = 0; k < basisTerm; k++)
//				{
//					redCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k);
//					greenCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k);
//					blueCoefficients.calcMipMapping(level, offset + k, index1 + k, index2 + k);
//				}
//			}
//		}
//		if (height % 2 != 0 && width % 2 != 0)
//		{
//			int index1 = (height*width - 1)*ordlen;
//			int offset = (height2*width2 - 1)*ordlen;
//			for (int k = 0; k < basisTerm; k++)
//			{
//				redCoefficients.calcMipMapping(level, offset + k, index1 + k);
//				greenCoefficients.calcMipMapping(level, offset + k, index1 + k);
//				blueCoefficients.calcMipMapping(level, offset + k, index1 + k);
//			}
//		}
//		mipMapSize[level] = QSize(width2, height2);
//	}
//
//	//Compute normals.
//	if (cb != NULL) (*cb)(75 , "Normals generation...");
//	Eigen::Vector3d l0(sin(M_PI/4)*cos(M_PI/6), sin(M_PI/4)*sin(M_PI/6), cos(M_PI/4));
//	Eigen::Vector3d l1(sin(M_PI/4)*cos(5*M_PI / 6), sin(M_PI/4)*sin(5*M_PI / 6), cos(M_PI/4));
//	Eigen::Vector3d l2(sin(M_PI/4)*cos(3*M_PI / 2), sin(M_PI/4)*sin(3*M_PI / 2), cos(M_PI/4));
//    float hweights0[16], hweights1[16], hweights2[16];
//	getHSH(M_PI / 4, M_PI / 6, hweights0, ordlen);
//	getHSH(M_PI / 4, 5*M_PI / 6, hweights1, ordlen);
//	getHSH(M_PI / 4, 3*M_PI / 2, hweights2, ordlen);
//	
//	
//	Eigen::Matrix3d L;
//	L.setIdentity();
//	L.row(0) = l0;
//	L.row(1) = l1;
//	L.row(2) = l2;
//	Eigen::Matrix3d LInverse = L.inverse();
//	
//	qDebug() << "Time Elapsed: " << myTimer.elapsed();
//	for (int level = 0; level < MIP_MAPPING_LEVELS; level++)
//	{
//		const float* rPtr = redCoefficients.getLevel(level);
//		const float* gPtr = greenCoefficients.getLevel(level);
//		const float* bPtr = blueCoefficients.getLevel(level);
//		vcg::Point3f* temp = new vcg::Point3f[mipMapSize[level].width()*mipMapSize[level].height()];
//		if (cb != NULL) (*cb)(75 + level*6, "Normal generation...");
//
//		#pragma omp parallel for
//		for (int y = 0; y < mipMapSize[level].height(); y++)
//		{
//			for (int x = 0; x < mipMapSize[level].width(); x++)
//			{
//				int offset= y * mipMapSize[level].width() + x;
//				Eigen::Vector3d f(0, 0, 0);
//				for (int k = 0; k < ordlen; k++)
//				{
//					f(0) += rPtr[offset*ordlen + k] * hweights0[k];
//					f(1) += rPtr[offset*ordlen + k] * hweights1[k];
//					f(2) += rPtr[offset*ordlen + k] * hweights2[k];
//				}
//				for (int k = 0; k < ordlen; k++)
//				{
//					f(0) += gPtr[offset*ordlen + k] * hweights0[k];
//					f(1) += gPtr[offset*ordlen + k] * hweights1[k];
//					f(2) += gPtr[offset*ordlen + k] * hweights2[k];
//				}
//				for (int k = 0; k < ordlen; k++)
//				{
//					f(0) += bPtr[offset*ordlen + k] * hweights0[k];
//					f(1) += bPtr[offset*ordlen + k] * hweights1[k];
//					f(2) += bPtr[offset*ordlen + k] * hweights2[k];
//				}
//				f /= 3.0;
//				Eigen::Vector3d normal = LInverse * f;
//				temp[offset] = vcg::Point3f(normal(0), normal(1), normal(2));
//				temp[offset].Normalize();
//			}
//		}
//		normals.setLevel(temp, mipMapSize[level].width()*mipMapSize[level].height(), level);
//	}
//	
//	qDebug() << "Time Elapsed: " << myTimer.elapsed();
//
//	return 0;
//
//}


int Hsh::save(QString name)
{
	// Not implemented for now...
	return 0;
}


int Hsh::loadCompressed()
{
	//if (filename.isEmpty())
	//	return -1;
	//else
	//	return loadCompressed(filename);

	return 1;
}


int Hsh::loadCompressed(QString name)
{
	//return loadCompressed(0,0,w,h,name);

	return 1;
}


int Hsh::loadCompressed(int xinf, int yinf, int xsup, int ysup, QString name)
{
	/*remote = false;
	Jpeg2000 jpegimage(name.toStdString().c_str());
	int offset,offset2;
	const float** coeffPtr = new const float*[ordlen*bands];
	for (int i = 0; i < ordlen*bands; i++)
		coeffPtr[i] = (const float*) jpegimage.componentData(i);
	
	for (int y = yinf; y < ysup; y++)
		for (int x = xinf; x < xsup; x++)
		{
			offset = x + y * w;
			offset2 = (x-xinf) + (y-yinf) * w;
			for (int k = 0; k < ordlen; k++)
			{
				redCoefficients.setElement(0, offset*ordlen + k, coeffPtr[k][offset2]);
				greenCoefficients.setElement(0, offset*ordlen + k, coeffPtr[ordlen + k][offset2]);
				blueCoefficients.setElement(0, offset*ordlen + k, coeffPtr[ordlen*2 + k][offset2]);
			}
		}

	delete [] coeffPtr;*/
	return 0;
}


int Hsh::saveCompressed(QString name)
{
	//return saveCompressed(0,0,w,h,0,name);

	return 1;
}


int Hsh::saveCompressed(int xinf, int yinf, int xsup, int ysup, int reslevel, QString name)
{
	//// coordinate adjustment
	//int ww;
	//if (reslevel > 0)
	//{
	//	if (xinf > 0)
	//		xinf = xinf >> reslevel;
	//	if (yinf > 0) 
	//		yinf = yinf >> reslevel;
	//	if (xsup == w)
	//		xsup = mipMapSize[reslevel].width();
	//	else
	//		xsup = xsup >> reslevel;
	//	if (ysup == h)
	//		ysup = mipMapSize[reslevel].height();
	//	else
	//		ysup = ysup >> reslevel;
	//	ww = mipMapSize[reslevel].width();
	//}
	//else
	//	ww = w;

	//int tilew = (xsup - xinf);
	//int tileh = (ysup - yinf);

	//int **comps = new int *[ordlen*bands];
	//for (int k = 0; k < ordlen*bands; k++)
	//	comps[k] = new int[tilew*tileh];

	//int offset, offset2;
	//const float* rPtr = redCoefficients.getLevel(reslevel);
	//const float* gPtr = greenCoefficients.getLevel(reslevel);
	//const float* bPtr = blueCoefficients.getLevel(reslevel);

	//for (int y = yinf; y < ysup; y++)
	//	for (int x = xinf; x < xsup; x++)
	//	{
	//		offset = x + y * ww;
	//		offset2 = (x-xinf) + (y-yinf) * tilew;
	//		for (int k = 0; k < ordlen; k++)
	//		{
	//			comps[k][offset2] = ((rPtr[offset*ordlen + k] - gmax[k])/gmin[k] * 255.0);
	//			comps[ordlen + k][offset2] = ((gPtr[offset*ordlen + k]  - gmax[k])/gmin[k] * 255.0);
	//			comps[ordlen*2 + k][offset2] = ((bPtr[offset*ordlen + k] - gmax[k])/gmin[k] * 255.0);
	//		}
	//	}

	//// Saves as a JPEG2000 image with 27 gray components of 16 bit each
	//Jpeg2000 jpegimage(tilew, tileh, 16, 16, ordlen*bands, comps, GRAY_CLRSPC, J2K_CFMT);
	//jpegimage.save(name.toStdString().c_str());

	//for (int k = 0; k < ordlen*3; k++)
	//	delete [] comps[k];

	//delete [] comps;

	return 0;
}


int Hsh::createImage(unsigned char** buffer, int& width, int& height, const vcg::Point3f& light, const QRectF& rect, int level, int mode)
{
#ifdef PRINT_DEBUG
	QTime first = QTime::currentTime();
#endif
	// Computes the width and the height of the texture.
	width = ceil(rect.width());
	height = ceil(rect.height());
	int offx = rect.x();
	int offy = rect.y();

	if (remote)
	{
		if (level < maxRemoteResolution - minRemoteResolution)
		{
			int size = 1 << maxRemoteResolution;
			float deltaW = static_cast<float>(w)/static_cast<float>(size);
			float deltaH = static_cast<float>(h)/static_cast<float>(size);
			int r1 = static_cast<int>(rect.y() / deltaH);
			int c1 = static_cast<int>(rect.x() / deltaW);
			int r2 = static_cast<int>(rect.bottom() / deltaH);
			int c2 = static_cast<int>(rect.right() / deltaW);
			int result = 15;
			for(int i = r1; i <= r2; i++)
				for (int j = c1; j <= c2; j++)
					result &= tiles[ZOrder::ZIndex(i, j, maxRemoteResolution)];
			bool found = false;
			while(!found && level < maxRemoteResolution - minRemoteResolution)
			{
				if (result & (1 << level))
					found = true;
				else
					level++;
			}
		}
		else
			level = maxRemoteResolution - minRemoteResolution;
	}
	//qDebug() << "Compare with: " << width << "--" << height;
	for (int i = 0; i < level; i++)
	{
		width = ceil(width/2.0);
		height = ceil(height/2.0);
		offx = offx/2;
		offy = offy/2;
	}
	//qDebug() << "Compare with: " << width << "--" << height;
	(*buffer) = new unsigned char[width*height*4];

    // Applies the current rendering mode.
    RenderingInfo info = {offx, offy, height, width, level, mode, light, ordlen};
    list->value(currentRendering)->applyHSH(redCoefficients, greenCoefficients, blueCoefficients, mipMapSize, normals, info, (*buffer));

#ifdef PRINT_DEBUG
	QTime second = QTime::currentTime();
        float diff = first.msecsTo(second) / 1000.0;
        printf("Default rendering: %.5f s\n", diff);
	
#endif

	return 0;
}


QImage* Hsh::createPreview(int width, int height)
{
	// Computes the height and the width of the preview.
	int level = MIP_MAPPING_LEVELS - 1;
	int imageH = mipMapSize[level].height();
	int imageW = mipMapSize[level].width();
	for (int i = 0; i < 4; i++)
	{
		if (mipMapSize[i].width() <= width || mipMapSize[i].height() <= height)
		{
			if (mipMapSize[i].width() < width && mipMapSize[i].height() < height && i > 0)
				i--;
			imageH = mipMapSize[i].height();
			imageW = mipMapSize[i].width();
			level = i;
			break;
		}
	}

	
	// Creates the preview.
	unsigned char* buffer = new unsigned char[imageH*imageW*4];
	int offsetBuf = 0;

	const float* redPtr = redCoefficients.getLevel(level);
	const float* greenPtr = greenCoefficients.getLevel(level);
	const float* bluePtr = blueCoefficients.getLevel(level);
	int tempW = mipMapSize[level].width();
        float hweights[16];
        float phi = 0.0f;
        float theta = acos(1.0);
    getHSH(theta, phi, hweights, sqrt((float)ordlen));
    int offset = 0;
		
	for (int y = 0; y < imageH; y++)
	{
		for (int x = 0; x < imageW; x++)
		{
			offset= y * imageW + x;
                        float val = 0;
			for (int k = 0; k < ordlen; k++)
				val += redPtr[offset*ordlen + k] * hweights[k];
			buffer[offsetBuf + 2] = tobyte(val*255);
			val = 0;
			for (int k = 0; k < ordlen; k++)
				val += greenPtr[offset*ordlen + k] * hweights[k];
			buffer[offsetBuf + 1] = tobyte(val*255);
			val = 0;
			for (int k = 0; k < ordlen; k++)
				val += bluePtr[offset*ordlen + k] * hweights[k];
			buffer[offsetBuf + 0] = tobyte(val*255);
			buffer[offsetBuf + 3] = 255;
			offsetBuf += 4;
		}
	}
    QImage* image = new QImage(buffer, imageW, imageH, QImage::Format_RGB32);

	return image;
}


int Hsh::allocateRemoteImage(QBuffer* b)
{
	//if (!b)
	//	return -1;
	//QDomDocument doc;
	//doc.setContent(b);
	//QDomNode root = doc.firstChild();
	//QDomElement infoNode = root.firstChildElement("Info");
	//if (infoNode.isNull())
	//	return -1;
	//bool error;
	////level info
	//int maxResLevel = infoNode.attribute("levels").toInt(&error);
	//if (!error)
	//	return -1;
	////width info
	//w = infoNode.attribute("width").toInt(&error);
	//if (!error)
	//	return -1;
	////height info
	//h = infoNode.attribute("height").toInt(&error);
	//if (!error)
	//	return -1;
	//ordlen = infoNode.attribute("ordlen").toInt(&error);
	//if (!error)
	//	return -1;
	//bands = infoNode.attribute("bands").toInt(&error);
	//if (!error)
	//	return -1;
	//
	//QDomElement scaleNode = root.firstChildElement("ScaleInfo");
	//if (scaleNode.isNull())
	//	return -1;
	//QStringList scaleList = scaleNode.text().split(" ", QString::SkipEmptyParts);
	//if (scaleList.size() < ordlen)
	//	return -1;
	//for (int i = 0; i < ordlen; i++)
	//{
 //               gmin[i] = scaleList.at(i).toDouble(&error);
	//	if (!error)
	//		return -1;
	//}
	//
	//QDomElement biasNode = root.firstChildElement("BiasInfo");
	//if (biasNode.isNull())
	//	return -1;
	//QStringList biasList = biasNode.text().split(" ", QString::SkipEmptyParts);
	//if (biasList.size() < ordlen)
	//	return -1;
	//for (int i = 0; i < ordlen; i++)
	//{
 //               gmax[i] = biasList.at(i).toDouble(&error);
	//	if (!error)
	//		return -1;
	//}
	//
	//((DefaultRendering*)list->value(DEFAULT))->setRemote(true);
	//remote = true;
	//maxRemoteResolution = maxResLevel;
	//minRemoteResolution = maxResLevel - 3 > 0 ? maxResLevel - 3 : 1;
	//int width, height;
	//for (int i = maxRemoteResolution; i > maxRemoteResolution - 4; i--)
	//{
	//	int n = 1 << (maxRemoteResolution - i);
 //               width = ceil(static_cast<float>(w)/static_cast<float>(n));
 //               height = ceil(static_cast<float>(h)/static_cast<float>(n));
	//	int size = width * height * ordlen;
	//	redCoefficients.allocateLevel(maxRemoteResolution - i, size);
	//	greenCoefficients.allocateLevel(maxRemoteResolution - i, size);
	//	blueCoefficients.allocateLevel(maxRemoteResolution - i, size);
	//	normals.allocateLevel(maxRemoteResolution - i ,width*height);
	//	mipMapSize[maxRemoteResolution - i] = QSize(width, height);
	//}
	//int n = 1 << maxRemoteResolution;
	//tiles = new unsigned int [n*n];
	//for(int i = 0; i <n*n; i++)
	//	tiles[i] = 0;
	//type = "HSH";
	return 0;
}


int Hsh::loadCompressedHttp(QBuffer* b, int xinf, int yinf, int xsup, int ysup, int level)
{
//	unsigned char* stream = (unsigned char*) b->buffer().data();
//	Jpeg2000 jpegimage(stream, b->buffer().length());
//	
//	if (xinf > 0)
//		xinf >>= level;
//	if (yinf > 0)
//		yinf >>= level;
//	
//	if (xsup == w)
//		xsup = mipMapSize[level].width();
//	else
//		xsup = xsup >> level;
//	if (ysup == h)
//		ysup = mipMapSize[level].height();
//	else
//		ysup = ysup >> level;
//	
//	int offset,offset2;
//	
//	int** coeffPtr = new int*[ordlen*bands];
//	for (int i = 0; i < ordlen*bands; i++)
//		coeffPtr[i] = jpegimage.componentData(i);
//	
//
//	Eigen::Vector3d l0(sin(M_PI/4)*cos(M_PI/6), sin(M_PI/4)*sin(M_PI/6), cos(M_PI/4));
//	Eigen::Vector3d l1(sin(M_PI/4)*cos(5*M_PI / 6), sin(M_PI/4)*sin(5*M_PI / 6), cos(M_PI/4));
//	Eigen::Vector3d l2(sin(M_PI/4)*cos(3*M_PI / 2), sin(M_PI/4)*sin(3*M_PI / 2), cos(M_PI/4));
//    float hweights0[16], hweights1[16], hweights2[16];
//	getHSH(M_PI / 4, M_PI / 6, hweights0, ordlen);
//	getHSH(M_PI / 4, 5*M_PI / 6, hweights1, ordlen);
//	getHSH(M_PI / 4, 3*M_PI / 2, hweights2, ordlen);
//	
//	Eigen::Matrix3d L;
//	L.setIdentity();
//	L.row(0) = l0;
//	L.row(1) = l1;
//	L.row(2) = l2;
//	Eigen::Matrix3d LInverse = L.inverse();
//
//	for (int y = yinf; y < ysup; y++)
//		for (int x = xinf; x < xsup; x++)
//		{
//			offset = x + y * mipMapSize[level].width();
//			offset2 = (x-xinf) + (y-yinf) * (xsup - xinf);
//			for (int k = 0; k < ordlen; k++)
//			{
//				redCoefficients.setElement(level, offset*ordlen + k, ((float)coeffPtr[k][offset2] / 255.0) * gmin[k] + gmax[k]);
//				greenCoefficients.setElement(level, offset*ordlen + k, ((float)coeffPtr[ordlen + k][offset2] / 255.0) * gmin[k] + gmax[k]);
//				blueCoefficients.setElement(level, offset*ordlen + k, ((float)coeffPtr[ordlen*2 + k][offset2] / 255.0) * gmin[k] + gmax[k]);
//			}
//
//			Eigen::Vector3d f(0, 0, 0);
//			for (int k = 0; k < ordlen; k++)
//			{
//				f(0) += redCoefficients.getLevel(level)[offset*ordlen + k] * hweights0[k];
//				f(1) += redCoefficients.getLevel(level)[offset*ordlen + k] * hweights1[k];
//				f(2) += redCoefficients.getLevel(level)[offset*ordlen + k] * hweights2[k];
//			}
//			for (int k = 0; k < ordlen; k++)
//			{
//				f(0) += greenCoefficients.getLevel(level)[offset*ordlen + k] * hweights0[k];
//				f(1) += greenCoefficients.getLevel(level)[offset*ordlen + k] * hweights1[k];
//				f(2) += greenCoefficients.getLevel(level)[offset*ordlen + k] * hweights2[k];
//			}
//			for (int k = 0; k < ordlen; k++)
//			{
//				f(0) += blueCoefficients.getLevel(level)[offset*ordlen + k] * hweights0[k];
//				f(1) += blueCoefficients.getLevel(level)[offset*ordlen + k] * hweights1[k];
//				f(2) += blueCoefficients.getLevel(level)[offset*ordlen + k] * hweights2[k];
//			}
//			f /= 3.0;
//			Eigen::Vector3d normal = LInverse * f;
//			normals.setElement(level, offset, vcg::Point3f(normal(0), normal(1), normal(2)).Normalize());
//
//		}
	return 0;
}


void Hsh::saveRemoteDescr(QString& filename, int level)
{
	QDomDocument doc;
	QDomElement root = doc.createElement("RemoteRTIInfo");
	doc.appendChild(root);

	QDomElement info = doc.createElement("Info");
	info.setAttribute(QString("type"), type); 
	info.setAttribute(QString("width"), QString("%1").arg(w));
	info.setAttribute(QString("height"), QString("%1").arg(h));
	info.setAttribute(QString("levels"), QString("%1").arg(level));
	info.setAttribute(QString("ordlen"), QString("%1").arg(ordlen));
	info.setAttribute(QString("bands"), QString("%1").arg(bands));
	root.appendChild(info);

	QDomElement scaleNode = doc.createElement("ScaleInfo");
	QString str;
	for (int i = 0; i < ordlen; i++)
		str.append(QString("%1 ").arg(gmin[i], 0, 'E', 10));
	QDomText scaleInfo = doc.createTextNode(str);
	scaleNode.appendChild(scaleInfo);
	root.appendChild(scaleNode);

	QDomElement biasNode = doc.createElement("BiasInfo");
	str = "";
	for (int i = 0; i < ordlen; i++)
		str.append(QString("%1 ").arg(gmax[i], 0, 'E', 10));
	QDomText biasInfo = doc.createTextNode(str);
	biasNode.appendChild(biasInfo);
	root.appendChild(biasNode);

	QFile infofile(filename);
	if (infofile.open(QFile::WriteOnly | QFile::Truncate))
	{
		QTextStream out(&infofile);
		doc.save(out, 2);
	}

}
