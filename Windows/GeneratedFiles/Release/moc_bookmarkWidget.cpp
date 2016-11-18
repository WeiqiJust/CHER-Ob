/****************************************************************************
** Meta object code from reading C++ file 'bookmarkWidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/information/bookmarkWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bookmarkWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BookmarkWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      39,   16,   15,   15, 0x05,
      82,   16,   15,   15, 0x05,
     154,  128,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
     198,   15,   15,   15, 0x0a,
     236,  220,  215,   15, 0x0a,
     280,  275,  215,   15, 0x2a,
     311,   15,   15,   15, 0x08,
     326,  275,   15,   15, 0x08,
     355,   15,   15,   15, 0x28,
     381,  368,   15,   15, 0x08,
     437,  429,   15,   15, 0x28,
     468,  275,   15,   15, 0x08,
     495,   15,   15,   15, 0x28,
     512,  506,   15,   15, 0x08,
     552,  275,   15,   15, 0x08,
     590,  586,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_BookmarkWidget[] = {
    "BookmarkWidget\0\0path,bookmarkName,uuid\0"
    "addNavigationItem(QString,QString,QString)\0"
    "removeNavigationItem(QString,QString,QString)\0"
    "path,newbookmarkName,uuid\0"
    "editNavigationItem(QString,QString,QString)\0"
    "createBookmark()\0bool\0item,objectPath\0"
    "viewBookmark(QTreeWidgetItem*,QString)\0"
    "item\0viewBookmark(QTreeWidgetItem*)\0"
    "createFolder()\0deleteItem(QTreeWidgetItem*)\0"
    "deleteItem()\0xmlPath,item\0"
    "deleteItemPermanently(QString,QTreeWidgetItem*)\0"
    "xmlPath\0deleteItemPermanently(QString)\0"
    "editItem(QTreeWidgetItem*)\0editItem()\0"
    "item,\0handleDoubleClick(QTreeWidgetItem*,int)\0"
    "saveFolderState(QTreeWidgetItem*)\0pos\0"
    "showContextMenu(QPoint)\0"
};

void BookmarkWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        BookmarkWidget *_t = static_cast<BookmarkWidget *>(_o);
        switch (_id) {
        case 0: _t->addNavigationItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 1: _t->removeNavigationItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 2: _t->editNavigationItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 3: _t->createBookmark(); break;
        case 4: { bool _r = _t->viewBookmark((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 5: { bool _r = _t->viewBookmark((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: _t->createFolder(); break;
        case 7: _t->deleteItem((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 8: _t->deleteItem(); break;
        case 9: _t->deleteItemPermanently((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 10: _t->deleteItemPermanently((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->editItem((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 12: _t->editItem(); break;
        case 13: _t->handleDoubleClick((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 14: _t->saveFolderState((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 15: _t->showContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData BookmarkWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject BookmarkWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_BookmarkWidget,
      qt_meta_data_BookmarkWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BookmarkWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BookmarkWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BookmarkWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BookmarkWidget))
        return static_cast<void*>(const_cast< BookmarkWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int BookmarkWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void BookmarkWidget::addNavigationItem(const QString _t1, const QString _t2, const QString _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void BookmarkWidget::removeNavigationItem(const QString _t1, const QString _t2, const QString _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void BookmarkWidget::editNavigationItem(const QString _t1, const QString _t2, const QString _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
