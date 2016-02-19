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


#ifndef PTM_H
#define PTM_H

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#include <cmath>
#endif

// Local headers
#include "rti.h"
#include "pyramid.h"
#include "renderingmode.h"
#include "defaultrendering.h"
#include "diffusegain.h"
#include "specularenhanc.h"
#include "normalenhanc.h"
#include "unsharpmasking.h"
#include "coeffenhanc.h"
#include "detailenhanc.h"
#include "dyndetailenhanc.h"
#include "normalsrendering.h"
//#include <jpeg2000.h>

#include "vtkWidget.h" // YY


// Qt headers
#include <QFile>
#include <QImage>
#include <QVector>
#include <QDebug>

#include <vcg/math/base.h>


//! PTM abstract class
class Ptm : public Rti
{
// private data member
protected:
	
	QString version; /*!< Version. */
 	QSize mipMapSize[MIP_MAPPING_LEVELS]; /*!< Size of mip-mapping levels. */

        float scale[6]; /*!< Scale value. */
	int bias[6]; /*!< Bias value. */


public:

	//! Constructor.
	Ptm(): Rti()
	{
		currentRendering = DEFAULT;
		// Create list of supported rendering mode.
		list = new QMap<int, RenderingMode*>();
		list->insert(DEFAULT, new DefaultRendering());
        list->insert(NORMALS, new NormalsRendering());
        list->insert(DIFFUSE_GAIN, new DiffuseGain());
		list->insert(SPECULAR_ENHANCEMENT, new SpecularEnhancement());
		list->insert(NORMAL_ENHANCEMENT, new NormalEnhancement());
		list->insert(UNSHARP_MASKING_IMG, new UnsharpMasking(0));
		list->insert(UNSHARP_MASKING_LUM, new UnsharpMasking(1));
		list->insert(COEFF_ENHANCEMENT, new CoeffEnhancement());
		list->insert(DETAIL_ENHANCEMENT, new DetailEnhancement());
		list->insert(DYN_DETAIL_ENHANCEMENT, new DynamicDetailEnh());
	};

	//! Deconstructor.
	virtual ~Ptm(){};

	/*!
	  Returns the correct sub-type of PTM.
	  \param in input stream.
	*/
	static Rti* getPtm(QTextStream &in);


	// protected methods
protected:

	
	/*!
	  Computes the normal from the coefficients.
	  \param coeff array of six coefficients.
	  \return the normal.
	*/
	vcg::Point3f calculateNormal(const int* coeff)
	{
                float a[6];
		for (int k = 0; k < 6; k++)
			a[k] = coeff[k]/256.0;
                float lx, ly, lz;
		
		if (vcg::math::Abs(4 * a[1] * a[0] - a[2] * a[2]) < zerotol)
		{
			lx = 0.0;
			ly = 0.0;
		}
		else
		{
			if (vcg::math::Abs(a[2]) < zerotol)
			{
				lx = -a[3] / (2.0 * a[0]);
				ly = -a[4] / (2.0 * a[1]);
			}
			else
			{
				lx = (a[2]*a[4] - 2.0*a[1]*a[3])/(4.0*a[0]*a[1] - a[2]*a[2]);
				ly = (a[2]*a[3] - 2.0*a[0]*a[4])/(4.0*a[0]*a[1] - a[2]*a[2]);
			}
		}

		if (vcg::math::Abs(a[0]) < zerotol && vcg::math::Abs(a[1]) < zerotol && vcg::math::Abs(a[2]) < zerotol
			&& vcg::math::Abs(a[3]) < zerotol && vcg::math::Abs(a[4]) < zerotol)
		{
			lx = 0.0;
			ly = 0.0;
			lz = 1.0;
		}
		else
		{
                        float length2d = lx * lx + ly * ly;
			int maxfound;
			if (4 * a[0] * a[1] - a[2] * a[2] > zerotol && a[0] < -zerotol)
				maxfound = 1;
			else
				maxfound = 0;
			if (length2d > 1 - zerotol || maxfound == 0) {
				int stat = computeMaximumOnCircle(a, lx, ly);
				if (stat == -1) // failed
				{
					length2d = sqrt(length2d);
					if (length2d > zerotol) {
						lx /= length2d;
						ly /= length2d;
					}
				}
			}
                        float disc = 1.0 - lx*lx - ly*ly;
			if (disc < 0.0)
				lz = 0.0;
			else 
				lz = sqrt(disc);
		}
		vcg::Point3f temp(lx, ly, lz);
		temp.Normalize();
		return temp;
	}


