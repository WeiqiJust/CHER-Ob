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

#include <QApplication>
#include <QColor>
#include <QPainter>
#include <QTime>

#include "detailenhanc.h"
#include "loadingdlg.h"

#include "../../rtiwebmaker/src/zorder.h"

#include <omp.h>

DetailEnhancement::DetailEnhancement():
	bufferReady(false),
	detailsBuffer(NULL),
	zMatrix(NULL),
	vectImage(NULL),
	loadParent(NULL),
	nOffset(OFFSET_10),
	minTileSize(TILE_SIZE_1),
	minLevel(2),
	sharpnessOperator(MAX_ENERGY_LAPLACE),
	sphereSampl(NON_UNIFORM),
	k1(0.7f),
	k2(0.3f),
	threshold(0.7f),
	filter(FILTER_3x3),
	nIterSmoothing(2)
{
	// Computes the initial light samples on entire hemishpere.
	std::vector<vcg::Point3f> uniformDirVec;
	vcg::GenNormal<float>::UniformCone(256, uniformDirVec, vcg::math::ToRad(80.0f), vcg::Point3f(0,0,1));
	std::random_shuffle(uniformDirVec.begin(), uniformDirVec.end());
	float limit = cos(vcg::math::ToRad(40.0f));
	for (unsigned int i = 0; i < uniformDirVec.size(); i++)
	{
		if (uniformDirVec[i].Z() < limit)
			defaultSamples.push_back(uniformDirVec[i]);
	}
}


DetailEnhancement::~DetailEnhancement() 
{
	if (detailsBuffer)
		delete[] detailsBuffer;
	if (zMatrix)
		delete[] zMatrix;
	if (vectImage)
		delete vectImage;
}



QString DetailEnhancement::getTitle() 
{
	return "Static Multi Light";
}


QWidget* DetailEnhancement::getControl(QWidget* parent)
{
	loadParent = parent;
	DetailConfDlg* advanced = new DetailConfDlg(parent);
	advanced->setCurrentValue(nOffset, minTileSize, minLevel, sharpnessOperator, sphereSampl, k1, k2, threshold, filter, nIterSmoothing);
	DetailEnhDlg* dialog = new DetailEnhDlg(parent);
	dialog->setAdvacedDlg(advanced);
	disconnect(this, SIGNAL(refreshImage()), 0, 0);
	connect(this, SIGNAL(refreshImage()), parent, SIGNAL(updateImage()));
	connect(advanced, SIGNAL(configUpdated(OffsetNum, TileSize, int, SharpnessMeasures, SphereSampling, float, float, float, SmoothingFilter, int)),
			this, SLOT(updateConfig(OffsetNum, TileSize, int, SharpnessMeasures, SphereSampling, float, float, float, SmoothingFilter, int)));
	return dialog;
}


bool DetailEnhancement::isLightInteractive()
{
	return false;
}

bool DetailEnhancement::supportRemoteView()
{
	return false;
}


bool DetailEnhancement::enabledLighting()
{
	return false;
}


void DetailEnhancement::applyPtmLRGB(const PyramidCoeff& coeff, const PyramidRGB& rgb, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
#ifdef PRINT_DEBUG
	QTime first2 = QTime::currentTime();
#endif

	if(!bufferReady)
	{
		// Computes detail enhancement.
		bufferReady = true;
		coefficient = &coeff; 
		color = &rgb;
		lrgb = true;
		if (zMatrix)
			delete[] zMatrix;
		if (vectImage)
			delete vectImage;
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		LoadingDlg* loading = new LoadingDlg(loadParent);
		loading->setWindowTitle("Static Multi Light Detail Enhancement...");
		loading->show();
		calcDetails(mipMapSize, LoadingDlg::QCallBack);
		loading->close();
		delete loading;
		QApplication::restoreOverrideCursor();
	}

#ifdef PRINT_DEBUG
	QTime second2 = QTime::currentTime();
        float diff = first2.msecsTo(second2) / 1000.0;
        printf("Details extraction: %.5f s\n", diff);
#endif
	
	// Creates the output texture.
	int offsetBuf = 0;
	bool f = (info.mode == LIGHT_VECTOR);
	
	#pragma omp parallel for schedule(static,CHUNK)
	for (int y = info.offy; y < info.offy + info.height; y++)
	{
		int offsetBuf = (y-info.offy)*info.width*4;
		for (int x = info.offx; x < info.offx + info.width; x++)
		{
			int offset= y * mipMapSize[0].width() + x;
			if (f)
			{
				// Draws the light vectors.
				int value = getLightVectImagePixel(offset, x, y);
				memcpy(&buffer[offsetBuf], &value, 4*sizeof(unsigned char));
			}
			else
			{
				for(int i = 0; i < 3; i++)
					buffer[offsetBuf + i] = detailsBuffer[offset*4 + i];
				buffer[offsetBuf + 3] = 255;
			}
			offsetBuf += 4;
		}
	}
}


void DetailEnhancement::applyPtmRGB(const PyramidCoeff& redCoeff, const PyramidCoeff& greenCoeff, const PyramidCoeff& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
#ifdef PRINT_DEBUG
		QTime first2 = QTime::currentTime();
#endif

		if(!bufferReady)
		{
			// Computes the detail enhancement.
			bufferReady = true;
			coefficientR = &redCoeff; 
			coefficientG = &greenCoeff;
			coefficientB = &blueCoeff;
			lrgb = false;
			if (zMatrix)
				delete[] zMatrix;
			if (vectImage)
				delete vectImage;
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			LoadingDlg* loading = new LoadingDlg(loadParent);
			loading->setWindowTitle("Static Multi Light Detail Enhancement...");
			loading->show();
			calcDetails(mipMapSize, LoadingDlg::QCallBack);
			loading->close();
			delete loading;
			QApplication::restoreOverrideCursor();
		}
		
#ifdef PRINT_DEBUG
		QTime second2 = QTime::currentTime();
                float diff = first2.msecsTo(second2) / 1000.0;
                printf("Detail extraction: %.5f s\n", diff);
#endif	

		// Creates the output texture.
		int offsetBuf = 0;
		bool f = (info.mode == LIGHT_VECTOR);
		
        #pragma omp parallel for schedule(static,CHUNK)
		for (int y = info.offy; y < info.offy + info.height; y++)
		{
			int offsetBuf = (y-info.offy)*info.width*4;
			for (int x = info.offx; x < info.offx + info.width; x++)
			{
				int offset= y * mipMapSize[0].width() + x;
				if (f)
				{
					// Draws the light vectors.
					int value = getLightVectImagePixel(offset, x, y);
					memcpy(&buffer[offsetBuf], &value, 4*sizeof(unsigned char));
				}
				else
				{
					for(int i = 0; i < 3; i++)
						buffer[offsetBuf + i] = detailsBuffer[offset*4 + i];
					buffer[offsetBuf + 3] = 255;
				}
				offsetBuf += 4;
			}
		}

}


