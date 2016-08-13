/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Min H. Kim (minhkim@cs.yale.edu)
 			 Weiqi Shi (weiqi.shi@yale.edu)

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
   annonymous help by various software engineering communities.

*****************************************************************************/
#include "annotationFilterDialog.h"

AnnotationFilterDialog::AnnotationFilterDialog(const QVector<QString> users)
{
	mDialog = new QDialog();
	mVbox = new QVBoxLayout();

	mDialog->setWindowTitle(tr("Annotation Filter"));

	mLabel = new QLabel("Please select the users of annotations.\nOnly the annotations with selected users will be shown after filtering.");

	mTreeWidget = new QTreeWidget();
	QStringList ColumnNames;
	ColumnNames << "User Name";
	mTreeWidget->setHeaderLabels(ColumnNames);
	for (int i = 0; i < users.size(); i++)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, QString(users[i]));
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(0, Qt::Unchecked);
		mItems.push_back(item);
	}
	mTreeWidget->insertTopLevelItems(0, mItems);

	mButtonGridBox = new QGridLayout();
	mFilterButton = new QPushButton("Filter");
	connect(mFilterButton, SIGNAL(clicked()), this, SLOT(filter()));
	mCancelButton = new QPushButton("Cancel");
	connect(mCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	mSelectAllButton = new QPushButton("Select All");
	connect(mSelectAllButton, SIGNAL(clicked()), this, SLOT(selectAll()));
	mButtonGridBox->addWidget(mSelectAllButton, 0, 5, 1, 1,  Qt::AlignVCenter | Qt::AlignLeft);
	mButtonGridBox->addWidget(mFilterButton, 0, 5, 1, 4,  Qt::AlignVCenter | Qt::AlignRight);
	mButtonGridBox->addWidget(mCancelButton, 0, 5, 1, 5, Qt::AlignVCenter | Qt::AlignRight);
	
	mVbox->addWidget(mLabel);
	mVbox->addWidget(mTreeWidget);
	mVbox->addLayout(mButtonGridBox);
	mDialog->setMinimumWidth(250);
	mDialog->setLayout(mVbox);
	isFiltered = false;
}

QVector<QString> AnnotationFilterDialog::getSelectedUsers()
{
	QVector<QString> selectedUsers;
	for (int i = 0; i < mSelectedUsers.size(); i++)
	{
		selectedUsers.push_back(mSelectedUsers[i]);
	}
	return selectedUsers;
}

void AnnotationFilterDialog::filter()
{
	for (int i = 0; i < mItems.size(); i++)
	{
		if (mItems[i]->checkState(0) == Qt::Checked)
		{
			mSelectedUsers.push_back(mItems[i]->text(0));
		}
	}
	mDialog->hide();
	isFiltered = true;
}


void AnnotationFilterDialog::cancel()
{
	mDialog->hide();
}

void AnnotationFilterDialog::selectAll()
{
	for (int i = 0; i < mItems.size(); i++)
	{
		mItems[i]->setCheckState(0, Qt::Checked);
	}
}