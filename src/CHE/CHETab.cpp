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
#include "CHETab.h"

CHETab::CHETab(const CHEInfoBasic* info, QWidget *parent)
	: CHEInfoDialog(parent)
{
	exportButton = new QPushButton("Export to Project");
	connect(exportButton, SIGNAL(clicked()), this, SLOT(exportPressed()));
	saveButton = new QPushButton("Save");
	connect(saveButton, SIGNAL(clicked()), this, SLOT(savePressed()));
	editButton = new QPushButton("Edit");
	connect(editButton, SIGNAL(clicked()), this, SLOT(editPressed()));
	saveButton->setEnabled(false);
	exportButton->setEnabled(false);

	mObjectInfo = new QTextEdit(info->object);
	mMeasurementInfo = new QTextEdit(info->measurement);
	mCreationInfo = new QTextEdit(info->creation);
	mMaterialInfo = new QTextEdit(info->material);
	mDescriptionInfo = new QTextEdit(info->description);
	mConservationInfo = new QTextEdit(info->conservation);
	mAnalysesInfo = new QTextEdit(info->analyses);
	mRelatedInfo = new QTextEdit(info->related);
	mAdministrationInfo = new QTextEdit(info->administration);
	mDocumentsInfo = new QTextEdit(info->documents);
	mOtherInfo = new QTextEdit(info->other);

	mVBox->insertWidget(0, mObject);
	mVBox->insertWidget(1, mObjectInfo);
	mVBox->insertWidget(2, mMeasurement);
	mVBox->insertWidget(3, mMeasurementInfo);
	mVBox->insertWidget(4, mCreation);
	mVBox->insertWidget(5, mCreationInfo);
	mVBox->insertWidget(6, mMaterial);
	mVBox->insertWidget(7, mMaterialInfo);
	mVBox->insertWidget(8, mDescription);
	mVBox->insertWidget(9, mDescriptionInfo);
	mVBox->insertWidget(10, mConservation);
	mVBox->insertWidget(11, mConservationInfo);
	mVBox->insertWidget(12, mAnalyses);
	mVBox->insertWidget(13, mAnalysesInfo);
	mVBox->insertWidget(14, mRelated);
	mVBox->insertWidget(15, mRelatedInfo);
	mVBox->insertWidget(16, mAdministration);
	mVBox->insertWidget(17, mAdministrationInfo);
	mVBox->insertWidget(18, mDocuments);
	mVBox->insertWidget(19, mDocumentsInfo);
	mVBox->insertWidget(20, mOther);
	mVBox->insertWidget(21, mOtherInfo);
	mHBox->addWidget(exportButton, 0, Qt::AlignRight);
	mHBox->addWidget(saveButton, 0, Qt::AlignRight);
	mHBox->addWidget(editButton, 0, Qt::AlignRight);
	mVBox->addLayout(mHBox);

	mObjectInfo->setEnabled(false);
	mMeasurementInfo->setEnabled(false);
	mCreationInfo->setEnabled(false);
	mMaterialInfo->setEnabled(false);
	mDescriptionInfo->setEnabled(false);
	mConservationInfo->setEnabled(false);
	mAnalysesInfo->setEnabled(false);
	mRelatedInfo->setEnabled(false);
	mAdministrationInfo->setEnabled(false);
	mDocumentsInfo->setEnabled(false);
	mOtherInfo->setEnabled(false);

    mDialog->setMinimumWidth(350);
    //mDialog->adjustSize();
	this->setLayout(mVBox);
	this->show();
}

void CHETab::editPressed()
{
	editButton->setEnabled(false);
	saveButton->setEnabled(true);

	mObjectInfo->setEnabled(true);
	mMeasurementInfo->setEnabled(true);
	mCreationInfo->setEnabled(true);
	mMaterialInfo->setEnabled(true);
	mDescriptionInfo->setEnabled(true);
	mConservationInfo->setEnabled(true);
	mAnalysesInfo->setEnabled(true);
	mRelatedInfo->setEnabled(true);
	mAdministrationInfo->setEnabled(true);
	mDocumentsInfo->setEnabled(true);
	mOtherInfo->setEnabled(true);
}

void CHETab::savePressed()
{
	editButton->setEnabled(true);
	saveButton->setEnabled(false);

	mObjectInfo->setEnabled(false);
	mMeasurementInfo->setEnabled(false);
	mCreationInfo->setEnabled(false);
	mMaterialInfo->setEnabled(false);
	mDescriptionInfo->setEnabled(false);
	mConservationInfo->setEnabled(false);
	mAnalysesInfo->setEnabled(false);
	mRelatedInfo->setEnabled(false);
	mAdministrationInfo->setEnabled(false);
	mDocumentsInfo->setEnabled(false);
	mOtherInfo->setEnabled(false);

	object = mObjectInfo->toPlainText();
	measurement = mMeasurementInfo->toPlainText();
	creation = mCreationInfo->toPlainText();
	material = mMaterialInfo->toPlainText();
	description = mDescriptionInfo->toPlainText();
	conservation = mConservationInfo->toPlainText();
	analyses = mAnalysesInfo->toPlainText();
	related = mRelatedInfo->toPlainText();
	administration = mAdministrationInfo->toPlainText();
	documents = mDocumentsInfo->toPlainText();
	other = mOtherInfo->toPlainText();
}

void CHETab::exportPressed()
{
	//to be done
}