/****************************************************************************
** Meta object code from reading C++ file 'informationWidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/information/informationWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'informationWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DTextEdit[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_DTextEdit[] = {
    "DTextEdit\0"
};

void DTextEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData DTextEdit::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DTextEdit::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DTextEdit,
      qt_meta_data_DTextEdit, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DTextEdit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DTextEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DTextEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DTextEdit))
        return static_cast<void*>(const_cast< DTextEdit*>(this));
    return QWidget::qt_metacast(_clname);
}

int DTextEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Information[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,
      23,   12,   12,   12, 0x05,
      50,   34,   12,   12, 0x05,
      83,   12,   12,   12, 0x05,
     110,   96,   12,   12, 0x05,
     165,  155,   12,   12, 0x25,
     218,  201,   12,   12, 0x05,
     283,  270,   12,   12, 0x25,

 // slots: signature, parameters, type, tag, flags
     338,  326,   12,   12, 0x0a,
     368,  326,   12,   12, 0x0a,
     400,  326,   12,   12, 0x0a,
     432,  326,   12,   12, 0x0a,
     464,  326,   12,   12, 0x0a,
     498,  326,   12,   12, 0x0a,
     532,   12,   12,   12, 0x08,
     552,   12,   12,   12, 0x08,
     569,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Information[] = {
    "Information\0\0saveAll()\0closeAll()\0"
    "newName,oldName\0replaceUserName(QString,QString)\0"
    "updateMenu()\0path,type,dim\0"
    "addNavigationItem(QString,NoteMode,NoteType)\0"
    "path,type\0addNavigationItem(QString,NoteMode)\0"
    "path,type,id,dim\0"
    "removeNavigationItem(QString,NoteMode,int,NoteType)\0"
    "path,type,id\0removeNavigationItem(QString,NoteMode,int)\0"
    "noteId,path\0removePointNote(int,QString*)\0"
    "removeSurfaceNote(int,QString*)\0"
    "removeFrustumNote(int,QString*)\0"
    "removePointNote2D(int,QString*)\0"
    "removeSurfaceNote2D(int,QString*)\0"
    "removePolygonNote2D(int,QString*)\0"
    "annotationChanges()\0saveAnnotation()\0"
    "removeAnnotation()\0"
};

void Information::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Information *_t = static_cast<Information *>(_o);
        switch (_id) {
        case 0: _t->saveAll(); break;
        case 1: _t->closeAll(); break;
        case 2: _t->replaceUserName((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->updateMenu(); break;
        case 4: _t->addNavigationItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const NoteMode(*)>(_a[2])),(*reinterpret_cast< const NoteType(*)>(_a[3]))); break;
        case 5: _t->addNavigationItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const NoteMode(*)>(_a[2]))); break;
        case 6: _t->removeNavigationItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const NoteMode(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])),(*reinterpret_cast< const NoteType(*)>(_a[4]))); break;
        case 7: _t->removeNavigationItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const NoteMode(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3]))); break;
        case 8: _t->removePointNote((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString*(*)>(_a[2]))); break;
        case 9: _t->removeSurfaceNote((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString*(*)>(_a[2]))); break;
        case 10: _t->removeFrustumNote((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString*(*)>(_a[2]))); break;
        case 11: _t->removePointNote2D((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString*(*)>(_a[2]))); break;
        case 12: _t->removeSurfaceNote2D((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString*(*)>(_a[2]))); break;
        case 13: _t->removePolygonNote2D((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString*(*)>(_a[2]))); break;
        case 14: _t->annotationChanges(); break;
        case 15: _t->saveAnnotation(); break;
        case 16: _t->removeAnnotation(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Information::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Information::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Information,
      qt_meta_data_Information, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Information::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Information::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Information::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Information))
        return static_cast<void*>(const_cast< Information*>(this));
    return QWidget::qt_metacast(_clname);
}

int Information::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void Information::saveAll()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Information::closeAll()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Information::replaceUserName(const QString _t1, const QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Information::updateMenu()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void Information::addNavigationItem(const QString _t1, const NoteMode _t2, const NoteType _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 6
void Information::removeNavigationItem(const QString _t1, const NoteMode _t2, const int _t3, const NoteType _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