void DetailEnhancement::calcDetails(const QSize* mipMapSize, CallBackPos * cb)
{
	if (cb != NULL)(*cb)(0, "Details extraction...");
    float temp = mipMapSize[0].width() > mipMapSize[0].height() ? mipMapSize[0].height(): mipMapSize[0].width();
	vectImage = new QImage(mipMapSize[0], QImage::Format_ARGB32);
	int value = qRgba(255, 255, 255, 0);
	vectImage->fill(value);
	temp /= minTileSize*2;
	// Computes the min and max levels of suddivition in tiles.
	maxLevel = log10(temp)/log10(2.0);
	if (maxLevel < 3)
		maxLevel = 3;
	if (maxLevel - minLevel < 5)
		minLevel = maxLevel - 5;
	if (minLevel < 0)
		minLevel = 0;
        float limit = 80 /(maxLevel - minLevel + 1);
	std::vector< std::vector<vcg::Point3f> > levelLight(maxLevel - minLevel +1);
	for (int i = 0; i < maxLevel - minLevel + 1; i++)
	{
		int n = 1 << ((minLevel + i + 1)*2);
		levelLight[i] = std::vector<vcg::Point3f>(n);
	}
	int size = 1 << (maxLevel + 1);
	tilesLight.empty();
	tilesCenter.empty();
	tilesLight = std::vector<vcg::Point3f>(size*size);
	tilesCenter = std::vector<vcg::Point2f>(size*size);
	// Extracts light directions from the info of mip-mapping levels.
	for (int level = 3; level >= 0; level--)
	{
		int n = 1 << (minLevel - level + 4);
		float deltaW = static_cast<float>(mipMapSize[level].width()) / static_cast<float>(n);
		float deltaH = static_cast<float>(mipMapSize[level].height()) / static_cast<float>(n);
		zMatrix = ZOrder::createZMatrix(minLevel - level + 4);
		int nParent = n*n/4;
		std::vector<std::vector<vcg::Point3f>*> parentLightSample(nParent);
		std::vector<std::vector<vcg::Point3f>*> tempLight(n*n);
		std::vector<bool> alreadyCal(nParent); 
		// Computes the better light vector for each tile.
		int th_id;
		#pragma omp parallel for private(th_id)
		for (int i = 0; i < n; i++)
		{
			th_id = omp_get_thread_num();
			if (th_id == 0)
			{
				if (cb != NULL)(*cb)((3.0 - level)*limit + i * limit / n, "Details extraction...");
			}
			for (int j = 0; j < n; j++)
			{
				int x0 = static_cast<int>(i*deltaW); 
				int y0 = static_cast<int>(j*deltaH); 
				int x1 = static_cast<int>((i + 1)*deltaW); 
				int y1 = static_cast<int>((j + 1)*deltaH);
				x0 = x0 > 0? x0 - 1: x0;
				y0 = y0 > 0? y0 - 1: y0;
				x1 = i != n - 1 ? x1 + 1: x1;
				y1 = j != n - 1 ? y1 + 1: y1;
				int tileW = x1 - x0;
				int tileH = y1 - y0;
				if (level == 3)
					tempLight[zMatrix[j*n + i]] = getBestLight(level, x0, y0, tileW, tileH, mipMapSize[level].width(), defaultSamples); 
				else
				{
					int parentIndex = zMatrix[j*n + i]/4;
					if (!alreadyCal[parentIndex])
					{
						parentLightSample[parentIndex] = getLightSamples(levelLight[2 - level][parentIndex]);
						alreadyCal[parentIndex] = true;
					}
					tempLight[zMatrix[j*n + i]] = getBestLight(level, x0, y0, tileW, tileH, mipMapSize[level].width(), *(parentLightSample[parentIndex])); 
					if (minLevel + 3 - level == maxLevel)
						tilesCenter[zMatrix[j*n + i]] = vcg::Point2f(x0 + tileW/2.0, y0 + tileH/2.0);
				}
			}
		}
		// Applies a global smoothing.
		calcLocalLight(tempLight, levelLight[3 - level], n);
		for (unsigned int i = 0; i < parentLightSample.size(); i++)
			delete parentLightSample[i];
		if (minLevel + 3 - level != maxLevel)
			delete[] zMatrix;
		for (unsigned int i = 0; i < tempLight.size(); i++)
			delete tempLight[i];
	}
	if (maxLevel - minLevel > 3)
	{
		// Extract light directions from the first level of mip-mapping.
		for(int level = 4; level < maxLevel - minLevel + 1; level++)
		{
			int n = 1 << (minLevel + level + 1);
			float deltaW = static_cast<float>(mipMapSize[0].width()) / static_cast<float>(n);
			float deltaH = static_cast<float>(mipMapSize[0].height()) / static_cast<float>(n);
			zMatrix = ZOrder::createZMatrix(minLevel + level + 1);
			int nParent = n*n/4;
			std::vector<std::vector<vcg::Point3f>* > parentLightSample(nParent);
			std::vector<std::vector<vcg::Point3f>* > tempLight(n*n);
			std::vector<bool> alreadyCal(nParent); 
			// Computes the better light vectors for each tile.
			int th_id;
			#pragma omp parallel for private(th_id)
			for (int i = 0; i < n; i++)
			{
				th_id = omp_get_thread_num();
				if (th_id == 0)
				{
					if (cb != NULL)(*cb)(level*limit + static_cast<float>(limit * i / n), "Details extraction...");
				}
				for (int j = 0; j < n; j++)
				{
					int x0 = static_cast<int>(i*deltaW);
					int y0 = static_cast<int>(j*deltaH);
					int x1 = static_cast<int>((i + 1) * deltaW);
					int y1 = static_cast<int>((j + 1) * deltaH);
					x0 = x0 > 0? x0 - 1: x0;
					y0 = y0 > 0? y0 - 1: y0;
					x1 = i != n - 1 ? x1 + 1: x1;
					y1 = j != n - 1 ? y1 + 1: y1;
					int tileW = x1 - x0;
					int tileH = y1 - y0;
					
					int parentIndex = zMatrix[j*n + i]/4;
					if (!alreadyCal[parentIndex])
					{
						parentLightSample[parentIndex] = getLightSamples(levelLight[level - 1][parentIndex]);
						alreadyCal[parentIndex] = true;
					}
					tempLight[zMatrix[j*n + i]] = getBestLight(0, x0, y0, tileW, tileH, mipMapSize[0].width(), *(parentLightSample[parentIndex])); 
					if (minLevel + level == maxLevel)
						tilesCenter[zMatrix[j*n + i]] = vcg::Point2f(x0 + tileW/2.0, y0 + tileH/2.0);
				}
			}
			// Applies a global smoothing.
			calcLocalLight(tempLight, levelLight[level], n);
			for (unsigned int i = 0; i < parentLightSample.size(); i++)
				delete parentLightSample[i];
			if (minLevel + level != maxLevel)
				delete[] zMatrix;
			for (unsigned int i = 0; i < tempLight.size(); i++)
				delete tempLight[i];
		}
	}
	// Applies the final smothing filter.
	calcSmooting(levelLight[maxLevel - minLevel], tilesLight, size);
	// Generate image with the drawing of the light vectors.
	generateVectImage();
	// Creates the detail buffer.
    detailsBuffer = new unsigned char[mipMapSize[0].width()*(mipMapSize[0].height()<<2)];
	if (lrgb)
	{
		const PTMCoefficient* coeffPtr = coefficient->getLevel(0);
		const unsigned char* rgbPtr = color->getLevel(0); 		
		int th_id;
		#pragma omp parallel for private(th_id)
		for (int j = 0; j < mipMapSize[0].height(); j++)
		{
			th_id = omp_get_thread_num();
			if (th_id == 0)
			{
				if (cb != NULL)(*cb)(80.0 + 20.0*j/mipMapSize[0].height(), "Image generation...");
			}
			int offset = j*mipMapSize[0].width();
			for(int i = 0; i < mipMapSize[0].width(); i++)
			{
				vcg::Point3f light = getLight(i, j, mipMapSize[0].width(), mipMapSize[0].height());
				LightMemoized lVec(light.X(), light.Y());
                float lum = coeffPtr[offset].evalPoly(lVec) / 255.0f;
				int offset3 = offset*3;
				int offset4 = offset*4;
				for (int k = 0; k < 3; k++)
					detailsBuffer[offset4 + k] = tobyte(lum*rgbPtr[offset3 + k]);
				detailsBuffer[offset4 + 3] = 255;
				offset++;
			}
		}
	}
	else
	{
		const PTMCoefficient* redPtr = coefficientR->getLevel(0);
		const PTMCoefficient* greenPtr = coefficientG->getLevel(0);
		const PTMCoefficient* bluePtr = coefficientB->getLevel(0); 
		int th_id;
		#pragma omp parallel for schedule(static,CHUNK)
		for (int j = 0; j < mipMapSize[0].height(); j++)
		{
			th_id = omp_get_thread_num();
			if (th_id == 0)
			{
				if (cb != NULL)(*cb)(80.0 + 20.0*j/mipMapSize[0].height(), "Image generation...");
			}
			int offset = j*mipMapSize[0].width();
			for(int i = 0; i < mipMapSize[0].width(); i++)
			{
				vcg::Point3f light = getLight(i, j, mipMapSize[0].width(), mipMapSize[0].height());
				LightMemoized lVec(light.X(), light.Y());
				int offset4 = offset*4;
				detailsBuffer[offset4] = tobyte(redPtr[offset].evalPoly(lVec) );
				detailsBuffer[offset4 + 1] = tobyte(greenPtr[offset].evalPoly(lVec) );
				detailsBuffer[offset4 + 2] = tobyte(bluePtr[offset].evalPoly(lVec) );
				detailsBuffer[offset4 + 3] = 255;
				offset++;
			}
		}
	}
}



