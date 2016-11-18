/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Weiqi Shi (weiqi.shi@yale.edu)
			 Zeyu Wang (zeyu.wang@yale.edu)

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
#include <QMessageBox>
#include "projectCITab.h"
#include "../vtkEnums.h"

ProjectClassifiedInfoTab::ProjectClassifiedInfoTab(const QString path, const QString userName, QWidget *parent)
	: QWidget(parent)
{
	mVBox = new QVBoxLayout();
	mGrid = new QGridLayout();
	mCHELabel = new QLabel("Cultural Heritage Entity");
	mCategoryLabel = new QLabel("Category");
	mCHEFilter = new QComboBox();
	mCategoryFilter = new QComboBox();
	connect(mCHEFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(setCHEFilterMode(int)));
	connect(mCategoryFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(setCategoryFilterMode(int)));
	mTextEdit = new QTextEdit();
	mGrid->addWidget(mCHELabel, 0, 0);
	mGrid->addWidget(mCategoryLabel, 0, 1);
	mGrid->addWidget(mCHEFilter, 1, 0);
	mGrid->addWidget(mCategoryFilter, 1, 1);
	mVBox->addLayout(mGrid);
	mVBox->addWidget(mTextEdit);
	this->setLayout(mVBox);
	this->show();
	mPath = path;
	mPath.append(QDir::separator() + QString("information.xml"));
	mFile = new QFile(mPath);
	if (mFile->exists())
		loadCHEInfo();
	else
		initFile(userName);
}

void ProjectClassifiedInfoTab::addCHEInfo(const QString CHEName, QVector<QString> categories, QVector<QString> contents)
{
	if (mClassifiedInfo.find(CHEName) != mClassifiedInfo.end())
	{
		QVector<QString> existCategory;
		for (int i = 0; i < mClassifiedInfo[CHEName].size(); i++)
			existCategory.push_back(mClassifiedInfo[CHEName][i].first);
		for (int i = 0; i < categories.size(); i++)
		{
			if (existCategory.indexOf(categories[i]) == -1)
			{
				QString item = QString(categories[i]);
				mClassifiedInfo[CHEName].push_back(qMakePair(item, contents[i]));
			}
		}
		mCHEFilter->setCurrentIndex(mCHEFilter->findData(CHEName));
		setCHEFilterMode(mCHEFilter->findData(CHEName));
	}
	else 
	{
		mCHEFilter->addItem(CHEName);
		QVector<QPair<QString, QString> > mCategories;
		for (int i = 0; i < categories.size(); i++)
		{
			QString item = QString(categories[i]);
			mCategories.push_back(qMakePair(item, contents[i]));
		}
		mClassifiedInfo[CHEName] = mCategories;
		mCHEFilter->setCurrentIndex(mCHEFilter->count()-1);
		setCHEFilterMode(mCHEFilter->count()-1);
	}
}

void ProjectClassifiedInfoTab::removeCHEInfo(const QString CHEName)
{
	int index = mCHEFilter->findText(CHEName);
	if (index != -1)
	{
		mCHEFilter->removeItem(index);
		mClassifiedInfo.remove(CHEName);
	}
	if (mCHEFilter->count())
	{
		mCHEFilter->setCurrentIndex(0);
	}
	else
	{
		mCategoryFilter->clear();
		mTextEdit->clear();
	}
}

void ProjectClassifiedInfoTab::clear()
{
	mCHEFilter->clear();
	mCategoryFilter->clear();
	mTextEdit->clear();
}

void ProjectClassifiedInfoTab::initFile(const QString userName)
{
    QDomElement root;
    QDomProcessingInstruction instruct = mDoc.createProcessingInstruction("xml", "version=\"1.0\"");
    mDoc.appendChild(instruct);

	root = mDoc.createElement("CHEROb.classified_information");
	QStringList elements = mPath.split(QDir::separator());
	root.setAttribute("name", elements[elements.size() - 2]);
	
    root.setAttribute("date", QDateTime::currentDateTimeUtc().toString());
	root.setAttribute("user", userName);
    mDoc.appendChild(root);

	if(!mFile->isOpen() && !mFile->open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox mb;
			mb.critical(this, tr("Project Error"), tr("Failed to create information.xml file."));
        mFile->close();
        return;
    }

	mFile->resize(0);
    QTextStream out(mFile);
    out << mDoc.toString();
    mFile->close();
}

