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
#include "../vtkEnums.h"

/**
 * This class is the dialog helping user to export CHE to a new project.
 */
class ExportToProjectDialog : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 * @param  user  User name of the exported project.
	 * @param  path  System last used path.
	 */
	ExportToProjectDialog(const QString user, const QString path = NULL);

	/**
	 * @brief  Get necessary information of the new project from the dialog.
	 */
	QString getProjectName()	{return mProjectNameEdit->text();}

	QString getProjectPath()	{return mLocationLineEdit->text();}

	QString getUserName()	{return mUserNameLineEdit->text();}

	QString getKeyword()	{return mKeywordLineEdit->text();}

	QString getAffiliation()	{return mAffiliationLineEdit->text();}

	QString getDescription()	{return mDescriptionEdit->toPlainText();}

	/**
	 * @brief  Get the selected categories of CHE that will be exported to the new project.
	 * @return The vector of categories encoded as int id.
	 */
	QList<int> getCategories();

	/**
	 * @brief  Show the dialog. Overload from QWidget.
	 */
	void exec()	{mDialog->exec();}

	/**
	 * @brief  Test whether the export should be executed or canceled.
	 * @return If the Ok button is pressed, then return true, otherwise return false.
	 */
	bool checkOk()	{return isOk;}

private slots:	
	/**
	 * @brief  Handle Browse buttons.
	 */
	void locationBrowse();

	/**
	 * @brief  Handle Next buttons.
	 */
	void next();

	/**
	 * @brief  Handle Cancel buttons of the warning dialog.
	 */
	void nextCancel();

	/**
	 * @brief  Handle Cancel buttons.
	 */
	void cancel();

	/**
	 * @brief  Handle SelectAll buttons.
	 */
	void selectAll();

	/**
	 * @brief  Process next step when Next button is pressed and handle OK button of the warning dialog.
	 *         Only when the name and location of the new project are specified, the next button 
	 *         can be enabled to start export.
	 */
	void nextReady();

	/**
	 * @brief  Enable next button.
	 */
	void enableNextButton();

	/**
	 * @brief  Save the state when name is specified or changed.
	 */
	void projectNameChanged(QString name);

	/**
	 * @brief  Save the state when location is specified or changed.
	 */
	void projectPathChanged(QString path);

signals:
	/**
	 * @brief  Send the signal when project info changes.
	 */
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