void DetailEnhancement::calcLocalLight(std::vector<std::vector<vcg::Point3f>*>& source, std::vector<vcg::Point3f>& dest, int size)
{
	int dist; 
	if (size <= 2)
	{
		for (int ii = 0; ii < size*size; ii++)
			dest[ii] = source[ii]->at(0);
		return;
	}
	// Computes size of the filter.
	switch(size)
	{
		case 4: dist = 1; break;
		case 8: dist = 1; break;
		case 16: dist = 1; break;
		case 32: dist = 2; break;
		case 64: dist = 4; break;
		case 128: dist = 6; break;
		case 256: dist = 8; break;
		default: dist = 8;
	}
	
	// Computes the average of the light vectors of the neighbouring tiles.
	std::vector<vcg::Point3f> avg(size*size);
	std::vector<int> nKernel(size*size);
	
    #pragma omp parallel for
	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			int offset = zMatrix[y * size + x];
			int sx, ex, sy, ey;
			sx = x - dist < 0? 0: x - dist;
			ex = x >= size - dist ? size - 1: x + dist; 
			sy = y - dist < 0? 0: y - dist;
			ey = y >= size - dist? size - 1: y + dist;
			int n = (ex - sx + 1)*(ey - sy + 1);
			nKernel[offset] = n;
			if (x > 0)
			{
				avg[offset] = avg[zMatrix[y*size + x - 1]];
				if (x <= dist)
				{
					for(int jj = sy; jj <= ey; jj++)
						avg[offset] += source[zMatrix[jj*size + x + dist]]->at(0);
				}
				else
				{
					for(int jj = sy; jj <= ey; jj++)
					{
						avg[offset] -= source[zMatrix[jj*size + x - dist - 1]]->at(0);
						if (x + dist < size)
							avg[offset] += source[zMatrix[jj*size + x + dist]]->at(0);
					}
				}
			}
			else
			{
				avg[offset] = vcg::Point3f(0,0,0);
				for (int ii = sx; ii <= ex; ii++)
					for(int jj = sy; jj <= ey; jj++)
						avg[offset] += source[zMatrix[jj*size + ii]]->at(0);
			}
		}
	}
	// Selects for aech tile the light vector nearer to the average vector.
    #pragma omp parallel for
	for (int ii = 0; ii < size*size; ii++)
	{
		avg[ii] /= static_cast<float>(nKernel[ii]);
		avg[ii].Normalize();
		float max = -2;
		int index = 0;
		for (unsigned int jj = 0; jj < source[ii]->size(); jj++)
		{
			float dot = source[ii]->at(jj)*avg[ii];
			if (dot > max)
			{
				max = dot;
				index = jj;
			}
		}
		dest[ii] = source[ii]->at(index);
	}
}


void DetailEnhancement::calcSmooting(std::vector<vcg::Point3f>& source, std::vector<vcg::Point3f>& dest, int size)
{
	int dist = filter/2;
	std::vector<vcg::Point3f> tempLight(size*size);
	std::copy(source.begin(), source.end(), dest.begin());
	std::vector<int> nKernel(size*size);
	

	for (int i = 0; i < nIterSmoothing; i++)
	{
			
        #pragma omp parallel for
		for (int y = 0; y < size; y++)
		{
			for (int x = 0; x < size; x++)
			{
				int offset = zMatrix[y * size + x];
				int sx, ex, sy, ey;
				sx = x - dist < 0? 0: x - dist;
				ex = x >= size - dist ? size - 1: x + dist; 
				sy = y - dist < 0? 0: y - dist;
				ey = y >= size - dist? size - 1: y + dist;
				int n = (ex - sx + 1)*(ey - sy + 1);
				nKernel[offset] = n;
				if (x > 0)
				{
					tempLight[offset] = tempLight[zMatrix[y*size + x - 1]];
					if (x <= dist)
					{
						for(int jj = sy; jj <= ey; jj++)
							tempLight[offset] += dest[zMatrix[jj*size + x + dist]];
					}
					else
					{
						for(int jj = sy; jj <= ey; jj++)
						{
							tempLight[offset] -= dest[zMatrix[jj*size + x - dist - 1]];
							if (x + dist < size)
								tempLight[offset] += dest[zMatrix[jj*size + x + dist]];
						}
					}
				}
				else
				{
					tempLight[offset] = vcg::Point3f(0,0,0);
					for (int ii = sx; ii <= ex; ii++)
						for(int jj = sy; jj <= ey; jj++)
							tempLight[offset] += dest[zMatrix[jj*size + ii]];
				}
			}
		}
        #pragma omp parallel for
		for (int ii = 0; ii < size*size; ii++)
		{
			tempLight[ii] /= static_cast<float>(nKernel[ii]);
			tempLight[ii].Normalize();
		}
		std::copy(tempLight.begin(), tempLight.end(), dest.begin());
	}
}


