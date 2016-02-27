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
#include <QLabel>
#include <QGridLayout>
#include <QPainter>

#include "dyndetailenhanc.h"

#include "../../rtiwebmaker/src/zorder.h"


DynamicDetailEControl::DynamicDetailEControl(unsigned int tileSize, unsigned int offset, QWidget *parent) 
:	QWidget(parent),
	config(NULL)
{
	QLabel* label1 = new QLabel("Tile size (px)");

#if _MSC_VER
	QLabel* label2 = new QLabel("Offset (1-20°)");
#elif __MINGW32__
    QLabel* label2 = new QLabel(trUtf8("Offset (1-20°)"));
#else
    QLabel* label2 = new QLabel("Offset (1-20°)");
#endif

	tileSizeCmb = new QComboBox(this);
	tileSizeCmb->setDuplicatesEnabled(false);
	int v = 8;
	for (int i = 1; v*i <= MAX_TILE_SIZE; i++)
		tileSizeCmb->addItem(tr("%1").arg(v*i), QVariant(v*i));
	tileSize /= 8;
	tileSize *= 8;
	tileSizeCmb->setCurrentIndex(tileSizeCmb->findData(QVariant(tileSize)));
	connect(tileSizeCmb, SIGNAL(activated(int)), this, SLOT(changeTileSize(int)));

	offsetCmb = new QSpinBox(this);
	offsetCmb->setRange(MIN_OFFSET, MAX_OFFSET);
#if _MSC_VER
	offsetCmb->setSuffix("°");
#elif __MINGW32__
    offsetCmb->setSuffix(trUtf8("°"));
#endif
	offsetCmb->setValue(offset);
	offsetCmb->setKeyboardTracking(false);
	connect(offsetCmb, SIGNAL(valueChanged(int)), this, SIGNAL(offsetChanged(int)));

	advancedBtn = new QPushButton("Advanced Settings", this);
	//advancedBtn->setFixedSize(170, 20);
	advancedBtn->setFixedSize(130, 23);
	connect(advancedBtn, SIGNAL(clicked()), this, SLOT(showAdvancedSettings()));
	
	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(label1, 0, 0, 1, 1);
	layout->addWidget(label2, 1, 0, 1, 1);
	layout->addWidget(tileSizeCmb, 0, 1, 1, 1);
	layout->addWidget(offsetCmb, 1, 1, 1, 1);
	layout->addWidget(advancedBtn, 2, 1, 1, 1);
	layout->setContentsMargins(5, 5, 5, 0);
	setLayout(layout);
}

DynamicDetailEControl::~DynamicDetailEControl()
{
	if (config != NULL)
		delete config;
}

void DynamicDetailEControl::setAdvacedDlg(QDialog *dlg)
{
	config = (DynamicDetConfDlg*) dlg;
}

void DynamicDetailEControl::changeTileSize(int s)
{
    int value = tileSizeCmb->itemData(s).toInt();
    emit tileSizeChanged(value);
}

void DynamicDetailEControl::setTileSize(int s)
{
    tileSizeCmb->setCurrentIndex(tileSizeCmb->findData(QVariant(s)));
}

void DynamicDetailEControl::setOffset(int o)
{
    offsetCmb->setValue(o);
}

void DynamicDetailEControl::showAdvancedSettings()
{
	if (config != NULL)
		config->show();
}

DynamicDetailEnh::DynamicDetailEnh():
	degreeOffset(10),
	tileSize(32),
	sharpnessOp(DYN_MAX_ENERGY_LAPLACE),
	sphereSampl(DYN_NON_UNIFORM),
	k1(0.7f),
	k2(0.3f),
	threshold(0.7f),
	filter(DYN_3x3),
	nIterFilter(2)
{

}

DynamicDetailEnh::~DynamicDetailEnh()
{

}


QString DynamicDetailEnh::getTitle() 
{
	return "Dynamic Multi Light";
}


QWidget* DynamicDetailEnh::getControl(QWidget* parent)
{
	DynamicDetConfDlg* advancedControl = new DynamicDetConfDlg(parent);
	advancedControl->setCurrentValue(sharpnessOp, sphereSampl, k1, k2, threshold, filter, nIterFilter);
	DynamicDetailEControl* control = new DynamicDetailEControl(tileSize, degreeOffset, parent);
	control->setAdvacedDlg(advancedControl);
	connect(control, SIGNAL(tileSizeChanged(int)), this, SLOT(setTileSize(int)));
    connect(this, SIGNAL(tileSizeChanged(int)), control, SLOT(setTileSize(int)));
    connect(control, SIGNAL(offsetChanged(int)), this, SLOT(setOffset(int)));
    connect(this, SIGNAL(offsetChanged(int)), control, SLOT(setOffset(int)));
    connect(advancedControl, SIGNAL(configUpdated(SharpnessMeasuresDyn, SphereSamplingDyn, float, float, float, SmoothingFilterDyn, int)),
			this, SLOT(updateConfig(SharpnessMeasuresDyn, SphereSamplingDyn, float, float, float, SmoothingFilterDyn, int)));
	disconnect(this, SIGNAL(refreshImage()), 0, 0);
	connect(this, SIGNAL(refreshImage()), parent, SIGNAL(updateImage()));
	return control;
}


