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

#include <QFileIconProvider>
#include <QMenu>

#include "navigation.h"
#include "../information/informationWidget.h"
#include "../mainWindow.h"

Navigation::Navigation()
{
	mTreeWidget = new QTreeWidget();
	QStringList ColumnNames;
	ColumnNames << "Navigation";
	mTreeWidget->setHeaderLabels(ColumnNames);
	mTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(mw()->mInformation, SIGNAL(addNavigationItem(const QString, const NoteMode, const NoteType)),
		this, SLOT(addNoteItem(const QString, const NoteMode, const NoteType)));
	connect(mw()->mInformation, SIGNAL(removeNavigationItem(const QString, const NoteMode, const int, const NoteType)),
		this, SLOT(removeNoteItem(const QString, const NoteMode, const int, const NoteType)));
	connect(mw()->mBookmark, SIGNAL(addNavigationItem(const QString, const QString, const QString)),
		this, SLOT(addBookmarkItem(const QString, const QString, const QString)));
	connect(mw()->mBookmark, SIGNAL(removeNavigationItem(const QString, const QString, const QString)),
		this, SLOT(removeBookmarkItem(const QString, const QString, const QString)));
	connect(mw()->mBookmark, SIGNAL(editNavigationItem(const QString, const QString, const QString)),
		this, SLOT(editBookmarkItem(const QString, const QString, const QString)));
	connect(mTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(showTreeWidgetItem(QTreeWidgetItem*, int)));
	connect(mTreeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
	mVBox = new QVBoxLayout();
	mVBox->addWidget(mTreeWidget);
	HtmlDelegate* delegation = new HtmlDelegate();
    mTreeWidget->setItemDelegate(delegation);
	this->setLayout(mVBox);
	this->show();
}

void Navigation::init(const QString name, bool isProject)
{
	//qDebug()<<"in init";
	clear();

	mName = name;
	QTreeWidgetItem *itm = new QTreeWidgetItem();
	isCurrentProject = isProject;
	if (isProject)
	{
		itm->setText(0, QString("Project: ").append(name));
		itm->setIcon(0, QIcon(":/icons/CHEROb_Logo128.png"));
	}
	else
	{
		QFileIconProvider iconProvider;
		itm->setText(0, QString("Cultural Heritage Entity: ").append(name));
		itm->setIcon(0, iconProvider.icon(QFileIconProvider::Drive));
	}
	
	mItems.push_back(itm);
	mTreeWidget->insertTopLevelItems(0, mItems);
}

void Navigation::clear()
{
	mItems.clear();
	mTreeWidget->clear();
	mName = QString();
}

void Navigation::addObject(const QString path, const QString CHE)
{
	//qDebug()<<"add object"<<path<<CHE;
	QString localPath = QDir::toNativeSeparators(path);
	QStringList nameElements = localPath.split(QDir::separator());
	QString objectName = nameElements[nameElements.size() - 1];
	QTreeWidgetItem *object = new QTreeWidgetItem();
	object->setText(0, objectName);
	QFileIconProvider iconProvider;
	object->setIcon(0, iconProvider.icon(QFileIconProvider::Folder));

	if (CHE != QString() && isCurrentProject)
	{
		bool isFound = false;
		int i = 0;
		for (; i < mItems[0]->childCount(); i++)
		{
			if (mItems[0]->child(i)->text(0) == CHE)
			{
				isFound = true;
				break;
			}
		}
		if (isFound)
		{
			mItems[0]->child(i)->addChild(object);
		}
		else
		{
			QTreeWidgetItem *cheItem = new QTreeWidgetItem();
			cheItem->setText(0, QString("Cultural Heritage Entity: ").append(CHE));
			cheItem->setIcon(0, iconProvider.icon(QFileIconProvider::Drive));
			mItems[0]->addChild(cheItem);
			cheItem->addChild(object);
		}
	}
	else
	{
		mItems[0]->addChild(object);
	}
	loadObjectNote(object, localPath);
	loadObjectBookMark(object, localPath);
	mTreeWidget->expandAll();
}

void Navigation::loadObjectNote(QTreeWidgetItem *object, const QString path)
{
	QFileIconProvider iconProvider;
	QTreeWidgetItem *noteLabel = new QTreeWidgetItem();
	noteLabel->setText(0, QString("Note"));
	noteLabel->setIcon(0, iconProvider.icon(QFileIconProvider::Folder));
	object->addChild(noteLabel);
	QVector<int> noteCount = mw()->mInformation->getNoteNumber(path);	// no notes are marked as removed
	if (noteCount.size() == 6)
	{
		for (int i = 0; i < noteCount.size(); i++)
		{
			if (noteCount[i] == 0)
				continue;
			QString label;
			switch(i)
			{
				case 0: label = QString("Annotation"); break;
				case 1: label = QString("PointNote_"); break;
				case 2: label = QString("SurfaceNote_"); break;
				case 3: label = QString("FrusumNote_"); break;
				case 4: label = QString("PointNote2D_"); break;
				case 5: label = QString("SurfaceNote2D_"); break;
			}
			if (i == 0)
			{
				// note item columns: note name, object path, valid (1 is valid, 0 is removed)
				QTreeWidgetItem *note = new QTreeWidgetItem();	
				QString text = label;
				note->setText(0, text);
				note->setText(1, path);
				note->setText(2, QString::number(1));
				note->setIcon(0, iconProvider.icon(QFileIconProvider::File));
				noteLabel->addChild(note);
			}
			else 
			{
				for (int j = 0; j < noteCount[i]; j++)
				{
					QTreeWidgetItem *note = new QTreeWidgetItem();
					QString text = label;
					note->setText(0, text.append(QString::number(j+1)));
					note->setText(1, path);
					note->setText(2, QString::number(1));
					note->setIcon(0, iconProvider.icon(QFileIconProvider::File));
					noteLabel->addChild(note);
				}
			}
		}
	}
}

void Navigation::loadObjectBookMark(QTreeWidgetItem *object, const QString path)
{
	QTreeWidgetItem *bookmark = new QTreeWidgetItem();
	bookmark->setText(0, QString("BookMark"));
	QFileIconProvider iconProvider;
	bookmark->setIcon(0, iconProvider.icon(QFileIconProvider::Folder));
	object->addChild(bookmark);

	QString bookmarkPath = path;
	bookmarkPath.append(QDir::separator() + QString("BookMark"));
	QString fn = bookmarkPath;
	fn.append(QDir::separator() + QString("bookmarks.xml"));
	QFile f(fn);
    if(!f.exists()) return;
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text)) 
	{
		f.close();
		return;
	}
	QXmlStreamReader xml;
    xml.setDevice(&f);
	
    if(xml.readNextStartElement()) 
	{
        if(xml.name() != BOOKMARK_XML_ROOT) 
            return;
    } else return;

    Q_ASSERT(xml.isStartElement());

	
    while (xml.readNextStartElement()) 
	{
        if (xml.name() == "bookmark")    
		{
			Q_ASSERT(xml.isStartElement() && xml.name() == "bookmark");

			if (xml.attributes().value("isHidden").toString() != QString("True"))	// this bookmark is temporarily removed.
			{
				QTreeWidgetItem *bookmarkItem = new QTreeWidgetItem();
				bookmarkItem->setText(0, xml.attributes().value("title").toString());
				bookmarkItem->setText(1, xml.attributes().value("uuid").toString());
				bookmarkItem->setText(2, path);
				bookmarkItem->setText(3, QString::number(1));
				bookmarkItem->setIcon(0, iconProvider.icon(QFileIconProvider::File));
				bookmark->addChild(bookmarkItem);
			}
			while (xml.readNextStartElement()) 
			{
				xml.skipCurrentElement();
			}
		}
        else
            xml.skipCurrentElement();
    }
	f.close();
}

