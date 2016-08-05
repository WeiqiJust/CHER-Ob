/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

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

   This file implements the bookmark widget that allows the user to add, edit,
   delete, and visualize bookmarks.

*****************************************************************************/

#include <QMouseEvent>

#include "bookmark.h"
#include "bookmarkWidget.h"
#include "bookmarkXMLReader.h"
#include "../function/CTControl.h"
#include "../function/lightControl.h"
#include "../function/lightControlRTI.h"
#include "../function/renderingdialog.h"
#include "../mainWindow.h"

BookmarkWidget::BookmarkWidget(QWidget *parent)
    : QWidget(parent)
{
  hbox1 = new QHBoxLayout();
  bTreeWidget = new BookmarkTreeWidget();
  // DT: allow for right-click popup context menu.
  bTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  hbox1->addWidget(bTreeWidget);

  hbox2 = new QHBoxLayout();
  //newFolderButton = new QPushButton("New Folder");
  addButton = new QPushButton("Add");
  editButton = new QPushButton("Edit");
  deleteButton = new QPushButton("Delete");
  //newFolderButton->setEnabled(false);
  addButton->setEnabled(false);
  editButton->setEnabled(false);
  deleteButton->setEnabled(false);

  //hbox2->addWidget(newFolderButton);
  hbox2->addWidget(addButton);
  hbox2->addWidget(editButton);
  hbox2->addWidget(deleteButton);

  connect(bTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(handleDoubleClick(QTreeWidgetItem*,int)));
  connect(bTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(saveFolderState(QTreeWidgetItem*)));
  connect(bTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(saveFolderState(QTreeWidgetItem*)));
  connect(bTreeWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(showContextMenu(const QPoint&)));
  //connect(newFolderButton, SIGNAL(clicked()), this, SLOT(createFolder()));
  connect(addButton, SIGNAL(clicked()), this, SLOT(createBookmark()));
  connect(editButton, SIGNAL(clicked()), this, SLOT(editItem()));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteItem()));

  vbox = new QVBoxLayout();
  vbox->addLayout(hbox1);
  vbox->addLayout(hbox2);

  setLayout(vbox);
  show();

  renderingMode = NULL;
}

BookmarkWidget::~BookmarkWidget()
{
    if(bTreeWidget)
        bTreeWidget->clear();
}

/*
 * Reimplemented from QTreeWidget::keyPressEvent() to allow keyboard
 * commands from the user.
 */
void BookmarkWidget::keyPressEvent(QKeyEvent * event)
{
    QList<QTreeWidgetItem*> list = bTreeWidget->selectedItems();
    if(list.isEmpty()) return;

    foreach(QTreeWidgetItem* twi, list) {
        if(!twi) continue;

        // DT: Pressing the enter/return key will expand/close the current folder
        // or display the current bookmark.
        if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
            int type = twi->data(TITLE_COLUMN, Qt::UserRole).toInt();
            if(type == BOOKMARK) {
                handleDoubleClick(twi,0);
            } else if(type == FOLDER) {
                twi->setExpanded(!twi->isExpanded());
            }
        } else if(event->key() == Qt::Key_Delete) {
            deleteItem();
        }

    }
}

/*
 * Implements the right-click context menu.
 * User can right-click to add a folder/bookmark if no item is selected, or edit/delete
 * the currently selected item.
 */
void BookmarkWidget::showContextMenu(const QPoint &pos)
{
    QString text, s_edit, s_delete;
    QMenu menu;

    // DT: only show menu if there is an item being visualized.
    if(!mw()->VTKA()) return;

    QPoint globalPos = bTreeWidget->mapToGlobal(pos);
    QTreeWidgetItem* item = bTreeWidget->itemAt(pos);
    if(!item) {
        //QAction* addFolderAct = menu.addAction(tr("Add Folder..."));
        //addFolderAct->setData(QVariant(ADD_FOLDER));
        QAction* addBookmarkAct = menu.addAction(tr("Add Bookmark..."));
        addBookmarkAct->setData(QVariant(ADD_BOOKMARK));
    } else { 
        text = item->text(TITLE_COLUMN);
        if(text.length() > STRING_MAX) {
            text.truncate(STRING_MAX);
            s_edit = "Edit \"" + text + "...\"";
            s_delete = "Delete \"" + text + "...\"";
        } else {
            s_edit = "Edit \"" + text + "\"";
            s_delete = "Delete \"" + text + "\"";
        }
        QAction* editItemAct = menu.addAction(s_edit);
        editItemAct->setData(QVariant(EDIT_ITEM));
        QAction* deleteItemAct = menu.addAction(s_delete);
        deleteItemAct->setData(QVariant(DELETE_ITEM));
        menu.addSeparator();
        QAction* getInfoAct = menu.addAction(tr("Get Info..."));
        getInfoAct->setData(QVariant(GET_INFO));
    }

    QAction* act = menu.exec(globalPos);
    if (act)
    {
        int a = act->data().toInt();
        switch(a) {
        //case ADD_FOLDER: { createFolder(); break; }
        case ADD_BOOKMARK: { createBookmark(); break; }
        case EDIT_ITEM: { editItem(item); break; }
        case DELETE_ITEM: { deleteItem(item); break; }
        case GET_INFO: { displayItemInfo(item); break; }
        default: break;
        }
    }
}

QDomElement BookmarkWidget::findElementbyUUID(QDomDocument& doc, QString uuid, int type)
{
    QString path = getBookmarkFilepath();
    QFile file(path);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        showFileOpenError();
        file.close();
        return doc.createElement(QString());
    }

    doc.setContent(&file);
    QDomNodeList list = doc.elementsByTagName(BOOKMARK_XML_ROOT);
    if(list.isEmpty()) {
        showInvalidFileError();
        file.close();
        return doc.createElement(QString());
    }

    QDomElement root = list.at(0).toElement();
    QDomNodeList nodes;
    if(type == BOOKMARK) nodes = root.elementsByTagName(BOOKMARK_NAME);
    else if(type == FOLDER) nodes = root.elementsByTagName(FOLDER_NAME);

    QDomElement elt;
    QString value;
    for(int i = 0; i < nodes.length(); i++) {
        elt = nodes.at(i).toElement();
        value = elt.attribute(UUID_NAME);
        if(value == uuid) break;
        value.clear();
    }

    if(!value.isEmpty()) return elt;
    else return doc.createElement(QString());

}

