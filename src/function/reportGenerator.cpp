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
#include <vtkSelectVisiblePoints.h>
#include <vtkInteractorObserver.h>
#include <vtkIdList.h>
#include <vtkCenterOfMass.h>
#include <vtkCellLocator.h>
#include <vtkCleanPolyData.h>
#include <vtkPlanes.h>
#include <vtkExtractSelectedFrustum.h>
#include <vtkDoubleArray.h>

#include "reportGenerator.h"
#include "../function/lightControl.h"
#include "../mainWindow.h"


ReportGenerator::ReportGenerator(QString path, bool project)
{
	isProject = project;
	mLocation = path;
	
    mPrinter = new QPrinter(QPrinter::PrinterResolution);
    mPrinter->setOutputFormat(QPrinter::PdfFormat);
    mPrinter->setPaperSize(QPrinter::A4);
    mPrinter->setOutputFileName(mLocation);
 
    mDoc = new QTextDocument();
	QFile file(path);
	file.close();
   // mDoc->setHtml("<h1>Hello, World!</h1>\n<p>Lorem ipsum dolor sit amet, consectitur adipisci elit.</p>");
    
}

void ReportGenerator::setCHEInfo(const CHEInfoBasic* info)
{
	mCHEInfo = new CHEInfoBasic();
	mCHEInfo->object = info->object;
	mCHEInfo->measurement = info->measurement;
	mCHEInfo->creation = info->creation;
	mCHEInfo->material = info->material;
	mCHEInfo->description = info->description;
	mCHEInfo->conservation = info->conservation;
	mCHEInfo->analyses = info->analyses;
	mCHEInfo->related = info->related;
	mCHEInfo->administration = info->administration;
	mCHEInfo->documents = info->documents;
	mCHEInfo->other = info->other;
}

