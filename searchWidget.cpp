/****************************************************************************

 - Codename: Hyper3D (Yale Computer Graphics Group)

 - Writers:   Min H. Kim (minhkim@cs.yale.edu)

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
   anonymous help by various software engineering communities.

*****************************************************************************/
#include <QMouseEvent>

#include "searchWidget.h"
#include "informationWidget.h"
#include "mainWindow.h"
#define MATCHSIZE 40

SearchWidget::SearchWidget(QWidget *parent)
    : QWidget(parent)
{
	//int width = this->frameGeometry().width();
	//int height = this->frameGeometry().height();

	mLabel = new QLabel(tr("Please input the keyword to search in current object files"));
	mInput = new QLineEdit();
	mInput->setFixedWidth(200);
    mLabel->setBuddy(mInput);
	mButton = new QPushButton("Search");
	mTreeWidget = new QTreeWidget();
	mTreeWidget->setColumnCount(5);
	mTreeWidget->setColumnWidth(0, 200);
	mTreeWidget->setColumnWidth(1, 80);
	mTreeWidget->setColumnWidth(2, 70);
	mTreeWidget->setColumnWidth(3, 150);
	mTreeWidget->setColumnWidth(4, 100);
	QStringList ColumnNames;
	ColumnNames << "Result" << "Object" << "Category" << "Path" << "File";
	mTreeWidget->setHeaderLabels(ColumnNames);
	mTreeWidget->setSortingEnabled(true);
	connect(mButton, SIGNAL(clicked()), this, SLOT(search()));
	connect(mTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(showTreeWidgetItem(QTreeWidgetItem*, int)));
	
	hlay = new QHBoxLayout();
	vlay = new QVBoxLayout();
	hlay->addWidget(mInput, 0 , Qt::AlignLeft);
	hlay->addWidget(mButton, 0 , Qt::AlignLeft);
	vlay->addWidget(mLabel);
	vlay->addLayout(hlay);
	vlay->addWidget(mTreeWidget);
	refreshSearchTab();
	this->setLayout(vlay);
	this->show();
}

void SearchWidget::showTreeWidgetItem(QTreeWidgetItem* item, int column)
{
	mw()->mInformation->openNoteFromTreeWidget(item);
}

void SearchWidget::search()
{
	if (!updateCurrentPath() || !mInput->isModified() || mInput->text() == QString())
		return;
	mTreeWidget->clear();
	mItems.clear();
	QRegExp text(mInput->text());
	int inputSize = mInput->text().size();
	int spareSize = (MATCHSIZE - inputSize) / 2;
	int category = -1;
	QDir dir(mPath);
	dir.setNameFilters(QStringList()<<"*.txt");
	dir.setSorting(QDir::Name|QDir::LocaleAware);
	QStringList fileList = dir.entryList();

	for (int i = 0; i < fileList.size(); ++i)
	{
		QString fileName = mPath;
		fileName.append(QDir::separator() + fileList[i]);
		qDebug() << fileName;
		QFile* file = new QFile(fileName);
		if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
		{
			continue;
		}
		QTextStream in(file);
		if (fileList[i] != QString("Annotation.txt"))
		{
			while(1)
			{
				QString signal = in.readLine();
				if (signal == QString("Color Type:"))
					break;
			}
			in >> category;
			while(1)
			{
				QString signal = in.readLine();
				if (signal == QString("Note Start:"))
					break;
			}
		}
		QString content = in.readAll();
		int size = content.size();
		int cur = 0;
		while (cur < size)
		{	
			int pos = content.indexOf(text, cur);
			if (pos == -1)
				break;
			cur = pos + inputSize;
			QString match;
			if (pos < spareSize)
			{
				match = content.left(MATCHSIZE);
				if (size > MATCHSIZE)
					match.append("...");
			}
			else if (pos > size - spareSize)
			{
				match = content.right(MATCHSIZE);
				if (size > MATCHSIZE)
					match.prepend("...");
			}
			else
			{
				match = content.mid(pos - spareSize, MATCHSIZE);
				match.append("...");
				match.prepend("...");
			}

			QStringList list;
			list.append(match);
			list.append(mFile);
			list.append(QString::number(category));
			list.append(mPath);
			list.append(fileList[i]);
			QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, list);
			mItems.append(item);
		}
	}
	mTreeWidget->insertTopLevelItems(0, mItems);
	mTreeWidget->sortByColumn(4, Qt::AscendingOrder);

	
}