bool DynamicDetailEnh::isLightInteractive()
{
	return true;
}


bool DynamicDetailEnh::supportRemoteView()
{
	return false;
}


bool DynamicDetailEnh::enabledLighting()
{
	return true;
}


void DynamicDetailEnh::applyPtmLRGB(const PyramidCoeff& coeff, const PyramidRGB& rgb, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
	int m;
	switch(info.mode)
	{
		case LIGHT_VECTOR: m = 1; break;
		case LIGHT_VECTOR2: m = 2; break;
		default: m = 0;
	}
	
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	bufferPtr = buffer;
	coefficient = coeff.getLevel(info.level);
	color = rgb.getLevel(info.level);
	w = info.width;
	h = info.height;
	lrgb = true;
	drawingMode = m; 
	calcDetails(info, mipMapSize[info.level].width());
	QApplication::restoreOverrideCursor();
}


void DynamicDetailEnh::applyPtmRGB(const PyramidCoeff& redCoeff, const PyramidCoeff& greenCoeff, const PyramidCoeff& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
	int m;
	switch(info.mode)
	{
		case LIGHT_VECTOR: m = 1; break;
		case LIGHT_VECTOR2: m = 2; break;
		default: m = 0;
	}
	
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	bufferPtr = buffer;
	red = redCoeff.getLevel(info.level);
	green = greenCoeff.getLevel(info.level);
	blue = blueCoeff.getLevel(info.level);
	w = info.width;
	h = info.height;
	lrgb = false;
	drawingMode = m; 
	calcDetails(info, mipMapSize[info.level].width());
	QApplication::restoreOverrideCursor();
	
}


