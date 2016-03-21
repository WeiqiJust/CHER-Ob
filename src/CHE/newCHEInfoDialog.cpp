/****************************************************************************

 - Codename: Hyper3D (Yale Computer Graphics Group)

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
#include "newCHEInfoDIalog.h"

NewCHEInfoDialog::NewCHEInfoDialog()
{
	okButton = new QPushButton("OK");
	connect(okButton, SIGNAL(clicked()), this, SLOT(okPressed()));
	backButton = new QPushButton("Back");
	connect(backButton, SIGNAL(clicked()), this, SLOT(backPressed()));
	cancelButton = new QPushButton("Cancel");
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()));
	mHBox->addWidget(okButton, 0, Qt::AlignRight);
	mHBox->addWidget(backButton, 0, Qt::AlignRight);
	mHBox->addWidget(cancelButton, 0, Qt::AlignRight);

	mVBox->addLayout(mGrid);
	mVBox->addLayout(mHBox);

	mDialog->setLayout(mVBox);

    mDialog->setMinimumWidth(350);
    mDialog->adjustSize();
}

void NewCHEInfoDialog::okPressed()
{
	CHEInfoBasic* info = new CHEInfoBasic();
	info->object = QString(mObjectContent->text());
	info->measurement = QString(mMeasurementContent->text());
	info->creation = QString(mCreationContent->text());
	info->material = QString(mMaterialContent->text());
	info->description = QString(mDescriptionContent->text());
	info->conservation = QString(mConservationContent->text());
	info->analyses = QString(mAnalysesContent->text());
	info->related = QString(mRelatedContent->text());
	info->administration = QString(mAdministrationContent->text());
	info->documents = QString(mDocumentsContent->text());
	info->other = QString(mOtherContent->text());
	emit ok(info);
}

void NewCHEInfoDialog::backPressed()
{
	mObjectContent->clear();
	mMeasurementContent->clear();
	mCreationContent->clear();
	mMaterialContent->clear();
	mDescriptionContent->clear();
	mConservationContent->clear();
	mAnalysesContent->clear();
	mRelatedContent->clear();
	mAdministrationContent->clear();
	mDocumentsContent->clear();
	mOtherContent->clear();

	emit back();
}