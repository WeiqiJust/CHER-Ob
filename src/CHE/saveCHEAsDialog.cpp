/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Weiqi Shi (weiqi.shi@yale.edu)
			 Zeyu Wang (zeyu.wang@yale.edu)

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
#include "saveCHEAsDialog.h"

SaveCHEAsDialog::SaveCHEAsDialog(const QString user, const CHEInfoBasic* info, const QString path)
{
	mLastPath = path;

	mUserNameLabel = new QLabel(tr("User Name:"));
	mUserNameLineEdit = new QLineEdit();
	mUserNameLineEdit->setFixedWidth(350);
	mUserNameLineEdit->setText(user);
    mUserNameLabel->setBuddy(mUserNameLineEdit);
	mUserNameHBox = new QHBoxLayout();
	mUserNameHBox->addWidget(mUserNameLabel);
	mUserNameHBox->addWidget(mUserNameLineEdit, 0 , Qt::AlignRight);

    mProjectNameLabel = new QLabel(tr("Name:  "));
	mProjectNameEdit = new QLineEdit();
	mProjectNameEdit->setFixedWidth(350);
    mProjectNameLabel->setBuddy(mProjectNameEdit);
	connect(mProjectNameEdit, SIGNAL(textChanged(QString)), this, SLOT(projectNameChanged(QString)));
	mProjectHBox = new QHBoxLayout();
	mProjectHBox->addWidget(mProjectNameLabel, 0 , Qt::AlignLeft);
    mProjectHBox->addWidget(mProjectNameEdit, 0 , Qt::AlignRight);
	
    mLocationLabel = new QLabel(tr("Location:"));
	mLocationLineEdit = new QLineEdit();
    mLocationLabel->setBuddy(mLocationLineEdit);
	connect(mLocationLineEdit, SIGNAL(textChanged(QString)), this, SLOT(projectPathChanged(QString)));
	mLocationLineEdit->setText(QCoreApplication::applicationDirPath());
	mProjectPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
	mLocationLineEdit->setFixedWidth(270);
	mLocationBrowseButton = new QPushButton("Browse");
	connect(mLocationBrowseButton, SIGNAL(clicked()), this, SLOT(locationBrowse()));
	mLocationHBox = new QHBoxLayout();
	mLocationHBox->addWidget(mLocationLabel);
	mLocationHBox->addWidget(mLocationLineEdit, 0 , Qt::AlignRight);
    mLocationHBox->addWidget(mLocationBrowseButton);

	mObjectContent->setText(info->object);
	mMeasurementContent->setText(info->measurement);
	mCreationContent->setText(info->creation);
	mMaterialContent->setText(info->material);
	mDescriptionContent->setText(info->description);
	mConservationContent->setText(info->conservation);
	mAnalysesContent->setText(info->analyses);
	mRelatedContent->setText(info->related);
	mAdministrationContent->setText(info->administration);
	mDocumentsContent->setText(info->documents);
	mOtherContent->setText(info->other);

	mButtonGridBox = new QGridLayout();
	mNextButton = new QPushButton("Next");
	mNextButton->setEnabled(false);
	connect(mNextButton, SIGNAL(clicked()), this, SLOT(next()));
	mCancelButton = new QPushButton("Cancel");
	connect(mCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	mButtonGridBox->addWidget(mNextButton, 0, 5, 1, 4,  Qt::AlignVCenter | Qt::AlignRight);
	mButtonGridBox->addWidget(mCancelButton, 0, 5, 1, 5, Qt::AlignVCenter | Qt::AlignRight);

	mDialog->setWindowTitle(tr("Save Cultural Heritage As.."));

	mVBox->addLayout(mProjectHBox);
	mVBox->addLayout(mLocationHBox);
	mVBox->addLayout(mUserNameHBox);
	mVBox->addLayout(mGrid);
	mVBox->addLayout(mButtonGridBox);
	
	connect(this, SIGNAL(infoUpdate()), this, SLOT(enableNextButton()));

    mDialog->setMinimumWidth(350);
    //mDialog->adjustSize();
	mDialog->setLayout(mVBox);
	isOk = false; // Modified by Zeyu Wang on Nov 13 to fix saveCHEAs bug
}

void SaveCHEAsDialog::locationBrowse()
{
	
	mProjectPath = QFileDialog::getExistingDirectory(this, tr("Location"), mLastPath,
                                                       QFileDialog::ShowDirsOnly
                                                       | QFileDialog::DontResolveSymlinks);
	mLocationLineEdit->setText(mProjectPath);
	mLocationLineEdit->displayText();
}


void SaveCHEAsDialog::next()
{
	mFullPath = mProjectPath.simplified();
	if (!QDir().exists(mProjectPath))
	{
		qDebug()<<"the path do not exist";
		QString notice;
		notice.append(QString("Error!\nThe directory ") + mProjectPath + QString(" does not exist!"));
		//mError = new ErrorDialog(notice);
		QMessageBox::critical(this, tr("Object Opening Error"), "Unable to open " + mProjectPath + ".");
		return;
	}
	if (mFullPath.split(QDir::separator())[1] == QString())
	{
		mFullPath.append(mProjectName);
	}
	else
	{
		mFullPath.append(QDir::separator() + mProjectName);
	}
	if (QDir().exists(mFullPath))
	{
		QString message("The project with specified name already exists in the specified location!\n");
		message.append(QString("Do you want to overwrite the existing project with this new project?"));

		QMessageBox::StandardButton reply;
		reply = QMessageBox::warning(this, tr("WARNING"), message, QMessageBox::Ok|QMessageBox::No);
		if (reply == QMessageBox::Ok) 
		{
			nextReady();
		}
		else 
		{
			nextCancel();
		}
	}
	else
	{
		nextReady();
	}
}

void SaveCHEAsDialog::nextReady()
{
	isOk = true;
	mDialog->hide();
}

void SaveCHEAsDialog::nextCancel()
{
	mProjectNameEdit->clear();
}


void SaveCHEAsDialog::cancel()
{
	mDialog->hide();
}

void SaveCHEAsDialog::projectNameChanged(QString name)
{
	mProjectName = name;
	emit infoUpdate();
}

void SaveCHEAsDialog::projectPathChanged(QString path)
{
	mProjectPath = path;
	emit infoUpdate();
}

void SaveCHEAsDialog::enableNextButton()
{
	if (!mProjectPath.isEmpty() && !mProjectName.isEmpty())
	{
		mNextButton->setEnabled(true);
	}
	else
	{
		mNextButton->setEnabled(false);
	}
}

CHEInfoBasic* SaveCHEAsDialog::getCHEInfo()
{
	CHEInfoBasic* info = new CHEInfoBasic();
	info->object = mObjectContent->text();
	info->measurement = mMeasurementContent->text();
	info->creation = mCreationContent->text();
	info->material = mMaterialContent->text();
	info->description = mDescriptionContent->text();
	info->conservation = mConservationContent->text();
	info->analyses = mAnalysesContent->text();
	info->related = mRelatedContent->text();
	info->administration = mAdministrationContent->text();
	info->documents = mDocumentsContent->text();
	info->other = mOtherContent->text();
	return info;
}