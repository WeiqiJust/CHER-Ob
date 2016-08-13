/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Min H. Kim (minhkim@cs.yale.edu)

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
   annonymous help by various software engineering communities.

*****************************************************************************/

#include "../mainWindow.h"
#include "metadata.h"
#include "../visualization/vtkWidget.h"

#define SIZE (1024)

DMetadataTreeWidget::DMetadataTreeWidget(QWidget *parent)
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

void DMetadataTreeWidget::mousePressEvent(QMouseEvent *event)
{
    QTreeWidgetItem* item = itemAt(event->pos());
    if(!item) {
        // DT: disable the edit & delete buttons since there is no element/category selected.
        ((Metadata*)(parentWidget()->parentWidget()))->editButton->setEnabled(false);
        ((Metadata*)(parentWidget()->parentWidget()))->editButton->setText("Edit");
        ((Metadata*)(parentWidget()->parentWidget()))->deleteButton->setEnabled(false);
        ((Metadata*)(parentWidget()->parentWidget()))->textEdit->setReadOnly(true);
        ((Metadata*)(parentWidget()->parentWidget()))->textEdit->clear();
    }
    BookmarkTreeWidget::mousePressEvent(event);
}


void DMetadataTreeWidget::dropEvent(QDropEvent* event)
{
    if(!event) {
        dragCaption.clear();
        dragParent = NULL;
        uuid.clear();
        ((Metadata*)(parentWidget()->parentWidget()))->refreshMetadata();
        return;
    }

    event->accept();

    // DT: user must hold down shift key to move item.
    if (event->keyboardModifiers() != Qt::ShiftModifier) {
        dragCaption.clear();
        dragParent = NULL;
        uuid.clear();
        ((Metadata*)(parentWidget()->parentWidget()))->refreshMetadata();
        return;
    }

    QString text = dragCaption;
    QTreeWidgetItem* parentItem = dragParent;
    QString quuid = uuid;
    qDebug() << "text:" << text;
    qDebug() << "quuid of parent:" << quuid;

    dragCaption.clear();
    dragParent = NULL;
    uuid.clear();

    qDebug() << "1";

    if(text.isEmpty()) {
        ((Metadata*)(parentWidget()->parentWidget()))->refreshMetadata();
        return;
    }
    qDebug() << "1.33";
    QList<QTreeWidgetItem*> list = this->findItems(quuid, Qt::MatchExactly | Qt::MatchRecursive, UUID_COLUMN);
    if(parentItem) qDebug() << "parent quuid:" << parentItem->text(UUID_COLUMN);
    qDebug() << "1.66";
    if(list.isEmpty()) {
        ((Metadata*)(parentWidget()->parentWidget()))->refreshMetadata();
        return;
    }
    QTreeWidgetItem* item = list.at(0);
    if(!item) {
        ((Metadata*)(parentWidget()->parentWidget()))->refreshMetadata();
        return;
    }

    qDebug() << "2";

    QTreeWidget::dropEvent(event);

    qDebug() << "3";

    QString filetype, texttype = "title";
    QIcon icon = item->icon(0);
    if(icon.isNull()) {
        filetype = "bookmark";
        texttype = "caption";
    } else {
        filetype = "folder";
        texttype = "title";
    }

    QTreeWidgetItem* parent = item->parent();
    QString parentText, parentFiletype, parentTexttype;
    if(parent) {
        parentText = parent->text(0);
        QIcon icon = parent->icon(0);
        if(icon.isNull()) {
            parentFiletype = "bookmark";
            parentTexttype = "caption";
        } else {
            parentFiletype = "folder";
            parentTexttype = "title";
        }
    }

    QDomNodeList rootList = ((Metadata*)(parentWidget()->parentWidget()))->xmlFile.elementsByTagName("CHEROb.metadata");
    if(list.isEmpty()) {
        QMessageBox mb;
        mb.critical(this, tr("Metadata Error"),
                    tr("Invalid CHEROb metadata file."));
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
        value = elt.attribute(texttype);
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
            value = elt.attribute(parentTexttype);
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

    ((Metadata*)(parentWidget()->parentWidget()))->refreshMetadata();

}


MainWindow* Metadata::mw()
{
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
    if (mainwindow)
    {
      return mainwindow;
    }
  }

}

