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

#include "specularenhanc.h"

#include <omp.h>

SpecularEControl::SpecularEControl(int kd, int ks, int exp, int minExp, int maxExp, QWidget *parent) : QWidget(parent)
{
    groups.append(new RenderControlGroup(this, "Diffuse Color", kd));
    connect(groups.at(0)->spinBox, SIGNAL(valueChanged(int)), this, SIGNAL(kdChanged(int)));
    groups.append(new RenderControlGroup(this, "Specularity", ks));
    connect(groups.at(1)->spinBox, SIGNAL(valueChanged(int)), this, SIGNAL(ksChanged(int)));
    groups.append(new RenderControlGroup(this, "Highlight Size", exp, minExp, maxExp));
    connect(groups.at(2)->spinBox, SIGNAL(valueChanged(int)), this, SIGNAL(expChanged(int)));
    setLayout(createLayout());
}

bool SpecularEControl::eventFilter(QObject* watched, QEvent* event)
{
    int s;
    if ((s = getSliderIndex(watched, event)) != -1)
        if (s == 0)
            emit (kdChanged(groups.at(s)->slider->value()));
        else if (s == 1)
            emit (ksChanged(groups.at(s)->slider->value()));
        else
            emit (expChanged(groups.at(s)->slider->value()));
    return false;
}

//SpecularEnhancement::SpecularEnhancement() :
//kd(0.4f),
//ks(0.7f),
//exp(75),
//minKd(0.0f),
//maxKd(1.0f),
//minKs(0.0f),
//maxKs(1.0f),
//minExp(1),
//maxExp(150)
//{	}

// YY
SpecularEnhancement::SpecularEnhancement() :
minKd(0.0f),
maxKd(1.0f),
minKs(0.0f),
maxKs(1.0f),
minExp(1),
maxExp(150)
{	}
float SpecularEnhancement::kd  = 0.4f;
float SpecularEnhancement::ks  = 0.7f;
int SpecularEnhancement::exp = 75;

SpecularEnhancement::~SpecularEnhancement() {}



QString SpecularEnhancement::getTitle() 
{
	return "Specular Enhancement";
}

QWidget* SpecularEnhancement::getControl(QWidget* parent)
{
    int initKd = roundParam((kd - minKd)*100/(maxKd - minKd));
    int initKs = roundParam((ks - minKs)*100/(maxKs - minKs));
    int initExp = exp;
//    int initExp = roundParam((exp - minExp)*100.0/(maxExp - minExp));
    SpecularEControl* control = new SpecularEControl(initKd, initKs, initExp, minExp, maxExp, parent);
	connect(control, SIGNAL(kdChanged(int)), this, SLOT(setKd(int)));
	connect(control, SIGNAL(ksChanged(int)), this, SLOT(setKs(int)));
	connect(control, SIGNAL(expChanged(int)), this, SLOT(setExp(int)));
	disconnect(this, SIGNAL(refreshImage()), 0, 0);
	connect(this, SIGNAL(refreshImage()), parent, SIGNAL(updateImage()));
	return control;
}


bool SpecularEnhancement::isLightInteractive()
{
	return true;
}

bool SpecularEnhancement::supportRemoteView()
{
	return false;
}


bool SpecularEnhancement::enabledLighting()
{
	return true;
}


float SpecularEnhancement::getKd()
{
    // Get kd as a value normalized to the range [0,100]

    return (kd - minKd)*100.0/(maxKd - minKd);
}

void SpecularEnhancement::setKd(int value)
{
	kd = minKd + value * (maxKd - minKd)/100;
	emit refreshImage();
}

float SpecularEnhancement::getKs()
{
    // Get ks as a value normalized to the range [0,100]

    return (ks - minKs)*100.0/(maxKs - minKs);
}

void SpecularEnhancement::setKs(int value)
{
	ks = minKs + value * (maxKs - minKs)/100;
	emit refreshImage();
}

float SpecularEnhancement::getExp()
{
    return exp;
/*
    // Get exp as a value normalized to the range [0,100]

    return (exp - minExp)*100.0/(maxExp - minExp);
*/
}

void SpecularEnhancement::setExp(int value)
{
    exp = value;
//    exp = minExp + value * (maxExp - minExp)/100;
	emit refreshImage();
}

void SpecularEnhancement::applyPtmLRGB(const PyramidCoeff& coeff, const PyramidRGB& rgb, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
	// Creates the output texture.
	int offsetBuf = 0;
	const PTMCoefficient* coeffPtr = coeff.getLevel(info.level);
	const unsigned char* rgbPtr = rgb.getLevel(info.level);
	const vcg::Point3f* normalsPtr = normals.getLevel(info.level);
	LightMemoized lVec(info.light.X(), info.light.Y());
	
	#pragma omp parallel for schedule(static,CHUNK) 
	for (int y = info.offy; y < info.offy + info.height; y++)
	{
		int offsetBuf = (y-info.offy)*info.width*4;
		int offset = y * mipMapSize[info.level].width() + info.offx;
		for (int x = info.offx; x < info.offx + info.width; x++)
		{
			float lum = coeffPtr[offset].evalPoly(lVec) / 255.0f;
            vcg::Point3f h(0, 0, 1);
			h += info.light;
			h /= 2;
			h.Normalize();
			float nDotH = h * normalsPtr[offset];
			if (nDotH < 0) 
				nDotH = 0.0;
			else if (nDotH > 1)
				nDotH = 1.0;
			nDotH = pow(nDotH, exp);
			nDotH *= ks*255;
			int offset3 = offset*3;
			for (int i = 0; i < 3; i++)
				buffer[offsetBuf + i]  = tobyte((rgbPtr[offset3 + i]*kd + nDotH)*lum);
			buffer[offsetBuf + 3] = 255;
			offsetBuf += 4;
			offset++;
		}
	}

}


