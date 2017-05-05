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
#include <vtkSelectVisiblePoints.h>
#include <vtkInteractorObserver.h>
#include <vtkIdList.h>
#include <vtkCenterOfMass.h>
#include <vtkCellLocator.h>
#include <vtkCleanPolyData.h>
#include <vtkPlanes.h>

#include <vtkDoubleArray.h>
#include <vtkCamera.h>
#include <vtkOBBTree.h>
#include <vtkSelectPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkCellArray.h>
#include <vtkMath.h>
#include <vtkStructuredPoints.h>

#include <limits>
#include <qwt_plot.h>
#include "videoGenerator.h"
#include "lightControl.h"
#include "../information/searchWidget.h"
#include "../mainWindow.h"
#include "CTControl.h"


std::string color2categoryFullName_(const std::string str)
{
	if (str == "MAROON")
		return "Object / Work";
	else if (str == "RED")
		return "Physical Dimensions / Measurements"; 
	else if (str == "ORANGE")
		return "Creation";
	else if (str == "YELLOW")
		return "Materials and Techniques";
	else if (str == "LIME")
		return "Stylistic Analysis and Descriptions";
	else if (str == "GREEN")
		return "Condition and Conservation";
	else if (str == "AQUA")
		return "Analyses";
	else if (str == "BLUE")
		return "Related Works";
	else if (str == "PINK")
		return "Exhibition / Loans and Legal Issues";
	else if (str == "PURPLE")
		return "Image/Audio Documentation";
	else if (str == "WHITE")
		return "Other";
	else
		return "Other";
}

VideoGenerator::VideoGenerator(QString path, bool project)
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

	if (QFileInfo(mLocation).suffix() == QString("wmv")) isWmv = true;
	else isWmv = false;
}

void VideoGenerator::setUpdateSetup(const QString project, const QString userName, const QString affiliation, const int videoFormat, const int resolutionOption,
		const int frameDuration2D, const int transDuration2D, const int frameDuration3D, const int transDuration3D, const int dolly3D, const bool isShowGeneral)
{
	mProjectName = project;
	mUserName = userName;
	mAffiliation = affiliation;
	mVideoFormat = videoFormat;
	mResolutionOption = resolutionOption;
	mFrameDuration2D = frameDuration2D;
	mTransDuration2D = transDuration2D;
	mFrameDuration3D = frameDuration3D;
	mTransDuration3D = transDuration3D;
	mDolly3D = dolly3D;
	mShowGeneral = isShowGeneral;
}

void VideoGenerator::setCHEInfo(const CHEInfoBasic* info)
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

