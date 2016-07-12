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

#ifndef SEARCH_H
#define SEARCH_H

#include <QtScript>
#include <QRegExp>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QFile>
#include <QWizard>
#include <QFileDialog>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QMessageBox>
#include <QTreeView>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "../vtkEnums.h"
#include "vtkImageData.h"
#include "vtkImageViewer2.h"
#include "vtkSmartPointer.h"
#include "vtkTexture.h"
#include "../function/htmlDelegate.h"

class MainWindow;

/**
 * This class implement the search widget tab on bottom of right doc.
 * It provides the search function for all the notes, annotation in the 
 * object of current window.
 */
class SearchWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 */
    SearchWidget(QWidget *parent);

	/**
	 * @brief  When there is an active window, enable all the button.
	 *         If there is no avtive window (project/CHE is closed), then remove
	 *         all the cached content.
	 * @param  activeWindow  Whether there is an active window in main frame.
	 */
	void refreshSearchTab(bool activeWindow);

	/**
	 * @brief  Find main frame.
	 */
	static MainWindow* mw();

private:
	/**
	 * @brief  Set mPath to the current object's note path for search.
	 * @return true if success, false if current object cannot be processed.
	 */
	bool updateCurrentPath();

	/**
	 * @brief  Set the filter for search. User can choose to search in certain categories.
	 * @param  mode  The int value of selected item.
	 */
	void setFilter(int mode);

private slots:
	/**
	 * Start to search.
	 */
	void search();

	/**
	 * @brief  If the item is note item, then open the note.
	 * @param  item  The item that is double clicked.
	 * @param  column  checked column number to match the signal, useless here.
	 */
	void showTreeWidgetItem(QTreeWidgetItem* item, int column);

	/**
	 * @brief  Set the filter for search to skip project info category (the first item) and call setFilter.
	 * @param  mode  The int value of selected item.
	 */
	void setFilterMode(int mode);

private:
	QHBoxLayout* hlay;
	QVBoxLayout* vlay;
	QLabel* mLabel;
	QLineEdit* mInput;
	QComboBox* mFilter;
	QPushButton* mButton;
	QTreeWidget* mTreeWidget;
	QList<QTreeWidgetItem *> mItems;
	QList<QTreeWidgetItem *> mFilteredItems;
	QString mPath;
	QString mFile;
	int mMode;
};

/**
 * This class implement the SearchAll widget tab on bottom of right doc.
 * It provides the search function for all the notes, annotation and
 * project info in the all the objects in project/CHE.
 */
class SearchAllWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 */
    SearchAllWidget(QWidget *parent);

	/**
	 * @brief  When there is an active window, enable all the button.
	 *         If there is no avtive window (project/CHE is closed), then remove
	 *         all the cached content.
	 * @param  activeWindow  Whether there is an active window in main frame.
	 */
	void refreshSearchTab(bool activeWindow);

	/**
	 * @brief  Find main frame.
	 */
	static MainWindow* mw();

private:
	/**
	 * @brief  Set mPath to the current object's note path for search.
	 * @return true if success, false if current object cannot be processed.
	 */
	bool updateCurrentPath();

	/**
	 * @brief  Match string for searching.
	 * @param  content   The input content for searching.
	 * @param  matchStr  The string to search.
	 * @return The list of matched strings in certain length.
	 */
	QStringList matchString(QString content, QString matchStr);

	/**
	 * @brief  Set the filter for search. User can choose to search in certain categories.
	 * @param  mode  The int value of selected item.
	 */
	void setFilter(int mode);

private slots:
	/**
	 * Start to search.
	 */
	void search();

	/**
	 * @brief  If the item is note item, then open the note.
	 * @param  item  The item that is double clicked.
	 * @param  column  checked column number to match the signal, useless here.
	 */
	void showTreeWidgetItem(QTreeWidgetItem* item, int column);

	/**
	 * @brief  Set the filter for search to skip project info category (the first item) and call setFilter.
	 * @param  mode  The int value of selected item.
	 */
	void setFilterMode(int mode);

private:
	QHBoxLayout* hlay;
	QVBoxLayout* vlay;
	QLabel* mLabel;
	QLineEdit* mInput;
	QPushButton* mButton;
	QComboBox* mFilter;
	QTreeWidget* mTreeWidget;
	QList<QTreeWidgetItem *> mItems;
	QList<QTreeWidgetItem *> mFilteredItems;
	QString mPath;
	QString mFile;
	int mMode;
};

#endif // SEARCH_H