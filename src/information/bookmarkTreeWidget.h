/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  David Tidmarsh (tidmarsh@aya.yale.edu)
			 Weiqi Shi (weiqi.shi@yale.edu)

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

   This file defines the bookmark tree widget that displays the user's current
   bookmarks.

*****************************************************************************/


#ifndef BOOKMARKTREEWIDGET_H
#define BOOKMARKTREEWIDGET_H

#include <QWidget>
#include <QTreeView>
#include <QTreeWidget>
#include <QTreeWidgetItem>

/**
 * This class provides the bookmarktreewidget function in bookmarkWidget.
 */
class BookmarkTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    BookmarkTreeWidget(QWidget* parent = 0);

    void dropEvent(QDropEvent *event);

public slots:

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void mousePressEvent(QMouseEvent *event);

    QString dragCaption;
    QTreeWidgetItem* dragParent;
    QString uuid;
};


#endif // BOOKMARKTREEWIDGET_H
