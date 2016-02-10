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


#include "lightControlRTI.h"

#include <QPainter>
#include <QRadialGradient>
#include <QMouseEvent>
#include <QDebug>


LightControlRTI::LightControlRTI(QWidget *parent, int size): QWidget(parent),
	width(size),
	height(size),
	xFocus(size/2),
	yFocus(size/2),
	timer(NULL),
	dx(size/2),
	dy(size/2),
	light(0,0,1),
	rightPressed(false),
	dragging(false),
	interactive(true),
	enabled(true)
{
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(fired()));
	setAutoFillBackground(true);
	setFixedSize(width, height);
	stops << QGradientStop(0.00, QColor::fromRgba(0xfffaffa0));
	stops << QGradientStop(0.02, QColor::fromRgba(0xfff9ff99));
	stops << QGradientStop(0.08, QColor::fromRgba(0xfff3ff86));
	stops << QGradientStop(0.14, QColor::fromRgba(0xffd3ff79));
	stops << QGradientStop(0.40, QColor::fromRgba(0xff93b353));
	stops << QGradientStop(0.80, QColor::fromRgba(0xff264619));
	stops << QGradientStop(0.98, QColor::fromRgba(0xff0c1306));
	stops << QGradientStop(1.00, QColor::fromRgba(0xff000000));
}


LightControlRTI::~LightControlRTI()
{
	if (timer)
		delete timer;
}


void LightControlRTI::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	QRadialGradient radial(QPointF(width/2, height/2), width/2, QPoint(xFocus, yFocus));
	radial.setStops(stops);
	radial.setSpread(QGradient::PadSpread);

	painter.setPen(Qt::NoPen);
	painter.setBrush(radial);
	painter.drawEllipse(0, 0, width, height);
	
	painter.setPen(QColor(255, 0, 0));
	painter.setBrush(Qt::NoBrush);
	if (rightPressed)
	{
		//Draw the shape to show the max light position shift
		if (width/2  - dx == 0 && height/2 - dy == 0) {
			painter.drawEllipse(width/2 - dx, height/2 - dy, dx * 2 - 1, dy * 2 -1); }
		else if (dx*dx + dy*dy <= width*width/4) {
			painter.drawRect(width/2 - dx, height/2 - dy, dx * 2 - 1, dy * 2 -1); }
		else
		{
                        float alpha = acos(2.0*dx/width);
                        float beta = acos(2.0*dy/height);
			int dx2 = width/2.0 * sin(beta);
			int dy2 = height/2.0 * sin(alpha);
			painter.drawArc(0, 0, width, height, 16*(M_PI/2 + beta)*180/M_PI, 16*(M_PI/2 - alpha - beta)*180/M_PI);
			painter.drawArc(0, 0, width, height, 16*(alpha)*180/M_PI, 16*(M_PI/2 - beta - alpha)*180/M_PI);
			painter.drawArc(0, 0, width, height, 16*(-M_PI/2 + beta)*180/M_PI, 16*(M_PI/2 - alpha - beta)*180/M_PI);
			painter.drawArc(0, 0, width, height, 16*(M_PI + alpha)*180/M_PI, 16*(M_PI/2 - alpha - beta)*180/M_PI);
			painter.drawLine(width/2 - dx, height/2 - dy2, width/2 - dx, height/2 + dy2); 
			painter.drawLine(width/2 - dx2, height/2 - dy, width/2 + dx2, height/2 - dy);
			painter.drawLine(width/2 + dx, height/2 - dy2, width/2 + dx, height/2 + dy2);
			painter.drawLine(width/2 - dx2, height/2 + dy, width/2 + dx2, height/2 + dy);
		}
	}
}


void LightControlRTI::mousePressEvent(QMouseEvent *event)
{
	if (!enabled) return;
	timer->start(40);
	if (event->button() == Qt::LeftButton)
	{
		dragging = true;
		if (isValidPoint(event->x(), event->y()))
		{
			//The mouse position is in the rectangle defined by max light position shift
			//The focus is setted to current mouse position 
			xFocus = event->x();
			yFocus = event->y();
		}
		else
			//Set focus to a point over the border of rectangle defined by max light position shift 
			intersect(event->x(), event->y(), xFocus, yFocus); 
	}
	else if (event->button() == Qt::RightButton)
		rightPressed = true;
}


void LightControlRTI::mouseMoveEvent(QMouseEvent *event)
{
	if (!enabled) return;
	if (dragging)
	{
		if (isValidPoint(event->x(), event->y()))
		{
			xFocus = event->x();
			yFocus = event->y();
		}
		else
			intersect(event->x(), event->y(), xFocus, yFocus);

		//qDebug() << "(xFocus, yFocus) = " << xFocus << ", " << yFocus;
	}
}


void LightControlRTI::mouseReleaseEvent(QMouseEvent *event)
{
	if (!enabled) return;
	timer->stop();
	if (event->button() == Qt::LeftButton)
	{
		dragging = false;
		if (isValidPoint(event->x(), event->y()))
		{
			xFocus = event->x();
			yFocus = event->y();
		}
		else
			intersect(event->x(), event->y(), xFocus, yFocus);
		calcLightVector();
		emit lightChanged(light);
	}
	else if (event->button() == Qt::RightButton)
		rightPressed = false;
	update();
}


void LightControlRTI::fired()
{
	calcLightVector();
	if (interactive)
		emit lightChanged(light);
	update();
}

void LightControlRTI::setEnabled(bool value)
{
	enabled = value;
	update();
}


void LightControlRTI::moveLightPosition(float offx, float offy)
{
	if (!enabled) return;
	int x = xFocus + offx * width;
	int y = yFocus + offy * height;
	if (isValidPoint(x, y))
	{
		xFocus = x;
		yFocus = y;
	}
	else
		intersect(x, y, xFocus, yFocus);
	calcLightVector();
	emit lightChanged(light);
	update();
}


void LightControlRTI::setLight(const vcg::Point3f& l, bool refresh)
{
	xFocus = width/2 + l.X()*width/2;
	yFocus = height/2 - l.Y()*height/2;
	light = l;
	emit lightChanged(light, refresh);
	update();
}

vcg::Point3f LightControlRTI::getLight()
{
    return light;
}
