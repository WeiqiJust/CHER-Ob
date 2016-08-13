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

   This file defines the XML reader for CHEROb bookmark files.

*****************************************************************************/

#ifndef BOOKMARKXMLREADER_H
#define BOOKMARKXMLREADER_H

#include "bookmarkWidget.h"

class BookmarkXMLReader
{
public:
  BookmarkXMLReader(BookmarkTreeWidget* w, QString filename);
  ~BookmarkXMLReader();

  /**
	* @brief  Reads entire XML file and displays it within our BookmarkTreeWidget.
	*/
  void readXML();

private:
  QTreeWidgetItem* createChildItem(QTreeWidgetItem* item);
  void readFolder(QTreeWidgetItem* item);
  void readBookmark(QTreeWidgetItem* item);

  BookmarkTreeWidget* btw;
  QString fn;
  QXmlStreamReader xml;
};

#endif // BOOKMARKXMLREADER_H
