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
#include "reportGenerator.h"

ReportGenerator::ReportGenerator(QString path, bool project)
{
	isProject = project;
	mLocation = path;
    mPrinter = new QPrinter(QPrinter::PrinterResolution);
    mPrinter->setOutputFormat(QPrinter::PdfFormat);
    mPrinter->setPaperSize(QPrinter::A4);
    mPrinter->setOutputFileName(mLocation);
 
    mDoc = new QTextDocument();
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
		QVector<QString> contents;
		QVector<QPair<int, int> > pointNote2D;
		QVector<int*> surfaceNote2D;
		//Parsing Notes
		for (int j = 0; j < mObjects[i]->mNotes.size(); j++)
		{
			QString note = mObjects[i]->mNotes[j];
			if (j == 0)
				annotation = note;
			else
			{
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
					}
					else
					{
						qDebug()<<"Parsing Error in report generation: Note Type Error!"<<firstLine;
						continue;
					}
				}
				else if (mObjects[i]->mMode == MODEL3D)
				{
				}
				int index = note.indexOf("Note Start:\n");
				if (index == -1)
					continue;
				note.remove(0, index+12);
				contents.push_back(note);
			}
		}
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
			painter.end(); 
		    mDoc->addResource(QTextDocument::ImageResource, QUrl(url), img);
			double height = 200, width = 300;
			height = (double)width * imgHeight / imgWidth;
			html = html + QString("<p><div align=\"center\"><img src=\"" + url + "\"width=\"" + QString::number(width) + "\" height=\"" + QString::number(height) + "\"></div></p>\n");
		}
		//Add annotation and notes
		if (!annotation.isEmpty())
			html = html + QString("<p><font size=\"3\">\nAnnotation " + QString(": ") + annotation + QString("\n</font></p>\n"));
		for (int j = 0; j < contents.size(); j++)
		{
			html = html + QString("<p><font size=\"3\">\nNote " +  QString::number(j+1) + QString(": ") + contents[j] + QString("\n</font></p>\n"));
		}
		for (int j = 0; j < surfaceNote2D.size(); j++)
		{
			delete surfaceNote2D[j];
		}
		
	}
	mDoc->setHtml(html);
	mDoc->setPageSize(mPrinter->pageRect().size()); // This is necessary if you want to hide the page number
    mDoc->print(mPrinter);
}