void SearchWidget::refreshSearchTab()
{
	if(!mw()->VTKA() )
	{
		mTreeWidget->clear();
		mItems.clear();
		mPath = QString("");
		mFile = QString("");
		mInput->clear();
		mButton->setEnabled(false);
		mInput->setEnabled(false);
		mTreeWidget->setEnabled(false);
		return;
	}
	mButton->setEnabled(true);
	mInput->setEnabled(true);
	mTreeWidget->setEnabled(true);
}

bool SearchWidget::updateCurrentPath()
{
	QString fn;
	if(mw()->VTKA()) {
        mPath = mw()->VTKA()->mProjectPath;
		QStringList list = mw()->VTKA()->mFilename.split(QDir::separator());
		mFile = list[list.size()-1];
	}
	else
	{
		qDebug() << "Error: No Object files can be searched";
		mPath = QString("");
		mFile = QString("");
		return false;
	}

	mPath.append(QDir::separator() + QString("Note"));	// Only search text in notes for now
	return true;
}

MainWindow* SearchWidget::mw()
{
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
    if (mainwindow)
    {
      return mainwindow;
    }
  }
}

SearchAllWidget::SearchAllWidget(QWidget *parent)
    : QWidget(parent)
{
	//int width = this->frameGeometry().width();
	//int height = this->frameGeometry().height();

	mLabel = new QLabel(tr("Please input the keyword to search in the whole Project"));
	mInput = new QLineEdit();
	mInput->setFixedWidth(200);
    mLabel->setBuddy(mInput);
	mButton = new QPushButton("Search");
	mTreeWidget = new QTreeWidget();
	mTreeWidget->setColumnCount(5);
	mTreeWidget->setColumnWidth(0, 200);
	mTreeWidget->setColumnWidth(1, 80);
	mTreeWidget->setColumnWidth(2, 70);
	mTreeWidget->setColumnWidth(3, 150);
	mTreeWidget->setColumnWidth(4, 100);
	QStringList ColumnNames;
	ColumnNames << "Result" << "Object" << "Category" << "Path" << "File";
	mTreeWidget->setHeaderLabels(ColumnNames);
	mTreeWidget->setSortingEnabled(true);
	connect(mButton, SIGNAL(clicked()), this, SLOT(search()));
	connect(mTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(showTreeWidgetItem(QTreeWidgetItem*, int)));
	
	hlay = new QHBoxLayout();
	vlay = new QVBoxLayout();
	hlay->addWidget(mInput, 0 , Qt::AlignLeft);
	hlay->addWidget(mButton, 0 , Qt::AlignLeft);
	vlay->addWidget(mLabel);
	vlay->addLayout(hlay);
	vlay->addWidget(mTreeWidget);
	refreshSearchTab();
	this->setLayout(vlay);
	this->show();
}

void SearchAllWidget::showTreeWidgetItem(QTreeWidgetItem* item, int column)
{
	mw()->mInformation->openNoteFromTreeWidget(item);
}

QStringList SearchAllWidget::matchString(QString content, QString matchStr)
{
	QRegExp text(matchStr);
	int inputSize = matchStr.size();
	int spareSize = (MATCHSIZE - inputSize) / 2;
	int size = content.size();
	int cur = 0;
	QStringList matchList;
	while (cur < size)
	{	
		int pos = content.indexOf(text, cur);
		if (pos == -1)
			break;
		cur = pos + inputSize;
		QString match;
		if (pos < spareSize)
		{
			match = content.left(MATCHSIZE);
			if (size > MATCHSIZE)
				match.append("...");
		}
		else if (pos > size - spareSize)
		{
			match = content.right(MATCHSIZE);
			if (size > MATCHSIZE)
				match.prepend("...");
		}
		else
		{
			match = content.mid(pos - spareSize, MATCHSIZE);
			match.append("...");
			match.prepend("...");
		}
		matchList.append(match);
	}
	return matchList;
}

