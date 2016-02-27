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


#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#include <cmath>
#endif

#include "unsharpmasking.h"

#include <QApplication>

UnsharpMControl::UnsharpMControl(int gain, QWidget *parent) : QWidget(parent)
{
    groups.append(new RenderControlGroup(this, "Gain", gain));
    connect(groups.at(0)->spinBox, SIGNAL(valueChanged(int)), this, SIGNAL(gainChanged(int)));
    setLayout(createLayout());
}

bool UnsharpMControl::eventFilter(QObject* watched, QEvent* event)
{
    int s;
    if ((s = getSliderIndex(watched, event)) != -1)
        emit (gainChanged(groups.at(s)->slider->value()));
    return false;
}

/* UnsharpMasking::UnsharpMasking(int n) :
	gain(1.0f),
	minGain(0.01f),
	maxGain(4.0f),
	nIter(5),
	type(n)
	{

	} */
	
	UnsharpMasking::UnsharpMasking(int n) :
	minGain(0.01f),
	maxGain(4.0f),
	nIter(5),
	type(n)
	{

	}
	float UnsharpMasking::gain = 1.0f;


UnsharpMasking::~UnsharpMasking() 
{

}
	
QString UnsharpMasking::getTitle() 
{
	if (type == 0)
		return "Image Unsharp Masking";
	else if (type == 1)
		return "Luminance Unsharp Masking";
	return "Unsharp Masking";
}


QWidget* UnsharpMasking::getControl(QWidget* parent)
{
    int initGain = roundParam((gain - minGain)*100/(maxGain - minGain));
	UnsharpMControl* control = new UnsharpMControl(initGain, parent);
	connect(control, SIGNAL(gainChanged(int)), this, SLOT(setGain(int)));
	disconnect(this, SIGNAL(refreshImage()), 0, 0);
	connect(this, SIGNAL(refreshImage()), parent, SIGNAL(updateImage()));
	return control;
}


bool UnsharpMasking::isLightInteractive()
{
	return true;
}


bool UnsharpMasking::supportRemoteView()
{
	return false;
}


bool UnsharpMasking::enabledLighting()
{
	return true;
}


float UnsharpMasking::getGain()
{
    // Get gain as a value normalized to the range [0,100]

    return (gain - minGain)*100.0/(maxGain - minGain);
}

void UnsharpMasking::setGain(int value)
{
	gain = minGain + value * (maxGain - minGain)/100;
	emit refreshImage();
}

