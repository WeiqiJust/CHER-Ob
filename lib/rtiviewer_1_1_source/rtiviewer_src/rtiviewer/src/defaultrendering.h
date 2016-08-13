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


#ifndef DEFAULT_REND_H
#define DEFAULT_REND_H

#include "renderingmode.h"

#include <QTimer>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include <omp.h>



/*!
Widget to show the progress of the downloading of a remote RTI
*/
class LoadRemoteWidget : public QWidget
{
	Q_OBJECT
private:

	QLabel* string;
	int i;

public:

	LoadRemoteWidget(bool remote, QWidget* parent = 0) : QWidget(parent)
	{
		i = 0;
		if (remote)
		{

			QVBoxLayout* layout = new QVBoxLayout;
			string = new QLabel("Downloading remote RTI ");
			layout->addWidget(string, 0, Qt::AlignVCenter);
			setLayout(layout);
			startTimer(500);
		}
	}

protected:

	void timerEvent(QTimerEvent * event)
	{
		i++;
		i = i % 10;
		QString point = "";
		for (int j = 0; j < i; j++)
			point.append(".");
		string->setText(tr("Downloading remote RTI ").append(point));
	}
};



//! Defaut Rendering for RTI images.
/*!
The class defines the default rendering for RTI images.
*/
class DefaultRendering : public QObject, public RenderingMode
{
	Q_OBJECT

private:
	bool remote;

public:

	DefaultRendering(): remote(false){}
	void setRemote(bool flag) {remote = flag;}

	QString getTitle() {return "Default";}

	QWidget* getControl(QWidget* parent)
	{
		LoadRemoteWidget* control = new LoadRemoteWidget(remote, parent);
		disconnect(parent, SIGNAL(resetRemote()), 0, 0);
		connect(parent, SIGNAL(resetRemote()), this, SLOT(resetRemote())); 
		return control;
	}

	bool isLightInteractive() {return true;}
	bool supportRemoteView()  {return true;}
	bool enabledLighting() {return true;}

	void applyPtmLRGB(const PyramidCoeff& coeff, const PyramidRGB& rgb, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
	{
		//int offsetBuf = 0;
		const PTMCoefficient* coeffPtr = coeff.getLevel(info.level);
		const unsigned char* rgbPtr = rgb.getLevel(info.level);
		int tempW = mipMapSize[info.level].width();

		LightMemoized lVec(info.light.X(), info.light.Y());
		
		#pragma omp parallel for schedule(static,CHUNK)
		for (int y = info.offy; y < info.offy + info.height; y++)
		{
            int offsetBuf = ((y-info.offy)*info.width) << 2;
			int offset= (y * tempW + info.offx)*3;
			for (int x = info.offx; x < info.offx + info.width; x++)
			{
				buffer[offsetBuf + 3] = 255;
                float lum = coeffPtr[offset / 3].evalPoly(lVec) / 255.0f;
                float b[4];
                for (int i = 0; i < 3; i++)
                    buffer[offsetBuf + i] = tobyte(rgbPtr[offset + i] * lum);
              	offsetBuf += 4;
				offset += 3;
			}
		}
	}


	void applyPtmRGB(const PyramidCoeff& redCoeff, const PyramidCoeff& greenCoeff, const PyramidCoeff& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
	{
		//int offsetBuf = 0;
		const PTMCoefficient* redPtr = redCoeff.getLevel(info.level);
		const PTMCoefficient* greenPtr = greenCoeff.getLevel(info.level);
		const PTMCoefficient* bluePtr = blueCoeff.getLevel(info.level);
		LightMemoized lVec(info.light.X(), info.light.Y());
		
		#pragma omp parallel for schedule(static,CHUNK)
		for (int y = info.offy; y < info.offy + info.height; y++)
		{
            int offsetBuf = (y-info.offy)*info.width<<2;
			int offset= y * mipMapSize[info.level].width() + info.offx;
			for (int x = info.offx; x < info.offx + info.width; x++)
			{	
				buffer[offsetBuf + 3] = 255;
				buffer[offsetBuf + 0] = tobyte(redPtr[offset].evalPoly(lVec)); //evalPoly(&(redPtr[offset][0]), info.light.X(), info.light.Y()));
				buffer[offsetBuf + 1] = tobyte(greenPtr[offset].evalPoly(lVec)); //evalPoly(&(greenPtr[offset][0]), info.light.X(), info.light.Y()));
				buffer[offsetBuf + 2] = tobyte(bluePtr[offset].evalPoly(lVec)); //evalPoly(&(bluePtr[offset][0]), info.light.X(), info.light.Y()));
				offset++;
				offsetBuf += 4;
			}
		}

	}

	void applyHSH(const PyramidCoeffF& redCoeff, const PyramidCoeffF& greenCoeff, const PyramidCoeffF& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
	{
		const float* redPtr = redCoeff.getLevel(info.level);
		const float* greenPtr = greenCoeff.getLevel(info.level);
		const float* bluePtr = blueCoeff.getLevel(info.level);
		int tempW = mipMapSize[info.level].width();
        float hweights[9];
		vcg::Point3d temp(info.light.X(), info.light.Y(), info.light.Z());
		temp.Normalize();
        float phi = atan2(temp.Y(), temp.X());
		if (phi<0) 
			phi = 2*M_PI+phi;
        float theta = qMin<float>(acos(temp.Z()/temp.Norm()), M_PI / 2 - 0.04);
		

		//int offsetBuf = 0;
		getHSH(theta, phi, hweights, sqrt((float)info.ordlen));
		
		#pragma omp parallel for schedule(static,CHUNK)
		for (int y = info.offy; y < info.offy + info.height; y++)
		{
			
            int offsetBuf = (y-info.offy)*info.width<<2;
			int offset= y * tempW + info.offx;
			for (int x = info.offx; x < info.offx + info.width; x++)
			{
				float r,b,g;
				r = 0;
				b = 0;
				g = 0;
				int offset2 = offset*info.ordlen;
				for (int k = 0; k < info.ordlen; k++)
				{
					int offset3 = offset2 + k;
					r += redPtr[offset3] * hweights[k];
					g += greenPtr[offset3] * hweights[k];
					b += bluePtr[offset3] * hweights[k];
				}
				buffer[offsetBuf + 0] = tobyte(r*255);
				buffer[offsetBuf + 1] = tobyte(g*255);
				buffer[offsetBuf + 2] = tobyte(b*255);
				buffer[offsetBuf + 3] = 255;
				offsetBuf += 4;
				offset++;
			}
		}

	}

	public slots:

		void resetRemote()
		{
			remote =  false;
		}

};

#endif //DEFAULT_REND_H
