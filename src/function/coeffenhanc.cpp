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

#include "coeffenhanc.h"

#include <QApplication>

#include <omp.h>

CoeffEnhancControl::CoeffEnhancControl(int gain, QWidget *parent) : QWidget(parent)
{
    groups.append(new RenderControlGroup(this, "Gain", gain));
    connect(groups.at(0)->spinBox, SIGNAL(valueChanged(int)), this, SIGNAL(gainChanged(int)));
    setLayout(createLayout());
}

bool CoeffEnhancControl::eventFilter(QObject* watched, QEvent* event)
{
    int s;
    if ((s = getSliderIndex(watched, event)) != -1)
        emit (gainChanged(groups.at(s)->slider->value()));
    return false;
}

/* CoeffEnhancement::CoeffEnhancement() :
	gain(1.0f),
	minGain(0.01f),
	maxGain(6.0f),
	nIter(2)
	{

	} */
	
	CoeffEnhancement::CoeffEnhancement() :
	minGain(0.01f),
	maxGain(6.0f),
	nIter(2)
	{

	}
	float CoeffEnhancement::gain = 1.0f;

CoeffEnhancement::~CoeffEnhancement() 
{

}

	
QString CoeffEnhancement::getTitle() 
{
	return "Coefficient Unsharp Masking";
}


QWidget* CoeffEnhancement::getControl(QWidget* parent)
{
    int initGain = roundParam((gain - minGain)*100/(maxGain - minGain));
	CoeffEnhancControl* control = new CoeffEnhancControl(initGain, parent);
	connect(control, SIGNAL(gainChanged(int)), this, SLOT(setGain(int)));
	disconnect(this, SIGNAL(refreshImage()), 0, 0);
	connect(this, SIGNAL(refreshImage()), parent, SIGNAL(updateImage()));
	return control;
}


bool CoeffEnhancement::isLightInteractive()
{
	return true;
}


bool CoeffEnhancement::supportRemoteView()
{
	return false;
}


bool CoeffEnhancement::enabledLighting() 
{
	return true;
}


float CoeffEnhancement::getGain()
{
    return (gain - minGain)*100.0/(maxGain - minGain);
}


void CoeffEnhancement::setGain(int value)
{
	gain = minGain + value * (maxGain - minGain)/100;
	emit refreshImage();
}

void CoeffEnhancement::applyPtmLRGB(const PyramidCoeff& coeff, const PyramidRGB& rgb, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
	//int offsetBuf = 0;
	const PTMCoefficient* coeffPtr = coeff.getLevel(info.level);
	const unsigned char* rgbPtr = rgb.getLevel(info.level);
	PTMCoefficient* coeffMap = new PTMCoefficient[info.width*info.height];
	int width = mipMapSize[info.level].width();
	// Creates the map of the coefficients of the sub-image in the current view of the browser
	#pragma omp parallel for schedule(static,CHUNK)
	for (int y = info.offy; y < info.offy + info.height; y++)
	{
		int offset = y * width + info.offx;
		int offset2 = (y - info.offy)*info.width;
		for (int x = info.offx; x < info.offx + info.width; x++)
		{
			memcpy(coeffMap[offset2], coeffPtr[offset], sizeof(PTMCoefficient));
			offset++;
			offset2++;
		}
	}
	// Computes the enhanced coefficients.
	enhancedCoeff(coeffMap, info.width, info.height, 6);
	// Creates the output texture.
	LightMemoized lVec(info.light.X(), info.light.Y());
	
	#pragma omp parallel for schedule(static,CHUNK)
	for (int y = info.offy; y < info.offy + info.height; y++)
	{
                int offsetLoc = (y-info.offy)*info.width;
                int offsetBuf = offsetLoc << 2;
		int offset = y * width + info.offx;
		for (int x = info.offx; x < info.offx + info.width; x++)
		{
                        float lum = coeffMap[offsetLoc].evalPoly(lVec) / 255.0f;
			int offset3 = offset * 3;
			for (int i = 0; i < 3; i++)
				buffer[offsetBuf + i] = tobyte(rgbPtr[offset3 + i] * lum);
			buffer[offsetBuf + 3] = 255;
			offsetBuf +=4;
                        offset++;
                        offsetLoc++;
		}
	}
	delete[] coeffMap;
}