void Navigation::removeObject(const QString path)
{
	
	QStringList elements = path.split(QDir::separator());
	QString fileName = elements[elements.size()-1];
	bool isFound = false;
	QTreeWidgetItem* object;
	isFound = findObjectItem(fileName, object);

	if (!isFound)
		return;
	QTreeWidgetItem* parent = object->parent();
	parent->removeChild(object);
	qDebug()<<"parent"<<parent->text(0);
	if (parent->text(0).split(": ")[0] == QString("Cultural Heritage Entity") && parent->childCount() == 0)
	{
		QTreeWidgetItem* project = parent->parent();
		if (project)
			project->removeChild(parent);
	}
}

bool Navigation::findObjectItem(const QString fileName, QTreeWidgetItem* &object)
{
	bool isFound = false;
	for (int i = 0; i < mItems[0]->childCount(); i++)
	{
		QString text = mItems[0]->child(i)->text(0);
		if (text.split(": ")[0] == QString("Cultural Heritage Entity"))
		{
			for (int j = 0; j < mItems[0]->child(i)->childCount(); j++)
			{
				if (mItems[0]->child(i)->child(j)->text(0) == fileName)
				{
					object = mItems[0]->child(i)->child(j);
					isFound = true;
					break;
				}
			}
		}
		else
		{
			if (text == fileName)
			{
				object = mItems[0]->child(i);
				isFound = true;
			}
		}
		if (isFound)
			return true;
	}
	return false;
}

