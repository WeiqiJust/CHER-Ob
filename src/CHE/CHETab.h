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

#ifndef CHE_TAB_H
#define CHE_TAB_H

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
#include <QComboBox>
#include "CHEInfoDialog.h"

/**
 * This class provides CHE tab information. It will be created when
 * a CHE is opened.
 */
class CHETab : public CHEInfoDialog
{
	Q_OBJECT

public:
	/**
	 * Constructor with basic CHE info as input, and it will be loaded into the tab.
	 */
	CHETab(const CHEInfoBasic* info, QWidget *parent = 0);

	/**
	 * Get CHE info from CHE tab.
	 */
	CHEInfoBasic* getCHEInfo();

	/**
	 * Update CHe info shown on the tab when it is changed.
	 */
	void updateCHEInfo(const CHEInfoBasic* info);

	/**
	 * Test whether current CHE info shown on the tab is saved.
	 */
	bool isSaved()	{return !saveButton->isEnabled();}

public slots:
	/**
	 * Send signal when save button is pressed.
	 */
	void savePressed();

private slots:
	/**
	 * Send signal when specific button is pressed.
	 */
	void exportPressed()	{emit exportProject();}

	void editPressed();

	/**
	 * Set the filter mode of the tab to show specific type of CHE info.
	 */
	void setFilterMode(int mode);

signals:
	/**
	 * Handle buttons.
	 */
	void save();

	void exportProject();

private:
	QPushButton* exportButton;
	QPushButton* saveButton;
	QPushButton* editButton;

	QComboBox* mFilter;
	QLabel *mLabel;
	QTextEdit* mTextEdit;

	int currentMode;
	QVector<QString> mTemptInfo;
};


#endif // CHE_TAB_H