std::vector<vcg::Point3f>* DetailEnhancement::getBestLight(int level, int x, int y, int tileW, int tileH,  int width, const std::vector<vcg::Point3f>& lightSample)
{
	
	int size = lightSample.size();
    float* gradient = new float[size];
    float* lightness = new float[size];
	int maxGrad = 0;
	int maxL = 0;
	unsigned char rgb[3];
        float max = 0;
	int index = 0;
	std::vector<vcg::Point3f>* vector = new std::vector<vcg::Point3f>();
	for(int k = 0; k < size; k++)
	{
		gradient[k] = 0;
		lightness[k] = 0;
		if (lightSample[k].Z() > 0)
		{
			LightMemoized lVec(lightSample[k].X(),lightSample[k].Y());
			int* image = new int[tileW*tileH];
			int offsetBuf = 0;
			// Creates a image from the tile.
			if (lrgb)
			{
				const PTMCoefficient* coeffPtr = coefficient->getLevel(level);
				const unsigned char* rgbPtr = color->getLevel(level);
					
                for(int j = y; j < y + tileH; j++)
				{
					int offsetBuf = (j-y)*tileW;
					int offset = j*width + x;
					for(int i = x; i < x + tileW; i++)
					{
						float lum = coeffPtr[offset].evalPoly(lVec) / 255.0f;
						rgb[0] = tobyte(lum*rgbPtr[offset*3]);
						rgb[1] = tobyte(lum*rgbPtr[offset*3 + 1]);
						rgb[2] = tobyte(lum*rgbPtr[offset*3 + 2]);
						lightness[k] += 0.299*rgb[0] + 0.587*rgb[1] + 0.114*rgb[2];
						image[offsetBuf] = 0x0 |(rgb[0] << 16) | (rgb[1] << 8) | rgb[2];
						offsetBuf++;
						offset++;
					}
				}
			}
			else
			{
				const PTMCoefficient* redPtr = coefficientR->getLevel(level);
				const PTMCoefficient* greenPtr = coefficientG->getLevel(level);
				const PTMCoefficient* bluePtr = coefficientB->getLevel(level);
               
				for(int j = y; j < y + tileH; j++)
				{
					int offset = j*width + x;
					int offsetBuf = (j-y)*tileW;
					for(int i = x; i < x + tileW; i++)
					{
						rgb[0] = tobyte(redPtr[offset].evalPoly(lVec) );
						rgb[1] = tobyte(greenPtr[offset].evalPoly(lVec) );
						rgb[2] = tobyte(bluePtr[offset].evalPoly(lVec) );
						lightness[k] += 0.299*rgb[0] + 0.587*rgb[1] + 0.114*rgb[2];
						image[offsetBuf] = 0x0 |(rgb[0] << 16) | (rgb[1] << 8) | rgb[2];
						offsetBuf++;
						offset++;
					}
				}

			}
			// Computes the sharpness operator on the image.
			gradient[k] = computeSharpOperator(image, tileW, tileH);
			if (gradient[k] > gradient[maxGrad])
				maxGrad = k;
			if (lightness[k] > lightness[maxL])
				maxL = k;
			delete[] image;
		}
	}
	// Selects the light vector with an enhancement measure greater than a threshold.
    float* value = new float[size];
	for (int k = 0; k < size; k++)
	{
		value[k] = k1*gradient[k]/gradient[maxGrad]+ k2*lightness[k]/lightness[maxL];
		if (value[k] > max)
		{
			max = value[k];
			index = k;
		}
	}
	vector->push_back(lightSample[index]);
	float limit = threshold * value[index];
   	for (int k = 0; k < size; k++)
	{
		if (k != index && value[k] > limit)
			vector->push_back(lightSample[k]);
	}
	delete[] value;
	delete[] gradient;
	delete[] lightness;
	return vector;
}


float DetailEnhancement::computeSharpOperator(int* image, int width, int height)
{
        float gradient = 0;
	if (sharpnessOperator == MAX_LAPLACE || sharpnessOperator == MAX_ENERGY_LAPLACE)
	{
		for(int i = 1; i < width - 1; i++)
		{
			for(int j = 1; j < height - 1; j++)
			{
                float g = 0;
				// Computes Laplace operator.
				g += image[(j-1)*width + i - 1];
				g += 4*image[(j-1)*width + i];
				g += image[(j-1)*width + i + 1];
				g += 4*image[j*width + i - 1];
				g += -20 * image[j*width + i];
				g += 4*image[j*width + i + 1];
				g += image[(j+1)*width + i - 1];
				g += 4*image[(j+1)*width + i ];
				g += image[(j+1)*width + i + 1];
				
				if (sharpnessOperator == MAX_LAPLACE && g > gradient)
					gradient = g;
				else if (sharpnessOperator == MAX_ENERGY_LAPLACE)
					gradient += g*g;
			} 
		}
	}
	else if (sharpnessOperator == NORM_L1_SOBEL || sharpnessOperator == NORM_L2_SOBEL)
	{
		for(int i = 1; i < width - 1; i++)
		{
			for(int j = 1; j < height - 1; j++)
			{
                float gx = 0, gy = 0;

				// Computes Sobel operator.
				gy += 1 * image[(j-1)*width + i - 1];
				gy += 2 * image[(j-1)*width + i];
				gy += 1 * image[(j-1)*width + i + 1];
				gy += -1 *image[(j+1)*width + i - 1];
				gy += -2 * image[(j+1)*width + i ];
				gy += -1 * image[(j+1)*width + i + 1];

				gx += -1 * image[(j-1)*width + i - 1];
				gx += 1 * image[(j-1)*width + i + 1];
				gx += -2 * image[j*width + i - 1];
				gx += 2 * image[j*width + i + 1];
				gx += -1 *image[(j+1)*width + i - 1];
				gx += 1 * image[(j+1)*width + i + 1];
				
				if (sharpnessOperator == NORM_L1_SOBEL)
				{
					gx = vcg::math::Abs(gx);
					gy = vcg::math::Abs(gy);
					gradient += (gx + gy);
				}
				else if (sharpnessOperator == NORM_L2_SOBEL)
				{
					gx *= gx;
					gy *= gy;
					gradient += (gx + gy);
				}
			} 
		}
	}
	return gradient;
}