void BookmarkWidget::saveDOMToFile(QDomDocument& doc)
{
    QString path = getBookmarkFilepath();
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly)) {
        showFileOpenError();
        return;
    }
    QTextStream out(&file);
    out << doc.toString();
    file.close();
}

void BookmarkWidget::displayItemInfo(QTreeWidgetItem* item)
{
    if(!item) {
        QList<QTreeWidgetItem *> selected = bTreeWidget->selectedItems();
        if(selected.isEmpty()) return;
        item = selected.at(0);
        if(!item) return;
    }

    int type = item->data(TITLE_COLUMN, Qt::UserRole).toInt();
    QString uuid = item->text(UUID_COLUMN);

    QDomDocument doc;
    QDomElement elt = findElementbyUUID(doc, uuid, type);

    if(elt.isNull()) return;

    infoDialog = new QDialog();
    setupInfoDialogUI(elt);

    if(type == BOOKMARK)
        infoDialog->setWindowTitle(tr("Bookmark Info"));
    else if(type == FOLDER)
        infoDialog->setWindowTitle(tr("Folder Info"));
    infoDialog->setLayout(grid);
    infoDialog->exec();

    delete infoDialog;
}

void BookmarkWidget::setupInfoDialogUI(const QDomElement& elt)
{
    grid = new QGridLayout();
    titleLabel = new QLabel("Title:");
    uuidLabel = new QLabel("UUID:");
    createdLabel = new QLabel("Created:");
    modifiedLabel = new QLabel("Modified:");
    accessedLabel = new QLabel("Accessed:");

    QString title = elt.attribute(TITLE);
    if(!title.isEmpty()) {
        titleLine = new QLineEdit(title);
        titleLine->setReadOnly(true);
    }
    else titleLine = NULL;

    QString id = elt.attribute(UUID_NAME);
    if(!id.isEmpty()) {
        uuidLine = new QLineEdit(id);
        uuidLine->setMinimumWidth(250);
        uuidLine->setReadOnly(true);
    }
    else uuidLine = NULL;

    QDateTime dt = QDateTime::fromString(elt.attribute(DATE_CREATED));
    if(dt.isValid()) {
        dt.setTimeSpec(Qt::UTC);
        dt = dt.toLocalTime();
        createdLabel2 = new QLabel(dt.toString(Qt::SystemLocaleLongDate));
    } else createdLabel2 = NULL;

    // Date/time info is stored in UTC. We convert from this to local time and
    // display the result in a more user-friendly format.
    dt = QDateTime::fromString(elt.attribute(DATE_MODIFIED));
    if(dt.isValid()) {
        dt.setTimeSpec(Qt::UTC);
        dt = dt.toLocalTime();
        modifiedLabel2 = new QLabel(dt.toString(Qt::SystemLocaleLongDate));
    } else modifiedLabel2 = NULL;

    dt = QDateTime::fromString(elt.attribute(DATE_ACCESSED));
    if(dt.isValid()) {
        dt.setTimeSpec(Qt::UTC);
        dt = dt.toLocalTime();
        accessedLabel2 = new QLabel(dt.toString(Qt::SystemLocaleLongDate));
    } else accessedLabel2 = NULL;

    int row = grid->rowCount();
    if(titleLine) {
        grid->addWidget(titleLabel, row, 0);
        grid->addWidget(titleLine, row, 1);
    }
    row = grid->rowCount();
    if(uuidLine) {
        grid->addWidget(uuidLabel, row, 0);
        grid->addWidget(uuidLine, row, 1);
    }
    row = grid->rowCount();
    if(createdLabel2) {
        grid->addWidget(createdLabel, row, 0);
        grid->addWidget(createdLabel2, row, 1);
    }
    row = grid->rowCount();
    if(modifiedLabel2) {
        grid->addWidget(modifiedLabel, row, 0);
        grid->addWidget(modifiedLabel2, row, 1);
    }
    row = grid->rowCount();
    if(accessedLabel2) {
        grid->addWidget(accessedLabel, row, 0);
        grid->addWidget(accessedLabel2, row, 1);
    }

    grid->setColumnStretch(0,0);
    grid->setColumnStretch(1,5);
    grid->setVerticalSpacing(15);
    grid->setSizeConstraint(QLayout::SetFixedSize);
}

/*
 * Expands/collapses the currently selected folder, or displays the currently
 * selected bookmark.
 */
void BookmarkWidget::handleDoubleClick(QTreeWidgetItem* item, int x)
{
    if(!item || !mw()->VTKA() ) return;

    int type = item->data(TITLE_COLUMN, Qt::UserRole).toInt();

    if(type == BOOKMARK) viewBookmark(item);
    else if(type == FOLDER) {
        // DT: not !item->isExpanded() -- when double clicking itemExpanded/Collapsed is also emitted
        // so there seems to be some lag in updating.
        item->setExpanded(item->isExpanded());
    }
}

/*
 * Refreshes the information associated with the currently visualized object.
 */
