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
#include "lightControlRTI.h"
#include "../information/informationWidget.h"
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

	//for (int i = 0; i < mObjects.size(); i++)
	//{
	//	QString screenshotObj = videoFolder;
	//	screenshotObj.append(QDir::separator() + mObjects[i]->mName);
	//	QString screenshotDict = screenshotObj;
	//	screenshotDict.append("_geo.png");
	//	// set google map JavaScript to relocate current object
	//	mw()->mGeoInfo->startGeoMarking();
	//	mw()->mGeoInfo->setPos4Video(screenshotDict, mw()->mInformation->getGeoInfo(mObjects[i]->mName));
	//	// save google map screenshot
	//}

	char myFourCC[4];
	switch (mVideoFormat)
	{
		default:
		case 0: myFourCC[0] = 'D'; myFourCC[1] = 'I'; myFourCC[2] = 'V'; myFourCC[3] = '3'; mLocation = mLocation + ".avi"; break;
		case 1: myFourCC[0] = 'M'; myFourCC[1] = 'P'; myFourCC[2] = '4'; myFourCC[3] = '2'; mLocation = mLocation + "_.avi"; break;
		case 2: myFourCC[0] = 'W'; myFourCC[1] = 'M'; myFourCC[2] = 'V'; myFourCC[3] = '2'; mLocation = mLocation + ".wmv"; break;
	}
	cv::Point ptTitle, ptAuthor, ptAffiliation, ptPowered;
	double fontSize;
	switch (mResolutionOption)
	{
		default:
		case 0:
			mysize = cv::Size(800, 600);
			ptTitle.x = 50; ptTitle.y = 200;
			ptAuthor.x = 50; ptAuthor.y = 350;
			ptAffiliation.x = 50; ptAffiliation.y = 400;
			ptPowered.x = 550; ptPowered.y = 550;
			fontSize = 0.5;
			break;
		case 1:
			mysize = cv::Size(1600, 900);
			ptTitle.x = 100; ptTitle.y = 300;
			ptAuthor.x = 100; ptAuthor.y = 500;
			ptAffiliation.x = 100; ptAffiliation.y = 600;
			ptPowered.x = 1100; ptPowered.y = 800;
			fontSize = 1.0;
			break;
		case 2:
			mysize = cv::Size(1600, 1200);
			ptTitle.x = 100; ptTitle.y = 400;
			ptAuthor.x = 100; ptAuthor.y = 700;
			ptAffiliation.x = 100; ptAffiliation.y = 800;
			ptPowered.x = 1100; ptPowered.y = 1100;
			fontSize = 1.0;
			break;
	}
	cv::VideoWriter outputVideo(mLocation.toStdString(), cv::VideoWriter::fourcc(myFourCC[0],myFourCC[1],myFourCC[2],myFourCC[3]), 30, mysize, true); // SIZE ISSUE

	// create title frame
	cv::Mat titleFrame = cv::Mat::zeros(mysize, CV_8UC3);
	cv::putText(titleFrame, mProjectName.toStdString(), ptTitle, cv::FONT_HERSHEY_DUPLEX, 2*fontSize, cv::Scalar(255, 255, 255), 1, CV_AA);
	cv::putText(titleFrame, mUserName.toStdString(), ptAuthor, cv::FONT_HERSHEY_DUPLEX, fontSize, cv::Scalar(255, 255, 255), 1, CV_AA);
	if (mAffiliation != "")
	{
		cv::putText(titleFrame, mAffiliation.toStdString(), ptAffiliation, cv::FONT_HERSHEY_DUPLEX, fontSize, cv::Scalar(255, 255, 255), 1, CV_AA);
	}
	cv::putText(titleFrame, "Powered by CHER-Ob", ptPowered, cv::FONT_HERSHEY_DUPLEX, fontSize, cv::Scalar(255, 255, 255), 1, CV_AA);
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
	
		switch (mObjects[i]->mMode)
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
					cv::Mat resized = resize2Video(frame, mysize);
					cv::Mat prevFrame(mysize, CV_8UC3, cv::Scalar(0, 0, 0)), currFrame = resized;

					// put general annotation, effect to be refined
					if (mObjects[i]->isShowGeneral)
					{
						screenshotDict = mObjects[i]->mNotesPath;
						screenshotDict.append("_geo.png");
						currFrame = putSubtitle(resized, annotation.toStdString(), mysize, screenshotDict.toStdString());
						blend2Video(prevFrame, currFrame, outputVideo);
						for (int duration = 0; duration < 30*mFrameDuration2D; duration++)
						{
							if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
							outputVideo.write(currFrame);
						}
					}
					else
					{
						blend2Video(prevFrame, currFrame, outputVideo);
					}
					for (int new_noteid = 0; new_noteid < mObjects[i]->mNotes.size() - 1; new_noteid++)
					{
						int noteid = mObjects[i]->mNoteReorder[new_noteid];
						if (noteid < 0)
						{
							continue;
						}
						else if (noteid < pointNote2D.size())
						{
							generatePointNote2D(noteid, outputVideo, frame, prevFrame, currFrame, pointNote2D);
						}
						else if (noteid < pointNote2D.size() + surfaceNote2D.size())
						{
							generateSurfaceNote2D(noteid - pointNote2D.size(), outputVideo, frame, prevFrame, currFrame, surfaceNote2D);
						}
						else
						{
							generatePolygonNote2D(noteid - pointNote2D.size() - surfaceNote2D.size(), outputVideo, frame, prevFrame, currFrame, polygonNote2D);
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
					if (mObjects[i]->isShowGeneral)
					{
						QString geoScreenshot = mObjects[i]->mNotesPath;
						geoScreenshot.append("_geo.png");
						// generate screenshots from different angles
						for (int angle = 0; angle < 360; angle++)
						{
							qDebug() << "12222\n\n";
							mObjects[i]->mGla->setArbitraryView((double)angle);
							qDebug() << "23333\n\n";
							screenshotDict = screenshotObj;
							screenshotDict.append(QString::number(angle));
							qDebug() << "34444\n\n";
							mObjects[i]->mPictures.push_back(mObjects[i]->mGla->screenshot(screenshotDict));
							qDebug() << "45555\n\n";
							cv::Mat frame = cv::imread(screenshotDict.toStdString() + ".png", CV_LOAD_IMAGE_COLOR);
							cv::Mat resized = resize2Video(frame, mysize);
							qDebug() << "56666\n\n";
							currFrame = putSubtitle(resized, annotation.toStdString(), mysize, geoScreenshot.toStdString());
							// blending
							if (angle == 0)
							{
								blend2Video(prevFrame, currFrame, outputVideo);
							}
							if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
							outputVideo.write(currFrame);
							qDebug() << "67777\n\n";
						}
					}
					else
					{
						mObjects[i]->mGla->setArbitraryView(0);
						screenshotDict = screenshotObj;
						screenshotDict.append(QString::number(0));
						mObjects[i]->mPictures.push_back(mObjects[i]->mGla->screenshot(screenshotDict));
						cv::Mat frame = cv::imread(screenshotDict.toStdString() + ".png", CV_LOAD_IMAGE_COLOR);
						cv::Mat resized = resize2Video(frame, mysize);
						currFrame = resized;
						blend2Video(prevFrame, currFrame, outputVideo);
					}
					mObjects[i]->mGla->getCameraPos(currCam);
					mObjects[i]->mGla->computeNormals3D();
					for (int new_noteid = 0; new_noteid < mObjects[i]->mNotes.size() - 1; new_noteid++)
					{
						int noteid = mObjects[i]->mNoteReorder[new_noteid];
						if (noteid < 0)
						{
							continue;
						}
						else if (noteid < pointNote3D.size())
						{
							generatePointNote3D(i, noteid, outputVideo, prevCam, currCam, currFrame, screenshotObj, screenshotDict, pointNote3D);
						}
						else if (noteid < pointNote3D.size() + surfaceNote3D.size())
						{
							generateSurfaceNote3D(i, noteid - pointNote3D.size(), outputVideo, prevCam, currCam, currFrame, screenshotObj, screenshotDict, surfaceNote3D);
						}
						else
						{
							generateFrustumNote3D(i, noteid - pointNote3D.size() - surfaceNote3D.size(), outputVideo, prevCam, currCam, currFrame, screenshotObj, screenshotDict, frustumNote3D);
						}
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
					{
						cv::addWeighted(resized, angle/(double)blendingFrames, resized, 0, 0, resized);
					}
					else if (angle >= 360 - blendingFrames)
					{
						cv::addWeighted(resized, (359-angle)/(double)blendingFrames, resized, 0, 0, resized);
					}
					if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
					outputVideo.write(resized);
				}
				recoverWidget(mObjects[i]->mGla, info, true);
				break;
			case RTI2D:
				{
					mw()->mLightControlRTI->setLight(vcg::Point3f(0, 0, 1), true);
					RTIScreenShot = mObjects[i]->mGla->getRTIScreenShot();
					if (!RTIScreenShot.isNull())
					{
						screenshotDict = screenshotObj;
						screenshotDict.append(".png");
						RTIScreenShot.save(screenshotDict);
						mObjects[i]->mPictures.push_back(screenshotDict);
						cv::Mat frame, prevFrame(mysize, CV_8UC3, cv::Scalar(0, 0, 0)), currFrame;

						// put general annotation, effect to be refined
						if (mObjects[i]->isShowGeneral)
						{
							// play with RTI lighting
							for (int duration = 0; duration < 30*mFrameDuration2D; duration++)
							{
								mw()->mLightControlRTI->setLight(vcg::Point3f(cos(duration*2*M_PI/30/mFrameDuration2D) * sin(70*M_PI/180),
									sin(duration*2*M_PI/30/mFrameDuration2D) * sin(70*M_PI/180), cos(70*M_PI/180)), true);
								RTIScreenShot = mObjects[i]->mGla->getRTIScreenShot();
								screenshotDict = screenshotObj;
								screenshotDict.append(QString::number(duration) + ".png");
								RTIScreenShot.save(screenshotDict);
								mObjects[i]->mPictures.push_back(screenshotDict);
								frame = cv::imread(screenshotDict.toStdString(), CV_LOAD_IMAGE_COLOR);
								cv::Mat resized = resize2Video(frame, mysize);
								screenshotDict = mObjects[i]->mNotesPath;
								screenshotDict.append("_geo.png");
								currFrame = putSubtitle(resized, annotation.toStdString(), mysize, screenshotDict.toStdString());
								if (duration == 0)
								{
									blend2Video(prevFrame, currFrame, outputVideo);
								}
								if (!outputVideo.isOpened()) qDebug() << "ERROR: outputVideo not opened!\n\n";
								outputVideo.write(currFrame);
							}
							screenshotDict = screenshotObj;
							screenshotDict.append(".png");
							frame = cv::imread(screenshotDict.toStdString(), CV_LOAD_IMAGE_COLOR);
						}
						else
						{
							screenshotDict = screenshotObj;
							screenshotDict.append(".png");
							frame = cv::imread(screenshotDict.toStdString(), CV_LOAD_IMAGE_COLOR);
							cv::Mat resized = resize2Video(frame, mysize);
							currFrame = resized;
							blend2Video(prevFrame, currFrame, outputVideo);
						}
						prevFrame = currFrame;

						for (int new_noteid = 0; new_noteid < mObjects[i]->mNotes.size() - 1; new_noteid++)
						{
							int noteid = mObjects[i]->mNoteReorder[new_noteid];
							if (noteid < 0)
							{
								continue;
							}
							else if (noteid < pointNote2D.size())
							{
								generatePointNote2D(noteid, outputVideo, frame, prevFrame, currFrame, pointNote2D);
							}
							else if (noteid < pointNote2D.size() + surfaceNote2D.size())
							{
								generateSurfaceNote2D(noteid - pointNote2D.size(), outputVideo, frame, prevFrame, currFrame, surfaceNote2D);
							}
							else
							{
								generatePolygonNote2D(noteid - pointNote2D.size() - surfaceNote2D.size(), outputVideo, frame, prevFrame, currFrame, polygonNote2D);
							}
						}
					}
					break;
				}
			default: break;
		}

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
	// change the extension to ".mp4" due to limitations of OpenCV
	if (mVideoFormat == 1)
	{
		QFile videoMP4(mLocation);
		QString renameMP4 = mLocation.split("_.avi")[0] + ".mp4";
		qDebug() << renameMP4 << "\n";
		bool isRename = videoMP4.rename(renameMP4);
		if (!isRename)
		{
			qDebug() << "MP4 rename failed!\n";
		}
	}
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
		switch (view)
		{
			case FRONT3D: projection[2] += 100; break;
			case LEFT3D: projection[0] -= 100; break;
			case RIGHT3D: projection[0] += 100; break;
			case TOP3D: projection[1] += 100; break;
			case BOTTOM3D: projection[1] -= 100; break;
			case BACK3D: projection[2] -= 100; break;
		}

		vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();
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
	cv::Rect roi;
	switch (mResolutionOption)
	{
		default:
		case 0: roi.x = 0; roi.y = 480; roi.width = 800; roi.height = 120; break;
		case 1: roi.x = 0; roi.y = 660; roi.width = 1600; roi.height = 240; break;
		case 2: roi.x = 0; roi.y = 960; roi.width = 1600; roi.height = 240; break;
	}
	cv::Mat subtitle1 = annotated(roi);
	cv::Mat subtitle2 = blackImg(roi);
	cv::addWeighted(subtitle1, 0.7, subtitle2, 0.3, 0, subtitle1);
	int lineLen = (img == "" ? 85 : 60); //// TODO: revise the number of characters per line
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
		switch (mResolutionOption)
		{
			default:
			case 0: cv::putText(annotated, line, cv::Point(50, 500 + i * 20), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(255, 255, 255), 1, CV_AA); break;
			case 1: cv::putText(annotated, line, cv::Point(100, 700 + i * 40), cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(255, 255, 255), 1, CV_AA); break;
			case 2: cv::putText(annotated, line, cv::Point(100, 1000 + i * 40), cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(255, 255, 255), 1, CV_AA); break;
		}
	}
	if (img != "")
	{
		cv::Mat assoImg = cv::imread(img, CV_LOAD_IMAGE_COLOR);
		if (!assoImg.data) // check for invalid input
		{
			return putSubtitle(src, mystr, mysize);
		}
		switch (mResolutionOption)
		{
			default:
			case 0:
				assoImg = resize2Video(assoImg, cv::Size(120, 120));
				assoImg.copyTo(annotated.colRange(680, 680 + assoImg.size().width).rowRange(480, 480 + assoImg.size().height));
				break;
			case 1:
				assoImg = resize2Video(assoImg, cv::Size(240, 240));
				assoImg.copyTo(annotated.colRange(1360, 1360 + assoImg.size().width).rowRange(660, 660 + assoImg.size().height));
				break;
			case 2:
				assoImg = resize2Video(assoImg, cv::Size(240, 240));
				assoImg.copyTo(annotated.colRange(1360, 1360 + assoImg.size().width).rowRange(960, 960 + assoImg.size().height));
				break;
		}
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


void VideoGenerator::generatePointNote2D(int noteid, cv::VideoWriter& outputVideo, cv::Mat& frame, cv::Mat& prevFrame, cv::Mat& currFrame, QVector<QPair<QPair<int, int>, QString> >& pointNote2D)
{
	cv::Point notePos(pointNote2D[noteid].first.first, frame.size().height - pointNote2D[noteid].first.second);
	cv::Mat currNote = emphasizeNote(frame, notePos, frame.size().height / 20);
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
void VideoGenerator::generateSurfaceNote2D(int noteid, cv::VideoWriter& outputVideo, cv::Mat& frame, cv::Mat& prevFrame, cv::Mat& currFrame, QVector<QPair<int*, QString> >& surfaceNote2D)
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
void VideoGenerator::generatePolygonNote2D(int noteid, cv::VideoWriter& outputVideo, cv::Mat& frame, cv::Mat& prevFrame, cv::Mat& currFrame, QVector<QPair<QVector<QPair<int, int> >, QString> >& polygonNote2D)
{
	cv::Point center(0, 0); 
	for (int vid = 0; vid < polygonNote2D[noteid].first.size() - 1; vid++)
	{
		center.x += polygonNote2D[noteid].first[vid].first;
		center.y += polygonNote2D[noteid].first[vid].second;
	}
	center.x = center.x / (polygonNote2D[noteid].first.size() - 1);
	center.y = center.y / (polygonNote2D[noteid].first.size() - 1);
	int radius = frame.size().height / 20;
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

void VideoGenerator::generatePointNote3D(int i, int noteid, cv::VideoWriter& outputVideo, double* prevCam, double* currCam, cv::Mat& currFrame, QString& screenshotObj, QString& screenshotDict, QVector<QPair<QPair<int, double*>, QString> >& pointNote3D)
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
void VideoGenerator::generateSurfaceNote3D(int i, int noteid, cv::VideoWriter& outputVideo, double* prevCam, double* currCam, cv::Mat& currFrame, QString& screenshotObj, QString& screenshotDict, QVector<QPair<QPair<int, double*>, QString> >& surfaceNote3D)
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
void VideoGenerator::generateFrustumNote3D(int i, int noteid, cv::VideoWriter& outputVideo, double* prevCam, double* currCam, cv::Mat& currFrame, QString& screenshotObj, QString& screenshotDict, QVector<QPair<double*, QString> >& frustumNote3D)
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