void ReportGenerator::generate()
{
	QString html;
	html.append("<p><font size=\"8\"><div align=\"center\">\n" + mProjectName + "\n</div></font></p>\n");
	html = html + QString("<p><font size=\"3\">\nUser Name:   ") + mUserName + QString("\n</font></p>\n");
	if (isProject)
	{
		html = html	+ QString("<p><font size=\"3\">\nKeyword: ") + mKeyword + QString("\n</font></p>\n")
			+ QString("<p><font size=\"3\">\nAffiliation: ") + mAffiliation + QString("\n</font></p>\n")
			+ QString("<p><font size=\"3\">\nDescription: <br>") + mDescription + QString("\n</br></font></p>\n");
	}
	else
	{
		html = html	+ QString("<p><font size=\"3\">\nObject / Work: ") + mCHEInfo->object + QString("\n</font></p>\n")
			+ QString("<p><font size=\"3\">\nPhysical Dimensions / Measurement: ") + mCHEInfo->measurement + QString("\n</font></p>\n")
			+ QString("<p><font size=\"3\">\nCreation: ") + mCHEInfo->creation + QString("\n</font></p>\n")
			+ QString("<p><font size=\"3\">\nMaterials and Techniques: ") + mCHEInfo->material + QString("\n</font></p>\n")
			+ QString("<p><font size=\"3\">\nStylisyic Analysis and Descriptions: ") + mCHEInfo->description + QString("\n</font></p>\n")
			+ QString("<p><font size=\"3\">\nCondition and Conservation: ") + mCHEInfo->conservation + QString("\n</font></p>\n")
			+ QString("<p><font size=\"3\">\nAnalyses: ") + mCHEInfo->analyses + QString("\n</font></p>\n")
			+ QString("<p><font size=\"3\">\nRelated Works: ") + mCHEInfo->related + QString("\n</font></p>\n")
			+ QString("<p><font size=\"3\">\nExhibition / Loans and Legal Issues: ") + mCHEInfo->administration + QString("\n</font></p>\n")
			+ QString("<p><font size=\"3\">\nImage/Audio Documentation: ") + mCHEInfo->documents + QString("\n</font></p>\n")
			+ QString("<p><font size=\"3\">\nOthers: ") + mCHEInfo->other + QString("\n</font></p>\n");
	}
	for (int i = 0; i < mObjects.size(); i++)
	{
		html = html + QString("<br></br>");
		html = html + QString("<p><font size=\"5\">\nObject " +  QString::number(i+1) + QString(": ") + mObjects[i]->mName + QString("\n</font></p>\n"));
		QString annotation;
		QVector<QPair<QString, NoteMode> > contents;
		QVector<QPair<int, int> > pointNote2D;
		QVector<int*> surfaceNote2D;
		QVector<double*> pointNote3D;
		QVector<double*> surfaceNote3D;
		QVector<double*> frustumNote3D;
		//Parsing Notes
		for (int j = 0; j < mObjects[i]->mNotes.size(); j++)
		{
			QString note = mObjects[i]->mNotes[j];
			if (j == 0)
				annotation = note;
			else
			{
				NoteMode mode;
				if (mObjects[i]->mMode == IMAGE2D)
				{
					QStringList lines = note.split("\n");
					QString firstLine = lines[0];
					if (firstLine.split(" ")[0] == QString("Point"))
					{
						bool ok0, ok1;
						int x = firstLine.split(" ")[8].split(",")[0].split("(")[1].toInt(&ok0);
						int y = firstLine.split(" ")[9].split(")")[0].toInt(&ok1);
						if (!ok0 || !ok1)
						{
							qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
							continue;
						}
						pointNote2D.push_back(qMakePair(x, y));
						mode = POINTNOTE;
					}
					else if (firstLine.split(" ")[0] == QString("Surface"))
					{
						bool ok0, ok1, ok2, ok3;
						int* points = new int[4];
						points[0] = firstLine.split(" ")[11].split(",")[0].split("(")[1].toInt(&ok0);
						points[1] = firstLine.split(" ")[12].split(")")[0].toInt(&ok1);
						points[2] = firstLine.split(" ")[14].split(",")[0].split("(")[1].toInt(&ok2);
						points[3] = firstLine.split(" ")[15].split(")")[0].toInt(&ok3);
						
						if (!ok0 || !ok1 || !ok2 || !ok3)
						{
							qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
							continue;
						}
						surfaceNote2D.push_back(points);
						mode = SURFACENOTE;
					}
					else
					{
						qDebug()<<"Parsing Error in report generation: Note Type Error!"<<firstLine;
						continue;
					}
				}
				else if (mObjects[i]->mMode == MODEL3D)
				{
					QStringList lines = note.split("\n");
					QString firstLine = lines[0];
					if (firstLine.split(" ")[0] == QString("Point"))
					{
						bool ok0, ok1, ok2;
						double *worldPosition = new double[3];
						worldPosition[0] = firstLine.split(" ")[9].split(",")[0].toDouble(&ok0);
						worldPosition[1] = firstLine.split(" ")[11].split(",")[0].toDouble(&ok1);
						worldPosition[2] = firstLine.split(" ")[13].split(",")[0].toDouble(&ok2);
						if (!ok0 || !ok1 || !ok2)
						{
							qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
						}
						pointNote3D.push_back(worldPosition);
						mode = POINTNOTE;
					}
					else if (firstLine.split(" ")[0] == QString("Surface"))
					{
						bool ok0;
						int cellNum = firstLine.split(" ")[7].toInt(&ok0);
						if (!ok0)
						{
							qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
						}
						QTextStream in(&note);
					    while(1)
						{
							QString signal = in.readLine();
							if (signal == QString("Polygon Ids:"))
								break;
						}
						QVector<int> ids;
						for (int k = 0; k < cellNum; k++)
						{
							int cellId;
							in >> cellId;
							ids.push_back(cellId);
						}
						double *center = new double[3];
						computeCenter(mObjects[i]->mGla, ids, center);
						surfaceNote3D.push_back(center);
						mode = SURFACENOTE;
					}
					else if (firstLine.split(" ")[0] == QString("Frustum"))
					{
						QTextStream in(&note);
						vtkSmartPointer<vtkPoints> points  = vtkSmartPointer<vtkPoints>::New();
						vtkSmartPointer<vtkDoubleArray> normalArray = vtkSmartPointer<vtkDoubleArray>::New();
						vtkSmartPointer<vtkDataArray> normals = vtkDataArray::SafeDownCast(normalArray);
						in.readLine();
						for (int k = 0; k < 6; k++)
						{
							QString line = in.readLine();
							bool ok0, ok1, ok2;
							double pos[3];
							pos[0] = line.split(" ")[0].split("(")[1].split(",")[0].toDouble(&ok0);
							pos[1] = line.split(" ")[1].split(",")[0].toDouble(&ok1);
							pos[2] = line.split(" ")[2].split(")")[0].toDouble(&ok2);
							if (!ok0 || !ok1 || !ok2)
							{
								qDebug() << "The Syntax of Line is incorrect. The First Line is " << line;
							}
							points->InsertNextPoint(pos);
						}

						while(1)
						{
							QString signal = in.readLine();
							if (signal == QString("Normal Point of Each Plane:"))
								break;
						}
						normals->SetNumberOfComponents(3);
						normals->SetNumberOfTuples(6);
						for (int k = 0; k < 6; k++)
						{
							QString line = in.readLine();
							bool ok0, ok1, ok2;
							double pos[3];
							pos[0] = line.split(" ")[0].split("(")[1].split(",")[0].toDouble(&ok0);
							pos[1] = line.split(" ")[1].split(",")[0].toDouble(&ok1);
							pos[2] = line.split(" ")[2].split(")")[0].toDouble(&ok2);
							if (!ok0 || !ok1 || !ok2)
							{
								qDebug() << "The Syntax of input note is incorrect. The First Line is " << line;
							}
							normals->InsertComponent(k, 0, pos[0]);
							normals->InsertComponent(k, 1, pos[1]);
							normals->InsertComponent(k, 2, pos[2]);
						}
						

						normals->Resize(6);

						double *center = new double[3];
						computeCenter(mObjects[i]->mGla, points, normals, center);
						frustumNote3D.push_back(center);
						mode = FRUSTUMNOTE;
					}
					else
					{
						qDebug()<<"Parsing Error in report generation: Note Type Error!"<<firstLine;
						continue;
					}
				}
				int index = note.indexOf("Note Start:\n");
				if (index == -1)
					continue;
				note.remove(0, index+12);
				contents.push_back(qMakePair(note, mode));
			}
		}
		
		// create screenshot folder
		QString location = mLocation;
		location.truncate(location.lastIndexOf(QDir::separator()));
		QString tmp = location;
		tmp.append(QDir::separator() + QString("tmp"));
		QDir().mkdir(tmp);
		QDir tmpFolder(tmp);
		QString screenshot = tmp;
		screenshot.append(QDir::separator() + mObjects[i]->mName);
		QString screenshotDict;
		// create images for report
		QVector<QPair<double, double> > pointNote3DFront, pointNote3DLeft, pointNote3DRight, pointNote3DTop, pointNote3DBottom, pointNote3DBack;
		QVector<QPair<double, double> > surfaceNote3DFront, surfaceNote3DLeft, surfaceNote3DRight, surfaceNote3DTop, surfaceNote3DBottom, surfaceNote3DBack;
		QVector<QPair<double, double> > frustumNote3DFront, frustumNote3DLeft, frustumNote3DRight, frustumNote3DTop, frustumNote3DBottom, frustumNote3DBack;
		WidgetInfo3D info;
		switch(mObjects[i]->mMode)
		{
			case EMPTYWIDGET:
				break;
			case IMAGE2D:
				mObjects[i]->mPictures.push_back(mObjects[i]->mGla->mFilename);
				break;
			case MODEL3D:
				saveWidgetinfo(mObjects[i]->mGla, info);
				initWidget(mObjects[i]->mGla);
				// generate front, left and top screenshot	
				mObjects[i]->mGla->setOrthogonalView(FRONT3D);
				screenshotDict = screenshot;
				screenshotDict.append("_front");
				mObjects[i]->mPictures.push_back(mObjects[i]->mGla->screenshot(screenshotDict));
				detectPointVisibility(mObjects[i]->mGla->mRenderer, pointNote3D, pointNote3DFront);
				detectPointVisibility(mObjects[i]->mGla->mRenderer, surfaceNote3D, surfaceNote3DFront);
				detectPointVisibility(mObjects[i]->mGla->mRenderer, frustumNote3D, frustumNote3DFront);
				
				mObjects[i]->mGla->setOrthogonalView(LEFT3D);
				screenshotDict = screenshot;
				screenshotDict.append("_left");
				mObjects[i]->mPictures.push_back(mObjects[i]->mGla->screenshot(screenshotDict));
				detectPointVisibility(mObjects[i]->mGla->mRenderer, pointNote3D, pointNote3DLeft);
				detectPointVisibility(mObjects[i]->mGla->mRenderer, surfaceNote3D, surfaceNote3DLeft);
				detectPointVisibility(mObjects[i]->mGla->mRenderer, frustumNote3D, frustumNote3DLeft);

				mObjects[i]->mGla->setOrthogonalView(RIGHT3D);
				screenshotDict = screenshot;
				screenshotDict.append("_right");
				mObjects[i]->mPictures.push_back(mObjects[i]->mGla->screenshot(screenshotDict));
				detectPointVisibility(mObjects[i]->mGla->mRenderer, pointNote3D, pointNote3DRight);
				detectPointVisibility(mObjects[i]->mGla->mRenderer, surfaceNote3D, surfaceNote3DRight);
				detectPointVisibility(mObjects[i]->mGla->mRenderer, frustumNote3D, frustumNote3DRight);

				mObjects[i]->mGla->setOrthogonalView(TOP3D);
				screenshotDict = screenshot;
				screenshotDict.append("_top");
				mObjects[i]->mPictures.push_back(mObjects[i]->mGla->screenshot(screenshotDict));
				detectPointVisibility(mObjects[i]->mGla->mRenderer, pointNote3D, pointNote3DTop);
				detectPointVisibility(mObjects[i]->mGla->mRenderer, surfaceNote3D, surfaceNote3DTop);
				detectPointVisibility(mObjects[i]->mGla->mRenderer, frustumNote3D, frustumNote3DTop);

				mObjects[i]->mGla->setOrthogonalView(BOTTOM3D);
				screenshotDict = screenshot;
				screenshotDict.append("_bottom");
				mObjects[i]->mPictures.push_back(mObjects[i]->mGla->screenshot(screenshotDict));
				detectPointVisibility(mObjects[i]->mGla->mRenderer, pointNote3D, pointNote3DBottom);
				detectPointVisibility(mObjects[i]->mGla->mRenderer, surfaceNote3D, surfaceNote3DBottom);
				detectPointVisibility(mObjects[i]->mGla->mRenderer, frustumNote3D, frustumNote3DBottom);

				mObjects[i]->mGla->setOrthogonalView(BACK3D);
				screenshotDict = screenshot;
				screenshotDict.append("_back");
				mObjects[i]->mPictures.push_back(mObjects[i]->mGla->screenshot(screenshotDict));
				detectPointVisibility(mObjects[i]->mGla->mRenderer, pointNote3D, pointNote3DBack);
				detectPointVisibility(mObjects[i]->mGla->mRenderer, surfaceNote3D, surfaceNote3DBack);
				detectPointVisibility(mObjects[i]->mGla->mRenderer, frustumNote3D, frustumNote3DBack);
				recoverWidget(mObjects[i]->mGla, info);
				break;
			case CTSTACK:
				break;
			case CTVOLUME:      
				break;
			case RTI2D:
				break;
			default: break;
		}
		QDir().rmdir(tmp);

		//Process and add image
		for (int j = 0; j < mObjects[i]->mPictures.size(); j++)
		{
			QImage img(mObjects[i]->mPictures[j]); 
			if (img == QImage())
				continue;
			int imgHeight = img.height();
			int imgWidth = img.width();
			QStringList nameElements = mObjects[i]->mPictures[j].split(QDir::separator());
			QString url = nameElements[nameElements.size() - 1];

			QPainter painter(&img);
			QFont font = painter.font();
			int pointSize = font.pointSize();
			pointSize *= imgHeight*imgWidth / 600 / 400;
			font.setPointSize(pointSize);
			painter.setFont(font);
			painter.setRenderHint(QPainter::Antialiasing);
			painter.setPen(QPen(Qt::black, 20, Qt::DashDotLine, Qt::RoundCap));
			for (int k = 0; k < pointNote2D.size(); k++)
			{
				int x = pointNote2D[k].first, y = img.height() - pointNote2D[k].second;
				int x1 = x - pointSize/2, y1 = y - pointSize/2;
				if (x1 <= 0)
					x1 = 1;
				else if (x1 + pointSize >= imgWidth)
					x1 = imgWidth - pointSize - 1;
				if (y1 <= 0)
					y1 = 1;
				else if (y1 + pointSize >= imgHeight)
					y1 = imgHeight - pointSize - 1;

				QRectF rectangle(x1, y1, pointSize, pointSize);
				painter.setPen(QPen(Qt::red, 4));
				painter.drawRect(rectangle);
				painter.setPen(QPen(Qt::black, 4));
				painter.drawText(rectangle,  Qt::AlignCenter, QString::number(k+1));
			}
			
			for (int k = 0; k < surfaceNote2D.size(); k++)
			{
				int x1 = surfaceNote2D[k][0];
				int y1 = surfaceNote2D[k][1];
				int x2 = surfaceNote2D[k][2];
				int y2 = surfaceNote2D[k][3];
				int width = abs(x2 - x1);
				int height = abs(y2 - y1);
				y1 = img.height() - y1;
				QRectF rectangle(x1, y1, width, height);
				painter.setPen(QPen(Qt::red, 4));
				painter.drawRect(rectangle);
				painter.setPen(QPen(Qt::black, 4));
				painter.drawText(rectangle,  Qt::AlignCenter, QString::number(k+pointNote2D.size()+1));
			}
			font = painter.font();
			pointSize = pointSize / 1.2;
			if (pointSize == 0)	pointSize = 1;
			font.setPointSize(pointSize);
			painter.setFont(font);
			QVector<QPair<double, double> > pointNote3DSelected, surfaceNote3DSelected, frustumNote3DSelected;
			switch(j)
			{
				case 0: 
					pointNote3DSelected = pointNote3DFront;
					surfaceNote3DSelected = surfaceNote3DFront;
					frustumNote3DSelected = frustumNote3DFront;
					break;
				case 1: 
					pointNote3DSelected = pointNote3DLeft;
					surfaceNote3DSelected = surfaceNote3DLeft;
					frustumNote3DSelected = frustumNote3DLeft;
					break;
				case 2: 
					pointNote3DSelected = pointNote3DRight;
					surfaceNote3DSelected = surfaceNote3DRight;
					frustumNote3DSelected = frustumNote3DRight;
					break;
				case 3: 
					pointNote3DSelected = pointNote3DTop;
					surfaceNote3DSelected = surfaceNote3DTop;
					frustumNote3DSelected = frustumNote3DTop;
					break;
				case 4: 
					pointNote3DSelected = pointNote3DBottom;
					surfaceNote3DSelected = surfaceNote3DBottom;
					frustumNote3DSelected = frustumNote3DBottom;
					break;
				case 5: 
					pointNote3DSelected = pointNote3DBack;
					surfaceNote3DSelected = surfaceNote3DBack;
					frustumNote3DSelected = frustumNote3DBack;
					break;
				default:break;
			}

			for (int k = 0; k < pointNote3DSelected.size(); k++)
			{
				int x = pointNote3DSelected[k].first, y = img.height() - pointNote3DSelected[k].second;
				if (x == -1 || y == -1)
					continue;
				int x1 = x - pointSize/2, y1 = y - pointSize/2;
				if (x1 <= 0)
					x1 = 1;
				else if (x1 + pointSize >= imgWidth)
					x1 = imgWidth - pointSize - 1;
				if (y1 <= 0)
					y1 = 1;
				else if (y1 + pointSize >= imgHeight)
					y1 = imgHeight - pointSize - 1;

				QRectF rectangle(x1, y1, pointSize, pointSize);
				painter.setPen(QPen(Qt::red, 4));
				painter.drawRect(rectangle);
				painter.setPen(QPen(Qt::black, 4));
				painter.drawText(rectangle,  Qt::AlignCenter, QString::number(k+1));
			}
			for (int k = 0; k < surfaceNote3DSelected.size(); k++)
			{
				int x = surfaceNote3DSelected[k].first, y = img.height() - surfaceNote3DSelected[k].second;
				if (x == -1 || y == -1)
					continue;
				int x1 = x - pointSize/2, y1 = y - pointSize/2;
				if (x1 <= 0)
					x1 = 1;
				else if (x1 + pointSize >= imgWidth)
					x1 = imgWidth - pointSize - 1;
				if (y1 <= 0)
					y1 = 1;
				else if (y1 + pointSize >= imgHeight)
					y1 = imgHeight - pointSize - 1;

				QRectF rectangle(x1, y1, pointSize, pointSize);
				painter.setPen(QPen(Qt::red, 4));
				painter.drawRect(rectangle);
				painter.setPen(QPen(Qt::black, 4));
				painter.drawText(rectangle,  Qt::AlignCenter, QString::number(k+1+pointNote3DSelected.size()));
			}
			for (int k = 0; k < frustumNote3DSelected.size(); k++)
			{
				int x = frustumNote3DSelected[k].first, y = img.height() - frustumNote3DSelected[k].second;
				qDebug()<<"frustum pixel"<<x<<y;
				if (x == -1 || y == -1)
					continue;
				int x1 = x - pointSize/2, y1 = y - pointSize/2;
				if (x1 <= 0)
					x1 = 1;
				else if (x1 + pointSize >= imgWidth)
					x1 = imgWidth - pointSize - 1;
				if (y1 <= 0)
					y1 = 1;
				else if (y1 + pointSize >= imgHeight)
					y1 = imgHeight - pointSize - 1;

				QRectF rectangle(x1, y1, pointSize, pointSize);
				painter.setPen(QPen(Qt::red, 4));
				painter.drawRect(rectangle);
				painter.setPen(QPen(Qt::black, 4));
				painter.drawText(rectangle,  Qt::AlignCenter, QString::number(k+1+pointNote3DSelected.size()+surfaceNote3DSelected.size()));
			}
			
			painter.end(); 
		    mDoc->addResource(QTextDocument::ImageResource, QUrl(url), img);
			if (mObjects[i]->mMode == IMAGE2D)
			{
				double height = 200, width = 300;
				height = (double)width * imgHeight / imgWidth;
				html = html + QString("<p><div align=\"center\"><img src=\"" + url + "\"width=\"" + QString::number(width) + "\" height=\"" + QString::number(height) + "\"></div></p>\n");
			}
			else if (mObjects[i]->mMode == MODEL3D)
			{
				double height = 100, width = 150;
				height = (double)width * imgHeight / imgWidth;
				if (j == 0)
					html += QString("<p><div align=\"center\"><img src=\"" + url + "\"width=\"" + QString::number(width) + "\" height=\"" + QString::number(height) + "\">");
				else if (j == mObjects[i]->mPictures.size() - 1)
					html += QString("<img src=\"" + url + "\"width=\"" + QString::number(width) + "\" height=\"" + QString::number(height) + "\"></div></p>\n");
				else
					html += QString("<img src=\"" + url + "\"width=\"" + QString::number(width) + "\" height=\"" + QString::number(height) + "\">");	
			}
		}
		//Add annotation and notes
		if (!annotation.isEmpty())
			html = html + QString("<p><font size=\"3\">\nAnnotation " + QString(": ") + annotation + QString("\n</font></p>\n"));
		for (int j = 0; j < contents.size(); j++)
		{
			html += QString("<p><font size=\"3\">\n");
			switch(contents[j].second)
			{
				case POINTNOTE:	html += QString("Point Note "); break;
				case SURFACENOTE:	html += QString("Surface Note "); break;
				case FRUSTUMNOTE:	html += QString("Frustum Note "); break;
				default: continue;
			}
			html += QString(QString::number(j+1) + QString(": ") + contents[j].first + QString("\n</font></p>\n"));
		}
		for (int j = 0; j < surfaceNote2D.size(); j++)
		{
			delete surfaceNote2D[j];
		}
		for (int j = 0; j < pointNote3D.size(); j++)
		{
			delete pointNote3D[j];
		}
		for (int j = 0; j < surfaceNote3D.size(); j++)
		{
			delete surfaceNote3D[j];
		}
		
	}
	mDoc->setHtml(html);
	mDoc->setPageSize(mPrinter->pageRect().size()); // This is necessary if you want to hide the page number
    mDoc->print(mPrinter);
}

