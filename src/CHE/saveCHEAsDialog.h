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

#ifndef SAVE_CHE_AS_DIALOG_H
#define SAVE_CHE_AS_DIALOG_H

#include <QtScript>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include "CHEInfoDialog.h"

class SaveCHEAsDialog : public CHEInfoDialog
{
	Q_OBJECT

public:
	SaveCHEAsDialog(const QString user, const CHEInfoBasic* info, const QString path = NULL);

	QString getProjectName()	{return mProjectNameEdit->text();}

	QString getProjectPath()	{return mLocationLineEdit->text();}

	QString getUserName()	{return mUserNameLineEdit->text();}

	CHEInfoBasic* getCHEInfo();

	void exec()	{mDialog->exec();}

	bool checkOk()	{return isOk;}

private slots:
	void locationBrowse();

	void next();

	void cancel();

	void projectNameChanged(QString name);

	void projectPathChanged(QString path);

	void enableNextButton();

	void nextReady();

	void nextCancel();

signals:
	void infoUpdate();

private:
	QHBoxLayout* mUserNameHBox;
	QHBoxLayout* mProjectHBox;
	QHBoxLayout* mLocationHBox;

	QGridLayout* mButtonGridBox;

	QLabel *mUserNameLabel;
    QLabel *mProjectNameLabel;
    QLabel *mLocationLabel;

	QLineEdit *mUserNameLineEdit;
    QLineEdit *mProjectNameEdit;
    QLineEdit *mLocationLineEdit;

	QPushButton* mLocationBrowseButton;
    QPushButton* mNextButton;
    QPushButton* mCancelButton;

	QString mLastPath;
	QString mProjectPath;
	QString mProjectName;
	QString mFullPath;
	bool isOk;
};


#endif // SAVE_CHE_AS_DIALOG_H
