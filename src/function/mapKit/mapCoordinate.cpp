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