vcg::Point3f DetailEnhancement::getLight(int x, int y, int width, int height)
{
	int size = 1 << (maxLevel + 1);
	float deltaW = static_cast<float>(width)/static_cast<float>(size);
	float deltaH = static_cast<float>(height)/static_cast<float>(size);
	int ytile = static_cast<int>(y / deltaH);
	int xtile = static_cast<int>(x / deltaW);
	vcg::Point3f l1, l2, l3, l4, final;
	vcg::Point2f v1, v2, v3, v4;
	int id1, id2, id3, id4, id5, id6, id7, id8, id9;
	
	//Computes light vector as linear interpolation of nine neighbouring tiles.

	if (ytile > 0 && ytile < size - 1 && xtile > 0 && xtile < size -1)
	{
		id1 = zMatrix[ytile*size + xtile];
		id2 = zMatrix[(ytile - 1)*size + xtile - 1];
		id3 = zMatrix[(ytile - 1)*size + xtile];
		id4 = zMatrix[(ytile - 1)*size + xtile + 1];
		id5 = zMatrix[ytile*size + xtile - 1];
		id6 = zMatrix[ytile *size + xtile + 1];
		id7 = zMatrix[(ytile + 1)*size + xtile - 1];
		id8 = zMatrix[(ytile + 1)*size + xtile];
		id9 = zMatrix[(ytile + 1)*size + xtile + 1];
		l1 = (tilesLight[id2] + tilesLight[id3] + tilesLight[id5] + tilesLight[id1])/4;
		l1.Normalize();
		l2 = (tilesLight[id3] + tilesLight[id4] + tilesLight[id6] + tilesLight[id1])/4;
		l2.Normalize();
		l3 = (tilesLight[id5] + tilesLight[id1] + tilesLight[id7] + tilesLight[id8])/4;
		l3.Normalize();
		l4 = (tilesLight[id1] + tilesLight[id6] + tilesLight[id8] + tilesLight[id9])/4;
		l4.Normalize();
		v1 = (tilesCenter[id2] + tilesCenter[id3] + tilesCenter[id5] + tilesCenter[id1])/4;
		v2 = (tilesCenter[id3] + tilesCenter[id4] + tilesCenter[id6] + tilesCenter[id1])/4;
		v3 = (tilesCenter[id5] + tilesCenter[id1] + tilesCenter[id7] + tilesCenter[id8])/4;
		v4 = (tilesCenter[id1] + tilesCenter[id6] + tilesCenter[id8] + tilesCenter[id9])/4;
	}
	else if (ytile == 0 && xtile == 0)
	{
		id1 = 0;
		id2 = 1;
		id3 = 2;
		id4 = 3;
		l1 = tilesLight[id1];
		l2 = (tilesLight[id1] + tilesLight[id2])/2;
		l2.Normalize();
		l3 = (tilesLight[id1] + tilesLight[id3])/2;
		l3.Normalize();
		l4 = (tilesLight[id1] + tilesLight[id2] + tilesLight[id3] + tilesLight[id4])/4; 
		l4.Normalize();
		v1 = vcg::Point2f(0,0);
		v2 = vcg::Point2f(deltaW - 1, 0);
		v3 = vcg::Point2f(0, deltaH - 1);
		v4 = vcg::Point2f(deltaW - 1, deltaH - 1);

	}
	else if (ytile == 0 && xtile == size - 1)
	{
		id1 = zMatrix[ytile*size + xtile];
		id2 = id1 - 1;
		id3 = id1 + 1;
		id4 = id1 + 2; 
		l1 = (tilesLight[id1] + tilesLight[id2])/2;
		l1.Normalize();
		l2 = tilesLight[id1];
		l4 = (tilesLight[id1] + tilesLight[id4])/2;
		l4.Normalize();
		l3 = (tilesLight[id1] + tilesLight[id2] + tilesLight[id3] + tilesLight[id4])/4; 
		l3.Normalize();
		v2 = vcg::Point2f(width - 1, 0);
		v1 = v2 - vcg::Point2f(deltaW - 1,0);
		v3 = v2 - vcg::Point2f(deltaW - 1, 1 - deltaH);
		v4 = v2 + vcg::Point2f(0, deltaH - 1);
	}
	else if (ytile == size - 1 && xtile == 0)
	{
		id1 = zMatrix[ytile*size + xtile];
		id2 = id1 - 2;
		id3 = id1 - 1;
		id4 = id1 + 1;
		l3 = tilesLight[id1];
		l1 = (tilesLight[id1] + tilesLight[id2])/2;
		l1.Normalize();
		l4 = (tilesLight[id1] + tilesLight[id4])/2;
		l4.Normalize();
		l3 = (tilesLight[id1] + tilesLight[id2] + tilesLight[id3] + tilesLight[id4])/4; 
		l3.Normalize();
		v3 = vcg::Point2f(0, height - 1);
		v1 = v3 - vcg::Point2f(0, deltaH - 1);
		v2 = v3 - vcg::Point2f(1 - deltaW, deltaH - 1);
		v4 = v4 + vcg::Point2f(deltaW - 1, 0);
	}
	else if (ytile == size - 1 && xtile == size - 1)
	{
		id1 = zMatrix[ytile*size + xtile];
		id2 = id1 - 2;
		id3 = id1 - 3;
		id4 = id1 - 1;
		l4 = tilesLight[id1];
		l2 = (tilesLight[id1] + tilesLight[id2])/2;
		l2.Normalize();
		l3 = (tilesLight[id1] + tilesLight[id4])/2;
		l3.Normalize();
		l1 = (tilesLight[id1] + tilesLight[id2] + tilesLight[id3] + tilesLight[id4])/4; 
		l1.Normalize();
		v4 = vcg::Point2f(width - 1, height - 1);
		v1 = v4 - vcg::Point2f(deltaW - 1, deltaH - 1);
		v2 = v4 - vcg::Point2f(0, deltaH - 1);
		v3 = v4 - vcg::Point2f(deltaW - 1, 0);

	}
	else if (ytile == 0)
	{
		id1 = zMatrix[ytile*size + xtile];
		id2 = zMatrix[ytile*size + xtile - 1];
		id3 = zMatrix[ytile*size + xtile + 1];
		id4 = zMatrix[(ytile + 1)*size + xtile - 1];
		id5 = zMatrix[(ytile + 1)*size + xtile];
		id6 = zMatrix[(ytile + 1)*size + xtile + 1];
		l1 = (tilesLight[id1] + tilesLight[id2])/2;
		l1.Normalize();
		l2 = (tilesLight[id1] + tilesLight[id3])/3;
		l2.Normalize();
		l3 = (tilesLight[id1] + tilesLight[id2] + tilesLight[id4] + tilesLight[id5])/4; 
		l3.Normalize();
		l4 = (tilesLight[id1] + tilesLight[id3] + tilesLight[id5] + tilesLight[id6])/4; 
		l4.Normalize();
		v3 = (tilesCenter[id1] + tilesCenter[id2] + tilesCenter[id4] + tilesCenter[id5])/4;
		v4 = (tilesCenter[id1] + tilesCenter[id3] + tilesCenter[id5] + tilesCenter[id6])/4;
		v1 = v3 - vcg::Point2f(0, deltaH - 1);
		v2 = v4 - vcg::Point2f(0, deltaH - 1);
	}
	else if (ytile == size - 1)
	{
		id1 = zMatrix[ytile*size + xtile];
		id2 = zMatrix[(ytile - 1)*size + xtile - 1];
		id3 = zMatrix[(ytile - 1)*size + xtile];
		id4 = zMatrix[(ytile - 1)*size + xtile + 1];
		id5 = zMatrix[ytile*size + xtile - 1];
		id6 = zMatrix[ytile*size + xtile + 1];
		l3 = (tilesLight[id1] + tilesLight[id5])/2;
		l3.Normalize();
		l4 = (tilesLight[id1] + tilesLight[id6])/3;
		l4.Normalize();
		l1 = (tilesLight[id1] + tilesLight[id2] + tilesLight[id3] + tilesLight[id5])/4; 
		l1.Normalize();
		l2 = (tilesLight[id1] + tilesLight[id3] + tilesLight[id4] + tilesLight[id6])/4; 
		l2.Normalize();
		v1 = (tilesCenter[id1] + tilesCenter[id2] + tilesCenter[id3] + tilesCenter[id5])/4;
		v2 = (tilesCenter[id1] + tilesCenter[id3] + tilesCenter[id4] + tilesCenter[id6])/4;
		v3 = v1 + vcg::Point2f(0, deltaH - 1);
		v4 = v2 + vcg::Point2f(0, deltaH - 1);
	}
	else if (xtile == 0)
	{
		id1 = zMatrix[ytile*size + xtile];
		id2 = zMatrix[(ytile - 1)*size + xtile];
		id3 = zMatrix[(ytile - 1)*size + xtile + 1];
		id4 = zMatrix[ytile*size + xtile + 1];
		id5 = zMatrix[(ytile + 1)*size + xtile + 1];
		id6 = zMatrix[(ytile + 1)*size + xtile];
		l1 = (tilesLight[id1] + tilesLight[id2])/2;
		l1.Normalize();
		l3 = (tilesLight[id1] + tilesLight[id6])/3;
		l3.Normalize();
		l2 = (tilesLight[id1] + tilesLight[id2] + tilesLight[id4] + tilesLight[id3])/4; 
		l2.Normalize();
		l4 = (tilesLight[id1] + tilesLight[id4] + tilesLight[id5] + tilesLight[id6])/4; 
		l4.Normalize();
		v2 = (tilesCenter[id1] + tilesCenter[id2] + tilesCenter[id4] + tilesCenter[id3])/4;
		v4 = (tilesCenter[id1] + tilesCenter[id4] + tilesCenter[id5] + tilesCenter[id6])/4;
		v1 = v2 - vcg::Point2f(deltaW - 1, 0);
		v3 = v4 - vcg::Point2f(deltaW - 1, 0);
	}
	else if (xtile == size - 1)
	{
		id1 = zMatrix[ytile*size + xtile];
		id2 = zMatrix[(ytile - 1)*size + xtile];
		id3 = zMatrix[(ytile - 1)*size + xtile - 1];
		id4 = zMatrix[ytile*size + xtile - 1];
		id5 = zMatrix[(ytile + 1)*size + xtile - 1];
		id6 = zMatrix[(ytile + 1)*size + xtile];
		l2 = (tilesLight[id1] + tilesLight[id2])/2;
		l2.Normalize();
		l4 = (tilesLight[id1] + tilesLight[id6])/3;
		l4.Normalize();
		l1 = (tilesLight[id1] + tilesLight[id2] + tilesLight[id4] + tilesLight[id3])/4; 
		l1.Normalize();
		l3 = (tilesLight[id1] + tilesLight[id4] + tilesLight[id5] + tilesLight[id6])/4; 
		l3.Normalize();
		v1 = (tilesCenter[id1] + tilesCenter[id2] + tilesCenter[id4] + tilesCenter[id3])/4;
		v3 = (tilesCenter[id1] + tilesCenter[id4] + tilesCenter[id5] + tilesCenter[id6])/4;
		v2 = v1 + vcg::Point2f(deltaW - 1, 0);
		v4 = v3 + vcg::Point2f(deltaW - 1, 0);
	}

	float a = vcg::math::Abs(v1.X() - v2.X());
	float x1 = vcg::math::Abs(v1.X() - x);
	float x2 = a - x1;
	float b = vcg::math::Abs(v1.Y() - v3.Y());
	float y1 = vcg::math::Abs(v1.Y() - y);
	float y2 = b - y1;
	vcg::Point3f la, lc;
	la = (l1*x2 + l2*x1)/a;
	la.Normalize();
	lc = (l3*x2 + l4*x1)/a;
	lc.Normalize();
	final = (la*y2 + lc*y1)/b;
	final.Normalize();
	return final;
}


