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
#include "mergeBackToCHELocationDialog.h"

MergeBackToCHELocationDialog::MergeBackToCHELocationDialog(QVector<QString> location)
{
	mDialog = new QDialog();
	mVbox = new QVBoxLayout();

	mDialog->setWindowTitle(tr("Specify Location"));

	mLabel = new QLabel("Please specify location for the new object to merge");

	mTreeWidget = new QTreeWidget();
	QStringList ColumnNames;
	ColumnNames << "Location";
	mTreeWidget->setHeaderLabels(ColumnNames);
	for (int i = 0; i < location.size(); i++)
	{
		if (location[i] == QString())
			continue;
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, QString(location[i]));
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(0, Qt::Unchecked);
		mItems.push_back(item);
	}
	mTreeWidget->insertTopLevelItems(0, mItems);
	connect(mTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem* , int)), this, SLOT(itemChanged(QTreeWidgetItem* , int)));

	mButtonGridBox = new QGridLayout();
	mOkButton = new QPushButton("Ok");
	connect(mOkButton, SIGNAL(clicked()), this, SLOT(ok()));
	mCancelButton = new QPushButton("Cancel");
	connect(mCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	mButtonGridBox->addWidget(mOkButton, 0, 5, 1, 4,  Qt::AlignVCenter | Qt::AlignRight);
	mButtonGridBox->addWidget(mCancelButton, 0, 5, 1, 5, Qt::AlignVCenter | Qt::AlignRight);
	
	mVbox->addWidget(mLabel);
	mVbox->addWidget(mTreeWidget);
	mVbox->addLayout(mButtonGridBox);
	mDialog->setMinimumWidth(250);
	mDialog->setLayout(mVbox);
	isOk = false;
}


void MergeBackToCHELocationDialog::ok()
{
	for (int i = 0; i < mItems.size(); i++)
	{
		if (mItems[i]->checkState(0) == Qt::Checked)
		{
			mSelected = mItems[i]->text(0);
			mDialog->hide();
			isOk = true;
			break;
		}
	}
}


void MergeBackToCHELocationDialog::cancel()
{
	mDialog->hide();
}


void MergeBackToCHELocationDialog::itemChanged(QTreeWidgetItem * item, int column)
{
	if (mItems.indexOf(item) != -1)
	{
		for (int i = 0; i < mItems.size(); i++)
		{
			if (item->text(0) != mItems[i]->text(0) && mItems[i]->checkState(0) == Qt::Checked)
			{
				mItems[i]->setCheckState(0, Qt::Unchecked);
			}
		}
		
	}
}