void Metadata::showContextMenu(const QPoint& pos)
{
    QPoint globalPos = dataTree->mapToGlobal(pos);

    QString text, parentText;
    QTreeWidgetItem* item = NULL, *itemParent = NULL;
    QList<QTreeWidgetItem*> items = dataTree->selectedItems();
    if(!items.isEmpty()) {
        item = items.at(0);
        if(item->parent()) {
            itemParent = item->parent();
            parentText = itemParent->text(0);
        }
        text = item->text(0);
    }

    QMenu menu;
    menu.addAction("Add Category...");

    if(!items.isEmpty() && item) {
        QString addElt, editElt, deleteIt;
        addElt = "Add Element to \"" + (parentText.isEmpty()? text : parentText) + "\"...";
        menu.addAction(addElt);

        menu.addSeparator();

        if (text.size() > 30) {text.truncate(30); text.append("...");}
        if(item->parent()) {
            editElt = "Edit \"" + text + "\"";
            menu.addAction(editElt);
            if(editButton->text() == "Save")
                menu.actions().at(3)->setEnabled(false);
            menu.addSeparator();
        }
        deleteIt = "Delete \"" + text + "\"";
        menu.addAction(deleteIt);
    }

    QAction* selectedItem = menu.exec(globalPos);
    if (selectedItem)
    {
        QString selectedText = selectedItem->text();
        if(selectedText.isEmpty()) return;
        else if(selectedText.contains("Add Category")) pickCategory();
        else if(selectedText.contains("Add Element")) {
            if(item->parent()) addElement(itemParent, parentText);
            else addElement(item, text);
        }
        else if(selectedText.contains("Delete")) deleteItem();
        else if(selectedText.contains("Edit")) { toggleEdit();}
    }
    else return;

}

void Metadata::buildMetadataHash()
{
    QStringList list;

    list << tr("Catalog Level") << tr("Type") << tr("Date") << tr("Components/Parts")
             << tr("Physical Description") << tr("Orientation/Arrangement");
    elements.push_back(list);
    categories.push_back("Object/Work");
    list.clear();

    list << tr("Text") << tr("Type") << tr("Preference") << tr("Language") << tr("Date");
    elements.push_back(list);
    categories.push_back("Titles/Names");
    list.clear();

    list << tr("Creator") << tr("Date") << tr("Place") << tr("Culture") << tr("Commissioner") << tr("Numbers");
    elements.push_back(list);
    categories.push_back("Creation");
    list.clear();

    list << tr("Description") << tr("Indexing Terms");
    elements.push_back(list);
    categories.push_back("Styles/Periods");
    list.clear();

    list << tr("Description") << tr("Type") << tr("Value") << tr("Unit") << tr("Extent") << tr("Qualifier")
         << tr("Date") << tr("Shape") << tr("Format/Size");
    elements.push_back(list);
    categories.push_back("Measurements");
    list.clear();

    list << tr("Transcription") << tr("Description") << tr("Type") << tr("Author") << tr("Location") << tr("Language") << tr("Date");
    elements.push_back(list);
    categories.push_back("Inscription");
    list.clear();

    list << tr("Description") << tr("Type") << tr("Agent") << tr("Date") << tr("Place");
    elements.push_back(list);
    categories.push_back("Condition/Examination");

    elements.push_back(list);
    categories.push_back("Conservation/Treatment");
    list.clear();

    list << tr("Depiction") << tr("Terms") << tr("Place") << tr("Iconography") << tr("Interpretive History");
    elements.push_back(list);
    categories.push_back("Subject Matter");
    list.clear();

    list << tr("Type") << tr("Identification") << tr("Date") << tr("Place") << tr("Agent");
    elements.push_back(list);
    categories.push_back("Historical Context");
    list.clear();

    list << tr("Building/Site") << tr("Part/Placement") << tr("Date");
    elements.push_back(list);
    categories.push_back("Architectural Context");
    list.clear();

    list << tr("Date") << tr("Place") << tr("Sector") << tr("Excavator");
    elements.push_back(list);
    categories.push_back("Archaeological Context");
    list.clear();

    list << tr("Date") << tr("Place") << tr("Agent");
    elements.push_back(list);
    categories.push_back("Natural Context");
    list.clear();

    list << tr("Domain") << tr("Kingdom") << tr("Phylum") << tr("Class") << tr("Order") << tr("Family") << tr("Genus")
         << tr("Species") << tr("Subspecies") << tr("Variety");
    elements.push_back(list);
    categories.push_back("Taxonomy");
    list.clear();

    list << tr("Composition") << tr("Age/Stage") << tr("Epoch") << tr("Texture/Structure") << tr("Features") << tr("Habitat")
         << tr("Physical Properties") << tr("Optical Properties") << tr("Adaptations") << tr("Crystal") << tr("Plumage/Pelage");
    elements.push_back(list);
    categories.push_back("Specimen");
    list.clear();

    list << tr("Statement") << tr("Date") << tr("Place") << tr("Agent");
    elements.push_back(list);
    categories.push_back("Rights");
    list.clear();

    list << tr("Provenance") << tr("Cost/Value") << tr("Legal Status") << tr("Date") << tr("Place") << tr("Owner/Agent");
    elements.push_back(list);
    categories.push_back("Ownership/Collecting");
    list.clear();

    list << tr("Description") << tr("Title/Name") << tr("Type") << tr("Curator") << tr("Date") << tr("Place");
    elements.push_back(list);
    categories.push_back("Exhibition/Loan");

    for(int i = 0; i < elements.size(); i++) {
        QStringList sl = elements.at(i);
        sl << tr("Remarks") << tr("Citations");
        hash.insert(categories.at(i), sl);
    }
}

