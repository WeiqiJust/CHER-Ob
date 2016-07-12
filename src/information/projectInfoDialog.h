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

#ifndef PROJECT_INFO_DIALOG_H
#define PROJECT_INFO_DIALOG_H

#include <QtScript>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QFile>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QObject>
#include <QMessageBox>
#include "../vtkEnums.h"

class MainWindow;
class VtkWidget;

/**
 * This class is the project info dialog, which presents necessary information for the whole project.
 * User can add the information in the dialog.
 */

class ProjectInfoDialog : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 * @param  Project name, project absolute full path, keyword, affiliation, user name and description of the project.
	 */
	ProjectInfoDialog(const QString name, const QString path, const QString keyword, const QString affiliation,
		const QString userName, const QString description);

	/**
	 * @brief  Show the dialog. Overload from QWidget.
	 */
	void exec()	{mDialog->exec();}

public slots:
	/**
	 * @brief  If the information is saved then close the dialog.
	 */
	void hide();

	/**
	 * @brief  Handle Edit button to change the label to lineedit.
	 */
	void edit();

	/**
	 * @brief  Handle Save button to change the lineedit to label.
	 */
	void save();

private:
	/**
	 * @brief  Event filter to handle the close event. When user press
	 *         the red cross to close the dialog, notify the unsaved changes
	 *         if exsits.
	 */
	bool eventFilter(QObject* obj, QEvent* e);

	/**
	 * @brief  Notify user that there are unsaved changes.
	 */
	bool saveNotification();

signals:
	/**
	 * @brief  If project info is changed, emit the signal to main frame.
	 */
	void projectInfoChanged(const QString userName, const QString keyword, const QString affiliation, const QString descrption);

private:
	QDialog* mDialog;
	QGridLayout* mGrid;
	QVBoxLayout* mVBox;
	QHBoxLayout* mHBox;
	QHBoxLayout* mHBoxDescription;
	QLabel *mName, *mNameContent, *mPath, *mPathContent, *mKeyword, *mKeywordContent, *mAffiliation, *mAffiliationContent;
	QLabel *mUserName, *mUserNameContent, *mDescription, *mDescriptionContent;
	QLineEdit *mUserNameLineEdit, *mKeywordLineEdit, *mAffiliationLineEdit;
	QTextEdit* mDescriptionText;
	QPushButton* cancelButton;
	QPushButton* editButton;
	QPushButton* saveButton;
	QString mCurrentKeyword;
	QString mCurrentAffiliation;
	QString mCurrentUserName;
	QString mCurrentDescription;
};

#endif // PROJECT_INFO_DIALOG_H