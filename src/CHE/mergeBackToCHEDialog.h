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

#ifndef MERGE_BACK_TO_CHE_DIALOG_H
#define MERGE_BACK_TO_CHE_DIALOG_H

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
 * This class is the dialog that help user to merge objects in current project back to their original CHE.
 */
class MergeBackToCHEDialog : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 * @param  objects  objects is the map of <object absolute path, its CHE absolute path>.
	 */
	MergeBackToCHEDialog(QMap<QString, QString> objects);

	/**
	 * @brief  Get the objects that should be merged back.
	 * @return The map of <object absolute path, its CHE absolute path>.
	 */
	QMap<QString, QString> getMergeList();

	/**
	 * @brief  Get the CHE categories of the given object
	 * @param  object  The object absolute path.
	 * @return The vector of categories encoded as int id.
	 */
	QVector<int> getCategories(const QString object);

	/**
	 * @brief  Show the dialog. Overload from QWidget.
	 */
	void exec()	{mDialog->exec();}

private slots:
	/**
	 * @brief  Handle Merge button.
	 */
	void merge();

	/**
	 * @brief  Handle Cancel button.
	 */
	void cancel();

	/**
	 * @brief  Handle SelectAll button.
	 */
	void selectAll();

	/**
	 * @brief  Handle the change of check state of items in tree widget.
	 *         If the object item is checked, then all its child (category items) should all be checked.
	 *         If the object item is unchecked, then all its child (category items) should all be unchecked.
	 *         If an object is selected without CHE path, MergeBackToCHELocationDialog will jump out to let
	 *         user specify the location to be merged.
	 * @param  item    item that is checked.
	 * @param  column  checked column number to match the signal, useless here.
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
	QMap<QString, QString> mObject;
	QMap<QString, QVector<int> > mSelected; 

};

#endif // MERGE_BACK_TO_CHE_DIALOG_H