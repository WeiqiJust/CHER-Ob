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

#include "cultureHeritageEntity.h"

NewCHEDialog::NewCHEDialog(const QString path)
{
	mDialog = new QDialog();
	mVbox = new QVBoxLayout();

	mLastPath = path;

	mDialog->setWindowTitle(tr("New Culture Heritage Entity"));

	mRoleVBox = new QVBoxLayout();
	mGroupBox = new QGroupBox(tr("User Mode"));
	mCreatorRadioButton = new QRadioButton(tr("Creator"));
    mModifierRadioButton = new QRadioButton(tr("Modifier"));
    mViewerRadioButton = new QRadioButton(tr("Viewer"));
    mCreatorRadioButton->setChecked(true);
	mUserNameLabel = new QLabel(tr("User Name:"));
	mUserNameLineEdit = new QLineEdit();
	mUserNameLineEdit->setFixedWidth(350);
    mUserNameLabel->setBuddy(mUserNameLineEdit);
	mUserNameHBox = new QHBoxLayout();
	mUserNameHBox->addWidget(mUserNameLabel);
	mUserNameHBox->addWidget(mUserNameLineEdit, 0 , Qt::AlignRight);
    mRoleVBox->addWidget(mCreatorRadioButton);
    mRoleVBox->addWidget(mModifierRadioButton);
    mRoleVBox->addWidget(mViewerRadioButton);
	mRoleVBox->addLayout(mUserNameHBox);
    mRoleVBox->addStretch(1);
    mGroupBox->setLayout(mRoleVBox);

	mObjectLabel = new QLabel(tr("2D/3D Data:"));
	mObjectLineEdit = new QLineEdit();
	mObjectLineEdit->setFixedWidth(270);
    mObjectLabel->setBuddy(mObjectLineEdit);
	mObjectBrowseButton = new QPushButton("Browse");
	connect(mObjectBrowseButton, SIGNAL(clicked()), this, SLOT(objectBrowse()));
	mObjectHBox = new QHBoxLayout();
	mObjectHBox->addWidget(mObjectLabel);
	mObjectHBox->addWidget(mObjectLineEdit, 0 , Qt::AlignRight);
	mObjectHBox->addWidget(mObjectBrowseButton);

	mCTLabel = new QLabel(tr("CT Director: "));
    mCTLineEdit = new QLineEdit();
	mCTLineEdit->setFixedWidth(270);
    mCTLabel->setBuddy(mCTLineEdit);
	mCTBrowseButton = new QPushButton("Browse");
	connect(mCTBrowseButton, SIGNAL(clicked()), this, SLOT(ctBrowse()));
	mCTHBox = new QHBoxLayout();
	mCTHBox->addWidget(mCTLabel);
	mCTHBox->addWidget(mCTLineEdit, 0 , Qt::AlignRight);
	mCTHBox->addWidget(mCTBrowseButton);

	mLoadDataVBox = new QVBoxLayout();
	mLoadDataGroupBox = new QGroupBox(tr("Load Data"));
	mLoadDataLabel = new QLabel("Load 2D/3D data file or CT directory, or do it later");
	mLoadDataVBox->addWidget(mLoadDataLabel);
	mLoadDataVBox->addLayout(mObjectHBox);
	mLoadDataVBox->addLayout(mCTHBox);
	mLoadDataVBox->addStretch(1);
	mLoadDataGroupBox->setLayout(mLoadDataVBox);

    mCHENameLabel = new QLabel(tr("Name:  "));
	mCHENameEdit = new QLineEdit();
	mCHENameEdit->setFixedWidth(350);
    mCHENameLabel->setBuddy(mCHENameEdit);
	connect(mCHENameEdit, SIGNAL(textChanged(QString)), this, SLOT(CHENameChanged(QString)));
	mCHEHBox = new QHBoxLayout();
	mCHEHBox->addWidget(mCHENameLabel, 0 , Qt::AlignLeft);
    mCHEHBox->addWidget(mCHENameEdit, 0 , Qt::AlignRight);
	
    mLocationLabel = new QLabel(tr("Location:"));
	mLocationLineEdit = new QLineEdit();
    mLocationLabel->setBuddy(mLocationLineEdit);
	connect(mLocationLineEdit, SIGNAL(textChanged(QString)), this, SLOT(CHEPathChanged(QString)));
	mLocationLineEdit->setText(QCoreApplication::applicationDirPath());
	mCHEPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
	mLocationLineEdit->setFixedWidth(270);
	mLocationBrowseButton = new QPushButton("Browse");
	connect(mLocationBrowseButton, SIGNAL(clicked()), this, SLOT(locationBrowse()));
	mLocationHBox = new QHBoxLayout();
	mLocationHBox->addWidget(mLocationLabel);
	mLocationHBox->addWidget(mLocationLineEdit, 0 , Qt::AlignRight);
    mLocationHBox->addWidget(mLocationBrowseButton);

	mCHEInfoVBox = new QVBoxLayout();
	mCHEInfoGroupBox = new QGroupBox(tr("CHE Info"));
	mCHEInfoVBox->addLayout(mCHEHBox);
	mCHEInfoVBox->addLayout(mLocationHBox);
	mCHEInfoVBox->addStretch(1);
	mCHEInfoGroupBox->setLayout(mCHEInfoVBox);

	mButtonGridBox = new QGridLayout();
	mNextButton = new QPushButton("Next");
	mNextButton->setEnabled(false);
	connect(mNextButton, SIGNAL(clicked()), this, SLOT(next()));
	mCancelButton = new QPushButton("Cancel");
	connect(mCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	mButtonGridBox->addWidget(mNextButton, 0, 5, 1, 4,  Qt::AlignVCenter | Qt::AlignRight);
	mButtonGridBox->addWidget(mCancelButton, 0, 5, 1, 5, Qt::AlignVCenter | Qt::AlignRight);
	
	mVbox->addWidget(mGroupBox);
	mVbox->addWidget(mLoadDataGroupBox);
	mVbox->addWidget(mCHEInfoGroupBox);
	mVbox->addLayout(mButtonGridBox);
	
	connect(this, SIGNAL(infoUpdate()), this, SLOT(enableNextButton()));
	mDialog->setMinimumWidth(350);
	mDialog->setLayout(mVbox);

	mCHEInfo = new CHENewInfoDialog();
	connect(mCHEInfo, SIGNAL(ok(const CHEInfoBasic*)), this, SLOT(CHEInfoOK(const CHEInfoBasic*)));
	connect(mCHEInfo, SIGNAL(back()), this, SLOT(CHEInfoBack()));
	connect(mCHEInfo, SIGNAL(cancel()), this, SLOT(CHEInfoCancel()));
}

void NewCHEDialog::locationBrowse()
{
	
	mCHEPath = QFileDialog::getExistingDirectory(this, tr("Location"), mLastPath,
                                                       QFileDialog::ShowDirsOnly
                                                       | QFileDialog::DontResolveSymlinks);
	mLocationLineEdit->setText(mCHEPath);
	mLocationLineEdit->displayText();
}

void NewCHEDialog::objectBrowse()
{
	QStringList filters;
	filters.push_back("Images (*.ply *.obj *.wrl *.png *.jpg *.tif *.bmp *.exr *.dcm *rti *ptm *hsh)");
	filters.push_back("*.ply");
	filters.push_back("*.obj");
	filters.push_back("*.wrl");
	filters.push_back("*.exr");
	filters.push_back("*.png");
	filters.push_back("*.jpg");
	filters.push_back("*.tif");
	filters.push_back("*.bmp");
	filters.push_back("*.dcm");
	filters.push_back("*.rti");
	filters.push_back("*.ptm");
	filters.push_back("*.hsh");

	QStringList fileNames;
	fileNames = QFileDialog::getOpenFileNames(this,tr("Load an Image or Object"), mLastPath, filters.join(";;")); // entering the last path
	QString fileName;
	for (int i = 0; i < fileNames.size(); i++)
	{
		fileName = fileName + fileNames[i] + QString(";");
	}
	mObjectLineEdit->setText(fileName);
	mObjectLineEdit->displayText();
}

void NewCHEDialog::ctBrowse()
{
	QString ctDir;
	ctDir = QFileDialog::getExistingDirectory(this, tr("Load a CT Directory"), mLastPath,
                                                       QFileDialog::ShowDirsOnly
                                                       | QFileDialog::DontResolveSymlinks);
	mCTLineEdit->setText(ctDir);
	mCTLineEdit->displayText();
}

void NewCHEDialog::next()
{
	mFullPath = mCHEPath.simplified();
	if (!QDir().exists(mCHEPath))
	{
		qDebug()<<"the path do not exist";
		QString notice;
		notice.append(QString("Error!\nThe directory ") + mCHEPath + QString(" does not exist!"));
		//mError = new ErrorDialog(notice);
		QMessageBox::critical(this, tr("Object Opening Error"), "Unable to open " + mCHEPath + ".");
		return;
	}
	if (mFullPath.split(QDir::separator())[1] == QString())
	{
		mFullPath.append(mCHEName);
	}
	else
	{
		mFullPath.append(QDir::separator() + mCHEName);
	}
	if (QDir().exists(mFullPath))
	{
		QString message("The CHE with specified name already exists in the specified location!\n");
		message.append(QString("Do you want to overwrite the existing CHE with this new CHE?"));

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

void NewCHEDialog::nextReady()
{
	mDialog->hide();
	mCHEInfo->exec();
}

void NewCHEDialog::CHEInfoOK(const CHEInfoBasic* info)
{
	mCHEInfo->hide();
	USERMODE userMode;
	if (mCreatorRadioButton->isChecked()) userMode = CREATOR;
	else if (mModifierRadioButton->isChecked()) userMode = MODIFIER;
	else userMode = VIEWER;
	emit createCHE(mFullPath, mCHEName, userMode, info, mUserNameLineEdit->text());
	
}

void NewCHEDialog::CHEInfoBack()
{
	mCHEInfo->hide();
	mDialog->show();
}

void NewCHEDialog::CHEInfoCancel()
{
	mCHEInfo->hide();
}

void NewCHEDialog::nextCancel()
{
	mCHENameEdit->clear();
}


void NewCHEDialog::cancel()
{
	mDialog->hide();
}

void NewCHEDialog::CHENameChanged(QString name)
{
	mCHEName = name;
	emit infoUpdate();
}

void NewCHEDialog::CHEPathChanged(QString path)
{
	mCHEPath = path;
	emit infoUpdate();
}

void NewCHEDialog::enableNextButton()
{
	if (!mCHEPath.isEmpty() && !mCHEName.isEmpty())
	{
		mNextButton->setEnabled(true);
	}
	else
	{
		mNextButton->setEnabled(false);
	}
}

QString NewCHEDialog::getCHEName()
{
	return mCHENameEdit->text();
}

QString NewCHEDialog::getCHEPath()
{
	return mLocationLineEdit->text();
}