	/*!
	  Computes the normals per pixel.
	  \param norm normals pyramid.
	  \param coeff coefficient.
	  \param lrgb flag to discriminate between LRGB and RGB.
	  \param cb callback to update the progress bar.
	  \param offset initial value of the progress bar.
	  \param limit maximum increment for the progress bar value.
	*/
	void calculateNormals(PyramidNormals& norm, const PyramidCoeff& coeff, bool lrgb = true, CallBackPos * cb = 0, int offset = 0, int limit = 0)
	{
		if (cb != NULL)	(*cb)(offset, "Normals generation...");
		for (int level = 0; level < MIP_MAPPING_LEVELS; level++)
		{
			QSize size = mipMapSize[level];
			const PTMCoefficient* coeffLevel = coeff.getLevel(level);
            int lenght = size.width()*size.height();
			vcg::Point3f* normalsLevel = new vcg::Point3f[lenght];
			if (!lrgb)
				memcpy(normalsLevel, norm.getLevel(level), sizeof(vcg::Point3f)*lenght);
			int th_id;
			#pragma omp parallel for
			for (int j = 0; j < size.height(); j++)
			{
				th_id = omp_get_thread_num();
				if (th_id == 0)
				{
					if (cb != NULL && (j % 50 == 0)) (*cb)(offset + level*limit/4.0 + limit/4.0 * j / size.height(), "Normals generation...");
				}
				for (int i = 0; i < size.width(); i++)
				{
					int offset = j * size.width() + i;
					vcg::Point3f temp = calculateNormal(&(coeffLevel[offset][0]));
					if (lrgb)
						normalsLevel[offset] = temp;
					else
						normalsLevel[offset] += temp;
				}
			}
			norm.setLevel(normalsLevel, lenght, level);
		}
	}


	/*!
	  Computes a specific mip-mapping level.
	  \param level mip-mapping level to compute.
	  \param width width of the previus level.
	  \param height height of the previus level.
	  \param cb callback to update the progress bar.
	  \param offset initial value of the progress bar.
	  \param limit maximum increment for the progress bar value.
	*/
	void generateMipMap(int level, int width, int height, CallBackPos * cb = 0, int offset = 0, int limit = 0)
	{
        if (level > 3) return;
		int width2 = ceil(width/2.0);
		int height2 = ceil(height/2.0);
        allocateSubLevel(level, width2, height2);
		int th_id;
        #pragma omp parallel for
        for (int i = 0; i < height - 1; i+=2)
		{
			th_id = omp_get_thread_num();
			if (th_id == 0)
			{
				if (cb != NULL && (i % 50 == 0)) (*cb)(offset + static_cast<float>(i*(limit/2.0)/height), "Mip mapping generation...");
			}
			for (int j = 0; j < width - 1; j+=2)
			{
				int index1 = (i * width + j);
				int index2 = (i * width + j + 1);
				int index3 = ((i + 1) * width + j);
				int index4 = ((i + 1) * width + j + 1);
				int offset = (i/2 * width2 + j/2);
				calculateMipMap(offset, level, index1, index2, index3, index4);
			}
		}
        if (width2 % 2 != 0)
		{
			for (int i = 0; i < height - 1; i+=2)
			{
				int index1 = ((i + 1) * width - 1);
				int index2 = ((i + 2) * width - 1);
				int offset = ((i/2 + 1) * width2 - 1);
				calculateMipMap(offset, level, index1, index2);
			}
		}
		if (height % 2 != 0)
		{
			for (int i = 0; i < width - 1; i+=2)
			{
				int index1 = ((height - 1) * width + i);
				int index2 = ((height - 1) * width + i + 1);
				int offset = ((height2 - 1) * width2 + i/2);
				calculateMipMap(offset, level, index1, index2);
			}
		}
        if (height % 2 != 0 && width % 2 != 0)
		{
			calculateMipMap((height2*width2 - 1), level, (height * width -1));
		}
        mipMapSize[level] = QSize(width2, height2);
		generateMipMap(level+1, width2, height2, cb, offset + limit/2.0, limit/2.0);
	}

// public methods
public:

	
	
private:

	/*!
	  Allocates a specific mip-mapping level.
	  \param level mip-mapping level.
	  \param w width of the level.
	  \param h height of the level.
	*/
	virtual void allocateSubLevel(int level, int w, int h) = 0;
	
	/*!
	  Computes mip-mapping.
	*/
	virtual void calculateMipMap(int pos, int level, int i1) = 0;
	
