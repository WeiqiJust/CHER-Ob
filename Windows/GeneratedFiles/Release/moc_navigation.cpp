/****************************************************************************
** Meta object code from reading C++ file 'navigation.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/function/navigation.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'navigation.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Navigation[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      24,   12,   11,   11, 0x08,
      79,   65,   11,   11, 0x08,
     128,  118,   11,   11, 0x28,
     175,  158,   11,   11, 0x08,
     234,  221,   11,   11, 0x28,
     294,  271,   11,   11, 0x08,
     335,  271,   11,   11, 0x08,
     405,  379,   11,   11, 0x08,
     451,  447,   11,   11, 0x08,
     482,  475,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Navigation[] = {
    "Navigation\0\0item,column\0"
    "showTreeWidgetItem(QTreeWidgetItem*,int)\0"
    "path,type,dim\0addNoteItem(QString,NoteMode,NoteType)\0"
    "path,type\0addNoteItem(QString,NoteMode)\0"
    "path,type,id,dim\0"
    "removeNoteItem(QString,NoteMode,int,NoteType)\0"
    "path,type,id\0removeNoteItem(QString,NoteMode,int)\0"
    "path,bookmarkName,uuid\0"
    "addBookmarkItem(QString,QString,QString)\0"
    "removeBookmarkItem(QString,QString,QString)\0"
    "path,newBookmarkName,uuid\0"
    "editBookmarkItem(QString,QString,QString)\0"
    "pos\0showContextMenu(QPoint)\0action\0"
    "menuSelection(QAction*)\0"
};

void Navigation::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Navigation *_t = static_cast<Navigation *>(_o);
        switch (_id) {
        case 0: _t->showTreeWidgetItem((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->addNoteItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const NoteMode(*)>(_a[2])),(*reinterpret_cast< const NoteType(*)>(_a[3]))); break;
        case 2: _t->addNoteItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const NoteMode(*)>(_a[2]))); break;
        case 3: _t->removeNoteItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const NoteMode(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])),(*reinterpret_cast< const NoteType(*)>(_a[4]))); break;
        case 4: _t->removeNoteItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const NoteMode(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3]))); break;
        case 5: _t->addBookmarkItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 6: _t->removeBookmarkItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 7: _t->editBookmarkItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 8: _t->showContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 9: _t->menuSelection((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Navigation::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Navigation::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Navigation,
      qt_meta_data_Navigation, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Navigation::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Navigation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Navigation::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Navigation))
        return static_cast<void*>(const_cast< Navigation*>(this));
    return QWidget::qt_metacast(_clname);
}

int Navigation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
