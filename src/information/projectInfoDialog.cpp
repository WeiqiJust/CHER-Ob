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
#include "projectInfoDialog.h"

#define SIZE (1024)

ProjectInfoDialog::ProjectInfoDialog(const QString name, const QString path, const QString keyword, const QString affiliation,
									 const QString userName, const QString description)
{
	mDialog = new QDialog();
	mGrid = new QGridLayout();
	mVBox = new QVBoxLayout();
	mHBox = new QHBoxLayout();
	mHBoxDescription = new QHBoxLayout();
	mUserNameLineEdit = new QLineEdit();
	mUserNameLineEdit->setFixedWidth(200);
	mKeywordLineEdit = new QLineEdit();
	mKeywordLineEdit->setFixedWidth(200);
	mAffiliationLineEdit = new QLineEdit();
	mAffiliationLineEdit->setFixedWidth(200);
	mName = new QLabel("Project Name:");
	mNameContent = new QLabel(name);
	mName->setBuddy(mNameContent);
	mPath = new QLabel("Location:");
	mPathContent = new QLabel(path);
	mPath->setBuddy(mPathContent);
	mKeyword = new QLabel("Keyword:");
	mKeywordContent = new QLabel(keyword);
	mKeyword->setBuddy(mKeywordContent);
	mAffiliation = new QLabel("Affiliation:");
	mAffiliationContent = new QLabel(affiliation);
	mAffiliation->setBuddy(mAffiliationContent);
	mUserName = new QLabel("User Name:");
	mUserNameContent = new QLabel(userName);
	mUserName->setBuddy(mUserNameContent);
	mDescription = new QLabel("Description:");
	mDescriptionContent = new QLabel(description);
	mDescriptionContent->setFixedHeight(150);
	mDescriptionContent->setAlignment(Qt::AlignTop);
	mDescriptionText = new QTextEdit();
	mDescriptionText->setText(description);
	cancelButton = new QPushButton("Cancel");
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(hide()));
	editButton = new QPushButton("Edit");
	connect(editButton, SIGNAL(clicked()), this, SLOT(edit()));
	saveButton = new QPushButton("Save");
	saveButton->setEnabled(false);
	connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
	mHBox->addWidget(saveButton, 0, Qt::AlignRight);
	mHBox->addWidget(editButton, 0, Qt::AlignRight);
	mHBox->addWidget(cancelButton, 0, Qt::AlignRight);

	mGrid->setColumnStretch(0,0);
    mGrid->setColumnStretch(1,5);
    mGrid->setVerticalSpacing(10);
   // mGrid->setSizeConstraint(QLayout::SetFixedSize);

	int row;
	row = mGrid->rowCount();
	mGrid->addWidget(mName, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mNameContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mPath, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mPathContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mUserName, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mUserNameContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mKeyword, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mKeywordContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mAffiliation, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mAffiliationContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mDescription, row, 0, 1, 1, Qt::AlignLeft);

	mHBoxDescription->addWidget(mDescriptionContent);

	mVBox->addLayout(mGrid);
	//mVBox->setSpacing(20);
	mVBox->addLayout(mHBoxDescription);
	mVBox->addLayout(mHBox);

	mDialog->setLayout(mVBox);

    QString title = tr("Project Info: ") + name;
	
	mDialog->setWindowTitle(title);

    mDialog->setMinimumWidth(350);
    mDialog->adjustSize();
	mDialog->installEventFilter(this);

	mCurrentKeyword = keyword;
	mCurrentUserName = userName;
	mCurrentAffiliation = affiliation;
	mCurrentDescription = description;

}

void ProjectInfoDialog::hide()
{
	bool isHide = saveNotification();
	if (isHide)
		mDialog->hide();
}