void Metadata::pickCategory()
{
    bool ok;
    QStringList categoryList;
    foreach(QString s, categories) {
        categoryList << s;
    }

    QString choice = QInputDialog::getItem(this, tr("Add Category"),
                                              tr("Select a new metadata category:"), categoryList, 0, false, &ok);

    if(!ok) return;

    QTreeWidgetItem* item = new QTreeWidgetItem(dataTree);
    QFileIconProvider provider;
    item->setIcon(0, provider.icon(QFileIconProvider::File));
    QFont font = item->font(0);
    font.setBold(true);
    item->setFont(0, font);
    QUuid id = QUuid::createUuid();
    item->setText(0, choice);
    item->setText(UUID_COLUMN, id.toString());
    item->setFlags(item->flags() ^ (Qt::ItemIsDropEnabled));

    mw()->isSaved = false;

    QDomElement cat = xmlFile.createElement("category");
    cat.setAttribute("title", choice);
    cat.setAttribute("date", QDateTime::currentDateTime().toString());
    cat.setAttribute("uuid", id.toString());
    cat.setAttribute("folded", "yes");
    root.appendChild(cat);

    QDomNodeList cats = root.elementsByTagName("category");
    for(int i = 0; i < cats.length(); i++) {
        QDomElement cat = cats.at(i).toElement();
        QString uuid = cat.attribute("uuid");
        QList<QTreeWidgetItem*> items = dataTree->findItems(uuid, Qt::MatchExactly | Qt::MatchRecursive, UUID_COLUMN);
        QTreeWidgetItem* item = items.at(0);
        if(!item) continue;
        bool expanded = item->isExpanded();
        cat.setAttribute("folded", expanded ? "no" : "yes");
    }
}

void Metadata::addElement(QTreeWidgetItem* item, const QString text)
{
    QStringList elts = hash.value(text);
    QString message = tr("Select a metadata element to add to the ") + text + tr(" category:");

    bool ok;
    QString choice = QInputDialog::getItem(this, tr("Add Element"), message, elts, 0, false, &ok);

    if(!ok) return;

    QTreeWidgetItem* it = new QTreeWidgetItem(item);
    QUuid id = QUuid::createUuid();
    it->setText(0, choice);
    it->setText(UUID_COLUMN, id.toString());
    it->setFlags(it->flags() ^ (Qt::ItemIsDropEnabled));

    mw()->isSaved = false;

    QDomElement elt = xmlFile.createElement("element");
    elt.setAttribute("title", choice);
    elt.setAttribute("date", QDateTime::currentDateTime().toString());
    elt.setAttribute("uuid", id.toString());

    QDomNodeList cats = root.elementsByTagName("category");
    QString uuid = item->text(UUID_COLUMN);
    QDomNode node;
    int i;
    for(i = 0; i < cats.length(); i++) {
        QDomElement elt = cats.at(i).toElement();
        QString value = elt.attribute("uuid");
        if(value == uuid) {
            node = cats.at(i);
            break;
        }
        value.clear();
    }
    if(i < cats.length()) node.toElement().appendChild(elt);

    for(int i = 0; i < cats.length(); i++) {
        QDomElement cat = cats.at(i).toElement();
        QString uuid = cat.attribute("uuid");
        QList<QTreeWidgetItem*> items = dataTree->findItems(uuid, Qt::MatchExactly | Qt::MatchRecursive, UUID_COLUMN);
        QTreeWidgetItem* item = items.at(0);
        if(!item) continue;
        bool expanded = item->isExpanded();
        cat.setAttribute("folded", expanded ? "no" : "yes");
    }
}

void Metadata::addCategory()
{
    QRadioButton* rb1, *rb2;
    QTreeWidgetItem* item;
    QString text;
    QList<QTreeWidgetItem*> items = dataTree->selectedItems();
    if(!items.isEmpty()) {
        item = items.at(0);
        if(item->parent())
            item = item->parent();
        text = item->text(0);
        QString rbtext = "Add an element to the category \"" + text + "\"";

        QDialog *dialog = new QDialog(this);
        dialog->setModal(true);
        dialog->setWindowTitle("Add Item");

        QPushButton* OK = new QPushButton("OK",dialog);
        QPushButton* cancel = new QPushButton("Cancel", dialog);
        QLabel* label = new QLabel("What would you like to add?",dialog);
        rb1 = new QRadioButton("Add a new metadata category",dialog);
        rb1->setChecked(true);
        rb2 = new QRadioButton(rbtext,dialog);

        QVBoxLayout* vbox = new QVBoxLayout;
        vbox->addWidget(label);
        vbox->addWidget(rb1);
        vbox->addWidget(rb2);
        vbox->setStretch(0, 10);

        QHBoxLayout* hbox = new QHBoxLayout;
        hbox->addWidget(OK);
        hbox->addWidget(cancel);
        vbox->addLayout(hbox);

        connect(OK, SIGNAL(clicked()), dialog, SLOT(accept()));
        connect(cancel, SIGNAL(clicked()), dialog, SLOT(reject()));

        dialog->setLayout(vbox);
        dialog->exec();

        int i = dialog->result();
        if(i == QDialog::Rejected) return;
    }

    if(items.isEmpty() || rb1->isChecked()) {
        pickCategory();
    } else {
        addElement(item, text);
    }

    refreshMetadata();
}

