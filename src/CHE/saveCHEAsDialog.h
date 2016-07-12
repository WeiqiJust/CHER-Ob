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

/**
 * This class is the dialog to help user save CHE as another CHE.
 */

class SaveCHEAsDialog : public CHEInfoDialog
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 * @param  user  User name.
	 * @param  info  CHE text information.
	 * @param  path  The system last used path.
	 */
	SaveCHEAsDialog(const QString user, const CHEInfoBasic* info, const QString path = NULL);

	/**
	 * @brief  Get the new CHE name.
	 */
	QString getProjectName()	{return mProjectNameEdit->text();}

	/**
	 * @brief  Get the new CHE absolute path.
	 */
	QString getProjectPath()	{return mLocationLineEdit->text();}

	/**
	 * @brief  Get the new CHE user name.
	 */
	QString getUserName()	{return mUserNameLineEdit->text();}

	/**
	 * @brief  Get the new CHE text information.
	 */
	CHEInfoBasic* getCHEInfo();

	/**
	 * @brief  Show the dialog. Overload from QWidget.
	 */
	void exec()	{mDialog->exec();}

	/**
	 * @brief  Test whether the dialog should be processed or canceled.
	 * @return If Ok button is pressed then return true, otherwise return false.
	 */
	bool checkOk()	{return isOk;}

private slots:
	/**
	 * @brief  Handle Location Browse button.
	 */
	void locationBrowse();

	/**
	 * @brief  Handle Next button.
	 */
	void next();

	/**
	 * @brief  Handle Cancel button.
	 */
	void cancel();

	/**
	 * @brief  Change CHE name and trigger infoUpdate signal.
	 * @param  name  The changed name.
	 */
	void projectNameChanged(QString name);

	/**
	 * @brief  Change CHE path and trigger infoUpdate signal.
	 * @param  path  The changed path.
	 */
	void projectPathChanged(QString path);

	/**
	 * @brief  Enable Next button when name and path are specified.
	 */
	void enableNextButton();

	/**
	 * @brief  Hide the dialog and show the newCHEInfoDialog.
	 */
	void nextReady();

	/**
	 * @brief  Handle the Cancle button of the warning dialog.
	 */
	void nextCancel();

signals:
	/**
	 * @brief  When CHE name or path is changed, send the signal. When both are specified,
	 *         then next button is enabled.
	 */
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