void DynamicDetailEnh::calcDetails(RenderingInfo info, int levelWidth)
{
	int deltaW, deltaH, ni, nj;
	// Computes the size of the grid of tiles.
	if (tileSize == 0)
	{
		ni = 1;
		nj = 1;
		deltaW = info.width;
		deltaH = info.height;
	}
	else
	{ 
		ni = ceil(1.0 * info.width / (tileSize + 1));
		nj = ceil(1.0 * info.height / (tileSize + 1));
		deltaW = tileSize + 1;
		deltaH = tileSize + 1;
	}
	std::vector<vcg::Point3f>* samples = getLightSamples(info.light);
	lights.empty();
	lights = std::vector<vcg::Point3f>(ni*nj);
	std::vector<std::vector<vcg::Point3f>*> tempLight(ni*nj);
    std::vector<int> pointerIndex(ni*nj);
	// Selects the better light vectors for each tile.
	#pragma omp parallel for
	for (int j = 0; j < nj; j++)
	{
		for (int i = 0; i < ni; i++)
		{
			int x0 = i*deltaW;
			int y0 = j*deltaH;
            if (y0 + deltaH >= info.height)
                pointerIndex[j*ni + i] = (j-1)*ni +i;
            else if (x0 + deltaW >= info.width)
                pointerIndex[j*ni + i] = j*ni + i - 1;
            else
            {
                pointerIndex[j*ni + i] = -1;
				tempLight[j*ni + i] = getBestLight(x0 + info.offx , y0 + info.offy, deltaW, deltaH, levelWidth, *samples);
            }
		}
	}
    for (int i = 0; i < tempLight.size(); i++)
    {
        if (pointerIndex[i] != -1)
            tempLight[i] = tempLight[pointerIndex[i]];
    }
    // Computes a global smoothing.
	calcLocalLight(tempLight, lights, ni, nj);
	// Applies the final smothing filter.
	calcSmoothing(lights, ni, nj);
	QImage vectImage;
	if (drawingMode != 0)
	{
		// Draws the light vectors selected for each tile.
		vectImage = QImage(info.width, info.height,  QImage::Format_ARGB32);
		int value = qRgba(255, 255, 255, 0);
		vectImage.fill(value);
		QPainter painter(&vectImage);
		painter.setRenderHint(QPainter::NonCosmeticDefaultPen);
		QPen pen(QColor(255, 0, 0));
		pen.setWidth(1);
		painter.setPen(pen);
		if (drawingMode == 1)
		{
            for (int j = 0; j < nj - 1; j++)
			{
                for (int i = 0; i < ni - 1; i++)
				{
					vcg::Point3f light = lights[j*ni + i];
					vcg::Point2f center = vcg::Point2f(i*deltaW + deltaW/2, j*deltaH + deltaH/2);
					int xEnd, yEnd;
					xEnd = center.X() + tileSize*light.X();
					yEnd = center.Y() - tileSize*light.Y();
					painter.drawLine(center.X(), center.Y(), xEnd, yEnd);
					painter.drawEllipse(QPoint(center.X(), center.Y()), 1, 1);
				}
			}
		}
		else if (drawingMode == 2)
		{
			for(unsigned int i = 0; i < 9; i++)
			{
				vcg::Point2f center = vcg::Point2f(info.width/2, info.height/2);
				int xEnd, yEnd;
				xEnd = center.X() + (info.width/2)*samples->at(i).X();
				yEnd = center.Y() - (info.height/2)*samples->at(i).Y();
				painter.drawLine(center.X(), center.Y(), xEnd, yEnd);
				painter.drawEllipse(QPoint(center.X(), center.Y()), 1, 1);
			}

		}
	}
	// Creates the output texture.
	if (lrgb)
	{
		if (drawingMode == 0)
		{
			#pragma omp parallel for schedule(static,CHUNK)
			for (int j = 0; j < info.height ; j++)
			{
				int offset2 = (j + info.offy)*levelWidth + info.offx;
				int offset = j*info.width;
				for(int i = 0; i < info.width; i++)
				{
					vcg::Point3f l = getLight(i, j, deltaW, deltaH, ni, nj);
					float lum = coefficient[offset2].evalPoly(l.X(), l.Y()) / 255.0;
					for (int k = 0; k < 3; k++)
							bufferPtr[offset*4 + k] = tobyte(lum*color[offset2*3 +k]);
					bufferPtr[offset*4 + 3] = 255;
					offset2++;
					offset++;
				}
			}
		}
		else
		{
			#pragma omp parallel for schedule(static,CHUNK)
			for (int j = 0; j < info.height ; j++)
			{
				int offset2 = (j + info.offy)*levelWidth + info.offx;
				int offset = j*info.width;
				for(int i = 0; i < info.width; i++)
				{
					vcg::Point3f l = getLight(i, j, deltaW, deltaH, ni, nj);
					float lum = coefficient[offset2].evalPoly(l.X(), l.Y()) / 255.0;
					QRgb rgb = vectImage.pixel(i, j);
					if (qAlpha(rgb) != 0)
					{
						int red = qRed(rgb);
						int green = qGreen(rgb);
						int blue = qBlue(rgb);
						bufferPtr[offset*4] = red;
						bufferPtr[offset*4 + 1] = green;
						bufferPtr[offset*4 + 2] = blue;
					}
					else
					{
						for (int k = 0; k < 3; k++)
							bufferPtr[offset*4 + k] = tobyte(lum*color[offset2*3 +k]);
					}
					bufferPtr[offset*4 + 3] = 255;
					offset2++;
					offset++;
				}
			}
		}
	}
	else
	{
        if (drawingMode == 0)
		{
			#pragma omp parallel for schedule(static,CHUNK)
			for (int j = 0; j < info.height ; j++)
			{
				int offset2 = (j + info.offy)*levelWidth + info.offx;
				int offset = j*info.width;
				for(int i = 0; i < info.width; i++)
				{
					vcg::Point3f l = getLight(i, j, deltaW, deltaH, ni, nj);
					int offset4 = offset*4;
					bufferPtr[offset4] = tobyte(red[offset2].evalPoly(l.X(), l.Y()));
					bufferPtr[offset4 + 1] = tobyte(green[offset2].evalPoly(l.X(), l.Y()));
					bufferPtr[offset4 + 2] = tobyte(blue[offset2].evalPoly(l.X(), l.Y()));
					bufferPtr[offset4 + 3] = 255;
					offset2++;
					offset++;
					
				}
			}
		}
		else
		{
			#pragma omp parallel for schedule(static,CHUNK)
			for (int j = 0; j < info.height ; j++)
			{
				int offset2 = (j + info.offy)*levelWidth + info.offx;
				int offset = j*info.width;
				for(int i = 0; i < info.width; i++)
				{
					vcg::Point3f l = getLight(i, j, deltaW, deltaH, ni, nj);
					QRgb rgb = vectImage.pixel(i, j);
					if (qAlpha(rgb) != 0)
					{
						int red = qRed(rgb);
						int green = qGreen(rgb);
						int blue = qBlue(rgb);
						bufferPtr[offset*4] = red;
						bufferPtr[offset*4 + 1] = green;
						bufferPtr[offset*4 + 2] = blue;
					}
					else
					{
						bufferPtr[offset*4] = tobyte(red[offset2].evalPoly(l.X(), l.Y()));
						bufferPtr[offset*4 + 1] = tobyte(green[offset2].evalPoly(l.X(), l.Y()));
						bufferPtr[offset*4 + 2] = tobyte(blue[offset2].evalPoly(l.X(), l.Y()));
					}
					bufferPtr[offset*4 + 3] = 255;
					offset2++;
					offset++;
					
				}
			}
		}
	}
	delete samples;
	for (int j = 0; j < nj - 1; j++)
		for (int i = 0; i < ni - 1; i++)
			delete tempLight[j*ni + i];
}