void ReportGenerator::detectPointVisibility(vtkSmartPointer<vtkRenderer> render, QVector<double*> points, QVector<QPair<double, double> >& visiblePoints)
{
	for (int i = 0; i < points.size(); i++)
	{
		vtkSmartPointer<vtkPoints> point = vtkSmartPointer<vtkPoints>::New();
		vtkSmartPointer<vtkPolyData> pointData = vtkSmartPointer<vtkPolyData>::New();
		point->SetNumberOfPoints(1);
		point->SetPoint(0, points[i][0], points[i][1], points[i][2]);
		pointData->SetPoints(point);
		vtkSmartPointer<vtkSelectVisiblePoints> selectVisiblePoints = vtkSmartPointer<vtkSelectVisiblePoints>::New();
		selectVisiblePoints->SetInput(pointData);
		selectVisiblePoints->SetRenderer(render);
		selectVisiblePoints->Update();
		if (selectVisiblePoints->GetOutput()->GetNumberOfPoints() != 0)
		{
			double *displayPt = new double[3];
			vtkInteractorObserver::ComputeWorldToDisplay (render, points[i][0], points[i][1], points[i][2], displayPt);
			visiblePoints.push_back(qMakePair(displayPt[0], displayPt[1]));
			delete displayPt;
		}
		else
		{
			double x = -1;
			visiblePoints.push_back(qMakePair(x, x));
		}
	}
}

