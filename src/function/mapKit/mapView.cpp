/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Zeyu Wang (zeyu.wang@yale.edu)

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

 - Acknowledgments: Some portions of this file are based on the example codes
   of ITK/VTK library from Kitware, QT API from Nokia. I would like to thank
   anonymous help from various software engineering communities.

   This file defines the different classes of bookmark that the user may work
   with (corresponding to the different file types that may be displayed).

*****************************************************************************/
/*****************************************************************************
 * mapView.cpp
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

#include "mapView.h"
#include <QHBoxLayout>
#include <QHash>
#include <QVariantMap>
#include <QWebFrame>
#include <QWebView>
#include <QtDebug>

class CustomWebPage : public QWebPage
{
    public:
        CustomWebPage(QObject *parent = 0) : QWebPage(parent) {}

    protected:
        virtual void javaScriptConsoleMessage(
                const QString &message, int lineNumber,
                const QString &sourceID);
};

void CustomWebPage::javaScriptConsoleMessage(
        const QString &message, int lineNumber, const QString &sourceID)
{
    qDebug() << "JavaScript" << sourceID << "line" << lineNumber <<
                ":" << message;
}


class mapViewPrivate
{
    Q_DECLARE_PUBLIC(mapView)

public:
    mapViewPrivate(mapView *q) : q_ptr(q), loaded(false)
    {
        webView = new QWebView();
        webView->setPage(new CustomWebPage());
		webView->load(QUrl("qrc:///gmap.html"));
    }
    ~mapViewPrivate() {}
    mapView *q_ptr;

    bool loaded;
    QWebView *webView;
    struct {
        mapCoordinate centerCoordinate;
        mapView::MapType mapType;
        uint zoomLevel;
    } initialValues;

    inline QWebFrame *frame() { return webView->page()->mainFrame(); }
    inline QVariant evaluateJavaScript(const QString &script,
                                       bool force = false)
    {
        if (loaded || force)
            return frame()->evaluateJavaScript(script);
        else
            return QVariant();
    }
    inline QString toJsMapType(mapView::MapType type)
    {
        QString typeName;
        switch (type)
        {
        case mapView::Hybrid:
            typeName = "HYBRID";
            break;
        case mapView::RoadMap:
            typeName = "ROADMAP";
            break;
        case mapView::Satellite:
            typeName = "SATELLITE";
            break;
        case mapView::Terrain:
            typeName = "TERRAIN";
            break;
        }
        return QString("google.maps.MapTypeId.%1").arg(typeName);
    }
    inline mapView::MapType fromJsMapType(QString &type)
    {
        static QHash<QString, mapView::MapType> types;
        if (types.isEmpty())
        {
            types.insert("hybrid", mapView::Hybrid);
            types.insert("roadmap", mapView::RoadMap);
            types.insert("satellite", mapView::Satellite);
            types.insert("terrain", mapView::Terrain);
        }
        return types.value(type, initialValues.mapType);
    }
};

mapView::mapView(MapType mapType, mapCoordinate center, uint zoomLevel,
                     QWidget *parent) :
    QWidget(parent), d_ptr(new mapViewPrivate(this))
{
    Q_D(mapView);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(d->webView);
    setLayout(mainLayout);

    d->initialValues.centerCoordinate = center;
    d->initialValues.mapType = mapType;
    d->initialValues.zoomLevel = zoomLevel;
    connect(d->frame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(insertNativeObject()));
    connect(d->webView, SIGNAL(loadFinished(bool)),
            this, SLOT(initializeMap()));
}

QVariant mapView::getPos4VideoView(QString name)
{
	Q_D(mapView);
	return d->evaluateJavaScript(QString("getPositionVideo(%1);").arg(name));
}

// make a screenshot of QWebView
void mapView::makeScreenshotView(QString path)
{
	// QImage image(d_ptr->frame()->contentsSize(), QImage::Format_ARGB32);
	QImage image(QSize(480, 480), QImage::Format_ARGB32);
	QPainter painter(&image);
	d_ptr->frame()->render(&painter);
	painter.end();
	image.save(path);
}

void mapView::insertNativeObject()
{
    Q_D(mapView);
    d->frame()->addToJavaScriptWindowObject("qMapView", this);
}

void mapView::initializeMap()
{
    Q_D(mapView);
    if (d->loaded)
        return;
    mapCoordinate &center = d->initialValues.centerCoordinate;
    QString js = QString("initialize(%1, %2, %3, %4);").arg(
                QString::number(center.longitude()),
                QString::number(center.latitude()),
                d->toJsMapType(d->initialValues.mapType),
                QString::number(d->initialValues.zoomLevel));
    d->evaluateJavaScript(js, true);

    d->loaded = true;
    emit mapLoaded();
}

void mapView::resizeEvent(QResizeEvent *)
{
    Q_D(mapView);
    d->evaluateJavaScript("google.maps.event.trigger(map, 'resize');");
}

mapView::MapType mapView::mapType() const
{
    QString res = d_ptr->evaluateJavaScript("map.getMapTypeId();").toString();
    return d_ptr->fromJsMapType(res);
}

/** Returns the map's current visible region.
 * Corresponds to `getBounds()`. Due to the limitation in the Google Map
 * JavaScript API, this method works only after the first `regionChanged()`
 * signal is sent. The result of any invocation of this method before that is
 * undifined.
 */