void Metadata::toggleEdit()
{
    QList<QTreeWidgetItem*> list = dataTree->selectedItems();
    if(list.isEmpty()) return;

    QTreeWidgetItem* item = list.at(0);
    QIcon icon = item->icon(0);
    if(!icon.isNull()) return; // DT: no info associated with categories themselves

    userIsEditing = !userIsEditing;
    if(userIsEditing) {
        editButton->setText("Save");
        textEdit->setReadOnly(false);
    } else { // Save information to XML.
        editButton->setText("Edit");
        textEdit->setReadOnly(true);
        saveItem(item);
    }
}

void Metadata::saveItem(QTreeWidgetItem* item)
{
    QString uuid = item->text(UUID_COLUMN);
    QDomNodeList list = root.elementsByTagName("element");

    QDomNode node;
    QDomElement elt;
    QString value;
    for(int i = 0; i < list.length(); i++) {
        elt = list.at(i).toElement();
        value = elt.attribute("uuid");
        if(value == uuid) {
            node = list.at(i);
            break;
        }
        value.clear();
    }
    if(value.isEmpty()) {
        QMessageBox mb;
        QString text = item->text(0);
        if(text.size() > 30) {text.truncate(30); text.append("...");}
        QString message = tr("The metadata element \"") + text + tr("\" cannot be found.");
        mb.critical(this, tr("Metadata Error"), message);
        return;
    }

    elt = node.toElement();
    list = elt.elementsByTagName("text");
    if(!list.isEmpty()) {
        for(int i = 0; i < list.size(); i++) node.removeChild(list.at(i));
    }

    QDomElement e = xmlFile.createElement("text");
    elt.appendChild(e);
    QString html = textEdit->toHtml();
    html = html.remove("&#xd;");
    html = html.simplified();
    QDomCDATASection cdata = xmlFile.createCDATASection(html);
    e.appendChild(cdata);

    refreshMetadata();
}

void Metadata::deleteItem()
{
    QList<QTreeWidgetItem*> items = dataTree->selectedItems();
    if(items.isEmpty()) return;

    QTreeWidgetItem* item = items.at(0);
    QTreeWidgetItem* parent = NULL;
    if(item->parent()) parent = item->parent();

    QString message;
    QTreeWidgetItem* child;
    QString text = item->text(0);
    if(text.size() > 30) {text.truncate(30); text.append("...");}
    if(parent) {
        message = "Are you sure that you want to delete the metadata element \"" + text + "\"?";
    } else {
        child = item->child(0);
        if(child) {
            message = "Are you sure that you want to delete the metadata category \"" + text + "\" and all of its elements?";
        } else {
            message = "Are you sure that you want to delete the metadata category \"" + text + "\"?";
        }
    }
    QMessageBox mb;
    mb.setText(message);
    mb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    mb.setDefaultButton(QMessageBox::Ok);
    int ret = mb.exec();

    if(ret == QMessageBox::Cancel) return;

    QDomNodeList list;
    if(parent) list = root.elementsByTagName("element");
    else list = root.elementsByTagName("category");

    QDomNode node;
    QDomElement elt;
    QString value;
    for(int i = 0; i < list.length(); i++) {
        elt = list.at(i).toElement();
        value = elt.attribute("uuid");
        if(value == item->text(UUID_COLUMN)) {
            node = list.at(i);
            break;
        }
        value.clear();
    }
    if(value.isEmpty()) {
        QMessageBox mb;
        QString message;
        QString text = item->text(0);
        if(text.size() > 30) {text.truncate(30); text.append("...");}
        if(parent) {
            message = tr("The metadata element \"") + text + tr("\" cannot be found.");
        } else {
            message = tr("The metadata category \"") + text + tr("\" cannot be found.");
        }
        mb.critical(this, tr("Metadata Error"), message);
        return;
    }

    QDomNode parentNode = node.parentNode();
    parentNode.removeChild(node);

    QDomNodeList cats = root.elementsByTagName("category");
    for(int i = 0; i < cats.length(); i++) {
        QDomElement elt = cats.at(i).toElement();
        QString uuid = elt.attribute("uuid");
        QList<QTreeWidgetItem*> items = dataTree->findItems(uuid, Qt::MatchExactly | Qt::MatchRecursive, UUID_COLUMN);
        if(items.isEmpty()) continue;
        QTreeWidgetItem* item = items.at(0);
        if(!item) continue;
        bool expanded = item->isExpanded();
        elt.setAttribute("folded", expanded ? "no" : "yes");
    }

    delete item;
    textEdit->clear();
    mw()->isSaved = false;

    items = dataTree->selectedItems();
    if(items.isEmpty()) deleteButton->setEnabled(false);
    else deleteButton->setEnabled(true);

    refreshMetadata();
}

