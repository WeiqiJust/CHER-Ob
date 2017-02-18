/*****************************************************************************
 * mapCoordinateRegion.cpp
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

#include "mapCoordinateRegion.h"
#include <QtDebug>

mapCoordinateRegion::mapCoordinateRegion() :
    _east(0.0), _west(0.0), _north(0.0), _south(0.0)
{
}

mapCoordinateRegion::mapCoordinateRegion(mapCoordinate southWest,
                                       mapCoordinate northEast) :
    _east(northEast.longitude()),
    _west(southWest.longitude()),
    _north(northEast.latitude()),
    _south(southWest.latitude())
{
}

mapCoordinateRegion::mapCoordinateRegion(qreal north, qreal south,
                                       qreal east, qreal west) :
    _east(east), _west(west), _north(north), _south(south)
{
}

mapCoordinateRegion::mapCoordinateRegion(mapCoordinate center,
                                       mapCoordinateSpan span) :
    _east(center.longitude() + span.longitudeDelta() / 2),
    _west(center.longitude() - span.longitudeDelta() / 2),
    _north(center.latitude() + span.latitudeDelta() / 2),
    _south(center.latitude() - span.latitudeDelta() / 2)
{
}

bool mapCoordinateRegion::contains(mapCoordinate &point, bool proper) const
{
    qreal lng = point.longitude();
    qreal lat = point.latitude();

    bool r = false;
    if (proper)
        r = lat > west() && lat < east() && lng > south() && lng < north();
    else
        r = lat >= west() && lat <= east() && lng >= south() && lng <= north();

    return r;
}

bool mapCoordinateRegion::intersects(mapCoordinateRegion &span) const
{
    Q_UNUSED(span)
    qWarning("mapCoordinateRegion::intersects not implemented; false returned");
    return false;
}

bool mapCoordinateRegion::isEmpty() const
{
    return (east() == west()) || (north() == south());
}

qreal mapCoordinateRegion::east() const
{
    return _east;
}

qreal mapCoordinateRegion::west() const
{
    return _west;
}

qreal mapCoordinateRegion::north() const
{
    return _north;
}

qreal mapCoordinateRegion::south() const
{
    return _south;
}

void mapCoordinateRegion::setEast(qreal value)
{
    _east = value;
}

void mapCoordinateRegion::setWest(qreal value)
{
    _west = value;
}

void mapCoordinateRegion::setNorth(qreal value)
{
    _north = value;
}

void mapCoordinateRegion::setSouth(qreal value)
{
    _south = value;
}

mapCoordinate mapCoordinateRegion::southWest() const
{
    return mapCoordinate(south(), west());
}

mapCoordinate mapCoordinateRegion::northEast() const
{
    return mapCoordinate(north(), east());
}

mapCoordinate mapCoordinateRegion::southEast() const
{
    return mapCoordinate(south(), east());
}

mapCoordinate mapCoordinateRegion::northWest() const
{
    return mapCoordinate(north(), west());
}

mapCoordinate mapCoordinateRegion::center() const
{
    return mapCoordinate((north() + south()) / 2, (east() + west()) / 2);
}

mapCoordinateSpan mapCoordinateRegion::span() const
{
    return mapCoordinateSpan(north() - south(), east() - west());
}

bool mapCoordinateRegion::operator ==(const mapCoordinateRegion &other)
{
    return (east() == other.east())
            && (west() == other.west())
            && (north() == other.north())
            && (south() == other.south());
}
