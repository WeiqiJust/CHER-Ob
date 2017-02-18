/*****************************************************************************
 * mapWidget.cpp
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

#include "mapWidget.h"
#include <QScrollBar>
#include <QtDebug>
#include "../../information/informationWidget.h"
#include "../../mainWindow.h"

MapWidget::MapWidget(QWidget *parent, Information *mInformation)
    : QWidget(parent)
{
	refInformation = mInformation;
	isLoaded = false;

    mainLayout = new QVBoxLayout();
    _mapView = new mapView(mapView::RoadMap,
                             mapCoordinate(41.313129, -72.925033),
                             15);
	_mapView->setEnabled(false);
    _controls = new QWidget();
    buildControls();
	markButton = new QPushButton("Mark");
	markButton->setEnabled(false);
    mainLayout->addWidget(_mapView, 1);
    // mainLayout->addWidget(_controls, 1);
    mainLayout->addWidget(markButton, 1);
    setLayout(mainLayout);

	connect(markButton, SIGNAL(clicked()), this, SLOT(createMark()));
    connect(_mapView, SIGNAL(mapLoaded()), this, SLOT(onMapLoaded()));
	connect(_mapView, SIGNAL(mouseClicked(mapCoordinate)), this, SLOT(onMouseClicked(mapCoordinate)));
    connect(_mapView, SIGNAL(regionChanged(mapCoordinateRegion)),
            this, SLOT(onRegionChanged(mapCoordinateRegion)));
    connect(_mapView, SIGNAL(mapBecameIdle()), this, SLOT(onMapBecameIdle()));
}

MapWidget::~MapWidget()
{

}

void MapWidget::buildControls()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    _logger = new QTextEdit();
    _logger->setReadOnly(true);
    // mainLayout->addWidget(_logger, 1);
    _controls->setLayout(mainLayout);
}

void MapWidget::log(QString text, QString delimiter)
{
    _logger->setPlainText(_logger->toPlainText() + delimiter + text);
    _logger->verticalScrollBar()->setSliderPosition(    // Scroll to bottom
                _logger->verticalScrollBar()->maximum());
}

void MapWidget::log(const char *text, QString delimiter)
{
    log(QString(text), delimiter);
}

void MapWidget::createMark()
{
	QString markName = refInformation->getNotePath();
	QStringList markNameList = markName.split(QDir::separator());
	markName = markNameList[markNameList.size() - 2];
	refInformation->setGeoInfo(markName, _mapView->center());

	QString arg = "'";
	arg.append(markName).append("'");
	_mapView->markCenter(arg, _mapView->center());
	refInformation->mw()->updateXML4Map();
}

void MapWidget::onMapLoaded()
{
    log("Loaded:");
    log(QString("Type %1 at (%2, %3) , zoom %4").arg(
            QString::number(_mapView->mapType()),
            QString::number(_mapView->center().latitude()),
            QString::number(_mapView->center().longitude()),
            QString::number(_mapView->zoomLevel())),
        " ");
}

void MapWidget::onMouseClicked(mapCoordinate latlong)
{
	log(QString("Mouse click at: (%1, %2)").arg(
		QString::number(latlong.latitude()),
		QString::number(latlong.longitude())),
		" ");
}

void MapWidget::onMapBecameIdle()
{
    log("Idle");
}

void MapWidget::onRegionChanged(mapCoordinateRegion region)
{
    log("Region changed:");
    log(QString("((%1, %2), (%3, %4))").arg(
            QString::number(region.northWest().latitude()),
            QString::number(region.west()),
            QString::number(region.south()),
            QString::number(region.southEast().longitude())),
        " ");
}

void MapWidget::startGeoMarking()
{
	if (!isLoaded)
	{
		_mapView->setCenter(refInformation->centerMarkers());
		_mapView->setZoomLevel(5);
		isLoaded = true;
	}
	_mapView->setEnabled(true);
	markButton->setEnabled(true);

	if (refInformation != NULL)
	{
		QString markName = refInformation->getNotePath();
		QStringList markNameList = markName.split(QDir::separator());
		markName = markNameList[markNameList.size() - 2];
		QString arg = "'";
		arg.append(markName).append("'");
		_mapView->bounceMarker(arg);
	}
}

void MapWidget::finishGeoMarking()
{
	_mapView->setEnabled(false);
	markButton->setEnabled(false);
}

void MapWidget::loadMark(QString name, mapCoordinate coord)
{
	QString arg = "'";
	arg.append(name).append("'");
	_mapView->markCenter(arg, coord);
}