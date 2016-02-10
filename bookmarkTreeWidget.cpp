/****************************************************************************

 - Codename: Hyper3D (Yale Computer Graphics Group)

 - Writers:  David Tidmarsh (tidmarsh@aya.yale.edu)

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

   This file implements the bookmark tree widget that displays the user's current
   bookmarks.

*****************************************************************************/


#include <QApplication>
#include <QDomDocument>
#include <QDropEvent>
#include <QEvent>
#include <QFile>
#include <QFileInfo>
#include <QHeaderView>
#include <QMessageBox>
#include <QMouseEvent>

#include "bookmarkTreeWidget.h"
#include "bookmarkWidget.h"
#include "mainWindow.h"


BookmarkTreeWidget::BookmarkTreeWidget(QWidget* parent)
{
    this->header()->close();
    this->setAcceptDrops(true);
    this->viewport()->setAcceptDrops(true);
    this->setDragEnabled(true);
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setDropIndicatorShown(true);
    this->setParent(parent);

    dragCaption.clear();
    dragParent = NULL;
    uuid.clear();
}

void BookmarkTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    QList<QTreeWidgetItem*> items = BookmarkWidget::mw()->mBookmark->bTreeWidget->selectedItems();
    if(items.size() == 0) return;
    QTreeWidgetItem* item = items.at(0);
    if(!item) return;

    if(Qt::ShiftModifier == QApplication::keyboardModifiers()) {
        dragCaption = item->text(0);
        dragParent = item->parent();
        if(dragParent) {
            uuid = dragParent->text(UUID_COLUMN);
        }
        QTreeWidget::dragEnterEvent(event);
    } else {
        dragCaption.clear();
        dragParent = NULL;
        uuid.clear();
        event->accept();
    }
}

void BookmarkTreeWidget::mousePressEvent(QMouseEvent *event)
{
    QTreeWidgetItem* item = itemAt(event->pos());
    if(!item) {
        this->clearSelection();
    }
    QTreeWidget::mousePressEvent(event);
}

// DT: reimplementing QTreeWidget::dropEvent so that we can update the QDomDocument.
void BookmarkTreeWidget::dropEvent(QDropEvent* event)
{
    if(!event) {dragCaption.clear(); return;}

    // DT: user must hold down shift key to move item.
    if (event->keyboardModifiers() != Qt::ShiftModifier) {
        event->ignore();
        if(BookmarkWidget::mw()->mBookmark)
            BookmarkWidget::mw()->mBookmark->refreshBookmarkList();
        dragCaption.clear();
        return;
    }

    QString text = dragCaption;
    dragCaption.clear();
    if(text.isEmpty()) {
        BookmarkWidget::mw()->mBookmark->refreshBookmarkList();
        return;
    }
    QList<QTreeWidgetItem*> list = this->findItems(text, Qt::MatchExactly | Qt::MatchRecursive, 0);
    if(list.isEmpty()) {
        BookmarkWidget::mw()->mBookmark->refreshBookmarkList();
        return;
    }
    QTreeWidgetItem* item = list.at(0);
    if(!item) {
        BookmarkWidget::mw()->mBookmark->refreshBookmarkList();
        return;
    }

    QTreeWidget::dropEvent(event);

    QString filetype;
    QIcon icon = item->icon(0);
    if(icon.isNull()) {
        filetype = "bookmark";
    } else {
        filetype = "folder";
    }

    QTreeWidgetItem* parent = item->parent();
    QString parentText, parentFiletype;
    if(parent) {
        parentText = parent->text(0);
        QIcon icon = parent->icon(0);
        if(icon.isNull()) {
            parentFiletype = "bookmark";
        } else {
            parentFiletype = "folder";
        }
    }

    QFileInfo finfo(BookmarkWidget::mw()->VTKA()->mFilename);
    QDir* dir = new QDir();
    QString fn = QDir::toNativeSeparators(dir->relativeFilePath(finfo.absoluteFilePath()));
	/*********** Modified by Weiqi Shi 1/19/2016*************/
    QString path = BookmarkWidget::mw()->VTKA()->mProjectPath + QDir::separator() + finfo.fileName().replace(" ", "_")
            + "_" + QString::number(qHash(fn)) + QDir::separator() + BOOKMARK_FN;
	/*********** Modified by Weiqi Shi 1/19/2016*************/
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox mb;
        mb.critical(this, tr("Bookmark Error"),
                    tr("The bookmark file for this object cannot be found."));
        return;
    }

    QDomDocument doc;
    doc.setContent(&file);
    QDomNodeList rootList = doc.elementsByTagName(BOOKMARK_XML_ROOT);
    if(list.isEmpty()) {
        QMessageBox mb;
        mb.critical(this, tr("Bookmark Error"),
                    tr("The bookmark file for this object is improperly structured because it lacks a root element."));
        return;
    }

    QDomNode rootNode = rootList.at(0);
    QDomElement root = rootNode.toElement();
    QDomNodeList bmks = root.elementsByTagName(filetype);
    QDomNode oldNode, newNode;
    QDomElement elt;
    QString value;
    for(int i = 0; i < bmks.length(); i++) {
        elt = bmks.at(i).toElement();
        value = elt.attribute("title");
        if(value == text) {
            oldNode = bmks.at(i);
            break;
        }
        value.clear();
    }
    if(value.isEmpty()) {
        return;
    }

    newNode = oldNode;
    QDomNode oldParent = oldNode.parentNode();

    QDomNode newParent;
    if(parent) {
        bmks = root.elementsByTagName(parentFiletype);
        for(int i = 0; i < bmks.length(); i++) {
            elt = bmks.at(i).toElement();
            value = elt.attribute("title");
            if(value == parentText) {
                newParent = bmks.at(i);
                break;
            }
        }
    }

    oldParent.removeChild(oldNode);
    if(!newParent.isNull()) {
        int index = item->parent()->indexOfChild(item);
        QDomNodeList children = newParent.childNodes();
        int len = children.length();
        if(index == len) {
            newParent.appendChild(newNode);
        } else {
            newParent.insertBefore(newNode, children.at(index));
        }
    } else {
        int index = this->indexOfTopLevelItem(item);
        QDomNodeList children = rootNode.childNodes();
        int len = children.length();
        if(index == len) {
            rootNode.appendChild(newNode);
        } else {
            rootNode.insertBefore(newNode, children.at(index));
        }
    }

    QDomNodeList folders = root.elementsByTagName("folder");
    for(int i = 0; i < folders.length(); i++) {
        elt = folders.at(i).toElement();
        value = elt.attribute("title");
        QList<QTreeWidgetItem*> items = this->findItems(value, Qt::MatchExactly | Qt::MatchRecursive, 0);
        QTreeWidgetItem* item = items.at(0);
        if(!item) continue;
        bool expanded = item->isExpanded();
        elt.setAttribute("folded", expanded ? "no" : "yes");
    }

    QFile f(path);
    if(!f.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox mb;
        mb.critical(this, tr("Bookmark Error"), tr("Failed to save changes."));
        f.close();
        return;
    }
    f.resize(0);
    QTextStream out(&f);
    out << doc.toString();
    f.close();

    if(BookmarkWidget::mw()->mBookmark)
        BookmarkWidget::mw()->mBookmark->refreshBookmarkList();
}