void ProjectClassifiedInfoTab::loadCHEInfo()
{
	mDoc.setContent(mFile);
	if(!mFile->isOpen() && !mFile->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox::critical(this, tr("Project Error"), tr("Cannot open information.xml file."));
		return;
	}
	QDomNodeList list = mDoc.elementsByTagName("CHEROb.classified_information");
	if (list.isEmpty())
	{
		QString message = mFile->fileName() + tr(" is not a valid CHEROb categorized information file.");
		QMessageBox::critical(this, tr("Project Error"), message);
		return;
	}
	QDomElement root = list.at(0).toElement();
	QDomNodeList CHEInfo = root.elementsByTagName("cultural_heritage_entity");
	for (int i = 0; i < CHEInfo.size(); i++)
	{
		QDomElement mCHEInfo = CHEInfo.at(i).toElement();
		QString CHEName = mCHEInfo.attribute("name");
		QDomNodeList childeNodes = mCHEInfo.childNodes();
		QVector<QString> categories, contents;
		qDebug()<<"load category xml"<<childeNodes.length();
		for (int j = 0; j < childeNodes.size(); j++)
		{
			QString category = childeNodes.at(j).toElement().tagName();
			QString content = childeNodes.at(j).toElement().text();
			categories.push_back(category);
			contents.push_back(content);
		}
		addCHEInfo(CHEName, categories, contents);
	}
	mCHEFilter->setCurrentIndex(0);
	setCHEFilterMode(0);
	mFile->close();
}

void ProjectClassifiedInfoTab::addCHEInfoToFile(const QString CHEName)
{
	QVector<QPair<QString, QString> > mCategories = mClassifiedInfo[CHEName];

	QDomElement root = mDoc.elementsByTagName("CHEROb.classified_information").at(0).toElement();
	QDomNodeList CHEInfos = root.elementsByTagName("cultural_heritage_entity");
	QDomElement mCHEInfo;
	bool isFound = false;
	if (CHEInfos.length() != 0)
	{
		for (int i = 0; i < CHEInfos.size(); i++)
		{
			
			if (CHEName == CHEInfos.at(i).toElement().attribute("name"))
			{
				qDebug()<<"in add CHE to file"<<CHEInfos.at(i).toElement().attribute("name")<<CHEName;
				QDomNodeList childNodes = CHEInfos.at(i).toElement().childNodes();
				for (int j = 0; j < childNodes.size(); j++)
					childNodes.at(j).toElement().clear();
				mCHEInfo = CHEInfos.at(i).toElement();
				isFound = true;
				break;
			}
		}
	}

	if (!isFound)
	{	
		mCHEInfo = mDoc.createElement("cultural_heritage_entity");
		mCHEInfo.setAttribute("name", CHEName);
		root.appendChild(mCHEInfo);
	}

	for (int i = 0; i < mCategories.size(); i++)
	{
		QString category = mCategories[i].first;
		QDomElement object = mDoc.createElement(category);
		object.appendChild(mDoc.createTextNode(mCategories[i].second));
		mCHEInfo.appendChild(object);
	}
	if(!mFile->isOpen() && !mFile->open(QFile::WriteOnly | QFile::Text)) 
	{
        QMessageBox mb;
			mb.critical(this, tr("Project Error"), tr("Failed to create information.xml file."));
        mFile->close();
        return;
    }

	mFile->resize(0);
    QTextStream out(mFile);
    out << mDoc.toString();
    mFile->close();
}

void ProjectClassifiedInfoTab::setCHEFilterMode(int mode)
{
	if (mode < 0)
		return;
	mCategoryFilter->clear();
	QVector<QPair<QString, QString> > categories = mClassifiedInfo[mCHEFilter->itemText(mode)];
	if (categories.size())
	{
		for (int i = 0; i < categories.size(); i++)
		{
			mCategoryFilter->addItem(categories[i].first);
		}
		mCategoryFilter->setCurrentIndex(0);
	}
}

void ProjectClassifiedInfoTab::setCategoryFilterMode(int mode)
{
	if (mode < 0)
		return;
	QString CHEName = mCHEFilter->itemText(mCHEFilter->currentIndex());
	mCategoryFilter->setCurrentIndex(mode);
	mTextEdit->setText(mClassifiedInfo[CHEName][mode].second);
}