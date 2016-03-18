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

   This file defines the bookmark widget that allows the user to add, edit,
   delete, and visualize bookmarks.

*****************************************************************************/

#ifndef BOOKMARKWIDGET_H
#define BOOKMARKWIDGET_H

#include <QApplication>
#include <QDomDocument>
#include <QDomElement>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QXmlStreamReader>

#include "vtkAssembly.h"
#include "vtkCamera.h"
#include "vtkSmartPointer.h"

#include "bookmarkTreeWidget.h"
#include "../vtkEnums.h"

// Enum to keep track of whether a QTreeWidgetItem is a folder or bookmark.
enum BTWItemType{FOLDER=1,BOOKMARK};

// These constants are used for storing information in the BookmarkTreeWidget.
static const int TITLE_COLUMN = 0;
static const int UUID_COLUMN = 1;
static const int DATE_CREATED_COLUMN = 2;
static const int DATE_MODIFIED_COLUMN = 3;

static const int STRING_MAX = 30;

// Character string representing filename of all valid Hyper3D bookmark files.
static const QString BOOKMARK_FN = "bookmarks.xml";
// Character string representing root of all valid Hyper3D bookmark files.
static const QString BOOKMARK_XML_ROOT = "hyper3d.bookmarks";

static const QString TITLE = "title";
static const QString UUID_NAME = "uuid";
static const QString DATE_CREATED = "created";
static const QString DATE_MODIFIED = "modified";
static const QString DATE_ACCESSED = "accessed";
static const QString FOLDER_NAME = "folder";
static const QString BOOKMARK_NAME = "bookmark";
static const QString FOLDED = "folded";
static const QString FILENAME = "filename";

// Enum for storing data about QActions from the right-click popup context menu.
enum BWContextMenu{ADD_FOLDER=1,ADD_BOOKMARK,EDIT_ITEM,DELETE_ITEM,GET_INFO};

class MainWindow;

class BookmarkWidget : public QWidget
{
  Q_OBJECT

public:
  BookmarkWidget(QWidget *parent = 0);
  ~BookmarkWidget();

  void refreshBookmarkList();
  void refreshCurrentFileInfo();

  static MainWindow* mw();

  BookmarkTreeWidget* bTreeWidget;

public slots:
  void createBookmark();
  bool viewBookmark(QTreeWidgetItem* item);

private slots:
  void createFolder();
  void deleteItem(QTreeWidgetItem* item = 0);
  void editItem(QTreeWidgetItem* item = 0);
  void handleDoubleClick(QTreeWidgetItem* item, int);
  void saveFolderState(QTreeWidgetItem* item);
  void showContextMenu(const QPoint &pos);

private:
  void buildDOMDocument(QDomDocument& doc, QDomElement& root);
  void createBookmarkSubclass(QString caption, QDomDocument& doc, QDomElement& root);
  void displayItemInfo(QTreeWidgetItem* item);
  QDomElement findElementbyUUID(QDomDocument& doc, QString uuid, int type);
  QString getBookmarkFilepath();
  void keyPressEvent(QKeyEvent* event);
  void saveDOMToFile(QDomDocument& doc);
  void setupInfoDialogUI(const QDomElement& elt);
  int verifyDelete(QTreeWidgetItem* item, int type);

  void showFileOpenError();
  void showInvalidFileError();

  /*
   * GUI stuff
   */
  // Main bookmark widget layouts/buttons
  QHBoxLayout* hbox1, *hbox2;
  QVBoxLayout* vbox;
  QPushButton* newFolderButton;
  QPushButton* editButton;
  QPushButton* deleteButton;

  // Popup info dialog layouts/labels
  QDialog* infoDialog;
  QGridLayout* grid;
  QLabel* titleLabel, *uuidLabel, *createdLabel, *createdLabel2, *modifiedLabel, *modifiedLabel2, *accessedLabel, *accessedLabel2;
  QLineEdit* titleLine, *uuidLine;
  /*
   * end GUI
   */

  // Properties of object currently being visualized
  QString bfn; // Filename
  WidgetMode mode; // Filetype
  bool dirLightOn; // Directional light on
  bool interpOn; // Interpolation on
  RenderMode3D renderMode; // 3D render mode
  bool textureOn; // Texture on
  QString tfn; // Texture filename
  int slice; // Current CT slice
  CTOrientation orientation; // Current CT orientation
  vtkSmartPointer<vtkCamera> camera; // Current camera
  CTVolumeRenderMode vRenderMode; // Current volume render mode
  int blend; // Current blend type
  float resolution; // Current resolution
};
#endif // BOOKMARKWIDGET_H