void UnsharpMasking::applyPtmLRGB(const PyramidCoeff& coeff, const PyramidRGB& rgb, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
	int offsetBuf = 0;
	const PTMCoefficient* coeffPtr = coeff.getLevel(info.level);
	const unsigned char* rgbPtr = rgb.getLevel(info.level);
    float* lumMap = new float[info.width*info.height];
	int width = mipMapSize[info.level].width();
	if (type == 0) //image unsharp masking
	{
		// Creates a map for Y component and a map for UV component. 
        float* uvMap = new float[info.width*info.height*2];
		LightMemoized lVec(info.light.X(), info.light.Y());
        
		#pragma omp parallel for schedule(static,CHUNK)
		for (int y = info.offy; y < info.offy + info.height; y++)
		{
            int offsetBuf = (y-info.offy)*info.width<<2;
			int offset = (y * width + info.offx)*3;
			int offset2 = ((y - info.offy)*info.width)*2;
			for (int x = info.offx; x < info.offx + info.width; x++)
			{
			    float lum =  coeffPtr[offset / 3].evalPoly(lVec) / 255.0;
                float r = rgbPtr[offset]*lum / 255.0;
                float g = rgbPtr[offset + 1]*lum / 255.0;
                float b = rgbPtr[offset + 2]*lum / 255.0;
				getYUV(r, g, b, lumMap[offset2 / 2], uvMap[offset2], uvMap[offset2 + 1]);
				offset += 3;
				offset2 += 2;
			}
		}
		// Computes the enhanced luminance.
		enhancedLuminance(lumMap, info.width, info.height, info.mode);
		// Creates the output texture.
		bool flag = (info.mode == LUM_UNSHARP_MODE || info.mode == SMOOTH_MODE || info.mode == CONTRAST_MODE ||info.mode == ENHANCED_MODE);
		if (flag)
		{
			#pragma omp parallel for schedule(static,CHUNK)
			for (int y = info.offy; y < info.offy + info.height; y++)
			{
				int offsetBuf = (y - info.offy) * info.width << 2;
				int offset2 =(y - info.offy) * info.width;
				for (int x = info.offx; x < info.offx + info.width; x++)
				{
					for (int i = 0; i < 3; i++)
						buffer[offsetBuf + i] = tobyte(lumMap[offset2] * 255.0);
					buffer[offsetBuf + 3] = 255;
					offsetBuf += 4;
					offset2++;
				}
			}
		}
		else
		{
			#pragma omp parallel for schedule(static,CHUNK)
			for (int y = info.offy; y < info.offy + info.height; y++)
			{
				int offsetBuf = (y - info.offy) * info.width << 2;
				int offset2 =(y - info.offy)*info.width * 2;
				for (int x = info.offx; x < info.offx + info.width; x++)
				{
					float r, g, b;
					getRGB(lumMap[offset2 / 2], uvMap[offset2], uvMap[offset2 + 1], r, g, b);
					buffer[offsetBuf ] = tobyte(r*255);
					buffer[offsetBuf + 1] = tobyte(g*255);
					buffer[offsetBuf + 2] = tobyte(b*255);
					buffer[offsetBuf + 3] = 255;
					offsetBuf += 4;
					offset2 += 2;
				}
			}
		}
		delete[] uvMap;
	}
	else //unsharp masking luminance
	{
		// Creates a map for the polynomial luminance.
		LightMemoized lVec(info.light.X(), info.light.Y());
		
		#pragma omp parallel for schedule(static,CHUNK)
		for (int y = info.offy; y < info.offy + info.height; y++)
		{
			int offset = y * width + info.offx;
			int offset2 = (y - info.offy)*info.width;
			for (int x = info.offx; x < info.offx + info.width; x++)
			{
				lumMap[offset2] = coeffPtr[offset].evalPoly(lVec) / 255.0;
				offset++;
				offset2++;
			}
		}
		// Computes the enhanced luminance
		enhancedLuminance(lumMap, info.width, info.height, info.mode);
		// Creates the output texture.
		bool flag = (info.mode == LUM_UNSHARP_MODE || info.mode == SMOOTH_MODE || info.mode == CONTRAST_MODE || info.mode == ENHANCED_MODE);
        if (flag)
		{
			#pragma omp parallel for schedule(static,CHUNK)
			for (int y = info.offy; y < info.offy + info.height; y++)
			{
				int offsetBuf = (y - info.offy) * info.width <<2;
				int offset2 = (y - info.offy) * info.width;
				for (int x = info.offx; x < info.offx + info.width; x++)
				{
					for (int i = 0; i < 3; i++)
						buffer[offsetBuf + i] = tobyte(lumMap[offset2] / 2.0 * 255.0);
					buffer[offsetBuf + 3] = 255;
					offsetBuf += 4;
					offset2++;
				}
			}
		}
		else
		{
			#pragma omp parallel for schedule(static,CHUNK)
			for (int y = info.offy; y < info.offy + info.height; y++)
			{
				int offsetBuf = (y - info.offy) * info.width << 2;
				int offset = (y * width + info.offx)*3;
				int offset2 = (y - info.offy) * info.width; 
				for (int x = info.offx; x < info.offx + info.width; x++)
				{
					for (int i = 0; i < 3; i++)
						buffer[offsetBuf + i] = tobyte(rgbPtr[offset + i] * lumMap[offset2]);
					buffer[offsetBuf + 3] = 255;
					offsetBuf += 4;
					offset += 3;
					offset2++;
				}
			}

		}
		
	}
	delete[] lumMap;
}


