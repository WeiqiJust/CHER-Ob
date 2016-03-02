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

#ifndef DIALOG_H
#define DIALOG_H

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
#include "vtkEnums.h"
#include "vtkImageData.h"
#include "vtkImageViewer2.h"
#include "vtkSmartPointer.h"
#include "vtkTexture.h"

class MainWindow;
class VtkWidget;

class NewProjectDialog : public QWidget
{
	Q_OBJECT

public:
    NewProjectDialog(const QString path = NULL);

	QString getProjectName();

	QString getProjectPath();

	void exec()	{mDialog->exec();}

signals:
	void infoUpdate();

	void nextPressed(const QString, const QString, const USERMODE, const QString, 
		const QString, const QString, const QString, const QString, const QString);

private slots:	
	void locationBrowse();

	void objectBrowse();

	void ctBrowse();

	void next();

	void cancel();

	void projectNameChanged(QString name);

	void projectPathChanged(QString path);

	void enableNextButton();

	void nextReady();

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

class FileInfoDialog : public QWidget
{
	Q_OBJECT

public:
	FileInfoDialog(VtkWidget* w = 0);

public slots:
	void hide()	{mDialog->hide();}

private:
	void get2DInfo();

	void get3DInfo();

	QString formatFilesize(qint64 bytes);

private:
	QDialog* mDialog;
	QGridLayout* grid;
	QHBoxLayout* hboxButtons;
	QLabel* openGLLabel, *coresLabel, *filenameLabel, *filesizeLabel, *filesizeLabel2, *dimensionsLabel, *dimensionsLabel2;
	QLabel* texFilenameLabel, *texFilesizeLabel, *texFilesizeLabel2, *texDimensionsLabel, *texDimensionsLabel2;
	QLabel* pointLabel, *pointLabel2, *polyLabel, *polyLabel2;
	QLineEdit* filenameLine, *texFilenameLine;
	QPushButton* okButton;
	QVBoxLayout* vbox, *vboxData;

	vtkImageData* hyperdata;
	vtkSmartPointer<vtkImageViewer2> viewer;
	vtkTexture* texture;
	VtkWidget* w;
	QFileInfo* finfo;
	QFileInfo* tfinfo;
	QLocale locale;
	QString textureFilename;
	int points, polys;
	bool isDICOM;
};

class ProjectInfoDialog : public QWidget
{
	Q_OBJECT

public:
	ProjectInfoDialog(const QString name, const QString path, const QString keyword, const QString affiliation,
		const QString userName, const QString description);

	void exec()	{mDialog->exec();}

public slots:
	void hide();

	void edit();

	void save();

private:
	bool eventFilter(QObject* obj, QEvent* e);

	bool saveNotification();

signals:
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

class OpenWindowDialog : public QWidget
{
	Q_OBJECT

public:
	OpenWindowDialog();

	void addItem(QString name);

	void exec()	{mDialog->exec();}

	QStringList selectedItems()	{return mSelection;}

private slots:
	void ok();

	void cancel();

private:
	QDialog* mDialog;
	QVBoxLayout* mVBox;
	QGridLayout* mGrid;
	QLabel *mInstruction;
	QTreeWidget* mTreeWidget;
	QList<QTreeWidgetItem *> mItems;
	QStringList mSelection;
	QPushButton* mOkButton;
	QPushButton* mCancelButton;
};

class RemoveObjectDialog : public QWidget
{
	Q_OBJECT

public:
	RemoveObjectDialog();

	void addItem(QString name);

	void exec()	{mDialog->exec();}

	QStringList selectedItems()	{return mSelection;}

	bool  checkExport()	{return isExport;}

	QString getPath()	{return mPath;}

private slots:
	void ok();

	void cancel();

private:
	QDialog* mDialog;
	QVBoxLayout* mVBox;
	QGridLayout* mGrid;
	QLabel *mInstruction;
	QTreeWidget* mTreeWidget;
	QList<QTreeWidgetItem *> mItems;
	QStringList mSelection;
	QPushButton* mOkButton;
	QPushButton* mCancelButton;
	bool isExport;
	QString mPath;
};

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



signals:
	void infoUpdate();

private slots:	
	void locationBrowse();

	void next();

	void cancel();

	void projectNameChanged(QString name);

	void projectPathChanged(QString path);

	void enableNextButton();

	void nextReady();

	void nextCancel();

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

#endif // DIALOG_H