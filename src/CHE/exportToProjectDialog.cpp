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
#include "exportToProjectDialog.h"


std::string num2category(int mode)
{
	switch (mode)
	{
		case 0: return "Object / Work";
		case 1: return "Physical Dimensions / Measurement";
		case 2: return "Creation";
		case 3: return "Materials and Techniques";
		case 4: return "Stylisyic Analysis and Descriptions";
		case 5: return "Condition and Conservation";
		case 6: return "Analyses";
		case 7: return "Related Works";
		case 8: return "Exhibition / Loans and Legal Issues";
		case 9: return "Image/Audio Documentation";
		case 10: return "Others";
	}
}

ExportToProjectDialog::ExportToProjectDialog(const QString user, const QString path)
{
	mDialog = new QDialog();
	mVbox = new QVBoxLayout();

	mLastPath = path;

	mDialog->setWindowTitle(tr("Export Cultural Heritage Entity to Project"));

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

	mCategoryLabel = new QLabel("Please select the categories to be exported");
	mTreeWidget = new QTreeWidget();
	QStringList ColumnNames;
	ColumnNames << "Category";
	mTreeWidget->setHeaderLabels(ColumnNames);
	for (int i = 0; i < 11; i++)
	{
		QTreeWidgetItem *itm = new QTreeWidgetItem();
		itm->setText(0, QString(num2category(i).c_str()));
		itm->setFlags(itm->flags() | Qt::ItemIsUserCheckable);
		itm->setCheckState(0, Qt::Unchecked);
		mItems.push_back(itm);
	}
	mTreeWidget->insertTopLevelItems(0, mItems);

	mButtonGridBox = new QGridLayout();
	mNextButton = new QPushButton("Export");
	mNextButton->setEnabled(false);
	connect(mNextButton, SIGNAL(clicked()), this, SLOT(next()));
	mCancelButton = new QPushButton("Cancel");
	connect(mCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	mSelectAllButton = new QPushButton("Select All");
	connect(mSelectAllButton, SIGNAL(clicked()), this, SLOT(selectAll()));
	mButtonGridBox->addWidget(mSelectAllButton, 0, 5, 1, 1,  Qt::AlignVCenter | Qt::AlignLeft);
	mButtonGridBox->addWidget(mNextButton, 0, 5, 1, 4,  Qt::AlignVCenter | Qt::AlignRight);
	mButtonGridBox->addWidget(mCancelButton, 0, 5, 1, 5, Qt::AlignVCenter | Qt::AlignRight);
	

	mVbox->addLayout(mProjectHBox);
	mVbox->addLayout(mLocationHBox);
	mVbox->addLayout(mUserNameHBox);
	mVbox->addLayout(mKeywordHBox);
	mVbox->addLayout(mAffiliationHBox);
	mVbox->addWidget(mDescriptionLabel);
	mVbox->addWidget(mDescriptionEdit);
	mVbox->addWidget(mCategoryLabel);
	mVbox->addWidget(mTreeWidget);
	mVbox->addLayout(mButtonGridBox);
	
	connect(this, SIGNAL(infoUpdate()), this, SLOT(enableNextButton()));
	mDialog->setMinimumWidth(350);
	mDialog->setLayout(mVbox);
	isOk = false;
}

QList<int> ExportToProjectDialog::getCategories()
{
	QList<int> result;
	for (int i = 0; i < mItems.size(); i++)
	{
		if (mItems[i]->checkState(0) == Qt::Checked)
			result.push_back(i);
	}
	return result;
}

void ExportToProjectDialog::locationBrowse()
{
	
	mProjectPath = QFileDialog::getExistingDirectory(this, tr("Location"), mLastPath,
                                                       QFileDialog::ShowDirsOnly
                                                       | QFileDialog::DontResolveSymlinks);
	mLocationLineEdit->setText(mProjectPath);
	mLocationLineEdit->displayText();
}


void ExportToProjectDialog::next()
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

void ExportToProjectDialog::nextReady()
{
	isOk = true;
	mDialog->hide();
	
}

void ExportToProjectDialog::nextCancel()
{
	mProjectNameEdit->clear();
}


void ExportToProjectDialog::cancel()
{
	mDialog->hide();
}

void ExportToProjectDialog::selectAll()
{
	for (int i = 0; i < 11; i++)
	{
		mItems[i]->setCheckState(0, Qt::Checked);
	}
}

void ExportToProjectDialog::enableNextButton()
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

void ExportToProjectDialog::projectNameChanged(QString name)
{
	mProjectName = name;
	emit infoUpdate();
}

void ExportToProjectDialog::projectPathChanged(QString path)
{
	mProjectPath = path;
	emit infoUpdate();
}


