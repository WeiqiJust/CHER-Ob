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

#ifndef MERGE_BACK_TO_CHE_LOCATION_DIALOG_H
#define MERGE_BACK_TO_CHE_LOCATION_DIALOG_H

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

/**
 * This class is the dialog to specify merge-back locations of the objects without original CHE path.
 * These objects are not imported from any CHE and their CHE paths are left as empty string.
 * Other objects' original CHE paths are options for user to choose.
 */

class MergeBackToCHELocationDialog : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 * @param  locations  locations is the vector of other objects' CHE absolute paths. 
	 *					  Each path only appear once in the vector.
	 */
	MergeBackToCHELocationDialog(QVector<QString> locations);

	/**
	 * @brief  Show the dialog. Overload from QWidget.
	 */
	void exec()	{mDialog->exec();}

	/**
	 * @brief  Test whether the dialog should be processed or canceled.
	 * @return If Ok button is pressed then return true, otherwise return false.
	 */
	bool checkOk() {return isOk;}

	/**
	 * @brief  Get selected location string.
	 * @return The string of selected absolute path.
	 */
	QString getSelected() {return mSelected;}

private slots:
	/**
	 * @brief  Handle Ok button.
	 */
	void ok();

	/**
	 * @brief  Handle Cancel button.
	 */
	void cancel();

	/**
	 * @brief  When one item is checked, set other items as unchecked.
	 * @param  item    item that is checked.
	 * @param  column  checked column number to match the signal, useless here.
	 */
	void itemChanged(QTreeWidgetItem * item, int column);

private:
	QDialog* mDialog;
	QVBoxLayout* mVbox;

	QGridLayout* mButtonGridBox;

	QPushButton* mOkButton;
	QPushButton* mCancelButton;

	QLabel *mLabel;

	QTreeWidget* mTreeWidget;
	QList<QTreeWidgetItem *> mItems;
	QString mSelected;
	bool isOk;

};

#endif