std::vector<vcg::Point3f>* DetailEnhancement::getLightSamples(const vcg::Point3f& base)
{
	int n;
	switch(nOffset)
	{
		case OFFSET_5: n = 1; break;
		case OFFSET_10: n = 2; break;
		case OFFSET_15: n = 3; break;
	}
	std::vector<vcg::Point3f>* sample = new std::vector<vcg::Point3f>(nOffset);
	std::vector<vcg::Point3f>& samplePtr = *sample;
	int offset = 0;
	if (sphereSampl == UNIFORM)
	{
		// Isotropic sampling.
		samplePtr[offset++] = base;
		vcg::Point3f n1, n2;
		if (base == vcg::Point3f(0, 1, 0) || base == vcg::Point3f(0, -1, 0))
			n1 = vcg::Point3f(1, 0, 0);
		else
			n1 = vcg::Point3f(0, 1, 0)^base;
		vcg::Matrix33f matrixAx, matrixAy, matrix;
		matrix.SetRotateDeg(90.0, base);
		n2 = matrix * n1;
		vcg::Point3f Ax1, Ay1, Ax2, Ay2, deltaX1, deltaY1, deltaX2, deltaY2;
		matrixAx.SetRotateDeg(5.0, n1);
		matrixAy.SetRotateDeg(5.0, n2);
		Ax1 = matrixAx*base;
		Ay1 = matrixAy*base;
		samplePtr[offset++] = Ax1;
		samplePtr[offset++] = Ay1;
		deltaX1 = base - Ax1;
		deltaY1 = base - Ay1;
		matrixAx.SetRotateDeg(-5.0, n1);
		matrixAy.SetRotateDeg(-5.0, n2);
		Ax2 = matrixAx*base;
		Ay2 = matrixAy*base;
		samplePtr[offset++] = Ax2;
		samplePtr[offset++] = Ay2;
		deltaX2 = base - Ax2;
		deltaY2 = base - Ay2;
		samplePtr[offset++] = (Ax1 - deltaY1).Normalize();
		samplePtr[offset++] = (Ax1 - deltaY2).Normalize();
		samplePtr[offset++] = (Ax2 - deltaY1).Normalize();
		samplePtr[offset++] = (Ax2 - deltaY2).Normalize();
		if (n > 1)
		{
			vcg::Point3f Ax3, Ay3, Ax4, Ay4, deltaX3, deltaY3, deltaX4, deltaY4;
			matrixAx.SetRotateDeg(10.0, n1);
			matrixAy.SetRotateDeg(10.0, n2);
			Ax3 = matrixAx*base;
			Ay3 = matrixAy*base;
			samplePtr[offset++] = Ax3;
			samplePtr[offset++] = Ay3;
			deltaX3 = base - Ax3;
			deltaY3 = base - Ay3;
			matrixAx.SetRotateDeg(-10.0, n1);
			matrixAy.SetRotateDeg(-10.0, n2);
			Ax4 = matrixAx*base;
			Ay4 = matrixAy*base;
			samplePtr[offset++] = Ax3;
			samplePtr[offset++] = Ay3;
			deltaX4 = base - Ax4;
			deltaY4 = base - Ay4;
			samplePtr[offset++] = (Ax3 - deltaY1).Normalize();
			samplePtr[offset++] = (Ax3 - deltaY2).Normalize();
			samplePtr[offset++] = (Ax3 - deltaY3).Normalize();
			samplePtr[offset++] = (Ax3 - deltaY4).Normalize();
			samplePtr[offset++] = (Ax4 - deltaY1).Normalize();
			samplePtr[offset++] = (Ax4 - deltaY2).Normalize();
			samplePtr[offset++] = (Ax4 - deltaY3).Normalize();
			samplePtr[offset++] = (Ax4 - deltaY4).Normalize();
			samplePtr[offset++] = (Ax1 - deltaY3).Normalize();
			samplePtr[offset++] = (Ax1 - deltaY4).Normalize();
			samplePtr[offset++] = (Ax2 - deltaY3).Normalize();
			samplePtr[offset++] = (Ax2 - deltaY4).Normalize();
			if ( n > 2)
				for (int i = offset; i < nOffset; i++)
					samplePtr[i] == vcg::Point3f(0, 0, -1);
		}
	}
	else if (sphereSampl == NON_UNIFORM)
	{
		// Anisotropic sampling.
		float sinb = sin(vcg::math::ToRad(5.0f*n));
		vcg::Point3f baseDir, baseN, baseTan, baseDirS, baseTanS;
		baseDir = base;
		baseDir.Z() = 0;
		if (vcg::math::Abs(baseDir.X()) < 0.00001 && vcg::math::Abs(baseDir.Y()) < 0.00001)
			baseN = vcg::Point3f(1,0,0);
		else
		{
			baseN = baseDir;
			baseN.Normalize();
		}
		baseTan = vcg::Point3f(baseN.Y(), -baseN.X(), baseN.Z());
		baseDirS = baseN * sinb * base.Z();
		baseTanS = baseTan * sinb;
		
		for (int i = -n; i <= n; i++)
		{
			for (int j = -n; j <= n; j++)
			{
				samplePtr[offset] = baseDir + baseDirS*(static_cast<float>(i)/static_cast<float>(n)) + baseTanS*(static_cast<float>(j)/static_cast<float>(n));
                float x2 = samplePtr[offset].X();
				x2 *= x2;
                float y2 = samplePtr[offset].Y();
				y2 *= y2;

				if (x2 + y2 > 0.999)
					samplePtr[offset].Z() = 0.0;
				else
					samplePtr[offset].Z() = vcg::math::Sqrt(1 - x2  - y2);
					
				samplePtr[offset].Normalize();
				offset++;
			}
		}
	}
	return sample;
}