void Navigation::showTreeWidgetItem(QTreeWidgetItem* item, int column)
{
	if (item->columnCount() == 3)	// check the item is note
		mw()->mInformation->openNoteFromNavigation(item);
	else if (item->columnCount() == 4)	// check the item is bookmark
		mw()->mBookmark->viewBookmark(item, item->text(2));
}

void Navigation::addNoteItem(const QString path, const NoteMode type, const NoteType dim)
{
	// path shoule be in the form of path/object/Note
	QStringList elements = path.split(QDir::separator());
	QString fileName = elements[elements.size()-2];
	QString objectPath = path;
	objectPath.truncate(objectPath.lastIndexOf(QDir::separator()));
	bool isFound = false;
	QTreeWidgetItem* object;

	isFound = findObjectItem(fileName, object);

	if (isFound)
	{
		object = object->child(0);
		int point = 0, surface = 0, frustum = 0, point2D = 0, surface2D = 0;
		for (int k = 0; k < object->childCount(); k++)
		{
			QString text = object->child(k)->text(0).split("_")[0];
			if (text == QString("PointNote") || text == QString("<font color=\"gray\">PointNote"))
				point++;
			else if (text == QString("SurfaceNote") || text == QString("<font color=\"gray\">SurfaceNote"))
				surface++;
			else if (text == QString("FrustumNote") || text == QString("<font color=\"gray\">FrustumNote"))
				frustum++;
			else if (text == QString("PointNote2D") || text == QString("<font color=\"gray\">PointNote2D"))
				point2D++;
			else if (text == QString("SurfaceNote2D") || text == QString("<font color=\"gray\">SurfaceNote2D"))
				surface2D++;
		}
		QFileIconProvider iconProvider;
		QTreeWidgetItem* newNote = new QTreeWidgetItem();
		newNote->setIcon(0, iconProvider.icon(QFileIconProvider::File));
		if (type == ANNOTATION)
		{
			newNote->setText(0, QString("Annotation"));
			if (object->childCount() == 0)
				object->addChild(newNote);
			else if (object->child(0)->text(0) == QString("<font color=\"gray\">Annotation</font>"))
			{
				object->child(0)->setText(0, QString("Annotation"));
				object->child(0)->setText(2, QString::number(1));
			}
			else if (object->child(0)->text(0) != QString("Annotation"))
				object->insertChild(0, newNote);

		}
		if (type ==	POINTNOTE)
		{
			if (dim == NOTE3D)
			{
				newNote->setText(0, QString("PointNote_").append(QString::number(point+1)));
				if (object->childCount() == 0)
					object->addChild(newNote);
				else if (object->child(0)->text(0) == QString("Annotation"))
					object->insertChild(point+1, newNote);
				else
					object->insertChild(point, newNote);
			}
			else if (dim == NOTE2D)
			{
				newNote->setText(0, QString("PointNote2D_").append(QString::number(point2D+1)));
				if (object->childCount() == 0)
					object->addChild(newNote);
				else if (object->child(0)->text(0) == QString("Annotation"))
					object->insertChild(point+surface+frustum+point2D+1, newNote);
				else
					object->insertChild(point+surface+frustum+point2D, newNote);
			}
		}
		else if (type == SURFACENOTE)
		{
			if (dim == NOTE3D)
			{
				newNote->setText(0, QString("SurfaceNote_").append(QString::number(surface+1)));
				if (object->childCount() == 0)
					object->addChild(newNote);
				else if (object->child(0)->text(0) == QString("Annotation"))
					object->insertChild(point+surface+1, newNote);
				else
					object->insertChild(point+surface, newNote);
			}
			else if (dim == NOTE2D)
			{
				newNote->setText(0, QString("SurfaceNote2D_").append(QString::number(surface2D+1)));
				if (object->childCount() == 0)
					object->addChild(newNote);
				else if (object->child(0)->text(0) == QString("Annotation"))
					object->insertChild(point+surface+frustum+point2D+surface2D+1, newNote);
				else
					object->insertChild(point+surface+frustum+point2D+surface2D, newNote);
			}
		}
		else if (type == FRUSTUMNOTE)
		{
			newNote->setText(0, QString("FrustumNote_").append(QString::number(frustum+1)));
			if (object->childCount() == 0)
				object->addChild(newNote);
			else if (object->child(0)->text(0) == QString("Annotation"))
				object->insertChild(point+surface+frustum+1, newNote);
			else
				object->insertChild(point+surface+frustum, newNote);
		}
		newNote->setText(1, objectPath);
		newNote->setText(2, QString::number(1));
	}
}