std::vector<vcg::Point3f>* DynamicDetailEnh::getLightSamples(const vcg::Point3f& base)
{
	std::vector<vcg::Point3f>* coneDirVec = new std::vector<vcg::Point3f>;
	if (sphereSampl == DYN_NON_UNIFORM)
	{
		// Anisotropic sampling.
		float sinb = sin(vcg::math::ToRad(static_cast<float>(degreeOffset)));
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
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				vcg::Point3f temp = baseDir + baseDirS*i + baseTanS*j;
                float x2 = temp.X();
				x2 *= x2;
                float y2 = temp.Y();
				y2 *= y2;
				if (x2 + y2 > 0.999)
					temp.Z() = 0.0;
				else
					temp.Z() = vcg::math::Sqrt(1 - x2  - y2);
				temp.Normalize();
				coneDirVec->push_back(temp);
			}
		}
	}
	else if (sphereSampl == DYN_UNIFORM)
	{
		// Isotropic sampling.
		vcg::Point3f n1, n2;
		if (base == vcg::Point3f(0, 1, 0) || base == vcg::Point3f(0, -1, 0))
			n1 = vcg::Point3f(1, 0, 0);
		else
			n1 = vcg::Point3f(0, 1, 0)^base;
		vcg::Matrix33f matrixAx, matrixAy, matrix;
		matrix.SetRotateDeg(90.0, base);
		n2 = matrix * n1;
		vcg::Point3f Ax, Ay, deltaX1, deltaY1, deltaX2, deltaY2;
		matrixAx.SetRotateDeg(degreeOffset, n1);
		matrixAy.SetRotateDeg(degreeOffset, n2);
		Ax = matrixAx*base;
		Ay = matrixAy*base;
		coneDirVec->push_back(Ax.Normalize());
		coneDirVec->push_back(Ay.Normalize());
		deltaX1 = base - Ax;
		deltaY1 = base - Ay;
		matrixAx.SetRotateDeg(-degreeOffset, n1);
		matrixAy.SetRotateDeg(-degreeOffset, n2);
		Ax = matrixAx*base;
		Ay = matrixAy*base;
		coneDirVec->push_back(Ax.Normalize());
		coneDirVec->push_back(Ay.Normalize());
		deltaX2 = base - Ax;
		deltaY2 = base - Ay;
		vcg::Point3f t1, t2;
		float k = static_cast<float>(M_SQRT2/2);
		coneDirVec->push_back((base - deltaX1*k - deltaY1*k).Normalize());
		coneDirVec->push_back((base - deltaX1*k - deltaY2*k).Normalize());
		coneDirVec->push_back((base - deltaX2*k - deltaY1*k).Normalize());
		coneDirVec->push_back((base - deltaX2*k - deltaY2*k).Normalize());
		coneDirVec->push_back(base);
	}
	return coneDirVec;
}


