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

#ifndef NEW_PROJECT_DIALOG_H
#define NEW_PROJECT_DIALOG_H

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
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "../vtkEnums.h"
#include "vtkImageData.h"
#include "vtkImageViewer2.h"
#include "vtkSmartPointer.h"
#include "vtkTexture.h"

class MainWindow;
class VtkWidget;

/**
 * This class is the dialog to help user generate new project.
 */

class NewProjectDialog : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 * @param  path  The last used system path.
	 */
    NewProjectDialog(const QString path = NULL);

	/**
	 * @brief  Get the project name.
	 * @return Project name string.
	 */
	QString getProjectName();

	/**
	 * @brief  Get the CHE absolute path.
	 * @return CHE absolute path.
	 */
	QString getProjectPath();

	/**
	 * @brief  Show the dialog. Overload from QWidget.
	 */
	void exec()	{mDialog->exec();}

signals:
	/**
	 * @brief  When CHE name or path is changed, send the signal. When both are specified,
	 *         then next button is enabled.
	 */
	void infoUpdate();

	/**
	 * @brief  When Next button is pressed, send this signal to main frame.
	 * @param  Project Full Path, Project Name, User Mode,  User Name, Imported Object Full Path, 
		       Imported CT Full Path, Keyword, Affiliation, Description.
	 */
	void nextPressed(const QString, const QString, const USERMODE, const QString, 
		const QString, const QString, const QString, const QString, const QString);

private slots:
	/**
	 * @brief  Handle Location Browse button.
	 */
	void locationBrowse();

	/**
	 * @brief  Handle Object Browse button.
	 */
	void objectBrowse();

	/**
	 * @brief  Handle CT Browse button.
	 */
	void ctBrowse();

	/**
	 * @brief  Handle Next Browse button.
	 */
	void next();

	/**
	 * @brief  Handle Cancel Browse button.
	 */
	void cancel();

	/**
	 * @brief  Change project name and trigger infoUpdate signal.
	 * @param  name  The changed name.
	 */
	void projectNameChanged(QString name);

	/**
	 * @brief  Change project path and trigger infoUpdate signal.
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

private:
	QDialog* mDialog;
	QVBoxLayout* mVbox;
	QVBoxLayout* mRoleVBox;
	QVBoxLayout* mLoadDataVBox;
	QHBoxLayout* mUserNameHBox;
	QHBoxLayout* mObjectHBox;
	QHBoxLayout* mCTHBox;
	QVBoxLayout* mProjectInfoVBox;
	QHBoxLayout* mProjectHBox;
	QHBoxLayout* mLocationHBox;
	QHBoxLayout* mKeywordHBox;
	QHBoxLayout* mAffiliationHBox;
	
	QGridLayout* mButtonGridBox;

	QGroupBox *mGroupBox;
	QGroupBox *mLoadDataGroupBox;
	QGroupBox *mProjectInfoGroupBox;

    QRadioButton *mCreatorRadioButton;
    QRadioButton *mModifierRadioButton;
    QRadioButton *mViewerRadioButton;

	QPushButton* mLocationBrowseButton;
	QPushButton* mObjectBrowseButton;
	QPushButton* mCTBrowseButton;
    QPushButton* mNextButton;
    QPushButton* mCancelButton;

	QLabel *mUserNameLabel;
	QLabel *mLoadDataLabel;
	QLabel *mObjectLabel;
    QLabel *mCTLabel;
    QLabel *mProjectNameLabel;
    QLabel *mLocationLabel;
	QLabel *mKeywordLabel;
	QLabel *mAffiliationLabel;
	QLabel *mDescriptionLabel;
	
	QLineEdit *mUserNameLineEdit;
	QLineEdit *mObjectLineEdit;
    QLineEdit *mCTLineEdit;
    QLineEdit *mProjectNameEdit;
    QLineEdit *mLocationLineEdit;
	QLineEdit *mKeywordLineEdit;
	QLineEdit *mAffiliationLineEdit;
	QTextEdit* mDescriptionEdit;

	QString mLastPath;
	QString mProjectPath;
	QString mProjectName;
	QString mFullPath;

};
#endif // NEW_PROJECT_DIALOG