mapCoordinateRegion mapView::region() const
{
    QVariantMap result = d_ptr->evaluateJavaScript("getMapBounds();").toMap();
    return mapCoordinateRegion(mapCoordinate(result["south"].toReal(),
                                           result["west"].toReal()),
                              mapCoordinate(result["north"].toReal(),
                                           result["east"].toReal()));
}

mapCoordinate mapView::center() const
{
    QVariantMap result = d_ptr->evaluateJavaScript("getMapCenter();").toMap();
    return mapCoordinate(result["latitude"].toReal(),
                        result["longitude"].toReal());
}

uint mapView::zoomLevel() const
{
    return d_ptr->evaluateJavaScript("map.getZoom();").toUInt();
}

qreal mapView::heading() const
{
    return d_ptr->evaluateJavaScript("map.getHeading();").toReal();
}

qreal mapView::tilt() const
{
    return d_ptr->evaluateJavaScript("map.getTilt();").toReal();
}

void mapView::setMapType(MapType type)
{
    Q_D(mapView);
    QString typeName = d->toJsMapType(type);
    QString script = QString("map.setMapTypeId(%1);").arg(typeName);
    d->evaluateJavaScript(script);
}

void mapView::setCenter(mapCoordinate center, bool animated)
{
    Q_D(mapView);
    QString format = QString("setMapCenter(%1, %2, %3);");
    QString js = format.arg(QString::number(center.latitude()),
                            QString::number(center.longitude()),
                            animated ? "true" : "false");
    d->evaluateJavaScript(js);
}

void mapView::setZoomLevel(uint zoom)
{
    Q_D(mapView);
    d->evaluateJavaScript(QString("map.setZoom(%1);").arg(zoom));
}

void mapView::clearMarkers()
{
    Q_D(mapView);
    d->evaluateJavaScript(QString("clearAllMarkers();"));
}

void mapView::makeRegionVisible(mapCoordinateRegion &region)
{
    Q_D(mapView);
    QString format = QString("panMapToBounds(%1, %2, %3, %4);");
    QString js = format
            .arg(region.north())
            .arg(region.south())
            .arg(region.east())
            .arg(region.west());
    d->evaluateJavaScript(js);
}

void mapView::fitRegion(mapCoordinateRegion &region)
{
    Q_D(mapView);
    QString format = QString("fitMapToBounds(%1, %2, %3, %4);");
    QString js = format
            .arg(region.north())
            .arg(region.south())
            .arg(region.east())
            .arg(region.west());
    d->evaluateJavaScript(js);
}

void mapView::markCenter(QString name, mapCoordinate center) 
{
	Q_D(mapView);
    QString format = QString("appendMarker(%1, %2, %3);");
    QString js = format
            .arg(name)
            .arg(center.latitude())
            .arg(center.longitude());
    d->evaluateJavaScript(js);
}


void mapView::bounceMarker(QString name)
{
	Q_D(mapView);
    QString format = QString("bounceMarkerJS(%1);");
    QString js = format.arg(name);
    d->evaluateJavaScript(js);
}

void mapView::regionDidChangeTo(qreal north, qreal south,
                                  qreal east, qreal west)
{
    emit regionChanged(mapCoordinateRegion(north, south, east, west));
}

void mapView::centerDidChangeTo(qreal latitude, qreal longitude)
{
    emit centerChanged(mapCoordinate(latitude, longitude));
}

void mapView::mapTypeDidChangeTo(QString typeString)
{
    Q_D(mapView);
    emit mapTypeChanged(d->fromJsMapType(typeString));
}

void mapView::mouseDidClickAt(qreal latitude, qreal longitude)
{
    emit mouseClicked(mapCoordinate(latitude, longitude));
}

void mapView::mouseDidDoubleClickAt(qreal latitude, qreal longitude)
{
    emit mouseDoubleClicked(mapCoordinate(latitude, longitude));
}

void mapView::mouseDidRightClickAt(qreal latitude, qreal longitude)
{
    emit mouseRightClicked(mapCoordinate(latitude, longitude));
}

void mapView::cursorDidMoveTo(qreal latitude, qreal longitude)
{
    emit cursorMoved(mapCoordinate(latitude, longitude));
}

void mapView::cursorDidEnterTo(qreal latitude, qreal longitude)
{
    emit cursorEntered(mapCoordinate(latitude, longitude));
}

void mapView::cursorDidLeaveFrom(qreal latitude, qreal longitude)
{
    emit cursorLeaved(mapCoordinate(latitude, longitude));
}
