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
		for (int j = 0; j < mObjects[i]->mPictures.size(); j++)
		{
			QImage img(mObjects[i]->mPictures[j].first); 
			QStringList nameElements = mObjects[i]->mPictures[j].first.split(QDir::separator());
			QString url = nameElements[nameElements.size() - 1];
			mDoc->addResource(QTextDocument::ImageResource, QUrl(url), img);
			double height = 200, width = 300;
			if (mObjects[i]->mPictures[j].second != -1)
				height = width * mObjects[i]->mPictures[j].second;
			html = html + QString("<p><div align=\"center\"><img src=\"" + url + "\"width=\"" + QString::number(width) + "\" height=\"" + QString::number(height) + "\"></div></p>\n");
		}
		for (int j = 0; j < mObjects[i]->mNotes.size(); j++)
		{
			QString note = mObjects[i]->mNotes[j];
			if (j == 0)
			{
				html = html + QString("<p><font size=\"3\">\nAnnotation " + QString(": ") + note + QString("\n</font></p>\n"));
			}
			else
			{
				int index = note.indexOf("Note Start:\n");
				if (index == -1)
					continue;
				note.remove(0, index+12);
				html = html + QString("<p><font size=\"3\">\nNote " +  QString::number(j) + QString(": ") + note + QString("\n</font></p>\n"));
			}
		}
	}
	mDoc->setHtml(html);
	mDoc->setPageSize(mPrinter->pageRect().size()); // This is necessary if you want to hide the page number
    mDoc->print(mPrinter);
}