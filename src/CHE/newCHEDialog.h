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

#ifndef CULTURE_HERITAGE_ENTITY_H
#define CULTURE_HERITAGE_ENTITY_H

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
#include <QCoreApplication>
#include "CHEInfoDialog.h"
#include "newCHEInfoDialog.h"
#include "../vtkEnums.h"

/**
 * This class is the dialog to help user generate new CHE.
 */

class NewCHEDialog : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 * @param  path  The last used system path.
	 */
    NewCHEDialog(const QString path = NULL);

	/**
	 * @brief  Get the CHE name.
	 * @return CHE name string.
	 */
	QString getCHEName();

	/**
	 * @brief  Get the CHE absolute path.
	 * @return CHE absolute path.
	 */
	QString getCHEPath();

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
	 * @param  CHE Full Path, CHE Name, User Mode, CHE info, User Name, Imported Object Full path, 
     *         Imported CT folder path.
	 */
	void createCHE(const QString, const QString, const USERMODE, const CHEInfoBasic*,
		const QString, const QString, const QString);

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
	 * @brief  Change CHE name and trigger infoUpdate signal.
	 * @param  name  The changed name.
	 */
	void CHENameChanged(QString name);

	/**
	 * @brief  Change CHE path and trigger infoUpdate signal.
	 * @param  path  The changed path.
	 */
	void CHEPathChanged(QString path);

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

	/**
	 * @brief  Handle newCHEInfoDialog Ok button.
	 * @param  info  The input CHE info from newCHEinfoDialog.
	 */
	void CHEInfoOK(const CHEInfoBasic* info);

	/**
	 * @brief  Handle newCHEInfoDialog Back button.
	 */
	void CHEInfoBack();

	/**
	 * @brief  Handle newCHEInfoDialog Cancel button.
	 */
	void CHEInfoCancel();

private:
	QDialog* mDialog;
	QVBoxLayout* mVbox;
	QVBoxLayout* mRoleVBox;
	QVBoxLayout* mLoadDataVBox;
	QHBoxLayout* mUserNameHBox;
	QHBoxLayout* mObjectHBox;
	QHBoxLayout* mCTHBox;
	QVBoxLayout* mCHEInfoVBox;
	QHBoxLayout* mCHEHBox;
	QHBoxLayout* mLocationHBox;
	
	QGridLayout* mButtonGridBox;

	QGroupBox *mGroupBox;
	QGroupBox *mLoadDataGroupBox;
	QGroupBox *mCHEInfoGroupBox;

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
    QLabel *mCHENameLabel;
    QLabel *mLocationLabel;
	
	QLineEdit *mUserNameLineEdit;
	QLineEdit *mObjectLineEdit;
    QLineEdit *mCTLineEdit;
    QLineEdit *mCHENameEdit;
    QLineEdit *mLocationLineEdit;

	QString mLastPath;
	QString mCHEPath;
	QString mCHEName;
	QString mFullPath;

	NewCHEInfoDialog *mCHEInfo;

};


#endif // CULTURE_HERITAGE_ENTITY_H