void Metadata::loadText(QTreeWidgetItem* item, int i)
{
    if(!item->parent()) {
        textEdit->setReadOnly(true);
        textEdit->clear();
        editButton->setText("Edit");
        editButton->setEnabled(false);
        deleteButton->setEnabled(true);
        return;
    } else {
        textEdit->setReadOnly(true);
        editButton->setText("Edit");
        editButton->setEnabled(true);
        deleteButton->setEnabled(true);
    }

    QDomNodeList list = xmlFile.elementsByTagName("element");
    QDomNode node;
    QDomElement elt;
    QString value;
    for(int i = 0; i < list.length(); i++) {
        elt = list.at(i).toElement();
        value = elt.attribute("uuid");
        if(value == item->text(UUID_COLUMN)) {
            node = list.at(i);
            break;
        }
        value.clear();
    }
    if(value.isEmpty()) {
        QMessageBox mb;
        QString text = item->text(0);
        if(text.size() > 30) {text.truncate(30); text.append("...");}
        QString message = tr("The metadata element \"") + text + tr("\" cannot be found.");
        mb.critical(this, tr("Metadata Error"), message);
        return;
    }

    list = elt.elementsByTagName("text");
    if(list.isEmpty()) {textEdit->clear(); return;}

    QString text = list.at(0).toElement().text();
    text.replace("> <", "><"); // DT: This will remove first newline that gets inserted when loading text.
    textEdit->setHtml(text);
}

QString Metadata::formatFilesize(qint64 bytes)
{
    QString filesize;

    if(bytes > SIZE) { // kb
        bytes >>= 10;
        if(bytes > SIZE) { // mb
            bytes >>= 10;
            if(bytes > SIZE) { // gb
                bytes >>= 10;
                filesize = QString::number(bytes) + " GB";
            } else
                filesize = QString::number(bytes) + " MB";
        } else
            filesize = QString::number(bytes) + " KB";
    } else
        filesize = QString::number(bytes) + " B";

    return filesize;
}

void Metadata::get2DInfo()
{
    QString dimensions;
    int numCompo;
    int dims[3];
    int row;

    vtkSmartPointer<vtkImageData> image = viewer->GetInput();

    if (image)
    {
        image->GetDimensions(dims);
        numCompo = image->GetNumberOfScalarComponents();

        if(isDICOM) {
            dimensions = locale.toString(dims[0]) + " x " + locale.toString(dims[1]) + " x " + locale.toString(dims[2]);
        } else {
            dimensions = locale.toString(dims[0]) + " x " + locale.toString(dims[1]) + " x " + locale.toString(numCompo);
        }
    }

    filenameLabel = new QLabel(tr("Filename:"), this);
    filenameLine = new QLineEdit(QDir::toNativeSeparators(finfo->absoluteFilePath()), this);
    filenameLine->setMinimumWidth(380);
    filenameLine->setReadOnly(true);

    filesizeLabel = new QLabel(tr("File Size:"), this);
    if(finfo->isFile())
        filesizeLabel2 = new QLabel(formatFilesize(finfo->size()), this);
    else {
        QDirIterator it(finfo->absoluteFilePath());
        qint64 size = 0;
        while(it.hasNext()) {
            QFileInfo fi(it.next());
            size += fi.size();
        }
        filesizeLabel2 = new QLabel(formatFilesize(size), this);
    }

    dimensionsLabel = new QLabel(tr("Image Dimensions:"), this);
    dimensionsLabel2 = new QLabel(dimensions, this);

    row = grid->rowCount();
    grid->addWidget(filenameLabel, row, 0, 1, 1, Qt::AlignLeft);
    grid->addWidget(filenameLine, row, 1);

    row = grid->rowCount();
    grid->addWidget(filesizeLabel, row, 0, 1, 1, Qt::AlignLeft);
    grid->addWidget(filesizeLabel2, row, 1, 1, 1, Qt::AlignLeft);

    row = grid->rowCount();
    grid->addWidget(dimensionsLabel, row, 0, 1, 1, Qt::AlignLeft);
    grid->addWidget(dimensionsLabel2, row, 1, 1, 1, Qt::AlignLeft);
}

