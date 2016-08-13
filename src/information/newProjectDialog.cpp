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
#include "newProjectDialog.h"
#include "../visualization/vtkWidget.h"

#define SIZE (1024)

NewProjectDialog::NewProjectDialog(const QString path)
{
	mDialog = new QDialog();
	mVbox = new QVBoxLayout();

	mLastPath = path;

	mDialog->setWindowTitle(tr("New Project"));

	mRoleVBox = new QVBoxLayout();
	mGroupBox = new QGroupBox(tr("User Mode (Coming soon)"));
	mCreatorRadioButton = new QRadioButton(tr("Creator"));
    mModifierRadioButton = new QRadioButton(tr("Modifier"));
    mViewerRadioButton = new QRadioButton(tr("Viewer"));
    mCreatorRadioButton->setChecked(true);
	mUserNameLabel = new QLabel(tr("User Name*:"));
	mUserNameLineEdit = new QLineEdit();
	mUserNameLineEdit->setFixedWidth(350);
    mUserNameLabel->setBuddy(mUserNameLineEdit);
	connect(mUserNameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(userNameChanged(QString)));
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

    mProjectNameLabel = new QLabel(tr("Name*:  "));
	mProjectNameEdit = new QLineEdit();
	mProjectNameEdit->setFixedWidth(350);
    mProjectNameLabel->setBuddy(mProjectNameEdit);
	connect(mProjectNameEdit, SIGNAL(textChanged(QString)), this, SLOT(projectNameChanged(QString)));
	mProjectHBox = new QHBoxLayout();
	mProjectHBox->addWidget(mProjectNameLabel, 0 , Qt::AlignLeft);
    mProjectHBox->addWidget(mProjectNameEdit, 0 , Qt::AlignRight);
	
    mLocationLabel = new QLabel(tr("Location*:"));
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

	mKeywordLabel = new QLabel(tr("Keywords:"));
	mKeywordLineEdit = new QLineEdit();
	mKeywordLineEdit->setFixedWidth(350);
    mKeywordLabel->setBuddy(mKeywordLineEdit);
	mKeywordHBox = new QHBoxLayout();
	mKeywordHBox->addWidget(mKeywordLabel);
    mKeywordHBox->addWidget(mKeywordLineEdit, 0 , Qt::AlignRight);

	mAffiliationLabel = new QLabel(tr("Affiliation:"));
	mAffiliationLineEdit = new QLineEdit();
	mAffiliationLineEdit->setFixedWidth(350);
    mAffiliationLabel->setBuddy(mAffiliationLineEdit);
	mAffiliationHBox = new QHBoxLayout();
	mAffiliationHBox->addWidget(mAffiliationLabel);
    mAffiliationHBox->addWidget(mAffiliationLineEdit, 0 , Qt::AlignRight);

	mDescriptionLabel = new QLabel(tr("Description:"));
	mDescriptionEdit = new QTextEdit(mDialog);

	mProjectInfoVBox = new QVBoxLayout();
	mProjectInfoGroupBox = new QGroupBox(tr("Project Info"));
	mProjectInfoVBox->addLayout(mProjectHBox);
	mProjectInfoVBox->addLayout(mLocationHBox);
	mProjectInfoVBox->addLayout(mKeywordHBox);
	mProjectInfoVBox->addLayout(mAffiliationHBox);
	mProjectInfoVBox->addWidget(mDescriptionLabel);
	mProjectInfoVBox->addWidget(mDescriptionEdit);
	mProjectInfoVBox->addStretch(1);
	mProjectInfoGroupBox->setLayout(mProjectInfoVBox);

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
	mVbox->addWidget(mProjectInfoGroupBox);
	mVbox->addLayout(mButtonGridBox);
	
	connect(this, SIGNAL(infoUpdate()), this, SLOT(enableNextButton()));
	mDialog->setMinimumWidth(450);
	mDialog->setLayout(mVbox);
}

void NewProjectDialog::locationBrowse()
{
	
	mProjectPath = QFileDialog::getExistingDirectory(this, tr("Location"), mLastPath,
                                                       QFileDialog::ShowDirsOnly
                                                       | QFileDialog::DontResolveSymlinks);
	mLocationLineEdit->setText(mProjectPath);
	mLocationLineEdit->displayText();
}

void NewProjectDialog::objectBrowse()
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

void NewProjectDialog::ctBrowse()
{
	QString ctDir;
	ctDir = QFileDialog::getExistingDirectory(this, tr("Load a CT Directory"), mLastPath,
                                                       QFileDialog::ShowDirsOnly
                                                       | QFileDialog::DontResolveSymlinks);
	mCTLineEdit->setText(ctDir);
	mCTLineEdit->displayText();
}

void NewProjectDialog::next()
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

void NewProjectDialog::nextReady()
{
	USERMODE userMode;
	if (mCreatorRadioButton->isChecked()) userMode = CREATOR;
	else if (mModifierRadioButton->isChecked()) userMode = MODIFIER;
	else userMode = VIEWER;
	emit nextPressed(mFullPath, mProjectName, userMode, mUserNameLineEdit->text(), mObjectLineEdit->text(), 
		mCTLineEdit->text(), mKeywordLineEdit->text(), mAffiliationLineEdit->text(), mDescriptionEdit->toPlainText());
	mDialog->hide();
	
}

void NewProjectDialog::nextCancel()
{
	mProjectNameEdit->clear();
}


void NewProjectDialog::cancel()
{
	mDialog->hide();
}

void NewProjectDialog::projectNameChanged(QString name)
{
	mProjectName = name;
	emit infoUpdate();
}

void NewProjectDialog::projectPathChanged(QString path)
{
	mProjectPath = path;
	emit infoUpdate();
}

void NewProjectDialog::userNameChanged(QString name)
{
	mUserName = name;
	emit infoUpdate();
}

void NewProjectDialog::enableNextButton()
{
	if (!mProjectPath.isEmpty() && !mProjectName.isEmpty() && !mUserName.isEmpty())
	{
		mNextButton->setEnabled(true);
	}
	else
	{
		mNextButton->setEnabled(false);
	}
}

QString NewProjectDialog::getProjectName()
{
	return mProjectNameEdit->text();
}

QString NewProjectDialog::getProjectPath()
{
	return mLocationLineEdit->text();
}