void UnsharpMasking::applyPtmRGB(const PyramidCoeff& redCoeff, const PyramidCoeff& greenCoeff, const PyramidCoeff& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
	int offsetBuf = 0;
	const PTMCoefficient* redPtr = redCoeff.getLevel(info.level);
	const PTMCoefficient* greenPtr = greenCoeff.getLevel(info.level);
	const PTMCoefficient* bluePtr = blueCoeff.getLevel(info.level);
	const vcg::Point3f* normalsPtr = normals.getLevel(info.level);
    float* lumMap = new float[info.width*info.height];
	int width = mipMapSize[info.level].width();
	if (type == 0) //classic unsharp masking
	{
        float* uvMap = new float[info.width*info.height*2];
		LightMemoized lVec(info.light.X(), info.light.Y());
        
		#pragma omp parallel for schedule(static,CHUNK)
		for (int y = info.offy; y < info.offy + info.height; y++)
		{
            int offsetBuf = (y-info.offy)*info.width<<2;
			int offset = y * width + info.offx;
			int offset2 = (y - info.offy)*info.width;
			for (int x = info.offx; x < info.offx + info.width; x++)
			{
				float r = redPtr[offset].evalPoly(lVec) / 255.0;//evalPoly((int*)&(redPtr[offset][0]), info.light.X(), info.light.Y()) / 255.0;
                float g = greenPtr[offset].evalPoly(lVec) / 255.0;//evalPoly((int*)&(greenPtr[offset][0]), info.light.X(), info.light.Y()) / 255.0;
                float b = bluePtr[offset].evalPoly(lVec) / 255.0;//evalPoly((int*)&(bluePtr[offset][0]), info.light.X(), info.light.Y()) / 255.0;
				getYUV(r, g, b, lumMap[offset2], uvMap[offset2*2], uvMap[offset2*2 + 1]);
				offset++;
				offset2++;
			}
		}
		enhancedLuminance(lumMap, info.width, info.height, info.mode);
		bool flag = (info.mode == LUM_UNSHARP_MODE || info.mode == SMOOTH_MODE || info.mode == CONTRAST_MODE || info.mode == ENHANCED_MODE);
		if (flag)
		{
			#pragma omp parallel for schedule(static,CHUNK)
			for (int y = info.offy; y < info.offy + info.height; y++)
			{
				int offsetBuf = (y-info.offy)*info.width<<2;
				int offset2 =(y - info.offy)*info.width; 
				for (int x = info.offx; x < info.offx + info.width; x++)
				{
					for (int i = 0; i < 3; i++)
						buffer[offsetBuf + i] = tobyte(lumMap[offset2] * 255.0);
					buffer[offsetBuf + 3] = 255;
					offsetBuf += 4;
					offset2++;
				}
			}
		}
		else
		{
			#pragma omp parallel for schedule(static,CHUNK)
			for (int y = info.offy; y < info.offy + info.height; y++)
			{
				int offsetBuf = (y-info.offy)*info.width<<2;
				int offset2 =(y - info.offy)*info.width; 
				for (int x = info.offx; x < info.offx + info.width; x++)
				{
					float r, g, b;
					getRGB(lumMap[offset2], uvMap[offset2*2], uvMap[offset2*2 +1], r, g, b);
					buffer[offsetBuf] = tobyte(r*255);
					buffer[offsetBuf + 1] = tobyte(g*255);
					buffer[offsetBuf + 2] = tobyte(b*255);
					buffer[offsetBuf + 3] = 255;
					offsetBuf += 4;
					offset2++;
				}
			}
		}
		delete[] uvMap;
	}
	else //luminance unsharp masking
	{
        #pragma omp parallel for schedule(static,CHUNK)
		for (int y = info.offy; y < info.offy + info.height; y++)
		{
			int offset= y * width + info.offx;
			int offset2 = (y - info.offy)*info.width;
			for (int x = info.offx; x < info.offx + info.width; x++)
			{
				lumMap[offset2] = getLum(normalsPtr[offset], info.light);
				offset++;
				offset2++;
			}
		}
		enhancedLuminance(lumMap, info.width, info.height, info.mode);
		bool flag = (info.mode == LUM_UNSHARP_MODE || info.mode == SMOOTH_MODE || info.mode == CONTRAST_MODE || info.mode == ENHANCED_MODE);
		LightMemoized lVec(info.light.X(), info.light.Y());
		if (flag)
		{
			#pragma omp parallel for schedule(static,CHUNK)
			for (int y = info.offy; y < info.offy + info.height; y++)
			{
				int offsetBuf = (y-info.offy)*info.width<<2;
				int offset = y * width + info.offx;
				int offset2 = (y - info.offy)*info.width;
				for (int x = info.offx; x < info.offx + info.width; x++)
				{
					float lum = lumMap[offset2];
					for (int i = 0; i < 3; i++)
						buffer[offsetBuf + i] = tobyte(lum / 2.0 * 255.0);
					buffer[offsetBuf + 3] = 255;
					offsetBuf += 4;
					offset++;
					offset2++;
				}
			}
		}
		else
		{
			#pragma omp parallel for schedule(static,CHUNK)
			for (int y = info.offy; y < info.offy + info.height; y++)
			{
				int offsetBuf = (y-info.offy)*info.width<<2;
				int offset = y * width + info.offx;
				int offset2 = (y - info.offy)*info.width;
				for (int x = info.offx; x < info.offx + info.width; x++)
				{
					float lum = lumMap[offset2];
					buffer[offsetBuf] = tobyte(redPtr[offset].evalPoly(lVec)*lum);//evalPoly((int*)&(redPtr[offset][0]), info.light.X(), info.light.Y()) * lum);
					buffer[offsetBuf + 1] = tobyte(greenPtr[offset].evalPoly(lVec)*lum);//evalPoly((int*)&(greenPtr[offset][0]), info.light.X(), info.light.Y()) * lum);
					buffer[offsetBuf + 2] = tobyte(bluePtr[offset].evalPoly(lVec)*lum);//evalPoly((int*)&(bluePtr[offset][0]), info.light.X(), info.light.Y()) * lum);
					buffer[offsetBuf + 3] = 255;
					offsetBuf += 4;
					offset++;
					offset2++;
				}
			}
		}
	}
	delete[] lumMap;
}