void BookmarkWidget::refreshCurrentFileInfo()
{
    if(mw()->VTKA()) {
        bfn = mw()->VTKA()->mFilename;
        mode = mw()->VTKA()->mWidgetMode;
        dirLightOn = mw()->VTKA()->getIsDirectionalLight();
        interpOn = mw()->VTKA()->mIsInterpolateOn;
        renderMode = mw()->VTKA()->mRenderMode3D;
        textureOn = mw()->VTKA()->mIsTextureOn;
        tfn = mw()->VTKA()->mRgbTextureFilename;
        vRenderMode = mw()->VTKA()->mVolumeRenderMode;
        blend = mw()->VTKA()->mBlendType;
        resolution = mw()->VTKA()->mReductionFactor;
        if(mw()->VTKA()->mRenderer)
            camera = mw()->VTKA()->mRenderer->GetActiveCamera();

		rtiP1 = -1.0; rtiP2 = -1.0; rtiP3 = -1.0; // invalid values indicate invalid fields
		if (mw()->VTKA()->mRTIbrowser) { 
			renderModeRTI = mw()->VTKA()->mRTIbrowser->getRenderingMode();
			QMap<int, RenderingMode*>* list = mw()->VTKA()->mRTIbrowser->getRenderingModes();
			renderingMode = list->value(static_cast<int>(renderModeRTI));
		}
		else { renderModeRTI = RenderingRTI::DEFAULT; }
    }
    if(mw()->mCtControl) {
        slice = mw()->mCtControl->GetSliceCurrent();
        orientation = mw()->mCtControl->GetOrientationCurrent();
    }
}

/*
 * Returns the filename where the bookmarks for the current object are stored.
 */
QString BookmarkWidget::getBookmarkFilepath()
{
    refreshCurrentFileInfo();

    if(bfn.isEmpty()) return QString();

    QFileInfo finfo(bfn);
	/*********** Modified by Weiqi Shi 1/15/2016*************/
    //QString path = mw()->getDataLocation();
	QString path = mw()->VTKA()->mProjectPath;
    // DT: ensures that the directory for the current file exists or will exist.
	/*********** Modified by Weiqi Shi 1/15/2016*************/
	// the comment parts are the previous version
    //mw()->createDataLocation(path);
	path.append(QDir::separator() + QString("BookMark"));

    QDir* dir = new QDir();
    //QString fn = QDir::toNativeSeparators(dir->relativeFilePath(finfo.absoluteFilePath()));

    //path.append(QDir::separator() + finfo.fileName().simplified().replace(" ", WORD_SEPARATOR) + WORD_SEPARATOR
    //            + QString::number(qHash(fn)));
	/*********** Modified by Weiqi Shi 1/15/2016*************/
    if(!QDir(path).exists()) QDir().mkdir(path);

    path = path + QDir::separator() + BOOKMARK_FN;
    return QDir::toNativeSeparators(path);
}

/*
 * Saves the folder state to file when it is expanded/collapsed.
 */
void BookmarkWidget::saveFolderState(QTreeWidgetItem* item)
{
    if(!item || !mw()->VTKA() ) return;

    QString path = getBookmarkFilepath();
    QFile f(path);
    if(!f.open(QIODevice::ReadWrite | QIODevice::Text)) {
        showFileOpenError();
        return;
    }

    QDomDocument doc;
    doc.setContent(&f);
    QDomNodeList list = doc.elementsByTagName(BOOKMARK_XML_ROOT);
    if(list.isEmpty()) {
        showInvalidFileError();
        return;
    }

    QDomElement root = list.at(0).toElement();
    QString dt = QDateTime::currentDateTimeUtc().toString();
    root.setAttribute(DATE_ACCESSED, dt);

    QDomNodeList folders = root.elementsByTagName(FOLDER_NAME);
    for(int i = 0; i < folders.length(); i++) {
        QDomElement elt = folders.at(i).toElement();
        QString value = elt.attribute(TITLE);
        if(value == item->text(0)) {
            elt.setAttribute(FOLDED, item->isExpanded() ? "no" : "yes");
            elt.setAttribute(DATE_ACCESSED, dt);
        }
    }

    saveDOMToFile(doc);
}


void BookmarkWidget::refreshBookmarkList()
{
    bTreeWidget->clear();

    if(!mw()->VTKA() ){
        //newFolderButton->setEnabled(false);
		addButton->setEnabled(false);
        editButton->setEnabled(false);
        deleteButton->setEnabled(false);
        return;
    }

    //newFolderButton->setEnabled(true);
	addButton->setEnabled(true);
    editButton->setEnabled(true);
    deleteButton->setEnabled(true);

    QString path = getBookmarkFilepath();
    BookmarkXMLReader reader(bTreeWidget, path);
    reader.readXML();
}

void BookmarkWidget::buildDOMDocument(QDomDocument& doc, QDomElement& root)
{
    QDomProcessingInstruction instruct = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruct);

    root = doc.createElement(BOOKMARK_XML_ROOT);

    QDir* dir = new QDir();
    QFileInfo finfo(bfn);
    QString fn = QDir::toNativeSeparators(dir->relativeFilePath(finfo.absoluteFilePath()));
    root.setAttribute(FILENAME, fn);
    root.setAttribute(UUID_NAME, QUuid::createUuid());

    QString dt = QDateTime::currentDateTimeUtc().toString();
    root.setAttribute(DATE_CREATED, dt);
    root.setAttribute(DATE_MODIFIED, dt);
    root.setAttribute(DATE_ACCESSED, dt);
    doc.appendChild(root);
}