void SearchAllWidget::search()
{
	if (!updateCurrentPath() || !mInput->isModified() || mInput->text() == QString())
		return;
	mTreeWidget->clear();
	mItems.clear();
	QString text = mInput->text();
	int category = -1;
	// search in annotation and notes
	QDir dir(mPath);
	QStringList subDirName = dir.entryList();
	for (int k = 0; k < subDirName.size(); k++)
	{
		if (subDirName[k] == QString(".") || subDirName[k] == QString(".."))
			continue;
		QDir subDir(subDirName[k]);
		if (!subDir.exists())
			continue;
		QDir projectDir(subDir.absolutePath());
		mFile = projectDir.dirName();
		qDebug()<<"sub dir"<<projectDir.absolutePath();
		if (!projectDir.cd("Note"))
			continue;

		projectDir.setNameFilters(QStringList()<<"*.txt");
		projectDir.setSorting(QDir::Name|QDir::LocaleAware);
		QStringList fileList = projectDir.entryList();
		mPath = QDir::toNativeSeparators(projectDir.absolutePath());
		for (int i = 0; i < fileList.size(); ++i)
		{
			QString fileName = mPath;
			fileName.append(QDir::separator() + fileList[i]);
			qDebug() << fileName;
			QFile* file = new QFile(fileName);
			if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
			{
				continue;
			}
			QTextStream in(file);
			if (fileList[i] != QString("Annotation.txt"))
			{
				while(1)
				{
					QString signal = in.readLine();
					if (signal == QString("Color Type:"))
						break;
				}
				in >> category;
				while(1)
				{
					QString signal = in.readLine();
					if (signal == QString("Note Start:"))
						break;
				}
			}
			QString content = in.readAll();
			QStringList match = matchString(content, text);
			for (int j = 0; j < match.size(); j++)
			{
				QStringList list;
				list.append(match[j]);
				list.append(mFile);
				list.append(QString::number(category));
				list.append(mPath);
				list.append(fileList[i]);
				QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, list);
				mItems.append(item);
			}
		}
	}
	//search in project info
	QString projectName, location, keyword, affiliation, userName, description;
	mw()->getProjectInfo(projectName, location, keyword, affiliation, userName, description);

	QStringList match = matchString(projectName, text);
	for (int i = 0; i < match.size(); i++)
	{
		QStringList list;
		list.append(match[i]);
		list.append(QString("Project Name"));
		list.append(QString::number(-2));
		list.append(QString("Project Info"));
		QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, list);
		mItems.append(item);
	}

	match = matchString(keyword, text);
	for (int i = 0; i < match.size(); i++)
	{
		QStringList list;
		list.append(match[i]);
		list.append(QString("Keyword"));
		list.append(QString::number(-2));
		list.append(QString("Project Info"));
		QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, list);
		mItems.append(item);
	}

	match = matchString(affiliation, text);
	for (int i = 0; i < match.size(); i++)
	{
		QStringList list;
		list.append(match[i]);
		list.append(QString("Affiliation"));
		list.append(QString::number(-2));
		list.append(QString("Project Info"));
		QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, list);
		mItems.append(item);
	}

	match = matchString(userName, text);
	for (int i = 0; i < match.size(); i++)
	{
		QStringList list;
		list.append(match[i]);
		list.append(QString("User Name"));
		list.append(QString::number(-2));
		list.append(QString("Project Info"));
		QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, list);
		mItems.append(item);
	}

	match = matchString(description, text);
	for (int i = 0; i < match.size(); i++)
	{
		QStringList list;
		list.append(match[i]);
		list.append(QString("Description"));
		list.append(QString::number(-2));
		list.append(QString("Project Info"));
		QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, list);
		mItems.append(item);
	}

	//Search in File info has not supported yet

	mTreeWidget->insertTopLevelItems(0, mItems);
	mTreeWidget->sortByColumn(4, Qt::AscendingOrder);

	
}

void SearchAllWidget::refreshSearchTab()
{
	if(!mw()->VTKA() )
	{
		mTreeWidget->clear();
		mItems.clear();
		mPath = QString("");
		mFile = QString("");
		mInput->clear();
		mButton->setEnabled(false);
		mInput->setEnabled(false);
		mTreeWidget->setEnabled(false);
		return;
	}
	mButton->setEnabled(true);
	mInput->setEnabled(true);
	mTreeWidget->setEnabled(true);
}

bool SearchAllWidget::updateCurrentPath()
{
	QString fn;
	if(mw()->VTKA()) {
        mPath = mw()->VTKA()->mProjectPath;
		int slashpos = mPath.lastIndexOf(QDir::separator());
		mPath.chop(mPath.length() - slashpos);
	}
	else
	{
		qDebug() << "Error: No Object files can be searched";
		mPath = QString("");
		mFile = QString("");
		return false;
	}
	return true;
}

MainWindow* SearchAllWidget::mw()
{
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
    if (mainwindow)
    {
      return mainwindow;
    }
  }
}