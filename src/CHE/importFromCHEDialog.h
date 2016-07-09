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

#ifndef IMPORT_FROM_CHE_DIALOG_H
#define IMPORT_FROM_CHE_DIALOG_H

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
 * This class is designed as a dialog to choose what information
 * should be imported from a CHE.
 */

class ImportFromCHEDialog : public QWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor with all the objects' full path as input.
	 */
	ImportFromCHEDialog(QVector<QString> objects);

	/**
	 * Get the filter result, which contains all the objects that are not selected for import.
	 */
	QVector<QString> getFilterList();

	/**
	 * Get the categories of the selected object.
	 * Input is the full path of a selected object .
	 */
	QVector<int> getCategories(const QString object);	

	void exec()	{mDialog->exec();}

	/**
	 * Test whether the import should be processed or canceled.
	 */
	bool isImport() {return isImported;}

private slots:
	/**
	 * Handle buttons.
	 */
	void import();

	void cancel();

	void selectAll();

	/**
	 * Handle the change of check state of items in tree widget.
	 * If the object item is checked, then all its child (category items) should all be checked.
	 * If the object item is unchecked, then all its child (category items) should all be unchecked.
	 */
	void itemChanged(QTreeWidgetItem * item, int column);

private:
	QDialog* mDialog;
	QVBoxLayout* mVbox;

	QGridLayout* mButtonGridBox;

	QPushButton* mImportButton;
	QPushButton* mCancelButton;
	QPushButton* mSelectAllButton;

	QLabel *mLabel;

	QTreeWidget* mTreeWidget;
	QList<QTreeWidgetItem *> mItems;
	QVector<QString> mObject;
	QMap<QString, QVector<int> > mSelected; 
	bool isImported;	// import button pressed true, otherwise false;

};

#endif // IMPORT_FROM_CHE_DIALOG