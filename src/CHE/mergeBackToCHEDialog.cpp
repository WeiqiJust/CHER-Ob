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
#include "mergeBackToCHEDialog.h"
#include "exportToProjectDialog.h"

MergeBackToCHEDialog::MergeBackToCHEDialog(QMap<QString, QString> objects)
{
	mDialog = new QDialog();
	mVbox = new QVBoxLayout();

	mDialog->setWindowTitle(tr("Merge Back to Cultural Heritage Entity"));

	mLabel = new QLabel("Please select the objects and categories to be merged back to cultural heritage entity\n Notice ");

	mTreeWidget = new QTreeWidget();
	QStringList ColumnNames;
	mTreeWidget->setColumnCount(2);
	ColumnNames << "Object" << "Location";
	mTreeWidget->setHeaderLabels(ColumnNames);
	mTreeWidget->setColumnWidth(0, 275);
	mTreeWidget->setColumnWidth(1, 120);
	mObject = objects;
	for (QMap<QString, QString>::iterator it = objects.begin(); it != objects.end(); it++)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, QString(it.key()));
		item->setText(1, QString(it.value()));
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
	mImportButton = new QPushButton("Merge");
	connect(mImportButton, SIGNAL(clicked()), this, SLOT(merge()));
	mCancelButton = new QPushButton("Cancel");
	connect(mCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	mSelectAllButton = new QPushButton("Select All");
	connect(mSelectAllButton, SIGNAL(clicked()), this, SLOT(selectAll()));
	mButtonGridBox->addWidget(mSelectAllButton, 0, 5, 1, 1,  Qt::AlignVCenter | Qt::AlignLeft);
	mButtonGridBox->addWidget(mImportButton, 0, 5, 1, 4,  Qt::AlignVCenter | Qt::AlignRight);
	mButtonGridBox->addWidget(mCancelButton, 0, 5, 1, 5, Qt::AlignVCenter | Qt::AlignRight);
	
	mVbox->addWidget(mLabel);
	mVbox->addWidget(mTreeWidget);
	mVbox->addLayout(mButtonGridBox);
	mDialog->setMinimumHeight(400);
	mDialog->setMinimumWidth(450);
	mDialog->setLayout(mVbox);
}

QMap<QString, QString> MergeBackToCHEDialog::getMergeList()
{
	QMap<QString, QString> mergeList;
	for (QMap<QString, QString>::iterator it = mObject.begin(); it != mObject.end(); it++)
	{
		if (mSelected.keys().indexOf(it.key()) != -1)
			mergeList[it.key()] = it.value();
	}
	return mergeList;
}

QVector<int> MergeBackToCHEDialog::getCategories(const QString object)
{
	QMap<QString, QVector<int> >::const_iterator it = mSelected.find(object);
	if (it != mSelected.end())
		return it.value();
	QVector<int> empty;
	return empty;
}

void MergeBackToCHEDialog::merge()
{
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
}


void MergeBackToCHEDialog::cancel()
{
	mDialog->hide();
}

void MergeBackToCHEDialog::selectAll()
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

void MergeBackToCHEDialog::itemChanged(QTreeWidgetItem * item, int column)
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


