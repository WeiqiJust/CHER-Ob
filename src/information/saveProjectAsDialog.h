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

#ifndef SAVE_PROJECT_AS_DIALOG_H
#define SAVE_PROJECT_AS_DIALOG_H

#include <QtScript>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QToolBar>
#include <QTextBrowser>
#include <QFile>
#include <QWizard>
#include <QFileDialog>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QObject>
#include <QMessageBox>
#include "../vtkEnums.h"

class MainWindow;
class VtkWidget;

class SaveProjectAsDialog : public QWidget
{
	Q_OBJECT

public:
    SaveProjectAsDialog(const QString user, const QString keyword, const QString affiliation, const QString description, const QString path = NULL);

	QString getProjectName()	{return mProjectNameEdit->text();}

	QString getProjectPath()	{return mLocationLineEdit->text();}

	QString getUserName()	{return mUserNameLineEdit->text();}

	QString getKeyword()	{return mKeywordLineEdit->text();}

	QString getAffiliation()	{return mAffiliationLineEdit->text();}

	QString getDescription()	{return mDescriptionEdit->toPlainText();}

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
	QDialog* mDialog;
	QVBoxLayout* mVbox;
	QHBoxLayout* mUserNameHBox;
	QHBoxLayout* mProjectHBox;
	QHBoxLayout* mLocationHBox;
	QHBoxLayout* mKeywordHBox;
	QHBoxLayout* mAffiliationHBox;
	
	QGridLayout* mButtonGridBox;

	QGroupBox *mGroupBox;

	QPushButton* mLocationBrowseButton;
    QPushButton* mNextButton;
    QPushButton* mCancelButton;

	QLabel *mUserNameLabel;
    QLabel *mProjectNameLabel;
    QLabel *mLocationLabel;
	QLabel *mKeywordLabel;
	QLabel *mAffiliationLabel;
	QLabel *mDescriptionLabel;
	
	QLineEdit *mUserNameLineEdit;
    QLineEdit *mProjectNameEdit;
    QLineEdit *mLocationLineEdit;
	QLineEdit *mKeywordLineEdit;
	QLineEdit *mAffiliationLineEdit;
	QTextEdit* mDescriptionEdit;

	QString mLastPath;
	QString mProjectPath;
	QString mProjectName;
	QString mFullPath;
	bool isOk;

};

#endif // SAVE_PROJECT_AS_DIALOG_H