bool BookmarkWidget::viewBookmark(QTreeWidgetItem* item)
{
    double pos[3]; // camera position
    double foc[3]; // focal point
    double angle; // view angle
    double view[3]; // view up direction
    double clip[2]; // clipping range
    double scale; // parallel scale
    int filetype;
    double brightness = mw()->mLightControl->GetIntensityL1();
    double contrast = mw()->mLightControl->GetIntensityL2();
    int directionalLightOn;
    int interpolationOn;
    int mode3D;
	int modeRTI;
    int textureOn;
    int ctSlice = 0;
    int ctOrientation = 0;
    int ctMode;
    int ctBlendType;
    double ctResolution;

    QString uuid = item->text(UUID_COLUMN);

    double orientation[3];
    vtkSmartPointer<vtkAssembly> assembly = mw()->mLightControl->GetAssembly();
    assembly->GetOrientation(orientation);

    if(!mw()->VTKA() ) return false;

    vtkSmartPointer<vtkCamera> camera = mw()->VTKA()->mRenderer->GetActiveCamera();

    QString path = getBookmarkFilepath();
    QFile file(path);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        showFileOpenError();
        file.close();
        return false;
    }
    QDomDocument doc;
    doc.setContent(&file);
    QDomNodeList list = doc.elementsByTagName(BOOKMARK_XML_ROOT);
    if(list.isEmpty()) {
        showInvalidFileError();
        file.close();
        return false;
    }
    QDomElement root = list.at(0).toElement();
    QDomNodeList bmks = root.elementsByTagName(BOOKMARK_NAME);
    QDomElement elt;
    QString value;
    for(int i = 0; i < bmks.length(); i++) {
        elt = bmks.at(i).toElement();
        value = elt.attribute(UUID_NAME);
        if(value == uuid) {
            break;
        }
        value.clear();
    }
    if(value.isEmpty()) {
        QMessageBox mb;
        mb.critical(this, tr("Bookmark Error"),
                    tr("The bookmark with the provided caption cannot be found."));
        file.close();
        return false;
    }

    list = elt.elementsByTagName("filetype");
    if(list.isEmpty()) {
        QMessageBox mb;
        mb.critical(this, tr("Bookmark Error"),
                    tr("This bookmark cannot be loaded because it corresponds to an unrecognized file type."));
        file.close();
        return false;
    } else {
        filetype = list.at(0).toElement().text().toInt();
        if(filetype < IMAGE2D && filetype > RTI2D) {
            QMessageBox mb;
            mb.critical(this, tr("Bookmark Error"),
                        tr("This bookmark cannot be loaded because it corresponds to an unrecognized file type."));
            file.close();
            return false;
        } else if(filetype != mode && !(filetype == CTSTACK && mode == CTVOLUME)
                  && !(filetype == CTVOLUME && mode == CTSTACK)) {
            QMessageBox mb;
            mb.critical(this, tr("Bookmark Error"),
                        tr("This bookmark cannot be loaded because it corresponds to a file type different from the current file."));
            file.close();
            return false;
        }
    }

    if(filetype == CTSTACK && mode == CTVOLUME) {
        mw()->mLightControl->reset();
        mw()->mCtControl->setCTStackView();
        viewBookmark(item);
        return true;
    } else if(filetype == CTVOLUME && mode == CTSTACK) {
        mw()->mLightControl->reset();
        mw()->mCtControl->setVTvolumeGPU();
        viewBookmark(item);
        return true;
    }

    QDomNodeList list1 = elt.elementsByTagName("camera_position");
    if(!list1.isEmpty()) {
        pos[0] = list1.at(0).toElement().elementsByTagName("x").at(0).toElement().text().toDouble();
        pos[1] = list1.at(0).toElement().elementsByTagName("y").at(0).toElement().text().toDouble();
        pos[2] = list1.at(0).toElement().elementsByTagName("z").at(0).toElement().text().toDouble();
        camera->SetPosition(pos);
    }

    QDomNodeList list2 = elt.elementsByTagName("focal_point");
    if(!list2.isEmpty()) {
        foc[0] = list2.at(0).toElement().elementsByTagName("x").at(0).toElement().text().toDouble();
        foc[1] = list2.at(0).toElement().elementsByTagName("y").at(0).toElement().text().toDouble();
        foc[2] = list2.at(0).toElement().elementsByTagName("z").at(0).toElement().text().toDouble();
        camera->SetFocalPoint(foc);
    }

    QDomNodeList list3 = elt.elementsByTagName("camera_angle");
    if(!list3.isEmpty()) {
        angle = list3.at(0).toElement().text().toDouble();
        camera->SetViewAngle(angle);
    }

    QDomNodeList list4 = elt.elementsByTagName("view_up");
    if(!list4.isEmpty()) {
        view[0] = list4.at(0).toElement().elementsByTagName("x").at(0).toElement().text().toDouble();
        view[1] = list4.at(0).toElement().elementsByTagName("y").at(0).toElement().text().toDouble();
        view[2] = list4.at(0).toElement().elementsByTagName("z").at(0).toElement().text().toDouble();
        camera->SetViewUp(view);
    }

    QDomNodeList list5 = elt.elementsByTagName("clipping_range");
    if(!list5.isEmpty()) {
        clip[0] = list5.at(0).toElement().elementsByTagName("near_plane").at(0).toElement().text().toDouble();
        clip[1] = list5.at(0).toElement().elementsByTagName("far_plane").at(0).toElement().text().toDouble();
        camera->SetClippingRange(clip);
    }

    QDomNodeList list6 = elt.elementsByTagName("parallel_scale");
    if(!list6.isEmpty()) {
        scale = list6.at(0).toElement().text().toDouble();
        camera->SetParallelScale(scale);
    }

    if(filetype == IMAGE2D) {
        list = elt.elementsByTagName("interpolation_on");
        if(!list.isEmpty()) {
            interpolationOn = list.at(0).toElement().text().toInt();
            if(interpolationOn == INTERPOLATION_ON) {
                mw()->renderModeInterpolationAct->setChecked(true);
                mw()->VTKA()->setInterpolateOn(true);
            } else {
                mw()->renderModeInterpolationAct->setChecked(false);
                mw()->VTKA()->setInterpolateOn(false);
            }
        }
	} else if(filetype == RTI2D) {
		list = elt.elementsByTagName("light_vector");
		vcg::Point3f light;
        if(!list.isEmpty()) {
            light[0] = list.at(0).toElement().elementsByTagName("x").at(0).toElement().text().toDouble();
            light[1] = list.at(0).toElement().elementsByTagName("y").at(0).toElement().text().toDouble();
            light[2] = list.at(0).toElement().elementsByTagName("z").at(0).toElement().text().toDouble();
        }

		list = elt.elementsByTagName("display_mode");
        if(!list.isEmpty()) {
            modeRTI = list.at(0).toElement().text().toInt();
			mw()->rendDlg->setRenderingMode(modeRTI);
		}

		list = elt.elementsByTagName("rti_para"); 
		if (!list.isEmpty()) {
			RenderingMode* renderingMode = mw()->VTKA()->mRTIbrowser->getRenderingModes()->value(modeRTI);
			switch (modeRTI)
			{
				case DIFFUSE_GAIN:
				{
					DiffuseGain* dg = static_cast<DiffuseGain*> (renderingMode);
					int gain = list.at(0).toElement().elementsByTagName("gain").at(0).toElement().text().toInt();
					dg->setGain(gain);

					DiffuseGControl * dgc = (DiffuseGControl*)mw()->rendDlg->getRenderingControl();
					dgc->groups.at(0)->spinBox->setValue(gain);

					break;
				}

				case SPECULAR_ENHANCEMENT:
				{
					SpecularEnhancement* se = static_cast<SpecularEnhancement*> (renderingMode);
					
					int diffuseColor  = list.at(0).toElement().elementsByTagName("diffuseColor").at(0).toElement().text().toInt();
					int specularity   = list.at(0).toElement().elementsByTagName("specularity").at(0).toElement().text().toInt();
					int highlightSize = list.at(0).toElement().elementsByTagName("highlightSize").at(0).toElement().text().toInt();
					se->setKd(diffuseColor);
					se->setKs(specularity);
					se->setExp(highlightSize);

					SpecularEControl * sec = (SpecularEControl*)mw()->rendDlg->getRenderingControl();
					sec->groups.at(0)->spinBox->setValue(diffuseColor);
					sec->groups.at(1)->spinBox->setValue(specularity);
					sec->groups.at(2)->spinBox->setValue(highlightSize);

					break;
				}

				case NORMAL_ENHANCEMENT:
				{
					NormalEnhancement* ne = static_cast<NormalEnhancement*> (renderingMode);
					
					int gain  = list.at(0).toElement().elementsByTagName("gain").at(0).toElement().text().toInt();
					int environment   = list.at(0).toElement().elementsByTagName("environment").at(0).toElement().text().toInt();
					
					ne->setGain(gain);
					ne->setEnvIll(environment);

					NormalEControl * nec = (NormalEControl*)mw()->rendDlg->getRenderingControl();
					nec->groups.at(0)->spinBox->setValue(gain);
					nec->groups.at(1)->spinBox->setValue(environment);

					break;
				}

				case UNSHARP_MASKING_IMG:
				{
					UnsharpMasking* um_img = static_cast<UnsharpMasking*> (renderingMode);
					int gain = list.at(0).toElement().elementsByTagName("gain").at(0).toElement().text().toInt();
					um_img->setGain(gain);

					UnsharpMControl * um_img_c = (UnsharpMControl*)mw()->rendDlg->getRenderingControl();
					um_img_c->groups.at(0)->spinBox->setValue(gain);

					break;
				}

				case UNSHARP_MASKING_LUM:
				{
					UnsharpMasking* um_lum = static_cast<UnsharpMasking*> (renderingMode);
					int gain = list.at(0).toElement().elementsByTagName("gain").at(0).toElement().text().toInt();
					um_lum->setGain(gain);

					UnsharpMControl * um_lum_c = (UnsharpMControl*)mw()->rendDlg->getRenderingControl();
					um_lum_c->groups.at(0)->spinBox->setValue(gain);

					break;
				}

				case COEFF_ENHANCEMENT:
				{
					CoeffEnhancement* ce = static_cast<CoeffEnhancement*> (renderingMode);
					int gain = list.at(0).toElement().elementsByTagName("gain").at(0).toElement().text().toInt();
					ce->setGain(gain);

					CoeffEnhancControl * cec = (CoeffEnhancControl*)mw()->rendDlg->getRenderingControl();
					cec->groups.at(0)->spinBox->setValue(gain);

					break;
				}

				case DETAIL_ENHANCEMENT:
				{
					DetailEnhancement* de = static_cast<DetailEnhancement*> (renderingMode);

					OffsetNum o;
					TileSize size;
					int level;
					SharpnessMeasures m;
					SphereSampling ss;
					float k1;
					float k2;
					float t;
					SmoothingFilter f;
					int nIter;

					o = (OffsetNum)((int)list.at(0).toElement().elementsByTagName("localOffset").at(0).toElement().text().toInt());
					size = (TileSize)((int)list.at(0).toElement().elementsByTagName("tileSize").at(0).toElement().text().toInt());
					level = list.at(0).toElement().elementsByTagName("numberInitialTiles").at(0).toElement().text().toInt();
					m = (SharpnessMeasures)((int)list.at(0).toElement().elementsByTagName("sharpnessOperator").at(0).toElement().text().toInt());
					ss = (SphereSampling)((int)list.at(0).toElement().elementsByTagName("lightSampling").at(0).toElement().text().toInt());
					k1 = list.at(0).toElement().elementsByTagName("k1Sharpness").at(0).toElement().text().toFloat();
					k2 = list.at(0).toElement().elementsByTagName("k2Lightness").at(0).toElement().text().toFloat();
					t = list.at(0).toElement().elementsByTagName("threshold").at(0).toElement().text().toFloat();
					f = (SmoothingFilter)((int)list.at(0).toElement().elementsByTagName("smoothingFilter").at(0).toElement().text().toInt());
					nIter = list.at(0).toElement().elementsByTagName("numberIterationSmoothing").at(0).toElement().text().toInt();
					de->updateConfig(o, size, level, m, ss, k1, k2, t, f, nIter);

					break;
				}

				case DYN_DETAIL_ENHANCEMENT: 
				{
					DynamicDetailEnh* dde = static_cast<DynamicDetailEnh*> (renderingMode);

					int o;
					int size;
					SharpnessMeasuresDyn m;
					SphereSamplingDyn ss;
					float k1;
					float k2;
					float t;
					SmoothingFilterDyn f;
					int nIter;

					o = (OffsetNum)((int)list.at(0).toElement().elementsByTagName("localOffset").at(0).toElement().text().toInt());
					size = (TileSize)((int)list.at(0).toElement().elementsByTagName("tileSize").at(0).toElement().text().toInt());
					m = (SharpnessMeasuresDyn)((int)list.at(0).toElement().elementsByTagName("sharpnessOperator").at(0).toElement().text().toInt());
					ss = (SphereSamplingDyn)((int)list.at(0).toElement().elementsByTagName("lightSampling").at(0).toElement().text().toInt());
					k1 = list.at(0).toElement().elementsByTagName("k1Sharpness").at(0).toElement().text().toFloat();
					k2 = list.at(0).toElement().elementsByTagName("k2Lightness").at(0).toElement().text().toFloat();
					t = list.at(0).toElement().elementsByTagName("threshold").at(0).toElement().text().toFloat();
					f = (SmoothingFilterDyn)((int)list.at(0).toElement().elementsByTagName("smoothingFilter").at(0).toElement().text().toInt());
					nIter = list.at(0).toElement().elementsByTagName("numberIterationSmoothing").at(0).toElement().text().toInt();
					
					dde->setOffset(o);
					dde->setTileSize(size);
					dde->updateConfig(m, ss, k1, k2, t,f, nIter);
				}

				default: { break; }
			}
		}

		list = elt.elementsByTagName("interpolation_on");
        if(!list.isEmpty()) {
            interpolationOn = list.at(0).toElement().text().toInt();
            if(interpolationOn == INTERPOLATION_ON) {
                mw()->renderModeInterpolationAct->setChecked(true);
                mw()->VTKA()->setInterpolateOn(true);
            } else {
                mw()->renderModeInterpolationAct->setChecked(false);
                mw()->VTKA()->setInterpolateOn(false);
            }
        }

		mw()->mLightControlRTI->setLight(light, true);
		// mw()->rendDlg->setRTIPara(p1, p2, p3);

	} else if(filetype == MODEL3D) {
        list = elt.elementsByTagName("directional_light");
        if(!list.isEmpty()) {
            directionalLightOn = list.at(0).toElement().text().toInt();
            if(directionalLightOn) {
                mw()->setDirectionalLightAct->setChecked(true);
                mw()->mLightControl->setDirectionalLight(true);
            } else {
                mw()->setDirectionalLightAct->setChecked(false);
                mw()->mLightControl->setDirectionalLight(false);
            }
        } else {
            mw()->setDirectionalLightAct->setChecked(true);
            mw()->mLightControl->setDirectionalLight(true);
        }

        list = elt.elementsByTagName("light_vector");
        if(!list.isEmpty()) {
            orientation[0] = list.at(0).toElement().elementsByTagName("x").at(0).toElement().text().toDouble();
            orientation[1] = list.at(0).toElement().elementsByTagName("y").at(0).toElement().text().toDouble();
            orientation[2] = list.at(0).toElement().elementsByTagName("z").at(0).toElement().text().toDouble();
        }

        list = elt.elementsByTagName("brightness");
        if(!list.isEmpty()) {
            brightness = list.at(0).toElement().text().toDouble();
        }
        list = elt.elementsByTagName("contrast");
        if(!list.isEmpty()) {
            contrast = list.at(0).toElement().text().toDouble();
        }

        list = elt.elementsByTagName("interpolation_on");
        if(!list.isEmpty()) {
            interpolationOn = list.at(0).toElement().text().toInt();
            if(interpolationOn == INTERPOLATION_ON) {
                mw()->renderModeInterpolationAct->setChecked(true);
                mw()->VTKA()->setInterpolateOn(true);
            } else {
                mw()->renderModeInterpolationAct->setChecked(false);
                mw()->VTKA()->setInterpolateOn(false);
            }
        }

        list = elt.elementsByTagName("display_mode");
        if(!list.isEmpty()) {
            mode3D = list.at(0).toElement().text().toInt();
            if(mode3D == POINTS3D) {
                mw()->renderModePointsAct->setChecked(true);
                mw()->renderModeWireAct->setChecked(false);
                mw()->renderModeFlatAct->setChecked(false);
                mw()->VTKA()->setRenderMode3D(POINTS3D);
            } else if(mode3D == WIREFRAME3D) {
                mw()->renderModePointsAct->setChecked(false);
                mw()->renderModeWireAct->setChecked(true);
                mw()->renderModeFlatAct->setChecked(false);
                mw()->VTKA()->setRenderMode3D(WIREFRAME3D);
            } else if (mode3D == SURFACE3D){
                mw()->renderModePointsAct->setChecked(false);
                mw()->renderModeWireAct->setChecked(false);
                mw()->renderModeFlatAct->setChecked(true);
                mw()->VTKA()->setRenderMode3D(SURFACE3D);
            }
        }

        list = elt.elementsByTagName("texture_on");
        if(!list.isEmpty()) {
            textureOn = list.at(0).toElement().text().toInt();
            if(textureOn == TEXTURE_ON) {
                mw()->renderModeTextureAct->setChecked(true);
                mw()->renderModeTextureAct->setIcon(QIcon(":/images/textureson.png"));
                mw()->VTKA()->setTextureOn(true);
            } else if(textureOn == TEXTURE_OFF){
                mw()->renderModeTextureAct->setChecked(false);
                mw()->renderModeTextureAct->setIcon(QIcon(":/images/texturesoff.png"));
                mw()->VTKA()->setTextureOn(false);
            }
        }

        mw()->mLightControl->restoreBookmarkLight(orientation, brightness, contrast, MODEL3D);

    } else if(filetype == CTSTACK) {
        list = elt.elementsByTagName("brightness");
        if(!list.isEmpty()) {
            brightness = list.at(0).toElement().text().toDouble();
        }
        list = elt.elementsByTagName("contrast");
        if(!list.isEmpty()) {
            contrast = list.at(0).toElement().text().toDouble();
        }

        list = elt.elementsByTagName("interpolation_on");
        if(!list.isEmpty()) {
            interpolationOn = list.at(0).toElement().text().toInt();
            if(interpolationOn == INTERPOLATION_ON) {
                mw()->renderModeInterpolationAct->setChecked(true);
                mw()->VTKA()->setInterpolateOn(true);
            } else {
                mw()->renderModeInterpolationAct->setChecked(false);
                mw()->VTKA()->setInterpolateOn(false);
            }
        }

        list = elt.elementsByTagName("slice");
        if(!list.isEmpty()) {
            ctSlice = list.at(0).toElement().text().toInt();
        }
        list = elt.elementsByTagName("orientation");
        if(!list.isEmpty()) {
            ctOrientation = list.at(0).toElement().text().toInt();
        }

        mw()->mLightControl->restoreBookmarkLight(orientation, brightness, contrast, CTSTACK);
        mw()->mCtControl->restoreBookmarkCTStack(ctSlice, ctOrientation);

        if(!list1.isEmpty()) camera->SetPosition(pos);
        if(!list2.isEmpty()) camera->SetFocalPoint(foc);
        if(!list3.isEmpty()) camera->SetViewAngle(angle);
        if(!list4.isEmpty()) camera->SetViewUp(view);
        if(!list5.isEmpty()) camera->SetClippingRange(clip);
        if(!list6.isEmpty()) camera->SetParallelScale(scale);

    } else if(filetype == CTVOLUME) {
        list = elt.elementsByTagName("render_mode");
        if(!list.isEmpty()) {
            ctMode = list.at(0).toElement().text().toInt();
        }
        list = elt.elementsByTagName("blend_type");
        if(!list.isEmpty()) {
            ctBlendType = list.at(0).toElement().text().toInt();
        }
        list = elt.elementsByTagName("resolution");
        if(!list.isEmpty()) {
            ctResolution = list.at(0).toElement().text().toDouble();
        }
        list = elt.elementsByTagName("light_vector");
        if(!list.isEmpty()) {
            orientation[0] = list.at(0).toElement().elementsByTagName("x").at(0).toElement().text().toDouble();
            orientation[1] = list.at(0).toElement().elementsByTagName("y").at(0).toElement().text().toDouble();
            orientation[2] = list.at(0).toElement().elementsByTagName("z").at(0).toElement().text().toDouble();
        }

        mw()->mCtControl->restoreBookmarkCTVolume(ctMode, ctBlendType, ctResolution);
        //mw()->mLightControl->restoreBookmarkLight(orientation, brightness, contrast, CTVOLUME);

        if(!list1.isEmpty()) camera->SetPosition(pos);
        if(!list2.isEmpty()) camera->SetFocalPoint(foc);
        if(!list3.isEmpty()) camera->SetViewAngle(angle);
        if(!list4.isEmpty()) camera->SetViewUp(view);
        if(!list5.isEmpty()) camera->SetClippingRange(clip);
        if(!list6.isEmpty()) camera->SetParallelScale(scale);

        if(!list1.isEmpty()) camera->SetPosition(pos);
        if(!list2.isEmpty()) camera->SetFocalPoint(foc);
        if(!list3.isEmpty()) camera->SetViewAngle(angle);
        if(!list4.isEmpty()) camera->SetViewUp(view);
        if(!list5.isEmpty()) camera->SetClippingRange(clip);
        if(!list6.isEmpty()) camera->SetParallelScale(scale);

    } else {
        QMessageBox mb;
        mb.critical(this, tr("Bookmark Error"),
                    tr("This bookmark cannot be loaded because it corresponds to an unrecognized file type."));
        file.close();
        return false;
    }

    if (mw()->VTKA()->mQVTKWidget) mw()->VTKA()->mQVTKWidget->show();
    if (mw()->VTKA()->mQVTKWidget) mw()->VTKA()->mQVTKWidget->update();

    QString dt = QDateTime::currentDateTimeUtc().toString();
    elt.setAttribute(DATE_ACCESSED, dt);
    saveDOMToFile(doc);
    return true;
}