void Metadata::get3DInfo()
{
    int hdims[3] = {0,0,0};
    int ldims[3] = {0,0,0};
    QString texDimensions;
    int row;

    if (texture)
    {
      hyperdata->GetDimensions(hdims);
      hdims[2] = hyperdata->GetNumberOfScalarComponents();
      texture->GetInput()->GetDimensions(ldims);
      ldims[2] = texture->GetInput()->GetNumberOfScalarComponents();
    }
    if (hdims[0] != 0 || hdims[1] != 0 ) // hyperspectral data
    {
        texDimensions = locale.toString(hdims[0]) + " x " +
                        locale.toString(hdims[1]) + " x " +
                        locale.toString(hdims[2]);
    } else
    {
        texDimensions = locale.toString(ldims[0]) + " x " +
                        locale.toString(ldims[1]) + " x " +
                        locale.toString(ldims[2]);
    }

    filenameLabel = new QLabel(tr("Geometry Filename:"), this);
    filenameLine = new QLineEdit(QDir::toNativeSeparators(finfo->absoluteFilePath()), this);
    filenameLine->setMinimumWidth(360);
    filenameLine->setReadOnly(true);

    filesizeLabel = new QLabel(tr("Geometry File Size:"), this);
    if(finfo->isFile())
        filesizeLabel2 = new QLabel(formatFilesize(finfo->size()), this);
    else {
        QDirIterator it(finfo->absoluteFilePath());
        qint64 size = 0;
        while(it.hasNext()) {
            QFileInfo fi(it.next());
            size += fi.size();
        }
        filesizeLabel2 = new QLabel(formatFilesize(size), this);
    }

    if(!textureFilename.isEmpty()) {
        texFilenameLabel = new QLabel(tr("Texture Filename:"), this);
        texFilenameLine = new QLineEdit(QDir::toNativeSeparators(tfinfo->absoluteFilePath()), this);
        texFilenameLine->setMinimumWidth(360);
        texFilenameLine->setReadOnly(true);

        texFilesizeLabel = new QLabel(tr("Texture File Size:"), this);
        if(finfo->isFile())
            texFilesizeLabel2 = new QLabel(formatFilesize(tfinfo->size()), this);
        else {
            QDirIterator it(tfinfo->absoluteFilePath());
            qint64 size = 0;
            while(it.hasNext()) {
                QFileInfo fi(it.next());
                size += fi.size();
            }
            texFilesizeLabel2 = new QLabel(formatFilesize(size), this);
        }

        texDimensionsLabel = new QLabel(tr("Texture Dimensions:"), this);
        texDimensionsLabel2 = new QLabel(texDimensions, this);
    }

    if(points > 0) {
        pointLabel = new QLabel(tr("Number of Points:"), this);
        pointLabel2 = new QLabel(locale.toString(points), this);
    }

    if(polys > 0) {
        polyLabel = new QLabel(tr("Number of Polygons:"), this);
        polyLabel2 = new QLabel(locale.toString(polys), this);
    }

    row = grid->rowCount();
    grid->addWidget(filenameLabel, row, 0, 1, 1, Qt::AlignLeft);
    grid->addWidget(filenameLine, row, 1);

    row = grid->rowCount();
    grid->addWidget(filesizeLabel, row, 0, 1, 1, Qt::AlignLeft);
    grid->addWidget(filesizeLabel2, row, 1, 1, 1, Qt::AlignLeft);

    row = grid->rowCount();
    if(points > 0) {
        grid->addWidget(pointLabel, row, 0, 1, 1, Qt::AlignLeft);
        grid->addWidget(pointLabel2, row, 1, 1, 1, Qt::AlignLeft);
    }

    row = grid->rowCount();
    if(polys > 0) {
        grid->addWidget(polyLabel, row, 0, 1, 1, Qt::AlignLeft);
        grid->addWidget(polyLabel2, row, 1, 1, 1, Qt::AlignLeft);
    }

    row = grid->rowCount();
    if(!textureFilename.isEmpty()) {
        grid->addWidget(texFilenameLabel, row, 0, 1, 1, Qt::AlignLeft);
        grid->addWidget(texFilenameLine, row, 1);

        grid->addWidget(texFilesizeLabel, row+1, 0, 1, 1, Qt::AlignLeft);
        grid->addWidget(texFilesizeLabel2, row+1, 1);

        grid->addWidget(texDimensionsLabel, row+2, 0, 1, 1, Qt::AlignLeft);
        grid->addWidget(texDimensionsLabel2, row+2, 1, 1, 1, Qt::AlignLeft);
    }
}

QTreeWidgetItem *Metadata::createChildItem(QTreeWidgetItem *item)
{
    QTreeWidgetItem *childItem;
    if (item) {
        childItem = new QTreeWidgetItem(item);
    } else {
        childItem = new QTreeWidgetItem(dataTree);
    }
    childItem->setData(0, Qt::UserRole, xml.name().toString());
    return childItem;
}

void Metadata::readXML()
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "CHEROb.metadata");

    qDebug() << "starting read.";
    while (xml.readNextStartElement()) {
        qDebug() << xml.name();
        if (xml.name() == "category")
            readCategory(0);
        else if (xml.name() == "element")
            readElement(0);
        else
            xml.skipCurrentElement();
    }
}