std::vector<vcg::Point3f>* DynamicDetailEnh::getBestLight(int x, int y, int tileW, int tileH, int width, const std::vector<vcg::Point3f>& lightSamples)
{
    float gradient[9];
    float lightness[9];
	int maxGrad = 0;
	int maxL = 0;

    float max = 0;
	int index = 0;
	std::vector<vcg::Point3f>* vector = new std::vector<vcg::Point3f>();
	for(int k = 0; k < 9; k++)
	{
		gradient[k] = 0;
		lightness[k] = 0;
		if (lightSamples[k].Z() != -1)
		{
			int* image = new int[tileW*tileH];
			int offsetBuf = 0;
			// Creates a image from the tile.
            unsigned char rgb[3];
            LightMemoized lVec(lightSamples[k].X(), lightSamples[k].Y());

			if (lrgb)
			{
                for(int j = y; j < y + tileH; j++)
				{

					float LValue = lightness[k];
					int offsetBuf = (j-y)*tileW;
					int *img; img = (image+offsetBuf);
					int offset = j*width + x;
					for(int i = x; i < x + tileW; i++)
					{
						float lum = coefficient[offset].evalPoly(lVec);
                        unsigned char r = tobyte(lum * color[offset*3]);
                        unsigned char g = tobyte(lum * color[offset*3 + 1]);
                        unsigned char b = tobyte(lum * color[offset*3 + 1]);
                        LValue += 0.299f*r + 0.587f*g + 0.114f*b;
                        img[i-x] = 0x0 |(r << 16) | (g << 8) | b;
						offsetBuf++;
						offset++;
					}
                    lightness[k] += LValue;
				}
			}
			else
			{
                
				for(int j = y; j < y + tileH; j++)
				{
                    int offsetBuf = (j-y)*tileW;
					int offset = j*width + x;
					for(int i = x; i < x + tileW; i++)
					{
						rgb[0] = tobyte(red[offset].evalPoly(lVec));
						rgb[1] = tobyte(green[offset].evalPoly(lVec));
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
    float value[9];
	for (int k = 0; k < 9; k++)
	{
		value[k] = k1*gradient[k]/gradient[maxGrad]+ k2*lightness[k]/lightness[maxL];
		if (value[k] > max)
		{
			max = value[k];
			index = k;
		}
	}
	vector->push_back(lightSamples[index]);
    float limit = threshold*value[index];
	for (int k = 0; k < 9; k++)
	{
		if (k != index && value[k] > limit)
			vector->push_back(lightSamples[k]);
	}
	return vector;
}


float DynamicDetailEnh::computeSharpOperator(int* image, int width, int height)
{
    float gradient = 0;
	if (sharpnessOp == DYN_MAX_LAPLACE || sharpnessOp == DYN_MAX_ENERGY_LAPLACE)
	{
		for(int i = 1; i < width - 1; i+=2)
		{
			for(int j = 1; j < height - 1; j+=2)
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
				
				if (sharpnessOp == DYN_MAX_LAPLACE && g > gradient)
					gradient = g;
				else if (sharpnessOp == DYN_MAX_ENERGY_LAPLACE)
					gradient += g*g;
			} 
		}
	}
	else if (sharpnessOp == DYN_NORM_L1_SOBEL || sharpnessOp == DYN_NORM_L2_SOBEL)
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
				
				if (sharpnessOp == DYN_NORM_L1_SOBEL)
				{
					gx = vcg::math::Abs(gx);
					gy = vcg::math::Abs(gy);
					gradient += (gx + gy);
				}
				else if (sharpnessOp == DYN_NORM_L2_SOBEL)
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


vcg::Point3f DynamicDetailEnh::getLight(int x, int y, int width, int height, int nx, int ny)
{
	int ytile = y / height;
	int xtile = x / width;
	
	vcg::Point3f l1, l2, l3, l4, final;
	vcg::Point2f v1, v2, v3, v4;
	int id1, id2, id3, id4, id5, id6, id7, id8, id9;
	
	//Computes light vector as linear interpolation of nine neighbouring tiles.

	if (ytile > 0 && ytile < ny - 1 && xtile > 0 && xtile < nx -1)
	{
		id1 = ytile*nx + xtile;
		id3 = id1 - nx;
		id2 = id3 - 1;
		id4 = id3 + 1;
		id5 = id1 - 1;
		id6 = id1 + 1;
		id8 = id1 + nx;
		id7 = id8 - 1;
		id9 = id8 + 1;
		l1 = (lights[id2] + lights[id3] + lights[id5] + lights[id1])/4;
		l2 = (lights[id3] + lights[id4] + lights[id6] + lights[id1])/4;
		l3 = (lights[id5] + lights[id1] + lights[id7] + lights[id8])/4;
		l4 = (lights[id1] + lights[id6] + lights[id8] + lights[id9])/4;
		v1 = vcg::Point2f(width*xtile, height*ytile);
		v2 = v1 + vcg::Point2f(width, 0);
		v3 = v1 + vcg::Point2f(0, height);
		v4 = v1 + vcg::Point2f(width, height);
	}
	else if (ytile == 0 && xtile == 0)
	{
		id1 = 0;
		id2 = 1;
		id3 = nx;
		id4 = nx + 1;
		l1 = lights[id1];
		l2 = (lights[id1] + lights[id2])/2;
		l3 = (lights[id1] + lights[id3])/2;
		l4 = (lights[id1] + lights[id2] + lights[id3] + lights[id4])/4; 
		v1 = vcg::Point2f(0, 0);
		v2 = vcg::Point2f(width, 0);
		v3 = vcg::Point2f(0, height);
		v4 = vcg::Point2f(width, height);
	}
	else if (ytile == 0 && xtile == nx - 1)
	{
		id1 = ytile*nx + xtile;
		id2 = id1 - 1;
		id4 = id1 + nx; 
		id3 = id4 - 1;
		l1 = (lights[id1] + lights[id2])/2;
		l2 = lights[id1];
		l4 = (lights[id1] + lights[id4])/2;
		l3 = (lights[id1] + lights[id2] + lights[id3] + lights[id4])/4; 
		v2 = vcg::Point2f(w - 1, 0);
		v1 = vcg::Point2f(width*xtile,0);
		v3 = v1 + vcg::Point2f(0, height);
		v4 = v2 + vcg::Point2f(0, height);
	}
	else if (ytile == ny - 1 && xtile == 0)
	{
		id1 = ytile*nx + xtile;
		id2 = id1 - nx;
		id3 = id2 + 1;
		id4 = id1 + 1;
		l3 = lights[id1];
		l1 = (lights[id1] + lights[id2])/2;
		l4 = (lights[id1] + lights[id4])/2;
        l2 = (lights[id1] + lights[id2] + lights[id3] + lights[id4])/4;
		v3 = vcg::Point2f(0, h - 1);
		v1 = vcg::Point2f(0, height*ytile);
		v2 = v1 + vcg::Point2f(width, 0);
		v4 = v3 + vcg::Point2f(width, 0);
	}
	else if (ytile == ny - 1 && xtile == nx - 1)
	{
		id1 = ytile*nx + xtile;
		id2 = id1 - nx;
		id3 = id2 - 1;
		id4 = id1 - 1;
		l4 = lights[id1];
		l2 = (lights[id1] + lights[id2])/2;
		l3 = (lights[id1] + lights[id4])/2;
		l1 = (lights[id1] + lights[id2] + lights[id3] + lights[id4])/4; 
		v4 = vcg::Point2f(w - 1, h - 1);
		v1 = vcg::Point2f(width*xtile, height*ytile);
		v2 = vcg::Point2f(w - 1, height*ytile);
		v3 = vcg::Point2f(width*xtile, h - 1);
	}
	else if (ytile == 0)
	{
		id1 = ytile*nx + xtile;
		id2 = id1 - 1;
		id3 = id1 + 1;
		id5 = id1 + nx;
		id4 = id5 - 1;
		id6 = id5 + 1;
		l1 = (lights[id1] + lights[id2])/2;
		l2 = (lights[id1] + lights[id3])/3;
		l3 = (lights[id1] + lights[id2] + lights[id4] + lights[id5])/4; 
		l4 = (lights[id1] + lights[id3] + lights[id5] + lights[id6])/4; 
		v1 = vcg::Point2f(width*xtile, 0);
		v2 = v1 + vcg::Point2f(width, 0);
		v3 = v1 + vcg::Point2f(0, height);
		v4 = v1 + vcg::Point2f(width, height);
	}
	else if (ytile == ny - 1)
	{
		id1 = ytile*nx + xtile;
		id3 = id1 - nx;
		id2 = id3 - 1;
		id4 = id3 + 1;
		id5 = id1 - 1;
		id6 = id1 + 1;
		l3 = (lights[id1] + lights[id5])/2;
		l4 = (lights[id1] + lights[id6])/3;
		l1 = (lights[id1] + lights[id2] + lights[id3] + lights[id5])/4; 
		l2 = (lights[id1] + lights[id3] + lights[id4] + lights[id6])/4; 
		v1 = vcg::Point2f(width*xtile, height*ytile);
		v2 = v1 + vcg::Point2f(width, 0);
		v3 = vcg::Point2f(width*xtile, w - 1);
		v4 = v3 + vcg::Point2f(width, 0);
	}
	else if (xtile == 0)
	{
		id1 = ytile*nx + xtile;
		id2 = id1 - nx;
		id3 = id2 + 1;
		id4 = id1 + 1;
		id6 = id1 + nx;
		id5 = id6 + 1;
		l1 = (lights[id1] + lights[id2])/2;
		l3 = (lights[id1] + lights[id6])/3;
		l2 = (lights[id1] + lights[id2] + lights[id4] + lights[id3])/4; 
		l4 = (lights[id1] + lights[id4] + lights[id5] + lights[id6])/4; 
		v1 = vcg::Point2f(0, height*ytile);
		v2 = v1 + vcg::Point2f(width, 0);
		v3 = v1 + vcg::Point2f(0, height);
		v4 = v1 + vcg::Point2f(width, height);
	}
	else if (xtile == nx - 1)
	{
		id1 = ytile*nx + xtile;
		id2 = id1 - nx;
		id3 = id2 - 1;
		id4 = id1 - 1;
		id6 = id1 + nx;
		id5 = id6 - 1;
		l2 = (lights[id1] + lights[id2])/2;
		l4 = (lights[id1] + lights[id6])/3;
		l1 = (lights[id1] + lights[id2] + lights[id4] + lights[id3])/4; 
		l3 = (lights[id1] + lights[id4] + lights[id5] + lights[id6])/4; 
		v1 = vcg::Point2f(width*xtile, height*ytile);
		v3 = v1 + vcg::Point2f(0, height);
		v2 = vcg::Point2f(w - 1, height*ytile);
		v4 = v2 + vcg::Point2f(0, height);
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


void DynamicDetailEnh::calcLocalLight(std::vector<std::vector<vcg::Point3f>*>& source, std::vector<vcg::Point3f>& dest, int nx, int ny)
{
	// Sets the size of the filter.
	int dist;
	switch(tileSize)
	{
		case 32: dist = 1; break;
		case 24: dist = 1; break;
		case 16: dist = 2; break;
		case 8: dist = 3;
	}

	// Computes the average of the light vectors of the neighbouring tiles.
	std::vector<vcg::Point3f> avg(nx*ny);
	std::vector<int> nKernel(nx*ny);
	#pragma omp parallel for
	for (int y = 0; y < ny; y++)
	{
		for (int x = 0; x < nx; x++)
		{
			int offset = y * nx + x;
			int sx, ex, sy, ey;
			sx = x - dist < 0? 0: x - dist;
			ex = x >= nx - dist ? nx - 1: x + dist; 
			sy = y - dist < 0? 0: y - dist;
			ey = y >= ny - dist? ny - 1: y + dist;
			int n = (ex - sx + 1)*(ey - sy + 1);
			nKernel[offset] = n;
			if (x > 0)
			{
				avg[offset] = avg[y*nx + x - 1];
				if (x <= dist)
				{
					for(int jj = sy; jj <= ey; jj++)
						avg[offset] += source[jj*nx + x + dist]->at(0);
				}
				else
				{
					for(int jj = sy; jj <= ey; jj++)
					{
						avg[offset] -= source[jj*nx + x - dist - 1]->at(0);
						if (x + dist < nx)
							avg[offset] += source[jj*nx + x + dist]->at(0);
					}
				}
			}
			else
			{
				avg[offset] = vcg::Point3f(0,0,0);
                for (int ii = sx; ii <= ex; ii++)
                    for(int jj = sy; jj <= ey; jj++)
						avg[offset] += source[jj*nx + ii]->at(0);
			}
		}
	}
	// Selects for aech tile the light vector nearer to the average vector.
	#pragma omp parallel for
	for (int ii = 0; ii < nx*ny; ii++)
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


void DynamicDetailEnh::calcSmoothing(std::vector<vcg::Point3f>& light, int nx, int ny)
{
	int dist = filter/2;
	std::vector<vcg::Point3f> tempLight(nx*ny);
	std::copy(light.begin(), light.end(), tempLight.begin());
	std::vector<int> nKernel(nx*ny);
	for (int i = 0; i < nIterFilter; i++)
	{
		#pragma omp parallel for
		for (int y = 0; y < ny; y++)
		{
			for (int x = 0; x < nx; x++)
			{
				int offset = y * nx + x;
				int sx, ex, sy, ey;
				sx = x - dist < 0? 0: x - dist;
				ex = x >= nx - dist ? nx - 1: x + dist; 
				sy = y - dist < 0? 0: y - dist;
				ey = y >= ny - dist? ny - 1: y + dist;
				int n = (ex - sx + 1)*(ey - sy + 1);
				nKernel[offset] = n;
				if (x > 0)
				{
					tempLight[offset] = tempLight[y*nx + x - 1];
					if (x <= dist)
					{
						for(int jj = sy; jj <= ey; jj++)
							tempLight[offset] += light[jj*nx + x + dist];
					}
					else
					{
						for(int jj = sy; jj <= ey; jj++)
						{
							tempLight[offset] -= light[jj*nx + x - dist - 1];
							if (x + dist < nx)
								tempLight[offset] += light[jj*nx + x + dist];
						}
					}
				}
				else
				{
					tempLight[offset] = vcg::Point3f(0,0,0);
					for (int ii = sx; ii <= ex; ii++)
						for(int jj = sy; jj <= ey; jj++)
							tempLight[offset] += light[jj*nx + ii];
				}
			}
		}
		#pragma omp parallel for
		for (int ii = 0; ii < nx*ny; ii++)
		{
			tempLight[ii] /= static_cast<float>(nKernel[ii]);
			tempLight[ii].Normalize();
		}
		std::copy(tempLight.begin(), tempLight.end(), light.begin());
	}
}


void DynamicDetailEnh::setOffset(int x)
{
	degreeOffset = x;
    emit offsetChanged(x);
	emit refreshImage();
}




void DynamicDetailEnh::setTileSize(int s)
{
	tileSize = s;
    emit tileSizeChanged(s);
	emit refreshImage();
}


void DynamicDetailEnh::updateConfig(SharpnessMeasuresDyn m, SphereSamplingDyn ss, float v1, float v2, float t, SmoothingFilterDyn f, int nIter)
{
	sharpnessOp = m;
	sphereSampl = ss;
	k1 = v1;
	k2 = v2;
	threshold = t;
	filter = f;
	nIterFilter = nIter;
	emit refreshImage();
}


DynamicDetConfDlg::DynamicDetConfDlg(QWidget *parent) : QDialog(parent)
{
	QLabel* label1 = new QLabel("Sharpness operator");
	QLabel* label2 = new QLabel("Light sampling");
	QLabel* label3 = new QLabel("K1 (Sharpness) (0-1)");
	QLabel* label4 = new QLabel("K2 (Lightness) (0-1)");
	QLabel* label5 = new QLabel("Threshold (0-1)");
	QLabel* label6 = new QLabel("Smoothing filter");
	QLabel* label7 = new QLabel("N. iteration smoothing");

	sharpnessOpCmb = new QComboBox(this);
	sharpnessOpCmb->setDuplicatesEnabled(false);
	sharpnessOpCmb->addItem("Max Laplacian", QVariant(DYN_MAX_LAPLACE));
	sharpnessOpCmb->addItem("Max Energy of Laplacian", QVariant(DYN_MAX_ENERGY_LAPLACE));
	sharpnessOpCmb->addItem("L1 norm Sobel", QVariant(DYN_NORM_L1_SOBEL));
	sharpnessOpCmb->addItem("L2 norm Sobel", QVariant(DYN_NORM_L2_SOBEL));
	
	sphereSamplCmb = new QComboBox(this);
	sphereSamplCmb->setDuplicatesEnabled(false);
	sphereSamplCmb->addItem("Isotropic", QVariant(DYN_UNIFORM));
	sphereSamplCmb->addItem("Anisotropic", QVariant(DYN_NON_UNIFORM));

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
	
	smootingFilterCmb = new QComboBox(this);
	smootingFilterCmb->setDuplicatesEnabled(false);
	smootingFilterCmb->addItem("3x3", QVariant(DYN_3x3));
	smootingFilterCmb->addItem("5x5", QVariant(DYN_5x5));
	smootingFilterCmb->addItem("7x7", QVariant(DYN_7x7));

	iterSmoothingSnb = new QSpinBox(this);
	iterSmoothingSnb->setRange(0, 10);
	iterSmoothingSnb->setValue(2);
	
	applyBtn = new QPushButton("Apply");
	applyBtn->setDefault(true);
	exitBtn = new QPushButton("Exit");
	exitBtn->setAutoDefault(false);

	buttonBox = new QDialogButtonBox;
	buttonBox->addButton(applyBtn, QDialogButtonBox::ActionRole);
	buttonBox->addButton(exitBtn, QDialogButtonBox::RejectRole);

	connect(exitBtn, SIGNAL(clicked()), this, SLOT(close()));
	connect(applyBtn, SIGNAL(clicked()), this, SLOT(applyPressed()));

	QGridLayout* layout = new QGridLayout;
	layout->addWidget(label1, 0, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(label2, 1, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(label3, 2, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(label4, 3, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(label5, 4, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(label6, 5, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(label7, 6, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(sharpnessOpCmb, 0, 1, 1, 1);
	layout->addWidget(sphereSamplCmb, 1, 1, 1, 1);
	layout->addWidget(k1Snb, 2, 1, 1, 1);
	layout->addWidget(k2Snb, 3, 1, 1, 1);
	layout->addWidget(thresholdSnb, 4, 1, 1, 1);
	layout->addWidget(smootingFilterCmb, 5, 1, 1, 1);
	layout->addWidget(iterSmoothingSnb, 6, 1, 1, 1);
	layout->addWidget(buttonBox, 7, 1, 1, 1);
	setLayout(layout);
	
	setModal(false);
	setWindowFlags(Qt::Tool);
	setWindowTitle(tr("Configure Dynamic Multi Light"));
}


int DynamicDetailEnh::getDegreeOffset()
{
    return degreeOffset;
}

int DynamicDetailEnh::getTileSize()
{
    return tileSize;
}

SharpnessMeasuresDyn DynamicDetailEnh::getSharpnessOperator()
{
    return sharpnessOp;
}

SphereSamplingDyn DynamicDetailEnh::getSphereSampling()
{
    return sphereSampl;
}

float DynamicDetailEnh::getK1()
{
    return k1;
}

float DynamicDetailEnh::getK2()
{
    return k2;
}

float DynamicDetailEnh::getThreshold()
{
    return threshold;
}

SmoothingFilterDyn DynamicDetailEnh::getFilter()
{
    return filter;
}

int DynamicDetailEnh::getNIterSmoothing()
{
    return nIterFilter;
}

void DynamicDetConfDlg::setCurrentValue(SharpnessMeasuresDyn sharpnessOp, SphereSamplingDyn sphereSampl, float k1, float k2, float threshold, SmoothingFilterDyn smoothFilter, int nIter)
{
	sharpnessOpCmb->setCurrentIndex(sharpnessOpCmb->findData(QVariant(sharpnessOp)));
	sphereSamplCmb->setCurrentIndex(sphereSamplCmb->findData(QVariant(sphereSampl)));
	smootingFilterCmb->setCurrentIndex(smootingFilterCmb->findData(QVariant(smoothFilter)));
	k1Snb->setValue(k1);
	k2Snb->setValue(k2);
	thresholdSnb->setValue(threshold);
	iterSmoothingSnb->setValue(nIter);
}


void DynamicDetConfDlg::applyPressed()
{
	SharpnessMeasuresDyn sharpOp = static_cast<SharpnessMeasuresDyn>(sharpnessOpCmb->itemData(sharpnessOpCmb->currentIndex()).toInt());
	SphereSamplingDyn sphereSampl = static_cast<SphereSamplingDyn>(sphereSamplCmb->itemData(sphereSamplCmb->currentIndex()).toInt());
	float k1 = k1Snb->value();
	float k2 = k2Snb->value();
	float threshold = thresholdSnb->value();
	SmoothingFilterDyn filter = static_cast<SmoothingFilterDyn>(smootingFilterCmb->itemData(smootingFilterCmb->currentIndex()).toInt());
	int iterSmooth = iterSmoothingSnb->value();
	emit configUpdated(sharpOp, sphereSampl, k1, k2, threshold, filter, iterSmooth);
}


void DynamicDetConfDlg::k1Changed(double value)
{
	k2Snb->setValue(1 - value);
}


void DynamicDetConfDlg::k2Changed(double value)
{
	k1Snb->setValue(1 - value);
}