void BookmarkWidget::createFolder()
{
    if(!mw()->VTKA() ) return;
    bool ok = true;

    QString path = getBookmarkFilepath();
    QFile bf(path);
    if(!bf.open(QFile::ReadWrite | QFile::Text)) {
        showFileOpenError();
        bf.close();
        return;
    }

    QDomDocument doc;
    QDomElement root;
    QDomNodeList list;
    if(bf.size() == 0) {
        buildDOMDocument(doc, root);
    } else {
        qDebug() << bf.size();
        doc.setContent(&bf);
        list = doc.elementsByTagName(BOOKMARK_XML_ROOT);
        if(list.isEmpty()) {
            showInvalidFileError();
            return;
        }
        root = list.at(0).toElement();
    }

    QInputDialog* titleBox = new QInputDialog(this);
    QString title;
    do
    {
        title = titleBox->getText(this, tr("New Folder"),
                                      tr("Enter the title for your new folder:"), QLineEdit::Normal, "", &ok);
    } while(title.isEmpty() && ok);

    if(!ok) return;

    QDomElement fldr = doc.createElement(FOLDER_NAME);
    fldr.setAttribute(TITLE, title);
    QString dt = QDateTime::currentDateTimeUtc().toString();
    fldr.setAttribute(DATE_CREATED, dt);
    fldr.setAttribute(DATE_MODIFIED, dt);
    fldr.setAttribute(DATE_ACCESSED, dt);
    fldr.setAttribute(UUID_NAME, QUuid::createUuid());
    fldr.setAttribute(FOLDED, "yes");
    root.appendChild(fldr);

    saveDOMToFile(doc);
    refreshBookmarkList();
}


