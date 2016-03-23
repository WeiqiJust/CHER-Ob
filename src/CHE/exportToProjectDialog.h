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

#ifndef EXPORT_TO_PROJECT_DIALOG_H
#define EXPORT_TO_PROJECT_DIALOG_H

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
#include <QTreeView>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class ExportToProjectDialog : public QWidget
{
	Q_OBJECT

public:
    ExportToProjectDialog(const QString user, const QString path = NULL);

	QString getProjectName()	{return mProjectNameEdit->text();}

	QString getProjectPath()	{return mLocationLineEdit->text();}

	QString getUserName()	{return mUserNameLineEdit->text();}

	QString getKeyword()	{return mKeywordLineEdit->text();}

	QString getAffiliation()	{return mAffiliationLineEdit->text();}

	QString getDescription()	{return mDescriptionEdit->toPlainText();}

	QList<int> getCategories();

	void exec()	{mDialog->exec();}

	bool checkOk()	{return isOk;}

private slots:	
	void locationBrowse();

	void next();

	void nextReady();

	void nextCancel();

	void cancel();

	void selectAll();

	void enableNextButton();

	void projectNameChanged(QString name);

	void projectPathChanged(QString path);

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

	QPushButton* mLocationBrowseButton;
    QPushButton* mNextButton;
    QPushButton* mCancelButton;
	QPushButton* mSelectAllButton;

	QLabel *mUserNameLabel;
    QLabel *mProjectNameLabel;
    QLabel *mLocationLabel;
	QLabel *mKeywordLabel;
	QLabel *mAffiliationLabel;
	QLabel *mDescriptionLabel;
	QLabel *mCategoryLabel;
	
	QLineEdit *mUserNameLineEdit;
    QLineEdit *mProjectNameEdit;
    QLineEdit *mLocationLineEdit;
	QLineEdit *mKeywordLineEdit;
	QLineEdit *mAffiliationLineEdit;
	QTextEdit* mDescriptionEdit;

	QTreeWidget* mTreeWidget;
	QList<QTreeWidgetItem *> mItems;

	QString mLastPath;
	QString mProjectPath;
	QString mProjectName;
	QString mFullPath;
	bool isOk;

};

#endif // EXPORT_TO_PROJECT_DIALOG_H