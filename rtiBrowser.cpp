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


// Local headers
#include "rtiBrowser.h"
#include "detailenhanc.h"

// Qt headers
#include <QMouseEvent>
#include <QTime>
#include <QApplication>
#include <QColor>
#include <QFileDialog>
#include <QPainter>
#include <QUuid>

#include <QDebug>
#include <QMessageBox>

#include <iostream>

RtiBrowser::RtiBrowser(Rti *image, QWidget *parent): QWidget(parent),
img(NULL),
light(0,0,1),
dxLight(0.0f),
dyLight(0.0f),
subimg(0,0,0,0),
level(0),
textureHeight(0),
textureWidth(0),
textureData(NULL),
FIRST_RTI_UPDATE(true)
{

    currentMode = DEFAULT_MODE;

    // set RTI image if given
    if (image)
        setImage(image);
}


RtiBrowser::~RtiBrowser()
{
    if (img)
        delete img;

    if (textureData)
    {
        delete[] textureData;
    }
}


void RtiBrowser::setImage(Rti* rti)
{
    if (img)
    {
        delete img;
        img = NULL;
    }

    if (!rti)
    {
        if (textureData)
            delete textureData;
        textureData = NULL;
        return;
    }
    img = rti;

    // Set sub-img
    subimg = QRectF(0.0, 0.0, img->width(), img->height());

    //level = zoom >= 1 ? 0 :  floor(log(1.0/zoom)/log(2.0));
	level = 0;
    updateTexture();
}


void RtiBrowser::setImage(Rti* rti, bool FIRST_RTI_UPDATE, bool LOADING_DONE)
{
    /*if (img)
    {
        delete img;
        img = NULL;
    }

    if (!rti)
    {
        if (textureData)
            delete textureData;
        textureData = NULL;
        return;
    }*/

    img = rti;
	this->FIRST_RTI_UPDATE = FIRST_RTI_UPDATE;

    // Set sub-img
    subimg = QRectF(0.0, 0.0, img->width(), img->height());
    
    //level = zoom >= 1 ? 0 :  floor(log(1.0/zoom)/log(2.0));
	level = 0;

    updateTexture();

	if (LOADING_DONE)
		emit rtiImageLoadingDone(this->getRenderingModes(), this->getCurrentRendering());
}

QMap<int, RenderingMode*>* RtiBrowser::getRenderingModes()
{
	if (img)
        return img->getSupportedRendering();
    return NULL;
}


int RtiBrowser::getCurrentRendering()
{
    if (img)
        return img->getCurrentRendering();
    return -1;
}

void RtiBrowser::setLight(vcg::Point3f l, bool refresh)
{
    light = l;
    if (img)
    {
        if (refresh)
            updateTexture();
    }
}


void RtiBrowser::updateTexture(bool refresh)
{
    if (textureData)
        delete textureData;
    QTime first = QTime::currentTime();
    img->createImage(&textureData, textureWidth, textureHeight, light, subimg, level, currentMode);

    QTime second = QTime::currentTime();
    if (first.msecsTo(second) > 120)
    {
        emit setInteractiveLight(false);
        interactive = false;
    }
    else
    {
        emit setInteractiveLight(true);
        interactive = true;
    }

	// YY
	//qDebug() << "(textureWidth, textureHeight) = " << textureWidth << "--" << textureHeight; // just for test
	if (!data.empty())
		data.clear();
	for (int i = 0; i < textureWidth*textureHeight*4; i++)
		data.push_back(textureData[i]);
	
	emit rtiImageChanged(data, textureWidth, textureHeight, FIRST_RTI_UPDATE); // YY: we'll capture in vtkWidget
}

void RtiBrowser::setRenderingMode(int mode)
{
	if (!img) {
		qDebug() << "ERROR!! RTI image doesn't exist! B";
	}
    img->setRenderingMode(mode);
    QMap<int, RenderingMode*>* list = img->getSupportedRendering();
    RenderingMode* rendering = list->value(mode);
    emit setInteractiveLight(rendering->isLightInteractive());
    emit setEnabledLight(rendering->enabledLighting());
    interactive = rendering->isLightInteractive();
    updateTexture();
}


void RtiBrowser::updateImage()
{
   updateTexture();
}