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
 * mapView.h
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

#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QWidget>
#include "mapKit.h"
#include "mapCoordinate.h"
#include "mapCoordinateRegion.h"
class mapViewPrivate;

class mapView : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(mapView)

public:
    enum MapType {
        Hybrid,
        RoadMap,
        Satellite,
        Terrain
    };

    mapView(MapType mapType, mapCoordinate center, uint zoomLevel,
              QWidget *parent = 0);
    MapType mapType() const;

    mapCoordinateRegion region() const;
    mapCoordinate center() const;

    uint zoomLevel() const;
    qreal heading() const;
    qreal tilt() const;

    void setMapType(MapType type);
    void setCenter(mapCoordinate center, bool animated = true);
    void setZoomLevel(uint zoom);
	void clearMarkers();

    void makeRegionVisible(mapCoordinateRegion &region);
    void fitRegion(mapCoordinateRegion &region);
	void markCenter(QString name, mapCoordinate center);
	void bounceMarker(QString name);

	QVariant getPos4VideoView(QString name);
	void makeScreenshotView(QString path);
//    void pan(int x, int y);
//    void setHeading(qreal heading);
//    void setTilt(qreal tile);

//    ??? projection() const; get
//    ??? streetView() const; get set
//    QUrl draggableCursor(); get set
//    QUrl draggingCursor(); get set
//    bool useMapMarker() const; get set

protected:
    void resizeEvent(QResizeEvent *);

signals:
    void mapLoaded();
    void mapBecameIdle();
    void regionChanged(mapCoordinateRegion region);
    void centerChanged(mapCoordinate center);
    void mouseClicked(mapCoordinate coordinate);
    void mouseDoubleClicked(mapCoordinate coordinate);
    void mouseRightClicked(mapCoordinate coordinate);
    void mouseDragged();
    void mouseDragStarted();
    void mouseDragEnded();
    void cursorMoved(mapCoordinate coordinate);
    void cursorEntered(mapCoordinate coordinate);
    void cursorLeaved(mapCoordinate coordinate);
    void headingChanged();
    void mapTypeChanged(MapType type);
//    void projectionChanged();
    void tilesLoaded();
    void tilesChanged();
    void zoomLevelChanged(uint level);

protected slots:
    void initializeMap();
    void insertNativeObject();
    void regionDidChangeTo(qreal north, qreal south, qreal east, qreal west);
    void centerDidChangeTo(qreal latitude, qreal longitude);
    void mapTypeDidChangeTo(QString typeString);
    void mouseDidClickAt(qreal latitude, qreal longitude);
    void mouseDidDoubleClickAt(qreal latitude, qreal longitude);
    void mouseDidRightClickAt(qreal latitude, qreal longitude);
    void cursorDidMoveTo(qreal latitude, qreal longitude);
    void cursorDidEnterTo(qreal latitude, qreal longitude);
    void cursorDidLeaveFrom(qreal latitude, qreal longitude);

private:
    mapViewPrivate *d_ptr;
};

#endif // QMMAPVIEW_H