void VideoGenerator::generate()
{
	// create screenshot folder
	QString location = mLocation;
	location.truncate(location.lastIndexOf(QDir::separator()));
	QString videoFolder = location;
	videoFolder.append(QDir::separator() + QString("video"));
	QDir().mkdir(videoFolder);
	QDir videoFolderDir(videoFolder);
	cv::Size mysize(800, 600);
	cv::VideoWriter outputVideo(mLocation.toStdString(), cv::VideoWriter::fourcc('D','I','V','3'), 30, mysize, true); // SIZE ISSUE

	// create title frame
	cv::Mat titleFrame = cv::Mat::zeros(mysize, CV_8UC3);
	cv::putText(titleFrame, mProjectName.toStdString(), cv::Point(50, 200), cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(255, 255, 255), 1, CV_AA);
	cv::putText(titleFrame, mUserName.toStdString(), cv::Point(50, 350), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(255, 255, 255), 1, CV_AA);
	if (mAffiliation != "")
	{
		cv::putText(titleFrame, mAffiliation.toStdString(), cv::Point(50, 400), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(255, 255, 255), 1, CV_AA);
	}
	cv::putText(titleFrame, "Powered by CHER-Ob", cv::Point(550, 550), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(255, 255, 255), 1, CV_AA);
	for (int duration = 0; duration < 30*mFrameDuration2D; duration++)
	{
		if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
		outputVideo.write(titleFrame);
	}

	bool isCTModeSwitched = false;
	for (int i = 0; i < mObjects.size(); i++)
	{
		QString annotation;
		QVector<QPair<QString, NoteMode> > contents;
		QVector<QPair<QPair<int, int>, QString> > pointNote2D;
		QVector<QPair<int*, QString> > surfaceNote2D;
		QVector<QPair<QVector<QPair<int, int> >, QString> > polygonNote2D;
		QVector<QPair<QPair<int, double*>, QString> > pointNote3D;
		QVector<QPair<double*, CTSurfaceCornerPoint_> > surfaceNote3D_CT;
		QVector<QPair<QPair<int, double*>, QString> > surfaceNote3D;
		QVector<QPair<double*, QString> > frustumNote3D;
		QVector<vtkSmartPointer<vtkDataSet> > dataset;
		if (mObjects[i]->mMode == CTSTACK)
		{
			mw()->mCtControl->setVTvolumeGPU();
			isCTModeSwitched = true;
		}
		//Parsing Notes
		for (int j = 0; j < mObjects[i]->mNotes.size(); j++)
		{
			QString note = mObjects[i]->mNotes[j].first;
			if (j == 0)
				annotation = note;
			else
			{
				int index = note.indexOf("\nColor Type:\n");
				if (index == -1)
					continue;
				QString color = note.split("\nColor Type:\n")[1].split("\n")[0];
				int type = color2type(color.toStdString());
				if (mObjects[i]->mCategories.indexOf(type) == -1)
					continue;
				NoteMode mode;
				if ((mObjects[i]->mMode == IMAGE2D || mObjects[i]->mMode == RTI2D) && mObjects[i]->mNotes[j].second == NOTE2D)
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
						note.remove(0, index+13); // remove note header for the convenience of further parsing
						pointNote2D.push_back(qMakePair(qMakePair(x, y), note));
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
						note.remove(0, index+13); // remove note header for the convenience of further parsing
						surfaceNote2D.push_back(qMakePair(points, note));
						mode = SURFACENOTE;
					}
					else if (firstLine.split(" ")[0] == QString("Polygon"))
					{
						QVector<QPair<int, int> > polygon;
						bool okNum;
						int num = firstLine.split(" ")[4].toInt(&okNum);
						if (!okNum)
						{
							qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
							continue;
						}
						for (int it = 1; it <= num; it++)
						{
							bool okImgX, okImgY;
							int posImageX, posImageY;
							posImageX = firstLine.split(" ")[2*it + 2*num + 6].split(",")[0].split("(")[1].toInt(&okImgX);
							posImageY = firstLine.split(" ")[2*it + 2*num + 7].split(")")[0].toInt(&okImgY);
							if (!okImgX || !okImgY)
							{
								qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
								continue;
							} 
							polygon.push_back(qMakePair(posImageX, posImageY));
						}
						note.remove(0, index+13); // remove note header for the convenience of further parsing
						polygonNote2D.push_back(qMakePair(polygon, note));
						mode = POLYGONNOTE;
					}
					else
					{
						qDebug() << "Parsing Error in video generation: Note Type Error!" << firstLine;
						continue;
					}
					contents.push_back(qMakePair(note, mode));
				}
				else if (mObjects[i]->mMode == MODEL3D && mObjects[i]->mNotes[j].second == NOTE3D)
				{
					QStringList lines = note.split("\n");
					QString firstLine = lines[0];
					if (firstLine.split(" ")[0] == QString("Point"))
					{
						bool okID, ok0, ok1, ok2;
						int polygonID;
						double *worldPosition = new double[3];
						polygonID = firstLine.split(" ")[4].split(")")[0].split("(")[1].toInt(&okID);
						worldPosition[0] = firstLine.split(" ")[6].split(",")[0].split("(")[1].toDouble(&ok0);
						worldPosition[1] = firstLine.split(" ")[7].split(",")[0].toDouble(&ok1);
						worldPosition[2] = firstLine.split(" ")[8].split(")")[0].toDouble(&ok2);
						if (!okID || !ok0 || !ok1 || !ok2)
						{
							qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
						}
						note.remove(0, index+13); // remove note header for the convenience of further parsing
						pointNote3D.push_back(qMakePair(qMakePair(polygonID, worldPosition), note));
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
						vtkIdType centerCellId = 0;
						computeCenter(mObjects[i]->mGla, ids, center, &centerCellId);
						note.remove(0, index+13); // remove note header for the convenience of further parsing
						surfaceNote3D.push_back(qMakePair(qMakePair((int)centerCellId, center), note));
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
						vtkSmartPointer<vtkDataSet> data = vtkSmartPointer<vtkStructuredPoints>::New(); 
						computeCenter(mObjects[i]->mGla, points, normals, center, data);
						dataset.push_back(data);
						note.remove(0, index+13); // remove note header for the convenience of further parsing
						frustumNote3D.push_back(qMakePair(center, note));
						mode = FRUSTUMNOTE;
					}
					else
					{
						qDebug()<<"Parsing Error in video generation: Note Type Error!"<<firstLine;
						continue;
					}
					contents.push_back(qMakePair(note, mode));
				}
				else if ((mObjects[i]->mMode == CTSTACK || mObjects[i]->mMode == CTVOLUME) && mObjects[i]->mNotes[j].second == NOTE3D)
				{
					// CTSTACK will be switched to CTVOLUME and only 3D notes will be appiled.
					QStringList lines = note.split("\n");
					QString firstLine = lines[0];
					if (firstLine.split(" ")[0] == QString("Point"))
					{
						bool okID, ok0, ok1, ok2;
						int polygonID;
						double *worldPosition = new double[3];
						polygonID = firstLine.split(" ")[4].split(")")[0].split("(")[1].toInt(&okID);
						worldPosition[0] = firstLine.split(" ")[6].split(",")[0].split("(")[1].toDouble(&ok0);
						worldPosition[1] = firstLine.split(" ")[7].split(",")[0].toDouble(&ok1);
						worldPosition[2] = firstLine.split(" ")[8].split(")")[0].toDouble(&ok2);
						if (!okID || !ok0 || !ok1 || !ok2)
						{
							qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
						}
						note.remove(0, index+13); // remove note header for the convenience of further parsing
						pointNote3D.push_back(qMakePair(qMakePair(polygonID, worldPosition), note));
						mode = POINTNOTE;
					}
					else if (firstLine.split(" ")[0] == QString("Surface"))
					{
						bool ok0, ok1, ok2;
						QTextStream in(&note);
					    while(!in.atEnd())
						{
							QString signal = in.readLine();
							if (signal == QString("Corner Points:"))
								break;
						}
						bool isParsingSuccess = true;
						CTSurfaceCornerPoint_ cornerPoints;
						for (int i = 0; i < 4; i++)
						{
							QString pointLine = in.readLine();
							cornerPoints.point[i][0] = pointLine.split(" ")[0].split(",")[0].split("(")[1].toDouble(&ok0);
							cornerPoints.point[i][1] = pointLine.split(" ")[1].split(",")[0].toDouble(&ok1);
							cornerPoints.point[i][2] = pointLine.split(" ")[2].split(")")[0].toDouble(&ok2);
							if (!ok0 || !ok1 || !ok2)
							{
								qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
								isParsingSuccess = false;
								break;
							}
						}
						if (!isParsingSuccess)
							continue;

						double *center = new double[3];
						computeCenter(cornerPoints, center);
						surfaceNote3D_CT.push_back(qMakePair(center, cornerPoints));
						mode = SURFACENOTE;
					}
					else
					{
						qDebug()<<"Parsing Error in video generation: Note Type Error!"<<firstLine;
						continue;
					}
					//note.remove(0, index+13);	// Remove note header for the convenience of further parsing
					contents.push_back(qMakePair(note, mode));
				}
				
			}
		}
		
		QString screenshotObj = videoFolder;
		screenshotObj.append(QDir::separator() + mObjects[i]->mName);
		QString screenshotDict;
		// create images for video //// READ THIS
		QVector<QPair<double, double> > pointNote3DFront, pointNote3DLeft, pointNote3DRight, pointNote3DTop, pointNote3DBottom, pointNote3DBack;
		QVector<QPair<double, double> > surfaceNote3DFront, surfaceNote3DLeft, surfaceNote3DRight, surfaceNote3DTop, surfaceNote3DBottom, surfaceNote3DBack;
		QVector<QPair<double*, CTSurfaceCornerPoint_> > surfaceNote3DFront_CT, surfaceNote3DLeft_CT, surfaceNote3DRight_CT,
			surfaceNote3DTop_CT, surfaceNote3DBottom_CT, surfaceNote3DBack_CT;
		QVector<QPair<double, double> > frustumNote3DFront, frustumNote3DLeft, frustumNote3DRight, frustumNote3DTop, frustumNote3DBottom, frustumNote3DBack;
		WidgetInfo3D_ info;
		QPixmap RTIScreenShot;
	
		switch(mObjects[i]->mMode)
		{
			case EMPTYWIDGET:
				break;
			case IMAGE2D:
				{
	// std::vector<int> compression_params;
    // compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    // compression_params.push_back(9);
	// cv::imwrite("C:\\Users\\zw274\\Desktop\\zw274\\CHER-Ob-Longmen\\test.png", debugImg, compression_params);

					mObjects[i]->mPictures.push_back(mObjects[i]->mGla->mFilename);
					cv::Mat frame = cv::imread(mObjects[i]->mGla->mFilename.toStdString(), CV_LOAD_IMAGE_COLOR);
					cv::Mat prevFrame(mysize, CV_8UC3, cv::Scalar(0, 0, 0)), currFrame;

					// put general annotation, effect to be refined
					cv::Mat resized = resize2Video(frame, mysize);
					screenshotDict = screenshotObj;
					screenshotDict.append("_geo.png");
					//// TODO: need to set google map JavaScript to relocate current object
					mw()->mGeoInfo->makeScreenshot(screenshotDict); // save google map screenshot
					currFrame = putSubtitle(resized, annotation.toStdString(), mysize, screenshotDict.toStdString());
					blend2Video(prevFrame, currFrame, outputVideo);
					for (int duration = 0; duration < 30*mFrameDuration2D; duration++)
					{
						if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
						outputVideo.write(currFrame);
					}
					for (int noteid = 0; noteid < pointNote2D.size(); noteid++)
					{
						cv::Point notePos(pointNote2D[noteid].first.first, frame.size().height - pointNote2D[noteid].first.second);
						cv::Mat currNote = emphasizeNote(frame, notePos, 20);
						// put subtitle and associated image
						QPair<QString, QString> textAndImg = parseTextAndImg(pointNote2D[noteid].second);
						prevFrame = currFrame;
						currFrame = resize2Video(currNote, mysize);
						currFrame = putSubtitle(currFrame, textAndImg.first.toStdString(), mysize, textAndImg.second.toStdString());
						blend2Video(prevFrame, currFrame, outputVideo);
						for (int duration = 0; duration < 30*mFrameDuration2D; duration++)
						{
							if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
							outputVideo.write(currFrame);
						}
					}
					for (int noteid = 0; noteid < surfaceNote2D.size(); noteid++)
					{
						cv::Rect myroi;
						myroi.x = std::min(surfaceNote2D[noteid].first[0], surfaceNote2D[noteid].first[2]);
						myroi.y = frame.size().height - std::max(surfaceNote2D[noteid].first[1], surfaceNote2D[noteid].first[3]);
						myroi.width = abs(surfaceNote2D[noteid].first[0] - surfaceNote2D[noteid].first[2]);
						myroi.height = abs(surfaceNote2D[noteid].first[1] - surfaceNote2D[noteid].first[3]);
						cv::Mat currNote = emphasizeNote(frame, myroi);
						// put subtitle and associated image
						QPair<QString, QString> textAndImg = parseTextAndImg(surfaceNote2D[noteid].second);
						prevFrame = currFrame;
						currFrame = resize2Video(currNote, mysize);
						currFrame = putSubtitle(currFrame, textAndImg.first.toStdString(), mysize, textAndImg.second.toStdString());
						blend2Video(prevFrame, currFrame, outputVideo);
						for (int duration = 0; duration < 30*mFrameDuration2D; duration++)
						{
							if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
							outputVideo.write(currFrame);
						}
					}
					for (int noteid = 0; noteid < polygonNote2D.size(); noteid++)
					{
						cv::Point center(0, 0); 
						for (int vid = 0; vid < polygonNote2D[noteid].first.size() - 1; vid++)
						{
							center.x += polygonNote2D[noteid].first[vid].first;
							center.y += polygonNote2D[noteid].first[vid].second;
						}
						center.x = center.x / (polygonNote2D[noteid].first.size() - 1);
						center.y = center.y / (polygonNote2D[noteid].first.size() - 1);
						int radius = 20;
						for (int vid = 0; vid < polygonNote2D[noteid].first.size() - 1; vid++)
						{
							int dx2 = (center.x - polygonNote2D[noteid].first[vid].first) * (center.x - polygonNote2D[noteid].first[vid].first);
							int dy2 = (center.y - polygonNote2D[noteid].first[vid].second) * (center.y - polygonNote2D[noteid].first[vid].second);
							int tmpR = (int)sqrt((double)(dx2 + dy2));
							radius = std::max(radius, tmpR);
						}
						center.y = frame.size().height - center.y;
						cv::Mat currNote = emphasizeNote(frame, center, radius);
						// put subtitle and associated image
						QPair<QString, QString> textAndImg = parseTextAndImg(polygonNote2D[noteid].second);
						prevFrame = currFrame;
						currFrame = resize2Video(currNote, mysize);
						currFrame = putSubtitle(currFrame, textAndImg.first.toStdString(), mysize, textAndImg.second.toStdString());
						blend2Video(prevFrame, currFrame, outputVideo);
						for (int duration = 0; duration < 30*mFrameDuration2D; duration++)
						{
							if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
							outputVideo.write(currFrame);
						}
					}
					break;
				}
			case MODEL3D:
				{
					saveWidgetinfo(mObjects[i]->mGla, info);
					initWidget(mObjects[i]->mGla, false);
					cv::Mat prevFrame(mysize, CV_8UC3, cv::Scalar(0, 0, 0)), currFrame;
					double prevCam[6], currCam[6]; // 0..2 camera position x y z, 3..5 camera focal point x y z
					// generate screenshots from different angles
					for (int angle = 0; angle < 360; angle++)
					{
						mObjects[i]->mGla->setArbitraryView((double)angle);
						screenshotDict = screenshotObj;
						screenshotDict.append(QString::number(angle));
						mObjects[i]->mPictures.push_back(mObjects[i]->mGla->screenshot(screenshotDict));
						cv::Mat frame = cv::imread(screenshotDict.toStdString() + ".png", CV_LOAD_IMAGE_COLOR);
						cv::Mat resized = resize2Video(frame, mysize);
						currFrame = putSubtitle(resized, annotation.toStdString(), mysize);
						// blending
						if (angle == 0)
							blend2Video(prevFrame, currFrame, outputVideo);
						if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
						outputVideo.write(currFrame);
					}
					mObjects[i]->mGla->getCameraPos(currCam);
					mObjects[i]->mGla->computeNormals3D();
					for (int noteid = 0; noteid < pointNote3D.size(); noteid++)
					{
						prevCam[0] = currCam[0]; prevCam[1] = currCam[1]; prevCam[2] = currCam[2];
						prevCam[3] = currCam[3]; prevCam[4] = currCam[4]; prevCam[5] = currCam[5];
						mObjects[i]->mGla->setPointNoteView(pointNote3D[noteid].first.first,
							pointNote3D[noteid].first.second[0], pointNote3D[noteid].first.second[1], pointNote3D[noteid].first.second[2], mDolly3D);
						mObjects[i]->mGla->getCameraPos(currCam);
						for (int duration = 0; duration < 30*mTransDuration3D; duration++)
						{
							double tempCam[6];
							tempCam[0] = prevCam[0] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[0] * duration / (double)(30*mTransDuration3D);
							tempCam[1] = prevCam[1] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[1] * duration / (double)(30*mTransDuration3D);
							tempCam[2] = prevCam[2] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[2] * duration / (double)(30*mTransDuration3D);
							tempCam[3] = prevCam[3] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[3] * duration / (double)(30*mTransDuration3D);
							tempCam[4] = prevCam[4] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[4] * duration / (double)(30*mTransDuration3D);
							tempCam[5] = prevCam[5] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[5] * duration / (double)(30*mTransDuration3D);
							mObjects[i]->mGla->setCameraPos(tempCam);
							screenshotDict = screenshotObj;
							screenshotDict.append("PointNote" + QString::number(noteid) + "_" + QString::number(duration));
							mObjects[i]->mPictures.push_back(mObjects[i]->mGla->screenshot(screenshotDict));
							cv::Mat frame = cv::imread(screenshotDict.toStdString() + ".png", CV_LOAD_IMAGE_COLOR);
							cv::Mat resized = resize2Video(frame, mysize);
							if (!outputVideo.isOpened()) qDebug()  << "ERROR: outputVideo not opened!\n\n";
							outputVideo.write(resized);
						}
						mObjects[i]->mGla->setCameraPos(currCam);
						screenshotDict = screenshotObj;
						screenshotDict.append("PointNote" + QString::number(noteid));
						mObjects[i]->mPictures.push_back(mObjects[i]->mGla->screenshot(screenshotDict));
						cv::Mat frame = cv::imread(screenshotDict.toStdString() + ".png", CV_LOAD_IMAGE_COLOR);
						cv::Mat resized = resize2Video(frame, mysize);
						// put subtitle and associated image
						QPair<QString, QString> textAndImg = parseTextAndImg(pointNote3D[noteid].second);
						currFrame = putSubtitle(resized, textAndImg.first.toStdString(), mysize, textAndImg.second.toStdString());
						for (int duration = 0; duration < 30*mFrameDuration3D; duration++)
						{
							if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
							outputVideo.write(currFrame);
						}
					}
					for (int noteid = 0; noteid < surfaceNote3D.size(); noteid++)
					{
						prevCam[0] = currCam[0]; prevCam[1] = currCam[1]; prevCam[2] = currCam[2];
						prevCam[3] = currCam[3]; prevCam[4] = currCam[4]; prevCam[5] = currCam[5];
						mObjects[i]->mGla->setSurfaceNoteView(surfaceNote3D[noteid].first.first,
							surfaceNote3D[noteid].first.second[0], surfaceNote3D[noteid].first.second[1], surfaceNote3D[noteid].first.second[2], mDolly3D);
						mObjects[i]->mGla->getCameraPos(currCam);
						for (int duration = 0; duration < 30*mTransDuration3D; duration++)
						{
							double tempCam[6];
							tempCam[0] = prevCam[0] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[0] * duration / (double)(30*mTransDuration3D);
							tempCam[1] = prevCam[1] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[1] * duration / (double)(30*mTransDuration3D);
							tempCam[2] = prevCam[2] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[2] * duration / (double)(30*mTransDuration3D);
							tempCam[3] = prevCam[3] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[3] * duration / (double)(30*mTransDuration3D);
							tempCam[4] = prevCam[4] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[4] * duration / (double)(30*mTransDuration3D);
							tempCam[5] = prevCam[5] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[5] * duration / (double)(30*mTransDuration3D);
							mObjects[i]->mGla->setCameraPos(tempCam);
							screenshotDict = screenshotObj;
							screenshotDict.append("SurfaceNote" + QString::number(noteid) + "_" + QString::number(duration));
							mObjects[i]->mPictures.push_back(mObjects[i]->mGla->screenshot(screenshotDict));
							cv::Mat frame = cv::imread(screenshotDict.toStdString() + ".png", CV_LOAD_IMAGE_COLOR);
							cv::Mat resized = resize2Video(frame, mysize);
							if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
							outputVideo.write(resized);
						}
						mObjects[i]->mGla->setCameraPos(currCam);
						screenshotDict = screenshotObj;
						screenshotDict.append("SurfaceNote" + QString::number(noteid));
						mObjects[i]->mPictures.push_back(mObjects[i]->mGla->screenshot(screenshotDict));
						cv::Mat frame = cv::imread(screenshotDict.toStdString() + ".png", CV_LOAD_IMAGE_COLOR);
						cv::Mat resized = resize2Video(frame, mysize);
						// put subtitle and associated image
						QPair<QString, QString> textAndImg = parseTextAndImg(surfaceNote3D[noteid].second);
						currFrame = putSubtitle(resized, textAndImg.first.toStdString(), mysize, textAndImg.second.toStdString());
						for (int duration = 0; duration < 30*mFrameDuration3D; duration++)
						{
							if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
							outputVideo.write(currFrame);
						}
					}
					for (int noteid = 0; noteid < frustumNote3D.size(); noteid++)
					{
						prevCam[0] = currCam[0]; prevCam[1] = currCam[1]; prevCam[2] = currCam[2];
						prevCam[3] = currCam[3]; prevCam[4] = currCam[4]; prevCam[5] = currCam[5];
						mObjects[i]->mGla->setFrustumNoteView(0, frustumNote3D[noteid].first[0], frustumNote3D[noteid].first[1], frustumNote3D[noteid].first[2], mDolly3D);
						mObjects[i]->mGla->getCameraPos(currCam);
						for (int duration = 0; duration < 30*mTransDuration3D; duration++)
						{
							double tempCam[6];
							tempCam[0] = prevCam[0] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[0] * duration / (double)(30*mTransDuration3D);
							tempCam[1] = prevCam[1] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[1] * duration / (double)(30*mTransDuration3D);
							tempCam[2] = prevCam[2] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[2] * duration / (double)(30*mTransDuration3D);
							tempCam[3] = prevCam[3] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[3] * duration / (double)(30*mTransDuration3D);
							tempCam[4] = prevCam[4] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[4] * duration / (double)(30*mTransDuration3D);
							tempCam[5] = prevCam[5] * (1 - duration / (double)(30*mTransDuration3D)) + currCam[5] * duration / (double)(30*mTransDuration3D);
							mObjects[i]->mGla->setCameraPos(tempCam);
							screenshotDict = screenshotObj;
							screenshotDict.append("FrustumNote" + QString::number(noteid) + "_" + QString::number(duration));
							mObjects[i]->mPictures.push_back(mObjects[i]->mGla->screenshot(screenshotDict));
							cv::Mat frame = cv::imread(screenshotDict.toStdString() + ".png", CV_LOAD_IMAGE_COLOR);
							cv::Mat resized = resize2Video(frame, mysize);
							if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
							outputVideo.write(resized);
						}
						for (int angle = 0; angle < 360; angle++)
						{
							mObjects[i]->mGla->setFrustumNoteView((double)angle,
								frustumNote3D[noteid].first[0], frustumNote3D[noteid].first[1], frustumNote3D[noteid].first[2], mDolly3D);
							screenshotDict = screenshotObj;
							screenshotDict.append("FrustumNote" + QString::number(noteid) + "_" + QString::number(angle));
							mObjects[i]->mPictures.push_back(mObjects[i]->mGla->screenshot(screenshotDict));
							cv::Mat frame = cv::imread(screenshotDict.toStdString() + ".png", CV_LOAD_IMAGE_COLOR);
							cv::Mat resized = resize2Video(frame, mysize);
							// put subtitle and associated image
							QPair<QString, QString> textAndImg = parseTextAndImg(frustumNote3D[noteid].second);
							currFrame = putSubtitle(resized, textAndImg.first.toStdString(), mysize, textAndImg.second.toStdString());
							if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
							outputVideo.write(currFrame);
						}
						mObjects[i]->mGla->getCameraPos(currCam);
					}
					recoverWidget(mObjects[i]->mGla, info, false);
					break;
				}
			case CTSTACK:
			case CTVOLUME:
				saveWidgetinfo(mObjects[i]->mGla, info);
				initWidget(mObjects[i]->mGla, true);
				// generate screenshots from different angles
				for (int angle = 0; angle < 360; angle++)
				{
					mObjects[i]->mGla->setArbitraryView((double)angle);
					screenshotDict = screenshotObj;
					screenshotDict.append(QString::number(angle));
					mObjects[i]->mPictures.push_back(mObjects[i]->mGla->screenshot(screenshotDict));
					cv::Mat frame = cv::imread(screenshotDict.toStdString() + ".png", CV_LOAD_IMAGE_COLOR);
					cv::Mat resized = resize2Video(frame, mysize);
					// blending
					int blendingFrames = 30*mTransDuration3D;
					if (angle < blendingFrames)
						cv::addWeighted(resized, angle/(double)blendingFrames, resized, 0, 0, resized);
					else if (angle >= 360 - blendingFrames)
						cv::addWeighted(resized, (359-angle)/(double)blendingFrames, resized, 0, 0, resized);
					if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
					outputVideo.write(resized);
				}
				recoverWidget(mObjects[i]->mGla, info, true);
				break;
			case RTI2D:
				{
					RTIScreenShot = mObjects[i]->mGla->getRTIScreenShot();
					if (!RTIScreenShot.isNull())
					{
						screenshotObj.append(".png");
						RTIScreenShot.save(screenshotObj);
						mObjects[i]->mPictures.push_back(screenshotObj);
						cv::Mat frame = cv::imread(screenshotObj.toStdString(), CV_LOAD_IMAGE_COLOR);
						cv::Mat prevFrame(mysize, CV_8UC3, cv::Scalar(0, 0, 0)), currFrame;
						// put general annotation, effect to be refined
						cv::Mat resized = resize2Video(frame, mysize);
						currFrame = putSubtitle(resized, annotation.toStdString(), mysize);
						blend2Video(prevFrame, currFrame, outputVideo);
						for (int duration = 0; duration < 30*mFrameDuration2D; duration++)
						{
							if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
							outputVideo.write(currFrame);
						}
						for (int noteid = 0; noteid < pointNote2D.size(); noteid++)
						{
							cv::Point notePos(pointNote2D[noteid].first.first, frame.size().height - pointNote2D[noteid].first.second);
							cv::Mat currNote = emphasizeNote(frame, notePos, 20);
							// put subtitle and associated image
							QPair<QString, QString> textAndImg = parseTextAndImg(pointNote2D[noteid].second);
							prevFrame = currFrame;
							currFrame = resize2Video(currNote, mysize);
							currFrame = putSubtitle(currFrame, textAndImg.first.toStdString(), mysize, textAndImg.second.toStdString());
							blend2Video(prevFrame, currFrame, outputVideo);
							for (int duration = 0; duration < 30*mFrameDuration2D; duration++)
							{
								if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
								outputVideo.write(currFrame);
							}
						}
						for (int noteid = 0; noteid < surfaceNote2D.size(); noteid++)
						{
							cv::Rect myroi;
							myroi.x = std::min(surfaceNote2D[noteid].first[0], surfaceNote2D[noteid].first[2]);
							myroi.y = frame.size().height - std::max(surfaceNote2D[noteid].first[1], surfaceNote2D[noteid].first[3]);
							myroi.width = abs(surfaceNote2D[noteid].first[0] - surfaceNote2D[noteid].first[2]);
							myroi.height = abs(surfaceNote2D[noteid].first[1] - surfaceNote2D[noteid].first[3]);
							cv::Mat currNote = emphasizeNote(frame, myroi);
							// put subtitle and associated image
							QPair<QString, QString> textAndImg = parseTextAndImg(surfaceNote2D[noteid].second);
							prevFrame = currFrame;
							currFrame = resize2Video(currNote, mysize);
							currFrame = putSubtitle(currFrame, textAndImg.first.toStdString(), mysize, textAndImg.second.toStdString());
							blend2Video(prevFrame, currFrame, outputVideo);
							for (int duration = 0; duration < 30*mFrameDuration2D; duration++)
							{
								if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
								outputVideo.write(currFrame);
							}
						}
						for (int noteid = 0; noteid < polygonNote2D.size(); noteid++)
						{
							cv::Point center(0, 0); 
							for (int vid = 0; vid < polygonNote2D[noteid].first.size() - 1; vid++)
							{
								center.x += polygonNote2D[noteid].first[vid].first;
								center.y += polygonNote2D[noteid].first[vid].second;
							}
							center.x = center.x / (polygonNote2D[noteid].first.size() - 1);
							center.y = center.y / (polygonNote2D[noteid].first.size() - 1);
							int radius = 20;
							for (int vid = 0; vid < polygonNote2D[noteid].first.size() - 1; vid++)
							{
								int dx2 = (center.x - polygonNote2D[noteid].first[vid].first) * (center.x - polygonNote2D[noteid].first[vid].first);
								int dy2 = (center.y - polygonNote2D[noteid].first[vid].second) * (center.y - polygonNote2D[noteid].first[vid].second);
								int tmpR = (int)sqrt((double)(dx2 + dy2));
								radius = std::max(radius, tmpR);
							}
							center.y = frame.size().height - center.y;
							cv::Mat currNote = emphasizeNote(frame, center, radius);
							// put subtitle and associated image
							QPair<QString, QString> textAndImg = parseTextAndImg(polygonNote2D[noteid].second);
							prevFrame = currFrame;
							currFrame = resize2Video(currNote, mysize);
							currFrame = putSubtitle(currFrame, textAndImg.first.toStdString(), mysize, textAndImg.second.toStdString());
							blend2Video(prevFrame, currFrame, outputVideo);
							for (int duration = 0; duration < 30*mFrameDuration2D; duration++)
							{
								if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
								outputVideo.write(currFrame);
							}
						}
					}
					break;
				}
			default: break;
		}

		/*
		// Switch back to origianl CT STACK mode.
		if (isCTModeSwitched)
			mw()->mCtControl->setCTStackView();

		//Process and add image
		for (int j = 0; j < mObjects[i]->mPictures.size(); j++)
		{
			QString path = QDir::toNativeSeparators(mObjects[i]->mPictures[j]);
			QImage img(path); 
			if (img == QImage())
			{
				QString message = QString("Cannot find image: ").append(mObjects[i]->mPictures[j]);
				QMessageBox::critical(this, tr("Project Error"), message);
				continue;
			}
			int imgHeight = img.height();
			int imgWidth = img.width();
			QStringList nameElements = mObjects[i]->mPictures[j].split(QDir::separator());
			QString url = nameElements[nameElements.size() - 1];
			QPainter painter(&img);
			QFont font = painter.font();
			int pointSize = font.pointSize();
			pointSize = pointSize*2 + 3.5*imgWidth / 400;
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

			for (int k = 0; k < polygonNote2D.size(); k++)
			{
				QVector<QPair<int, int> >::iterator it;
				int maxX = 0, maxY = 0, minX = 0xFFFF, minY = 0xFFFF;
				for (it = polygonNote2D[k].begin(); it != polygonNote2D[k].end(); it++)
				{
					if (it->first > maxX) maxX = it->first;
					if (it->first < minX) minX = it->first;
					if (it->second > maxY) maxY = it->second;
					if (it->second < minY) minY = it->second;
				}
				int width = maxX - minX;
				int height = maxY - minY;
				minY = img.height() - maxY;
				QRectF rectangle(minX, minY, width, height);

				QVector<QPair<int, int> >::iterator it1 = polygonNote2D[k].begin();
				QVector<QPair<int, int> >::iterator it2 = polygonNote2D[k].begin();
				it2++;
				painter.setPen(QPen(Qt::red, 4));
				for (; it2 != polygonNote2D[k].end(); it1++, it2++)
				{
					QLineF line(it1->first, img.height() - it1->second, it2->first, img.height() - it2->second);
					painter.drawLine(line);
				}
				it2 = polygonNote2D[k].begin();
				QLineF lastLine(it1->first, img.height() - it1->second, it2->first, img.height() - it2->second);
				painter.drawLine(lastLine);

				painter.setPen(QPen(Qt::black, 4));
				painter.drawText(rectangle,  Qt::AlignCenter, QString::number(k+pointNote2D.size()+surfaceNote2D.size()+1));
				
			}

			font = painter.font();
			pointSize = pointSize * 1.2;
			//if (pointSize == 0)	pointSize = 1;
			font.setPointSize(pointSize);
			painter.setFont(font);
			QVector<QPair<double, double> > pointNote3DSelected, surfaceNote3DSelected, frustumNote3DSelected;
			QVector<QPair<double*, CTSurfaceCornerPoint_> > surfaceNote3DSelected_CT;
			switch(j)
			{
				case 0: 
					pointNote3DSelected = pointNote3DFront;
					surfaceNote3DSelected = surfaceNote3DFront;
					frustumNote3DSelected = frustumNote3DFront;
					surfaceNote3DSelected_CT = surfaceNote3DFront_CT;
					break;
				case 1: 
					pointNote3DSelected = pointNote3DLeft;
					surfaceNote3DSelected = surfaceNote3DLeft;
					frustumNote3DSelected = frustumNote3DLeft;
					surfaceNote3DSelected_CT = surfaceNote3DLeft_CT;
					break;
				case 2: 
					pointNote3DSelected = pointNote3DRight;
					surfaceNote3DSelected = surfaceNote3DRight;
					frustumNote3DSelected = frustumNote3DRight;
					surfaceNote3DSelected_CT = surfaceNote3DRight_CT;
					break;
				case 3: 
					pointNote3DSelected = pointNote3DTop;
					surfaceNote3DSelected = surfaceNote3DTop;
					frustumNote3DSelected = frustumNote3DTop;
					surfaceNote3DSelected_CT = surfaceNote3DTop_CT;
					break;
				case 4: 
					pointNote3DSelected = pointNote3DBottom;
					surfaceNote3DSelected = surfaceNote3DBottom;
					frustumNote3DSelected = frustumNote3DBottom;
					surfaceNote3DSelected_CT = surfaceNote3DBottom_CT;
					break;
				case 5: 
					pointNote3DSelected = pointNote3DBack;
					surfaceNote3DSelected = surfaceNote3DBack;
					frustumNote3DSelected = frustumNote3DBack;
					surfaceNote3DSelected_CT = surfaceNote3DBack_CT;
					break;
				default:break;
			}

			for (int k = 0; k < pointNote3DSelected.size(); k++)
			{
				double x = pointNote3DSelected[k].first, y = img.height() - pointNote3DSelected[k].second;
				if (x == -1 || y == -1)
					continue;
				double x1 = x - pointSize/2, y1 = y - pointSize/2;
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
				double x = surfaceNote3DSelected[k].first, y = img.height() - surfaceNote3DSelected[k].second;
				if (x == -1 || y == -1)
					continue;
				double x1 = x - pointSize/2, y1 = y - pointSize/2;
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
				painter.drawText(rectangle, Qt::AlignCenter, QString::number(k+1+pointNote3DSelected.size()));
			}
			for (int k = 0; k < frustumNote3DSelected.size(); k++)
			{
				double x = frustumNote3DSelected[k].first, y = img.height() - frustumNote3DSelected[k].second;
				if (x == -1 || y == -1)
					continue;
				double x1 = x - pointSize/2, y1 = y - pointSize/2;
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
			for (int k = 0; k < surfaceNote3DSelected_CT.size(); k++)
			{
				double x = surfaceNote3DSelected_CT[k].first[0], y = img.height() - surfaceNote3DSelected_CT[k].first[1];
				if (x == -1 || y == -1)
					continue;
				double x1 = x - pointSize/2, y1 = y - pointSize/2;
				if (x1 <= 0)
					x1 = 1;
				else if (x1 + pointSize >= imgWidth)
					x1 = imgWidth - pointSize - 1;
				if (y1 <= 0)
					y1 = 1;
				else if (y1 + pointSize >= imgHeight)
					y1 = imgHeight - pointSize - 1;


				QPolygon polygon;
				polygon << QPoint(surfaceNote3DSelected_CT[k].second.point[0][0], img.height() - surfaceNote3DSelected_CT[k].second.point[0][1])
					<< QPoint(surfaceNote3DSelected_CT[k].second.point[1][0], img.height() - surfaceNote3DSelected_CT[k].second.point[1][1])
					<< QPoint(surfaceNote3DSelected_CT[k].second.point[2][0], img.height() - surfaceNote3DSelected_CT[k].second.point[2][1])
					<< QPoint(surfaceNote3DSelected_CT[k].second.point[3][0], img.height() - surfaceNote3DSelected_CT[k].second.point[3][1])
					<< QPoint(surfaceNote3DSelected_CT[k].second.point[0][0], img.height() - surfaceNote3DSelected_CT[k].second.point[0][1]);

				qDebug()<<"point 0"<<surfaceNote3DSelected_CT[k].second.point[0][0]<<surfaceNote3DSelected_CT[k].second.point[0][1];
				qDebug()<<"point 1"<<surfaceNote3DSelected_CT[k].second.point[1][0]<<surfaceNote3DSelected_CT[k].second.point[1][1];
				qDebug()<<"point 2"<<surfaceNote3DSelected_CT[k].second.point[2][0]<<surfaceNote3DSelected_CT[k].second.point[2][1];
				qDebug()<<"point 3"<<surfaceNote3DSelected_CT[k].second.point[3][0]<<surfaceNote3DSelected_CT[k].second.point[3][1];

				QRectF rectangle(x1, y1, pointSize, pointSize);
				painter.setPen(QPen(Qt::red, 4));
				painter.drawPolygon(polygon);
				painter.setPen(QPen(Qt::black, 4));
				painter.drawText(rectangle, Qt::AlignCenter, QString::number(k+1+pointNote3DSelected.size()));
			}
			
			painter.end(); 
		    mDoc->addResource(QTextDocument::ImageResource, QUrl(url), img);
			
			if (mObjects[i]->mMode == IMAGE2D || mObjects[i]->mMode == RTI2D)
			{
				double height = 200, width = 300;
				height = (double)width * imgHeight / imgWidth;
				if (isWmv)
					html = html + QString("<p><div align=\"center\"><img src=\"" + url + "\"width=\"" + QString::number(width) + "\" height=\"" + QString::number(height) + "\"></div></p>\n");
				else
				{
					QString fileName = htmlFolder;
					fileName.append(QDir::separator() + url);
					img.save(fileName);
					html = html + QString("<p><div align=\"center\"><img src=\"video/" + url + "\"width=\"" + QString::number(width) + "\" height=\"" + QString::number(height) + "\"></div></p>\n");
				}
			}
			else if (mObjects[i]->mMode == MODEL3D || mObjects[i]->mMode == CTSTACK || mObjects[i]->mMode == CTVOLUME)
			{
				double height = 100, width = 150;
				height = (double)width * imgHeight / imgWidth;
				if (isWmv)
				{
					if (j == 0)
						html += QString("<p><div align=\"center\">");
					html += QString("<img src=\"" + url + "\"width=\"" + QString::number(width) + "\" height=\"" + QString::number(height) + "\">");
					if (j == mObjects[i]->mPictures.size() - 1)
						html += QString("</div></p>\n");
				}
				else
				{
					QString fileName = htmlFolder;
					fileName.append(QDir::separator() + url);
					img.save(fileName);
					if (j == 0)
						html += QString("<p><div align=\"center\">");
					html += QString("<img src=\"video/" + url + "\"width=\"" + QString::number(width) + "\" height=\"" + QString::number(height) + "\">");
					if (j == mObjects[i]->mPictures.size() - 1)
						html += QString("</div></p>\n");
				}
			}
		}
		*/
		// Add annotation and notes
		/*
		if (!annotation.isEmpty())
			html = html + QString("<p><font size=\"2\" face=\"Garamond\">") + annotation + QString("</font></p><hr>\n");
		for (int j = 0; j < contents.size(); j++)
		{
			QString content = contents[j].first;
			QString color = content.split("\nNote Start:\n")[0];
			int type = color2type(color.toStdString());
			if (mObjects[i]->mCategories.indexOf(type) == -1)
				continue;
			html += QString("<p><font size=\"2\" color=\"#033F81\" face=\"Garamond\">\n");
			switch(contents[j].second)
			{
				case POINTNOTE:		html += QString("Point Note "); break;
				case SURFACENOTE:	html += QString("Surface Note "); break;
				case POLYGONNOTE:	html += QString("Polygon Note "); break;
				case FRUSTUMNOTE:	html += QString("Frustum Note "); break;
				default: continue;
			}

			QString text = content.split("\nLinked Images:\n")[0].split("Note Start:\n")[1];
			QString category = QString(color2categoryFullName_(color.toStdString()).c_str());
			QStringList imagePaths = content.split("\nLinked Images:\n")[1].split("\n", QString::SkipEmptyParts);
			QDir dir(mObjects[i]->mNotesPath);
			html += QString::number(j+1) + QString(": </font></p>\n")
				+ QString("<p><font size=\"2\" color=\"#033F81\" face=\"Garamond\">Category: ") + category + QString("</font></p>");
			for (int k = 0; k < imagePaths.size(); k++)
			{
				QImage imgNote(dir.absoluteFilePath(imagePaths[k]));
				int imgNoteHeight = imgNote.height();
				int imgNoteWidth = imgNote.width();
				if (imgNoteHeight == 0 || imgNoteWidth == 0)
					continue;
				int height, width;
				if (imgNoteWidth/imgNoteHeight >= 3)
				{	
					width = 300;
					height = (double)width * imgNoteHeight / imgNoteWidth;
				}
				else if (imgNoteWidth/imgNoteHeight >= 0.5)
				{
					width = 150;
					height = (double)width * imgNoteHeight / imgNoteWidth;
				}
				else
				{
					height = 250;
					width = (double)height * imgNoteWidth / imgNoteHeight;
				}
				QString name = mObjects[i]->mName;
				name.insert(name.lastIndexOf("."), QString("_" + QString::number(j) + "_" + QString::number(k)));
				mDoc->addResource(QTextDocument::ImageResource, QUrl(name), imgNote);
				if (isWmv)
					html += QString("<p><div align=\"center\"><img src=\"" + name + "\"width=\"" + QString::number(width) + "\" height=\"" + QString::number(height) + "\"></div></p>");
				else
				{
					QString fileName = htmlFolder;
					fileName.append(QDir::separator() + name);
					imgNote.save(fileName);
					html += QString("<p><div align=\"center\"><img src=\"video/" + name + "\"width=\"" + QString::number(width) + "\" height=\"" + QString::number(height) + "\"></div></p>");
				}
			}
			html += QString("<p><font size=\"2\" face=\"Garamond\">") + text + QString("</font></p>");
			if (j != contents.size()-1)
				html += QString("<hr>\n");
		}
		*/
		// delete allocated pointers
		for (int j = 0; j < surfaceNote2D.size(); j++)
		{
			delete surfaceNote2D[j].first;
		}
		for (int j = 0; j < pointNote3D.size(); j++)
		{
			delete pointNote3D[j].first.second;
		}
		for (int j = 0; j < surfaceNote3D.size(); j++)
		{
			delete surfaceNote3D[j].first.second;
		}
		for (int j = 0; j < frustumNote3D.size(); j++)
		{
			delete frustumNote3D[j].first;
		}
	}
	outputVideo.release();

	videoFolderDir.setNameFilters(QStringList() << "*.*");
	videoFolderDir.setFilter(QDir::Files);
	foreach(QString dirFile, videoFolderDir.entryList())
	{
		videoFolderDir.remove(dirFile);
	}
	QDir().rmdir(videoFolder);

	/*
	if (QFileInfo(mLocation).suffix() == QString("html"))
	{
		QFile file(mLocation);
		  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			qDebug() << "Cannot write to file " << mLocation; 
			return;
		}
		QTextStream out(&file);
		out << html;
		file.close();
		return;
	}

	mDoc->setHtml(html);
	mPrinter->setPageMargins(10, 10, 5, 20, QPrinter::Millimeter);
	mDoc->setPageSize(mPrinter->pageRect().size());

	QRect innerRect = mPrinter->pageRect();
    innerRect.setTop(innerRect.top());
    innerRect.setBottom(innerRect.bottom());
    QRect contentRect = QRect(QPoint(0,0), mDoc->size().toSize());
    QRect currentRect = QRect(QPoint(0,0), innerRect.size());
	

    QPainter painter(mPrinter);
	QDate date = QDate::currentDate();
	
    int count = 0;
    painter.save();
    painter.translate(0, 20);
    while (currentRect.intersects(contentRect)) 
	{
        mDoc->drawContents(&painter, currentRect);
        count++;
        currentRect.translate(0, currentRect.height());
        painter.restore();
		painter.setPen(QColor(3,63,129));
		painter.setFont(QFont("Garamond", 12, QFont::Bold));
        painter.drawText(mPrinter->pageRect().right() - 100, 10, QString("CHER-Ob"));
		painter.setPen(Qt::black);
		painter.setFont(QFont("Garamond", 8));
		painter.drawText(mPrinter->pageRect().left(), mPrinter->pageRect().bottom()+12, date.toString("MM/dd/yyyy"));
        painter.drawText(mPrinter->pageRect().right() - 70, mPrinter->pageRect().bottom()+12, QString("Page %1").arg(count));
        painter.save();
        painter.translate(0, -currentRect.height() * count+20);
        if (currentRect.intersects(contentRect))
            mPrinter->newPage();
    }
    painter.restore();
    painter.end();
	*/
}