void Navigation::removeNoteItem(const QString path, const NoteMode type, const int id, const NoteType dim)
{
	// path shoule be in the form of path/object/Note
	QStringList elements = path.split(QDir::separator());
	QString fileName = elements[elements.size()-2];
	QString objectPath = path;
	objectPath.truncate(objectPath.lastIndexOf(QDir::separator()));
	bool isFound = false;
	QTreeWidgetItem* object;
	isFound = findObjectItem(fileName, object);
	qDebug()<<"remove note item"<<object->text(0)<<path;
	
	if (isFound)
	{
		object = object->child(0);
		int point = 0, surface = 0, frustum = 0, point2D = 0, surface2D = 0;
		for (int k = 0; k < object->childCount(); k++)
		{
			QString text = object->child(k)->text(0).split("_")[0];
			if (text == QString("PointNote") || text == QString("<font color=\"gray\">PointNote"))
				point++;
			else if (text == QString("SurfaceNote") || text == QString("<font color=\"gray\">SurfaceNote"))
				surface++;
			else if (text == QString("FrustumNote") || text == QString("<font color=\"gray\">FrustumNote"))
				frustum++;
			if (text == QString("PointNote2D") || text == QString("<font color=\"gray\">PointNote2D"))
				point2D++;
			else if (text == QString("SurfaceNote2D") || text == QString("<font color=\"gray\">SurfaceNote2D"))
				surface2D++;
		}
		if (type == ANNOTATION)
		{
			if (object->childCount() != 0 && object->child(0)->text(0) == QString("Annotation"))
			{
				//object->removeChild(object->child(0));
				QString text = object->child(0)->text(0);
				text.append(QString("</font>"));
				text.prepend(QString("<font color=\"gray\">"));
				object->child(0)->setText(0, text);
				object->child(0)->setText(2, QString::number(0));
			}
		}
		if (type == POINTNOTE)
		{
			int itemNum;
			if (dim == NOTE3D)
				itemNum = id;
			else if (dim == NOTE2D)
				itemNum = id + point + surface + frustum;
			if (object->childCount() != 0)
			{
				if (object->child(0)->text(0) == QString("Annotation"))
				{
					QString text = object->child(itemNum+1)->text(0);
					text.append(QString("</font>"));
					text.prepend(QString("<font color=\"gray\">"));
					object->child(itemNum+1)->setText(0, text);
					object->child(itemNum+1)->setText(2, QString::number(0));
				}
				else
				{
					QString text = object->child(itemNum)->text(0);
					text.append(QString("</font>"));
					text.prepend(QString("<font color=\"gray\">"));
					object->child(itemNum)->setText(0, text);
					object->child(itemNum)->setText(2, QString::number(0));
				}
			}
		}
		else if (type == SURFACENOTE)
		{
			int itemNum;
			if (dim == NOTE3D)
				itemNum = id + point;
			else if (dim == NOTE2D)
				itemNum = id + point + surface + frustum + point2D;
			if (object->childCount() != 0)
			{
				if (object->child(0)->text(0) == QString("Annotation"))
				{
					QString text = object->child(itemNum+1)->text(0);
					text.append(QString("</font>"));
					text.prepend(QString("<font color=\"gray\">"));
					object->child(itemNum+1)->setText(0, text);
					object->child(itemNum+1)->setText(2, QString::number(0));
				}
				else
				{
					QString text = object->child(itemNum)->text(0);
					text.append(QString("</font>"));
					text.prepend(QString("<font color=\"gray\">"));
					object->child(itemNum)->setText(0, text);
					object->child(itemNum)->setText(2, QString::number(0));
				}
			}
		}
		else if (type == FRUSTUMNOTE)
		{
			if (object->childCount() != 0)
			{
				if (object->child(0)->text(0) == QString("Annotation"))
				{
					QString text = object->child(id+point+surface+1)->text(0);
					text.append(QString("</font>"));
					text.prepend(QString("<font color=\"gray\">"));
					object->child(id+point+surface+1)->setText(0, text);
					object->child(id+point+surface+1)->setText(2, QString::number(0));
				}
				else
				{
					QString text = object->child(id+point+surface)->text(0);
					text.append(QString("</font>"));
					text.prepend(QString("<font color=\"gray\">"));
					object->child(id+point+surface)->setText(0, text);
					object->child(id+point+surface)->setText(2, QString::number(0));
				}
			}
		}

	}
}

