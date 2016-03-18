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
#include "CHEBasicInfo.h"
#include "vtkEnums.h"

class NewCHEDialog : public QWidget
{
	Q_OBJECT

public:
    NewCHEDialog(const QString path = NULL);

	QString getCHEName();

	QString getCHEPath();

	void exec()	{mDialog->exec();}

signals:
	void infoUpdate();

	void createCHE(const QString, const QString, const USERMODE, const CHEInfoBasic*, const QString);

private slots:	
	void locationBrowse();

	void objectBrowse();

	void ctBrowse();

	void next();

	void cancel();

	void CHENameChanged(QString name);

	void CHEPathChanged(QString path);

	void enableNextButton();

	void nextReady();

	void nextCancel();

	void CHEInfoOK(const CHEInfoBasic* info);

	void CHEInfoBack();

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

	CHENewInfoDialog *mCHEInfo;

};


#endif // CULTURE_HERITAGE_ENTITY_H