void SpecularEnhancement::applyPtmRGB(const PyramidCoeff& redCoeff, const PyramidCoeff& greenCoeff, const PyramidCoeff& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
	// Creates the output texture.
	const PTMCoefficient* redPtr = redCoeff.getLevel(info.level);
	const PTMCoefficient* greenPtr = greenCoeff.getLevel(info.level);
	const PTMCoefficient* bluePtr = blueCoeff.getLevel(info.level);
	const vcg::Point3f* normalsPtr = normals.getLevel(info.level);
	LightMemoized lVec(info.light.X(), info.light.Y());
	
	#pragma omp parallel for schedule(static,CHUNK)
	for (int y = info.offy; y < info.offy + info.height; y++)
	{
		int offsetBuf = (y-info.offy)*info.width<<2;
		int offset = y * mipMapSize[info.level].width() + info.offx;
		for (int x = info.offx; x < info.offx + info.width; x++)
		{
			vcg::Point3f h(0, 0, 1);
			h += info.light;
			h /= 2;
			h.Normalize();
            float nDotH = h * normalsPtr[offset];
			if (nDotH < 0) 
				nDotH = 0.0;
			else if (nDotH > 1)
				nDotH = 1.0;
			nDotH = pow(nDotH, exp);
			float r = redPtr[offset].evalPoly(lVec);
			float g = greenPtr[offset].evalPoly(lVec);
            float b = bluePtr[offset].evalPoly(lVec);
			float temp = (r + g + b)/3;
            float lum =  temp * ks * 2 * nDotH;
			buffer[offsetBuf + 0] = tobyte( r * kd + lum);
			buffer[offsetBuf + 1] = tobyte( g * kd + lum );
			buffer[offsetBuf + 2] = tobyte( b * kd + lum );
			buffer[offsetBuf + 3] = 255;
			offsetBuf += 4;
			offset++;
		}
	}
}


void SpecularEnhancement::applyHSH(const PyramidCoeffF& redCoeff, const PyramidCoeffF& greenCoeff, const PyramidCoeffF& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
	const float* redPtr = redCoeff.getLevel(info.level);
	const float* greenPtr = greenCoeff.getLevel(info.level);
	const float* bluePtr = blueCoeff.getLevel(info.level);
	const vcg::Point3f* normalsPtr = normals.getLevel(info.level);
	int tempW = mipMapSize[info.level].width();
    float hweights[9];
	vcg::Point3d temp(info.light.X(), info.light.Y(), info.light.Z());
	temp.Normalize();
	float phi = atan2(temp.Y(), temp.X());
	if (phi<0) 
		phi = 2*M_PI+phi;
    float theta = qMin<float>(acos(temp.Z()/temp.Norm()), M_PI / 2 - 0.04);

	int offsetBuf = 0;
	getHSH(theta, phi, hweights, sqrt((float)info.ordlen));
	
	#pragma omp parallel for schedule(static,CHUNK) 
	for (int y = info.offy; y < info.offy + info.height; y++)
	{
		int offsetBuf = (y-info.offy)*info.width*4;
		int offset= y * tempW + info.offx;
		for (int x = info.offx; x < info.offx + info.width; x++)
		{
            float red = 0, green = 0, blue = 0;
			int offset2 = offset * info.ordlen;
			for (int k = 0; k < info.ordlen; k++)
			{
				int offset3 = offset2 + k;
				red += redPtr[offset3] * hweights[k];
				green += greenPtr[offset3] * hweights[k];
				blue += bluePtr[offset3] * hweights[k];
			}
			red *= 256;
			green *= 256;
			blue *= 256;

            vcg::Point3f h(0.0f, 0.0f, 1.0f);
			h += info.light;
            h /= 2.0f;
			h.Normalize();

            float nDotH = h * normalsPtr[offset];
			if (nDotH < 0) 
				nDotH = 0.0;
            else if (nDotH > 1.0f)
				nDotH = 1.0;
            nDotH = pow(nDotH, exp/5.0f);

            float temp = (red + green + blue)/3;
            float lum =  temp * ks * 4.0f * nDotH;
			buffer[offsetBuf + 0] = tobyte( red * kd + lum);
			buffer[offsetBuf + 1] = tobyte( green * kd + lum );
			buffer[offsetBuf + 2] = tobyte( blue * kd + lum );
            buffer[offsetBuf + 3] = 0xff;
			offsetBuf += 4;
			offset++;
		}
	}


}