void Navigation::addBookmarkItem(const QString path, const QString bookmarkName, const QString uuid)
{
	// path shoule be in the form of path/object/BookMark
	QStringList elements = path.split(QDir::separator());
	QString fileName = elements[elements.size()-2];
	QString objectPath = path;
	objectPath.truncate(objectPath.lastIndexOf(QDir::separator()));
	bool isFound = false;
	QTreeWidgetItem* object;

	isFound = findObjectItem(fileName, object);

	if (isFound)
	{
		object = object->child(1);
		QFileIconProvider iconProvider;
		QTreeWidgetItem* newBookmark = new QTreeWidgetItem();
		newBookmark->setIcon(0, iconProvider.icon(QFileIconProvider::File));
		newBookmark->setText(0, bookmarkName);
		object->addChild(newBookmark);
		newBookmark->setText(1, uuid);
		newBookmark->setText(2, objectPath);
		newBookmark->setText(3, QString::number(1));	// bool value: 1 represent it is not removed
	}
}

void Navigation::removeBookmarkItem(const QString path, const QString bookmarkName, const QString uuid)
{
	// path shoule be in the form of path/object/BookMark
	QStringList elements = path.split(QDir::separator());
	QString fileName = elements[elements.size()-2];
	QString objectPath = path;
	objectPath.truncate(objectPath.lastIndexOf(QDir::separator()));
	bool isFound = false;
	QTreeWidgetItem* object;
	isFound = findObjectItem(fileName, object);
	//qDebug()<<"remove bookmakr item"<<object->text(0)<<path;
	
	if (isFound)
	{
		object = object->child(1);
		for (int i = 0; i < object->childCount(); i++)
		{
			//qDebug()<<"Delete item"<<object->child(i)->text(1)<<uuid;
			if (object->child(i)->text(0) == bookmarkName && object->child(i)->text(1) == uuid)
			{
				QString text = object->child(i)->text(0);
				text.append(QString("</font>"));
				text.prepend(QString("<font color=\"gray\">"));
				object->child(i)->setText(0, text);
				object->child(i)->setText(3, QString::number(0));
			}
		}	
	}
}

void Navigation::editBookmarkItem(const QString path, const QString newBookmarkName, const QString uuid)
{
	// path shoule be in the form of path/object/BookMark
	QStringList elements = path.split(QDir::separator());
	QString fileName = elements[elements.size()-2];
	QString objectPath = path;
	objectPath.truncate(objectPath.lastIndexOf(QDir::separator()));
	bool isFound = false;
	QTreeWidgetItem* object;
	isFound = findObjectItem(fileName, object);

	if (isFound)
	{
		object = object->child(1);
		for (int i = 0; i < object->childCount(); i++)
		{
			if (object->child(i)->text(1) == uuid)
			{
				object->child(i)->setText(0, newBookmarkName);
			}
		}	
	}
}

void Navigation::showContextMenu(const QPoint& pos)
{
	QMenu menu;

	// only show menu if there is an item being visualized.
	if(!mw()->VTKA()) return;

	QPoint globalPos = mTreeWidget->mapToGlobal(pos);
	QTreeWidgetItem* item = mTreeWidget->itemAt(pos);
	if(item && ((item->columnCount() == 3 && item->text(2).toInt() == 0) // if item is a removed note
		|| (item->columnCount() == 4 && item->text(3).toInt() == 0)))	 // if item is a removed bookmark
		menu.addAction("Undo Remove");
	else
		return;

	QAction* act = menu.exec(globalPos);
	if (act)
	{
		if (item->columnCount() == 3)
			mw()->mInformation->undoRemoveNote(item);
		else
			mw()->mBookmark->undoRemoveBookmark(item);
	}
}

MainWindow* Navigation::mw()
{
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
    if (mainwindow)
    {
      return mainwindow;
    }
  }
  return NULL;
}
