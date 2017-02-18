/*****************************************************************************
 * mapCoordinate.cpp
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

#include "mapCoordinate.h"

mapCoordinate::mapCoordinate() : _latitude(0.0), _longitude(0.0)
{
}

mapCoordinate::mapCoordinate(qreal latitude, qreal longitude) :
    _latitude(latitude), _longitude(longitude)
{
}

qreal mapCoordinate::latitude() const
{
    return _latitude;
}

void mapCoordinate::setLatitude(qreal latitude)
{
    _latitude = latitude;
}

qreal mapCoordinate::longitude() const
{
    return _longitude;
}

void mapCoordinate::setLongitude(qreal longitude)
{
    _longitude = longitude;
}

bool mapCoordinate::operator==(const mapCoordinate &other)
{
    return (latitude() == other.latitude())
           && (longitude() == other.longitude());
}
