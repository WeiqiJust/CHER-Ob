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
#include "videoNoteFilter.h"
#include "../CHE/exportToProjectDialog.h"
#include <QDebug>

VideoNoteFilter::VideoNoteFilter(VideoObject* mainVideoObject)
{
	thisVideoObject = mainVideoObject;

	mDialog = new QDialog();
	mVbox = new QVBoxLayout();

	mDialog->setWindowTitle(tr("Video Note Filter"));

	mLabel = new QLabel("Please select the notes to be presented in video");

	mTreeWidget = new dragTreeWidget();
	//// Zeyu playing with drag-and-drop
	mTreeWidget->setDragEnabled(true);
	mTreeWidget->viewport()->setAcceptDrops(true);
	mTreeWidget->setDropIndicatorShown(true);
	mTreeWidget->setDragDropMode(QAbstractItemView::InternalMove);

	QStringList ColumnNames;
	ColumnNames << "Note";
	mTreeWidget->setHeaderLabels(ColumnNames);
	for (int i = 1; i < thisVideoObject->mNotes.size(); i++)
	{
		QString text = thisVideoObject->mNotes[i].first.split("\nLinked Images:\n")[0].split("Note Start:\n")[1];
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, text);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(0, Qt::Checked);
		mItems.push_back(item);
	}
	mTreeWidget->insertTopLevelItems(0, mItems);

	mButtonGridBox = new QGridLayout();
	mGenerateButton = new QPushButton("Generate");
	connect(mGenerateButton, SIGNAL(clicked()), this, SLOT(generate()));
	mCancelButton = new QPushButton("Cancel");
	connect(mCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	mSkipAllButton = new QPushButton("Skip All");
	connect(mSkipAllButton, SIGNAL(clicked()), this, SLOT(skipAll()));
	mButtonGridBox->addWidget(mSkipAllButton, 0, 4, 1, 1,  Qt::AlignVCenter | Qt::AlignLeft);
	mButtonGridBox->addWidget(mGenerateButton, 0, 4, 1, 3,  Qt::AlignVCenter | Qt::AlignRight);
	mButtonGridBox->addWidget(mCancelButton, 0, 4, 1, 4, Qt::AlignVCenter | Qt::AlignRight);
	
	mVbox->addWidget(mLabel);
	mVbox->addWidget(mTreeWidget);
	mVbox->addLayout(mButtonGridBox);
	mDialog->setMinimumWidth(350);
	mDialog->setLayout(mVbox);
	isGenerate = false;
	isSkipAll = false;
}

void VideoNoteFilter::generate()
{
	for (int i = 0; i < mItems.size(); i++)
	{
		if (mTreeWidget->topLevelItem(i)->checkState(0) != Qt::Checked)
		{
			dragPermutation.push_back(-1);
			continue;
		}
		for (int k = 0; k < mItems.size(); k++)
		{
			if (mTreeWidget->topLevelItem(i)->text(0) == mItems[k]->text(0))
			{
				dragPermutation.push_back(k);
				break;
			}
		}
	}
	mDialog->hide();
	isGenerate = true;
}


void VideoNoteFilter::cancel()
{
	mDialog->hide();
}

void VideoNoteFilter::skipAll()
{
	isSkipAll = true;
	generate();
}