void BookmarkWidget::createBookmark()
{
    if(!mw()->VTKA() ) return;

    QString path = getBookmarkFilepath();
    QFile bf(path);
    if(!bf.open(QFile::ReadWrite | QFile::Text)) {
        showFileOpenError();
        bf.close();
        return;
    }

    QDomDocument doc;
    QDomElement root;
    QDomNodeList list;

    if(bf.size() == 0) {
        buildDOMDocument(doc, root);
    } else {   
        doc.setContent(&bf);
        list = doc.elementsByTagName(BOOKMARK_XML_ROOT);
        if(list.isEmpty()) {
            showInvalidFileError();
            return;
        }
        root = list.at(0).toElement();
    }

    QInputDialog* captionBox = new QInputDialog(this);
    QString caption;
    bool ok = true;
    do
    {
        caption = captionBox->getText(this, tr("Bookmark View"),
                                      tr("Enter a caption for this viewpoint:"), QLineEdit::Normal, "", &ok);
    } while(caption.isEmpty() && ok);

    if(!ok) return;

    createBookmarkSubclass(caption, doc, root);

    root.setAttribute(DATE_MODIFIED, QDateTime::currentDateTimeUtc().toString());
    saveDOMToFile(doc);
    if(mw()->VTKA()->mQVTKWidget) mw()->VTKA()->mQVTKWidget->update();
    refreshBookmarkList();
}