MainWindow* ReportGenerator::mw()
{
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
    if (mainwindow)
    {
      return mainwindow;
    }
  }
  return NULL;
}

void ReportGenerator::saveWidgetinfo(const VtkWidget* gla, WidgetInfo3D &info)
{
	info.infoStatus = gla->getDisplayInfoOn();
	info.isMeasuring = gla->getUseRubberband();
	info.isAnnotation = gla->getUserAnnotationOn();
	vtkSmartPointer<vtkCamera> camera = gla->mRenderer->GetActiveCamera();
	camera->GetPosition(info.pos);
	camera->GetFocalPoint(info.foc);
	info.angle = camera->GetViewAngle();
	camera->GetViewUp(info.view);
	camera->GetClippingRange(info.clip);
	info.scale = camera->GetParallelScale();
	info.isDirectionalLight = gla->getIsDirectionalLight();

	vtkSmartPointer<vtkAssembly> assembly = mw()->mLightControl->GetAssembly();
	assembly->GetOrientation(info.orientation);

	info.brightness = mw()->mLightControl->GetIntensityL1();
	info.contrast = mw()->mLightControl->GetIntensityL2();
	info.renderMode = gla->getRenderMode3D();
	if (gla->getIsInterpolationOn()) 
		info.interpolation = 1;
	else if(!gla->getmRgbTextureFilename().isEmpty())
        info.interpolation = 0;
	else
		info.interpolation = -1;

	if(gla->getIsTextureOn()) 
		info.textureOn = 1;
	else if (!gla->getmRgbTextureFilename().isEmpty())
		info.textureOn = 0;
	else
		info.textureOn = -1;
}

