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
	//exportButton->setEnabled(false);
	mTextEdit = new QTextEdit(info->object);

	mLabel = new QLabel("Please select the category");

	mFilter = new QComboBox();
	mFilter->addItem("Object / Work"); 
	mFilter->addItem("Physical Dimensions / Measurement");
	mFilter->addItem("Creation"); 
	mFilter->addItem("Materials and Techniques"); 
	mFilter->addItem("Stylisyic Analysis and Descriptions"); 
	mFilter->addItem("Condition and Conservation"); 
	mFilter->addItem("Analyses"); 
	mFilter->addItem("Related Works"); 
	mFilter->addItem("Exhibition / Loans and Legal Issues"); 
	mFilter->addItem("Image/Audio Documentation"); 
	mFilter->addItem("Others");
	mFilter->setCurrentIndex(0);
	currentMode = 0;
	connect(mFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(setFilterMode(int)));

	mVBox->addWidget(mLabel);
	mVBox->addWidget(mFilter);
	mVBox->addWidget(mTextEdit);

	mTemptInfo.push_back(info->object);
	mTemptInfo.push_back(info->measurement);
	mTemptInfo.push_back(info->creation);
	mTemptInfo.push_back(info->material);
	mTemptInfo.push_back(info->description);
	mTemptInfo.push_back(info->conservation);
	mTemptInfo.push_back(info->analyses);
	mTemptInfo.push_back(info->related);
	mTemptInfo.push_back(info->administration);
	mTemptInfo.push_back(info->documents);
	mTemptInfo.push_back(info->other);

	mHBox->addWidget(exportButton);
	mHBox->addWidget(saveButton);
	mHBox->addWidget(editButton);
	mVBox->addLayout(mHBox);

	savePressed();

    mDialog->setMinimumWidth(350);
    //mDialog->adjustSize();
	this->setLayout(mVBox);
	this->show();
}

CHEInfoBasic* CHETab::getCHEInfo()
{
	CHEInfoBasic* info = new CHEInfoBasic();

	info->object = mTemptInfo[0];
	info->measurement = mTemptInfo[1];
	info->creation = mTemptInfo[2];
	info->material = mTemptInfo[3];
	info->description = mTemptInfo[4];
	info->conservation = mTemptInfo[5];
	info->analyses = mTemptInfo[6];
	info->related = mTemptInfo[7];
	info->administration = mTemptInfo[8];
	info->documents = mTemptInfo[9];
	info->other = mTemptInfo[10];
	return info;
}

void CHETab::updateCHEInfo(const CHEInfoBasic* info)
{
	mTemptInfo[0] = info->object;
	mTemptInfo[1] = info->measurement;
	mTemptInfo[2] = info->creation;
	mTemptInfo[3] = info->material;
	mTemptInfo[4] = info->description;
	mTemptInfo[5] = info->conservation;
	mTemptInfo[6] = info->analyses;
	mTemptInfo[7] = info->related;
	mTemptInfo[8] = info->administration;
	mTemptInfo[9] = info->documents;
	mTemptInfo[10] = info->other;
	mTextEdit->setText(mTemptInfo[currentMode]);
	savePressed();
}

void CHETab::editPressed()
{
	editButton->setEnabled(false);
	saveButton->setEnabled(true);

	mTextEdit->setEnabled(true);
}

void CHETab::savePressed()
{
	editButton->setEnabled(true);
	saveButton->setEnabled(false);

	mTemptInfo[currentMode] = mTextEdit->toPlainText();

	mTextEdit->setEnabled(false);
	object = mTemptInfo[0];
	measurement = mTemptInfo[1];
	creation = mTemptInfo[2];
	material = mTemptInfo[3];
	description = mTemptInfo[4];
	conservation = mTemptInfo[5];
	analyses = mTemptInfo[6];
	related = mTemptInfo[7];
	administration = mTemptInfo[8];
	documents = mTemptInfo[9];
	other = mTemptInfo[10];

	emit save();
}


void CHETab::setFilterMode(int mode)
{
	mTemptInfo[currentMode] = mTextEdit->toPlainText();
	mTextEdit->setText(mTemptInfo[mode]);
	currentMode = mode;
}