const vcg::Point3f& DetailEnhancement::getPixelLight(int x, int y)
{
	int n = 1 << (maxLevel + 1);
	float deltaW = static_cast<float>(vectImage->width()) / static_cast<float>(n);
	float deltaH = static_cast<float>(vectImage->height()) / static_cast<float>(n);
	int ytile = static_cast<int>(y / deltaH);
	int xtile = static_cast<int>(x / deltaW);
	return tilesLight[zMatrix[ytile*n + xtile]]; 
}


int DetailEnhancement::getLightVectImagePixel(int offset, int x, int y)
{
	QRgb rgb = vectImage->pixel(x, y);
	int red = qRed(rgb);
	int green = qGreen(rgb);
	int blue = qBlue(rgb);
	int value;
	if (qAlpha(rgb) == 0)
		value = detailsBuffer[offset*4]| detailsBuffer[offset*4 + 1] << 8| detailsBuffer[offset*4 + 2] << 16;
	else
		value = red << 24 | green << 16 | blue << 8 | 0xff;
	return value;
}




void DetailEnhancement::generateVectImage()
{
	int n = 1 << (maxLevel + 1);
	float deltaW = static_cast<float>(vectImage->width()) / static_cast<float>(n);
	float deltaH = static_cast<float>(vectImage->height()) / static_cast<float>(n);
	int length = deltaW < deltaH ? deltaW: deltaH;
	length -= 3;
	QPainter painter(vectImage);
	painter.setRenderHint(QPainter::NonCosmeticDefaultPen);
	QPen pen(QColor(255, 0, 0));
	pen.setWidth(1);
	painter.setPen(pen);
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			vcg::Point3f light = tilesLight[zMatrix[j*n + i]];
			vcg::Point2f center = tilesCenter[zMatrix[j*n + i]];
			int xEnd, yEnd;
			xEnd = center.X() + light.X()*length;
			yEnd = center.Y() - light.Y()*length;
			painter.drawLine(center.X(), center.Y(), xEnd, yEnd);
			painter.drawEllipse(QPoint(center.X(), center.Y()), 1, 1);
		}
	}

}


void DetailEnhancement::updateConfig(OffsetNum o, TileSize size, int level, SharpnessMeasures m, SphereSampling ss, float v1, float v2, float t, SmoothingFilter f, int nIter)
{
	nOffset = o;
	minTileSize = size;
	minLevel = level;
	sharpnessOperator = m;
	sphereSampl = ss;
	k1 = v1;
	k2 = v2;
	threshold = t;
	filter = f; 
	nIterSmoothing = nIter;
	
	bufferReady = false;
	if (detailsBuffer)
		delete[] detailsBuffer;

	emit refreshImage();
}

DetailEnhDlg::DetailEnhDlg(QWidget* parent) 
:	QWidget(parent),
	config(NULL)
{
	advancedBtn = new QPushButton("Advanced Settings", this);
	advancedBtn->setFixedSize(170, 25);
	connect(advancedBtn, SIGNAL(clicked()), this, SLOT(showAdvancedSettings()));

	QGridLayout* layout = new QGridLayout(this);
	layout->addItem(new QSpacerItem(10, 20, QSizePolicy::Fixed), 0, 0, 1, 2);
	layout->addItem(new QSpacerItem(25, 10, QSizePolicy::Minimum, QSizePolicy::Fixed), 1, 0, 1, 1);
	layout->addWidget(advancedBtn, 1, 1, 1, 1);
	setLayout(layout);
}


DetailEnhDlg::~DetailEnhDlg()
{
	if (config != NULL)
		delete config;
}

void DetailEnhDlg::setAdvacedDlg(QDialog* dlg)
{
	config = (DetailConfDlg*)dlg;
}

void DetailEnhDlg::showAdvancedSettings()
{
	if (config != NULL)
		config->show();
}