void ReportGenerator::initWidget(VtkWidget* gla)
{
	gla->setDisplayInfoOn(false);
	mw()->mLightControl->reset();
	if (gla->getIsTextureOn())
	{
		gla->setTextureOn(true);
	}
	gla->setRenderMode3D(SURFACE3D); 
	gla->setVisibilityDistance(false);
	gla->annotate(true);
}

void ReportGenerator::recoverWidget(VtkWidget* gla, WidgetInfo3D info)
{
	gla->setDisplayInfoOn(info.infoStatus);
	vtkSmartPointer<vtkCamera> camera = gla->mRenderer->GetActiveCamera();
	camera->SetPosition(info.pos);
	camera->SetFocalPoint(info.foc);
	camera->SetViewAngle(info.angle);
	camera->SetViewUp(info.view);
	camera->SetClippingRange(info.clip);
	camera->SetParallelScale(info.scale);
	if(info.isDirectionalLight) 
		mw()->mLightControl->setDirectionalLight(true); 
	else 
		mw()->mLightControl->setDirectionalLight(false);

	if(info.textureOn == 1) 
		gla->setTextureOn(true);
	else if (info.textureOn == 0)
        gla->setTextureOn(false);

	if(info.interpolation == 1) 
		gla->setInterpolateOn(true);
	else if (info.interpolation == 0)
        gla->setInterpolateOn(false);

	mw()->mLightControl->restoreBookmarkLight(info.orientation, info.brightness, info.contrast, MODEL3D);
   
	gla->setRenderMode3D(info.renderMode);
	gla->setMeasureDistance(info.isMeasuring);
	gla->annotate(info.isAnnotation);
	if(gla->mQVTKWidget) gla->mQVTKWidget->update();
}

