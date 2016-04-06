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
#include "openWindowDialog.h"

#define SIZE (1024)

OpenWindowDialog::OpenWindowDialog()
{
	mDialog = new QDialog();
	mVBox = new QVBoxLayout();
	mGrid = new QGridLayout();
	mDialog->setWindowTitle(tr("Open Window"));
	mInstruction = new QLabel(tr("Please select the closed window to open"));

	mOkButton = new QPushButton("OK");
	mCancelButton = new QPushButton("Cancel");
	mOkButton->adjustSize();
	mCancelButton->adjustSize();
	
	connect(mOkButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(mCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

	mTreeWidget = new QTreeWidget();
	mTreeWidget->setColumnCount(1);
	mTreeWidget->setColumnWidth(0, 250);

	QStringList ColumnNames;
	ColumnNames << "Object" ;
	mTreeWidget->setHeaderLabels(ColumnNames);
	mTreeWidget->setSortingEnabled(true);
	mTreeWidget->setSelectionMode(QAbstractItemView::MultiSelection);

	mGrid->addWidget(mOkButton, 0, 3, 1, 2,  Qt::AlignVCenter | Qt::AlignRight);
	mGrid->addWidget(mCancelButton, 0, 3, 1, 3, Qt::AlignVCenter | Qt::AlignRight);
	
	mVBox->addWidget(mInstruction);
	mVBox->addWidget(mTreeWidget);
	mVBox->addLayout(mGrid);
	mDialog->setLayout(mVBox);
}

void OpenWindowDialog::addItem(QString name)
{
	QStringList list;
	list.append(name);
	QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, list);
	mItems.append(item);
	mTreeWidget->addTopLevelItem(item);
}

void OpenWindowDialog::ok()
{
	QList<QTreeWidgetItem*> selected = mTreeWidget->selectedItems();
	foreach(QTreeWidgetItem* item, selected)
	{
		mSelection.append(item->text(0));
	}
	mDialog->close();
}

void OpenWindowDialog::cancel()
{
	mDialog->close();
}