void VideoGenerator::detectPointVisibility(vtkSmartPointer<vtkRenderer> render, QVector<double*> points, QVector<QPair<double, double> >& visiblePoints)
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

void VideoGenerator::detectFrustumVisibility(const VtkWidget* gla, QVector<double*> center, 
											  QVector<QPair<double, double> >& visiblePoints,
											  QVector<vtkSmartPointer<vtkDataSet> > dataset,
											  OrthogonalView3D view)
{
	vtkSmartPointer<vtkCamera> camera = gla->mRenderer->GetActiveCamera();
	double cameraPos[3];
	camera->GetPosition(cameraPos);
	double point[3];
	for (int i = 0; i < center.size(); i++)
	{
		vtkSmartPointer<vtkPoints> selectionPoints = vtkSmartPointer<vtkPoints>::New();
		selectionPoints->SetNumberOfPoints(dataset[i]->GetNumberOfPoints());
		for (int j = 0; j < dataset[i]->GetNumberOfPoints(); j++)
		{
			dataset[i]->GetPoint(j, point);
			selectionPoints->SetPoint(j, point);
		}

		vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
		for (int j = 0; j < dataset[i]->GetNumberOfCells(); j++)
		{
			vtkCell* cell = dataset[i]->GetCell(j);
			cellArray->InsertNextCell(cell);
		}

		vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
		polyData->SetPoints(selectionPoints);
		polyData->SetPolys(cellArray);

		double intersectPoints[3], pcoord[3], t;
		double projection[3];
		projection[0] = center[i][0];
		projection[1] = center[i][1];
		projection[2] = center[i][2];
		int sub;
		switch(view)
		{
		case FRONT3D:projection[2]+=100;break;
		case LEFT3D:projection[0]-=100;break;
		case RIGHT3D:projection[0]+=100;break;
		case TOP3D:projection[1]+=100;break;
		case BOTTOM3D:projection[1]-=100;break;
		case BACK3D:projection[2]-=100;break;
		}

		vtkSmartPointer<vtkCellLocator> cellLocator = 
		vtkSmartPointer<vtkCellLocator>::New();
		cellLocator->SetDataSet(polyData);
		cellLocator->BuildLocator();

		int result = cellLocator->IntersectWithLine(projection, center[i], 100, t, intersectPoints, pcoord, sub);

		if (result == 0)
		{
			double x = -1;
			visiblePoints.push_back(qMakePair(x, x));
		}
		else
		{
			vtkSmartPointer<vtkPoints> point = vtkSmartPointer<vtkPoints>::New();
			vtkSmartPointer<vtkPolyData> pointData = vtkSmartPointer<vtkPolyData>::New();
			point->SetNumberOfPoints(1);
			point->SetPoint(0, intersectPoints[0], intersectPoints[1], intersectPoints[2]);
			pointData->SetPoints(point);
			vtkSmartPointer<vtkSelectVisiblePoints> selectVisiblePoints = vtkSmartPointer<vtkSelectVisiblePoints>::New();
			selectVisiblePoints->SetInput(pointData);
			selectVisiblePoints->SetRenderer(gla->mRenderer);
			selectVisiblePoints->Update();
			if (selectVisiblePoints->GetOutput()->GetNumberOfPoints() != 0)
			{
				double *displayPt = new double[3];
				vtkInteractorObserver::ComputeWorldToDisplay (gla->mRenderer, intersectPoints[0], intersectPoints[1], intersectPoints[2], displayPt);
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
}

void VideoGenerator::detectCTSurfaceVisibility(vtkSmartPointer<vtkRenderer> render, 
		QVector<QPair<double*, CTSurfaceCornerPoint_> > points, QVector<QPair<double*, CTSurfaceCornerPoint_> >& visiblePoints)
{
	for (int i = 0; i < points.size(); i++)
	{
		vtkSmartPointer<vtkPoints> point = vtkSmartPointer<vtkPoints>::New();
		vtkSmartPointer<vtkPolyData> pointData = vtkSmartPointer<vtkPolyData>::New();
		point->SetNumberOfPoints(1);
		point->SetPoint(0, points[i].first[0], points[i].first[1], points[i].first[2]);
		pointData->SetPoints(point);
		vtkSmartPointer<vtkSelectVisiblePoints> selectVisiblePoints = vtkSmartPointer<vtkSelectVisiblePoints>::New();
		selectVisiblePoints->SetInput(pointData);
		selectVisiblePoints->SetRenderer(render);
		selectVisiblePoints->Update();
		double *displayPt = new double[3];
		if (selectVisiblePoints->GetOutput()->GetNumberOfPoints() != 0)
		{
			vtkInteractorObserver::ComputeWorldToDisplay (render, points[i].first[0], points[i].first[1], points[i].first[2], displayPt);
			CTSurfaceCornerPoint_ corners;
			for (int j = 0; j < 4; j++)
			{
				vtkInteractorObserver::ComputeWorldToDisplay (render, points[i].second.point[j][0],
					points[i].second.point[j][1], points[i].second.point[j][2], corners.point[j]);
				qDebug()<<"in detect"<<corners.point[j][0]<<corners.point[j][1];
			}
			visiblePoints.push_back(qMakePair(displayPt, corners));
		}
		else
		{
			displayPt[0] = -1;
			displayPt[1] = -1;
			displayPt[2] = -1;
			visiblePoints.push_back(qMakePair(displayPt, points[i].second));
		}
	}
}

MainWindow* VideoGenerator::mw()
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

void VideoGenerator::saveWidgetinfo(const VtkWidget* gla, WidgetInfo3D_ &info)
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

void VideoGenerator::initWidget(VtkWidget* gla, bool isCTVolume)
{
	gla->setDisplayInfoOn(false);
	mw()->mLightControl->reset();
	if (gla->getIsTextureOn())
	{
		gla->setTextureOn(true);
	}
	if (isCTVolume)
	{
		gla->annotate(false);
	}
	else
	{
		gla->setRenderMode3D(SURFACE3D);
		gla->annotate(true);
	}
	gla->setVisibilityDistance(false);
	
}

void VideoGenerator::recoverWidget(VtkWidget* gla, WidgetInfo3D_ info, bool isCTVolume)
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
   
	if (!isCTVolume)	
		gla->setRenderMode3D(info.renderMode);
	gla->setMeasureDistance(info.isMeasuring);
	gla->annotate(info.isAnnotation);
	if(gla->mQVTKWidget) gla->mQVTKWidget->update();
}

void VideoGenerator::computeCenter(const VtkWidget* gla, QVector<int> cellIds, double* center, vtkIdType* centerCellId)
{
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	for (int i = 0; i < cellIds.size(); i++)
	{
		vtkSmartPointer<vtkIdList> ptIds = vtkSmartPointer<vtkIdList>::New();
		gla->get3DPolyData()->GetCellPoints(cellIds[i], ptIds);
		double point[3];
		for (int j = 0; j < ptIds->GetNumberOfIds(); j++)
		{
			gla->get3DPolyData()->GetPoint(ptIds->GetId(j), point);
			points->InsertPoint(points->GetNumberOfPoints(), point);
		}
	}
	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);

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
	int subId;
	cellLocator->FindClosestPoint(CoM, center, *centerCellId, subId, closestPointDist2);
}

void VideoGenerator::computeCenter(const VtkWidget* gla, vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals, 
									double* center, vtkSmartPointer<vtkDataSet>& polyData)
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
	polyData  = vtkDataSet::SafeDownCast(extractor->GetOutput());
	
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

void VideoGenerator::computeCenter(CTSurfaceCornerPoint_ cornerPoints, double* center)
{
	center[0] = 0;
	center[1] = 0;
	center[2] = 0;
	for (int i = 0; i < 4; i++)
	{
		center[0] += cornerPoints.point[i][0]/4;
		center[1] += cornerPoints.point[i][1]/4;
		center[2] += cornerPoints.point[i][2]/4;
	}
}

cv::Mat VideoGenerator::resize2Video(cv::Mat& src, cv::Size mysize)
{
	cv::Mat dst(mysize, src.type(), cv::Scalar(0, 0, 0));
	cv::Rect roi;
	if (mysize.width / (double)mysize.height > src.size().width / (double)src.size().height)
	{
		double ratio = mysize.height / (double)src.size().height;
		roi.width = (int)(src.size().width * ratio);
		roi.height = mysize.height;
		roi.x = (mysize.width - roi.width) / 2;
		roi.y = 0;
	}
	else
	{
		double ratio = mysize.width / (double)src.size().width;
		roi.width = mysize.width;
		roi.height = (int)(src.size().height * ratio);
		roi.x = 0;
		roi.y = (mysize.height - roi.height) / 2;
	}
	cv::resize(src, dst(roi), roi.size());
	return dst;
}

cv::Mat VideoGenerator::putSubtitle(cv::Mat& src, std::string mystr, cv::Size mysize, std::string img)
{
	cv::Mat annotated = src.clone();
	cv::Mat blackImg(mysize, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Rect roi(0, 480, 800, 120);
	cv::Mat subtitle1 = annotated(roi);
	cv::Mat subtitle2 = blackImg(roi);
	cv::addWeighted(subtitle1, 0.7, subtitle2, 0.3, 0, subtitle1);
	int lineLen = (img == "" ? 85 : 60);
	int eps = 0;
	for (int i = 0; i < 5; i++)
	{
		if (mystr.size() <= i * lineLen) break;
		// line break rules to be refined
		std::string line = mystr.substr(i * lineLen + eps, lineLen);
		if (mystr.size() > (i + 1) * lineLen)
		{
			if (mystr[(i + 1) * lineLen + eps] == ' ') eps++;
			else
			{
				int spacePos = line.rfind(' ', line.size());
				line = line.substr(0, spacePos);
				eps -= lineLen - spacePos - 1;
			}
		}
		cv::putText(annotated, line, cv::Point(50, 500 + i * 20), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(255, 255, 255), 1, CV_AA);
	}
	if (img != "")
	{
		cv::Mat assoImg = cv::imread(img, CV_LOAD_IMAGE_COLOR);
		assoImg = resize2Video(assoImg, cv::Size(120, 120));
		assoImg.copyTo(annotated.colRange(680, 680 + assoImg.size().width).rowRange(480, 480 + assoImg.size().height));
	}
	return annotated;
}


cv::Mat VideoGenerator::emphasizeNote(cv::Mat& src, cv::Rect roi)
{
	if (roi.x < 0) roi.x = 0;
	if (roi.y < 0) roi.y = 0;
	if (roi.x + roi.width > src.size().width) roi.width = src.size().width - roi.x;
	if (roi.y + roi.height > src.size().height) roi.height = src.size().height - roi.y;
	cv::Mat emphasized;
	cv::bilateralFilter(src, emphasized, 15, 80, 80);
	cv::addWeighted(emphasized, 0.7, emphasized, 0, 0, emphasized);
	// smoother boundary
	cv::Mat noteRegion = src.clone();
	noteRegion = noteRegion(roi);
	int smoother = 10;
	if (noteRegion.size().width < 2 * smoother) smoother = noteRegion.size().width / 2;
	if (noteRegion.size().height < 2 * smoother) smoother = noteRegion.size().height / 2;
	for (int k = 0; k < smoother; k++)
	{
		for (int i = k + 1; i < noteRegion.size().height - k - 1; i++)
		{
			noteRegion.at<cv::Vec3b>(i, k)[0] *= (0.7 + 0.03 * k);
			noteRegion.at<cv::Vec3b>(i, k)[1] *= (0.7 + 0.03 * k);
			noteRegion.at<cv::Vec3b>(i, k)[2] *= (0.7 + 0.03 * k);
			noteRegion.at<cv::Vec3b>(i, noteRegion.size().width - k - 1)[0] *= (0.7 + 0.03 * k);
			noteRegion.at<cv::Vec3b>(i, noteRegion.size().width - k - 1)[1] *= (0.7 + 0.03 * k);
			noteRegion.at<cv::Vec3b>(i, noteRegion.size().width - k - 1)[2] *= (0.7 + 0.03 * k);
		}
		for (int j = k + 1; j < noteRegion.size().width - k - 1; j++)
		{
			noteRegion.at<cv::Vec3b>(k, j)[0] *= (0.7 + 0.03 * k);
			noteRegion.at<cv::Vec3b>(k, j)[1] *= (0.7 + 0.03 * k);
			noteRegion.at<cv::Vec3b>(k, j)[2] *= (0.7 + 0.03 * k);
			noteRegion.at<cv::Vec3b>(noteRegion.size().height - k - 1, j)[0] *= (0.7 + 0.03 * k);
			noteRegion.at<cv::Vec3b>(noteRegion.size().height - k - 1, j)[1] *= (0.7 + 0.03 * k);
			noteRegion.at<cv::Vec3b>(noteRegion.size().height - k - 1, j)[2] *= (0.7 + 0.03 * k);
		}
		noteRegion.at<cv::Vec3b>(k, k)[0] *= (0.7 + 0.03 * k);
		noteRegion.at<cv::Vec3b>(k, k)[1] *= (0.7 + 0.03 * k);
		noteRegion.at<cv::Vec3b>(k, k)[2] *= (0.7 + 0.03 * k);
		noteRegion.at<cv::Vec3b>(k, noteRegion.size().width - k - 1)[0] *= (0.7 + 0.03 * k);
		noteRegion.at<cv::Vec3b>(k, noteRegion.size().width - k - 1)[1] *= (0.7 + 0.03 * k);
		noteRegion.at<cv::Vec3b>(k, noteRegion.size().width - k - 1)[2] *= (0.7 + 0.03 * k);
		noteRegion.at<cv::Vec3b>(noteRegion.size().height - k - 1, k)[0] *= (0.7 + 0.03 * k);
		noteRegion.at<cv::Vec3b>(noteRegion.size().height - k - 1, k)[1] *= (0.7 + 0.03 * k);
		noteRegion.at<cv::Vec3b>(noteRegion.size().height - k - 1, k)[2] *= (0.7 + 0.03 * k);
		noteRegion.at<cv::Vec3b>(noteRegion.size().height - k - 1, noteRegion.size().width - k - 1)[0] *= (0.7 + 0.03 * k);
		noteRegion.at<cv::Vec3b>(noteRegion.size().height - k - 1, noteRegion.size().width - k - 1)[1] *= (0.7 + 0.03 * k);
		noteRegion.at<cv::Vec3b>(noteRegion.size().height - k - 1, noteRegion.size().width - k - 1)[2] *= (0.7 + 0.03 * k);
	}
	noteRegion.copyTo(emphasized.colRange(roi.x, roi.x + roi.width).rowRange(roi.y, roi.y + roi.height));
	return emphasized;
}

	
cv::Mat VideoGenerator::emphasizeNote(cv::Mat& src, cv::Point center, int radius)
{
	if (center.x - radius < 0) radius = center.x;
	if (center.y - radius < 0) radius = center.y;
	if (center.x + radius >= src.size().width) radius = src.size().width - center.x - 1;
	if (center.y + radius >= src.size().height) radius = src.size().height - center.y - 1;
	cv::Mat emphasized;
	cv::bilateralFilter(src, emphasized, 15, 80, 80);
	cv::addWeighted(emphasized, 0.7, emphasized, 0, 0, emphasized);
	// smoother boundary
	cv::Mat mask = cv::Mat::zeros(src.size(), CV_8UC1);
	cv::circle(mask, center, radius, CV_RGB(255, 255, 255), -1);
	cv::Mat noteRegion = src.clone();
	int smoother = 10;
	for (int i = center.y - radius; i <= center.y + radius; i++)
	{
		for (int j = center.x - radius; j <= center.x + radius; j++)
		{
			double dist = sqrt((double)((i - center.y) * (i - center.y) + (j - center.x) * (j - center.x)));
			if (dist <= radius && dist >= radius - smoother)
			{
				double ratio = (radius - dist) / smoother;
				noteRegion.at<cv::Vec3b>(i, j)[0] *= (0.7 + 0.3 * ratio);
				noteRegion.at<cv::Vec3b>(i, j)[1] *= (0.7 + 0.3 * ratio);
				noteRegion.at<cv::Vec3b>(i, j)[2] *= (0.7 + 0.3 * ratio);
			}
		}
	}
	noteRegion.copyTo(emphasized, mask);
	return emphasized;
}

QPair<QString, QString> VideoGenerator::parseTextAndImg(QString content)
{
	QString color = content.split("\nNote Start:\n")[0];
	int type = color2type(color.toStdString());
	QString text = content.split("\nLinked Images:\n")[0].split("Note Start:\n")[1];
	QString category = QString(color2categoryFullName_(color.toStdString()).c_str());
	QStringList imagePaths = content.split("\nLinked Images:\n")[1].split("\n", QString::SkipEmptyParts);
	if (imagePaths.size() > 0)
		return qMakePair(text, imagePaths[0]);
	QString emptyPath;
	return qMakePair(text, emptyPath);
}

void VideoGenerator::blend2Video(cv::Mat& img1, cv::Mat& img2, cv::VideoWriter& outputVideo)
{
	int blendFrames = 30*mTransDuration2D;
	if (!outputVideo.isOpened())
	{
		qDebug() << "ERROR: outputVideo not opened!\n\n";
		return;
	}
	for (int i = 0; i <= blendFrames; i++)
	{
		cv::Mat blended;
		cv::addWeighted(img1, (blendFrames-i)/(double)blendFrames, img2, i/(double)blendFrames, 0, blended);
		outputVideo.write(blended);
	}
}