void ReportGenerator::computeCenter(const VtkWidget* gla, QVector<int> cellIds, double* center)
{
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	for (int i = 0; i < cellIds.size(); i++)
	{
		vtkSmartPointer<vtkIdList> ptIds = vtkSmartPointer<vtkIdList>::New();
		gla->get3DPolyData()->GetCellPoints(cellIds[i], ptIds);
		for (int j = 0; j < ptIds->GetNumberOfIds(); j++)
		{
			double point[3];
			gla->get3DPolyData()->GetPoint(ptIds->GetId(j), point);
			points->InsertPoint(points->GetNumberOfPoints(), point);
		}
	}
	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);
	/*
	double center[3];
	polyData->ComputeBounds();
	polyData->GetCenter(center);
	qDebug()<<"bounding box "<<center[0]<<center[1]<<center[2];*/

	vtkSmartPointer<vtkCenterOfMass> centerOfMassFilter = vtkSmartPointer<vtkCenterOfMass>::New();
	#if VTK_MAJOR_VERSION <= 5
	centerOfMassFilter->SetInput(polyData);
	#else
	centerOfMassFilter->SetInputData(polyData);
	#endif
	centerOfMassFilter->SetUseScalarsAsWeights(false);
	centerOfMassFilter->Update();

	double CoM[3];
	centerOfMassFilter->GetCenter(CoM);
	vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();
	
	cellLocator->SetDataSet(gla->get3DPolyData());
	cellLocator->BuildLocator();
	double closestPointDist2;
	vtkIdType cellId;
	int subId;
	cellLocator->FindClosestPoint(CoM, center, cellId, subId, closestPointDist2);
}