	/*!
	  Computes mip-mapping.
	*/
	virtual void calculateMipMap(int pos, int level, int i1, int i2) = 0;
	
	/*!
	  Computes mip-mapping.
	*/
	virtual void calculateMipMap(int pos, int level, int i1, int i2, int i3, int i4) = 0;

//accessors
public:

	/*!
	  Sets the version.
	*/
	void setVersion(QString s){version = s;}

//protected: // YY
//	MainWindow * mw() 
//	{
//		foreach (QWidget *widget, QApplication::topLevelWidgets()) {
//			MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
//			if (mainwindow)
//			{
//				return mainwindow;
//			}
//		}
//	}  // current MainWindow

protected:// YY
	MainWindow * mw(); 
};



//! RGB-PTM class
/*!
  The class manages the RGB-PTM image format.
*/
class RGBPtm : public Ptm
{
//private data member
private:

	PyramidCoeff redCoefficients; /*!< Coefficients for red component. */
	PyramidCoeff greenCoefficients; /*!< Coefficients for green component. */
	PyramidCoeff blueCoefficients; /*!< Coefficients for blue component. */

	PyramidNormals normals; /*!< Normals. */
	
// constructors
public:

	//! Constructor.
	RGBPtm();

	//! Deconstructor.
	virtual ~RGBPtm();


// public methods
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

private:
	virtual void allocateSubLevel(int level, int w, int h);
	virtual void calculateMipMap(int pos, int level, int i1);
	virtual void calculateMipMap(int pos, int level, int i1, int i2);
	virtual void calculateMipMap(int pos, int level, int i1, int i2, int i3, int i4);

private:// YY
	void updateCoeffandRender(PTMCoefficient* redCoeff, PTMCoefficient* greenCoeff, PTMCoefficient* blueCoeff, bool FIRST_RTI_RENDERING, bool LOADING_DONE); //YY

};



//! LRGB-PTM class.
/*!
  The class manages the LRGB-PTM image format.
*/
class LRGBPtm : public Ptm
{
//private data member
protected:

	PyramidCoeff coefficients; /*!< Luminance coefficients. */
	PyramidRGB rgb; /*!< RGB components. */
	PyramidNormals normals; /*!< Normals. */

// constructor
public:

	//! Constructor.
	LRGBPtm();

	//! Deconstructor.
	~LRGBPtm();

// public methods
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

private:
	virtual void allocateSubLevel(int level, int w, int h);
	virtual void calculateMipMap(int pos, int level, int i1);
	virtual void calculateMipMap(int pos, int level, int i1, int i2);
	virtual void calculateMipMap(int pos, int level, int i1, int i2, int i3, int i4);

private:// YY 
	void updateCoeffandRender(float* redPtr, float* greenPtr, float* bluePtr, int size, bool FIRST_RTI_RENDERING);

};


//! JPEG LRG-PTM class.
/*!
  The class manages the JPEG LRGB-PTM image format.
*/
class JPEGLRGBPtm : public LRGBPtm
{

public:

	//! Constructor.
	JPEGLRGBPtm();

	//! Deconstructor.
	~JPEGLRGBPtm();

	virtual int load(CallBackPos * cb = 0);
	virtual int load(QString name, CallBackPos * cb = 0);
	virtual int save(QString name);

private:
	
	int indexOf(int x, int* a, int size)
	{
		int answer = -1;
		for (int i = 0; i < size; i++) {
			if (a[i] == x)
				answer = i;
		}
		return answer;
	}

	
	int* combine(int* ref, unsigned char* plane, int size)
	{
		int* result = new int[size];
		for (int i = 0; i < size; i++)
		{
			result[i] = ref[i] + plane[i] - 128;
			if (result[i] < 0)
				result[i] += 256;
		}
		return result;
	}


	int* invert(int* source, int size)
	{
		int* result = new int [size];
		for(int i = 0; i < size; i++)
			result[i] = 255 - source[i];
		return result;
	}

	
	void correctCoeff(int* c, unsigned char* info, int sizeInfo, int w1, int h1)
	{
		for(int i = 0; i < sizeInfo; i+=5)
		{
			int p3 = info[i];
			int p2 = info[i+1];
			int p1 = info[i+2];
			int p0 = info[i+3];
			int v = info[i+4];
			int idx = p3<<24 | p2<<16 | p1<<8 | p0;
			int w2 = idx % w1;
			int h3 = idx / w1;
			int h2 = h1 - h3 - 1;
			int idx2 = h2*w1 + w2;
			c[idx2] = v;
		}
	}

};


#endif  /* PTM_H */