void CoeffEnhancement::applyPtmRGB(const PyramidCoeff& redCoeff, const PyramidCoeff& greenCoeff, const PyramidCoeff& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
	//int offsetBuf = 0;
	const PTMCoefficient* redPtr = redCoeff.getLevel(info.level);
	const PTMCoefficient* greenPtr = greenCoeff.getLevel(info.level);
	const PTMCoefficient* bluePtr = blueCoeff.getLevel(info.level);
	int lenght = info.width * info.height;
	PTMCoefficient* redC = new PTMCoefficient[lenght];
	PTMCoefficient* greenC = new PTMCoefficient[lenght];
	PTMCoefficient* blueC = new PTMCoefficient[lenght];
	int width = mipMapSize[info.level].width();
	// Creates the map of the coefficients of the sub-image in the current view of the browser
	#pragma omp parallel for schedule(static,CHUNK)
	for (int y = info.offy; y < info.offy + info.height; y++)
	{
		int offset = y * width + info.offx;
		int offset2 = (y - info.offy)*info.width;
		for (int x = info.offx; x < info.offx + info.width; x++)
		{
			memcpy(&redC[offset2], &redPtr[offset], sizeof(PTMCoefficient));
			memcpy(&greenC[offset2], &greenPtr[offset], sizeof(PTMCoefficient));
			memcpy(&blueC[offset2], &bluePtr[offset], sizeof(PTMCoefficient));
			offset++;
			offset2++;
		}
	}
	// Computes the enhanced coefficients
	enhancedCoeff(redC, info.width, info.height, 6);
	enhancedCoeff(greenC, info.width, info.height, 6);
	enhancedCoeff(blueC, info.width, info.height, 6);
	// Creates the output texture.	
	LightMemoized lVec(info.light.X(), info.light.Y());
	
	#pragma omp parallel for schedule(static,CHUNK)
	for (int y = info.offy; y < info.offy + info.height; y++)
	{
		int offsetBuf = (y-info.offy)*info.width*4;
		int offset2 = (y - info.offy)*info.width;
		for (int x = info.offx; x < info.offx + info.width; x++)
		{
			buffer[offsetBuf] = tobyte(redC[offset2].evalPoly(lVec)); 
			buffer[offsetBuf + 1] = tobyte(greenC[offset2].evalPoly(lVec));
			buffer[offsetBuf + 2] = tobyte(blueC[offset2].evalPoly(lVec));
			buffer[offsetBuf + 3] = 255;
			offsetBuf += 4;
			offset2++;
		}
	}
	delete[] redC;
	delete[] greenC;
	delete[] blueC;
}


void CoeffEnhancement::enhancedCoeff(PTMCoefficient *coeffMap, int width, int height, int ncomp)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	int dist = 1;
	float* tempCoeff = new float[width*height*ncomp];
	float* smootCoeff = new float[width*height*ncomp];
	int* nKernel = new int[width*height];

	
	#pragma omp parallel for schedule(static,CHUNK)
	for (int i = 0; i < width*height*ncomp; i++)
		smootCoeff[i] = coeffMap[i/6][i%6];

	for (int i = 0; i < nIter; i++)
	{
		memset(tempCoeff, 0, width*height*ncomp*sizeof(float));
		#pragma omp parallel for schedule(static,CHUNK)
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int offset = y * width + x;
				int sx, ex, sy, ey;
				sx = x == 0? x: x - 1;
				ex = x == width - 1? x: x + 1; 
				sy = y == 0? y: y - 1;
				ey = y == height - 1? y: y + 1;
				int n = (ex - sx + 1)*(ey - sy + 1);
				nKernel[offset] = n;
				if (x > 0)
				{
					for(int k = 0; k < ncomp; k++)
					{
						tempCoeff[offset*ncomp+k] = tempCoeff[(offset-1)*ncomp +k];
						if (x <= dist)
						{
							for(int jj = sy; jj <= ey; jj++)
								tempCoeff[offset*ncomp+k] += smootCoeff[(jj*width + x + dist)*ncomp + k];
						}
						else
						{
							for(int jj = sy; jj <= ey; jj++)
							{
								tempCoeff[offset*ncomp+k] -= smootCoeff[(jj*width + x - dist - 1)*ncomp + k];
								if (x + dist < width)
									tempCoeff[offset*ncomp+k] += smootCoeff[(jj*width + x + dist)*ncomp + k];
							}
						}
					}

				}
				else
				{
					for (int ii = sx; ii <= ex; ii++)
						for(int jj = sy; jj <= ey; jj++)
							for(int k = 0; k < ncomp; k++)
								tempCoeff[offset*ncomp+k] += smootCoeff[(jj*width + ii)*ncomp +k];					
				}
			}
		}
		#pragma omp parallel for schedule(static,CHUNK)
		for (int y = 0; y < height; y++)
		{		
			int offset = y * width;
			for (int x = 0; x < width; x++)
			{
				for (int k = 0; k < ncomp; k++)
					tempCoeff[offset*ncomp+k] /= nKernel[offset];
				offset++;
			}
		}
		memcpy(smootCoeff, tempCoeff, width*height*ncomp*sizeof(float));
	}
	delete[] tempCoeff;
	#pragma omp parallel for schedule(static,CHUNK)
	for (int i = 0; i < height*width*ncomp; i++)
		coeffMap[i/6][i%6] = coeffMap[i/6][i%6] + gain *(coeffMap[i/6][i%6] - smootCoeff[i]);
	delete[] smootCoeff;
	delete[] nKernel;
	QApplication::restoreOverrideCursor();
}
