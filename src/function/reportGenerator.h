/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Weiqi Shi (weiqi.shi@yale.edu)

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
#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include <QtScript>
#include <QPrinter>
#include <QWidget>
#include <QFileDialog>
#include <QDomDocument>
#include <QPainter>
#include <QFont>
#include <QTextDocument>
#include <QRectF>
#include "../CHE/CHEInfoDialog.h"
#include "../vtkEnums.h"

class ReportObject : public QWidget
{
	Q_OBJECT

public:
	WidgetMode mMode;
	QString mName;
	QVector<QString> mNotes;
	QVector<QString> mPictures; // <picture path, height/width ratio>
	QString mCHEName;
};

class ReportGenerator : public QWidget
{
	Q_OBJECT

public:
	ReportGenerator(QString path, bool project = true);

	void setProjectName(const QString project)	{mProjectName = project;}

	void setUserName(const QString userName)	{mUserName = userName;}

	void setKeyword(const QString keyword)	{mKeyword = keyword;}

	void setAffiliation(const QString affiliation)	{mAffiliation = affiliation;}

	void setDescription(const QString description)	{mDescription = description;}

	void setCHEInfo(const CHEInfoBasic* info);

	void addObject(ReportObject* object)	{mObjects.push_back(object);}

	void generate();

private:
	bool isProject;
	QString mLocation;
	QPrinter* mPrinter;
	QTextDocument* mDoc;
	QString mProjectName;
	QString mUserName;
	QString mKeyword, mAffiliation, mDescription;
	CHEInfoBasic* mCHEInfo;
	QVector<ReportObject*> mObjects;

};

#endif // REPORT_GENERATOR_H