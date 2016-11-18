/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Weiqi Shi (weiqi.shi@yale.edu)
			 Zeyu Wang (zeyu.wang@yale.edu)
			 Ying Yang (ying.yang.yy368@yale.edu)

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

#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <QtScript>
#include <QDir>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QToolBar>
#include <QTextBrowser>
#include <QLabel>
#include <QFile>
#include <QFileDialog>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPoint>
#include <QXmlStreamReader>
#include "htmlDelegate.h"
#include "../vtkEnums.h" 

class MainWindow;

/**
 * This class provides the navigation tab. The project/CHE is the parent item.
 * For a project, if an object has its original CHE path with it, which means
 * it was imported from a CHE, the object item is under the its original CHE item,
 * representing the import relationship. 
 * For CHE, each object item is directly under the CHE parent item.
 * For each object, It has child items including note folder and bookmark folder items.
 * The specific file items are saved under the folder items respectively.
 */

class Navigation: public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 */
	Navigation();

	/**
	 * @brief  Tab initiation when a new project or CHE is opened.
	 * @param  name       Project name.
	 * @param  isProject  Whether the current opened is project or CHE.
	 */
	void init(const QString name, bool isProject);

	/**
	 * @brief  Remove all the treewidge items and clear the name.
	 */
	void clear();

	/**
	 * @brief  When an object is added, the navigation items is added correspondently.
	 * @param  path is the absolute path of object data file, CHE is the belonging CHE name.
	 */
	void addObject(const QString path, const QString CHE = QString());

	/**
	 * @brief  Add the note item of specific object.
	 * @param  object  The object that should be loaded.
	 * @param  path    The note path.
	 */
	void loadObjectNote(QTreeWidgetItem *object, const QString path);

	/**
	 * @brief  Add the note item of specific object.
	 * @param  object  The object that should be loaded.
	 * @param  path    The note path.
	 */
	void loadObjectBookMark(QTreeWidgetItem *object, const QString path);

	/**
	 * @brief  Remove all the items under the object
	 * @param  path  The full path of the object.
	 */
	void removeObject(const QString path);

private:
	/**
	 * @brief  Find the main frame.
	 */
	static MainWindow* mw();

	/**
	 * @brief  Find if  certain object item exists.
	 * @return True if exists.
	 */
	bool findObjectItem(const QString fileName, QTreeWidgetItem* &object);

	/**
	 * Added by Ying to copy annotations
	 */
	QString getPath(QTreeWidgetItem* item);
	QString getImageFileNameInNote(QString fileName);

private slots:
	/**
	 * @brief  If the item is note item, then open the note.
	 * @param  item  The item that is double clicked.
	 * @param  column  checked column number to match the signal, useless here.
	 */
	void showTreeWidgetItem(QTreeWidgetItem* item, int column);

	/**
	 * @brief  Add a note item when a note is created.
	 * @param  path  The full path of the note folder. It shoule be in the form of path/object/Note.
	 * @param  type  Note type.
	 * @param  dim   Whether the note belongs to 2D or 3D object. NONE is for annotation.
	 */
	void addNoteItem(const QString path, const NoteMode type, const NoteType dim = NONE);

	/**
	 * @brief  Remove the note item when a note is removed.
	 * @param  path  The full path of the note folder. It shoule be in the form of path/object/Note.
	 * @param  type  Note type.
	 * @param  id    Note id.
	 * @param  dim   Whether the note belongs to 2D or 3D object. NONE is for annotation.
	 */
	void removeNoteItem(const QString path, const NoteMode type, const int id, const NoteType dim = NONE);

	/**
	 * @brief  Add a bookmark item when a it is created.
	 * @param  path          The full path of the bookmakr folder. It shoule be in the form of path/object/bookmark.
	 * @param  bookmarkName  The name of the bookmark.
	 * @param  uuid          The id of the bookmark.
	 */
	void addBookmarkItem(const QString path, const QString bookmarkName, const QString uuid);

	/**
	 * @brief  Remove a bookmark item when a it is deleted.
	 * @param  path          The full path of the bookmakr folder. It shoule be in the form of path/object/bookmark.
	 * @param  bookmarkName  The name of the bookmark.
	 * @param  uuid          The id of the bookmark.
	 */
	void removeBookmarkItem(const QString path, const QString bookmarkName, const QString uuid);

	/**
	 * @brief  Edit a bookmark item when a it is modified.
	 * @param  path             The full path of the bookmakr folder. It shoule be in the form of path/object/bookmark.
	 * @param  newBookmarkName  The new name of the bookmark.
	 * @param  uuid             The id of the bookmark.
	 */
	void editBookmarkItem(const QString path, const QString newBookmarkName, const QString uuid);

	/**
	 * @brief  Show the tab.
	 * @param  pos  The point that is clicked by mouse.
	 */
	void showContextMenu(const QPoint& pos);

	/**
	 * Added by Ying to copy annotations
	 */
	void menuSelection(QAction* action);

private:
	QVBoxLayout* mVBox;
	QString mName;
	QTreeWidget* mTreeWidget;
	QList<QTreeWidgetItem *> mItems;
	bool isCurrentProject;

	// Added by Ying to copy annotations
	QString actionStr;
	QString srcStr;
	QString srcPath;
	QString destStr;
	QVector<QString> noteModeType;
};

#endif