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
#include "videoSetup.h"

VideoSetup::VideoSetup(QString title, QString author, QString affiliation)
{
	mProjectName = title;
	mUserName = author;
	mAffiliation = affiliation;

	mDialog = new QDialog();
	mVbox = new QVBoxLayout();
	mGrid = new QGridLayout();

	mDialog->setWindowTitle(tr("Video Setup"));

	mProjectNameContent = new QLineEdit();
	mProjectNameContent->setFixedWidth(200);
	mProjectNameContent->setMaxLength(35);
	mProjectNameContent->setText(mProjectName);
	mUserNameContent = new QLineEdit();
	mUserNameContent->setFixedWidth(200);
	mUserNameContent->setMaxLength(35);
	mUserNameContent->setText(mUserName);
	mAffiliationContent = new QLineEdit();
	mAffiliationContent->setFixedWidth(200);
	mAffiliationContent->setMaxLength(35);
	mAffiliationContent->setText(mAffiliation);

	mVideoFormatContent = new QComboBox();
	mVideoFormatContent->setFixedWidth(200);
	mVideoFormatContent->addItem(".avi (DIV3)");
	mVideoFormatContent->addItem(".mp4 (MP42)");
	mVideoFormatContent->addItem(".wmv (WMV2)");
	mVideoFormatContent->setCurrentIndex(0);
	connect(mVideoFormatContent, SIGNAL(currentIndexChanged(int)), this, SLOT(setVideoFormat(int)));

	mResolutionContent = new QComboBox();
	mResolutionContent->setFixedWidth(200);
	mResolutionContent->addItem("800*600");
	mResolutionContent->addItem("1600*900");
	mResolutionContent->addItem("1600*1200");
	mResolutionContent->setCurrentIndex(0);
	connect(mResolutionContent, SIGNAL(currentIndexChanged(int)), this, SLOT(setResolution(int)));

	mFrameDuration2DContent = new QLineEdit();
	mFrameDuration2DContent->setValidator(new QIntValidator(1, 30, this));
	mFrameDuration2DContent->setFixedWidth(200);
	mFrameDuration2DContent->setText("4");
	mTransDuration2DContent = new QLineEdit();
	mTransDuration2DContent->setValidator(new QIntValidator(0, 9, this));
	mTransDuration2DContent->setFixedWidth(200);
	mTransDuration2DContent->setText("1");
	mFrameDuration3DContent = new QLineEdit();
	mFrameDuration3DContent->setValidator(new QIntValidator(1, 30, this));
	mFrameDuration3DContent->setFixedWidth(200);
	mFrameDuration3DContent->setText("4");
	mTransDuration3DContent = new QLineEdit();
	mTransDuration3DContent->setValidator(new QIntValidator(0, 9, this));
	mTransDuration3DContent->setFixedWidth(200);
	mTransDuration3DContent->setText("2");
	mDolly3DContent = new QLineEdit();
	mDolly3DContent->setValidator(new QIntValidator(1, 999, this));
	mDolly3DContent->setFixedWidth(200);
	mDolly3DContent->setText("10");

	mProjectNameLabel = new QLabel("Title: ");
	mProjectNameLabel->setBuddy(mProjectNameContent);
	mUserNameLabel = new QLabel("Author: ");
	mUserNameLabel->setBuddy(mUserNameContent);
	mAffiliationLabel = new QLabel("Affiliation: ");
	mAffiliationLabel->setBuddy(mAffiliationContent);
	mVideoFormatLabel = new QLabel("Video format: ");
	mVideoFormatLabel->setBuddy(mVideoFormatContent);
	mResolutionLabel = new QLabel("Resolution: ");
	mResolutionLabel->setBuddy(mResolutionContent);
	mFrameDuration2DLabel = new QLabel("Duration per 2D frame: ");
	mFrameDuration2DLabel->setBuddy(mFrameDuration2DContent);
	mTransDuration2DLabel = new QLabel("Transtition time 2D: ");
	mTransDuration2DLabel->setBuddy(mTransDuration2DContent);
	mFrameDuration3DLabel = new QLabel("Duration per 3D frame: ");
	mFrameDuration3DLabel->setBuddy(mFrameDuration3DContent);
	mTransDuration3DLabel = new QLabel("Transtition time 3D: ");
	mTransDuration3DLabel->setBuddy(mTransDuration3DContent);
	mDolly3DLabel = new QLabel("Dolly in 3D observation: ");
	mDolly3DLabel->setBuddy(mDolly3DContent);

	mGrid->setColumnStretch(0,0);
    mGrid->setColumnStretch(1,5);
    mGrid->setVerticalSpacing(10);
	// mGrid->setSizeConstraint(QLayout::SetFixedSize);

	int row;
	row = mGrid->rowCount();
	mGrid->addWidget(mProjectNameLabel, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mProjectNameContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mUserNameLabel, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mUserNameContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mAffiliationLabel, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mAffiliationContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mVideoFormatLabel, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mVideoFormatContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mResolutionLabel, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mResolutionContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mFrameDuration2DLabel, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mFrameDuration2DContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mTransDuration2DLabel, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mTransDuration2DContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mFrameDuration3DLabel, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mFrameDuration3DContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mTransDuration3DLabel, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mTransDuration3DContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mDolly3DLabel, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mDolly3DContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	mShowGeneralContent = new QCheckBox("Check this if you want to show general frames for objects");
	mShowGeneralContent->setChecked(true);
	connect(mShowGeneralContent, SIGNAL(stateChanged(int)), this, SLOT(setShowGeneral(int)));

	mShow2RTIsContent = new QCheckBox("Check this if you want to compare two RTIs in the project");
	mShow2RTIsContent->setChecked(false);
	connect(mShow2RTIsContent, SIGNAL(stateChanged(int)), this, SLOT(setShow2RTIs(int)));

	mButtonGridBox = new QGridLayout();
	mNextButton = new QPushButton("Next");
	connect(mNextButton, SIGNAL(clicked()), this, SLOT(next()));
	mCancelButton = new QPushButton("Cancel");
	connect(mCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	mButtonGridBox->addWidget(mNextButton, 0, 4, 1, 3,  Qt::AlignVCenter | Qt::AlignRight);
	mButtonGridBox->addWidget(mCancelButton, 0, 4, 1, 4, Qt::AlignVCenter | Qt::AlignRight);
	
	mVbox->addLayout(mGrid);
	mVbox->addWidget(mShowGeneralContent);
	mVbox->addWidget(mShow2RTIsContent);
	mVbox->addLayout(mButtonGridBox);
	mDialog->setMinimumWidth(250);
	mDialog->setLayout(mVbox);
	isNext = false;
}
void VideoSetup::updateSetup(QString* newProjectName, QString* newUserName, QString* newAffiliation, int* setupVideoFormat, int* setupResolution,
							 int* setupFrameDuration2D, int* setupTransDuration2D, int* setupFrameDuration3D, int* setupTransDuration3D, int* setupDolly3D, bool* setupShowGeneral, bool* setupShow2RTIs)
{
	*newProjectName = mProjectName;
	*newUserName = mUserName;
	*newAffiliation = mAffiliation;
	*setupVideoFormat = mVideoFormat;
	*setupResolution = mResolution;
	*setupFrameDuration2D = mFrameDuration2D;
	*setupTransDuration2D = mTransDuration2D;
	*setupFrameDuration3D = mFrameDuration3D;
	*setupTransDuration3D = mTransDuration3D;
	*setupDolly3D = mDolly3D;
	*setupShowGeneral = isShowGeneral;
	*setupShow2RTIs = isShow2RTIs;
}
void VideoSetup::next()
{
	mProjectName = mProjectNameContent->text();
	mUserName = mUserNameContent->text();
	mAffiliation = mAffiliationContent->text();
	mVideoFormat = mVideoFormatContent->currentIndex();
	mResolution = mResolutionContent->currentIndex();
	mFrameDuration2D = mFrameDuration2DContent->text().toInt();
	mTransDuration2D = mTransDuration2DContent->text().toInt();
	mFrameDuration3D = mFrameDuration3DContent->text().toInt();
	mTransDuration3D = mTransDuration3DContent->text().toInt();
	mDolly3D = mDolly3DContent->text().toInt();
	mDialog->hide();
	isNext = true;
}

void VideoSetup::cancel()
{
	mDialog->hide();
}

void VideoSetup::setVideoFormat(int index)
{
	mVideoFormat = index;
}

void VideoSetup::setResolution(int index)
{
	mResolution = index;
}

void VideoSetup::setShowGeneral(int state)
{
	isShowGeneral = (state > 0);
}

void VideoSetup::setShow2RTIs(int state)
{
	isShow2RTIs = (state > 0);
}