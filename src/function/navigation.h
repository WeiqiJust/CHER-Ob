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
#include "htmlDelegate.h"
#include "../vtkEnums.h" 

class MainWindow;

class Navigation: public QWidget
{
	Q_OBJECT

public:
	Navigation();

	void init(const QString name, bool isProject);

	void clear();

	// path is the absolute path of object data file, CHE is the belonging CHE name
	void addObject(const QString path, const QString CHE = QString());

	void loadObjectNote(QTreeWidgetItem *object, const QString path);

	void loadObjectBookMark(QTreeWidgetItem *object, const QString path);

	void removeObject(const QString path);

private:
	static MainWindow* mw();

private slots:
	void showTreeWidgetItem(QTreeWidgetItem* item, int column);

	void addNoteItem(const QString path, const NoteMode type);

	void removeNoteItem(const QString path, const NoteMode type, const int id);

	void showContextMenu(const QPoint& pos);

private:
	QVBoxLayout* mVBox;
	QString mName;
	QTreeWidget* mTreeWidget;
	QList<QTreeWidgetItem *> mItems;
	bool isCurrentProject;

};

#endif