void ProjectInfoDialog::edit()
{
	editButton->setEnabled(false);
	saveButton->setEnabled(true);

	QString text = mUserNameContent->text();
	mGrid->removeWidget(mUserNameContent);
	delete mUserNameContent;
	mUserNameLineEdit = new QLineEdit();
	mUserNameLineEdit->setFixedWidth(200);
	mUserNameLineEdit->setText(text);
	mUserName->setBuddy(mUserNameLineEdit);
	mGrid->addWidget(mUserNameLineEdit, 3, 1, 1, 1, Qt::AlignLeft);

	text = mKeywordContent->text();
	mGrid->removeWidget(mKeywordContent);
	delete mKeywordContent;
	mKeywordLineEdit = new QLineEdit();
	mKeywordLineEdit->setFixedWidth(200);
	mKeywordLineEdit->setText(text);
	mKeyword->setBuddy(mKeywordLineEdit);
	mGrid->addWidget(mKeywordLineEdit, 4, 1, 1, 1, Qt::AlignLeft);

	text = mAffiliationContent->text();
	mGrid->removeWidget(mAffiliationContent);
	delete mAffiliationContent;
	mAffiliationLineEdit = new QLineEdit();
	mAffiliationLineEdit->setFixedWidth(200);
	mAffiliationLineEdit->setText(text);
	mAffiliation->setBuddy(mAffiliationLineEdit);
	mGrid->addWidget(mAffiliationLineEdit, 5, 1, 1, 1, Qt::AlignLeft);

	text = mDescriptionContent->text();
	mHBoxDescription->removeWidget(mDescriptionContent);
	delete mDescriptionContent;
	mDescriptionText = new QTextEdit();
	mDescriptionText->setText(text);
	mDescriptionText->setFixedHeight(150);
	mHBoxDescription->addWidget(mDescriptionText);
}

void ProjectInfoDialog::save()
{
	editButton->setEnabled(true);
	saveButton->setEnabled(false);

	mCurrentUserName = mUserNameLineEdit->text();
	mGrid->removeWidget(mUserNameLineEdit);
	delete mUserNameLineEdit;
	mUserNameContent = new QLabel(mCurrentUserName);
	mUserName->setBuddy(mUserNameContent);
	mGrid->addWidget(mUserNameContent, 3, 1, 1, 1, Qt::AlignLeft);

	mCurrentKeyword = mKeywordLineEdit->text();
	mGrid->removeWidget(mKeywordLineEdit);
	delete mKeywordLineEdit;
	mKeywordContent = new QLabel(mCurrentKeyword);
	mKeyword->setBuddy(mKeywordContent);
	mGrid->addWidget(mKeywordContent, 4, 1, 1, 1, Qt::AlignLeft);

	mCurrentAffiliation = mAffiliationLineEdit->text();
	mGrid->removeWidget(mAffiliationLineEdit);
	delete mAffiliationLineEdit;
	mAffiliationContent = new QLabel(mCurrentAffiliation);
	mAffiliation->setBuddy(mAffiliationContent);
	mGrid->addWidget(mAffiliationContent, 5, 1, 1, 1, Qt::AlignLeft);

	mCurrentDescription = mDescriptionText->toPlainText();
	mHBoxDescription->removeWidget(mDescriptionText);
	delete mDescriptionText;
	mDescriptionContent = new QLabel();
	mDescriptionContent->setText(mCurrentDescription);
	mDescriptionContent->setFixedHeight(150);
	mDescriptionContent->setAlignment(Qt::AlignTop);
	mHBoxDescription->addWidget(mDescriptionContent);

	emit projectInfoChanged(mCurrentUserName, mCurrentKeyword, mCurrentAffiliation, mCurrentDescription);

}

bool ProjectInfoDialog::eventFilter(QObject * obj, QEvent * e)
{
    switch (e->type())
    {
        case QEvent::Close:
        {
			bool isHide = saveNotification();
			if (!isHide)
			{
				e->ignore();
				return true;
			}
            break;
        }
        default:
            qt_noop();
    }
    return QObject::eventFilter(obj, e);
}

bool ProjectInfoDialog::saveNotification()
{
	if (saveButton->isEnabled() && !mDialog->isHidden())
	{
		if (mCurrentUserName != mUserNameLineEdit->text() || mCurrentKeyword != mKeywordLineEdit->text() || mCurrentAffiliation != mAffiliationLineEdit->text() 
			|| mCurrentDescription != mDescriptionText->toPlainText())
		{
			int ret = QMessageBox::warning(this, tr("Warning"),
				   tr("Current Project Info has unsaved changes. Do you want to save before closing this window?"),
				   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
				   QMessageBox::Save);
			if(ret == QMessageBox::Cancel) 
			{
				return false;
			}
			else if (ret == QMessageBox::Save)
			{
				save();
			}
		}
	}
	return true;
}