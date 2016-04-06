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
#include "removeObjectDialog.h"

#define SIZE (1024)

RemoveObjectDialog::RemoveObjectDialog()
{
	mDialog = new QDialog();
	mVBox = new QVBoxLayout();
	mGrid = new QGridLayout();
	mDialog->setWindowTitle(tr("Remove Object"));
	mInstruction = new QLabel(tr("Please select the obejct to be removed"));

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
	isExport = false;
	mPath = QString("");
}

void RemoveObjectDialog::addItem(QString name)
{
	QStringList list;
	list.append(name);
	QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, list);
	mItems.append(item);
	mTreeWidget->addTopLevelItem(item);
}

void RemoveObjectDialog::ok()
{
	int ret = QMessageBox::warning(this, tr("Warning"),
                               tr("Do you want to save the selected object into other place before removing it? "),
                               QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                               QMessageBox::Yes);
	if(ret == QMessageBox::Cancel) return;
	else
	{
		QList<QTreeWidgetItem*> selected = mTreeWidget->selectedItems();
		foreach(QTreeWidgetItem* item, selected)
		{
			mSelection.append(item->text(0));
		}
		if (ret == QMessageBox::Yes)
		{
			mPath = QFileDialog::getExistingDirectory(this, tr("Export Location"), QString(),
													   QFileDialog::ShowDirsOnly
													   | QFileDialog::DontResolveSymlinks);
			isExport = true;
		}
	}
	mDialog->close();
}

void RemoveObjectDialog::cancel()
{
	mDialog->close();
}
