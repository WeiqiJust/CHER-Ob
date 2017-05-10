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
 * mapWidget.h
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

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include "mapView.h"
#include "mapCoordinateRegion.h"

class Information;

class MapWidget : public QWidget
{
    Q_OBJECT

public:
    MapWidget(QWidget *parent = 0, Information *mInformation = NULL);
    ~MapWidget();
	void init(Information *mInformation);
	void clear();
	void startGeoMarking();
	void finishGeoMarking();
	void loadMark(QString name, mapCoordinate coord);
	void setPos4Video(mapCoordinate pos);
	void makeScreenshot(QString path);
	//// refresh() to be implemented! e.g. close and open another

public slots:
    void log(QString text, QString delimiter = QString("\n"));
    void log(const char *text, QString delimiter = QString("\n"));

private slots:
	void createMark();
    void onMapLoaded();
	void onMouseClicked(mapCoordinate latlong);
    void onMapBecameIdle();
    void onRegionChanged(mapCoordinateRegion region);

private:
	Information *refInformation;
	QVBoxLayout *mainLayout;
    mapView *_mapView;
	QPushButton *markButton;
    QWidget *_controls;
    QTextEdit *_logger;
    void buildControls();
	bool isLoaded;
};

#endif // WIDGET_H