void Metadata::readCategory(QTreeWidgetItem *item)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "category");

    QTreeWidgetItem *cat = createChildItem(item);
    QFileIconProvider provider;
    cat->setIcon(0, provider.icon(QFileIconProvider::File));
    cat->setFlags(cat->flags() ^ (Qt::ItemIsDropEnabled));
    QFont font = cat->font(0);
    font.setBold(true);
    cat->setFont(0, font);
    cat->setText(0, xml.attributes().value("title").toString());
    cat->setText(1, xml.attributes().value("date").toString());
    cat->setText(UUID_COLUMN, xml.attributes().value("uuid").toString());
    bool folded = (xml.attributes().value("folded") != "no");
    dataTree->setItemExpanded(cat, !folded);

    while (xml.readNextStartElement()) {
        qDebug() << "read category. next start:" << xml.name() << xml.attributes().value("title");
        if (xml.name() == "category")
            readCategory(cat);
        else if (xml.name() == "element")
            readElement(cat);
        else
            xml.skipCurrentElement();
    }
}

void Metadata::readElement(QTreeWidgetItem *item)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "element");

    QTreeWidgetItem *elt = createChildItem(item);
    elt->setFlags(elt->flags() ^ (Qt::ItemIsDropEnabled));
    QString text = xml.attributes().value("title").toString();
    qDebug() << "the text of this element is" << text;
    elt->setText(1, xml.attributes().value("date").toString());
    elt->setText(UUID_COLUMN, xml.attributes().value("uuid").toString());

    xml.readNext();
    qDebug() << xml.name();
    QTextDocument doc;
    //doc.setHtml(s);

    /*s = s.remove("&#xd;");
    s = s.replace("&lt;","<");
    s.remove(QRegExp("<[^>]*>"));
    s.remove(QRegExp("<[^>]*>"));*/

    QString simp = doc.toPlainText().simplified();
    qDebug() << "simp:" << simp;
    if(!simp.isEmpty()) text = text + ": " + simp;

    elt->setText(0, text);
}

void Metadata::refreshMetadata()
{
	
    dataTree->clear();
	QString path = mPath + QDir::separator() + "metadatatmp.xml";
    path = QDir::toNativeSeparators(path);
    QFile f(path);
    if(!f.open(QFile::WriteOnly | QFile::Text)) {
        return;
    }
	qDebug()<<"test!!";
    f.resize(0);
    QTextStream out(&f);
    out << xmlFile.toString();
    f.close();

    if(!f.exists()) return;
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    xml.setDevice(&f);
    if(xml.readNextStartElement()) {
        if(xml.name() == "CHEROb.metadata") {
            readXML();
        } else {
            xml.raiseError(tr("Invalid XML file."));
        }
    }
    f.close();
    f.remove();
}

