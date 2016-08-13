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

#include "diffusegain.h"
#include <omp.h>
#include <QString>
#include <QMessageBox>

const float d256 = 1.0f/256.0f;

DiffuseGControl::DiffuseGControl(int gain, QWidget *parent) : QWidget(parent)
{
    groups.append(new RenderControlGroup(this, "Gain", gain));
    connect(groups.at(0)->spinBox, SIGNAL(valueChanged(int)), this, SIGNAL(gainChanged(int)));
    setLayout(createLayout());
}

bool DiffuseGControl::eventFilter(QObject* watched, QEvent* event)
{
    int s = getSliderIndex(watched, event);
    if (s != -1)
       emit (gainChanged(groups.at(s)->slider->value()));
    return false;
}

//DiffuseGain::DiffuseGain() :
//	gain(2.0f),
//	minGain(1.0f),
//    maxGain(10.0f)
//	{	 }

// YY
DiffuseGain::DiffuseGain() :
	minGain(1.0f),
    maxGain(10.0f)
	{	 }
float DiffuseGain::gain = 2.0f;


DiffuseGain::~DiffuseGain() {}

QString DiffuseGain::getTitle() 
{
	return "Diffuse Gain";
}

QWidget* DiffuseGain::getControl(QWidget* parent)
{
    int initValue = roundParam((gain - minGain)*100/(maxGain - minGain));
	DiffuseGControl* control = new DiffuseGControl(initValue, parent);

	control = new DiffuseGControl(initValue, parent);
    connect(control, SIGNAL(gainChanged(int)), this, SLOT(setGain(int)));

    disconnect(this, SIGNAL(refreshImage()), 0, 0);
	connect(this, SIGNAL(refreshImage()), parent, SIGNAL(updateImage()));
	return control;
}


bool DiffuseGain::isLightInteractive()
{
	return true;
}


bool DiffuseGain::supportRemoteView()
{
	return true;
}


bool DiffuseGain::enabledLighting()
{
	return true;
}


float DiffuseGain::getGain()
{
    // Get the gain as a value normalized to the range [0,100]

    return ((gain - minGain)*100.0/(maxGain - minGain));
}

void DiffuseGain::setGain(int value)
{
	gain = minGain + value * (maxGain - minGain)/100;
	emit refreshImage();
}

void DiffuseGain::applyPtmLRGB(const PyramidCoeff& coeff, const PyramidRGB& rgb, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
	int offsetBuf = 0;
	const unsigned char* rgbPtr = rgb.getLevel(info.level);
	const PTMCoefficient* coeffPtr = coeff.getLevel(info.level);
	const vcg::Point3f* normalsPtr = normals.getLevel(info.level);
	// Creates the output texture.

    #pragma omp parallel for schedule(static,CHUNK)
	for (int y = info.offy; y < info.offy + info.height; y++)
	{
        int offsetBuf = ((y-info.offy)*info.width)<<2;
		int offset = y * mipMapSize[info.level].width() + info.offx;
		for (int x = info.offx; x < info.offx + info.width; x++)
		{
			//int offset = y * mipMapSize[info.level].width() + x;
            float lum = applyModel(&(coeffPtr[offset][0]), normalsPtr[offset].X(), normalsPtr[offset].Y(), info.light.X(), info.light.Y()) / 256.0f;
			int offset3 = offset*3;
			for (int i = 0; i < 3; i++)
				buffer[offsetBuf + i] = tobyte(rgbPtr[offset3 + i] * lum);
            buffer[offsetBuf + 3] = 255;
			offsetBuf += 4;
			offset++;
		}
	}
	//qDebug() << "gain = " << DiffuseGain::gain;
}


void DiffuseGain::applyPtmRGB(const PyramidCoeff& redCoeff, const PyramidCoeff& greenCoeff, const PyramidCoeff& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
	int offsetBuf = 0;
	const PTMCoefficient* redPtr = redCoeff.getLevel(info.level);
	const PTMCoefficient* greenPtr = greenCoeff.getLevel(info.level);
	const PTMCoefficient* bluePtr = blueCoeff.getLevel(info.level);
	const vcg::Point3f* normalsPtr = normals.getLevel(info.level);
	// Creates the output texture.
	
    #pragma omp parallel for schedule(static,CHUNK)
	for (int y = info.offy; y < info.offy + info.height; y++)
	{
        int offsetBuf = ((y-info.offy)*info.width)<<2;
		int offset = y * mipMapSize[info.level].width() + info.offx;
		for (int x = info.offx; x < info.offx + info.width; x++)
		{
			buffer[offsetBuf + 0] = tobyte(applyModel(&(redPtr[offset][0]), normalsPtr[offset].X(), normalsPtr[offset].Y(), info.light.X(), info.light.Y()));
			buffer[offsetBuf + 1] = tobyte(applyModel(&(greenPtr[offset][0]), normalsPtr[offset].X(), normalsPtr[offset].Y(), info.light.X(), info.light.Y()));
			buffer[offsetBuf + 2] = tobyte(applyModel(&(bluePtr[offset][0]), normalsPtr[offset].X(), normalsPtr[offset].Y(), info.light.X(), info.light.Y()));
            buffer[offsetBuf + 3] = 255;
			offset++;
			offsetBuf += 4;
		}
	}
	//qDebug() << "gain = " << DiffuseGain::gain;
}


float DiffuseGain::applyModel(const int* a, float nu, float nv, float lu, float lv)
{
    float a0 = gain * a[0];
    float a1 = gain * a[1];
    float a2 = gain * a[2];
    float a3t =  ((a[0]<<1)*nu + a[2]*nv);
    float a3 = (1.0f - gain) * a3t + a[3];
    float a4t = ((a[1]<<1)*nv + a[2]*nu);
    float a4 = (1.0f - gain) * a4t + a[4];
    float a5 = (1.0f - gain) * (a[0]*nu*nu + a[1]*nv*nv + a[2]*nu*nv) + (a[3] - a3) * nu
			+ (a[4] - a4) * nv + a[5];
	return a0*lu*lu + a1*lv*lv + a2*lu*lv + a3*lu + a4*lv + a5; 
}
