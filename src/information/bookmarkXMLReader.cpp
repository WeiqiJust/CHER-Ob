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

   This file defines the XML reader for Hyper3D bookmark files.

*****************************************************************************/

// DT: Much of this code is based on the QXmlStream bookmarks example at
// http://doc-snapshot.qt-project.org/qt5-stable/qtxml/streambookmarks.html
// Software license reproduced below:
/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
#include <QFile>
#include <QMessageBox>

#include "bookmarkXMLReader.h"
#include "../mainWindow.h"

BookmarkXMLReader::BookmarkXMLReader(BookmarkTreeWidget* w, QString filename)
{
    btw = w;
    fn = filename;
}

BookmarkXMLReader::~BookmarkXMLReader()
{

}

QTreeWidgetItem* BookmarkXMLReader::createChildItem(QTreeWidgetItem *item)
{
    QTreeWidgetItem *childItem;
    if (item) {
        childItem = new QTreeWidgetItem(item);
    } else {
        childItem = new QTreeWidgetItem(btw);
    }
    return childItem;
}

void BookmarkXMLReader::readFolder(QTreeWidgetItem *item)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "folder");

    QTreeWidgetItem* folder = createChildItem(item);
    folder->setData(TITLE_COLUMN, Qt::UserRole, FOLDER);
    QFileIconProvider provider;
    folder->setIcon(0, provider.icon(QFileIconProvider::Folder));
    folder->setText(TITLE_COLUMN, xml.attributes().value("title").toString());
    folder->setText(UUID_COLUMN, xml.attributes().value("uuid").toString());
    folder->setText(DATE_CREATED_COLUMN, xml.attributes().value("created").toString());
    folder->setText(DATE_MODIFIED_COLUMN, xml.attributes().value("modified").toString());
    bool folded = (xml.attributes().value("folded") != "no");
    btw->setItemExpanded(folder, !folded);

    while (xml.readNextStartElement()) {
        if (xml.name() == "folder")
            readFolder(folder);
        else if (xml.name() == "bookmark")
            readBookmark(folder);
        else
            xml.skipCurrentElement();
    }
}

/*
 * Reads a single bookmark and creates a QTreeWidgetItem to represent it.
 */
void BookmarkXMLReader::readBookmark(QTreeWidgetItem *item)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "bookmark");

    QTreeWidgetItem *bookmark = createChildItem(item);
    bookmark->setData(TITLE_COLUMN, Qt::UserRole, BOOKMARK);
    bookmark->setFlags(bookmark->flags() ^ (Qt::ItemIsDropEnabled));
    bookmark->setText(TITLE_COLUMN, xml.attributes().value("title").toString());
    bookmark->setText(UUID_COLUMN, xml.attributes().value("uuid").toString());
    bookmark->setText(DATE_CREATED_COLUMN, xml.attributes().value("created").toString());
    bookmark->setText(DATE_MODIFIED_COLUMN, xml.attributes().value("modified").toString());

    while (xml.readNextStartElement()) {
        xml.skipCurrentElement();
    }
}

/*
 * Reads entire XML file and displays it within our BookmarkTreeWidget.
 */
void BookmarkXMLReader::readXML()
{
    QFile f(fn);
    if(!f.exists()) return;
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    xml.setDevice(&f);
    if(xml.readNextStartElement()) {
        if(xml.name() != BOOKMARK_XML_ROOT) {
            QMessageBox mb;
            mb.critical(btw, QObject::tr("Bookmark Error"),
                        QObject::tr("Invalid Hyper3D bookmark file."));
            return;
        }
    } else return;

    Q_ASSERT(xml.isStartElement());

    while (xml.readNextStartElement()) {
        if (xml.name() == "folder")
            readFolder(0);
        else if (xml.name() == "bookmark")
            readBookmark(0);
        else
            xml.skipCurrentElement();
    }
}
