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

#ifndef VIDEO_NOTE_FILTER_H
#define VIDEO_NOTE_FILTER_H

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
#include <QDropEvent>
#include "videoFilter.h"
#include "videoGenerator.h"

class VideoObject;
class dragTreeWidget;

/**
 * This class is the dialog help user to select contents that would appear
 * in video.
 */
class VideoNoteFilter : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 * @param  The video object that contains note information.
	 */
	VideoNoteFilter(VideoObject* mainVideoObject);

	/**
	 * @brief  Show the dialog. Overload from QWidget.
	 */
	void exec()	{mDialog->exec();}

	/**
	 * @brief  Test whether the dialog should be processed or canceled.
	 * @return If Generate button is pressed then return true, otherwise return false.
	 */
	bool checkGenerate() {return isGenerate;}

	bool checkSkipAll() {return isSkipAll;}
	
	QVector<int> dragPermutation;


private slots:
	/**
	 * @brief  Handle Generate button and start to generate the report.
	 */
	void generate();

	/**
	 * @brief  Handle Cancel button.
	 */
	void cancel();

	/**
	 * @brief  Handle SkipAll button.
	 */
	void skipAll();

private:
	QDialog* mDialog;
	QVBoxLayout* mVbox;

	QGridLayout* mButtonGridBox;

	QPushButton* mGenerateButton;
	QPushButton* mCancelButton;
	QPushButton* mSkipAllButton;

	QLabel* mLabel;

	dragTreeWidget* mTreeWidget;
	QList<QTreeWidgetItem*> mItems;
	bool isGenerate, isSkipAll;

	VideoObject* thisVideoObject;
};

#endif // VIDEO_NOTE_FILTER