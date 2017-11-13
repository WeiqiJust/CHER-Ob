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
#include "videoFilter.h"
#include "../CHE/exportToProjectDialog.h"
#include <QDebug>

VideoFilter::VideoFilter(QVector<QString> objects)
{
	mDialog = new QDialog();
	mVbox = new QVBoxLayout();

	mDialog->setWindowTitle(tr("Video Filter"));

	mLabel = new QLabel("Please select the objects and categories to be presented in video");

	mTreeWidget = new dragTreeWidget();
	// Experimental drag-and-drop by Zeyu
	mTreeWidget->setDragEnabled(true);
	mTreeWidget->viewport()->setAcceptDrops(true);
	mTreeWidget->setDropIndicatorShown(true);
	mTreeWidget->setDragDropMode(QAbstractItemView::InternalMove);

	QStringList ColumnNames;
	ColumnNames << "Object";
	mTreeWidget->setHeaderLabels(ColumnNames);
	mObject = objects;
	for (int i = 0; i < objects.size(); i++)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, QString(objects[i]));
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(0, Qt::Unchecked);
		for (int j = 0; j < 11; j++)
		{
			QTreeWidgetItem *child = new QTreeWidgetItem();
			child->setText(0, QString(num2category(j).c_str()));
			child->setFlags(child->flags() | Qt::ItemIsUserCheckable);
			child->setCheckState(0, Qt::Unchecked);
			item->addChild(child);
		}
		mItems.push_back(item);
	}
	mTreeWidget->insertTopLevelItems(0, mItems);
	connect(mTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem* , int)), this, SLOT(itemChanged(QTreeWidgetItem* , int)));

	mButtonGridBox = new QGridLayout();
	mGenerateButton = new QPushButton("Generate");
	connect(mGenerateButton, SIGNAL(clicked()), this, SLOT(generate()));
	mCancelButton = new QPushButton("Cancel");
	connect(mCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	mSelectAllButton = new QPushButton("Select All");
	connect(mSelectAllButton, SIGNAL(clicked()), this, SLOT(selectAll()));
	mButtonGridBox->addWidget(mSelectAllButton, 0, 4, 1, 1,  Qt::AlignVCenter | Qt::AlignLeft);
	mButtonGridBox->addWidget(mGenerateButton, 0, 4, 1, 3,  Qt::AlignVCenter | Qt::AlignRight);
	mButtonGridBox->addWidget(mCancelButton, 0, 4, 1, 4, Qt::AlignVCenter | Qt::AlignRight);
	
	mVbox->addWidget(mLabel);
	mVbox->addWidget(mTreeWidget);
	mVbox->addLayout(mButtonGridBox);
	mDialog->setMinimumWidth(250);
	mDialog->setLayout(mVbox);
	isGenerate = false;
}

QVector<QString> VideoFilter::getFilterList()
{
	QVector<QString> filter;
	for (int i = 0; i < mObject.size(); i++)
	{
		if (mSelected.keys().indexOf(mObject[i]) == -1)
			filter.push_back(mObject[i]);
	}
	return filter;
}

QVector<int> VideoFilter::getCategories(const QString object)
{
	QMap<QString, QVector<int> >::const_iterator it = mSelected.find(object);
	if (it != mSelected.end())
		return it.value();
	QVector<int> empty;
	return empty;
}

void VideoFilter::generate()
{
	for (int i = 0; i < mItems.size(); i++)
	{
		for (int k = 0; k < mItems.size(); k++)
		{
			if (mTreeWidget->topLevelItem(i)->text(0) == mItems[k]->text(0))
			{
				dragPermutation.push_back(k);
				break;
			}
		}
	}
	for (int i = 0; i < mItems.size(); i++)
	{
		if (mItems[i]->checkState(0) == Qt::Checked)
		{
			QVector<int> categories;
			for (int j = 0; j < mItems[i]->childCount(); j++)
			{
				if (mItems[i]->child(j)->checkState(0) == Qt::Checked)
				{
					categories.push_back(j);
				}
			}
			mSelected[mItems[i]->text(0)] = categories;
		}
	}
	mDialog->hide();
	isGenerate = true;
}


void VideoFilter::cancel()
{
	mDialog->hide();
}

void VideoFilter::selectAll()
{
	for (int i = 0; i < mItems.size(); i++)
	{
		mItems[i]->setCheckState(0, Qt::Checked);
		for (int j = 0; j < mItems[i]->childCount(); j++)
		{
			mItems[i]->child(j)->setCheckState(0, Qt::Checked);
		}
	}
}

void VideoFilter::itemChanged(QTreeWidgetItem* item, int column)
{
	if (mItems.indexOf(item) != -1)
	{
		if (item->checkState(column) == Qt::Unchecked)
		{
			for (int i = 0; i < item->childCount(); i++)
			{
				item->child(i)->setCheckState(column, Qt::Unchecked);
			}
		}
		if (item->checkState(column) == Qt::Checked)
		{
			for (int i = 0; i < item->childCount(); i++)
			{
				item->child(i)->setCheckState(column, Qt::Checked);
			}
		}
	}
}