void ReportGenerator::computeCenter(const VtkWidget* gla, vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals, double* center)
{
	vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkDataArray> newNormals = normals;
	vtkSmartPointer<vtkPlanes> frustum = vtkSmartPointer<vtkPlanes>::New();
	newPoints->DeepCopy(points);
	newNormals->DeepCopy(normals);
	
	frustum->SetPoints(newPoints);
	frustum->SetNormals(newNormals);
	vtkSmartPointer<vtkExtractSelectedFrustum> extractor = vtkSmartPointer<vtkExtractSelectedFrustum>::New();
	extractor->SetInput(gla->get3DPolyData());
	extractor->PreserveTopologyOff();
	extractor->SetFrustum(frustum);
	extractor->Update();

	//vtkSmartPointer<vtkDataSet> polyData = vtkSmartPointer<vtkDataSet>::New();
	vtkDataSet* polyData  = vtkDataSet::SafeDownCast(extractor->GetOutput());
	qDebug()<<"selected points"<<polyData->GetNumberOfPoints();
	vtkSmartPointer<vtkCenterOfMass> centerOfMassFilter = vtkSmartPointer<vtkCenterOfMass>::New();
	#if VTK_MAJOR_VERSION <= 5
	centerOfMassFilter->SetInput(polyData);
	#else
	centerOfMassFilter->SetInputData(polyData);
	#endif
	centerOfMassFilter->SetUseScalarsAsWeights(false);
	centerOfMassFilter->Update();

	double CoM[3];
	centerOfMassFilter->GetCenter(CoM);
	vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();
	
	cellLocator->SetDataSet(gla->get3DPolyData());
	cellLocator->BuildLocator();
	double closestPointDist2;
	vtkIdType cellId;
	int subId;
	cellLocator->FindClosestPoint(CoM, center, cellId, subId, closestPointDist2);
	qDebug()<<"center"<<center[0]<<center[1]<<center[2];
}
