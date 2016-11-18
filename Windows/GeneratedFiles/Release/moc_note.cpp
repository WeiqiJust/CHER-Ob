/****************************************************************************
** Meta object code from reading C++ file 'note.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/information/note.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'note.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Note[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,    6,    5,    5, 0x05,

 // slots: signature, parameters, type, tag, flags
      45,    5,    5,    5, 0x0a,
      52,    5,    5,    5, 0x0a,
      63,   60,    5,    5, 0x0a,
      96,   80,    5,    5, 0x0a,
     137,  129,    5,    5, 0x2a,
     171,  162,    5,    5, 0x09,
     189,    5,    5,    5, 0x29,
     200,    5,    5,    5, 0x08,
     209,    5,    5,    5, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Note[] = {
    "Note\0\0mNoteId,mPath\0removeNote(int,QString*)\0"
    "save()\0close()\0id\0removeImage(int)\0"
    "newName,oldName\0replaceUserName(QString,QString)\0"
    "newName\0replaceUserName(QString)\0"
    "fileName\0addImage(QString)\0addImage()\0"
    "remove()\0textChanged()\0"
};

void Note::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Note *_t = static_cast<Note *>(_o);
        switch (_id) {
        case 0: _t->removeNote((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString*(*)>(_a[2]))); break;
        case 1: _t->save(); break;
        case 2: _t->close(); break;
        case 3: _t->removeImage((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->replaceUserName((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 5: _t->replaceUserName((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->addImage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->addImage(); break;
        case 8: _t->remove(); break;
        case 9: _t->textChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Note::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Note::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Note,
      qt_meta_data_Note, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Note::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Note::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Note::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Note))
        return static_cast<void*>(const_cast< Note*>(this));
    return QWidget::qt_metacast(_clname);
}

int Note::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void Note::removeNote(int _t1, QString * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_PointNote[] = {

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

static const char qt_meta_stringdata_PointNote[] = {
    "PointNote\0"
};

void PointNote::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData PointNote::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PointNote::staticMetaObject = {
    { &Note::staticMetaObject, qt_meta_stringdata_PointNote,
      qt_meta_data_PointNote, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PointNote::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PointNote::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PointNote::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PointNote))
        return static_cast<void*>(const_cast< PointNote*>(this));
    return Note::qt_metacast(_clname);
}

int PointNote::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Note::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_SurfaceNote[] = {

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

static const char qt_meta_stringdata_SurfaceNote[] = {
    "SurfaceNote\0"
};

void SurfaceNote::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SurfaceNote::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SurfaceNote::staticMetaObject = {
    { &Note::staticMetaObject, qt_meta_stringdata_SurfaceNote,
      qt_meta_data_SurfaceNote, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SurfaceNote::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SurfaceNote::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SurfaceNote::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SurfaceNote))
        return static_cast<void*>(const_cast< SurfaceNote*>(this));
    return Note::qt_metacast(_clname);
}

int SurfaceNote::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Note::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_FrustumNote[] = {

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

static const char qt_meta_stringdata_FrustumNote[] = {
    "FrustumNote\0"
};

void FrustumNote::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FrustumNote::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FrustumNote::staticMetaObject = {
    { &Note::staticMetaObject, qt_meta_stringdata_FrustumNote,
      qt_meta_data_FrustumNote, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FrustumNote::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FrustumNote::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FrustumNote::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FrustumNote))
        return static_cast<void*>(const_cast< FrustumNote*>(this));
    return Note::qt_metacast(_clname);
}

int FrustumNote::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Note::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_PointNote2D[] = {

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

static const char qt_meta_stringdata_PointNote2D[] = {
    "PointNote2D\0"
};

void PointNote2D::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData PointNote2D::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PointNote2D::staticMetaObject = {
    { &Note::staticMetaObject, qt_meta_stringdata_PointNote2D,
      qt_meta_data_PointNote2D, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PointNote2D::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PointNote2D::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PointNote2D::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PointNote2D))
        return static_cast<void*>(const_cast< PointNote2D*>(this));
    return Note::qt_metacast(_clname);
}

int PointNote2D::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Note::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_SurfaceNote2D[] = {

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

static const char qt_meta_stringdata_SurfaceNote2D[] = {
    "SurfaceNote2D\0"
};

void SurfaceNote2D::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SurfaceNote2D::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SurfaceNote2D::staticMetaObject = {
    { &Note::staticMetaObject, qt_meta_stringdata_SurfaceNote2D,
      qt_meta_data_SurfaceNote2D, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SurfaceNote2D::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SurfaceNote2D::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SurfaceNote2D::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SurfaceNote2D))
        return static_cast<void*>(const_cast< SurfaceNote2D*>(this));
    return Note::qt_metacast(_clname);
}

int SurfaceNote2D::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Note::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_PolygonNote2D[] = {

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

static const char qt_meta_stringdata_PolygonNote2D[] = {
    "PolygonNote2D\0"
};

void PolygonNote2D::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData PolygonNote2D::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PolygonNote2D::staticMetaObject = {
    { &Note::staticMetaObject, qt_meta_stringdata_PolygonNote2D,
      qt_meta_data_PolygonNote2D, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PolygonNote2D::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PolygonNote2D::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PolygonNote2D::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PolygonNote2D))
        return static_cast<void*>(const_cast< PolygonNote2D*>(this));
    return Note::qt_metacast(_clname);
}

int PolygonNote2D::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Note::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