void BookmarkWidget::createBookmarkSubclass(QString caption, QDomDocument& doc, QDomElement& root)
{
    vtkSmartPointer<vtkAssembly> assembly = mw()->mLightControl->GetAssembly();

	vcg::Point3f light = mw()->mLightControlRTI->getLight(); /*!< Light vector. */

    QUuid uuid = QUuid::createUuid();

    switch(mode)
    {
    case IMAGE2D:
    {
        BookmarkIMAGE2D bmk(caption, uuid, doc, camera, mode, interpOn);
        root.appendChild(bmk.getBookmark());
        break;
    }

	case RTI2D:
    {
		BookmarkRTI2D bmk(caption, uuid, doc, camera, mode, interpOn, light, renderModeRTI, renderingMode);
        root.appendChild(bmk.getBookmark());

        return;
    }

    case MODEL3D:
    {
        BookmarkMODEL3D bmk(caption, uuid, doc, camera, mode, interpOn, dirLightOn, assembly,
                            mw()->mLightControl->GetIntensityL1(), mw()->mLightControl->GetIntensityL2(),
                            renderMode, textureOn, tfn);
        root.appendChild(bmk.getBookmark());
        break;
    }

    case CTSTACK:
    {
        BookmarkCTSTACK bmk(caption, uuid, doc, camera, mode, interpOn, mw()->mLightControl->GetIntensityL1(),
                            mw()->mLightControl->GetIntensityL2(), slice, orientation);
        root.appendChild(bmk.getBookmark());
        break;
    }

    case CTVOLUME:
    {
        BookmarkCTVOLUME bmk(caption, uuid, doc, camera, mode, assembly, vRenderMode, blend, resolution);
        root.appendChild(bmk.getBookmark());
        break;
    }

    default:
        return;
    }
}