void UnsharpMasking::enhancedLuminance(float* lumMap, int width, int height, int mode)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	int dist = 2;
    float* tempLum = new float[width*height];
    float* smootLum = new float[width*height];
	int* nKernel = new int[width*height];
    memcpy(smootLum, lumMap, width*height*sizeof(float));
	for (int i = 0; i < nIter; i++)
	{
        memset(tempLum, 0, width*height*sizeof(float));
        #pragma omp parallel for schedule(static,CHUNK)
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int offset = y * width + x;
				int sx, ex, sy, ey;
				sx = x - dist < 0? 0 : x - dist;
				ex = x >= width - dist? width - 1: x + dist; 
				sy = y - dist < 0? 0: y - dist;
				ey = y >= height - dist? height - 1: y + dist;
				int n = (ex - sx + 1)*(ey - sy + 1);
				nKernel[offset] = n;
				if (x > 0)
				{
					tempLum[offset] = tempLum[offset-1];
					if (x <= dist)
					{
						for(int jj = sy; jj <= ey; jj++)
							tempLum[offset] += smootLum[jj*width + x + dist];
					}
					else
					{
						for(int jj = sy; jj <= ey; jj++)
						{
							tempLum[offset] -= smootLum[jj*width + x - dist - 1];
							if (x + dist < width)
								tempLum[offset] += smootLum[jj*width + x + dist];
						}
					}

				}
				else
				{
					for (int ii = sx; ii <= ex; ii++)
						for(int jj = sy; jj <= ey; jj++)
							tempLum[offset] += smootLum[jj*width + ii];					
				}
			}
		}
		#pragma omp parallel for schedule(static,CHUNK)
		for (int ii = 0; ii < width*height; ii++)
			tempLum[ii] /= static_cast<float>(nKernel[ii]);
        memcpy(smootLum, tempLum, width*height*sizeof(float));
	}
	delete[] tempLum;
   
	switch(mode)
	{
		case LUM_UNSHARP_MODE:
			break;
		case SMOOTH_MODE:
			#pragma omp parallel for schedule(static,CHUNK)
			for (int i = 0; i < height*width; i++)
				lumMap[i] = smootLum[i];
			break;
		case CONTRAST_MODE:
			#pragma omp parallel for schedule(static,CHUNK)
			for (int i = 0; i < height*width; i++)
				lumMap[i] = (lumMap[i] - smootLum[i])*4.0f;
			break;
		default:
			#pragma omp parallel for schedule(static,CHUNK)
			for (int i = 0; i < height*width; i++)
				lumMap[i] = lumMap[i] + gain *(lumMap[i] - smootLum[i]);
			break;		
	}
	delete[] smootLum;
	delete[] nKernel;
	QApplication::restoreOverrideCursor();
}


float UnsharpMasking::getLum(const vcg::Point3f& normal, const vcg::Point3f& l)
{
    float nDotL = normal*l;
	if (nDotL < 0) 
		nDotL = 0.0;
	else if (nDotL > 1)
		nDotL = 1.0;
	return nDotL;
}


void UnsharpMasking::getYUV(float r, float g, float b, float& l, float& u, float& v)
{
	l = r * 0.299 + g * 0.587 + b * 0.144;
	u = r * -0.14713 + g * -0.28886 + b * 0.436;
	v = r * 0.615 + g * -0.51499 + b * -0.10001;
}

void UnsharpMasking::getRGB(float y, float u, float v, float& r, float& g, float& b)
{
	r = y + v * 1.13983;
	g = y + u * -0.39465 + v * -0.5806;
	b = y + u * 2.03211;
}
