/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Zeyu Wang (zeyu.wang@yale.edu)

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

#ifndef VIDEO_SETUP_H
#define VIDEO_SETUP_H

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
#include <QObject>

/**
 * This class is the dialog help user to select preferences to set up the video
 */
class VideoSetup : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 * @param  title		Initial project name
	 * @param  author		Initial user name
	 * @param  affiliation	Initial affiliation
	 */
	VideoSetup(QString title, QString author, QString affiliation);

	/**
	 * @brief  Show the dialog. Overload from QWidget.
	 */
	void exec()	{mDialog->exec();}

	/**
	 * @brief  Test whether the dialog should be processed or canceled.
	 * @return If Generate button is pressed then return true, otherwise return false.
	 */
	bool checkNext() {return isNext;}

	void updateSetup(QString* newProjectName, QString* newUserName, QString* newAffiliation, int* setupVideoFormat, int* setupResolution,
		int* setupFrameDuration2D, int* setupTransDuration2D, int* setupFrameDuration3D, int* setupTransDuration3D, int* setupDolly3D, bool* setupShowGeneral, bool* setupShow2RTIs);

private slots:
	/**
	 * @brief  Handle Generate button and start to generate the report.
	 */
	void next();

	/**
	 * @brief  Handle Cancel button.
	 */
	void cancel();

	void setVideoFormat(int index);

	void setResolution(int index);

	void setShowGeneral(int state);

	void setShow2RTIs(int state);

private:
	QDialog* mDialog;
	QVBoxLayout* mVbox;

	QGridLayout* mGrid;
	QGridLayout* mButtonGridBox;

	QPushButton* mNextButton;
	QPushButton* mCancelButton;

	QLabel *mProjectNameLabel, *mUserNameLabel, *mAffiliationLabel, *mVideoFormatLabel, *mResolutionLabel,
		*mFrameDuration2DLabel, *mFrameDuration3DLabel, *mTransDuration2DLabel, *mTransDuration3DLabel, *mDolly3DLabel;
	QLineEdit *mProjectNameContent, *mUserNameContent, *mAffiliationContent,
		*mFrameDuration2DContent, *mFrameDuration3DContent, *mTransDuration2DContent, *mTransDuration3DContent, *mDolly3DContent;
	QComboBox *mVideoFormatContent, *mResolutionContent;
	QCheckBox *mShowGeneralContent, *mShow2RTIsContent;

	QString mProjectName;
	QString mUserName;
	QString mAffiliation;
	int mVideoFormat; // DIV3, MP42, WMV2
	int mResolution; // 800*600, 1600*900, 1600*1200 // QComboBox
	int mFrameDuration2D;
	int mTransDuration2D;
	int mFrameDuration3D;
	int mTransDuration3D;
	int mDolly3D;
	bool isShowGeneral; // QCheckBox
	bool isShow2RTIs; // QCheckBox
	bool isNext;

};

#endif // VIDEO_SETUP