void BookmarkWidget::editItem(QTreeWidgetItem* item)
{
    if(!item) {
        QList<QTreeWidgetItem *> selected = bTreeWidget->selectedItems();
        if(selected.isEmpty()) return;
        item = selected.at(0);
        if(!item) return;
    }

    int type = item->data(TITLE_COLUMN, Qt::UserRole).toInt();
    QString uuid = item->text(UUID_COLUMN);
    QString oldTitle = item->text(TITLE_COLUMN);

    QString header, message;
    if(type == FOLDER) {
        header = tr("Edit Folder");
        message = tr("Enter a new title for this folder:");
    }
    else if(type == BOOKMARK) {
        header = tr("Edit Bookmark");
        message = tr("Enter a new caption for this viewpoint:");
    }
    if(header.isEmpty() || message.isEmpty()) return;

    QInputDialog* titleBox = new QInputDialog(this);
    QString title;
    bool ok = true;
    do
    {
        title = titleBox->getText(this, header, message, QLineEdit::Normal, oldTitle, &ok);
    } while(title.isEmpty() && ok);

    if(!ok) return;

    QDomDocument doc;
    QDomElement elt = findElementbyUUID(doc, uuid, type);
    if(elt.isNull()) return;
    elt.setAttribute(TITLE, title);
    elt.setAttribute(DATE_MODIFIED, QDateTime::currentDateTimeUtc().toString());
    saveDOMToFile(doc);
    refreshBookmarkList();
}

void BookmarkWidget::deleteItem(QTreeWidgetItem* item)
{
    if(!item) {
        QList<QTreeWidgetItem*> list = bTreeWidget->selectedItems();
        if(list.isEmpty()) return;
        item = list.at(0);
        if(!item) return;
    }

    // DT: don't move this!
    int type = item->data(TITLE_COLUMN, Qt::UserRole).toInt();
    QString uuid = item->text(UUID_COLUMN);

    if(verifyDelete(item, type) == QMessageBox::Cancel) return;

    QDomDocument doc;
    QDomElement elt = findElementbyUUID(doc, uuid, type);

    QList<QTreeWidgetItem *> items = bTreeWidget->findItems(uuid, Qt::MatchExactly, UUID_COLUMN);
    item = items.at(0);
    delete item;

    if(elt.isNull()) return;

    QDomNode parent = elt.parentNode();
    parent.removeChild(elt);

    saveDOMToFile(doc);
    refreshBookmarkList();
}

/*
 * Checks that the user really wants to delete this item.
 */
int BookmarkWidget::verifyDelete(QTreeWidgetItem* item, int type)
{
    QString caption = item->text(TITLE_COLUMN);
    QString message;
    if(type == BOOKMARK)
        message = tr("Are you sure that you want to delete the bookmark \"") + caption + tr("\"?");
    else if(type == FOLDER)
        message = tr("Are you sure that you want to delete the folder \"") + caption + tr("\" and all of its contents?");
    QMessageBox mb;
    mb.setText(message);
    mb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    mb.setDefaultButton(QMessageBox::Ok);
    return mb.exec();
}

void BookmarkWidget::showFileOpenError()
{
    QMessageBox mb;
    mb.critical(this, tr("Bookmark Error"), tr("Cannot open CHEROb bookmark file."));
}

void BookmarkWidget::showInvalidFileError()
{
    QMessageBox mb;
    mb.critical(this, tr("Bookmark Error"), tr("Invalid CHEROb bookmark file."));

}

MainWindow* BookmarkWidget::mw()
{
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
    if (mainwindow)
    {
      return mainwindow;
    }
  }
}
