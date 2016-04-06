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

#include "CHEInfoDialog.h"

CHEInfoBasic::CHEInfoBasic(QWidget *parent)
	: QWidget(parent)
{
	object = QString();
	measurement = QString();
	creation = QString();
	material = QString();
	description = QString();
	conservation = QString();
	analyses = QString();
	related = QString();
	administration = QString();
	documents = QString();
	other = QString();
}

CHEInfoDialog::CHEInfoDialog(QWidget *parent)
	: CHEInfoBasic(parent)
{
	mDialog = new QDialog();
	mGrid = new QGridLayout();
	mVBox = new QVBoxLayout();
	mHBox = new QHBoxLayout();
	mObjectContent = new QLineEdit();
	mObjectContent->setFixedWidth(200);
	mMeasurementContent = new QLineEdit();
	mMeasurementContent->setFixedWidth(200);
	mCreationContent = new QLineEdit();
	mCreationContent->setFixedWidth(200);
	mMaterialContent = new QLineEdit();
	mMaterialContent->setFixedWidth(200);
	mDescriptionContent = new QLineEdit();
	mDescriptionContent->setFixedWidth(200);
	mConservationContent = new QLineEdit();
	mConservationContent->setFixedWidth(200);
	mAnalysesContent = new QLineEdit();
	mAnalysesContent->setFixedWidth(200);
	mRelatedContent = new QLineEdit();
	mRelatedContent->setFixedWidth(200);
	mAdministrationContent = new QLineEdit();
	mAdministrationContent->setFixedWidth(200);
	mDocumentsContent = new QLineEdit();
	mDocumentsContent->setFixedWidth(200);
	mOtherContent = new QLineEdit();
	mOtherContent->setFixedWidth(200);

	mObject = new QLabel("Object / Work:");
	mObject->setBuddy(mObjectContent);
	mMeasurement = new QLabel("Physical Dimensions / Measurement:");
	mMeasurement->setBuddy(mMeasurementContent);
	mCreation = new QLabel("Creation:");
	mCreation->setBuddy(mCreationContent);
	mMaterial = new QLabel("Materials and Techniques:");
	mMaterial->setBuddy(mMaterialContent);
	mDescription = new QLabel("Stylisyic Analysis and Descriptions:");
	mDescription->setBuddy(mDescriptionContent);
	mConservation = new QLabel("Condition and Conservation:");
	mConservation->setBuddy(mConservationContent);
	mAnalyses = new QLabel("Analyses:");
	mAnalyses->setBuddy(mAnalysesContent);
	mRelated = new QLabel("Related Works:");
	mRelated->setBuddy(mRelatedContent);
	mAdministration = new QLabel("Exhibition / Loans and Legal Issues:");
	mAdministration->setBuddy(mAdministrationContent);
	mDocuments = new QLabel("Image/Audio Documentation:");
	mDocuments->setBuddy(mDocumentsContent);
	mOther = new QLabel("Others:");
	mOther->setBuddy(mOtherContent);

	mGrid->setColumnStretch(0,0);
    mGrid->setColumnStretch(1,5);
    mGrid->setVerticalSpacing(10);
   // mGrid->setSizeConstraint(QLayout::SetFixedSize);

	int row;
	row = mGrid->rowCount();
	mGrid->addWidget(mObject, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mObjectContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mMeasurement, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mMeasurementContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mCreation, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mCreationContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mMaterial, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mMaterialContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mDescription, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mDescriptionContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mConservation, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mConservationContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mAnalyses, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mAnalysesContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mRelated, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mRelatedContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mAdministration, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mAdministrationContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mDocuments, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mDocumentsContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mOther, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mOtherContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);
}