DetailConfDlg::DetailConfDlg(QWidget *parent) : QDialog(parent)
{
	QLabel* label1 = new QLabel("Local offset");
	QLabel* label2 = new QLabel("Tile size (px)");
	QLabel* label3 = new QLabel("N. initial tiles");
	QLabel* label4 = new QLabel("Sharpness operator");
	QLabel* label5 = new QLabel("Light sampling");
	QLabel* label6 = new QLabel("K1 (Sharpness)(0-1)");
	QLabel* label7 = new QLabel("K2 (Lightness)(0-1)");
	QLabel* label8 = new QLabel("Threshold (0-1)");
	QLabel* label9 = new QLabel("Smoothing filter");
	QLabel* label10 = new QLabel("N. iteration smoothing");

	localOffsetCmb = new QComboBox(this);
	localOffsetCmb->setDuplicatesEnabled(false);
#if _MSC_VER
    localOffsetCmb->addItem("5° (9 samples)", QVariant(OFFSET_5));
    localOffsetCmb->addItem("10° (25 samples)", QVariant(OFFSET_10));
    localOffsetCmb->addItem("15° (49 samples)", QVariant(OFFSET_15));
#elif __MINGW32__
    localOffsetCmb->addItem(trUtf8("5° (9 samples)"), QVariant(OFFSET_5));
    localOffsetCmb->addItem(trUtf8("10° (25 samples)"), QVariant(OFFSET_10));
    localOffsetCmb->addItem(trUtf8("15° (49 samples)"), QVariant(OFFSET_15));
#endif
	tilesSizeCmb = new QComboBox(this);
	tilesSizeCmb->setDuplicatesEnabled(false);
	tilesSizeCmb->addItem("8", QVariant(8));
	tilesSizeCmb->addItem("16", QVariant(16));
	tilesSizeCmb->addItem("32", QVariant(32));

	minLevelCmb = new QComboBox(this);
	minLevelCmb->setDuplicatesEnabled(false);
	minLevelCmb->addItem("4", QVariant(0));
	minLevelCmb->addItem("16", QVariant(1));
	minLevelCmb->addItem("64", QVariant(2));

	sharpOperatorCmb = new QComboBox(this);
	sharpOperatorCmb->setDuplicatesEnabled(false);
	sharpOperatorCmb->addItem("Max Laplacian", QVariant(MAX_LAPLACE));
	sharpOperatorCmb->addItem("Max Energy of Laplacian", QVariant(MAX_ENERGY_LAPLACE));
	sharpOperatorCmb->addItem("L1 norm Sobel", QVariant(NORM_L1_SOBEL));
	sharpOperatorCmb->addItem("L2 norm Sobel", QVariant(NORM_L2_SOBEL));
	
	sphereSamplCmb = new QComboBox(this);
	sphereSamplCmb->setDuplicatesEnabled(false);
	sphereSamplCmb->addItem("Isotropic", QVariant(UNIFORM));
	sphereSamplCmb->addItem("Anisotropic", QVariant(NON_UNIFORM));

        k1Snb = new QDoubleSpinBox(this);
	k1Snb->setRange(0.0, 1.0);
	k1Snb->setDecimals(2);
	k1Snb->setSingleStep(0.01);
	k1Snb->setValue(0.7);
        connect(k1Snb, SIGNAL(valueChanged(double)), this, SLOT(k1Changed(double)));
	
        k2Snb = new QDoubleSpinBox(this);
	k2Snb->setRange(0.0, 1.0);
	k2Snb->setDecimals(2);
	k2Snb->setSingleStep(0.01);
	k2Snb->setValue(0.3);
        connect(k2Snb, SIGNAL(valueChanged(double)), this, SLOT(k2Changed(double)));
	
        thresholdSnb = new QDoubleSpinBox(this);
	thresholdSnb->setRange(0.0, 1.0);
	thresholdSnb->setDecimals(2);
	thresholdSnb->setSingleStep(0.01);
	thresholdSnb->setValue(0.7);
		
	filterCmb = new QComboBox(this);
	filterCmb->setDuplicatesEnabled(false);
	filterCmb->addItem("3x3", QVariant(FILTER_3x3));
	filterCmb->addItem("5x5", QVariant(FILTER_5x5));
	filterCmb->addItem("7x7", QVariant(FILTER_7x7));

	nIterCmb = new QSpinBox(this);
	nIterCmb->setRange(0, 10);
	nIterCmb->setValue(2);
	
	okBtn = new QPushButton("OK");
	okBtn->setDefault(true);
	cancelBtn = new QPushButton("Cancel");
	cancelBtn->setAutoDefault(false);

	buttonBox = new QDialogButtonBox;
	buttonBox->addButton(okBtn, QDialogButtonBox::ActionRole);
	buttonBox->addButton(cancelBtn, QDialogButtonBox::RejectRole);

	connect(cancelBtn, SIGNAL(clicked()), this, SLOT(close()));
	connect(okBtn, SIGNAL(clicked()), this, SLOT(okPressed()));

	QGridLayout* layout = new QGridLayout;
	layout->addWidget(label1, 0, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(label2, 1, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(label3, 2, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(label4, 3, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(label5, 4, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(label6, 5, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(label7, 6, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(label8, 7, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(label9, 8, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(label10, 9, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(localOffsetCmb, 0, 1, 1, 1);
	layout->addWidget(tilesSizeCmb, 1, 1, 1, 1);
	layout->addWidget(minLevelCmb, 2, 1, 1, 1);
	layout->addWidget(sharpOperatorCmb, 3, 1, 1, 1);
	layout->addWidget(sphereSamplCmb, 4, 1, 1, 1);
	layout->addWidget(k1Snb, 5, 1, 1, 1);
	layout->addWidget(k2Snb, 6, 1, 1, 1);
	layout->addWidget(thresholdSnb, 7, 1, 1, 1);
	layout->addWidget(filterCmb, 8, 1, 1, 1);
	layout->addWidget(nIterCmb, 9, 1, 1, 1);
	layout->addWidget(buttonBox, 10, 1, 1, 1);
	setLayout(layout);
	
	setModal(true);
	setWindowFlags(Qt::Tool);
	setWindowTitle(tr("Configure Static Multi Light"));
}


OffsetNum DetailEnhancement::getNOffset()
{
    return nOffset;
}

TileSize DetailEnhancement::getMinTileSize()
{
    return minTileSize;
}

int DetailEnhancement::getMinLevel()
{
    return minLevel;
}

SharpnessMeasures DetailEnhancement::getSharpnessOperator()
{
    return sharpnessOperator;
}

SphereSampling DetailEnhancement::getSphereSampling()
{
    return sphereSampl;
}

float DetailEnhancement::getK1()
{
    return k1;
}

float DetailEnhancement::getK2()
{
    return k2;
}

float DetailEnhancement::getThreshold()
{
    return threshold;
}

SmoothingFilter DetailEnhancement::getFilter()
{
    return filter;
}

int DetailEnhancement::getNIterSmoothing()
{
    return nIterSmoothing;
}

void DetailConfDlg::setCurrentValue(OffsetNum o, TileSize size, int level, SharpnessMeasures m, SphereSampling ss, float v1, float v2, float t, SmoothingFilter f, int nIter)
{
	nOffset = o;
	minTileSize = size;
	minLevel = level;
	sharpnessOperator = m;
	sphereSampl = ss;
	k1 = v1;
	k2 = v2;
	threshold = t;
	filter = f;
	nIterSmoothing = nIter;
	localOffsetCmb->setCurrentIndex(localOffsetCmb->findData(QVariant(nOffset)));
	tilesSizeCmb->setCurrentIndex(tilesSizeCmb->findData(QVariant(minTileSize)));
	minLevelCmb->setCurrentIndex(minLevelCmb->findData(QVariant(minLevel)));
	sharpOperatorCmb->setCurrentIndex(sharpOperatorCmb->findData(QVariant(sharpnessOperator)));
	sphereSamplCmb->setCurrentIndex(sphereSamplCmb->findData(QVariant(sphereSampl)));
	k1Snb->setValue(k1);
	k2Snb->setValue(k2);
	thresholdSnb->setValue(threshold);
	nIterCmb->setValue(nIterSmoothing);
	filterCmb->setCurrentIndex(filterCmb->findData(QVariant(filter)));
}


void DetailConfDlg::okPressed()
{
	int a = localOffsetCmb->itemData(localOffsetCmb->currentIndex()).toInt();
	int b = tilesSizeCmb->itemData(tilesSizeCmb->currentIndex()).toInt();
	int c = minLevelCmb->itemData(minLevelCmb->currentIndex()).toInt();
	int d = sharpOperatorCmb->itemData(sharpOperatorCmb->currentIndex()).toInt();
	int e = sphereSamplCmb->itemData(sphereSamplCmb->currentIndex()).toInt();
	float f = k1Snb->value();
	float g = k2Snb->value();
	float h = thresholdSnb->value();
	int i = filterCmb->itemData(filterCmb->currentIndex()).toInt();
	int l = nIterCmb->value();
	if (nOffset == a && minTileSize == b && minLevel == c && sharpnessOperator == d && sphereSampl == e
		&& k1 == f && k2 == g && threshold == h && filter == i && nIterSmoothing == l)
	{
		close();
		return;
	}
	nOffset = (OffsetNum) a;
	minTileSize = (TileSize) b;
	minLevel = c;
	sharpnessOperator = (SharpnessMeasures) d;
	sphereSampl = (SphereSampling) e;
	k1 = f;
	k2 = g; 
	threshold = h;
	filter = (SmoothingFilter) i;
	nIterSmoothing = l;
	close();
	emit configUpdated(static_cast<OffsetNum>(a), static_cast<TileSize>(b), c, static_cast<SharpnessMeasures>(d),
						static_cast<SphereSampling>(e), f, g, h, static_cast<SmoothingFilter>(i), l);
}


void DetailConfDlg::k1Changed(double k)
{
	k2Snb->setValue(1- k);
}


void DetailConfDlg::k2Changed(double k)
{
	k1Snb->setValue(1-k);
}




