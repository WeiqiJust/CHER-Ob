/*****************************************************************************
 * QtMapWidget.cpp
 *
 * Created: 08/7 2013 by uranusjr
 *
 * Copyright 2013 uranusjr. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/

#include "QtMapWidget.h"
#include <QVBoxLayout>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QtDebug>
#include <QPushButton>

QtMapWidget::QtMapWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    _mapView = new QMMapView(QMMapView::RoadMap,
                             QMCoordinate(41.313129, -72.925033),
                             12);
	_mapView->setEnabled(false);
    _controls = new QWidget();
    buildControls();
	QPushButton *markButton = new QPushButton("Mark");
	markButton->setEnabled(false);
    mainLayout->addWidget(_mapView, 1);
    // mainLayout->addWidget(_controls, 1);
    mainLayout->addWidget(markButton, 1);
    setLayout(mainLayout);

	connect(markButton, SIGNAL(clicked()), this, SLOT(createMark()));
    connect(_mapView, SIGNAL(mapLoaded()), this, SLOT(onMapLoaded()));
	connect(_mapView, SIGNAL(mouseClicked(QMCoordinate)), this, SLOT(onMouseClicked(QMCoordinate)));
    connect(_mapView, SIGNAL(regionChanged(QMCoordinateRegion)),
            this, SLOT(onRegionChanged(QMCoordinateRegion)));
    connect(_mapView, SIGNAL(mapBecameIdle()), this, SLOT(onMapBecameIdle()));
}

QtMapWidget::~QtMapWidget()
{

}

void QtMapWidget::buildControls()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    _logger = new QTextEdit();
    _logger->setReadOnly(true);
    // mainLayout->addWidget(_logger, 1);
    _controls->setLayout(mainLayout);
}

void QtMapWidget::log(QString text, QString delimiter)
{
    _logger->setPlainText(_logger->toPlainText() + delimiter + text);
    _logger->verticalScrollBar()->setSliderPosition(    // Scroll to bottom
                _logger->verticalScrollBar()->maximum());
}

void QtMapWidget::log(const char *text, QString delimiter)
{
    log(QString(text), delimiter);
}

void QtMapWidget::createMark()
{
	_mapView->markCenter(QString("'test'"), _mapView->center());
	log(QString("Center marked: (%1, %2)").arg(
		QString::number(_mapView->center().latitude()),
		QString::number(_mapView->center().longitude())),
		"\n");
}

void QtMapWidget::onMapLoaded()
{
    log("Loaded:");
    log(QString("Type %1 at (%2, %3) , zoom %4").arg(
            QString::number(_mapView->mapType()),
            QString::number(_mapView->center().latitude()),
            QString::number(_mapView->center().longitude()),
            QString::number(_mapView->zoomLevel())),
        " ");
}

void QtMapWidget::onMouseClicked(QMCoordinate latlong)
{
	log(QString("Mouse click at: (%1, %2)").arg(
		QString::number(latlong.latitude()),
		QString::number(latlong.longitude())),
		" ");
}

void QtMapWidget::onMapBecameIdle()
{
    log("Idle");
}

void QtMapWidget::onRegionChanged(QMCoordinateRegion region)
{
    log("Region changed:");
    log(QString("((%1, %2), (%3, %4))").arg(
            QString::number(region.northWest().latitude()),
            QString::number(region.west()),
            QString::number(region.south()),
            QString::number(region.southEast().longitude())),
        " ");
}
