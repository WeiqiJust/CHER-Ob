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

#include <vtkExtractSelectedFrustum.h>
#include <vtkDataSet.h>

#include "../CHE/CHEInfoDialog.h"
#include "../vtkEnums.h"
#include "../visualization/vtkWidget.h"

class WidgetInfo3D
{
public:
	bool infoStatus;
	bool isMeasuring;
	bool isAnnotation;
	double pos[3]; // camera position
    double foc[3]; // focal point
    double angle; // view angle
    double view[3]; // view up direction
    double clip[2]; // clipping range
    double scale; // parallel scale
	bool isDirectionalLight;
	double orientation[3];
	double brightness;
	double contrast;
	RenderMode3D renderMode;
	int interpolation;
	int textureOn;
};


class ReportObject : public QWidget
{
	Q_OBJECT

public:
	WidgetMode mMode;
	QString mName;
	QString mNotesPath;
	QVector<QString> mNotes;	// note content
	QVector<QString> mPictures;	// image notes
	QVector<int> mCategories;	// selected categories for notes
	QString mCHEName;
	VtkWidget* mGla;
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

	static MainWindow* mw();

private:
	void detectPointVisibility(vtkSmartPointer<vtkRenderer> render, QVector<double*> points, QVector<QPair<double, double> >& visiblePoints);

	void detectFrustumVisibility(const VtkWidget* gla, QVector<double*> center, QVector<QPair<double, double> >& visiblePoints, QVector<vtkSmartPointer<vtkDataSet> > dataset, OrthogonalView3D view);

	void saveWidgetinfo(const VtkWidget* gla, WidgetInfo3D &info);

	void initWidget(VtkWidget* gla);

	void recoverWidget(VtkWidget* gla, WidgetInfo3D info);

	void computeCenter(const VtkWidget* gla, QVector<int> cellIds, double* center);

	void computeCenter(const VtkWidget* gla, vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals, double* center, vtkSmartPointer<vtkDataSet>& polyData);

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