Metadata::Metadata(QString path, VtkWidget *gla, bool file)
{
    //setModal(true);
    //setWindowModality(Qt::ApplicationModal);

    hyperdata = NULL;
    texture = NULL;
    userIsEditing = false;
	mPath = QDir::toNativeSeparators(path);

    vbox = new QVBoxLayout(this);

    if(gla && file) {
        isFile = true;
        w = gla;

        if(w->mHyperImageData)
            hyperdata = w->mHyperImageData;
        if (w->mRgbTexture)
            texture = w->mRgbTexture;

        grid = new QGridLayout();
        textureFilename = w->mRgbTextureFilename;
        finfo = new QFileInfo(w->mFilename);
        tfinfo = new QFileInfo(textureFilename);
        locale = QLocale::system();
        int mode = w->mWidgetMode;
        viewer = w->mVtkImageViewer;
        isDICOM = w->mIsDICOM;
        points = w->mNumberOfPoints;
        polys = w->mNumberOfPolys;

        if(w->format.hasOpenGL()) {
            QString openGLVersion = "OpenGL " + QString::number(w->format.majorVersion()) + "." + QString::number(w->format.minorVersion());
            openGLLabel = new QLabel(openGLVersion, this);
        } else {
            openGLLabel = new QLabel(tr("OpenGL not supported"), this);
        }

        QString cores = tr("Number of Cores: ") + QString::number(omp_get_num_procs());
        coresLabel = new QLabel(cores, this);

        int row = grid->rowCount();
        grid->addWidget(openGLLabel, row, 0, 1, 1, Qt::AlignLeft);
        grid->addWidget(coresLabel, row, 1, 1, 1, Qt::AlignRight);

        if(mode == IMAGE2D || mode == CTSTACK || mode == RTI2D) {
            get2DInfo();

        } else if(mode == MODEL3D || mode == CTVOLUME) {
            get3DInfo();

        } else {
            QMessageBox mb;
            mb.critical(this, tr("File Info Error"),
                        tr("Information about this file is not available because it is of an unknown file type."));
            return;
        }
		QString path = mPath;
        QStringList tokens = path.split(QDir::separator());
        QString CHEROb = tokens.takeLast();
        path = tokens.join(QDir::separator());
        path = QDir::toNativeSeparators(path);
        if(!QDir(path).exists()) {
            QDir().mkdir(path);
        }
        path.append(QDir::separator() + CHEROb);
        if(!QDir(path).exists()) {
            QDir().mkdir(path);
        }
        QDir* dir = new QDir();
        QString fn = QDir::toNativeSeparators(dir->relativeFilePath(finfo->absoluteFilePath()));

        xmlPath = path + QDir::separator() + "metadata.xml";

        QFile f(xmlPath);
        QDomNodeList list;
        if(!f.exists()) {
            QDomProcessingInstruction instruct = xmlFile.createProcessingInstruction("xml", "version=\"1.0\"");
            xmlFile.appendChild(instruct);

            root = xmlFile.createElement("CHEROb.metadata");
            root.setAttribute("filename", fn);
            xmlFile.appendChild(root);
        } else {
            xmlFile.setContent(&f);
            list = xmlFile.elementsByTagName("CHEROb.metadata");
            root = list.at(0).toElement();
        }

        grid->setColumnStretch(0,0);
        grid->setColumnStretch(1,5);
        grid->setVerticalSpacing(15);
        grid->setSizeConstraint(QLayout::SetFixedSize);
        vbox->addLayout(grid);
        vbox->setSpacing(20);

    }
    else {
        isFile = false;
        w = NULL;
        xmlFile = mw()->currentProjectMetadata;
    }

    if(w && isFile)
        groupBox = new QGroupBox("File Metadata", this);
    else
        groupBox = new QGroupBox("Project Metadata", this);

    dataTree = new DMetadataTreeWidget(this);
    dataTree->setContextMenuPolicy(Qt::CustomContextMenu);

    textEdit = new QTextEdit();
    textEdit->setReadOnly(true);

    vboxData = new QVBoxLayout();

    hboxButtons = new QHBoxLayout();

    addButton = new QPushButton(tr("Add..."));
    editButton = new QPushButton(tr("Edit"));
    deleteButton = new QPushButton(tr("Delete"));
    exportButton = new QPushButton(tr("Export..."));

    editButton->setEnabled(false);
    deleteButton->setEnabled(false);

    addButton->setDefault(false);
    addButton->setAutoDefault(false);
    editButton->setDefault(false);
    editButton->setAutoDefault(false);
    deleteButton->setDefault(false);
    deleteButton->setAutoDefault(false);
    exportButton->setDefault(false);
    exportButton->setAutoDefault(false);

    hboxButtons->addWidget(addButton);
    hboxButtons->addWidget(editButton);
    hboxButtons->addWidget(deleteButton);
    hboxButtons->addWidget(exportButton);

    connect(addButton, SIGNAL(clicked()), this, SLOT(addCategory()));
    connect(editButton, SIGNAL(clicked()), this, SLOT(toggleEdit()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteItem()));
    connect(dataTree, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showContextMenu(const QPoint&)));
    connect(dataTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(loadText(QTreeWidgetItem*,int)));

    vboxData->addWidget(dataTree);
    vboxData->addWidget(textEdit);
    vboxData->addLayout(hboxButtons);

    groupBox->setLayout(vboxData);
    groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    groupBox->setMinimumHeight(200);

    hbox = new QHBoxLayout();
    hbox->addWidget(groupBox);
    hbox->setSizeConstraint(QLayout::SetMinimumSize);

    vbox->addLayout(hbox);
    this->setLayout(vbox);

    if(w && isFile) {
        QString title = tr("File Info: ") + finfo->fileName();
        this->setWindowTitle(title);
    }
    else {
        QString title = tr("Project Info: ") + mw()->currentProjectName;
        this->setWindowTitle(title);
    }
	

    buildMetadataHash();

    refreshMetadata();
qDebug()<<"test";
    this->setMinimumWidth(550);
    this->adjustSize();
    this->show();
}

Metadata::~Metadata()
{
    if(isFile && w) {
        QFile f(xmlPath);
        if(!f.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox mb;
            mb.critical(this, tr("Metadata Error"), tr("Failed to save changes."));
            f.close();
            return;
        }
        f.resize(0);
        QTextStream out(&f);
        out << xmlFile.toString();
        f.close();
    }
}

void Metadata::updateButtons()
{
    addButton->setEnabled(true);
    exportButton->setEnabled(true);

}

void Metadata::keyPressEvent(QKeyEvent* event)
{
    QList<QTreeWidgetItem *> list = dataTree->selectedItems();
    QTreeWidgetItem* item = list.at(0);
    if(!item) return;

    if(event->key() == Qt::Key_Delete) deleteItem();
    else if(event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {qDebug() << "loading..."; loadText(item, 0);}
    QDialog::keyPressEvent(event);
}
